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
#include "cmsis_nvic.h"
#include "hal_analogif.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "tgt_hardware.h"
#include "hal_location.h"
#include "analog.h"
#include "usbphy.h"
#include "reg_usbphy_best3003.h"

#ifdef CHIP_HAS_USBPHY

#ifdef CHIP_HAS_DCO
void usbphy_dco_960m_sel(uint8_t sel)
{
	uint16_t val;

	sel = !!sel;

	usbphy_read(0x18, &val);
	if (sel) {
        // to use the DCO 960MHz
		val |= USB_CLK960M_SEL;
	} else {
	    // to disable DCO 960MHz
		val &= ~USB_CLK960M_SEL;
	}
	usbphy_write(0x18, val);
}
#endif

void usbphy_open(void)
{
    unsigned short val_01, val;

#ifdef USB_HIGH_SPEED

    usbphy_read(0x01, &val_01);
    val_01 &= ~(CFG_ANAPHY_RESETN | CFG_POL_CDRCLK | CFG_POL_CLKPLL);
    usbphy_write(0x01, val_01);

    val_01 |= CFG_CKCDR_EN | CFG_CKPLL_EN | CFG_EN_CLKMAC | CFG_MODE_HS_LINES_TX;
    usbphy_write(0x01, val_01);

    usbphy_read(0x02, &val);
    // ??? here
//    val = (val & ~(0x1F << 10)) | (0xC << 10);
    val &= ~(CFG_RXRESET | CFG_EBUF_THRD_MASK);
    val |= CFG_EBUF_THRD(2);
    usbphy_write(0x02, val);

    usbphy_read(0x05, &val);
    val |= CFG_UTMISRP_BVALID;
    usbphy_write(0x05, val);

    // Tx off during rx
    usbphy_read(0x06, &val);
    val |= CFG_HS_EN_IOFF_REG | CFG_EN_HS_S2P_REG;
    // Set hs_ibias_trim to 7 (default is 4)
    //val = (val & ~CFG_HS_IBIAS_TRIM_REG_MASK) | CFG_HS_IBIAS_TRIM_REG(7);
    usbphy_write(0x06, val);

    usbphy_read(0x07, &val);
    val &= ~CFG_FS_CDR_BYPASS_REG;
    usbphy_write(0x07, val);

    usbphy_read(0x08, &val);
    val = (val & ~(CFG_RES_GN_RCV_REG_MASK | CFG_I_RCV_REG_MASK | CFG_PI_GN_REG_MASK)) | CFG_RES_GN_RCV_REG(0) | CFG_I_RCV_REG(3) | CFG_PI_GN_REG(1);
    usbphy_write(0x08, val);

    usbphy_read(0x09, &val);
    val &= ~CFG_EN_ZPS_REG;
    val |= CFG_SEL_TERR_REG;
    usbphy_write(0x09, val);

    // Disable force clocks, and disable hs son signal
    // Enable parallel dig-to-phy interface
    usbphy_read(0x22, &val);
    val &= ~(CFG_FORCE_TXCK | CFG_FORCE_RXCK | CFG_FORCE_CDRCK | CFG_MODE_S_ON);
    val |= CFG_CKTXP_EN | CFG_CKRXP_EN;
    usbphy_write(0x22, val);

    // Ignore all UTMI errors
    usbphy_write(0x12, 0x0003);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    val_01 |= CFG_ANAPHY_RESETN;
    usbphy_write(0x01, val_01);

#if defined(CHIP_HAS_DCO)
	// to use the DCXO-960M
    usbphy_read(0x18, &val);
    val |= USB_CLK960M_SEL;
    usbphy_write(0x18, val);
#endif

	// RX FIFO is set to 32 bits
	usbphy_read(0x13, &val);
	val &= ~CFG_PEBUF_THRD_MASK;
	val |= CFG_PEBUF_THRD(2);
	usbphy_write(0x13, val);

#ifdef USB_HS_LOOPBACK_TEST
    usbphy_read(0x0A, &val);
    val |= CFG_DR_TERMSEL | CFG_DR_XCVRSEL | CFG_DR_OPMODE;
    usbphy_write(0x0A, val);

    usbphy_read(0x0B, &val);
    val &= ~(CFG_REG_TERM | CFG_REG_XCVRSEL | CFG_REG_OPMODE_MASK);
    usbphy_write(0x0B, val);

    usbphy_read(0x09, &val);
    val |= CFG_LOOP_BACK_REG;
    usbphy_write(0x09, val);

    usbphy_read(0x06, &val);
    val |= CFG_HS_SRC_SEL_REG;
    usbphy_write(0x06, val);

    hal_sys_timer_delay(MS_TO_TICKS(1));

#define LOOPBACK_PATTERN                (*(volatile uint8_t *)0x20000004)

    LOOPBACK_PATTERN = 0;
    uint8_t pattern = LOOPBACK_PATTERN;

    // Set tx pattern
    usbphy_read(0x03, &val);
    val = (val & ~CFG_TXPATTERN_MASK) | CFG_TXPATTERN(pattern);
    usbphy_write(0x03, val);

    // Enable force clocks
    usbphy_read(0x22, &val);
    val |= CFG_FORCE_TXCK | CFG_FORCE_RXCK | CFG_FORCE_CDRCK;
    usbphy_write(0x22, val);

    // Enable tx
    usbphy_read(0x04, &val);
    val |= CFG_FORCETX;
    usbphy_write(0x04, val);

    // Update tx pattern
    while (1) {
        if (pattern != LOOPBACK_PATTERN) {
            pattern = LOOPBACK_PATTERN;
            usbphy_read(0x03, &val);
            val = (val & ~CFG_TXPATTERN_MASK) | CFG_TXPATTERN(pattern);
            usbphy_write(0x03, val);
        }
    }
#endif

#else // !USB_HIGH_SPEED

    usbphy_read(0x01, &val_01);
    val_01 &= ~CFG_ANAPHY_RESETN;
    usbphy_write(0x01, val_01);
    val_01 |= CFG_EN_CLKMAC;
    usbphy_write(0x01, val_01);

    val = CFG_EN_HS_S2P_REG;
    usbphy_write(0x06, val);

    usbphy_read(0x07, &val);
    val &= ~CFG_FS_CDR_BYPASS_REG;
    usbphy_write(0x07, val);

    val = 0;
    usbphy_write(0x08, val);

    val = 0;
    usbphy_write(0x09, val);

    val_01 |= CFG_ANAPHY_RESETN;
    usbphy_write(0x01, val_01);

#endif // USB_HIGH_SPEED

}

void usbphy_close (void)
{

}

void usbphy_sleep(void)
{
    uint16_t val;

    usbphy_read(0x01, &val);
#ifdef USB_HIGH_SPEED
    val &= ~(CFG_CKCDR_EN | CFG_CKPLL_EN | CFG_EN_CLKMAC);
#else
    val &= ~CFG_EN_CLKMAC;
#endif
    usbphy_write(0x01, val);
}

void usbphy_wakeup(void)
{
    uint16_t val;

    usbphy_read(0x01, &val);
#ifdef USB_HIGH_SPEED
    val |= CFG_CKCDR_EN | CFG_CKPLL_EN | CFG_EN_CLKMAC;
#else
    val |= CFG_EN_CLKMAC;
#endif
    usbphy_write(0x01, val);
}

void usbphy_test_set(void)
{
	uint16_t val;

	usbphy_read(0x07, &val);
	val |= CFG_MODE_LINESTATE;
	usbphy_write(0x07, val);
}

void usbphy_loopback_test (void)
{
	usbphy_write(0x01, 0x70ef);
	usbphy_write(0x02, 0x3018);
	usbphy_write(0x06, 0x28ff);
	usbphy_write(0x08, 0xac70);
	usbphy_write(0x09, 0x0008);
	usbphy_write(0x12, 0x0003);
	usbphy_write(0x13, 0x0021);
	usbphy_write(0x14, 0x0fa8);
	usbphy_write(0x22, 0x3dbf);

	*((uint32_t*)0x401c0000) = 0x20000;
	*((uint32_t*)0x401c0008) = 0x803f;

	uint16_t val;
	usbphy_read(0x0a, &val);
	val |= CFG_BISTEN;
	usbphy_write(0x0a, val);

	do {
		usbphy_read(0x20, &val);
		if (val == 0x528) {
			break;
		}

		hal_sys_timer_delay(MS_TO_TICKS(1000));
	} while(1);
}

static uint64_t PLL_cfg_val;

static void usbphy_pll_set_freq_val(uint64_t freq_val)
{
    uint16_t val;
    uint16_t v[3];
    uint16_t freq_en;

    v[0] = freq_val & 0xFFFF;
    v[1] = (freq_val >> 16) & 0xFFFF;
    v[2] = (freq_val >> 32) & 0xFFFF;

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

void usbphy_pll_config(uint32_t freq, uint32_t div)
{
    uint16_t val;
    uint32_t crystal = hal_cmu_get_crystal_freq();

    PLL_cfg_val = ((uint64_t)(1 << 28) * (freq * div) + crystal / 2) / crystal;

    usbphy_pll_set_freq_val(PLL_cfg_val);

    usbphy_read(USBPHY_REG_1B, &val);
    val = SET_BITFIELD(val, DIG_USBPLL_POST_DIVN, div);
    usbphy_write(USBPHY_REG_1B, val);

    // Delay for 7us
    usbphy_read(USBPHY_REG_00, &val);
}

void usbphy_pll_set_dig_div(uint32_t div)
{
    uint16_t val;

    usbphy_read(USBPHY_REG_1B, &val);
    val = SET_BITFIELD(val, DIG_USBPLL_DIVN_480M, div);
    usbphy_write(USBPHY_REG_1B, val);
}

void usbphy_pll_codec_enable (bool enable)
{
    uint16_t val;
    static uint8_t pll_first_enabled = 0;

    if (!pll_first_enabled) {
        usbphy_read(USBPHY_REG_18, &val);
        val &= ~REG_USBPLL_CPMODE;
        usbphy_write(USBPHY_REG_18, val);

        usbphy_read(USBPHY_REG_1B, &val);
        val &= ~REG_USBPLL_BW_SEL_MASK;
        val |= REG_USBPLL_BW_SEL(2);
        usbphy_write(USBPHY_REG_1B, val);

        usbphy_read(USBPHY_REG_1E, &val);
        val &= ~REG_USBPLL_ICP_MASK;
        val |= REG_USBPLL_ICP(0x8);
        usbphy_write(USBPHY_REG_1E, val);

        usbphy_read(USBPHY_REG_36, &val);
        val &= ~PLL_INT_DEC_SEL_MASK;
        val |= PLL_INT_DEC_SEL(0x2);
        usbphy_write(USBPHY_REG_36, val);

        pll_first_enabled = 1;
    }

    usbphy_read(USBPHY_REG_18, &val);
    if (enable) {
        val |= DIG_USBPLL_BT_CLK_EN;
    } else {
        val &= ~DIG_USBPLL_BT_CLK_EN;
    }
    usbphy_write(USBPHY_REG_18, val);

    usbphy_read(USBPHY_REG_1D, &val);
    if (enable) {
        val = SET_BITFIELD(val, DIG_USBPLL_FDIV_SEL, 3);
    } else {
        val = SET_BITFIELD(val, DIG_USBPLL_FDIV_SEL, 0);
    }
    usbphy_write(USBPHY_REG_1D, val);
}

void usbphy_pll_tune(float ratio)
{
    int64_t delta, new_pll;

    new_pll = (int64_t)PLL_cfg_val;
    delta = (int64_t)(new_pll * ratio);

    new_pll += delta;

    usbphy_pll_set_freq_val((uint64_t)new_pll);
}


void pll_config_update(uint32_t tgt_freq)
{
    usbphy_pll_config(tgt_freq / 32, 32);
}


/**********************************************************************************************************************

    FUNCTION NAME:      usbphy_usb_clk_default_sel
    INPUT PARAMETERS:
        - void
    RETURN VALUE:
        - void
    DESCRIPTION:
            This function is to enable and select the 480MHz clocking source of the digital.
    CHANGELOG:
        1) 2019/10/23 binbinguo@bestechnic.com
            Initial version

**********************************************************************************************************************/

void BOOT_TEXT_FLASH_LOC usbphy_usb_clk_default_sel (void)
{
    uint16_t val_18, val_1f, val;

    usbphy_read(USBPHY_REG_18, &val_18);
    usbphy_read(USBPHY_REG_1F, &val_1f);

#ifdef CHIP_HAS_DCO
    if (dco_enabled()) {

        // enable the 480MHz clock source of the digital
        val_1f |= REG_USBPLL_DIGCLK_EN;
        // set the 480MHz clock source to DCO
        val_1f |= REG_USB_PLL_DCO_CLK480M_EN;
        usbphy_write(USBPHY_REG_1F, val_1f);

        // disable PLL 480MHz for the dig
        val_18 &= ~DIG_USBPLL_USB_EN;
        usbphy_write(USBPHY_REG_18, val_18);
        
    } else 
#endif
    {
        // enable PLL 480Mhz for the dig
        val_18 |= DIG_USBPLL_USB_EN;
        usbphy_write(USBPHY_REG_18, val_18);

        // enable the 480MHz clock source of the digital
        val_1f |= REG_USBPLL_DIGCLK_EN;
        // set the 480MHz clock source to PLL
        val_1f &= ~REG_USB_PLL_DCO_CLK480M_EN;
        usbphy_write(USBPHY_REG_1F, val_1f);
    }

    usbphy_read(USBPHY_REG_34, &val);
    val &= ~REG_USBPLL_POSTDIV_RST_TMP;
    usbphy_write(USBPHY_REG_34, val);
}

#endif





