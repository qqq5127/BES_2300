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
#include "plat_addr_map.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "hal_psc.h"
#include "reg_psc_best2000.h"

#define PSC_WRITE_ENABLE                    0xCAFE0000

#define PSC_AON_MCU_INTR_GPIO_00_37         0xFFFFFFFF
#define PSC_AON_MCU_INTR_GPIO_40_47         0xFF
#define PSC_AON_MCU_INTR_TIMER_0            (1 << 8)
#define PSC_AON_MCU_INTR_TIMER_1            (1 << 9)
#define PSC_AON_MCU_INTR_WDT                (1 << 10)
#define PSC_AON_MCU_INTR_PMU_RTC            (1 << 11)
#define PSC_AON_MCU_INTR_PMU_USB            (1 << 12)
#define PSC_AON_MCU_INTR_PMU_GPADC          (1 << 13)
#define PSC_AON_MCU_INTR_PMU_CHARGER        (1 << 14)
#define PSC_AON_MCU_INTR_PMU_PWRKEY         (1 << 15)

static struct AONPSC_T * const psc = (struct AONPSC_T *)AON_PSC_BASE;

void BOOT_TEXT_FLASH_LOC hal_psc_init(void)
{
    // Enable AON WDT IRQ and disable all others
    psc->REG_080 = 0;
    psc->REG_084 = PSC_AON_MCU_INTR_WDT;
}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_enable(void)
{
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR |
        PSC_AON_CODEC_RESETN_ASSERT_DR |
        PSC_AON_CODEC_ISO_EN_DR |
        PSC_AON_CODEC_CLK_STOP_DR;
}

void BOOT_TEXT_FLASH_LOC hal_psc_codec_disable(void)
{
    psc->REG_078 = PSC_WRITE_ENABLE |
        PSC_AON_CODEC_PSW_EN_DR | PSC_AON_CODEC_PSW_EN_REG |
        PSC_AON_CODEC_RESETN_ASSERT_DR | PSC_AON_CODEC_RESETN_ASSERT_REG |
        PSC_AON_CODEC_ISO_EN_DR | PSC_AON_CODEC_ISO_EN_REG |
        PSC_AON_CODEC_CLK_STOP_DR | PSC_AON_CODEC_CLK_STOP_REG;
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_enable(void)
{
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR | PSC_AON_BT_RESETN_ASSERT_REG |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR |
        PSC_AON_BT_RESETN_ASSERT_DR |
        PSC_AON_BT_ISO_EN_DR |
        PSC_AON_BT_CLK_STOP_DR;

#ifdef JTAG_BT
    psc->REG_064 |= PSC_AON_CODEC_RESERVED(1 << 3);
    psc->REG_064 &= ~PSC_AON_CODEC_RESERVED(1 << 2);
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_bt_disable(void)
{
#ifdef JTAG_BT
    psc->REG_064 &= ~PSC_AON_CODEC_RESERVED(1 << 3);
    psc->REG_064 |= PSC_AON_CODEC_RESERVED(1 << 2);
#endif

    psc->REG_038 = PSC_WRITE_ENABLE |
        PSC_AON_BT_PSW_EN_DR | PSC_AON_BT_PSW_EN_REG |
        PSC_AON_BT_RESETN_ASSERT_DR | PSC_AON_BT_RESETN_ASSERT_REG |
        PSC_AON_BT_ISO_EN_DR | PSC_AON_BT_ISO_EN_REG |
        PSC_AON_BT_CLK_STOP_DR | PSC_AON_BT_CLK_STOP_REG;
}

void BOOT_TEXT_FLASH_LOC hal_psc_wifi_enable(void)
{
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR | PSC_AON_WLAN_RESETN_ASSERT_REG |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR |
        PSC_AON_WLAN_RESETN_ASSERT_DR |
        PSC_AON_WLAN_ISO_EN_DR |
        PSC_AON_WLAN_CLK_STOP_DR;

#ifdef JTAG_WIFI
    psc->REG_064 &= ~PSC_AON_CODEC_RESERVED(1 << 3);
    psc->REG_064 &= ~PSC_AON_CODEC_RESERVED(1 << 2);
#endif
}

void BOOT_TEXT_FLASH_LOC hal_psc_wifi_disable(void)
{
#ifdef JTAG_WIFI
    psc->REG_064 |= PSC_AON_CODEC_RESERVED(1 << 3);
#endif

    psc->REG_058 = PSC_WRITE_ENABLE |
        PSC_AON_WLAN_PSW_EN_DR | PSC_AON_WLAN_PSW_EN_REG |
        PSC_AON_WLAN_RESETN_ASSERT_DR | PSC_AON_WLAN_RESETN_ASSERT_REG |
        PSC_AON_WLAN_ISO_EN_DR | PSC_AON_WLAN_ISO_EN_REG |
        PSC_AON_WLAN_CLK_STOP_DR | PSC_AON_WLAN_CLK_STOP_REG;
}

