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
#ifndef __REG_AONCMU_BEST2000_H__
#define __REG_AONCMU_BEST2000_H__

#include "plat_types.h"

struct AONCMU_T {
    __I  uint32_t CHIP_ID;          // 0x00
    __IO uint32_t TOP_CLK_ENABLE;   // 0x04
    __IO uint32_t TOP_CLK_DISABLE;  // 0x08
    __IO uint32_t RESET_PULSE;      // 0x0C
    __IO uint32_t RESET_SET;        // 0x10
    __IO uint32_t RESET_CLR;        // 0x14
    __IO uint32_t CLK_SELECT;       // 0x18
    __IO uint32_t CLK_OUT;          // 0x1C
    __IO uint32_t WRITE_UNLOCK;     // 0x20
    __IO uint32_t MEMSC[4];         // 0x24
    __I  uint32_t MEMSC_STATUS;     // 0x34
    __IO uint32_t BOOTMODE;         // 0x38
         uint32_t RESERVED_03C;     // 0x3C
    __IO uint32_t MOD_CLK_ENABLE;   // 0x40
    __IO uint32_t MOD_CLK_DISABLE;  // 0x44
    __IO uint32_t MOD_CLK_MODE;     // 0x48
    __IO uint32_t PLL_DIV;          // 0x4C
    __IO uint32_t TIMER_CLK;        // 0x50
    __IO uint32_t PWM01_CLK;        // 0x54
    __IO uint32_t PWM23_CLK;        // 0x58
    __IO uint32_t DCDC_CLK_ENABLE;  // 0x5C
    __IO uint32_t DCDC_CLK_DISABLE; // 0x60
    __IO uint32_t PCM_I2S_CLK;      // 0x64
    __IO uint32_t SPDIF_CLK;        // 0x68
    __IO uint32_t SLEEP_TIMER_OSC;  // 0x6C
    __IO uint32_t SLEEP_TIMER_32K;  // 0x70
    __IO uint32_t STORE_GPIO_MASK;  // 0x74
    __IO uint32_t CODEC_IIR;        // 0x78
         uint32_t RESERVED_07C[0x1D]; // 0x7C
     __IO uint32_t WAKEUP_PC;        // 0xF0
     __IO uint32_t DEBUG_RES[2];     // 0xF4
    __IO uint32_t CHIP_FEATURE;     // 0xFC
};

// reg_00
#define AON_CMU_CHIP_ID(n)                      (((n) & 0xFFFF) << 0)
#define AON_CMU_CHIP_ID_MASK                    (0xFFFF << 0)
#define AON_CMU_CHIP_ID_SHIFT                   (0)
#define AON_CMU_REVISION_ID(n)                  (((n) & 0xFFFF) << 16)
#define AON_CMU_REVISION_ID_MASK                (0xFFFF << 16)
#define AON_CMU_REVISION_ID_SHIFT               (16)

// reg_04
#define AON_CMU_EN_CLK_TOP_PLLBB_ENABLE         (1 << 0)
#define AON_CMU_EN_CLK_TOP_PLLAUD_ENABLE        (1 << 1)
#define AON_CMU_EN_CLK_TOP_OSCX2_ENABLE         (1 << 2)
#define AON_CMU_EN_CLK_TOP_OSC_ENABLE           (1 << 3)
#define AON_CMU_EN_CLK_TOP_JTAG_ENABLE          (1 << 4)
#define AON_CMU_EN_CLK_TOP_PLLBB2_ENABLE        (1 << 5)
#define AON_CMU_EN_CLK_TOP_PLLAUD2_ENABLE       (1 << 6)
#define AON_CMU_EN_CLK_PLL_CODEC_ENABLE         (1 << 7)
#define AON_CMU_EN_CLK_CODEC_HCLK_ENABLE        (1 << 8)
#define AON_CMU_EN_CLK_CODEC_ENABLE             (1 << 9)
#define AON_CMU_EN_CLK_CODEC_IIR_ENABLE         (1 << 10)
#define AON_CMU_EN_CLK_OSCX2_MCU_ENABLE         (1 << 11)
#define AON_CMU_EN_CLK_OSC_MCU_ENABLE           (1 << 12)
#define AON_CMU_EN_CLK_32K_MCU_ENABLE           (1 << 13)
#define AON_CMU_EN_CLK_DPD_WF_ENABLE            (1 << 14)
#define AON_CMU_EN_CLK_OSCX2_WF_ENABLE          (1 << 15)
#define AON_CMU_EN_CLK_OSC_WF_ENABLE            (1 << 16)
#define AON_CMU_EN_CLK_32K_WF_ENABLE            (1 << 17)
#define AON_CMU_EN_CLK_PLL_BT_ENABLE            (1 << 18)
#define AON_CMU_EN_CLK_60M_BT_ENABLE            (1 << 19)
#define AON_CMU_EN_CLK_OSCX2_BT_ENABLE          (1 << 20)
#define AON_CMU_EN_CLK_OSC_BT_ENABLE            (1 << 21)
#define AON_CMU_EN_CLK_32K_BT_ENABLE            (1 << 22)
#define AON_CMU_EN_CLK_PLL_PER_ENABLE           (1 << 23)
#define AON_CMU_PU_PLLBB_ENABLE                 (1 << 24)
#define AON_CMU_PU_PLLWF_ENABLE                 (1 << 25)
#define AON_CMU_PU_PLLUSB_ENABLE                (1 << 26)
#define AON_CMU_PU_PLLAUD_ENABLE                (1 << 27)
#define AON_CMU_PU_OSC_ENABLE                   (1 << 28)

// reg_08
#define AON_CMU_EN_CLK_TOP_PLLBB_DISABLE        (1 << 0)
#define AON_CMU_EN_CLK_TOP_PLLAUD_DISABLE       (1 << 1)
#define AON_CMU_EN_CLK_TOP_OSCX2_DISABLE        (1 << 2)
#define AON_CMU_EN_CLK_TOP_OSC_DISABLE          (1 << 3)
#define AON_CMU_EN_CLK_TOP_JTAG_DISABLE         (1 << 4)
#define AON_CMU_EN_CLK_TOP_PLLBB2_DISABLE       (1 << 5)
#define AON_CMU_EN_CLK_TOP_PLLAUD2_DISABLE      (1 << 6)
#define AON_CMU_EN_CLK_PLL_CODEC_DISABLE        (1 << 7)
#define AON_CMU_EN_CLK_CODEC_HCLK_DISABLE       (1 << 8)
#define AON_CMU_EN_CLK_CODEC_DISABLE            (1 << 9)
#define AON_CMU_EN_CLK_CODEC_IIR_DISABLE        (1 << 10)
#define AON_CMU_EN_CLK_OSCX2_MCU_DISABLE        (1 << 11)
#define AON_CMU_EN_CLK_OSC_MCU_DISABLE          (1 << 12)
#define AON_CMU_EN_CLK_32K_MCU_DISABLE          (1 << 13)
#define AON_CMU_EN_CLK_DPD_WF_DISABLE           (1 << 14)
#define AON_CMU_EN_CLK_OSCX2_WF_DISABLE         (1 << 15)
#define AON_CMU_EN_CLK_OSC_WF_DISABLE           (1 << 16)
#define AON_CMU_EN_CLK_32K_WF_DISABLE           (1 << 17)
#define AON_CMU_EN_CLK_PLL_BT_DISABLE           (1 << 18)
#define AON_CMU_EN_CLK_60M_BT_DISABLE           (1 << 19)
#define AON_CMU_EN_CLK_OSCX2_BT_DISABLE         (1 << 20)
#define AON_CMU_EN_CLK_OSC_BT_DISABLE           (1 << 21)
#define AON_CMU_EN_CLK_32K_BT_DISABLE           (1 << 22)
#define AON_CMU_EN_CLK_PLL_PER_DISABLE          (1 << 23)
#define AON_CMU_PU_PLLBB_DISABLE                (1 << 24)
#define AON_CMU_PU_PLLWF_DISABLE                (1 << 25)
#define AON_CMU_PU_PLLUSB_DISABLE               (1 << 26)
#define AON_CMU_PU_PLLAUD_DISABLE               (1 << 27)
#define AON_CMU_PU_OSC_DISABLE                  (1 << 28)

#define AON_ARST_NUM                            14
#define AON_ORST_NUM                            10
#define AON_ACLK_NUM                            AON_ARST_NUM
#define AON_OCLK_NUM                            AON_ORST_NUM

// reg_0c
#define AON_CMU_ARESETN_PULSE(n)                (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_PULSE_MASK              (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_PULSE_SHIFT             (0)
#define AON_CMU_ORESETN_PULSE(n)                (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_PULSE_MASK              (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_PULSE_SHIFT             (AON_ARST_NUM)
#define AON_CMU_SOFT_RSTN_MCU_PULSE             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1-1))
#define AON_CMU_SOFT_RSTN_CODEC_PULSE           (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1-1))
#define AON_CMU_SOFT_RSTN_WF_PULSE              (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BT_PULSE              (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_MCUCPU_PULSE          (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WFCPU_PULSE           (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BTCPU_PULSE           (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1-1))
#define AON_CMU_GLOBAL_RESETN_PULSE             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1-1))

// reg_10
#define AON_CMU_ARESETN_SET(n)                  (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_SET_MASK                (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_SET_SHIFT               (0)
#define AON_CMU_ORESETN_SET(n)                  (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_SET_MASK                (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_SET_SHIFT               (AON_ARST_NUM)
#define AON_CMU_SOFT_RSTN_MCU_SET               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1-1))
#define AON_CMU_SOFT_RSTN_CODEC_SET             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1-1))
#define AON_CMU_SOFT_RSTN_WF_SET                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BT_SET                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_MCUCPU_SET            (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WFCPU_SET             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BTCPU_SET             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1-1))
#define AON_CMU_GLOBAL_RESETN_SET               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1-1))

// reg_14
#define AON_CMU_ARESETN_CLR(n)                  (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_ARESETN_CLR_MASK                (0xFFFFFFFF << 0)
#define AON_CMU_ARESETN_CLR_SHIFT               (0)
#define AON_CMU_ORESETN_CLR(n)                  (((n) & 0xFFFFFFFF) << AON_ARST_NUM)
#define AON_CMU_ORESETN_CLR_MASK                (0xFFFFFFFF << AON_ARST_NUM)
#define AON_CMU_ORESETN_CLR_SHIFT               (AON_ARST_NUM)
#define AON_CMU_SOFT_RSTN_MCU_CLR               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1-1))
#define AON_CMU_SOFT_RSTN_CODEC_CLR             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1-1))
#define AON_CMU_SOFT_RSTN_WF_CLR                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BT_CLR                (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_MCUCPU_CLR            (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_WFCPU_CLR             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1-1))
#define AON_CMU_SOFT_RSTN_BTCPU_CLR             (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1-1))
#define AON_CMU_GLOBAL_RESETN_CLR               (1 << (AON_ARST_NUM+AON_ORST_NUM+32-8-AON_ARST_NUM-AON_ORST_NUM+1+1+1+1+1+1+1+1-1))

// reg_18
#define AON_CMU_BYPASS_DIV_BTSYS                (1 << 0)
#define AON_CMU_CFG_DIV_BTSYS(n)                (((n) & 0x3) << 1)
#define AON_CMU_CFG_DIV_BTSYS_MASK              (0x3 << 1)
#define AON_CMU_CFG_DIV_BTSYS_SHIFT             (1)
#define AON_CMU_CFG_DIV_BT60M(n)                (((n) & 0x7) << 3)
#define AON_CMU_CFG_DIV_BT60M_MASK              (0x7 << 3)
#define AON_CMU_CFG_DIV_BT60M_SHIFT             (3)
#define AON_CMU_SEL_PLLAUD_BT                   (1 << 6)
#define AON_CMU_SEL_PLLAUD_PER                  (1 << 7)
#define AON_CMU_SEL_PLLBB_AUD                   (1 << 8)
#define AON_CMU_SEL_OSCX2_DIG                   (1 << 9)
#define AON_CMU_SEL_X2_PHASE(n)                 (((n) & 0x1F) << 10)
#define AON_CMU_SEL_X2_PHASE_MASK               (0x1F << 10)
#define AON_CMU_SEL_X2_PHASE_SHIFT              (10)
#define AON_CMU_SEL_X4_BB                       (1 << 15)
#define AON_CMU_SEL_X4_AUD                      (1 << 16)
#define AON_CMU_SEL_X4_USB                      (1 << 17)
#define AON_CMU_SEL_X4_PER                      (1 << 18)
#define AON_CMU_SEL_X4_PHASE(n)                 (((n) & 0x1F) << 19)
#define AON_CMU_SEL_X4_PHASE_MASK               (0x1F << 19)
#define AON_CMU_SEL_X4_PHASE_SHIFT              (19)
#define AON_CMU_CFG_DIV_PER(n)                  (((n) & 0x3) << 24)
#define AON_CMU_CFG_DIV_PER_MASK                (0x3 << 24)
#define AON_CMU_CFG_DIV_PER_SHIFT               (24)
#define AON_CMU_SEL_32K_TIMER                   (1 << 26)
#define AON_CMU_SEL_32K_WDT                     (1 << 27)
#define AON_CMU_SEL_TIMER_FAST                  (1 << 28)
#define AON_CMU_LPU_AUTO_SWITCH26               (1 << 29)
#define AON_CMU_EN_MCU_WDG_RESET                (1 << 30)
#define AON_CMU_SEL_USB_EXT                     (1 << 31)

// reg_1c
#define AON_CMU_EN_CLK_OUT                      (1 << 0)
#define AON_CMU_SEL_CLK_OUT(n)                  (((n) & 0x7) << 1)
#define AON_CMU_SEL_CLK_OUT_MASK                (0x7 << 1)
#define AON_CMU_SEL_CLK_OUT_SHIFT               (1)
#define AON_CMU_CFG_CLK_OUT(n)                  (((n) & 0xF) << 4)
#define AON_CMU_CFG_CLK_OUT_MASK                (0xF << 4)
#define AON_CMU_CFG_CLK_OUT_SHIFT               (4)
#define AON_CMU_CFG_DIV_DCDC(n)                 (((n) & 0x7F) << 8)
#define AON_CMU_CFG_DIV_DCDC_MASK               (0x7F << 8)
#define AON_CMU_CFG_DIV_DCDC_SHIFT              (8)
#define AON_CMU_SEL_DCDC_PLL                    (1 << 15)
#define AON_CMU_SEL_DCDC_OSCX2                  (1 << 16)
#define AON_CMU_SEL_DCDC_PHASE0(n)              (((n) & 0x1F) << 17)
#define AON_CMU_SEL_DCDC_PHASE0_MASK            (0x1F << 17)
#define AON_CMU_SEL_DCDC_PHASE0_SHIFT           (17)
#define AON_CMU_SEL_DCDC_PHASE1(n)              (((n) & 0x1F) << 22)
#define AON_CMU_SEL_DCDC_PHASE1_MASK            (0x1F << 22)
#define AON_CMU_SEL_DCDC_PHASE1_SHIFT           (22)
#define AON_CMU_SEL_DCDC_PHASE2(n)              (((n) & 0x1F) << 27)
#define AON_CMU_SEL_DCDC_PHASE2_MASK            (0x1F << 27)
#define AON_CMU_SEL_DCDC_PHASE2_SHIFT           (27)

// reg_20
#define AON_CMU_WRITE_UNLOCK_H                  (1 << 0)
#define AON_CMU_WRITE_UNLOCK_STATUS             (1 << 1)

// reg_24
#define AON_CMU_MEMSC0                          (1 << 0)

// reg_28
#define AON_CMU_MEMSC1                          (1 << 0)

// reg_2c
#define AON_CMU_MEMSC2                          (1 << 0)

// reg_30
#define AON_CMU_MEMSC3                          (1 << 0)

// reg_34
#define AON_CMU_MEMSC_STATUS0                   (1 << 0)
#define AON_CMU_MEMSC_STATUS1                   (1 << 1)
#define AON_CMU_MEMSC_STATUS2                   (1 << 2)
#define AON_CMU_MEMSC_STATUS3                   (1 << 3)

// reg_38
#define AON_CMU_WATCHDOG_RESET                  (1 << 0)
#define AON_CMU_SOFT_GLOBLE_RESET               (1 << 1)
#define AON_CMU_RTC_INTR_H                      (1 << 2)
#define AON_CMU_CHG_INTR_H                      (1 << 3)
#define AON_CMU_SOFT_BOOT_MODE(n)               (((n) & 0xFFFFFFF) << 4)
#define AON_CMU_SOFT_BOOT_MODE_MASK             (0xFFFFFFF << 4)
#define AON_CMU_SOFT_BOOT_MODE_SHIFT            (4)

// reg_3c
#define AON_CMU_RESERVED(n)                     (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_RESERVED_MASK                   (0xFFFFFFFF << 0)
#define AON_CMU_RESERVED_SHIFT                  (0)

// reg_40
#define AON_CMU_MANUAL_ACLK_ENABLE(n)           (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MANUAL_ACLK_ENABLE_MASK         (0xFFFFFFFF << 0)
#define AON_CMU_MANUAL_ACLK_ENABLE_SHIFT        (0)
#define AON_CMU_MANUAL_OCLK_ENABLE(n)           (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_ENABLE_MASK         (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_ENABLE_SHIFT        (AON_ACLK_NUM)

// reg_44
#define AON_CMU_MANUAL_ACLK_DISABLE(n)          (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MANUAL_ACLK_DISABLE_MASK        (0xFFFFFFFF << 0)
#define AON_CMU_MANUAL_ACLK_DISABLE_SHIFT       (0)
#define AON_CMU_MANUAL_OCLK_DISABLE(n)          (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_DISABLE_MASK        (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MANUAL_OCLK_DISABLE_SHIFT       (AON_ACLK_NUM)

// reg_48
#define AON_CMU_MODE_ACLK(n)                    (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_MODE_ACLK_MASK                  (0xFFFFFFFF << 0)
#define AON_CMU_MODE_ACLK_SHIFT                 (0)
#define AON_CMU_MODE_OCLK(n)                    (((n) & 0xFFFFFFFF) << AON_ACLK_NUM)
#define AON_CMU_MODE_OCLK_MASK                  (0xFFFFFFFF << AON_ACLK_NUM)
#define AON_CMU_MODE_OCLK_SHIFT                 (AON_ACLK_NUM)

// reg_4c
#define AON_CMU_SEL_CLK_OSC                     (1 << 0)
#define AON_CMU_SEL_CLK_OSCX2                   (1 << 1)
#define AON_CMU_CFG_DIV_CODEC(n)                (((n) & 0x1F) << 2)
#define AON_CMU_CFG_DIV_CODEC_MASK              (0x1F << 2)
#define AON_CMU_CFG_DIV_CODEC_SHIFT             (2)
#define AON_CMU_SEL_OSC_CODEC                   (1 << 7)
#define AON_CMU_SEL_OSCX2_CODECHCLK             (1 << 8)
#define AON_CMU_SEL_PLL_CODECHCLK               (1 << 9)
#define AON_CMU_SEL_CODEC_HCLK_AON              (1 << 10)
#define AON_CMU_BYPASS_DIV_CODEC                (1 << 11)
#define AON_CMU_BYPASS_LOCK_PLLBB               (1 << 12)
#define AON_CMU_BYPASS_LOCK_PLLAUD              (1 << 13)
#define AON_CMU_POL_SPI_CS(n)                   (((n) & 0x7) << 14)
#define AON_CMU_POL_SPI_CS_MASK                 (0x7 << 14)
#define AON_CMU_POL_SPI_CS_SHIFT                (14)
#define AON_CMU_CFG_SPI_ARB(n)                  (((n) & 0x7) << 17)
#define AON_CMU_CFG_SPI_ARB_MASK                (0x7 << 17)
#define AON_CMU_CFG_SPI_ARB_SHIFT               (17)
#define AON_CMU_PU_FLASH_IO                     (1 << 20)
#define AON_CMU_PU_PSRAM_IO                     (1 << 21)
#define AON_CMU_GPIO_PMU_EN                     (1 << 22)
#define AON_CMU_LOCK_PLLBB                      (1 << 23)
#define AON_CMU_LOCK_PLLAUD                     (1 << 24)

// reg_50
#define AON_CMU_CFG_DIV_TIMER0(n)               (((n) & 0xFFFF) << 0)
#define AON_CMU_CFG_DIV_TIMER0_MASK             (0xFFFF << 0)
#define AON_CMU_CFG_DIV_TIMER0_SHIFT            (0)
#define AON_CMU_CFG_DIV_TIMER1(n)               (((n) & 0xFFFF) << 16)
#define AON_CMU_CFG_DIV_TIMER1_MASK             (0xFFFF << 16)
#define AON_CMU_CFG_DIV_TIMER1_SHIFT            (16)

// reg_54
#define AON_CMU_CFG_DIV_PWM0(n)                 (((n) & 0xFFF) << 0)
#define AON_CMU_CFG_DIV_PWM0_MASK               (0xFFF << 0)
#define AON_CMU_CFG_DIV_PWM0_SHIFT              (0)
#define AON_CMU_SEL_OSC_PWM0                    (1 << 12)
#define AON_CMU_EN_OSC_PWM0                     (1 << 13)
#define AON_CMU_CFG_DIV_PWM1(n)                 (((n) & 0xFFF) << 16)
#define AON_CMU_CFG_DIV_PWM1_MASK               (0xFFF << 16)
#define AON_CMU_CFG_DIV_PWM1_SHIFT              (16)
#define AON_CMU_SEL_OSC_PWM1                    (1 << 28)
#define AON_CMU_EN_OSC_PWM1                     (1 << 29)

// reg_58
#define AON_CMU_CFG_DIV_PWM2(n)                 (((n) & 0xFFF) << 0)
#define AON_CMU_CFG_DIV_PWM2_MASK               (0xFFF << 0)
#define AON_CMU_CFG_DIV_PWM2_SHIFT              (0)
#define AON_CMU_SEL_OSC_PWM2                    (1 << 12)
#define AON_CMU_EN_OSC_PWM2                     (1 << 13)
#define AON_CMU_CFG_DIV_PWM3(n)                 (((n) & 0xFFF) << 16)
#define AON_CMU_CFG_DIV_PWM3_MASK               (0xFFF << 16)
#define AON_CMU_CFG_DIV_PWM3_SHIFT              (16)
#define AON_CMU_SEL_OSC_PWM3                    (1 << 28)
#define AON_CMU_EN_OSC_PWM3                     (1 << 29)

// reg_5c
#define AON_CMU_EN_CLK_DCDC0_ENABLE             (1 << 0)
#define AON_CMU_EN_CLK_DCDC1_ENABLE             (1 << 1)
#define AON_CMU_EN_CLK_DCDC2_ENABLE             (1 << 2)
#define AON_CMU_WF_SEL_OSC_ENABLE               (1 << 3)
#define AON_CMU_WF_SEL_32K_ENABLE               (1 << 4)
#define AON_CMU_EN_X4_ANA_ENABLE                (1 << 5)
#define AON_CMU_EN_X2_DIG_ENABLE                (1 << 6)
#define AON_CMU_EN_X4_DIG_ENABLE                (1 << 7)
#define AON_CMU_EN_X4_AUD2_ENABLE               (1 << 8)
#define AON_CMU_EN_USB_EXT_ENABLE               (1 << 9)

// reg_60
#define AON_CMU_EN_CLK_DCDC0_DISABLE            (1 << 0)
#define AON_CMU_EN_CLK_DCDC1_DISABLE            (1 << 1)
#define AON_CMU_EN_CLK_DCDC2_DISABLE            (1 << 2)
#define AON_CMU_WF_SEL_OSC_DISABLE              (1 << 3)
#define AON_CMU_WF_SEL_32K_DISABLE              (1 << 4)
#define AON_CMU_EN_X4_ANA_DISABLE               (1 << 5)
#define AON_CMU_EN_X2_DIG_DISABLE               (1 << 6)
#define AON_CMU_EN_X4_DIG_DISABLE               (1 << 7)
#define AON_CMU_EN_X4_AUD2_DISABLE              (1 << 8)
#define AON_CMU_EN_USB_EXT_DISABLE              (1 << 9)

// reg_64
#define AON_CMU_CFG_DIV_PCM(n)                  (((n) & 0x1FFF) << 0)
#define AON_CMU_CFG_DIV_PCM_MASK                (0x1FFF << 0)
#define AON_CMU_CFG_DIV_PCM_SHIFT               (0)
#define AON_CMU_EN_CLK_PLL_PCM                  (1 << 13)
#define AON_CMU_CFG_DIV_I2S(n)                  (((n) & 0x1FFF) << 16)
#define AON_CMU_CFG_DIV_I2S_MASK                (0x1FFF << 16)
#define AON_CMU_CFG_DIV_I2S_SHIFT               (16)
#define AON_CMU_EN_CLK_PLL_I2S                  (1 << 29)
#define AON_CMU_EN_I2S_MCLK                     (1 << 30)

// reg_68
#define AON_CMU_CFG_DIV_SPDIF0(n)               (((n) & 0x1FFF) << 0)
#define AON_CMU_CFG_DIV_SPDIF0_MASK             (0x1FFF << 0)
#define AON_CMU_CFG_DIV_SPDIF0_SHIFT            (0)
#define AON_CMU_EN_CLK_PLL_SPDIF0               (1 << 13)
#define AON_CMU_CFG_DIV_SPDIF1(n)               (((n) & 0x1FFF) << 16)
#define AON_CMU_CFG_DIV_SPDIF1_MASK             (0x1FFF << 16)
#define AON_CMU_CFG_DIV_SPDIF1_SHIFT            (16)
#define AON_CMU_EN_CLK_PLL_SPDIF1               (1 << 29)

// reg_6c
#define AON_CMU_SLEEP_TIMER_OSC(n)              (((n) & 0x7FF) << 0)
#define AON_CMU_SLEEP_TIMER_OSC_MASK            (0x7FF << 0)
#define AON_CMU_SLEEP_TIMER_OSC_SHIFT           (0)
#define AON_CMU_STORE_GPIO_MASK2(n)             (((n) & 0xFF) << 16)
#define AON_CMU_STORE_GPIO_MASK2_MASK           (0xFF << 16)
#define AON_CMU_STORE_GPIO_MASK2_SHIFT          (16)
#define AON_CMU_STORE_GPIO_EN                   (1 << 30)
#define AON_CMU_STORE_TIMER                     (1 << 31)

// reg_70
#define AON_CMU_SLEEP_TIMER_32K(n)              (((n) & 0xFFFFFF) << 0)
#define AON_CMU_SLEEP_TIMER_32K_MASK            (0xFFFFFF << 0)
#define AON_CMU_SLEEP_TIMER_32K_SHIFT           (0)

// reg_74
#define AON_CMU_STORE_GPIO_MASK(n)              (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_STORE_GPIO_MASK_MASK            (0xFFFFFFFF << 0)
#define AON_CMU_STORE_GPIO_MASK_SHIFT           (0)

// reg_78
#define AON_CMU_CFG_DIV_CODECIIR(n)             (((n) & 0xF) << 0)
#define AON_CMU_CFG_DIV_CODECIIR_MASK           (0xF << 0)
#define AON_CMU_CFG_DIV_CODECIIR_SHIFT          (0)
#define AON_CMU_SEL_OSC_CODECIIR                (1 << 4)
#define AON_CMU_SEL_OSCX2_CODECIIR              (1 << 5)
#define AON_CMU_SEL_OSCX4_CODECIIR              (1 << 6)
#define AON_CMU_BYPASS_DIV_CODECIIR             (1 << 7)
#define AON_CMU_EN_CODECIIR                     (1 << 8)

// reg_f0
#define AON_CMU_DEBUG0(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG0_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG0_SHIFT                    (0)

// reg_f4
#define AON_CMU_DEBUG1(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG1_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG1_SHIFT                    (0)

// reg_f8
#define AON_CMU_DEBUG2(n)                       (((n) & 0xFFFFFFFF) << 0)
#define AON_CMU_DEBUG2_MASK                     (0xFFFFFFFF << 0)
#define AON_CMU_DEBUG2_SHIFT                    (0)

// reg_fc
#define AON_CMU_EFUSE(n)                        (((n) & 0xFFFF) << 0)
#define AON_CMU_EFUSE_MASK                      (0xFFFF << 0)
#define AON_CMU_EFUSE_SHIFT                     (0)
#define AON_CMU_EFUSE_LOCK                      (1 << 31)


// APB and AHB Clocks:
#define AON_ACLK_CMU                    (1 << 0)
#define AON_ARST_CMU                    (1 << 0)
#define AON_ACLK_GPIO                   (1 << 1)
#define AON_ARST_GPIO                   (1 << 1)
#define AON_ACLK_GPIO_INT               (1 << 2)
#define AON_ARST_GPIO_INT               (1 << 2)
#define AON_ACLK_WDT                    (1 << 3)
#define AON_ARST_WDT                    (1 << 3)
#define AON_ACLK_PWM                    (1 << 4)
#define AON_ARST_PWM                    (1 << 4)
#define AON_ACLK_TIMER                  (1 << 5)
#define AON_ARST_TIMER                  (1 << 5)
#define AON_ACLK_PSC                    (1 << 6)
#define AON_ARST_PSC                    (1 << 6)
#define AON_ACLK_IOMUX                  (1 << 7)
#define AON_ARST_IOMUX                  (1 << 7)
#define AON_ACLK_AHBC                   (1 << 8)
#define AON_ARST_AHBC                   (1 << 8)
#define AON_ACLK_APBC                   (1 << 9)
#define AON_ARST_APBC                   (1 << 9)
#define AON_ACLK_SRAM                   (1 << 10)
#define AON_ARST_SRAM                   (1 << 10)
#define AON_ACLK_H2H_MCU                (1 << 11)
#define AON_ARST_H2H_MCU                (1 << 11)
#define AON_ACLK_H2H_BT                 (1 << 12)
#define AON_ARST_H2H_BT                 (1 << 12)
#define AON_ACLK_H2H_WF                 (1 << 13)
#define AON_ARST_H2H_WF                 (1 << 13)

// AON other Clocks:
#define AON_OCLK_WDT                    (1 << 0)
#define AON_ORST_WDT                    (1 << 0)
#define AON_OCLK_TIMER                  (1 << 1)
#define AON_ORST_TIMER                  (1 << 1)
#define AON_OCLK_GPIO                   (1 << 2)
#define AON_ORST_GPIO                   (1 << 2)
#define AON_OCLK_PWM0                   (1 << 3)
#define AON_ORST_PWM0                   (1 << 3)
#define AON_OCLK_PWM1                   (1 << 4)
#define AON_ORST_PWM1                   (1 << 4)
#define AON_OCLK_PWM2                   (1 << 5)
#define AON_ORST_PWM2                   (1 << 5)
#define AON_OCLK_PWM3                   (1 << 6)
#define AON_ORST_PWM3                   (1 << 6)
#define AON_OCLK_IOMUX                  (1 << 7)
#define AON_ORST_IOMUX                  (1 << 7)
#define AON_OCLK_SLP32K                 (1 << 8)
#define AON_ORST_SLP32K                 (1 << 8)
#define AON_OCLK_SLP26M                 (1 << 9)
#define AON_ORST_SLP26M                 (1 << 9)

#endif

