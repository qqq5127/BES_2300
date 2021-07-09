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
#include "reg_cmu_best3001.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_bootmode.h"
#include "hal_chipid.h"
#include "hal_location.h"
#include "hal_psc.h"
#include "hal_sleep_mcu_pd.h"
#include "hal_timer.h"
#include "cmsis_nvic.h"
#include "pmu.h"
#include "hal_trace.h"

#if defined(ANA_26M_X4_ENABLE) && !defined(DIG_OSC_X2_ENABLE)
#define NO_OSC_X2
#endif

#define HAL_CMU_AUD_PLL_CLOCK           (CODEC_FREQ_48K_SERIES * CODEC_CMU_DIV)

#define HAL_CMU_PWM_SLOW_CLOCK          (32 * 1000)

#define HAL_CMU_PLL_LOCKED_TIMEOUT      US_TO_TICKS(200)
#define HAL_CMU_26M_READY_TIMEOUT       MS_TO_TICKS(3)
#define HAL_CMU_LPU_EXTRA_TIMEOUT       MS_TO_TICKS(1)

enum CMU_USB_CLK_SRC_T {
    CMU_USB_CLK_SRC_PLL_60M         = 0,
    CMU_USB_CLK_SRC_6M              = 1,
    CMU_USB_CLK_SRC_PLL_48M         = 2,
    CMU_USB_CLK_SRC_OSC_24M_X2      = 3,
#ifdef CHIP_HAS_DCO
    CMU_USB_CLK_SRC_DCO_60M         = 4,
#endif
};

static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;

static uint8_t BOOT_BSS_LOC pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(pll_user_map[0]) * 8, "Too many PLL users");

#ifdef ROM_BUILD
static enum HAL_CMU_USB_CLOCK_SEL_T usb_clk_sel;
#endif

static bool s_force_usb_pll;

void hal_cmu_force_usb_pll (bool force)
{
    s_force_usb_pll = force;
}

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_ENABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_ENABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->OCLK_ENABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

int hal_cmu_clock_disable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_DISABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_DISABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->OCLK_DISABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

enum HAL_CMU_CLK_STATUS_T hal_cmu_clock_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_CLK_DISABLED;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HCLK_ENABLE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PCLK_ENABLE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        status = cmu->OCLK_ENABLE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return status ? HAL_CMU_CLK_ENABLED : HAL_CMU_CLK_DISABLED;
}

int hal_cmu_clock_set_mode(enum HAL_CMU_MOD_ID_T id, enum HAL_CMU_CLK_MODE_T mode)
{
    __IO uint32_t *reg;
    uint32_t val;
    uint32_t lock;

    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        reg = &cmu->HCLK_MODE;
        val = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        reg = &cmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        reg = &cmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    lock = int_lock();
    if (mode == HAL_CMU_CLK_MANUAL) {
        *reg |= val;
    } else {
        *reg &= ~val;
    }
    int_unlock(lock);

    return 0;
}

enum HAL_CMU_CLK_MODE_T hal_cmu_clock_get_mode(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t mode;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_CLK_AUTO;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        mode = cmu->HCLK_MODE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        mode = cmu->PCLK_MODE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        mode = cmu->OCLK_MODE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return mode ? HAL_CMU_CLK_MANUAL : HAL_CMU_CLK_AUTO;
}

int hal_cmu_reset_set(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_SET = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_SET = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->ORESET_SET = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

int hal_cmu_reset_clear(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_CLR = (1 << id);
        asm volatile("nop; nop;");
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_CLR = (1 << (id - HAL_CMU_MOD_P_CMU));
        asm volatile("nop; nop; nop; nop;");
    } else {
        cmu->ORESET_CLR = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

enum HAL_CMU_RST_STATUS_T hal_cmu_reset_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_RST_SET;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HRESET_SET & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PRESET_SET & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        status = cmu->ORESET_SET & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return status ? HAL_CMU_RST_CLR : HAL_CMU_RST_SET;
}

int hal_cmu_reset_pulse(enum HAL_CMU_MOD_ID_T id)
{
    volatile int i;

    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (hal_cmu_reset_get_status(id) == HAL_CMU_RST_SET) {
        return hal_cmu_reset_clear(id);
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_PULSE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_PULSE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->ORESET_PULSE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }
    // Delay 5+ PCLK cycles (10+ HCLK cycles)
    for (i = 0; i < 3; i++);

    return 0;
}

void hal_cmu_timer0_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->UART_CLK |= (1 << CMU_SEL_TIMER_FAST_SHIFT);
    int_unlock(lock);
}

void hal_cmu_timer0_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->UART_CLK &= ~(1 << CMU_SEL_TIMER_FAST_SHIFT);
    int_unlock(lock);
}

void hal_cmu_timer1_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->UART_CLK |= (1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void hal_cmu_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->UART_CLK &= ~(1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

#ifdef NO_OSC_X2
#define SYS_SET_52M_FREQ(F, d) \
    enable = CMU_SEL_PLL_ ##F## _ENABLE | CMU_RSTN_DIV_ ##F## _DISABLE; \
    disable = CMU_BYPASS_DIV_ ##F## _ENABLE; \
    div = d;
#else
#define SYS_SET_52M_FREQ(F, d) \
    enable = CMU_SEL_OSCX2_ ##F## _ENABLE; \
    disable = CMU_SEL_PLL_ ##F## _DISABLE | CMU_RSTN_DIV_ ##F## _DISABLE | CMU_BYPASS_DIV_ ##F## _DISABLE;
#endif

#ifdef OSC_26M_X4_AUD2BB

// Any of 78M/104M/208M is changed to 26M x4 (104M)

#define SYS_SET_FREQ_FUNC(f, F) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_FREQ_T freq) \
{ \
    uint32_t lock; \
    uint32_t enable; \
    uint32_t disable; \
    int div = -1; \
    if (freq >= HAL_CMU_FREQ_QTY) { \
        return 1; \
    } \
    if (freq == HAL_CMU_FREQ_32K) { \
        enable = 0; \
        disable = CMU_SEL_OSC_ ##F## _DISABLE | CMU_SEL_OSCX2_ ##F## _DISABLE | \
            CMU_SEL_PLL_ ##F## _DISABLE | CMU_RSTN_DIV_ ##F## _DISABLE | CMU_BYPASS_DIV_ ##F## _DISABLE; \
    } else if (freq == HAL_CMU_FREQ_26M) { \
        enable = CMU_SEL_OSC_ ##F## _ENABLE; \
        disable = CMU_SEL_OSCX2_ ##F## _DISABLE | \
            CMU_SEL_PLL_ ##F## _DISABLE | CMU_RSTN_DIV_ ##F## _DISABLE | CMU_BYPASS_DIV_ ##F## _DISABLE; \
    } else if (freq == HAL_CMU_FREQ_52M) { \
        SYS_SET_52M_FREQ(F, 0); \
    } else { \
        enable = CMU_SEL_PLL_ ##F## _ENABLE | CMU_BYPASS_DIV_ ##F## _ENABLE; \
        disable = CMU_RSTN_DIV_ ##F## _DISABLE; \
    } \
    if (div >= 0) { \
        lock = int_lock(); \
        cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_ ##F, div); \
        int_unlock(lock); \
    } \
    if (enable & CMU_SEL_PLL_ ##F## _ENABLE) { \
        cmu->SYS_CLK_ENABLE = CMU_RSTN_DIV_ ##F## _ENABLE; \
        if (enable & CMU_BYPASS_DIV_ ##F## _ENABLE) { \
            cmu->SYS_CLK_ENABLE = CMU_BYPASS_DIV_ ##F## _ENABLE; \
        } else { \
            cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_ ##F## _DISABLE; \
        } \
    } \
    cmu->SYS_CLK_ENABLE = enable; \
    if (enable & CMU_SEL_PLL_ ##F## _ENABLE) { \
        cmu->SYS_CLK_DISABLE = disable; \
    } else { \
        cmu->SYS_CLK_DISABLE = disable & ~(CMU_RSTN_DIV_ ##F## _DISABLE | CMU_BYPASS_DIV_ ##F## _DISABLE); \
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_ ##F## _DISABLE; \
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_ ##F## _DISABLE; \
    } \
    return 0; \
}

#else // !OSC_26M_X4_AUD2BB

#define SYS_SET_FREQ_FUNC(f, F) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_FREQ_T freq) \
{ \
    uint32_t lock; \
    uint32_t enable; \
    uint32_t disable; \
    int div = -1; \
    if (freq >= HAL_CMU_FREQ_QTY) { \
        return 1; \
    } \
    if (freq == HAL_CMU_FREQ_32K) { \
        enable = 0; \
        disable = CMU_SEL_OSC_ ##F## _DISABLE | CMU_SEL_OSCX2_ ##F## _DISABLE | \
            CMU_SEL_PLL_ ##F## _DISABLE | CMU_RSTN_DIV_ ##F## _DISABLE | CMU_BYPASS_DIV_ ##F## _DISABLE; \
    } else if (freq == HAL_CMU_FREQ_26M) { \
        enable = CMU_SEL_OSC_ ##F## _ENABLE; \
        disable = CMU_SEL_OSCX2_ ##F## _DISABLE | \
            CMU_SEL_PLL_ ##F## _DISABLE | CMU_RSTN_DIV_ ##F## _DISABLE | CMU_BYPASS_DIV_ ##F## _DISABLE; \
    } else if (freq == HAL_CMU_FREQ_52M) { \
        SYS_SET_52M_FREQ(F, 2); \
    } else if (freq == HAL_CMU_FREQ_78M) { \
        enable = CMU_SEL_PLL_ ##F## _ENABLE | CMU_RSTN_DIV_ ##F## _ENABLE; \
        disable = CMU_BYPASS_DIV_ ##F## _DISABLE; \
        div = 1; \
    } else if (freq == HAL_CMU_FREQ_104M) { \
        enable = CMU_SEL_PLL_ ##F## _ENABLE | CMU_RSTN_DIV_ ##F## _ENABLE; \
        disable = CMU_BYPASS_DIV_ ##F## _DISABLE; \
        div = 0; \
    } else { \
        enable = CMU_SEL_PLL_ ##F## _ENABLE | CMU_BYPASS_DIV_ ##F## _ENABLE; \
        disable = CMU_RSTN_DIV_ ##F## _DISABLE; \
    } \
    if (div >= 0) { \
        lock = int_lock(); \
        cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_ ##F, div); \
        int_unlock(lock); \
    } \
    if (enable & CMU_SEL_PLL_ ##F## _ENABLE) { \
        cmu->SYS_CLK_ENABLE = CMU_RSTN_DIV_ ##F## _ENABLE; \
        if (enable & CMU_BYPASS_DIV_ ##F## _ENABLE) { \
            cmu->SYS_CLK_ENABLE = CMU_BYPASS_DIV_ ##F## _ENABLE; \
        } else { \
            cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_ ##F## _DISABLE; \
        } \
    } \
    cmu->SYS_CLK_ENABLE = enable; \
    if (enable & CMU_SEL_PLL_ ##F## _ENABLE) { \
        cmu->SYS_CLK_DISABLE = disable; \
    } else { \
        cmu->SYS_CLK_DISABLE = disable & ~(CMU_RSTN_DIV_ ##F## _DISABLE | CMU_BYPASS_DIV_ ##F## _DISABLE); \
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_ ##F## _DISABLE; \
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_ ##F## _DISABLE; \
    } \
    return 0; \
}

#endif // !OSC_26M_X4_AUD2BB

BOOT_TEXT_SRAM_LOC SYS_SET_FREQ_FUNC(flash, FLS);
SYS_SET_FREQ_FUNC(sys, SYS);

int hal_cmu_mem_set_freq(enum HAL_CMU_FREQ_T freq)
{
    return 0;
}

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_sys_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = cmu->SYS_CLK_ENABLE;

    if (sys_clk & CMU_SEL_PLL_SYS_ENABLE) {
        if (sys_clk & CMU_BYPASS_DIV_SYS_ENABLE) {
            return HAL_CMU_FREQ_208M;
        } else {
            div = GET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_SYS);
            if (div == 0) {
                return HAL_CMU_FREQ_104M;
            } else if (div == 1) {
                // (div == 1): 69M
                return HAL_CMU_FREQ_78M;
            } else {
                // (div == 2): 52M
                // (div == 3): 42M
                return HAL_CMU_FREQ_52M;
            }
        }
    } else if (sys_clk & CMU_SEL_OSCX2_SYS_ENABLE) {
        return HAL_CMU_FREQ_52M;
    } else if (sys_clk & CMU_SEL_OSC_SYS_ENABLE) {
        return HAL_CMU_FREQ_26M;
    } else {
        return HAL_CMU_FREQ_32K;
    }
}

int BOOT_TEXT_SRAM_LOC hal_cmu_flash_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_mem_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_sys_select_pll(enum HAL_CMU_PLL_T pll)
{
    return 0;
}

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll)
{
    return !!(cmu->SYS_CLK_ENABLE & CMU_EN_PLLUSB_ENABLE);
}

int hal_cmu_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t lock;
    uint32_t start;
    uint32_t timeout;
    uint32_t check = 0;
    uint32_t map = 0;
    int i;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    lock = int_lock();
    for (i = 0; i < HAL_CMU_PLL_QTY; i++) {
        map |= pll_user_map[i];
    }
    if (map == 0) {
        cmu->SYS_CLK_ENABLE = CMU_PU_PLLUSB_ENABLE;
        check = CMU_LOCK_PLLUSB;
    }
    pll_user_map[pll] |= (1 << user);
    int_unlock(lock);

    start = hal_sys_timer_get();
    timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
    do {
        if (check) {
            if (cmu->I2C_CLK & check) {
                //break;
            }
        } else {
            if (cmu->SYS_CLK_ENABLE & CMU_EN_PLLUSB_ENABLE) {
                break;
            }
        }
    } while ((hal_sys_timer_get() - start) < timeout);

    cmu->SYS_CLK_ENABLE = CMU_EN_PLLUSB_ENABLE;

    return (cmu->CLK_OUT & check) ? 0 : 2;
}

int hal_cmu_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t lock;
    uint32_t map = 0;
    int i;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    lock = int_lock();
    pll_user_map[pll] &= ~(1 << user);
    for (i = 0; i < HAL_CMU_PLL_QTY; i++) {
        map |= pll_user_map[i];
    }
    if (map == 0) {
        cmu->SYS_CLK_DISABLE = CMU_EN_PLLUSB_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_PU_PLLUSB_DISABLE;
    }
    int_unlock(lock);

    return 0;
}

int hal_cmu_codec_adc_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    cmu->CODEC_CLK = SET_BITFIELD(cmu->CODEC_CLK, CMU_CFG_DIV_CODECRS, div);
    int_unlock(lock);

    return 0;
}

uint32_t hal_cmu_codec_adc_get_div(void)
{
    return GET_BITFIELD(cmu->CODEC_CLK, CMU_CFG_DIV_CODECRS) + 2;
}

int hal_cmu_codec_dac_set_div(uint32_t div)
{
    return hal_cmu_codec_adc_set_div(div);
}

uint32_t hal_cmu_codec_dac_get_div(void)
{
    return hal_cmu_codec_adc_get_div();;
}

void hal_cmu_codec_iir_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;

    mask = CMU_SEL_OSC_CODECIIR | CMU_SEL_OSCX2_CODECIIR | CMU_BYPASS_DIV_CODECIIR;
    val = 0;

#ifdef __AUDIO_RESAMPLE__
    int resample_en;

    resample_en = hal_cmu_get_audio_resample_status();
#ifndef ANA_26M_X4_ENABLE
    if (resample_en && speed > 52000000) {
        speed = 52000000;
    }
#endif
#endif

    hal_cmu_clock_enable(HAL_CMU_MOD_O_CODECIIR);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_CODECIIR);

    if (speed <= 26000000) {
        val |= CMU_SEL_OSC_CODECIIR | CMU_SEL_OSCX2_CODECIIR;
    } else if (speed <= 52000000) {
        val |= CMU_SEL_OSCX2_CODECIIR;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (resample_en) {
            lock = int_lock();
            cmu->CLK_SELECT |= CMU_SEL_X4_AUD;
            int_unlock(lock);

            val |= CMU_BYPASS_DIV_CODECIIR;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = HAL_CMU_AUD_PLL_CLOCK / speed;
            if (div >= 2) {
                hal_cmu_codec_iir_set_div(div);
            } else {
                val |= CMU_BYPASS_DIV_CODECIIR;
            }
        }
    }

    lock = int_lock();
    cmu->CODEC_CLK = (cmu->CODEC_CLK & ~mask) | val;
    int_unlock(lock);
}

void hal_cmu_codec_iir_disable(void)
{
    uint32_t lock;

    hal_cmu_reset_set(HAL_CMU_MOD_O_CODECIIR);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_CODECIIR);

    lock = int_lock();
    cmu->CODEC_CLK |= CMU_SEL_OSC_CODECIIR | CMU_SEL_OSCX2_CODECIIR;
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
    cmu->CLK_SELECT &= ~CMU_SEL_X4_AUD;
#endif
    int_unlock(lock);
}

int hal_cmu_codec_iir_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    cmu->CODEC_CLK = SET_BITFIELD(cmu->CODEC_CLK, CMU_CFG_DIV_CODECIIR, div);
    int_unlock(lock);

    return 0;
}

#ifdef __1801_SUPPT__

static uint8_t s_1801_clk_usr_map;
enum {
    ANA1801_USR_DAC,
    ANA1801_USR_EXT_HS
};

void hal_cmu_1801_clock_enable (void)
{
    if (!s_1801_clk_usr_map) {
        cmu->SYS_CLK_ENABLE = CMU_EN_CLK_DCDC1_ENABLE;
    }

    if ((s_1801_clk_usr_map & ANA1801_USR_EXT_HS) != ANA1801_USR_EXT_HS) {
        TRACE(1,"==> %s", __func__);
        s_1801_clk_usr_map |= (1<<ANA1801_USR_EXT_HS);
    }
}

void hal_cmu_1801_clock_disable (void)
{
    if ((s_1801_clk_usr_map & ANA1801_USR_EXT_HS) == ANA1801_USR_EXT_HS) {
        TRACE(1,"==> %s", __func__);
        s_1801_clk_usr_map &= ~(1<<ANA1801_USR_EXT_HS);
    }

    if (!s_1801_clk_usr_map) {
        cmu->SYS_CLK_DISABLE = CMU_EN_CLK_DCDC1_DISABLE;
    }
}

#endif

void hal_cmu_codec_clock_enable(void)
{
#ifdef __AUDIO_RESAMPLE__
    if (hal_cmu_get_audio_resample_status()) {
        uint32_t lock;
        lock = int_lock();
        cmu->CODEC_CLK |= CMU_SEL_OSC_CODECRS | CMU_SEL_OSCX2_CODECRS;
        int_unlock(lock);
    }
    else
#endif
    {
    }
    hal_cmu_clock_enable(HAL_CMU_MOD_H_CODEC);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_CODEC);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_CODECRS);

    cmu->I2C_CLK |= CMU_POL_CLK_DAC_OUT;

#ifdef __1801_SUPPT__
#ifndef AUDIO_OUTPUT_DIFF
    if (!s_1801_clk_usr_map) {
        cmu->SYS_CLK_ENABLE = CMU_EN_CLK_DCDC1_ENABLE;
    }
    s_1801_clk_usr_map |= (1<<ANA1801_USR_DAC);
#endif
#endif
}

void hal_cmu_codec_clock_disable(void)
{
#ifdef __1801_SUPPT__
#ifndef AUDIO_OUTPUT_DIFF
    s_1801_clk_usr_map &= ~(1<<ANA1801_USR_DAC);
    if (!s_1801_clk_usr_map) {
        cmu->SYS_CLK_DISABLE = CMU_EN_CLK_DCDC1_DISABLE;
    }
#endif
#endif

    hal_cmu_clock_disable(HAL_CMU_MOD_O_CODECRS);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_CODEC);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_CODEC);
#ifdef __AUDIO_RESAMPLE__
    if (hal_cmu_get_audio_resample_status()) {
        uint32_t lock;
        lock = int_lock();
        cmu->CODEC_CLK |= CMU_SEL_OSC_CODECRS | CMU_SEL_OSCX2_CODECRS;
        int_unlock(lock);
    }
    else
#endif
    {
    }
}

void hal_cmu_codec_reset_set(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_O_CODECRS);
    hal_cmu_reset_set(HAL_CMU_MOD_O_CODEC);
    hal_cmu_reset_set(HAL_CMU_MOD_H_CODEC);
}

void hal_cmu_codec_reset_clear(void)
{
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CODEC);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_CODEC);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_CODECRS);
}

void hal_cmu_i2s_clock_out_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK |= CMU_EN_CLK_I2S_OUT;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK &= ~CMU_EN_CLK_I2S_OUT;
    int_unlock(lock);
}

void hal_cmu_i2s_set_slave_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK |= CMU_SEL_I2S_CLKIN;
    int_unlock(lock);
}

void hal_cmu_i2s_set_master_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK &= ~CMU_SEL_I2S_CLKIN;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2S_SPDIF_CLK |= CMU_EN_CLK_PLL_I2S;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2S_SPDIF_CLK &= ~CMU_EN_CLK_PLL_I2S;
    int_unlock(lock);
}

int hal_cmu_i2s_set_div(enum HAL_I2S_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (CMU_CFG_DIV_I2S_MASK >> CMU_CFG_DIV_I2S_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    cmu->I2S_SPDIF_CLK = SET_BITFIELD(cmu->I2S_SPDIF_CLK, CMU_CFG_DIV_I2S, div);
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_enable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    lock = int_lock();
    cmu->I2S_SPDIF_CLK |= CMU_EN_CLK_PLL_SPDIF;
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_disable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    lock = int_lock();
    cmu->I2S_SPDIF_CLK &= ~CMU_EN_CLK_PLL_SPDIF;
    int_unlock(lock);

    return 0;
}

int hal_cmu_spdif_set_div(enum HAL_SPDIF_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    if (div < 2) {
        return 2;
    }

    div -= 2;
    if ((div & (CMU_CFG_DIV_SPDIF_MASK >> CMU_CFG_DIV_SPDIF_SHIFT)) != div) {
        return 2;
    }

    lock = int_lock();
    cmu->I2S_SPDIF_CLK = SET_BITFIELD(cmu->I2S_SPDIF_CLK, CMU_CFG_DIV_SPDIF, div);
    int_unlock(lock);
    return 0;
}

#ifdef CHIP_HAS_USB
void hal_cmu_usb_set_device_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->SYS_DIV |= CMU_USB_ID;
    int_unlock(lock);
}

void hal_cmu_usb_set_host_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->SYS_DIV &= ~CMU_USB_ID;
    int_unlock(lock);
}

#ifdef ROM_BUILD
void hal_cmu_rom_select_usb_clock(enum HAL_CMU_USB_CLOCK_SEL_T sel)
{
    usb_clk_sel = sel;
}
#endif

static uint32_t hal_cmu_usb_get_clock_source(void)
{
    uint32_t src;

#ifdef ROM_BUILD

#ifdef USB_HIGH_SPEED
    src = CMU_USB_CLK_SRC_PLL_60M;
#else
    if (usb_clk_sel == HAL_CMU_USB_CLOCK_SEL_24M_X2) {
        src = CMU_USB_CLK_SRC_OSC_24M_X2;
#ifdef NO_OSC_X2
        ASSERT(false, "No OSC X2 for usb clk");
#endif
    } else {
        src = CMU_USB_CLK_SRC_PLL_48M;
    }
#endif

#else // !ROM_BUILD

#ifdef CHIP_HAS_DCO
#ifdef USB_HIGH_SPEED
    src = CMU_USB_CLK_SRC_DCO_60M;
#else
    src = CMU_USB_CLK_SRC_OSC_24M_X2;
#ifdef NO_OSC_X2
    ASSERT(false, "No OSC X2 for usb clk");
#endif
#endif
#else
#ifdef USB_HIGH_SPEED
    src = CMU_USB_CLK_SRC_PLL_60M;
#else
    src = CMU_USB_CLK_SRC_OSC_24M_X2;
#ifdef NO_OSC_X2
    ASSERT(false, "No OSC X2 for usb clk");
#endif
#endif
#endif

#endif // !ROM_BUILD

    return src;
}

void hal_cmu_usb_clock_enable(void)
{
    uint32_t lock;
    uint32_t src;

    src = hal_cmu_usb_get_clock_source();

    if (src == CMU_USB_CLK_SRC_PLL_60M || src == CMU_USB_CLK_SRC_PLL_48M) {
        hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_USB);
        if (src == CMU_USB_CLK_SRC_PLL_48M) {
            cmu->SYS_CLK_ENABLE = CMU_EN_CLK_USB48M_ENABLE;
        }
    }

    lock = int_lock();
    cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_SEL_USB_SRC, src);
    int_unlock(lock);

    hal_cmu_clock_enable(HAL_CMU_MOD_H_USBC);
#ifdef USB_HIGH_SPEED
    hal_cmu_clock_enable(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_clock_enable(HAL_CMU_MOD_O_USB32K);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_USB);

    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_sys_timer_delay(US_TO_TICKS(60));
    hal_cmu_reset_clear(HAL_CMU_MOD_H_USBC);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_clear(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_clear(HAL_CMU_MOD_O_USB32K);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_USB);
}

void hal_cmu_usb_clock_disable(void)
{
    uint32_t src;

    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_USB);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_USB32K);
#ifdef USB_HIGH_SPEED
    hal_cmu_clock_disable(HAL_CMU_MOD_H_USBH);
#endif
    hal_cmu_clock_disable(HAL_CMU_MOD_H_USBC);

    src = hal_cmu_usb_get_clock_source();
    if (src == CMU_USB_CLK_SRC_PLL_60M || src == CMU_USB_CLK_SRC_PLL_48M) {
        if (src == CMU_USB_CLK_SRC_PLL_48M) {
            cmu->SYS_CLK_DISABLE = CMU_EN_CLK_USB48M_DISABLE;
        }
        hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_USB);
    }
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_apb_init_div(void)
{
}

#ifdef NO_OSC_X2
#define PERPH_SET_52M_FREQ(F, r)
#else
#define PERPH_SET_52M_FREQ(F, r) \
    cmu->r = (cmu->r & ~(CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F)) | CMU_SEL_OSCX2_ ##F;
#endif

#define PERPH_SET_FREQ_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (freq == HAL_CMU_PERIPH_FREQ_26M) { \
        cmu->r &= ~(CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F); \
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) { \
        PERPH_SET_52M_FREQ(F, r); \
    } else { \
        ret = 1; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_FREQ_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_FREQ_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_FREQ_FUNC(i2c, I2C, I2C_CLK);
PERPH_SET_FREQ_FUNC(spi, SPI0, SYS_DIV);

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        cmu->SYS_DIV &= ~CMU_SEL_OSCX2_SPI1;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        cmu->SYS_DIV |= CMU_SEL_OSCX2_SPI1;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

int hal_cmu_clock_out_enable(enum HAL_CMU_CLOCK_OUT_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->CLK_OUT = SET_BITFIELD(cmu->CLK_OUT, CMU_CFG_CLK_OUT, id) | CMU_EN_CLK_OUT;
    int_unlock(lock);

    return 0;
}

void hal_cmu_clock_out_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->CLK_OUT &= ~CMU_EN_CLK_OUT;
    int_unlock(lock);
}

int hal_cmu_i2s_mclk_enable(enum HAL_CMU_I2S_MCLK_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2S_SPDIF_CLK = SET_BITFIELD(cmu->I2S_SPDIF_CLK, CMU_SEL_I2S_MCLK, id) | CMU_EN_I2S_MCLK;
    int_unlock(lock);

    return 0;
}

void hal_cmu_i2s_mclk_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2S_SPDIF_CLK &= ~CMU_EN_I2S_MCLK;
    int_unlock(lock);
}

int hal_cmu_pwm_set_freq(enum HAL_PWM_ID_T id, uint32_t freq)
{
    uint32_t lock;
    int clk_32k;
    uint32_t div;

    if (id >= HAL_PWM_ID_QTY) {
        return 1;
    }

    if (freq == HAL_CMU_PWM_SLOW_CLOCK) {
        clk_32k = 1;
        div = 0;
    } else {
        clk_32k = 0;
        div = hal_cmu_get_crystal_freq() / freq;
        if (div < 2) {
            return 1;
        }

        div -= 2;
        if ((div & (CMU_CFG_DIV_PWM0_MASK >> CMU_CFG_DIV_PWM0_SHIFT)) != div) {
            return 1;
        }
    }

    lock = int_lock();
    if (id == HAL_PWM_ID_0) {
        cmu->PWM01_CLK = (cmu->PWM01_CLK & ~(CMU_CFG_DIV_PWM0_MASK | CMU_SEL_OSC_PWM0 | CMU_EN_OSC_PWM0)) |
            CMU_CFG_DIV_PWM0(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM0 | CMU_EN_OSC_PWM0));
    } else if (id == HAL_PWM_ID_1) {
        cmu->PWM01_CLK = (cmu->PWM01_CLK & ~(CMU_CFG_DIV_PWM1_MASK | CMU_SEL_OSC_PWM1 | CMU_EN_OSC_PWM1)) |
            CMU_CFG_DIV_PWM1(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM1 | CMU_EN_OSC_PWM1));
    } else if (id == HAL_PWM_ID_2) {
        cmu->PWM23_CLK = (cmu->PWM23_CLK & ~(CMU_CFG_DIV_PWM2_MASK | CMU_SEL_OSC_PWM2 | CMU_EN_OSC_PWM2)) |
            CMU_CFG_DIV_PWM2(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM2 | CMU_EN_OSC_PWM2));
    } else {
        cmu->PWM23_CLK = (cmu->PWM23_CLK & ~(CMU_CFG_DIV_PWM3_MASK | CMU_SEL_OSC_PWM3 | CMU_EN_OSC_PWM3)) |
            CMU_CFG_DIV_PWM3(div) | (clk_32k ? 0 : (CMU_SEL_OSC_PWM3 | CMU_EN_OSC_PWM3));
    }
    int_unlock(lock);
    return 0;
}

void hal_cmu_jtag_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->MCU_TIMER &= ~CMU_SECURE_BOOT;
    int_unlock(lock);
}

void hal_cmu_jtag_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->MCU_TIMER |= CMU_SECURE_BOOT;
    int_unlock(lock);
}

void hal_cmu_jtag_clock_enable(void)
{
    cmu->SYS_CLK_ENABLE = CMU_EN_CLK_TOP_JTAG_ENABLE;
}

void hal_cmu_jtag_clock_disable(void)
{
    cmu->SYS_CLK_DISABLE = CMU_EN_CLK_TOP_JTAG_DISABLE;
}

void hal_cmu_rom_clock_init(void)
{
    cmu->I2C_CLK |= CMU_BYPASS_LOCK_PLLUSB;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x2_enable(void)
{
#ifndef NO_OSC_X2
    // Enable OSCX2 for the whole digital domain
    cmu->SYS_CLK_ENABLE = CMU_EN_CLK_TOP_OSCX2_ENABLE;
#ifdef DIG_OSC_X2_ENABLE
    cmu->CLK_SELECT |= CMU_SEL_OSCX2_DIG;
    cmu->CLK_SELECT &= ~CMU_SEL_X2_PHASE_MASK;
    cmu->CLK_SELECT |= CMU_SEL_X2_PHASE(0x16);
    cmu->SYS_CLK_ENABLE = CMU_EN_X2_DIG_ENABLE;
#else
    cmu->CLK_SELECT &= ~CMU_SEL_OSCX2_DIG;
#endif
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x4_enable(void)
{
#if defined(ANA_26M_X4_ENABLE) || defined(DIG_OSC_X4_ENABLE)
    cmu->SYS_CLK_ENABLE = CMU_EN_CLK_TOP_OSCX4_ENABLE;
#ifdef DIG_OSC_X4_ENABLE
    cmu->CLK_SELECT |= CMU_SEL_OSCX4_DIG;
    cmu->CLK_SELECT &= ~CMU_SEL_X4_PHASE_MASK;
    cmu->CLK_SELECT |= CMU_SEL_X4_PHASE(0xc);
    cmu->SYS_CLK_ENABLE = CMU_EN_X4_DIG_ENABLE;
#else
    cmu->CLK_SELECT &= ~CMU_SEL_OSCX4_DIG;
#endif
#endif

#ifdef OSC_26M_X4_AUD2BB
    cmu->CLK_SELECT |= CMU_SEL_X4_SYS;
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_module_init_state(void)
{
    cmu->I2C_CLK |= CMU_BYPASS_LOCK_PLLUSB;

#ifndef SIMU
    cmu->ORESET_SET = SYS_ORST_USB | SYS_ORST_I2C | SYS_ORST_SPI | SYS_ORST_UART0 | SYS_ORST_UART1 |
        SYS_ORST_I2S | SYS_ORST_SPDIF | SYS_ORST_USB32K | SYS_ORST_PWM0 | SYS_ORST_PWM1 |
        SYS_ORST_PWM2 | SYS_ORST_PWM3 | SYS_ORST_CODEC | SYS_ORST_CODECIIR | SYS_ORST_CODECRS |
        SYS_ORST_CODECADC | SYS_ORST_CODECAD0 | SYS_ORST_CODECAD1 | SYS_ORST_CODECDA0 |
        SYS_ORST_CODECDA1;
    cmu->PRESET_SET = SYS_PRST_PWM | SYS_PRST_I2C | SYS_PRST_SPI | SYS_PRST_UART0 | SYS_PRST_UART1 |
        SYS_PRST_I2S | SYS_PRST_SPDIF;
    cmu->HRESET_SET = SYS_HRST_USBC | SYS_HRST_CODEC | SYS_HRST_USBH | SYS_HRST_FFT;

    cmu->OCLK_DISABLE = SYS_OCLK_USB | SYS_OCLK_I2C | SYS_OCLK_SPI | SYS_OCLK_UART0 | SYS_OCLK_UART1 |
        SYS_OCLK_I2S | SYS_OCLK_SPDIF | SYS_OCLK_USB32K | SYS_OCLK_PWM0 | SYS_OCLK_PWM1 |
        SYS_OCLK_PWM2 | SYS_OCLK_PWM3 | SYS_OCLK_CODEC | SYS_OCLK_CODECIIR | SYS_OCLK_CODECRS |
        SYS_OCLK_CODECADC | SYS_OCLK_CODECAD0 | SYS_OCLK_CODECAD1 | SYS_OCLK_CODECDA0 |
        SYS_OCLK_CODECDA1;
    cmu->PCLK_DISABLE = SYS_PCLK_PWM | SYS_PCLK_I2C |SYS_PCLK_SPI |SYS_PCLK_UART0 | SYS_PCLK_UART1 |
        SYS_PCLK_I2S | SYS_PCLK_SPDIF;
    cmu->HCLK_DISABLE = SYS_HCLK_USBC | SYS_HCLK_CODEC | SYS_HCLK_USBH | SYS_HCLK_FFT;
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_ema_init(void)
{
}

void hal_cmu_lpu_wait_26m_ready(void)
{
    while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0);
}

int hal_cmu_lpu_busy(void)
{
    if ((cmu->WAKEUP_CLK_CFG & CMU_LPU_AUTO_SWITCH26) &&
        (cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0) {
        return 1;
    }
    if ((cmu->WAKEUP_CLK_CFG & CMU_LPU_AUTO_SWITCHPLL) &&
        (cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_PLL) == 0) {
        return 1;
    }
    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg)
{
    uint32_t lpu_clk;
    uint32_t timer_26m;
    uint32_t timer_pll;

    timer_26m = LPU_TIMER_US(TICKS_TO_US(HAL_CMU_26M_READY_TIMEOUT));
    timer_pll = LPU_TIMER_US(TICKS_TO_US(HAL_CMU_PLL_LOCKED_TIMEOUT));

    if (cfg >= HAL_CMU_LPU_CLK_QTY) {
        return 1;
    }
    if ((timer_26m & (CMU_TIMER_WT26_MASK >> CMU_TIMER_WT26_SHIFT)) != timer_26m) {
        return 2;
    }
    if ((timer_pll & (CMU_TIMER_WTPLL_MASK >> CMU_TIMER_WTPLL_SHIFT)) != timer_pll) {
        return 3;
    }
    if (hal_cmu_lpu_busy()) {
        return -1;
    }

    if (cfg == HAL_CMU_LPU_CLK_26M) {
        lpu_clk = CMU_LPU_AUTO_SWITCH26;
    } else if (cfg == HAL_CMU_LPU_CLK_PLL) {
        lpu_clk = CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26;
    } else {
        lpu_clk = 0;
    }

    if (lpu_clk & CMU_LPU_AUTO_SWITCH26) {
        // Disable RAM wakeup early
        cmu->MCU_TIMER &= ~CMU_RAM_RETN_UP_EARLY;
        // MCU/ROM/RAM auto clock gating (which depends on RAM gating signal)
        cmu->HCLK_MODE &= ~(SYS_HCLK_MCU | SYS_HCLK_ROM | SYS_HCLK_RAM0 | SYS_HCLK_RAM1);
    }

    cmu->WAKEUP_CLK_CFG = CMU_TIMER_WT26(timer_26m) | CMU_TIMER_WTPLL(0) | lpu_clk;
    if (timer_pll) {
        hal_sys_timer_delay(US_TO_TICKS(60));
        cmu->WAKEUP_CLK_CFG = CMU_TIMER_WT26(timer_26m) | CMU_TIMER_WTPLL(timer_pll) | lpu_clk;
    }
    return 0;
}

int SRAM_TEXT_LOC hal_cmu_lpu_sleep(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_clk_cfg;
    uint32_t wakeup_cfg;

    saved_clk_cfg = cmu->SYS_CLK_ENABLE;

    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        wakeup_cfg = cmu->WAKEUP_CLK_CFG;
    } else {
        wakeup_cfg = 0;
    }

    // Setup wakeup mask
    cmu->WAKEUP_MASK0 = NVIC->ISER[0];
    cmu->WAKEUP_MASK1 = NVIC->ISER[1];

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        // Do nothing
        // Hardware will switch system/memory/flash freq to 32K and shutdown PLLs automatically
    } else {
        // Switch memory/flash freq to 26M or 32K
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            cmu->SYS_CLK_ENABLE = CMU_SEL_OSC_FLS_ENABLE;
            cmu->SYS_CLK_DISABLE = CMU_SEL_OSCX2_FLS_DISABLE | CMU_SEL_PLL_FLS_DISABLE;
            cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_FLS_DISABLE;
        } else {
            cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_FLS_DISABLE |
                CMU_SEL_OSCX2_FLS_DISABLE | CMU_SEL_PLL_FLS_DISABLE;
            cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_FLS_DISABLE;
        }
        // Switch system freq to 26M
        cmu->SYS_CLK_ENABLE = CMU_SEL_OSC_SYS_ENABLE;
        cmu->SYS_CLK_DISABLE = CMU_SEL_OSCX2_SYS_DISABLE | CMU_SEL_PLL_SYS_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_SYS_DISABLE;
        // Shutdown PLLs
        if (saved_clk_cfg & CMU_PU_PLLUSB_ENABLE) {
            cmu->SYS_CLK_DISABLE = CMU_EN_PLLUSB_ENABLE;
            cmu->SYS_CLK_DISABLE = CMU_PU_PLLUSB_ENABLE;
        }
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            // Do nothing
            // Hardware will switch system/memory/flash freq to 32K automatically
        } else {
            // Switch system freq to 32K
            cmu->SYS_CLK_DISABLE = CMU_SEL_OSC_SYS_DISABLE;
        }
    }

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
        // Enable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_MANUAL_RAM_RETN) |
            CMU_DEEPSLEEP_EN | CMU_DEEPSLEEP_ROMRAM_EN | CMU_DEEPSLEEP_START;
    } else {
        // Disable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_DEEPSLEEP_ROMRAM_EN) |
            CMU_DEEPSLEEP_EN | CMU_MANUAL_RAM_RETN | CMU_DEEPSLEEP_START;
    }

    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
    } else {
        SCB->SCR = 0;
    }
    __DSB();
    __WFI();

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_PLL_LOCKED_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
        while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_PLL) == 0 &&
            (hal_sys_timer_get() - start) < timeout);
        // !!! CAUTION !!!
        // Hardware will switch system/memory/flash freq to PLL divider and enable PLLs automatically
    } else {
        // Wait for 26M ready
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            start = hal_sys_timer_get_in_sleep();
            timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
            while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0 &&
                (hal_sys_timer_get_in_sleep() - start) < timeout);
            // Hardware will switch system/memory/flash freq to 26M automatically
        } else {
            if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
                timeout = HAL_CMU_26M_READY_TIMEOUT;
                hal_sys_timer_delay_in_sleep(timeout);
            }
            // Switch system freq to 26M
            cmu->SYS_CLK_ENABLE = CMU_SEL_OSC_SYS_ENABLE;
        }
        // System freq is 26M now and will be restored later
        // Restore PLLs
        if (saved_clk_cfg & CMU_PU_PLLUSB_ENABLE) {
            cmu->SYS_CLK_ENABLE = CMU_PU_PLLUSB_ENABLE;
            start = hal_sys_timer_get();
            timeout = HAL_CMU_PLL_LOCKED_TIMEOUT;
            while ((hal_sys_timer_get() - start) < timeout);
            if (saved_clk_cfg & CMU_EN_PLLUSB_ENABLE) {
                cmu->SYS_CLK_ENABLE = CMU_EN_PLLUSB_ENABLE;
            }
        }
    }

    // Restore system/flash freq
    cmu->SYS_CLK_ENABLE = saved_clk_cfg &
        (CMU_RSTN_DIV_FLS_ENABLE | CMU_RSTN_DIV_SYS_ENABLE);
    cmu->SYS_CLK_ENABLE = saved_clk_cfg;
    // The original system/flash freq are at least 26M
    //cmu->SYS_CLK_DISABLE = ~saved_clk_cfg;

    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        // Wait at least 100us until the voltages become stable
        hal_sys_timer_delay_us(100);
    }

    return 0;
}

volatile uint32_t *hal_cmu_get_bootmode_addr(void)
{
    return &cmu->BOOTMODE;
}

volatile uint32_t *hal_cmu_get_memsc_addr(void)
{
    return &cmu->MEMSC[0];
}

