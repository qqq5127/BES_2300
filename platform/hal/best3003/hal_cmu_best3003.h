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
#ifndef __HAL_CMU_BEST3003_H__
#define __HAL_CMU_BEST3003_H__

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_CMU_DEFAULT_CRYSTAL_FREQ        24000000

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_MCU = 0,      // 0
    HAL_CMU_MOD_H_ROM,          // 1
    HAL_CMU_MOD_H_RAM0,         // 2
    HAL_CMU_MOD_H_RAM1,         // 3
    HAL_CMU_MOD_H_AHB0,         // 4
    HAL_CMU_MOD_H_AHB1,         // 5
    HAL_CMU_MOD_H_GDMA,         // 6
    HAL_CMU_MOD_H_EXTMEM,       // 7
    HAL_CMU_MOD_H_FLASH,        // 8
    HAL_CMU_MOD_H_USBC,         // 9
    HAL_CMU_MOD_H_CODEC,        // 10
    HAL_CMU_MOD_H_I2C_SLAVE,    // 11
    HAL_CMU_MOD_H_USBH,         // 12
    HAL_CMU_MOD_H_FFT,          // 13
    HAL_CMU_MOD_H_RAM2,         // 14
    HAL_CMU_MOD_H_RAM3,         // 15
    HAL_CMU_MOD_H_RAM4,         // 16
    HAL_CMU_MOD_H_RAM5,         // 17
    HAL_CMU_MOD_H_RAM6,         // 18
    HAL_CMU_MOD_H_FMDUMP0,      // 19
    HAL_CMU_MOD_H_FMDUMP1,      // 20
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 21
    HAL_CMU_MOD_P_GPIO,         // 22
    HAL_CMU_MOD_P_GPIO_INT,     // 23
    HAL_CMU_MOD_P_WDT,          // 24
    HAL_CMU_MOD_P_PWM,          // 25
    HAL_CMU_MOD_P_TIMER0,       // 26
    HAL_CMU_MOD_P_TIMER1,       // 27
    HAL_CMU_MOD_P_I2C0,         // 28
    HAL_CMU_MOD_P_SPI,          // 29
    HAL_CMU_MOD_P_SPI_ITN,      // 30
    HAL_CMU_MOD_P_UART0,        // 31
    HAL_CMU_MOD_P_UART1,        // 32
    HAL_CMU_MOD_P_I2S0,         // 33
    HAL_CMU_MOD_P_SPDIF0,       // 34
    HAL_CMU_MOD_P_IOMUX,        // 35
    HAL_CMU_MOD_P_FM,           // 36
    HAL_CMU_MOD_P_GLOBAL,       // 37
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 38
    HAL_CMU_MOD_O_FLASH,        // 39
    HAL_CMU_MOD_O_USB,          // 40
    HAL_CMU_MOD_O_GPIO,         // 41
    HAL_CMU_MOD_O_WDT,          // 42
    HAL_CMU_MOD_O_TIMER0,       // 43
    HAL_CMU_MOD_O_TIMER1,       // 44
    HAL_CMU_MOD_O_I2C0,         // 45
    HAL_CMU_MOD_O_SPI,          // 46
    HAL_CMU_MOD_O_SPI_ITN,      // 47
    HAL_CMU_MOD_O_UART0,        // 48
    HAL_CMU_MOD_O_UART1,        // 49
    HAL_CMU_MOD_O_I2S0,         // 50
    HAL_CMU_MOD_O_SPDIF0,       // 51
    HAL_CMU_MOD_O_USB32K,       // 52
    HAL_CMU_MOD_O_IOMUX,        // 53
    HAL_CMU_MOD_O_PWM0,         // 54
    HAL_CMU_MOD_O_PWM1,         // 55
    HAL_CMU_MOD_O_PWM2,         // 56
    HAL_CMU_MOD_O_PWM3,         // 57
    HAL_CMU_MOD_O_FM,           // 58
    HAL_CMU_MOD_O_FMDUMP0,      // 59
    HAL_CMU_MOD_O_FMDUMP1,      // 60

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_MOD_P_GLOBAL,
};

enum HAL_CMU_CLOCK_OUT_ID_T {
    HAL_CMU_CLOCK_OUT_32K,
    HAL_CMU_CLOCK_OUT_OSC,    
    HAL_CMU_CLOCK_OUT_OSCX2,
    HAL_CMU_CLOCK_OUT_OSCX4,
    HAL_CMU_CLOCK_OUT_SYS,
    HAL_CMU_CLOCK_OUT_PCLK,
    HAL_CMU_CLOCK_OUT_FLASH,
    HAL_CMU_CLOCK_OUT_USB,
    HAL_CMU_CLOCK_OUT_I2S,
    HAL_CMU_CLOCK_OUT_SPDIF,
    HAL_CMU_CLOCK_OUT_SPI0,
    HAL_CMU_CLOCK_OUT_SPI1,
    HAL_CMU_CLOCK_OUT_UART0,
    HAL_CMU_CLOCK_OUT_I2C,
    HAL_CMU_CLOCK_OUT_CODEC,
    HAL_CMU_CLOCK_OUT_CODEC_IIR,
    HAL_CMU_CLOCK_OUT_CODEC_FIR,
    HAL_CMU_CLOCK_OUT_CODEC_RESAMPLER,
    HAL_CMU_CLOCK_OUT_CODEC_ADC,
    HAL_CMU_CLOCK_OUT_CODEC_SARADC,
    HAL_CMU_CLOCK_OUT_PWM0,
    HAL_CMU_CLOCK_OUT_I2S_MCLK,
    HAL_CMU_CLOCK_OUT_FM_60M,
    HAL_CMU_CLOCK_OUT_FM_30M,
    HAL_CMU_CLOCK_OUT_FM_15M,
    HAL_CMU_CLOCK_OUT_FM_468K,
    HAL_CMU_CLOCK_OUT_FM_156K,
    HAL_CMU_CLOCK_OUT_FM_52K,
    HAL_CMU_CLOCK_OUT_DCDC,
    HAL_CMU_CLOCK_OUT_PER,

    HAL_CMU_CLOCK_OUT_QTY,
};

enum HAL_CMU_I2S_MCLK_ID_T {
    HAL_CMU_I2S_MCLK_QTY,
};

enum HAL_CMU_ANC_CLK_USER_T {
    HAL_CMU_ANC_CLK_USER_ANC,

    HAL_CMU_ANC_CLK_USER_QTY
};

void hal_cmu_anc_enable(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_anc_disable(enum HAL_CMU_ANC_CLK_USER_T user);

int hal_cmu_anc_get_status(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_codec_adda_clock_enable(uint32_t val);

void hal_cmu_codec_adda_clock_disable(uint32_t val);

void hal_cmu_codec_adda_reset_set(uint32_t val);

void hal_cmu_codec_adda_reset_clear(uint32_t val);

void hal_cmu_codec_adda_reset_pulse(uint32_t val);

#ifdef __cplusplus
}
#endif

#endif

