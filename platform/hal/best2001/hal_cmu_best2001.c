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
#include "reg_cmu_best2001.h"
#include "reg_aoncmu_best2001.h"
#include "reg_btcmu_best2001.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_bootmode.h"
#include "hal_chipid.h"
#include "hal_codec.h"
#include "hal_location.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "cmsis_nvic.h"
#include "pmu.h"
#include "system_cp.h"

#define WORKAROUND_DSP_TIMER_BUG

#define HAL_CMU_USB_PLL_CLOCK           (192 * 1000 * 1000)
#define HAL_CMU_AUD_PLL_CLOCK           (CODEC_FREQ_48K_SERIES * CODEC_CMU_DIV)

#define HAL_CMU_USB_CLOCK_60M           (60 * 1000 * 1000)
#define HAL_CMU_USB_CLOCK_48M           (48 * 1000 * 1000)

#define HAL_CMU_PWM_SLOW_CLOCK          (32 * 1000)

#define HAL_CMU_PLL_LOCKED_TIMEOUT      US_TO_TICKS(200)
#define HAL_CMU_26M_READY_TIMEOUT       MS_TO_TICKS(3)
#define HAL_CMU_LPU_EXTRA_TIMEOUT       MS_TO_TICKS(1)

enum CMU_USB_CLK_SRC_T {
    CMU_USB_CLK_SRC_PLL_48M         = 0,
    CMU_USB_CLK_SRC_PLL_60M         = 1,
};

enum CMU_AUD_26M_X4_USER_T {
    CMU_AUD_26M_X4_USER_IIR,
    CMU_AUD_26M_X4_USER_RS,

    CMU_AUD_26M_X4_USER_QTY,
};

struct CP_STARTUP_CFG_T {
    __IO uint32_t stack;
    __IO uint32_t reset_hdlr;
};

static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;

static struct AONCMU_T * const aoncmu = (struct AONCMU_T *)AON_CMU_BASE;

static struct BTCMU_T * const POSSIBLY_UNUSED btcmu = (struct BTCMU_T *)BT_CMU_BASE;

static uint32_t cp_entry;

static uint8_t BOOT_BSS_LOC pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(pll_user_map[0]) * 8, "Too many PLL users");

#ifdef __AUDIO_RESAMPLE__
static bool aud_resample_en = true;
#ifdef ANA_26M_X4_ENABLE
static uint8_t aud_26m_x4_map;
STATIC_ASSERT(CMU_AUD_26M_X4_USER_QTY <= sizeof(aud_26m_x4_map) * 8, "Too many aud_26m_x4 users");
#endif
#endif

#ifdef LOW_SYS_FREQ
#ifndef CHIP_BEST2001_DSP
static enum HAL_CMU_LOW_SYS_FREQ_T BOOT_BSS_LOC low_sys_freq;
static bool BOOT_BSS_LOC low_sys_freq_en;
#endif
static enum HAL_CMU_LOW_SYS_FREQ_T BOOT_BSS_LOC low_dsp_freq;
static bool BOOT_BSS_LOC low_dsp_freq_en;
#endif

void hal_cmu_audio_resample_enable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = true;
#endif
}

void hal_cmu_audio_resample_disable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = false;
#endif
}

int hal_cmu_get_audio_resample_status(void)
{
#ifdef __AUDIO_RESAMPLE__
    return aud_resample_en;
#else
    return false;
#endif
}

static inline void aocmu_reg_update_wait(void)
{
    // Make sure AOCMU (26M clock domain) write opertions finish before return
    aoncmu->CHIP_ID;
}

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_AON_A7) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_ENABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_ENABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->OCLK_ENABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        aoncmu->MOD_CLK_ENABLE = (1 << (id - HAL_CMU_AON_A_CMU));
        aocmu_reg_update_wait();
    }

    return 0;
}

int hal_cmu_clock_disable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_AON_A7) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_DISABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_DISABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->OCLK_DISABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        aoncmu->MOD_CLK_DISABLE = (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return 0;
}

enum HAL_CMU_CLK_STATUS_T hal_cmu_clock_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_AON_A7) {
        return HAL_CMU_CLK_DISABLED;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HCLK_ENABLE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PCLK_ENABLE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = cmu->OCLK_ENABLE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        status = aoncmu->MOD_CLK_ENABLE & (1 << (id - HAL_CMU_AON_A_CMU));
    }

    return status ? HAL_CMU_CLK_ENABLED : HAL_CMU_CLK_DISABLED;
}

int hal_cmu_clock_set_mode(enum HAL_CMU_MOD_ID_T id, enum HAL_CMU_CLK_MODE_T mode)
{
    __IO uint32_t *reg;
    uint32_t val;
    uint32_t lock;

    if (id >= HAL_CMU_AON_A7) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        reg = &cmu->HCLK_MODE;
        val = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        reg = &cmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        reg = &cmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        reg = &aoncmu->MOD_CLK_MODE;
        val = (1 << (id - HAL_CMU_AON_A_CMU));
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

    if (id >= HAL_CMU_AON_A7) {
        return HAL_CMU_CLK_AUTO;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        mode = cmu->HCLK_MODE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        mode = cmu->PCLK_MODE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else if (id < HAL_CMU_AON_A_CMU) {
        mode = cmu->OCLK_MODE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        mode = aoncmu->MOD_CLK_MODE & (1 << (id - HAL_CMU_AON_A_CMU));
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
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->ORESET_SET = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        aoncmu->RESET_SET = (1 << (id - HAL_CMU_AON_A_CMU));
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
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->ORESET_CLR = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        aoncmu->RESET_CLR = (1 << (id - HAL_CMU_AON_A_CMU));
        aocmu_reg_update_wait();
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
    } else if (id < HAL_CMU_AON_A_CMU) {
        status = cmu->ORESET_SET & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        status = aoncmu->RESET_SET & (1 << (id - HAL_CMU_AON_A_CMU));
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
    } else if (id < HAL_CMU_AON_A_CMU) {
        cmu->ORESET_PULSE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    } else {
        aoncmu->RESET_PULSE = (1 << (id - HAL_CMU_AON_A_CMU));
        // Total 3 CLK-26M cycles needed
        // AOCMU runs in 26M clock domain and its read operations consume at least 1 26M-clock cycle.
        // (Whereas its write operations will finish at 1 HCLK cycle -- finish once in async bridge fifo)
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
        aoncmu->CHIP_ID;
    }
    // Delay 5+ PCLK cycles (10+ HCLK cycles)
    for (i = 0; i < 3; i++);

    return 0;
}

int hal_cmu_timer_set_div(enum HAL_CMU_TIMER_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 1) {
        return 1;
    }

    div -= 1;
    if ((div & (CMU_CFG_DIV_TIMER00_MASK >> CMU_CFG_DIV_TIMER00_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    if (id == HAL_CMU_TIMER_ID_00) {
        cmu->TIMER0_CLK = SET_BITFIELD(cmu->TIMER0_CLK, CMU_CFG_DIV_TIMER00, div);
    } else if (id == HAL_CMU_TIMER_ID_01) {
        cmu->TIMER0_CLK = SET_BITFIELD(cmu->TIMER0_CLK, CMU_CFG_DIV_TIMER01, div);
    } else if (id == HAL_CMU_TIMER_ID_10) {
        cmu->TIMER1_CLK = SET_BITFIELD(cmu->TIMER1_CLK, CMU_CFG_DIV_TIMER10, div);
    } else if (id == HAL_CMU_TIMER_ID_11) {
        cmu->TIMER1_CLK = SET_BITFIELD(cmu->TIMER1_CLK, CMU_CFG_DIV_TIMER11, div);
    } else if (id == HAL_CMU_TIMER_ID_20) {
        cmu->TIMER2_CLK = SET_BITFIELD(cmu->TIMER2_CLK, CMU_CFG_DIV_TIMER20, div);
    } else if (id == HAL_CMU_TIMER_ID_21) {
        cmu->TIMER2_CLK = SET_BITFIELD(cmu->TIMER2_CLK, CMU_CFG_DIV_TIMER21, div);
    }
    int_unlock(lock);

    return 0;
}

void hal_cmu_codec_set_fault_mask(uint32_t msk)
{
    uint32_t lock;

    lock = int_lock();
    // If bit set 1, DAC will be muted when some faults occur
    cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_MASK_OBS, msk);
    int_unlock(lock);
}

void hal_cmu_timer0_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->PERIPH_CLK |= (1 << CMU_SEL_TIMER_FAST_SHIFT);
    // AON Timer
    aoncmu->TIMER_CLK |= AON_CMU_SEL_TIMER_FAST;
    int_unlock(lock);
}

void hal_cmu_timer0_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << CMU_SEL_TIMER_FAST_SHIFT);
    // AON Timer
    aoncmu->TIMER_CLK &= ~AON_CMU_SEL_TIMER_FAST;
    int_unlock(lock);
}

void hal_cmu_timer1_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->PERIPH_CLK |= (1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void hal_cmu_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << (CMU_SEL_TIMER_FAST_SHIFT + 1));
    int_unlock(lock);
}

void hal_cmu_timer2_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->PERIPH_CLK |= (1 << (CMU_SEL_TIMER_FAST_SHIFT + 2));
    int_unlock(lock);
}

void hal_cmu_timer2_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->PERIPH_CLK &= ~(1 << (CMU_SEL_TIMER_FAST_SHIFT + 2));
    int_unlock(lock);
}

void hal_cmu_dsp_timer0_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
#ifdef WORKAROUND_DSP_TIMER_BUG
    /* workaround for dsp timer1 selection bug.
      * timer0 and timer1 select fast pll, and enable clk_div to obtain 16k
      */
    ///TODO: switch timer0 to 16k before cmu goto sleep
    cmu->DSP_DIV |= (1 << CMU_SEL_TIMER_FAST_AP_SHIFT);
#else
    // 6.5M
    cmu->DSP_DIV |= (1 << CMU_SEL_TIMER_FAST_AP_SHIFT);
#endif
    int_unlock(lock);
}

void hal_cmu_dsp_timer0_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
#ifdef WORKAROUND_DSP_TIMER_BUG
    cmu->DSP_DIV |= (1 << CMU_SEL_TIMER_FAST_AP_SHIFT);
    cmu->AP_TIMER1_CLK = CONFIG_FAST_SYSTICK_HZ/CONFIG_SYSTICK_HZ - 1;
#else
    // 16K
    cmu->DSP_DIV &= ~(1 << CMU_SEL_TIMER_FAST_AP_SHIFT);
#endif
    int_unlock(lock);
}

void hal_cmu_dsp_timer1_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
#ifdef WORKAROUND_DSP_TIMER_BUG
#else
    // 6.5M
    cmu->DSP_DIV |= (1 << (CMU_SEL_TIMER_FAST_AP_SHIFT + 1));
#endif
    int_unlock(lock);
}

void hal_cmu_dsp_timer1_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
#ifdef WORKAROUND_DSP_TIMER_BUG
    cmu->AP_TIMER2_CLK = CONFIG_FAST_SYSTICK_HZ/CONFIG_SYSTICK_HZ - 1;
#else
    // 16K
    cmu->DSP_DIV &= ~(1 << (CMU_SEL_TIMER_FAST_AP_SHIFT + 1));
#endif
    int_unlock(lock);
}

int hal_cmu_sdmmc_set_pll_div(uint32_t div)
{
    uint32_t lock;
    if (div < 2) {
        return 1;
    }
    div -= 2;
    if ((div & (CMU_CFG_DIV_SDMMC_MASK >> CMU_CFG_DIV_SDMMC_SHIFT)) != div) {
        return 1;
    }
    lock = int_lock();
    cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_CFG_DIV_SDMMC, div) |
        CMU_SEL_OSCX2_SDMMC | CMU_SEL_PLL_SDMMC | CMU_EN_PLL_SDMMC;
    int_unlock(lock);
    return 0;
}

int BOOT_TEXT_SRAM_LOC hal_cmu_flash_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t set;
    uint32_t clr;
    uint32_t lock;
    bool clk_en;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }
    if (freq == HAL_CMU_FREQ_32K) {
        return 2;
    }

    switch (freq) {
    case HAL_CMU_FREQ_26M:
        set = 0;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_OSCX4 | AON_CMU_SEL_FLS_PLL;
        break;
    case HAL_CMU_FREQ_52M:
        set = AON_CMU_SEL_FLS_OSCX2;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX4 | AON_CMU_SEL_FLS_PLL;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        set = AON_CMU_SEL_FLS_OSCX4;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_PLL;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
        set = AON_CMU_RSTN_DIV_FLS | AON_CMU_SEL_FLS_PLL;
        clr = AON_CMU_CFG_DIV_FLS_MASK | AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_OSCX4;
        if (0) {
#ifndef OSC_26M_X4_AUD2BB
        } else if (freq <= HAL_CMU_FREQ_78M) {
            set |= AON_CMU_CFG_DIV_FLS(3);
        } else if (freq <= HAL_CMU_FREQ_104M) {
            set |= AON_CMU_CFG_DIV_FLS(2);
#endif
        } else if (freq <= HAL_CMU_FREQ_156M) {
            set |= AON_CMU_CFG_DIV_FLS(1);
        } else { // 208M or 260M
            set |= AON_CMU_CFG_DIV_FLS(0);
        }
        break;
    case HAL_CMU_FREQ_390M:
    case HAL_CMU_FREQ_780M:
    default:
        set = AON_CMU_BYPASS_DIV_FLS | AON_CMU_SEL_FLS_PLL;
        clr = AON_CMU_RSTN_DIV_FLS | AON_CMU_SEL_FLS_OSCX2 | AON_CMU_SEL_FLS_OSCX4;
        break;
    };

    lock = int_lock();
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_FLASH);
    if (clk_en) {
        cmu->OCLK_DISABLE = SYS_OCLK_FLASH;
        cmu->HCLK_DISABLE = SYS_HCLK_FLASH;
        // Wait at least 2 cycles of flash controller. The min freq is 26M, the same as AON.
        aocmu_reg_update_wait();
        aocmu_reg_update_wait();
    }
    aoncmu->FLS_PSR_CLK = (aoncmu->FLS_PSR_CLK & ~clr) | set;
    if (clk_en) {
        cmu->HCLK_ENABLE = SYS_HCLK_FLASH;
        cmu->OCLK_ENABLE = SYS_OCLK_FLASH;
    }
    int_unlock(lock);

    return 0;
}

int hal_cmu_mem_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t set;
    uint32_t clr;
    uint32_t lock;
    bool clk_en;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }
    if (freq == HAL_CMU_FREQ_32K) {
        return 2;
    }

    switch (freq) {
    case HAL_CMU_FREQ_26M:
        set = 0;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_OSCX4 | AON_CMU_SEL_PSR_PLL;
        break;
    case HAL_CMU_FREQ_52M:
        set = AON_CMU_SEL_PSR_OSCX2;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX4 | AON_CMU_SEL_PSR_PLL;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        set = AON_CMU_SEL_PSR_OSCX4;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_PLL;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
        set = AON_CMU_RSTN_DIV_PSR | AON_CMU_SEL_PSR_PLL;
        clr = AON_CMU_CFG_DIV_PSR_MASK | AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_OSCX4;
        if (0) {
#ifndef OSC_26M_X4_AUD2BB
        } else if (freq <= HAL_CMU_FREQ_78M) {
            set |= AON_CMU_CFG_DIV_PSR(3);
        } else if (freq <= HAL_CMU_FREQ_104M) {
            set |= AON_CMU_CFG_DIV_PSR(2);
#endif
        } else if (freq <= HAL_CMU_FREQ_156M) {
            set |= AON_CMU_CFG_DIV_PSR(1);
        } else { // 208M or 260M
            set |= AON_CMU_CFG_DIV_PSR(0);
        }
        break;
    case HAL_CMU_FREQ_390M:
    case HAL_CMU_FREQ_780M:
    default:
        set = AON_CMU_BYPASS_DIV_PSR | AON_CMU_SEL_PSR_PLL;
        clr = AON_CMU_RSTN_DIV_PSR | AON_CMU_SEL_PSR_OSCX2 | AON_CMU_SEL_PSR_OSCX4;
        break;
    };

    if (freq <= HAL_CMU_FREQ_104M) {
        // Clock from dig
        set |= AON_CMU_SEL_PSR_INT;
    } else {
        // Clock from phy
        clr |= AON_CMU_SEL_PSR_INT;
    }

    lock = int_lock();
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_PSRAM200);
    if (clk_en) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PHY_PS_DISABLE;
        cmu->OCLK_DISABLE = SYS_OCLK_PSRAM200;
        cmu->HCLK_DISABLE = SYS_HCLK_PSRAM200;
        // Wait at least 2 cycles of psram controller. The min freq is 26M, the same as AON.
        aocmu_reg_update_wait();
        aocmu_reg_update_wait();
    }
    aoncmu->FLS_PSR_CLK = (aoncmu->FLS_PSR_CLK & ~clr) | set;
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PHY_PS_ENABLE;
    if (clk_en) {
        cmu->HCLK_ENABLE = SYS_HCLK_PSRAM200;
        cmu->OCLK_ENABLE = SYS_OCLK_PSRAM200;
    }
    int_unlock(lock);

    return 0;
}

int hal_cmu_ddr_clock_enable()
{
    uint32_t set;
    uint32_t clr;
    uint32_t lock;
    bool clk_en;

    // PSRAMUHS can't use ddr pll divider
    set = AON_CMU_BYPASS_DIV_DDR | AON_CMU_SEL_DDR_PLL;
    clr = AON_CMU_RSTN_DIV_DDR | AON_CMU_SEL_DDR_OSCX2 | AON_CMU_SEL_DDR_OSCX4;

    lock = int_lock();
    clk_en = !!(cmu->OCLK_DISABLE & SYS_OCLK_PSRAM1G);
    if (clk_en) {
        cmu->OCLK_DISABLE = SYS_OCLK_PSRAM1G;
        cmu->HCLK_DISABLE = SYS_HCLK_PSRAM1G;
        // Wait at least 2 cycles of psram controller. The min freq is 26M, the same as AON.
        aocmu_reg_update_wait();
        aocmu_reg_update_wait();
    }
    aoncmu->DDR_CLK = (aoncmu->DDR_CLK & ~clr) | set;
    if (clk_en) {
        cmu->HCLK_ENABLE = SYS_HCLK_PSRAM1G;
        cmu->OCLK_ENABLE = SYS_OCLK_PSRAM1G;
    }
    int_unlock(lock);

    return 0;
}

#ifdef CHIP_BEST2001_DSP

#ifdef LOW_SYS_FREQ
void hal_cmu_low_sys_clock_set(enum HAL_CMU_LOW_SYS_FREQ_T freq) __attribute__((alias("hal_cmu_low_dsp_clock_set")));
#endif

int hal_cmu_sys_set_freq(enum HAL_CMU_FREQ_T freq) __attribute__((alias("hal_cmu_dsp_set_freq")));

enum HAL_CMU_FREQ_T hal_cmu_sys_get_freq(void) __attribute__((alias("hal_cmu_dsp_get_freq")));

#else // !CHIP_BEST2001_DSP

#ifdef LOW_SYS_FREQ
void hal_cmu_low_sys_clock_set(enum HAL_CMU_LOW_SYS_FREQ_T freq)
{
    uint32_t lock;

    lock = int_lock();
    low_sys_freq = freq;
    hal_cmu_sys_set_freq(hal_sysfreq_get_hw_freq());
    int_unlock(lock);
}

int hal_cmu_fast_timer_offline(void)
{
    return low_sys_freq_en && low_sys_freq != HAL_CMU_LOW_SYS_FREQ_13M;
}
#endif

int hal_cmu_sys_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    int div;
    uint32_t lock;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }

#ifdef LOW_SYS_FREQ
    low_sys_freq_en = false;
#endif

    div = -1;

    switch (freq) {
    case HAL_CMU_FREQ_32K:
        enable = 0;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_26M:
#ifdef LOW_SYS_FREQ
        if (low_sys_freq == HAL_CMU_LOW_SYS_FREQ_13M) {
            low_sys_freq_en = true;
            enable = CMU_SEL_MCU_OSC_2_ENABLE | CMU_SEL_MCU_SLOW_ENABLE;
            disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE |
                CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
            break;
        } else if (low_sys_freq == HAL_CMU_LOW_SYS_FREQ_6P5M) {
            low_sys_freq_en = true;
            enable = CMU_SEL_MCU_OSC_4_ENABLE;
            disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
                CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
            break;
        }
#endif
        enable = CMU_SEL_MCU_SLOW_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_52M:
        enable = CMU_SEL_MCU_FAST_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        enable = CMU_SEL_MCU_OSCX4_ENABLE | CMU_SEL_MCU_FAST_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
            CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
        enable = CMU_RSTN_DIV_MCU_ENABLE | CMU_SEL_MCU_PLL_ENABLE;
        disable = CMU_BYPASS_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_FAST_DISABLE;
        if (0) {
#ifndef OSC_26M_X4_AUD2BB
        } else if (freq <= HAL_CMU_FREQ_78M) {
            div = 3;
        } else if (freq <= HAL_CMU_FREQ_104M) {
            div = 2;
#endif
        } else if (freq <= HAL_CMU_FREQ_156M) {
            div = 1;
        } else { // 208M or 260M
            div = 0;
        }
        break;
    case HAL_CMU_FREQ_390M:
    case HAL_CMU_FREQ_780M:
    default:
        enable = CMU_BYPASS_DIV_MCU_ENABLE | CMU_SEL_MCU_PLL_ENABLE;
        disable = CMU_RSTN_DIV_MCU_DISABLE | CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
            CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_SLOW_DISABLE | CMU_SEL_MCU_FAST_DISABLE;
        break;
    };

    if (div >= 0) {
        lock = int_lock();
        cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_MCU, div);
        int_unlock(lock);
    }

    if (enable & CMU_SEL_MCU_PLL_ENABLE) {
        cmu->SYS_CLK_ENABLE = CMU_RSTN_DIV_MCU_ENABLE;
        if (enable & CMU_BYPASS_DIV_MCU_ENABLE) {
            cmu->SYS_CLK_ENABLE = CMU_BYPASS_DIV_MCU_ENABLE;
        } else {
            cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_MCU_DISABLE;
        }
    }
    cmu->SYS_CLK_ENABLE = enable;
    if (enable & CMU_SEL_MCU_PLL_ENABLE) {
        cmu->SYS_CLK_DISABLE = disable;
    } else {
        cmu->SYS_CLK_DISABLE = disable & ~(CMU_RSTN_DIV_MCU_DISABLE | CMU_BYPASS_DIV_MCU_DISABLE);
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_MCU_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_MCU_DISABLE;
    }

    return 0;
}

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_sys_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = cmu->SYS_CLK_ENABLE;

    if (sys_clk & CMU_SEL_MCU_PLL_ENABLE) {
        if (sys_clk & CMU_BYPASS_DIV_MCU_ENABLE) {
            // 384M
            return HAL_CMU_FREQ_390M;
        } else {
            div = GET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_MCU);
            if (div == 0) {
                // 192M
                return HAL_CMU_FREQ_208M;
            } else if (div == 1) {
                // 128M
                return HAL_CMU_FREQ_156M;
            } else if (div == 2) {
                // 96M
                return HAL_CMU_FREQ_104M;
            } else { // div == 3
                // 76.8M
                return HAL_CMU_FREQ_78M;
            }
        }
    } else if (sys_clk & CMU_SEL_MCU_FAST_ENABLE) {
        if (sys_clk & CMU_SEL_MCU_OSCX4_ENABLE) {
            return HAL_CMU_FREQ_104M;
        } else {
            return HAL_CMU_FREQ_52M;
        }
    } else if (sys_clk & CMU_SEL_MCU_SLOW_ENABLE) {
        return HAL_CMU_FREQ_26M;
    } else {
        return HAL_CMU_FREQ_32K;
    }
}

#endif // !CHIP_BEST2001_DSP

#ifdef LOW_SYS_FREQ
void hal_cmu_low_dsp_clock_set(enum HAL_CMU_LOW_SYS_FREQ_T freq)
{
    uint32_t lock;

    lock = int_lock();
    low_dsp_freq = freq;
    hal_cmu_dsp_set_freq(hal_sysfreq_get_hw_freq());
    int_unlock(lock);
}

int hal_cmu_low_dsp_clock_enabled(void)
{
    return low_dsp_freq_en;
}
#endif

int hal_cmu_dsp_set_freq(enum HAL_CMU_FREQ_T freq)
{
    uint32_t enable;
    uint32_t disable;
    int div;
    uint32_t lock;

    if (freq >= HAL_CMU_FREQ_QTY) {
        return 1;
    }

#ifdef PSRAMUHS_ENABLE
    /* if a7 frequency < psramuhs frequency, tCPHW should be set to 100 or larger */
    if (freq < HAL_CMU_FREQ_780M) {
        freq = HAL_CMU_FREQ_780M;
    }
#endif

#ifdef LOW_SYS_FREQ
    low_dsp_freq_en = false;
#endif

    div = -1;

    switch (freq) {
    case HAL_CMU_FREQ_32K:
        enable = 0;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_26M:
#ifndef WORKAROUND_DSP_TIMER_BUG
#ifdef LOW_SYS_FREQ
        if (low_sys_freq == HAL_CMU_LOW_SYS_FREQ_13M) {
            low_dsp_freq_en = true;
            enable = CMU_SEL_A7_OSC_2_ENABLE | CMU_SEL_A7_SLOW_ENABLE;
            disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE |
                CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
            break;
        } else if (low_sys_freq == HAL_CMU_LOW_SYS_FREQ_6P5M) {
            low_dsp_freq_en = true;
            enable = CMU_SEL_A7_OSC_4_ENABLE;
            disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
                CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
            break;
        }
#endif
        enable = CMU_SEL_A7_SLOW_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_FAST_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_52M:
        enable = CMU_SEL_A7_FAST_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
    case HAL_CMU_FREQ_78M:
    case HAL_CMU_FREQ_104M:
#ifdef OSC_26M_X4_AUD2BB
        enable = CMU_SEL_A7_OSCX4_ENABLE | CMU_SEL_A7_FAST_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_PLL_DISABLE;
        break;
#endif
    case HAL_CMU_FREQ_156M:
    case HAL_CMU_FREQ_208M:
    case HAL_CMU_FREQ_260M:
    case HAL_CMU_FREQ_390M:
        enable = CMU_RSTN_DIV_A7_ENABLE | CMU_SEL_A7_PLL_ENABLE;
        disable = CMU_BYPASS_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_FAST_DISABLE;
        if (0) {
        } else if (freq <= HAL_CMU_FREQ_156M) {
            div = 3;
        } else if (freq <= HAL_CMU_FREQ_208M) {
            div = 2;
        } else if (freq <= HAL_CMU_FREQ_260M) {
            div = 1;
        } else { // 390M
            div = 0;
        }
        break;
    case HAL_CMU_FREQ_780M:
    default:
        enable = CMU_BYPASS_DIV_A7_ENABLE | CMU_SEL_A7_PLL_ENABLE;
        disable = CMU_RSTN_DIV_A7_DISABLE | CMU_SEL_A7_OSC_4_DISABLE | CMU_SEL_A7_OSC_2_DISABLE |
            CMU_SEL_A7_OSCX4_DISABLE | CMU_SEL_A7_SLOW_DISABLE | CMU_SEL_A7_FAST_DISABLE;
        break;
    };

    if (div >= 0) {
        lock = int_lock();
        cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_A7, div);
        int_unlock(lock);
    }

    if (enable & CMU_SEL_A7_PLL_ENABLE) {
        cmu->SYS_CLK_ENABLE = CMU_RSTN_DIV_A7_ENABLE;
        if (enable & CMU_BYPASS_DIV_A7_ENABLE) {
            cmu->SYS_CLK_ENABLE = CMU_BYPASS_DIV_A7_ENABLE;
        } else {
            cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_A7_DISABLE;
        }
    }
    cmu->SYS_CLK_ENABLE = enable;
    if (enable & CMU_SEL_A7_PLL_ENABLE) {
        cmu->SYS_CLK_DISABLE = disable;
    } else {
        cmu->SYS_CLK_DISABLE = disable & ~(CMU_RSTN_DIV_A7_DISABLE | CMU_BYPASS_DIV_A7_DISABLE);
        cmu->SYS_CLK_DISABLE = CMU_BYPASS_DIV_A7_DISABLE;
        cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_A7_DISABLE;
    }

    return 0;
}

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_dsp_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = cmu->SYS_CLK_ENABLE;

    if (sys_clk & CMU_SEL_A7_PLL_ENABLE) {
        if (sys_clk & CMU_BYPASS_DIV_A7_ENABLE) {
            return HAL_CMU_FREQ_780M;
        } else {
            div = GET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_A7);
            if (div == 0) {
                return HAL_CMU_FREQ_390M;
            } else if (div == 1) {
                return HAL_CMU_FREQ_260M;
            } else if (div == 2) {
                return HAL_CMU_FREQ_208M;
            } else { // div == 3
                return HAL_CMU_FREQ_156M;
            }
        }
    } else if (sys_clk & CMU_SEL_A7_FAST_ENABLE) {
        if (sys_clk & CMU_SEL_A7_OSCX4_ENABLE) {
            return HAL_CMU_FREQ_104M;
        } else {
            return HAL_CMU_FREQ_52M;
        }
    } else if (sys_clk & CMU_SEL_A7_SLOW_ENABLE) {
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

int hal_cmu_dsp_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint32_t set;
    uint32_t clr;

    if (pll == HAL_CMU_PLL_USB) {
        set = AON_CMU_SEL_A7_PLLUSB;
        clr = AON_CMU_SEL_A7_PLLBB;
    } else if (pll == HAL_CMU_PLL_DSP) {
        set = 0;
        clr = AON_CMU_SEL_A7_PLLUSB | AON_CMU_SEL_A7_PLLBB;
    } else if (pll == HAL_CMU_PLL_BB) {
        set = AON_CMU_SEL_A7_PLLBB;
        clr = AON_CMU_SEL_A7_PLLUSB;
    } else {
        // == HAL_CMU_PLL_DDR or == HAL_CMU_PLL_BB_PSRAM or >= HAL_CMU_PLL_QTY
        return 1;
    }

    lock = int_lock();
    aoncmu->DSP_PLL_SELECT = (aoncmu->DSP_PLL_SELECT & ~clr) | set;
    int_unlock(lock);

    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_sys_select_pll(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint32_t set;
    uint32_t clr;

    if (pll == HAL_CMU_PLL_USB) {
        set = AON_CMU_SEL_MCU_PLLUSB | AON_CMU_SEL_MCU_PLLA7USB;
        clr = AON_CMU_SEL_MCU_PLLBB_PS;
    } else if (pll == HAL_CMU_PLL_DSP) {
        set = AON_CMU_SEL_MCU_PLLA7USB;
        clr = AON_CMU_SEL_MCU_PLLBB_PS | AON_CMU_SEL_MCU_PLLUSB;
    } else if (pll == HAL_CMU_PLL_BB) {
        set = 0;
        clr = AON_CMU_SEL_MCU_PLLBB_PS | AON_CMU_SEL_MCU_PLLUSB | AON_CMU_SEL_MCU_PLLA7USB;
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
        set = AON_CMU_SEL_MCU_PLLBB_PS;
        clr = AON_CMU_SEL_MCU_PLLUSB | AON_CMU_SEL_MCU_PLLA7USB;
    } else {
        // == HAL_CMU_PLL_DDR or >= HAL_CMU_PLL_QTY
        return 1;
    }

    lock = int_lock();
    aoncmu->FLS_PSR_CLK = (aoncmu->FLS_PSR_CLK & ~clr) | set;
    int_unlock(lock);

    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_audio_select_pll(enum HAL_CMU_PLL_T pll)
{
    if (pll == HAL_CMU_PLL_BB) {
        aoncmu->PCM_I2S_CLK &= ~AON_CMU_SEL_AUD_PLLUSB;
    } else {
        aoncmu->PCM_I2S_CLK |= AON_CMU_SEL_AUD_PLLUSB;
    }

    return 0;
}

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll)
{
    bool en;

    if (pll == HAL_CMU_PLL_USB) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE);
    } else if (pll == HAL_CMU_PLL_DDR) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLDDR_ENABLE);
    } else if (pll == HAL_CMU_PLL_DSP) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLA7_ENABLE);
    } else if (pll == HAL_CMU_PLL_BB) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLBB_ENABLE);
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
        en = !!(aoncmu->TOP_CLK_ENABLE & AON_CMU_EN_CLK_TOP_PLLBB_PS_ENABLE);
    } else {
        en = false;
    }

    return en;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t pu_val;
    uint32_t en_val;
    uint32_t lock;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    if (pll == HAL_CMU_PLL_USB) {
        pu_val = AON_CMU_PU_PLLUSB_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
    } else if (pll == HAL_CMU_PLL_DDR) {
        pu_val = AON_CMU_PU_PLLDDR_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDDR_ENABLE;
    } else if (pll == HAL_CMU_PLL_DSP) {
        pu_val = AON_CMU_PU_PLLA7_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLA7_ENABLE;
    } else if (pll == HAL_CMU_PLL_BB) {
        pu_val = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_MCU_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
        pu_val = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_PS_ENABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_PS_ENABLE;
    } else {
        pu_val = 0;
        en_val = 0;
    }

    lock = int_lock();

    if (pll == HAL_CMU_PLL_USB) {
        ASSERT((pll_user_map[pll]&user) != (HAL_CMU_PLL_USER_AUD|HAL_CMU_PLL_USER_USB),
            "USB pll couldn't used by AUD and USB at same time. map:x0%x, new use:0x%x",
            pll_user_map[pll], user);
    }
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
#ifndef ROM_BUILD
        pmu_pll_div_reset_set(pll);
#endif
        aoncmu->PLL_ENABLE = pu_val;
#ifndef ROM_BUILD
        hal_sys_timer_delay_us(20);
        pmu_pll_div_reset_clear(pll);
        // Wait at least 10us for clock ready
        hal_sys_timer_delay_us(10);
#endif
        aoncmu->TOP_CLK_ENABLE = en_val;
    }
    if (user < HAL_CMU_PLL_USER_QTY) {
        pll_user_map[pll] |= (1 << user);
    }
    int_unlock(lock);

    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t pu_val;
    uint32_t en_val;
    uint32_t lock;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    if (pll == HAL_CMU_PLL_USB) {
        pu_val = AON_CMU_PU_PLLUSB_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
    } else if (pll == HAL_CMU_PLL_DDR) {
        pu_val = AON_CMU_PU_PLLDDR_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLDDR_DISABLE;
    } else if (pll == HAL_CMU_PLL_DSP) {
        pu_val = AON_CMU_PU_PLLA7_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLA7_DISABLE;
    } else if (pll == HAL_CMU_PLL_BB) {
        pu_val = AON_CMU_PU_PLLBB_DIV_MCU_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_DISABLE;
    } else if (pll == HAL_CMU_PLL_BB_PSRAM) {
        pu_val = AON_CMU_PU_PLLBB_DIV_PS_DISABLE;
        en_val = AON_CMU_EN_CLK_TOP_PLLBB_PS_DISABLE;
    } else {
        pu_val = 0;
        en_val = 0;
    }

    lock = int_lock();
    if (user < HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] &= ~(1 << user);
    }
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        if ((pll == HAL_CMU_PLL_BB_PSRAM && pll_user_map[HAL_CMU_PLL_BB] == 0) ||
                (pll == HAL_CMU_PLL_BB && pll_user_map[HAL_CMU_PLL_BB_PSRAM] == 0)) {
            pu_val |= AON_CMU_PU_PLLBB_DISABLE;
        }
        aoncmu->TOP_CLK_DISABLE = en_val;
        aoncmu->PLL_DISABLE = pu_val;
    }
    int_unlock(lock);

    return 0;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_low_freq_mode_init(void)
{
}

void hal_cmu_low_freq_mode_enable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T switch_freq;

    // TODO: Select the PLL used by sys
    pll = HAL_CMU_PLL_BB;

#ifdef OSC_26M_X4_AUD2BB
    switch_freq = HAL_CMU_FREQ_104M;
#else
    switch_freq = HAL_CMU_FREQ_52M;
#endif

    if (old_freq > switch_freq && new_freq <= switch_freq) {
        hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_SYS);
    }
}

void hal_cmu_low_freq_mode_disable(enum HAL_CMU_FREQ_T old_freq, enum HAL_CMU_FREQ_T new_freq)
{
    enum HAL_CMU_PLL_T pll;
    enum HAL_CMU_FREQ_T switch_freq;

    // TODO: Select the PLL used by sys
    pll = HAL_CMU_PLL_BB;

#ifdef OSC_26M_X4_AUD2BB
    switch_freq = HAL_CMU_FREQ_104M;
#else
    switch_freq = HAL_CMU_FREQ_52M;
#endif

    if (old_freq <= switch_freq && new_freq > switch_freq) {
        hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_SYS);
    }
}

void hal_cmu_rom_enable_pll(void)
{
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_SYS);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_BB);
}

void hal_cmu_programmer_enable_pll(void)
{
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_SYS);
    hal_cmu_flash_select_pll(HAL_CMU_PLL_BB);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_BB);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_init_pll_selection(void)
{
    enum HAL_CMU_PLL_T sys;

    // Disable the PLL which might be enabled in ROM
    hal_cmu_pll_disable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_ALL);

#ifdef SYS_USE_USBPLL
    sys = HAL_CMU_PLL_USB;
#elif defined(SYS_USE_BB_PSRAMPLL) || defined(MCU_HIGH_PERFORMANCE_MODE_320)
    sys = HAL_CMU_PLL_BB_PSRAM;
#elif defined(SYS_USE_DSPPLL)
    sys = HAL_CMU_PLL_DSP;
#else
    sys = HAL_CMU_PLL_BB;
#endif
    hal_cmu_sys_select_pll(sys);

#ifdef AUDIO_USE_BBPLL
    hal_cmu_audio_select_pll(HAL_CMU_PLL_BB);
#else
    hal_cmu_audio_select_pll(HAL_CMU_PLL_AUD);
#endif

#ifndef ULTRA_LOW_POWER
    hal_cmu_pll_enable(sys, HAL_CMU_PLL_USER_SYS);
#endif
#if !(defined(FLASH_LOW_SPEED) || defined(OSC_26M_X4_AUD2BB))
    hal_cmu_pll_enable(HAL_CMU_PLL_BB, HAL_CMU_PLL_USER_FLASH);
#endif
#if defined(PSRAM_ENABLE) && !defined(PSRAM_LOW_SPEED)
    hal_cmu_pll_enable(HAL_CMU_PLL_BB_PSRAM, HAL_CMU_PLL_USER_PSRAM);
#endif
#if defined(PSRAMUHS_ENABLE) && !defined(PSRAM_LOW_SPEED)
    hal_cmu_pll_enable(HAL_CMU_PLL_DDR, HAL_CMU_PLL_USER_PSRAM);
#endif
}

#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
void hal_cmu_audio_26m_x4_enable(enum CMU_AUD_26M_X4_USER_T user)
{
    uint32_t lock;

    if (user >= CMU_AUD_26M_X4_USER_QTY) {
        return;
    }

    lock = int_lock();

    if (aud_26m_x4_map == 0) {
        aoncmu->PCM_I2S_CLK |= AON_CMU_SEL_AUD_X4;
    }
    aud_26m_x4_map |= (1 << user);

    int_unlock(lock);
}

void hal_cmu_audio_26m_x4_disable(enum CMU_AUD_26M_X4_USER_T user)
{
    uint32_t lock;

    if (user >= CMU_AUD_26M_X4_USER_QTY) {
        return;
    }

    lock = int_lock();

    if (aud_26m_x4_map & (1 << user)) {
        aud_26m_x4_map &= ~(1 << user);
        if (aud_26m_x4_map == 0) {
            aoncmu->PCM_I2S_CLK &= ~AON_CMU_SEL_X4_AUD;
        }
    }

    int_unlock(lock);
}
#endif

void hal_cmu_codec_iir_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;

    mask = AON_CMU_SEL_CODECIIR_OSC | AON_CMU_SEL_CODECIIR_OSCX2 | AON_CMU_BYPASS_DIV_CODECIIR;
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

    if (speed <= 26000000) {
        val |= AON_CMU_SEL_CODECIIR_OSC | AON_CMU_SEL_CODECIIR_OSCX2;
        speed = 26000000;
    } else if (speed <= 52000000) {
        val |= AON_CMU_SEL_CODECIIR_OSCX2;
        speed = 52000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (resample_en) {
            hal_cmu_audio_26m_x4_enable(CMU_AUD_26M_X4_USER_IIR);
            val |= AON_CMU_BYPASS_DIV_CODECIIR;
            speed = 104000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = HAL_CMU_AUD_PLL_CLOCK / speed;
            if (div >= 2) {
                hal_cmu_codec_iir_set_div(div);
                speed = HAL_CMU_AUD_PLL_CLOCK / div;
            } else {
                val |= AON_CMU_BYPASS_DIV_CODECIIR;
                speed = HAL_CMU_AUD_PLL_CLOCK;
            }
        }

       //pmu_iir_freq_config(speed);
    }

    lock = int_lock();
    aoncmu->CODEC_IIR = (aoncmu->CODEC_IIR & ~mask) | val;
    int_unlock(lock);

    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_CODECIIR_ENABLE;

    aocmu_reg_update_wait();
}

void hal_cmu_codec_iir_disable(void)
{
    uint32_t lock;
    uint32_t val;
    bool high_speed;

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODECIIR_DISABLE;

#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
    hal_cmu_audio_26m_x4_disable(CMU_AUD_26M_X4_USER_IIR);
#endif

    high_speed = !(aoncmu->CODEC_IIR & AON_CMU_SEL_CODECIIR_OSC);

    val = AON_CMU_SEL_CODECIIR_OSC | AON_CMU_SEL_CODECIIR_OSCX2;

    lock = int_lock();
    aoncmu->CODEC_IIR |= val;
    int_unlock(lock);

    if (high_speed) {
        //pmu_iir_freq_config(0);
    }
}

int hal_cmu_codec_iir_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    aoncmu->CODEC_IIR = SET_BITFIELD(aoncmu->CODEC_IIR, AON_CMU_CFG_DIV_CODECIIR, div);
    int_unlock(lock);

    return 0;
}

void hal_cmu_codec_rs_enable(uint32_t speed)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t val;
    uint32_t div;

    mask = AON_CMU_SEL_CODECRS0_OSC | AON_CMU_SEL_CODECRS0_OSCX2 | AON_CMU_BYPASS_DIV_CODECRS0;
    mask |= AON_CMU_SEL_CODECRS1_OSC | AON_CMU_SEL_CODECRS1_OSCX2 | AON_CMU_BYPASS_DIV_CODECRS1;
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

    if (speed <= 26000000) {
        val |= AON_CMU_SEL_CODECRS0_OSC | AON_CMU_SEL_CODECRS0_OSCX2;
        val |= AON_CMU_SEL_CODECRS1_OSC | AON_CMU_SEL_CODECRS1_OSCX2;
        speed = 26000000;
    } else if (speed <= 52000000) {
        val |= AON_CMU_SEL_CODECRS0_OSCX2;
        val |= AON_CMU_SEL_CODECRS1_OSCX2;
        speed = 52000000;
    } else {
#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
        if (resample_en) {
            hal_cmu_audio_26m_x4_enable(CMU_AUD_26M_X4_USER_RS);
            val |= AON_CMU_BYPASS_DIV_CODECRS0;
            val |= AON_CMU_BYPASS_DIV_CODECRS1;
            speed = 104000000;
        }
        else
#endif
        {
            // Assume audio stream is one of 48K series
            div = HAL_CMU_AUD_PLL_CLOCK / speed;
            if (div >= 2) {
                hal_cmu_codec_rs_set_div(div);
                speed = HAL_CMU_AUD_PLL_CLOCK / div;
            } else {
                val |= AON_CMU_BYPASS_DIV_CODECRS0;
                val |= AON_CMU_BYPASS_DIV_CODECRS1;
                speed = HAL_CMU_AUD_PLL_CLOCK;
            }
        }
    }

    lock = int_lock();
    aoncmu->CODEC_IIR = (aoncmu->CODEC_IIR & ~mask) | val;
    int_unlock(lock);

    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_CODECRS0_ENABLE | AON_CMU_EN_CLK_CODECRS1_ENABLE;

    aocmu_reg_update_wait();
}

void hal_cmu_codec_rs_disable(void)
{
    uint32_t lock;
    //bool high_speed;

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODECRS0_DISABLE | AON_CMU_EN_CLK_CODECRS1_DISABLE;

#if defined(__AUDIO_RESAMPLE__) && defined(ANA_26M_X4_ENABLE)
    hal_cmu_audio_26m_x4_disable(CMU_AUD_26M_X4_USER_RS);
#endif

    lock = int_lock();
    aoncmu->CODEC_IIR |= AON_CMU_SEL_CODECRS0_OSC | AON_CMU_SEL_CODECRS0_OSCX2 | AON_CMU_SEL_CODECRS1_OSC | AON_CMU_SEL_CODECRS1_OSCX2;
    int_unlock(lock);
}

int hal_cmu_codec_rs_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    aoncmu->CODEC_IIR = (aoncmu->CODEC_IIR & ~(AON_CMU_CFG_DIV_CODECRS0_MASK | AON_CMU_CFG_DIV_CODECRS1_MASK)) |
        AON_CMU_CFG_DIV_CODECRS0(div) | AON_CMU_CFG_DIV_CODECRS1(div);
    int_unlock(lock);

    return 0;
}

void hal_cmu_codec_clock_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CODEC_DIV = (aoncmu->CODEC_DIV & ~(AON_CMU_SEL_CODEC_OSC | AON_CMU_SEL_CODECHCLK_OSCX2 | AON_CMU_SEL_CODECHCLK_PLL)) |
        AON_CMU_SEL_CODEC_ANA_2 | AON_CMU_SEL_CODEC_OSC_2;
    int_unlock(lock);

    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_CODECHCLK_ENABLE | AON_CMU_EN_CLK_CODEC_ENABLE;
    hal_cmu_clock_enable(HAL_CMU_MOD_H_CODEC);
}

void hal_cmu_codec_clock_disable(void)
{
    hal_cmu_clock_disable(HAL_CMU_MOD_H_CODEC);
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODECHCLK_DISABLE | AON_CMU_EN_CLK_CODEC_DISABLE;
}

void hal_cmu_codec_vad_clock_enable(uint32_t enabled)
{
    uint32_t lock;
    lock = int_lock();

    if (enabled) {
        aoncmu->CODEC_DIV |= (AON_CMU_EN_VAD_IIR | AON_CMU_EN_VAD_RS);
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSC_ENABLE | AON_CMU_EN_CLK_VAD32K_ENABLE;
    } else {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_OSC_DISABLE | AON_CMU_EN_CLK_VAD32K_DISABLE;
        aoncmu->CODEC_DIV &= ~(AON_CMU_EN_VAD_IIR | AON_CMU_EN_VAD_RS);
    }
    int_unlock(lock);
}

void hal_cmu_codec_reset_set(void)
{
    aoncmu->RESET_SET = AON_CMU_SOFT_RSTN_CODEC_SET;
}

void hal_cmu_codec_reset_clear(void)
{
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CODEC);
    aoncmu->RESET_CLR = AON_CMU_SOFT_RSTN_CODEC_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_i2s_clock_out_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_I2S0_OUT;
    } else {
        val = CMU_EN_CLK_I2S1_OUT;
    }

    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_EN_CLK_I2S0_OUT;
    } else {
        val = CMU_EN_CLK_I2S1_OUT;
    }

    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_slave_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_CLKIN;
    } else {
        val = CMU_SEL_I2S1_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_set_master_mode(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;

    if (id == HAL_I2S_ID_0) {
        val = CMU_SEL_I2S0_CLKIN;
    } else {
        val = CMU_SEL_I2S1_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK &= ~val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_enable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;
    volatile uint32_t *reg;

    if (id == HAL_I2S_ID_0) {
        val = AON_CMU_EN_CLK_PLL_I2S0;
        reg = &aoncmu->PCM_I2S_CLK;
    } else {
        val = AON_CMU_EN_CLK_PLL_I2S1;
        reg = &aoncmu->SPDIF_CLK;
    }

    lock = int_lock();
    *reg |= val;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_disable(enum HAL_I2S_ID_T id)
{
    uint32_t lock;
    uint32_t val;
    volatile uint32_t *reg;

    if (id == HAL_I2S_ID_0) {
        val = AON_CMU_EN_CLK_PLL_I2S0;
        reg = &aoncmu->PCM_I2S_CLK;
    } else {
        val = AON_CMU_EN_CLK_PLL_I2S1;
        reg = &aoncmu->SPDIF_CLK;
    }

    lock = int_lock();
    *reg &= ~val;
    int_unlock(lock);
}

int hal_cmu_i2s_set_div(enum HAL_I2S_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (AON_CMU_CFG_DIV_I2S0_MASK >> AON_CMU_CFG_DIV_I2S0_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    if (id == HAL_I2S_ID_0) {
        aoncmu->PCM_I2S_CLK = SET_BITFIELD(aoncmu->PCM_I2S_CLK, AON_CMU_CFG_DIV_I2S0, div);
    } else {
        aoncmu->SPDIF_CLK = SET_BITFIELD(aoncmu->SPDIF_CLK, AON_CMU_CFG_DIV_I2S1, div);
    }
    int_unlock(lock);

    return 0;
}

void hal_cmu_pcm_clock_out_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK |= CMU_EN_CLK_PCM_OUT;
    int_unlock(lock);
}

void hal_cmu_pcm_clock_out_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK &= ~CMU_EN_CLK_PCM_OUT;
    int_unlock(lock);
}

void hal_cmu_pcm_set_slave_mode(int clk_pol)
{
    uint32_t lock;
    uint32_t mask;
    uint32_t cfg;

    mask = CMU_SEL_PCM_CLKIN | CMU_POL_CLK_PCM_IN;

    if (clk_pol) {
        cfg = CMU_SEL_PCM_CLKIN | CMU_POL_CLK_PCM_IN;
    } else {
        cfg = CMU_SEL_PCM_CLKIN;
    }

    lock = int_lock();
    cmu->I2C_CLK = (cmu->I2C_CLK & ~mask) | cfg;
    int_unlock(lock);
}

void hal_cmu_pcm_set_master_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->I2C_CLK &= ~CMU_SEL_PCM_CLKIN;
    int_unlock(lock);
}

void hal_cmu_pcm_clock_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->PCM_I2S_CLK |= AON_CMU_EN_CLK_PLL_PCM;
    int_unlock(lock);
}

void hal_cmu_pcm_clock_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->PCM_I2S_CLK &= ~AON_CMU_EN_CLK_PLL_PCM;
    int_unlock(lock);
}

int hal_cmu_pcm_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (AON_CMU_CFG_DIV_PCM_MASK >> AON_CMU_CFG_DIV_PCM_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    aoncmu->PCM_I2S_CLK = SET_BITFIELD(aoncmu->PCM_I2S_CLK, AON_CMU_CFG_DIV_PCM, div);
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_enable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;
    uint32_t mask;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    mask = AON_CMU_EN_CLK_PLL_SPDIF0;

    lock = int_lock();
    aoncmu->SPDIF_CLK |= mask;
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_disable(enum HAL_SPDIF_ID_T id)
{
    uint32_t lock;
    uint32_t mask;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    mask = AON_CMU_EN_CLK_PLL_SPDIF0;

    lock = int_lock();
    aoncmu->SPDIF_CLK &= ~mask;
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
    if ((div & (AON_CMU_CFG_DIV_SPDIF0_MASK >> AON_CMU_CFG_DIV_SPDIF0_SHIFT)) != div) {
        return 2;
    }

    lock = int_lock();
    aoncmu->SPDIF_CLK = SET_BITFIELD(aoncmu->SPDIF_CLK, AON_CMU_CFG_DIV_SPDIF0, div);
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
}
#endif

static uint32_t hal_cmu_usb_get_clock_source(void)
{
    uint32_t src;

#ifdef USB_HIGH_SPEED
    src = CMU_USB_CLK_SRC_PLL_60M;
#else
    src = CMU_USB_CLK_SRC_PLL_48M;
#endif

    return src;
}

void hal_cmu_usb_clock_enable(void)
{
    enum HAL_CMU_PLL_T pll;
    uint32_t lock;
    uint32_t src;
    uint32_t div;

#if defined(USB_HIGH_SPEED) || defined(USB_USE_USBPLL)
    pll = HAL_CMU_PLL_USB;
#else
    pll = HAL_CMU_PLL_BB;
#endif
    src = hal_cmu_usb_get_clock_source();

    hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_USB);

    lock = int_lock();
    if (pll == HAL_CMU_PLL_USB) {
        aoncmu->CLK_SELECT |= AON_CMU_SEL_USB_PLLUSB;
        // USBPLL to dig: 240M
#ifdef USB_HIGH_SPEED
        div = 4;
#else
        div = 5;
#endif
    } else {
        aoncmu->CLK_SELECT &= ~AON_CMU_SEL_USB_PLLUSB;
        // BBPLL to dig: 384M
        div = 8;
    }
    cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_SEL_USB_SRC, src);
    div -= 2;
    aoncmu->CLK_SELECT = SET_BITFIELD(aoncmu->CLK_SELECT, AON_CMU_CFG_DIV_USB, div);
    int_unlock(lock);
#ifndef USB_HIGH_SPEED
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_USB_PLL_ENABLE;
#endif
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
    enum HAL_CMU_PLL_T pll;

#if defined(USB_HIGH_SPEED) || defined(USB_USE_USBPLL)
    pll = HAL_CMU_PLL_USB;
#else
    pll = HAL_CMU_PLL_BB;
#endif

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
#ifndef USB_HIGH_SPEED
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_USB_PLL_DISABLE;
#endif

    hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_USB);
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_apb_init_div(void)
{
    // Divider defaults to 2 (div = reg_val + 2)
    //cmu->SYS_DIV = SET_BITFIELD(cmu->SYS_DIV, CMU_CFG_DIV_PCLK, 0);
}

int hal_cmu_periph_set_div(uint32_t div)
{
    uint32_t lock;
    int ret = 0;

    if (div == 0 || div > ((AON_CMU_CFG_DIV_PER_MASK >> AON_CMU_CFG_DIV_PER_SHIFT) + 2)) {
        ret = 1;
    } else {
        lock = int_lock();
        if (div == 1) {
            aoncmu->CLK_SELECT |= AON_CMU_BYPASS_DIV_PER;
        } else {
            div -= 2;
            aoncmu->CLK_SELECT = (aoncmu->CLK_SELECT & ~(AON_CMU_CFG_DIV_PER_MASK | AON_CMU_BYPASS_DIV_PER)) |
                AON_CMU_CFG_DIV_PER(div);
        }
        int_unlock(lock);
    }

    return ret;
}

#define PERPH_SET_DIV_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_div(uint32_t div) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (div < 2 || div > ((CMU_CFG_DIV_ ##F## _MASK >> CMU_CFG_DIV_ ##F## _SHIFT) + 2)) { \
        cmu->r &= ~(CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F); \
        ret = 1; \
    } else { \
        div -= 2; \
        cmu->r = (cmu->r & ~(CMU_CFG_DIV_ ##F## _MASK)) | CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | \
            CMU_CFG_DIV_ ##F(div); \
        cmu->r |= CMU_EN_PLL_ ##F; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_DIV_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_DIV_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_DIV_FUNC(uart2, UART2, UART_CLK);
PERPH_SET_DIV_FUNC(spi, SPI1, SYS_DIV);
PERPH_SET_DIV_FUNC(slcd, SPI0, SYS_DIV);
PERPH_SET_DIV_FUNC(sdmmc, SDMMC, PERIPH_CLK);
PERPH_SET_DIV_FUNC(i2c, I2C, I2C_CLK);

#define PERPH_SET_FREQ_FUNC(f, F, r) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (freq == HAL_CMU_PERIPH_FREQ_26M) { \
        cmu->r &= ~(CMU_SEL_OSCX2_ ##F | CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F); \
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) { \
        cmu->r = (cmu->r & ~(CMU_SEL_PLL_ ##F | CMU_EN_PLL_ ##F)) | CMU_SEL_OSCX2_ ##F; \
    } else { \
        ret = 1; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_FREQ_FUNC(uart0, UART0, UART_CLK);
PERPH_SET_FREQ_FUNC(uart1, UART1, UART_CLK);
PERPH_SET_FREQ_FUNC(uart2, UART2, UART_CLK);
PERPH_SET_FREQ_FUNC(spi, SPI1, SYS_DIV);
PERPH_SET_FREQ_FUNC(slcd, SPI0, SYS_DIV);
PERPH_SET_FREQ_FUNC(sdmmc, SDMMC, PERIPH_CLK);
PERPH_SET_FREQ_FUNC(i2c, I2C, I2C_CLK);

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        cmu->SYS_DIV &= ~CMU_SEL_OSCX2_SPI2;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        cmu->SYS_DIV |= CMU_SEL_OSCX2_SPI2;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

int hal_cmu_clock_out_enable(enum HAL_CMU_CLOCK_OUT_ID_T id)
{
    uint32_t lock;
    uint32_t sel;
    uint32_t cfg;

    enum CMU_CLK_OUT_SEL_T {
        CMU_CLK_OUT_SEL_AON     = 0,
        CMU_CLK_OUT_SEL_CODEC   = 1,
        CMU_CLK_OUT_SEL_BT      = 2,
        CMU_CLK_OUT_SEL_MCU     = 3,

        CMU_CLK_OUT_SEL_QTY
    };

    sel = CMU_CLK_OUT_SEL_QTY;
    cfg = 0;

    if (id <= HAL_CMU_CLOCK_OUT_AON_SYS) {
        sel = CMU_CLK_OUT_SEL_AON;
        cfg = id - HAL_CMU_CLOCK_OUT_AON_32K;
    } else if (HAL_CMU_CLOCK_OUT_MCU_32K <= id && id <= HAL_CMU_CLOCK_OUT_MCU_SPI1) {
        sel = CMU_CLK_OUT_SEL_MCU;
        lock = int_lock();
        cmu->PERIPH_CLK = SET_BITFIELD(cmu->PERIPH_CLK, CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_MCU_32K);
        int_unlock(lock);
    } else if (HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA <= id && id <= HAL_CMU_CLOCK_OUT_CODEC_HCLK) {
        sel = CMU_CLK_OUT_SEL_CODEC;
        hal_codec_select_clock_out(id - HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA);
    } else if (HAL_CMU_CLOCK_OUT_BT_32K <= id && id <= HAL_CMU_CLOCK_OUT_BT_26M) {
        sel = CMU_CLK_OUT_SEL_BT;
        btcmu->CLK_OUT = SET_BITFIELD(btcmu->CLK_OUT, BT_CMU_CFG_CLK_OUT, id - HAL_CMU_CLOCK_OUT_BT_32K);
    }

    if (sel < CMU_CLK_OUT_SEL_QTY) {
        lock = int_lock();
        aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~(AON_CMU_SEL_CLK_OUT_MASK | AON_CMU_CFG_CLK_OUT_MASK)) |
            AON_CMU_SEL_CLK_OUT(sel) | AON_CMU_CFG_CLK_OUT(cfg) | AON_CMU_EN_CLK_OUT;
        int_unlock(lock);

        return 0;
    }

    return 1;
}

void hal_cmu_clock_out_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CLK_OUT &= ~AON_CMU_EN_CLK_OUT;
    int_unlock(lock);
}

int hal_cmu_i2s_mclk_enable(enum HAL_CMU_I2S_MCLK_ID_T id)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CODEC_DIV = SET_BITFIELD(aoncmu->CODEC_DIV, AON_CMU_SEL_I2S_MCLK, id) | AON_CMU_EN_I2S_MCLK;
    int_unlock(lock);

    return 0;
}

void hal_cmu_i2s_mclk_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    aoncmu->CODEC_DIV &= ~AON_CMU_EN_I2S_MCLK;
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
        if ((div & (AON_CMU_CFG_DIV_PWM0_MASK >> AON_CMU_CFG_DIV_PWM0_SHIFT)) != div) {
            return 1;
        }
    }

    lock = int_lock();
    if (id == HAL_PWM_ID_0) {
        aoncmu->PWM01_CLK = (aoncmu->PWM01_CLK & ~(AON_CMU_CFG_DIV_PWM0_MASK | AON_CMU_SEL_PWM0_OSC | AON_CMU_EN_CLK_PWM0_OSC)) |
            AON_CMU_CFG_DIV_PWM0(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM0_OSC | AON_CMU_EN_CLK_PWM0_OSC));
    } else if (id == HAL_PWM_ID_1) {
        aoncmu->PWM01_CLK = (aoncmu->PWM01_CLK & ~(AON_CMU_CFG_DIV_PWM1_MASK | AON_CMU_SEL_PWM1_OSC | AON_CMU_EN_CLK_PWM1_OSC)) |
            AON_CMU_CFG_DIV_PWM1(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM1_OSC | AON_CMU_EN_CLK_PWM1_OSC));
    } else if (id == HAL_PWM_ID_2) {
        aoncmu->PWM23_CLK = (aoncmu->PWM23_CLK & ~(AON_CMU_CFG_DIV_PWM2_MASK | AON_CMU_SEL_PWM2_OSC | AON_CMU_EN_CLK_PWM2_OSC)) |
            AON_CMU_CFG_DIV_PWM2(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM2_OSC | AON_CMU_EN_CLK_PWM2_OSC));
    } else {
        aoncmu->PWM23_CLK = (aoncmu->PWM23_CLK & ~(AON_CMU_CFG_DIV_PWM3_MASK | AON_CMU_SEL_PWM3_OSC | AON_CMU_EN_CLK_PWM3_OSC)) |
            AON_CMU_CFG_DIV_PWM3(div) | (clk_32k ? 0 : (AON_CMU_SEL_PWM3_OSC | AON_CMU_EN_CLK_PWM3_OSC));
    }
    int_unlock(lock);
    return 0;
}

void hal_cmu_jtag_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->MCU_TIMER &= ~(CMU_SECURE_BOOT_JTAG | CMU_SECURE_BOOT_I2C);
    int_unlock(lock);
}

void hal_cmu_jtag_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->MCU_TIMER |= (CMU_SECURE_BOOT_JTAG | CMU_SECURE_BOOT_I2C);
    int_unlock(lock);
}

void hal_cmu_jtag_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_JTAG_ENABLE;
}

void hal_cmu_jtag_clock_disable(void)
{
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_JTAG_DISABLE;
}

void hal_cmu_rom_clock_init(void)
{
    aoncmu->CODEC_DIV = (aoncmu->CODEC_DIV & ~AON_CMU_SEL_AON_OSCX2) | AON_CMU_SEL_AON_OSC;
    // Enable PMU fast clock
    //aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~AON_CMU_SEL_DCDC_PLL) | AON_CMU_SEL_DCDC_OSC | AON_CMU_BYPASS_DIV_DCDC | AON_CMU_EN_CLK_DCDC0;
}

void hal_cmu_init_chip_feature(uint16_t feature)
{
    aoncmu->CHIP_FEATURE = feature | AON_CMU_EFUSE_LOCK;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x2_enable(void)
{
    // Debug Select CMU REG F4
    cmu->MCU_TIMER = SET_BITFIELD(cmu->MCU_TIMER, CMU_DEBUG_REG_SEL, 7);
    // Power on OSCX2
    aoncmu->PLL_ENABLE = AON_CMU_PU_OSCX2_ENABLE;
    // Disable DIG OSCX2
    aoncmu->CLK_SELECT &= ~AON_CMU_SEL_OSCX2_DIG;
    // Enable OSCX2
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX2_ENABLE | AON_CMU_EN_CLK_MCU_OSCX2_ENABLE;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x4_enable(void)
{
#ifdef ANA_26M_X4_ENABLE
    // Power on OSCX4
    aoncmu->PLL_ENABLE = AON_CMU_PU_OSCX4_ENABLE;
    // Disable DIG OSCX4
    aoncmu->CLK_SELECT &= ~AON_CMU_SEL_OSCX4_DIG;
    // Enable OSCX4
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX4_ENABLE;
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_module_init_state(void)
{
    aoncmu->CODEC_DIV = (aoncmu->CODEC_DIV & ~AON_CMU_SEL_AON_OSCX2) | AON_CMU_SEL_AON_OSC;
    // Slow down PMU fast clock
    //aoncmu->CLK_OUT = (aoncmu->CLK_OUT & ~(AON_CMU_BYPASS_DIV_DCDC | AON_CMU_CFG_DIV_DCDC_MASK)) | AON_CMU_CFG_DIV_DCDC(2);

    // DMA channel config
    cmu->ADMA_CH0_4_REQ =
        CMU_ADMA_CH0_REQ_IDX(CODEC_DMA_RX_REQ) | CMU_ADMA_CH1_REQ_IDX(CODEC_DMA_TX_REQ) |
        CMU_ADMA_CH2_REQ_IDX(DSD_DMA_RX_REQ) | CMU_ADMA_CH3_REQ_IDX(DSD_DMA_TX_REQ) |
        CMU_ADMA_CH4_REQ_IDX(CODEC_DMA_TX2_REQ);
    cmu->ADMA_CH5_9_REQ =
        CMU_ADMA_CH5_REQ_IDX(I2C1_DMA_RX_REQ) | CMU_ADMA_CH6_REQ_IDX(I2C1_DMA_TX_REQ) |
        CMU_ADMA_CH7_REQ_IDX(NULL_DMA_REQ) | CMU_ADMA_CH8_REQ_IDX(NULL_DMA_REQ) |
        CMU_ADMA_CH9_REQ_IDX(NULL_DMA_REQ);
    cmu->ADMA_CH10_14_REQ =
        CMU_ADMA_CH10_REQ_IDX(PCM_DMA_RX_REQ) | CMU_ADMA_CH11_REQ_IDX(PCM_DMA_TX_REQ) |
        CMU_ADMA_CH12_REQ_IDX(BTDUMP_DMA_REQ) | CMU_ADMA_CH13_REQ_IDX(SDEMMC_DMA_REQ) |
        CMU_ADMA_CH14_REQ_IDX(UART2_DMA_RX_REQ);
    cmu->ADMA_CH15_REQ =
        CMU_ADMA_CH15_REQ_IDX(UART2_DMA_TX_REQ);

    cmu->GDMA_CH0_4_REQ =
        CMU_GDMA_CH0_REQ_IDX(SPILCD0_DMA_RX_REQ) | CMU_GDMA_CH1_REQ_IDX(SPILCD0_DMA_TX_REQ) |
        CMU_GDMA_CH2_REQ_IDX(SPI_ITN_DMA_RX_REQ) | CMU_GDMA_CH3_REQ_IDX(SPI_ITN_DMA_TX_REQ) |
        CMU_GDMA_CH4_REQ_IDX(UART0_DMA_RX_REQ);
    cmu->GDMA_CH5_9_REQ =
        CMU_GDMA_CH5_REQ_IDX(UART0_DMA_TX_REQ) | CMU_GDMA_CH6_REQ_IDX(UART1_DMA_RX_REQ) |
        CMU_GDMA_CH7_REQ_IDX(UART1_DMA_TX_REQ) | CMU_GDMA_CH8_REQ_IDX(NULL_DMA_REQ) |
        CMU_GDMA_CH9_REQ_IDX(NULL_DMA_REQ);
    cmu->GDMA_CH10_14_REQ =
        CMU_GDMA_CH10_REQ_IDX(I2S0_DMA_RX_REQ) | CMU_GDMA_CH11_REQ_IDX(I2S0_DMA_TX_REQ) |
        CMU_GDMA_CH12_REQ_IDX(SPDIF0_DMA_RX_REQ) | CMU_GDMA_CH13_REQ_IDX(SPDIF0_DMA_TX_REQ) |
        CMU_GDMA_CH14_REQ_IDX(I2C0_DMA_RX_REQ);
    cmu->GDMA_CH15_REQ =
        CMU_GDMA_CH15_REQ_IDX(I2C0_DMA_TX_REQ);

#ifndef SIMU
    cmu->ORESET_SET = SYS_ORST_USB | SYS_ORST_USB32K | SYS_ORST_PSRAM1G | SYS_ORST_PSRAM200 | SYS_ORST_SDMMC |
        SYS_ORST_WDT | SYS_ORST_TIMER2 | SYS_ORST_I2C0 | SYS_ORST_I2C1 | SYS_ORST_SPI | SYS_ORST_SLCD | SYS_ORST_SPI_PHY |
        SYS_ORST_UART0 | SYS_ORST_UART1 | SYS_ORST_UART2 | SYS_ORST_PCM | SYS_ORST_I2S0 | SYS_ORST_SPDIF0 |
        SYS_ORST_I2S1 | SYS_ORST_A7 | SYS_ORST_TSF | SYS_ORST_WDT_AP | SYS_ORST_TIMER0_AP | SYS_ORST_TIMER1_AP;
    cmu->PRESET_SET = SYS_PRST_WDT | SYS_PRST_TIMER2 | SYS_PRST_I2C0 | SYS_PRST_I2C1 |
        SYS_PRST_SPI | SYS_PRST_SLCD | SYS_PRST_SPI_PHY |
        SYS_PRST_UART0 | SYS_PRST_UART1 | SYS_PRST_UART2 |
        SYS_PRST_PCM | SYS_PRST_I2S0 | SYS_PRST_SPDIF0 | SYS_PRST_TQWF | SYS_PRST_TQA7 |
        SYS_PRST_TRNG | SYS_PRST_BCM | SYS_PRST_TZC;
    cmu->HRESET_SET = SYS_HRST_CORE1 | SYS_HRST_CACHE1 | SYS_HRST_BCM | SYS_HRST_USBC | SYS_HRST_USBH | SYS_HRST_CODEC |
        SYS_HRST_PSRAM1G | SYS_HRST_PSRAM200 | SYS_HRST_BT_DUMP | SYS_HRST_WF_DUMP | SYS_HRST_SDMMC |
        SYS_HRST_CHECKSUM | SYS_HRST_CRC;
    cmu->XRESET_SET = SYS_XRST_DMA | SYS_XRST_NIC | SYS_XRST_IMEMLO | SYS_XRST_IMEMHI | SYS_XRST_PSRAM1G | SYS_XRST_PER |
        SYS_XRST_PDBG | SYS_XRST_CORE0 | SYS_XRST_CORE1 | SYS_XRST_CORE2 | SYS_XRST_CORE3 | SYS_XRST_DBG | SYS_XRST_SCU;
    cmu->APRESET_SET = SYS_APRST_BOOTREG | SYS_APRST_WDT| SYS_APRST_TIMER0 | SYS_APRST_TIMER1 | SYS_APRST_TQ | SYS_APRST_DAP;

    cmu->OCLK_DISABLE = SYS_OCLK_USB | SYS_OCLK_USB32K | SYS_OCLK_PSRAM1G | SYS_OCLK_PSRAM200 | SYS_OCLK_SDMMC |
        SYS_OCLK_WDT | SYS_OCLK_TIMER2 | SYS_OCLK_I2C0 | SYS_OCLK_I2C1 | SYS_OCLK_SPI | SYS_OCLK_SLCD | SYS_OCLK_SPI_PHY |
        SYS_OCLK_UART0 | SYS_OCLK_UART1 | SYS_OCLK_UART2 | SYS_OCLK_PCM | SYS_OCLK_I2S0 | SYS_OCLK_SPDIF0 |
        SYS_OCLK_I2S1 | SYS_OCLK_A7 | SYS_OCLK_TSF | SYS_OCLK_WDT_AP | SYS_OCLK_TIMER0_AP | SYS_OCLK_TIMER1_AP;
    cmu->PCLK_DISABLE = SYS_PCLK_WDT | SYS_PCLK_TIMER2 | SYS_PCLK_I2C0 | SYS_PCLK_I2C1 |
        SYS_PCLK_SPI | SYS_PCLK_SLCD | SYS_PCLK_SPI_PHY |
        SYS_PCLK_UART0 | SYS_PCLK_UART1 | SYS_PCLK_UART2 |
        SYS_PCLK_PCM | SYS_PCLK_I2S0 | SYS_PCLK_SPDIF0 | SYS_PCLK_TQWF | SYS_PCLK_TQA7 |
        SYS_PCLK_TRNG | SYS_PCLK_BCM | SYS_PCLK_TZC;
    cmu->HCLK_DISABLE = SYS_HCLK_CORE1 | SYS_HCLK_CACHE1 | SYS_HCLK_BCM | SYS_HCLK_USBC | SYS_HCLK_USBH | SYS_HCLK_CODEC |
        SYS_HCLK_PSRAM1G | SYS_HCLK_PSRAM200 | SYS_HCLK_BT_DUMP | SYS_HCLK_WF_DUMP | SYS_HCLK_SDMMC |
        SYS_HCLK_CHECKSUM | SYS_HCLK_CRC;
    cmu->XCLK_DISABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PSRAM1G | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU;
    cmu->APCLK_DISABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;

    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_CODEC_DISABLE | AON_CMU_EN_CLK_CODECIIR_DISABLE | AON_CMU_EN_CLK_CODECRS0_DISABLE |
        AON_CMU_EN_CLK_CODECRS1_DISABLE | AON_CMU_EN_CLK_CODECHCLK_DISABLE | AON_CMU_EN_CLK_VAD32K_DISABLE |
        AON_CMU_EN_CLK_BT_DISABLE | AON_CMU_EN_CLK_WF_DISABLE;

    aoncmu->RESET_SET = AON_CMU_ARESETN_SET(AON_ARST_PWM) |
        AON_CMU_ORESETN_SET(AON_ORST_PWM0 | AON_ORST_PWM1 | AON_ORST_PWM2 | AON_ORST_PWM3) |
        AON_CMU_SOFT_RSTN_CODEC_SET |
        AON_CMU_SOFT_RSTN_A7_SET | AON_CMU_SOFT_RSTN_A7CPU_SET |
        AON_CMU_SOFT_RSTN_WF_SET | AON_CMU_SOFT_RSTN_WFCPU_SET |
        AON_CMU_SOFT_RSTN_BT_SET | AON_CMU_SOFT_RSTN_BTCPU_SET;

    aoncmu->MOD_CLK_DISABLE = AON_CMU_MANUAL_ACLK_DISABLE(AON_ACLK_PWM) |
        AON_CMU_MANUAL_OCLK_DISABLE(AON_OCLK_PWM0 | AON_OCLK_PWM1 | AON_OCLK_PWM2 | AON_OCLK_PWM3);

    //aoncmu->MOD_CLK_MODE = 0;
    //cmu->HCLK_MODE = 0;
    //cmu->PCLK_MODE = SYS_PCLK_UART0 | SYS_PCLK_UART1 | SYS_PCLK_UART2;
    //cmu->OCLK_MODE = 0;
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_ema_init(void)
{
    // Never change EMA
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
        cmu->HCLK_MODE &= ~(SYS_HCLK_CORE0 | SYS_HCLK_ROM0 | SYS_HCLK_RAM0 | SYS_HCLK_RAM1_2 |
            SYS_HCLK_RAM3_6 | SYS_HCLK_RAM7 | SYS_HCLK_RAM8 | SYS_HCLK_RAM9);
        // AON_CMU enable auto switch 26M (AON_CMU must have selected 26M and disabled 52M/32K already)
        aoncmu->CODEC_DIV |= AON_CMU_LPU_AUTO_SWITCH26;
    } else {
        // AON_CMU disable auto switch 26M
        aoncmu->CODEC_DIV &= ~AON_CMU_LPU_AUTO_SWITCH26;
    }

#ifdef LARGE_RAM
    //M33 use ram0-ram9
    lpu_clk |= CMU_CFG_SRAM_IN_M33(0x1f);
#else
    //M33 use ram0-ram2
    lpu_clk |= CMU_CFG_SRAM_IN_M33(0x1);
#endif

    lpu_clk |= CMU_TIMER_WT26(timer_26m);
    if (timer_pll) {
        lpu_clk |= CMU_TIMER_WTPLL(timer_pll);
        hal_sys_timer_delay(US_TO_TICKS(60));
        cmu->WAKEUP_CLK_CFG = lpu_clk;
    } else {
        lpu_clk |= CMU_TIMER_WTPLL(0);
        cmu->WAKEUP_CLK_CFG = lpu_clk;
    }
    return 0;
}

int SRAM_TEXT_LOC hal_cmu_lpu_sleep(enum HAL_CMU_LPU_SLEEP_MODE_T mode)
{
    uint32_t start;
    uint32_t timeout;
    uint32_t saved_pll_cfg;
    uint32_t saved_clk_cfg;
    uint32_t saved_codec_div;
    uint32_t wakeup_cfg;
    uint32_t saved_hclk;
    uint32_t saved_oclk;

    saved_codec_div = aoncmu->CODEC_DIV;
    saved_pll_cfg = aoncmu->PLL_ENABLE;
    saved_clk_cfg = cmu->SYS_CLK_ENABLE;
    saved_hclk = cmu->HCLK_ENABLE;
    saved_oclk = cmu->OCLK_ENABLE;

    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        wakeup_cfg = cmu->WAKEUP_CLK_CFG;
    } else {
        wakeup_cfg = 0;
    }

#ifndef ROM_BUILD
    // Reset pll div if pll is enabled
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_USB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_DDR);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_DSP);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_BB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        pmu_pll_div_reset_set(HAL_CMU_PLL_BB_PSRAM);
    }
#endif

    // Setup wakeup mask
#ifdef __ARM_ARCH_ISA_ARM
    cmu->WAKEUP_MASK0 = GICDistributor->ISENABLER[0];
    cmu->WAKEUP_MASK1 = GICDistributor->ISENABLER[1];
    cmu->WAKEUP_MASK2 = GICDistributor->ISENABLER[2];
#else
    cmu->WAKEUP_MASK0 = NVIC->ISER[0];
    cmu->WAKEUP_MASK1 = NVIC->ISER[1];
    cmu->WAKEUP_MASK2 = NVIC->ISER[2];
#endif

    // Disable memory/flash freq
    cmu->OCLK_DISABLE = SYS_OCLK_PSRAM1G | SYS_OCLK_PSRAM200 | SYS_OCLK_FLASH;
    cmu->HCLK_DISABLE = SYS_HCLK_PSRAM1G | SYS_HCLK_PSRAM200 | SYS_HCLK_FLASH;

    // Switch system freq to 26M
    cmu->SYS_CLK_ENABLE = CMU_SEL_MCU_SLOW_ENABLE;
    cmu->SYS_CLK_DISABLE = CMU_SEL_MCU_OSC_4_DISABLE | CMU_SEL_MCU_OSC_2_DISABLE |
        CMU_SEL_MCU_OSCX4_DISABLE | CMU_SEL_MCU_FAST_DISABLE | CMU_SEL_MCU_PLL_DISABLE;
    cmu->SYS_CLK_DISABLE = CMU_RSTN_DIV_MCU_DISABLE;
    // Shutdown PLLs
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLUSB_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLUSB_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLDDR_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLDDR_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLA7_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLA7_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_PU_PLLBB_DIV_MCU_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLDDR_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_PLLBB_PS_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLBB_DIV_PS_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_ENABLE) {
        aoncmu->PLL_DISABLE = AON_CMU_PU_PLLBB_DISABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_OSCX4_ENABLE) {
        aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_TOP_OSCX4_DISABLE;
        aoncmu->PLL_DISABLE = AON_CMU_PU_OSCX4_DISABLE;
    }

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        // Do nothing
        // Hardware will switch system freq to 32K and shutdown PLLs automatically
    } else {
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            // Do nothing
            // Hardware will switch system freq to 32K automatically
        } else {
            // Manually switch AON_CMU clock to 32K
            aoncmu->CODEC_DIV = saved_codec_div & ~(AON_CMU_SEL_AON_OSC | AON_CMU_SEL_AON_OSCX2);
            // Switch system freq to 32K
            cmu->SYS_CLK_DISABLE = CMU_SEL_MCU_SLOW_DISABLE;
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

#ifndef __ARM_ARCH_ISA_ARM
    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
    } else {
        SCB->SCR = 0;
    }
#endif
    __DSB();
    __WFI();

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_PLL_LOCKED_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
        while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_PLL) == 0 &&
            (hal_sys_timer_get() - start) < timeout);
        // Hardware will switch system to PLL divider and enable PLLs automatically
    } else {
        // Wait for 26M ready
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26) {
            start = hal_sys_timer_get();
            timeout = HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_LPU_EXTRA_TIMEOUT;
            while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_STATUS_26M) == 0 &&
                (hal_sys_timer_get() - start) < timeout);
            // Hardware will switch system freq to 26M automatically
        } else {
            if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
                timeout = HAL_CMU_26M_READY_TIMEOUT;
                hal_sys_timer_delay(timeout);
            }
            // Switch system freq to 26M
            cmu->SYS_CLK_ENABLE = CMU_SEL_MCU_SLOW_ENABLE;
            // Restore AON_CMU clock
            aoncmu->CODEC_DIV = saved_codec_div;
        }
    }

    // System freq is 26M now and will be restored later
    // Restore PLLs
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLUSB_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLDDR_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLA7_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_MCU_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_PLLBB_ENABLE | AON_CMU_PU_PLLBB_DIV_PS_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_OSCX4_ENABLE) {
        aoncmu->PLL_ENABLE = AON_CMU_PU_OSCX4_ENABLE;
    }
#ifndef ROM_BUILD
    hal_sys_timer_delay_us(20);
    // Clear pll div reset if pll is enabled
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_USB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_DDR);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_DSP);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_BB);
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        pmu_pll_div_reset_clear(HAL_CMU_PLL_BB_PSRAM);
    }
    hal_sys_timer_delay_us(10);
#endif
    if (saved_pll_cfg & AON_CMU_PU_PLLUSB_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLUSB_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLDDR_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLDDR_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLA7_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLA7_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_MCU_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLBB_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_PLLBB_DIV_PS_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_PLLBB_PS_ENABLE;
    }
    if (saved_pll_cfg & AON_CMU_PU_OSCX4_ENABLE) {
        aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_TOP_OSCX4_ENABLE;
    }
    // Restore system freq
    cmu->SYS_CLK_ENABLE = saved_clk_cfg & CMU_RSTN_DIV_MCU_ENABLE;
    cmu->SYS_CLK_ENABLE = saved_clk_cfg;
    // The original system freq are at least 26M
    //cmu->SYS_CLK_DISABLE = ~saved_clk_cfg;

    // Restore memory/flash freq
    cmu->HCLK_ENABLE = saved_hclk;
    cmu->OCLK_ENABLE = saved_oclk;

    if (mode == HAL_CMU_LPU_SLEEP_MODE_CHIP) {
        // Wait at least 100us until the voltages become stable
        hal_sys_timer_delay_us(100);
    }

    return 0;
}

volatile uint32_t *hal_cmu_get_bootmode_addr(void)
{
    return &aoncmu->BOOTMODE;
}

SRAM_TEXT_LOC
volatile uint32_t *hal_cmu_get_memsc_addr(void)
{
    return &aoncmu->MEMSC[0];
}

void hal_cmu_bt_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_BT_ENABLE;
    aocmu_reg_update_wait();
}

void hal_cmu_bt_clock_disable(void)
{
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_BT_DISABLE;
}

void hal_cmu_bt_reset_set(void)
{
    aoncmu->RESET_SET = AON_CMU_SOFT_RSTN_BT_SET | AON_CMU_SOFT_RSTN_BTCPU_SET;
}

void hal_cmu_bt_reset_clear(void)
{
    aoncmu->RESET_CLR = AON_CMU_SOFT_RSTN_BT_CLR | AON_CMU_SOFT_RSTN_BTCPU_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_bt_module_init(void)
{
    //btcmu->CLK_MODE = 0;
}

void hal_cmu_wifi_clock_enable(void)
{
    aoncmu->TOP_CLK_ENABLE = AON_CMU_EN_CLK_WF_ENABLE;
    aocmu_reg_update_wait();
}

void hal_cmu_wifi_clock_disable(void)
{
    aoncmu->TOP_CLK_DISABLE = AON_CMU_EN_CLK_WF_DISABLE;
}

void hal_cmu_wifi_reset_set(void)
{
    aoncmu->RESET_SET = AON_CMU_SOFT_RSTN_WF_SET | AON_CMU_SOFT_RSTN_WFCPU_SET;
}

void hal_cmu_wifi_reset_clear(void)
{
    aoncmu->RESET_CLR = AON_CMU_SOFT_RSTN_WF_CLR | AON_CMU_SOFT_RSTN_WFCPU_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_dsp_clock_enable(void)
{
    enum HAL_CMU_PLL_T dsp;

    // DSP AXI clock divider defaults to 4 (div = reg_val + 2)
    cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_XCLK, 2);
#ifdef WORKAROUND_DSP_TIMER_BUG
    cmu->DSP_DIV = SET_BITFIELD(cmu->DSP_DIV, CMU_CFG_DIV_APCLK, 0);
#endif
    cmu->XCLK_ENABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PSRAM1G | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU;
    cmu->APCLK_ENABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;

#ifdef DSP_USE_BBPLL
    dsp = HAL_CMU_PLL_BB;
#elif defined(DSP_USE_USBPLL)
    dsp = HAL_CMU_PLL_USB;
#else
    dsp = HAL_CMU_PLL_DSP;
#endif
    hal_cmu_dsp_select_pll(dsp);
    hal_cmu_pll_enable(dsp, HAL_CMU_PLL_USER_DSP);

    hal_cmu_dsp_set_freq(HAL_CMU_FREQ_780M);

    hal_cmu_clock_enable(HAL_CMU_MOD_O_A7);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_WDT_AP);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_TIMER0_AP);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_TIMER1_AP);
}

void hal_cmu_dsp_clock_disable(void)
{
    enum HAL_CMU_PLL_T dsp;

    hal_cmu_clock_disable(HAL_CMU_MOD_O_A7);

#ifdef DSP_USE_BBPLL
    dsp = HAL_CMU_PLL_BB;
#elif defined(DSP_USE_USBPLL)
    dsp = HAL_CMU_PLL_USB;
#else
    dsp = HAL_CMU_PLL_DSP;
#endif
    hal_cmu_pll_disable(dsp, HAL_CMU_PLL_USER_DSP);

    cmu->XCLK_DISABLE = SYS_XCLK_DMA | SYS_XCLK_NIC | SYS_XCLK_IMEMLO | SYS_XCLK_IMEMHI | SYS_XCLK_PSRAM1G | SYS_XCLK_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU;
    cmu->APCLK_DISABLE = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
}

void hal_cmu_dsp_reset_set(void)
{
    aoncmu->RESET_SET = AON_CMU_SOFT_RSTN_A7_SET | AON_CMU_SOFT_RSTN_A7CPU_SET;
    cmu->XRESET_SET = SYS_XRST_DMA | SYS_XRST_NIC | SYS_XRST_IMEMLO | SYS_XRST_IMEMHI | SYS_XRST_PSRAM1G | SYS_XRST_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU;
    cmu->APRESET_SET = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
}

void hal_cmu_dsp_reset_clear(void)
{
    cmu->APRESET_CLR = SYS_APCLK_BOOTREG | SYS_APCLK_WDT| SYS_APCLK_TIMER0 | SYS_APCLK_TIMER1 | SYS_APCLK_TQ | SYS_APCLK_DAP;
    cmu->XRESET_CLR = SYS_XRST_DMA | SYS_XRST_NIC | SYS_XRST_IMEMLO | SYS_XRST_IMEMHI | SYS_XRST_PSRAM1G | SYS_XRST_PER |
        SYS_XCLK_PDBG | SYS_XCLK_CORE0 | SYS_XCLK_CORE1 | SYS_XCLK_CORE2 | SYS_XCLK_CORE3 | SYS_XCLK_DBG | SYS_XCLK_SCU;
    aoncmu->RESET_CLR = AON_CMU_SOFT_RSTN_A7_CLR;
    aocmu_reg_update_wait();
}

void hal_cmu_dsp_init_boot_reg(uint32_t entry)
{
    int i;
    volatile uint32_t *boot = (volatile uint32_t *)DSP_BOOT_REG;

    // Unlock
    boot[32] = 0xCAFE0001;
    __DMB();

    for (i = 0; i < 8; i++) {
        // ldr pc, [pc, #24]
        boot[0 + i] = 0xE59FF018;
        // Init_Handler
        boot[8 + i] = 0x00000040;
    }
    // b   40
    boot[16] = 0xEAFFFFFE;

    // Update reset handler
    boot[8] = entry;

    // Lock
    __DMB();
    boot[32] = 0xCAFE0000;
    __DMB();
}

void hal_cmu_dsp_start_cpu(void)
{
    aoncmu->RESET_CLR = AON_CMU_SOFT_RSTN_A7CPU_CLR;
}

uint32_t hal_cmu_get_aon_chip_id(void)
{
    return aoncmu->CHIP_ID;
}

uint32_t hal_cmu_get_aon_revision_id(void)
{
    return GET_BITFIELD(aoncmu->CHIP_ID, AON_CMU_REVISION_ID);
}

void hal_cmu_dma_dsd_enable(void)
{
}

void hal_cmu_dma_dsd_disable(void)
{
}

void hal_cmu_cp_enable(uint32_t sp, uint32_t entry)
{
    struct CP_STARTUP_CFG_T * cp_cfg;
    uint32_t cfg_addr;

    // Use (sp - 128) as the default vector. The Address must aligned to 128-byte boundary.
    cfg_addr = (sp - (1 << 7)) & ~((1 << 7) - 1);

    cmu->CP_VTOR = cfg_addr;
    cp_cfg = (struct CP_STARTUP_CFG_T *)cfg_addr;

    cp_cfg->stack = sp;
    cp_cfg->reset_hdlr = (uint32_t)system_cp_reset_handler;
    cp_entry = entry;

    hal_cmu_clock_enable(HAL_CMU_MOD_H_CACHE1);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CACHE1);

    hal_cmu_clock_enable(HAL_CMU_MOD_H_CP);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_CP);
}

void hal_cmu_cp_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_H_CP);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_CP);
}

uint32_t hal_cmu_cp_get_entry_addr(void)
{
    return cp_entry;
}

void hal_cmu_dsp_setup(void)
{
    hal_cmu_dsp_timer0_select_slow();
#ifdef TIMER1_BASE
    hal_cmu_dsp_timer1_select_fast();
#endif
    hal_sys_timer_open();

    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_780M);

}
