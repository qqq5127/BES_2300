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
#ifndef __REG_CMU_BEST2000_H__
#define __REG_CMU_BEST2000_H__

#include "plat_types.h"

struct CMU_T {
    __IO uint32_t HCLK_ENABLE;      // 0x00
    __IO uint32_t HCLK_DISABLE;     // 0x04
    __IO uint32_t PCLK_ENABLE;      // 0x08
    __IO uint32_t PCLK_DISABLE;     // 0x0C
    __IO uint32_t OCLK_ENABLE;      // 0x10
    __IO uint32_t OCLK_DISABLE;     // 0x14
    __IO uint32_t HCLK_MODE;        // 0x18
    __IO uint32_t PCLK_MODE;        // 0x1C
    __IO uint32_t OCLK_MODE;        // 0x20
         uint32_t RESERVED_024;     // 0x24
    __IO uint32_t HRESET_PULSE;     // 0x28
    __IO uint32_t PRESET_PULSE;     // 0x2C
    __IO uint32_t ORESET_PULSE;     // 0x30
    __IO uint32_t HRESET_SET;       // 0x34
    __IO uint32_t HRESET_CLR;       // 0x38
    __IO uint32_t PRESET_SET;       // 0x3C
    __IO uint32_t PRESET_CLR;       // 0x40
    __IO uint32_t ORESET_SET;       // 0x44
    __IO uint32_t ORESET_CLR;       // 0x48
    __IO uint32_t TIMER_CLK;        // 0x4C
    __IO uint32_t BOOTMODE;         // 0x50
    __IO uint32_t MCU_TIMER;        // 0x54
    __IO uint32_t SLEEP;            // 0x58
    __IO uint32_t PERIPH_CLK;       // 0x5C
    __IO uint32_t SYS_CLK_ENABLE;   // 0x60
    __IO uint32_t SYS_CLK_DISABLE;  // 0x64
    __IO uint32_t RESERVED_068;     // 0x68
    __IO uint32_t BOOT_DVS;         // 0x6C
    __IO uint32_t UART_CLK;         // 0x70
    __IO uint32_t I2C_CLK;          // 0x74
    __IO uint32_t PWM01_CLK;        // 0x78
    __IO uint32_t PWM23_CLK;        // 0x7C
    __IO uint32_t WRITE_UNLOCK;     // 0x80
    __IO uint32_t WAKEUP_MASK0;     // 0x84
    __IO uint32_t WAKEUP_MASK1;     // 0x88
    __IO uint32_t WAKEUP_CLK_CFG;   // 0x8C
    __IO uint32_t PSRAM_DLL;        // 0x90
    __IO uint32_t RESERVED_094;     // 0x94
    __IO uint32_t RESERVED_098;     // 0x98
    __IO uint32_t RESERVED_09C;     // 0x9C
    __IO uint32_t ISIRQ_SET;        // 0xA0
    __IO uint32_t ISIRQ_CLR;        // 0xA4
    __IO uint32_t SYS_DIV;          // 0xA8
    __IO uint32_t DMA_CFG;          // 0xAC
    __IO uint32_t MCU2BT_INTMASK0;  // 0xB0
    __IO uint32_t MCU2BT_INTMASK1;  // 0xB4
    __IO uint32_t MCU2WF_INTMASK0;  // 0xB8
    __IO uint32_t MCU2WF_INTMASK1;  // 0xBC
    __IO uint32_t MEMSC[4];         // 0xC0
    __I  uint32_t MEMSC_STATUS;     // 0xD0
         uint32_t RESERVED2[7];     // 0xD4
    __IO uint32_t MISC;             // 0xF0
    __IO uint32_t SIMU_RES;         // 0xF4
};

// reg_00
#define CMU_MANUAL_HCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_ENABLE_SHIFT            (0)

// reg_04
#define CMU_MANUAL_HCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_DISABLE_SHIFT           (0)

// reg_08
#define CMU_MANUAL_PCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_ENABLE_SHIFT            (0)

// reg_0c
#define CMU_MANUAL_PCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_DISABLE_SHIFT           (0)

// reg_10
#define CMU_MANUAL_OCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_ENABLE_SHIFT            (0)

// reg_14
#define CMU_MANUAL_OCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_DISABLE_SHIFT           (0)

// reg_18
#define CMU_MODE_HCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_HCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_HCLK_SHIFT                     (0)

// reg_1c
#define CMU_MODE_PCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_PCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_PCLK_SHIFT                     (0)

// reg_20
#define CMU_MODE_OCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_OCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_OCLK_SHIFT                     (0)

// reg_28
#define CMU_HRESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_HRESETN_PULSE_SHIFT                 (0)

#define SYS_PRST_NUM                            19

// reg_2c
#define CMU_PRESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_PRESETN_PULSE_SHIFT                 (0)
#define CMU_GLOBAL_RESETN_PULSE                 (1 << SYS_PRST_NUM+1-1)

// reg_30
#define CMU_ORESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_ORESETN_PULSE_SHIFT                 (0)

// reg_34
#define CMU_HRESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_HRESETN_SET_SHIFT                   (0)

// reg_38
#define CMU_HRESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_HRESETN_CLR_SHIFT                   (0)

// reg_3c
#define CMU_PRESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_PRESETN_SET_SHIFT                   (0)
#define CMU_GLOBAL_RESETN_SET                   (1 << SYS_PRST_NUM+1-1)

// reg_40
#define CMU_PRESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_PRESETN_CLR_SHIFT                   (0)
#define CMU_GLOBAL_RESETN_CLR                   (1 << SYS_PRST_NUM+1-1)

// reg_44
#define CMU_ORESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_ORESETN_SET_SHIFT                   (0)

// reg_48
#define CMU_ORESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_ORESETN_CLR_SHIFT                   (0)

// reg_4c
#define CMU_CFG_DIV_TIMER0(n)                   (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER0_MASK                 (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER0_SHIFT                (0)
#define CMU_CFG_DIV_TIMER1(n)                   (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER1_MASK                 (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER1_SHIFT                (16)

// reg_50
#define CMU_WATCHDOG_RESET                      (1 << 0)
#define CMU_SOFT_GLOBLE_RESET                   (1 << 1)
#define CMU_RTC_INTR_H                          (1 << 2)
#define CMU_CHG_INTR_H                          (1 << 3)
#define CMU_SOFT_BOOT_MODE(n)                   (((n) & 0xFFFFFFF) << 4)
#define CMU_SOFT_BOOT_MODE_MASK                 (0xFFFFFFF << 4)
#define CMU_SOFT_BOOT_MODE_SHIFT                (4)

// reg_54
#define CMU_CFG_HCLK_MCU_OFF_TIMER(n)           (((n) & 0xFF) << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_MASK         (0xFF << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_SHIFT        (0)
#define CMU_HCLK_MCU_ENABLE                     (1 << 8)
#define CMU_SECURE_BOOT                         (1 << 9)
#define CMU_RAM_RETN_UP_EARLY                   (1 << 10)

// reg_58
#define CMU_SLEEP_TIMER(n)                      (((n) & 0xFFFFFF) << 0)
#define CMU_SLEEP_TIMER_MASK                    (0xFFFFFF << 0)
#define CMU_SLEEP_TIMER_SHIFT                   (0)
#define CMU_SLEEP_TIMER_EN                      (1 << 24)
#define CMU_DEEPSLEEP_EN                        (1 << 25)
#define CMU_DEEPSLEEP_ROMRAM_EN                 (1 << 26)
#define CMU_MANUAL_RAM_RETN                     (1 << 27)
#define CMU_DEEPSLEEP_START                     (1 << 28)
#define CMU_DEEPSLEEP_MODE                      (1 << 29)
#define CMU_PU_OSC                              (1 << 30)
#define CMU_WAKEUP_DEEPSLEEP_L                  (1 << 31)

// reg_5c
#define CMU_CFG_DIV_MCLK(n)                     (((n) & 0xF) << 0)
#define CMU_CFG_DIV_MCLK_MASK                   (0xF << 0)
#define CMU_CFG_DIV_MCLK_SHIFT                  (0)
#define CMU_SEL_OSCX2_MCLK                      (1 << 4)
#define CMU_SEL_PLL_MCLK                        (1 << 5)
#define CMU_EN_PLL_MCLK                         (1 << 6)
#define CMU_EN_MCLK                             (1 << 7)
#define CMU_POL_CLK_MCLK                        (1 << 8)
#define CMU_EN_CAMP                             (1 << 9)
#define CMU_POL_CLK_CAMP                        (1 << 10)
#define CMU_CFG_DIV_SDIO(n)                     (((n) & 0xF) << 11)
#define CMU_CFG_DIV_SDIO_MASK                   (0xF << 11)
#define CMU_CFG_DIV_SDIO_SHIFT                  (11)
#define CMU_SEL_OSCX2_SDIO                      (1 << 15)
#define CMU_SEL_PLL_SDIO                        (1 << 16)
#define CMU_EN_PLL_SDIO                         (1 << 17)
#define CMU_CFG_DIV_SDMMC(n)                    (((n) & 0xF) << 18)
#define CMU_CFG_DIV_SDMMC_MASK                  (0xF << 18)
#define CMU_CFG_DIV_SDMMC_SHIFT                 (18)
#define CMU_SEL_OSCX2_SDMMC                     (1 << 22)
#define CMU_SEL_PLL_SDMMC                       (1 << 23)
#define CMU_EN_PLL_SDMMC                        (1 << 24)
#define CMU_SEL_32K_TIMER                       (1 << 25)
#define CMU_SEL_32K_WDT                         (1 << 26)
#define CMU_SEL_TIMER_FAST                      (1 << 27)
#define CMU_CFG_CLK_OUT(n)                      (((n) & 0xF) << 28)
#define CMU_CFG_CLK_OUT_MASK                    (0xF << 28)
#define CMU_CFG_CLK_OUT_SHIFT                   (28)

// reg_60
#define CMU_RSTN_DIV_FLS_ENABLE                 (1 << 0)
#define CMU_SEL_OSC_FLS_ENABLE                  (1 << 1)
#define CMU_SEL_OSCX2_FLS_ENABLE                (1 << 2)
#define CMU_SEL_PLL_FLS_ENABLE                  (1 << 3)
#define CMU_SEL_PLLAUD_FLS_ENABLE               (1 << 4)
#define CMU_BYPASS_DIV_FLS_ENABLE               (1 << 5)
#define CMU_RSTN_DIV_MEM_ENABLE                 (1 << 6)
#define CMU_SEL_OSC_MEM_ENABLE                  (1 << 7)
#define CMU_SEL_OSCX2_MEM_ENABLE                (1 << 8)
#define CMU_SEL_PLL_MEM_ENABLE                  (1 << 9)
#define CMU_SEL_PLLAUD_MEM_ENABLE               (1 << 10)
#define CMU_BYPASS_DIV_MEM_ENABLE               (1 << 11)
#define CMU_RSTN_DIV_SYS_ENABLE                 (1 << 12)
#define CMU_SEL_OSC_SYS_ENABLE                  (1 << 13)
#define CMU_SEL_OSCX2_SYS_ENABLE                (1 << 14)
#define CMU_SEL_PLL_SYS_ENABLE                  (1 << 15)
#define CMU_SEL_PLLAUD_SYS_ENABLE               (1 << 16)
#define CMU_BYPASS_DIV_SYS_ENABLE               (1 << 17)
#define CMU_EN_PLLUSB_ENABLE                    (1 << 18)
#define CMU_EN_PLL_AUD_ENABLE                   (1 << 19)
#define CMU_EN_PLL_BB_ENABLE                    (1 << 20)
#define CMU_PU_PLL_AUD_ENABLE                   (1 << 21)
#define CMU_PU_PLL_BB_ENABLE                    (1 << 22)

// reg_64
#define CMU_RSTN_DIV_FLS_DISABLE                (1 << 0)
#define CMU_SEL_OSC_FLS_DISABLE                 (1 << 1)
#define CMU_SEL_OSCX2_FLS_DISABLE               (1 << 2)
#define CMU_SEL_PLL_FLS_DISABLE                 (1 << 3)
#define CMU_SEL_PLLAUD_FLS_DISABLE              (1 << 4)
#define CMU_BYPASS_DIV_FLS_DISABLE              (1 << 5)
#define CMU_RSTN_DIV_MEM_DISABLE                (1 << 6)
#define CMU_SEL_OSC_MEM_DISABLE                 (1 << 7)
#define CMU_SEL_OSCX2_MEM_DISABLE               (1 << 8)
#define CMU_SEL_PLL_MEM_DISABLE                 (1 << 9)
#define CMU_SEL_PLLAUD_MEM_DISABLE              (1 << 10)
#define CMU_BYPASS_DIV_MEM_DISABLE              (1 << 11)
#define CMU_RSTN_DIV_SYS_DISABLE                (1 << 12)
#define CMU_SEL_OSC_SYS_DISABLE                 (1 << 13)
#define CMU_SEL_OSCX2_SYS_DISABLE               (1 << 14)
#define CMU_SEL_PLL_SYS_DISABLE                 (1 << 15)
#define CMU_SEL_PLLAUD_SYS_DISABLE              (1 << 16)
#define CMU_BYPASS_DIV_SYS_DISABLE              (1 << 17)
#define CMU_EN_PLLUSB_DISABLE                   (1 << 18)
#define CMU_EN_PLL_AUD_DISABLE                  (1 << 19)
#define CMU_EN_PLL_BB_DISABLE                   (1 << 20)
#define CMU_PU_PLL_AUD_DISABLE                  (1 << 21)
#define CMU_PU_PLL_BB_DISABLE                   (1 << 22)

// reg_6c
#define CMU_ROM_EMA(n)                          (((n) & 0x7) << 0)
#define CMU_ROM_EMA_MASK                        (0x7 << 0)
#define CMU_ROM_EMA_SHIFT                       (0)
#define CMU_ROM_KEN                             (1 << 3)
#define CMU_RAM_EMA(n)                          (((n) & 0x7) << 4)
#define CMU_RAM_EMA_MASK                        (0x7 << 4)
#define CMU_RAM_EMA_SHIFT                       (4)
#define CMU_RAM_EMAW(n)                         (((n) & 0x3) << 7)
#define CMU_RAM_EMAW_MASK                       (0x3 << 7)
#define CMU_RAM_EMAW_SHIFT                      (7)

// reg_70
#define CMU_CFG_DIV_UART0(n)                    (((n) & 0x1F) << 0)
#define CMU_CFG_DIV_UART0_MASK                  (0x1F << 0)
#define CMU_CFG_DIV_UART0_SHIFT                 (0)
#define CMU_SEL_OSCX2_UART0                     (1 << 5)
#define CMU_SEL_PLL_UART0                       (1 << 6)
#define CMU_EN_PLL_UART0                        (1 << 7)
#define CMU_CFG_DIV_UART1(n)                    (((n) & 0x1F) << 8)
#define CMU_CFG_DIV_UART1_MASK                  (0x1F << 8)
#define CMU_CFG_DIV_UART1_SHIFT                 (8)
#define CMU_SEL_OSCX2_UART1                     (1 << 13)
#define CMU_SEL_PLL_UART1                       (1 << 14)
#define CMU_EN_PLL_UART1                        (1 << 15)
#define CMU_CFG_DIV_UART2(n)                    (((n) & 0x1F) << 16)
#define CMU_CFG_DIV_UART2_MASK                  (0x1F << 16)
#define CMU_CFG_DIV_UART2_SHIFT                 (16)
#define CMU_SEL_OSCX2_UART2                     (1 << 21)
#define CMU_SEL_PLL_UART2                       (1 << 22)
#define CMU_EN_PLL_UART2                        (1 << 23)

// reg_74
#define CMU_CFG_DIV_I2C(n)                      (((n) & 0xFF) << 0)
#define CMU_CFG_DIV_I2C_MASK                    (0xFF << 0)
#define CMU_CFG_DIV_I2C_SHIFT                   (0)
#define CMU_SEL_OSC_I2C                         (1 << 8)
#define CMU_SEL_OSCX2_I2C                       (1 << 9)
#define CMU_SEL_PLL_I2C                         (1 << 10)
#define CMU_EN_PLL_I2C                          (1 << 11)
#define CMU_POL_CLK_PCM_IN                      (1 << 12)
#define CMU_SEL_PCM_CLKIN                       (1 << 13)
#define CMU_EN_CLK_PCM_OUT                      (1 << 14)
#define CMU_POL_CLK_PCM_OUT                     (1 << 15)
#define CMU_POL_CLK_I2S_IN                      (1 << 16)
#define CMU_SEL_I2S_CLKIN                       (1 << 17)
#define CMU_EN_CLK_I2S_OUT                      (1 << 18)
#define CMU_POL_CLK_I2S_OUT                     (1 << 19)
#define CMU_FORCE_PU_OFF                        (1 << 20)
#define CMU_LOCK_CPU_EN                         (1 << 21)
#define CMU_SEL_ROM_FAST                        (1 << 22)

// reg_78
#define CMU_CFG_DIV_PWM0(n)                     (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM0_MASK                   (0xFFF << 0)
#define CMU_CFG_DIV_PWM0_SHIFT                  (0)
#define CMU_SEL_OSC_PWM0                        (1 << 12)
#define CMU_EN_OSC_PWM0                         (1 << 13)
#define CMU_CFG_DIV_PWM1(n)                     (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM1_MASK                   (0xFFF << 16)
#define CMU_CFG_DIV_PWM1_SHIFT                  (16)
#define CMU_SEL_OSC_PWM1                        (1 << 28)
#define CMU_EN_OSC_PWM1                         (1 << 29)

// reg_7c
#define CMU_CFG_DIV_PWM2(n)                     (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM2_MASK                   (0xFFF << 0)
#define CMU_CFG_DIV_PWM2_SHIFT                  (0)
#define CMU_SEL_OSC_PWM2                        (1 << 12)
#define CMU_EN_OSC_PWM2                         (1 << 13)
#define CMU_CFG_DIV_PWM3(n)                     (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM3_MASK                   (0xFFF << 16)
#define CMU_CFG_DIV_PWM3_SHIFT                  (16)
#define CMU_SEL_OSC_PWM3                        (1 << 28)
#define CMU_EN_OSC_PWM3                         (1 << 29)

// reg_80
#define CMU_WRITE_UNLOCK_H                      (1 << 0)
#define CMU_WRITE_UNLOCK_STATUS                 (1 << 1)

// reg_84
#define CMU_WAKEUP_IRQ_MASK0(n)                 (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK0_MASK               (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK0_SHIFT              (0)

// reg_88
#define CMU_WAKEUP_IRQ_MASK1(n)                 (((n) & 0xFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK1_MASK               (0xFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK1_SHIFT              (0)

// reg_8c
#define CMU_TIMER_WT26(n)                       (((n) & 0xFF) << 0)
#define CMU_TIMER_WT26_MASK                     (0xFF << 0)
#define CMU_TIMER_WT26_SHIFT                    (0)
#define CMU_TIMER_WTPLL(n)                      (((n) & 0xF) << 8)
#define CMU_TIMER_WTPLL_MASK                    (0xF << 8)
#define CMU_TIMER_WTPLL_SHIFT                   (8)
#define CMU_LPU_AUTO_SWITCH26                   (1 << 12)
#define CMU_LPU_AUTO_SWITCHPLL                  (1 << 13)
#define CMU_LPU_STATUS_26M                      (1 << 14)
#define CMU_LPU_STATUS_PLL                      (1 << 15)

// reg_90
#define CMU_PSRAM_DLL_LOCK_DIV(n)               (((n) & 0xFFFF) << 0)
#define CMU_PSRAM_DLL_LOCK_DIV_MASK             (0xFFFF << 0)
#define CMU_PSRAM_DLL_LOCK_DIV_SHIFT            (0)
#define CMU_PSRAM_DLL_LOCK_TRIGGER              (1 << 16)
#define CMU_PSRAM_LOCK_STAT                     (1 << 20)

// reg_a0
#define CMU_BT2MCU_DATA_DONE_SET                (1 << 0)
#define CMU_BT2MCU_DATA1_DONE_SET               (1 << 1)
#define CMU_MCU2BT_DATA_IND_SET                 (1 << 2)
#define CMU_MCU2BT_DATA1_IND_SET                (1 << 3)
#define CMU_BT_ALLIRQ_MASK_SET                  (1 << 4)

// reg_a4
#define CMU_BT2MCU_DATA_DONE_CLR                (1 << 0)
#define CMU_BT2MCU_DATA1_DONE_CLR               (1 << 1)
#define CMU_MCU2BT_DATA_IND_CLR                 (1 << 2)
#define CMU_MCU2BT_DATA1_IND_CLR                (1 << 3)
#define CMU_BT_ALLIRQ_MASK_CLR                  (1 << 4)

// reg_a8
#define CMU_CFG_DIV_SYS(n)                      (((n) & 0x3) << 0)
#define CMU_CFG_DIV_SYS_MASK                    (0x3 << 0)
#define CMU_CFG_DIV_SYS_SHIFT                   (0)
#define CMU_CFG_DIV_MEM(n)                      (((n) & 0x3) << 2)
#define CMU_CFG_DIV_MEM_MASK                    (0x3 << 2)
#define CMU_CFG_DIV_MEM_SHIFT                   (2)
#define CMU_CFG_DIV_FLS(n)                      (((n) & 0x3) << 4)
#define CMU_CFG_DIV_FLS_MASK                    (0x3 << 4)
#define CMU_CFG_DIV_FLS_SHIFT                   (4)
#define CMU_SEL_USB_6M                          (1 << 6)
#define CMU_SEL_USB_SRC(n)                      (((n) & 0x7) << 7)
#define CMU_SEL_USB_SRC_MASK                    (0x7 << 7)
#define CMU_SEL_USB_SRC_SHIFT                   (7)
#define CMU_POL_CLK_USB                         (1 << 10)
#define CMU_USB_ID                              (1 << 11)
#define CMU_CFG_DIV_PCLK(n)                     (((n) & 0x3) << 12)
#define CMU_CFG_DIV_PCLK_MASK                   (0x3 << 12)
#define CMU_CFG_DIV_PCLK_SHIFT                  (12)
#define CMU_CFG_DIV_SPI0(n)                     (((n) & 0xF) << 14)
#define CMU_CFG_DIV_SPI0_MASK                   (0xF << 14)
#define CMU_CFG_DIV_SPI0_SHIFT                  (14)
#define CMU_SEL_OSCX2_SPI0                      (1 << 18)
#define CMU_SEL_PLL_SPI0                        (1 << 19)
#define CMU_EN_PLL_SPI0                         (1 << 20)
#define CMU_CFG_DIV_SPI1(n)                     (((n) & 0xF) << 21)
#define CMU_CFG_DIV_SPI1_MASK                   (0xF << 21)
#define CMU_CFG_DIV_SPI1_SHIFT                  (21)
#define CMU_SEL_OSCX2_SPI1                      (1 << 25)
#define CMU_SEL_PLL_SPI1                        (1 << 26)
#define CMU_EN_PLL_SPI1                         (1 << 27)
#define CMU_SEL_OSCX2_SPI2                      (1 << 28)
#define CMU_CFG_SEL_WLAN(n)                     (((n) & 0x7) << 29)
#define CMU_CFG_SEL_WLAN_MASK                   (0x7 << 29)
#define CMU_CFG_SEL_WLAN_SHIFT                  (29)

// reg_ac
#define CMU_DMA_HANDSHAKE_SWAP(n)               (((n) & 0xFFFF) << 0)
#define CMU_DMA_HANDSHAKE_SWAP_MASK             (0xFFFF << 0)
#define CMU_DMA_HANDSHAKE_SWAP_SHIFT            (0)
#define CMU_CAM_SPI_DMAREQ_SEL                  (1 << 16)
#define CMU_DSD_PCM_DMAREQ_SEL                  (1 << 17)
#define CMU_RESERVED_2(n)                       (((n) & 0x3FFF) << 18)
#define CMU_RESERVED_2_MASK                     (0x3FFF << 18)
#define CMU_RESERVED_2_SHIFT                    (18)

// reg_b0
#define CMU_MCU2BT_INTISR_MASK0(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MCU2BT_INTISR_MASK0_MASK            (0xFFFFFFFF << 0)
#define CMU_MCU2BT_INTISR_MASK0_SHIFT           (0)

// reg_b4
#define CMU_MCU2BT_INTISR_MASK1(n)              (((n) & 0xFFFF) << 0)
#define CMU_MCU2BT_INTISR_MASK1_MASK            (0xFFFF << 0)
#define CMU_MCU2BT_INTISR_MASK1_SHIFT           (0)

// reg_b8
#define CMU_MCU2WF_INTISR_MASK0(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MCU2WF_INTISR_MASK0_MASK            (0xFFFFFFFF << 0)
#define CMU_MCU2WF_INTISR_MASK0_SHIFT           (0)

// reg_bc
#define CMU_MCU2WF_INTISR_MASK1(n)              (((n) & 0xFFFF) << 0)
#define CMU_MCU2WF_INTISR_MASK1_MASK            (0xFFFF << 0)
#define CMU_MCU2WF_INTISR_MASK1_SHIFT           (0)
#define CMU_WF2MCU_INTR_MASK                    (1 << 16)

// reg_c0
#define CMU_MEMSC0                              (1 << 0)

// reg_c4
#define CMU_MEMSC1                              (1 << 0)

// reg_c8
#define CMU_MEMSC2                              (1 << 0)

// reg_cc
#define CMU_MEMSC3                              (1 << 0)

// reg_d0
#define CMU_MEMSC_STATUS0                       (1 << 0)
#define CMU_MEMSC_STATUS1                       (1 << 1)
#define CMU_MEMSC_STATUS2                       (1 << 2)
#define CMU_MEMSC_STATUS3                       (1 << 3)

// reg_f0
#define CMU_RESERVED(n)                         (((n) & 0xFFFFFFFF) << 0)
#define CMU_RESERVED_MASK                       (0xFFFFFFFF << 0)
#define CMU_RESERVED_SHIFT                      (0)

// reg_f4
#define CMU_DEBUG(n)                            (((n) & 0xFFFFFFFF) << 0)
#define CMU_DEBUG_MASK                          (0xFFFFFFFF << 0)
#define CMU_DEBUG_SHIFT                         (0)

// MCU System AHB Clocks:
#define SYS_HCLK_MCU                    (1 << 0)
#define SYS_HRST_MCU                    (1 << 0)
#define SYS_HCLK_ROM                    (1 << 1)
#define SYS_HRST_ROM                    (1 << 1)
#define SYS_HCLK_RAM0A                  (1 << 2)
#define SYS_HRST_RAM0A                  (1 << 2)
#define SYS_HCLK_RAM1                   (1 << 3)
#define SYS_HRST_RAM1                   (1 << 3)
#define SYS_HCLK_AHB0                   (1 << 4)
#define SYS_HRST_AHB0                   (1 << 4)
#define SYS_HCLK_AHB1                   (1 << 5)
#define SYS_HRST_AHB1                   (1 << 5)
#define SYS_HCLK_AH2H_BT                (1 << 6)
#define SYS_HRST_AH2H_BT                (1 << 6)
#define SYS_HCLK_AH2H_WF                (1 << 7)
#define SYS_HRST_AH2H_WF                (1 << 7)
#define SYS_HCLK_ADMA                   (1 << 8)
#define SYS_HRST_ADMA                   (1 << 8)
#define SYS_HCLK_GDMA                   (1 << 9)
#define SYS_HRST_GDMA                   (1 << 9)
#define SYS_HCLK_EXTMEM                 (1 << 10)
#define SYS_HRST_EXTMEM                 (1 << 10)
#define SYS_HCLK_PSRAM                  (1 << 11)
#define SYS_HRST_PSRAM                  (1 << 11)
#define SYS_HCLK_FLASH                  (1 << 12)
#define SYS_HRST_FLASH                  (1 << 12)
#define SYS_HCLK_SDIO                   (1 << 13)
#define SYS_HRST_SDIO                   (1 << 13)
#define SYS_HCLK_SDMMC                  (1 << 14)
#define SYS_HRST_SDMMC                  (1 << 14)
#define SYS_HCLK_USBC                   (1 << 15)
#define SYS_HRST_USBC                   (1 << 15)
#define SYS_HCLK_CAM                    (1 << 16)
#define SYS_HRST_CAM                    (1 << 16)
#define SYS_HCLK_CODEC                  (1 << 17)
#define SYS_HRST_CODEC                  (1 << 17)
#define SYS_HCLK_FFT                    (1 << 18)
#define SYS_HRST_FFT                    (1 << 18)
#define SYS_HCLK_DSDIF                  (1 << 19)
#define SYS_HRST_DSDIF                  (1 << 19)
#define SYS_HCLK_I2C_SLAVE              (1 << 20)
#define SYS_HRST_I2C_SLAVE              (1 << 20)
#define SYS_HCLK_RAM2                   (1 << 21)
#define SYS_HRST_RAM2                   (1 << 21)
#define SYS_HCLK_RAM3                   (1 << 22)
#define SYS_HRST_RAM3                   (1 << 22)
#define SYS_HCLK_ROM1                   (1 << 23)
#define SYS_HRST_ROM1                   (1 << 23)
#define SYS_HCLK_RAM0B                  (1 << 24)
#define SYS_HRST_RAM0B                  (1 << 24)
#define SYS_HCLK_USBH                   (1 << 25)
#define SYS_HRST_USBH                   (1 << 25)

// MCU System APB Clocks:
#define SYS_PCLK_CMU                    (1 << 0)
#define SYS_PRST_CMU                    (1 << 0)
#define SYS_PCLK_GPIO                   (1 << 1)
#define SYS_PRST_GPIO                   (1 << 1)
#define SYS_PCLK_GPIO_INT               (1 << 2)
#define SYS_PRST_GPIO_INT               (1 << 2)
#define SYS_PCLK_WDT                    (1 << 3)
#define SYS_PRST_WDT                    (1 << 3)
#define SYS_PCLK_PWM                    (1 << 4)
#define SYS_PRST_PWM                    (1 << 4)
#define SYS_PCLK_TIMER                  (1 << 5)
#define SYS_PRST_TIMER                  (1 << 5)
#define SYS_PCLK_I2C                    (1 << 6)
#define SYS_PRST_I2C                    (1 << 6)
#define SYS_PCLK_SPI                    (1 << 7)
#define SYS_PRST_SPI                    (1 << 7)
#define SYS_PCLK_SLCD                   (1 << 8)
#define SYS_PRST_SLCD                   (1 << 8)
#define SYS_PCLK_SPI_ITN                (1 << 9)
#define SYS_PRST_SPI_ITN                (1 << 9)
#define SYS_PCLK_UART0                  (1 << 10)
#define SYS_PRST_UART0                  (1 << 10)
#define SYS_PCLK_UART1                  (1 << 11)
#define SYS_PRST_UART1                  (1 << 11)
#define SYS_PCLK_UART2                  (1 << 12)
#define SYS_PRST_UART2                  (1 << 12)
#define SYS_PCLK_PCM                    (1 << 13)
#define SYS_PRST_PCM                    (1 << 13)
#define SYS_PCLK_I2S                    (1 << 14)
#define SYS_PRST_I2S                    (1 << 14)
#define SYS_PCLK_SPDIF0                 (1 << 15)
#define SYS_PRST_SPDIF0                 (1 << 15)
#define SYS_PCLK_SPDIF1                 (1 << 16)
#define SYS_PRST_SPDIF1                 (1 << 16)
#define SYS_PCLK_TQWF                   (1 << 17)
#define SYS_PRST_TQWF                   (1 << 17)
#define SYS_PCLK_SPI_PHY                (1 << 18)
#define SYS_PRST_SPI_PHY                (1 << 18)

// MCU System Other Clocks:
#define SYS_OCLK_SLEEP                  (1 << 0)
#define SYS_ORST_SLEEP                  (1 << 0)
#define SYS_OCLK_FLASH                  (1 << 1)
#define SYS_ORST_FLASH                  (1 << 1)
#define SYS_OCLK_PSRAM                  (1 << 2)
#define SYS_ORST_PSRAM                  (1 << 2)
#define SYS_OCLK_USB                    (1 << 3)
#define SYS_ORST_USB                    (1 << 3)
#define SYS_OCLK_SDMMC                  (1 << 4)
#define SYS_ORST_SDMMC                  (1 << 4)
#define SYS_OCLK_SDIO                   (1 << 5)
#define SYS_ORST_SDIO                   (1 << 5)
#define SYS_OCLK_WDT                    (1 << 6)
#define SYS_ORST_WDT                    (1 << 6)
#define SYS_OCLK_TIMER                  (1 << 7)
#define SYS_ORST_TIMER                  (1 << 7)
#define SYS_OCLK_I2C                    (1 << 8)
#define SYS_ORST_I2C                    (1 << 8)
#define SYS_OCLK_SPI                    (1 << 9)
#define SYS_ORST_SPI                    (1 << 9)
#define SYS_OCLK_SLCD                   (1 << 10)
#define SYS_ORST_SLCD                   (1 << 10)
#define SYS_OCLK_UART0                  (1 << 11)
#define SYS_ORST_UART0                  (1 << 11)
#define SYS_OCLK_UART1                  (1 << 12)
#define SYS_ORST_UART1                  (1 << 12)
#define SYS_OCLK_UART2                  (1 << 13)
#define SYS_ORST_UART2                  (1 << 13)
#define SYS_OCLK_I2S                    (1 << 14)
#define SYS_ORST_I2S                    (1 << 14)
#define SYS_OCLK_SPDIF0                 (1 << 15)
#define SYS_ORST_SPDIF0                 (1 << 15)
#define SYS_OCLK_SPDIF1                 (1 << 16)
#define SYS_ORST_SPDIF1                 (1 << 16)
#define SYS_OCLK_PCM                    (1 << 17)
#define SYS_ORST_PCM                    (1 << 17)
#define SYS_OCLK_SPI_ITN                (1 << 18)
#define SYS_ORST_SPI_ITN                (1 << 18)
#define SYS_OCLK_GPIO                   (1 << 19)
#define SYS_ORST_GPIO                   (1 << 19)
#define SYS_OCLK_PWM0                   (1 << 20)
#define SYS_ORST_PWM0                   (1 << 20)
#define SYS_OCLK_PWM1                   (1 << 21)
#define SYS_ORST_PWM1                   (1 << 21)
#define SYS_OCLK_PWM2                   (1 << 22)
#define SYS_ORST_PWM2                   (1 << 22)
#define SYS_OCLK_PWM3                   (1 << 23)
#define SYS_ORST_PWM3                   (1 << 23)
#define SYS_OCLK_CAMP                   (1 << 24)
#define SYS_ORST_CAMP                   (1 << 24)
#define SYS_OCLK_CAMC                   (1 << 25)
#define SYS_ORST_CAMC                   (1 << 25)
#define SYS_OCLK_USB32K                 (1 << 26)
#define SYS_ORST_USB32K                 (1 << 26)
#define SYS_OCLK_SPI_PHY                (1 << 27)
#define SYS_ORST_SPI_PHY                (1 << 27)

#endif

