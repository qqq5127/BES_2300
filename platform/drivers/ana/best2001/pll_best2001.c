/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "psramuhsphy.h"
#include "usbphy.h"
#include "hal_location.h"

#include CHIP_SPECIFIC_HDR(reg_usbphy)
#include CHIP_SPECIFIC_HDR(reg_ddrpll)
#include CHIP_SPECIFIC_HDR(reg_dsppll)
#include CHIP_SPECIFIC_HDR(reg_bbpll)

#define PLL_MHZ 1000000

#ifdef CHIP_HAS_SPIPHY

void usb_pll_enable(int enable)
{
    int ret;
    uint16_t val;

    ret = hal_phyif_open(SPIPHY_USB_CS);
    if (ret) {
        return;
    }

    usbphy_read(USBPHY_REG_18, &val);
    if (enable) {
        val |= DIG_USBPLL_USB_EN;
    } else {
        val &= ~DIG_USBPLL_USB_EN;
    }
    usbphy_write(USBPHY_REG_18, val);

    //bit0 for CODEC, bit1 for USB?
    usbphy_read(USBPHY_REG_1D, &val);
    val = SET_BITFIELD(val, DIG_USBPLL_FDIV_SEL, 0x3);
    usbphy_write(USBPHY_REG_1D, val);
}

void usb_pll_config(uint64_t freq)
{
    int ret;
    uint64_t PLL_cfg_val;
    uint16_t v[3];
    uint16_t val;
    uint16_t freq_en;
    uint32_t crystal = hal_cmu_get_crystal_freq();

    ret = hal_phyif_open(SPIPHY_USB_CS);
    if (ret) {
        return;
    }

    PLL_cfg_val = ((uint64_t)(1 << 28) * freq + crystal / 2) / crystal;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;
    v[2] = (PLL_cfg_val >> 32) & 0xFFFF;

    usbphy_read(USBPHY_REG_36, &freq_en);
    freq_en &= ~PLL_FREQ_EN;
    usbphy_write(USBPHY_REG_36, freq_en);

    usbphy_write(USBPHY_REG_39, v[0]);
    usbphy_write(USBPHY_REG_3A, v[1]);
    usbphy_read(USBPHY_REG_37, &val);
    val = SET_BITFIELD(val, PLL_FREQ_34_32, v[2]);
    usbphy_write(USBPHY_REG_37, val);

    freq_en |= PLL_FREQ_EN;
    usbphy_write(USBPHY_REG_36, freq_en);
}

void ddr_pll_config()
{
    uint64_t freq_mhz;
    uint8_t ddr_div;
    int32_t ret;
    uint64_t PLL_cfg_val;
    uint16_t v[2];
    uint16_t val;
    uint32_t crystal_mhz = hal_cmu_get_crystal_freq() / PLL_MHZ;
#if (PSRAMUHS_SPEED == 350)
    freq_mhz = (uint64_t)2100;
    ddr_div = 3;
#elif (PSRAMUHS_SPEED == 400)
    freq_mhz = (uint64_t)2400;
    ddr_div = 3;
#elif (PSRAMUHS_SPEED == 600)
    freq_mhz = (uint64_t)2400;
    ddr_div = 2;
#elif (PSRAMUHS_SPEED == 800)
    freq_mhz = (uint64_t)1600;
    ddr_div = 0;
#elif (PSRAMUHS_SPEED == 900)
    freq_mhz = (uint64_t)1800;
    ddr_div = 0;
#elif (PSRAMUHS_SPEED == 1000)
    freq_mhz = (uint64_t)2000;
    ddr_div = 0;
#elif (PSRAMUHS_SPEED == 1066)
    freq_mhz = (uint64_t)2132;
    ddr_div = 0;
#else
#error "invalid PSRAMUHS_SPEED"
#endif

    ret = hal_phyif_open(SPIPHY_PSRAMUHS0_CS);
    if (ret) {
        return;
    }

    if (crystal_mhz == 26) {
        psramuhsphy0_read(DDRPLL_REG_35, &val);
        val = SET_BITFIELD(val, REG_DDRPLL_REFCLK_SEL, 0x3);
        psramuhsphy0_write(DDRPLL_REG_35, val);
    } else if (crystal_mhz == 40) {
        psramuhsphy0_read(DDRPLL_REG_35, &val);
        val = SET_BITFIELD(val, REG_DDRPLL_REFCLK_SEL, 0x1);
        psramuhsphy0_write(DDRPLL_REG_35, val);
    } else {
        ASSERT(1, "ERROR unsupport crystal freq");
    }

    PLL_cfg_val = ((uint64_t)(1 << 25) * freq_mhz / 2 + crystal_mhz / 2) / crystal_mhz;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;

    psramuhsphy0_write(DDRPLL_REG_39, v[0]);
    psramuhsphy0_write(DDRPLL_REG_3A, v[1]);

    psramuhsphy0_read(DDRPLL_REG_31, &val);
    val = SET_BITFIELD(val, REG_DDRPLL_PDIV_PSRAM, ddr_div);
    val = SET_BITFIELD(val, REG_DDRPLL_PDIV_PSRAM2, ddr_div);
    val = SET_BITFIELD(val, REG_DDRPLL_PATH_EN, 0x3);
    psramuhsphy0_write(DDRPLL_REG_31, val);

    psramuhsphy0_read(DDRPLL_REG_51, &val);
    val |= 0x3<<6;
    psramuhsphy0_write(DDRPLL_REG_51, val);
    psramuhsphy0_read(DDRPLL_REG_50, &val);
    val |= REG_BIAS_PRECHG;
    psramuhsphy0_write(DDRPLL_REG_50, val);
    val &= ~REG_BIAS_PRECHG;
    psramuhsphy0_write(DDRPLL_REG_50, val);
}

void dsp_pll_config(uint64_t freq)
{
    int32_t ret;
    uint8_t ddr_div;
    uint32_t freq_mhz;
    uint64_t PLL_cfg_val;
    uint16_t v[2];
    uint16_t val;
    uint32_t crystal_mhz = hal_cmu_get_crystal_freq() / PLL_MHZ;
    uint8_t valid_div[14] = {2, 3, 4, 6, 8, 10, 12, 14, 6, 9, 12, 15, 18, 21};
    uint8_t i;

    freq_mhz = freq / PLL_MHZ;

    ret = hal_phyif_open(SPIPHY_PSRAMUHS0_CS);
    if (ret) {
        return;
    }
    ret = hal_phyif_open(SPIPHY_PSRAMUHS1_CS);
    if (ret) {
        return;
    }

    if (crystal_mhz == 26) {
        psramuhsphy1_read(DSPPLL_REG_35, &val);
        val = SET_BITFIELD(val, REG_A7PLL_REFCLK_SEL, 0x3);
        psramuhsphy1_write(DSPPLL_REG_35, val);
    } else if (crystal_mhz == 40) {
        psramuhsphy1_read(DSPPLL_REG_35, &val);
        val = SET_BITFIELD(val, REG_A7PLL_REFCLK_SEL, 0x1);
        psramuhsphy1_write(DSPPLL_REG_35, val);
    } else {
        ASSERT(1, "ERROR unsupport crystal freq");
    }

    for (i=0; i<ARRAY_SIZE(valid_div); ++i) {
        if (freq_mhz*valid_div[i]>=(uint64_t)1800 && freq_mhz*valid_div[i]<=(uint64_t)2400) {
            freq_mhz = freq_mhz*valid_div[i];
            ddr_div = valid_div[i];
            break;
        }
    }
    ASSERT(i < ARRAY_SIZE(valid_div), "Invalid freq input:%dMHz", freq_mhz);

    PLL_cfg_val = ((uint64_t)(1 << 25) * freq_mhz / 2 + crystal_mhz / 2) / crystal_mhz;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;

    psramuhsphy1_write(DSPPLL_REG_39, v[0]);
    psramuhsphy1_write(DSPPLL_REG_3A, v[1]);

    //div =  div[0] ? (2+div[4]) : (2+div[4])*(div[3:1])
    if (ddr_div == 2) {
        ddr_div = 0x1;
    } else if (ddr_div == 3) {
        ddr_div = 0x1;
        ddr_div |= (1<<4);
    } else if (((ddr_div%2)==0) && (ddr_div>=2 && ddr_div<=14)) {
        ddr_div /= 2;
        ddr_div = ddr_div<<1;
    } else if (((ddr_div%3)==0) && (ddr_div>=3 && ddr_div<=21)) {
        ddr_div /= 3;
        ddr_div = ddr_div<<1;
        ddr_div |= (1<<4);
    } else {
        ASSERT(0, "invalid ddr_div:%d", ddr_div);
    }
    psramuhsphy1_read(DSPPLL_REG_32, &val);
    val = SET_BITFIELD(val, REG_A7PLL_POSTDIV, ddr_div);
    psramuhsphy1_write(DSPPLL_REG_32, val);

    //write psram0
    psramuhsphy0_read(DDRPLL_REG_51, &val);
    val |= 0x3<<8;
    psramuhsphy0_write(DDRPLL_REG_51, val);
    psramuhsphy0_read(DDRPLL_REG_50, &val);
    val |= REG_BIAS_PRECHG;
    psramuhsphy0_write(DDRPLL_REG_50, val);
    val &= ~REG_BIAS_PRECHG;
    psramuhsphy0_write(DDRPLL_REG_50, val);
}

#endif

void bb_pll_config(uint64_t freq)
{
    uint32_t freq_mhz;
    uint64_t PLL_cfg_val;
    uint16_t v[2];
    uint32_t crystal_mhz = hal_cmu_get_crystal_freq() / PLL_MHZ;

    freq_mhz = freq / PLL_MHZ;

    PLL_cfg_val = ((uint64_t)(1 << 25) * freq_mhz / 2 + crystal_mhz / 2) / crystal_mhz;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;

    wfrf_write(BBPLL_REG_BD, v[0]);
    wfrf_write(BBPLL_REG_BE, v[1]);
}

