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
#include "usbphy.h"
#include "hal_cmu.h"
#include "hal_phyif.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include CHIP_SPECIFIC_HDR(reg_usbphy)

#ifdef CHIP_HAS_SPIPHY

//#define USBPHY_SERIAL_ITF

void usbphy_open(void)
{
    int ret;
    unsigned short val_01, val;

    ret = hal_phyif_open(SPIPHY_USB_CS);
    if (ret) {
        return;
    }

#ifdef USB_HIGH_SPEED

    usbphy_read(0x01, &val_01);
    val_01 &= ~((1 << 0) | (1 << 11));
    usbphy_write(0x01, val_01);
    val_01 |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 13);
#ifdef USBPHY_SERIAL_ITF
    // cdr_clk polariy=1
    val_01 |= (1 << 4);
#else
    // cdr_clk polariy=0
    val_01 &= ~(1 << 4);
#endif
    usbphy_write(0x01, val_01);

    usbphy_read(0x02, &val);
    val = (val & ~(0x1F << 10)) | (0xC << 10);
    usbphy_write(0x02, val);

    usbphy_read(0x05, &val);
    val |= (1 << 8);
    usbphy_write(0x05, val);

    // Tx off during rx
    usbphy_read(0x06, &val);
    val |= (1 << 13) | (1 << 14);
    // Set hs_ibias_trim to 7 (default is 4)
    //val = (val & ~(7 << 4)) | (7 << 4);
    usbphy_write(0x06, val);

    usbphy_read(0x07, &val);
    val &= ~(1 << 8);
    usbphy_write(0x07, val);

    usbphy_read(0x08, &val);
    val = (val & ~((0xF << 0) | (0x3 << 4) | (0x3 << 6))) | (0x0 << 0) | (0x3 << 4) | (0x1 << 6);
    usbphy_write(0x08, val);

    usbphy_read(0x09, &val);
    val &= ~(1 << 0);
    val |= (1 << 3);
    usbphy_write(0x09, val);

    // Disable force clocks, and disable hs son signal
    // Enable parallel dig-to-phy interface
    usbphy_read(0x22, &val);
    val &= ~((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));
    val |= (1 << 10) | (1 << 11);
    usbphy_write(0x22, val);

    // Ignore all UTMI errors
    usbphy_write(0x12, 0x0003);

    // RX FIFO is set to 32 bits
    usbphy_read(0x13, &val);
    val = (val & ~(0xF << 3)) | (0x4 << 3);
    usbphy_write(0x13, val);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    val_01 |= (1 << 0);
    usbphy_write(0x01, val_01);

#ifdef USB_HS_LOOPBACK_TEST
    usbphy_read(0x0A, &val);
    val |= (1 << 3) | (1 << 4) | (1 << 6);
    usbphy_write(0x0A, val);
    usbphy_read(0x0B, &val);
    val &= ~((1 << 3) | (1 << 4) | (3 << 6));
    usbphy_write(0x0B, val);
    usbphy_read(0x09, &val);
    val |= (1 << 2);
    usbphy_write(0x09, val);
    usbphy_read(0x06, &val);
    val |= (1 << 12);
    usbphy_write(0x06, val);

    hal_sys_timer_delay(MS_TO_TICKS(1));

#define LOOPBACK_PATTERN                (*(volatile uint8_t *)0x20000004)

    LOOPBACK_PATTERN = 0;
    uint8_t pattern = LOOPBACK_PATTERN;

    // Set tx pattern
    usbphy_read(0x03, &val);
    val = (val & ~(0xFF << 8)) | (pattern << 8);
    usbphy_write(0x03, val);

    // Enable force clocks
    usbphy_read(0x22, &val);
    val |= (1 << 4) | (1 << 5) | (1 << 6);
    usbphy_write(0x22, val);

    // Enable tx
    usbphy_read(0x04, &val);
    val |= (1 << 1);
    usbphy_write(0x04, val);

    // Update tx pattern
    while (1) {
        if (pattern != LOOPBACK_PATTERN) {
            pattern = LOOPBACK_PATTERN;
            usbphy_read(0x03, &val);
            val = (val & ~(0xFF << 8)) | (pattern << 8);
            usbphy_write(0x03, val);
        }
    }
#endif

#else // !USB_HIGH_SPEED

    usbphy_read(0x01, &val_01);
    val_01 &= ~(1 << 0);
    usbphy_write(0x01, val_01);
    val_01 |= (1 << 3);
    usbphy_write(0x01, val_01);

    val = (1 << 13);
    usbphy_write(0x06, val);

    usbphy_read(0x07, &val);
    val &= ~(1 << 8);
    usbphy_write(0x07, val);

    val = 0;
    usbphy_write(0x08, val);

    val = 0;
    usbphy_write(0x09, val);

    val_01 |= (1 << 0);
    usbphy_write(0x01, val_01);

#endif // USB_HIGH_SPEED

}

void usbphy_close(void)
{
    hal_phyif_close(SPIPHY_USB_CS);
}

void usbphy_sleep(void)
{
    uint16_t val;

    usbphy_read(0x01, &val);
#ifdef USB_HIGH_SPEED
    val &= ~((1 << 1) | (1 << 2) | (1 << 3));
#else
    val &= ~(1 << 3);
#endif
    usbphy_write(0x01, val);
}

void usbphy_wakeup(void)
{
    uint16_t val;

    usbphy_read(0x01, &val);
#ifdef USB_HIGH_SPEED
    val |= (1 << 1) | (1 << 2) | (1 << 3);
#else
    val |= (1 << 3);
#endif
    usbphy_write(0x01, val);
}

void usbphy_div_reset_set()
{
    uint16_t val;
    usbphy_read(USBPHY_REG_34, &val);
    val |= REG_USBPLL_POSTDIV_RST;
    usbphy_write(USBPHY_REG_34, val);
    usbphy_read(USBPHY_REG_1B, &val);
    val |= DIG_USBPLL_POST_DIVN_SHIFT;
    usbphy_write(USBPHY_REG_1B, val);
}
void usbphy_div_reset_clear(uint32_t div)
{
    uint16_t val;
    usbphy_read(USBPHY_REG_34, &val);
    val &= ~REG_USBPLL_POSTDIV_RST;
    usbphy_write(USBPHY_REG_34, val);
    usbphy_read(USBPHY_REG_1B, &val);
    val = SET_BITFIELD(val, DIG_USBPLL_POST_DIVN, div);
    usbphy_write(USBPHY_REG_1B, val);
}
void usbphy_div_set(uint32_t div)
{
    uint16_t val;

    usbphy_read(USBPHY_REG_1B, &val);
    val |= DIG_USBPLL_POST_DIVN_SHIFT;
    usbphy_write(USBPHY_REG_1B, val);
    if (div != (DIG_USBPLL_POST_DIVN_MASK >> DIG_USBPLL_POST_DIVN_SHIFT)) {
        usbphy_read(USBPHY_REG_1B, &val);
        val = SET_BITFIELD(val, DIG_USBPLL_POST_DIVN, div);
        usbphy_write(USBPHY_REG_1B, val);
    }
}


#endif

