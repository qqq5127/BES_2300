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
#include <stdint.h>
#include "stdbool.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_chipid.h"
#include "hal_location.h"
#include "analog.h"
#include "pmu.h"
#include "usbphy_best3003.h"
#include "reg_analog_best3003.h"

#define PU_TX_REGULATOR
#define ANALOG_TRACE(n, s, ...)             //TRACE(n, s, ##__VA_ARGS__)

enum ANA_CODEC_USER_T {
    ANA_CODEC_USER_DAC,
    ANA_CODEC_USER_ADC,

    ANA_CODEC_USER_CODEC,
    ANA_CODEC_USER_MICKEY,

    ANA_CODEC_USER_ANC_FF,
    ANA_CODEC_USER_ANC_FB,

    ANA_CODEC_USER_QTY
};

typedef uint8_t CODEC_USER_MAP;
STATIC_ASSERT(sizeof(CODEC_USER_MAP) * 8 >= ANA_CODEC_USER_QTY, "Invalid codec user map type size");

struct ANALOG_PLL_CFG_T {
    uint32_t freq;
    uint8_t div;
    uint64_t val;
};

#define DEFAULT_ANC_FF_ADC_GAIN_DB          6
#define DEFAULT_ANC_FB_ADC_GAIN_DB          6
#define DEFAULT_VOICE_ADC_GAIN_DB           12

#ifndef ANALOG_ADC_A_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH0) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH0))
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH0) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH0))
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_B_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH1) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH1))
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH1) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH1))
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_C_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH2) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH2))
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH2) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH2))
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_D_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH3) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH3))
#define ANALOG_ADC_D_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH3) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH3))
#define ANALOG_ADC_D_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_D_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_E_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH4) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH4))
#define ANALOG_ADC_E_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH4) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH4))
#define ANALOG_ADC_E_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_E_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef LINEIN_ADC_GAIN_DB
#define LINEIN_ADC_GAIN_DB                  0
#endif

#ifndef MAX_ANA_MIC_CH_NUM
#define MAX_ANA_MIC_CH_NUM                  5
#endif

#ifndef ANC_VMIC_CFG
#define ANC_VMIC_CFG                        (AUD_VMIC_MAP_VMIC1)
#endif

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

#define TX_PA_GAIN_0DB_DIFF                 0x1
#define TX_PA_GAIN_0DB_SE                   0x2

#ifdef AUDIO_OUTPUT_DIFF
#define DEFAULT_TX_PA_GAIN                  TX_PA_GAIN_0DB_DIFF
#else   // AUDIO_OUTPUT_DIFF
#define DEFAULT_TX_PA_GAIN                  TX_PA_GAIN_0DB_SE
#endif  // AUDIO_OUTPUT_DIFF

static OPT_TYPE uint16_t vcodec_mv =
#ifdef VCODEC_1P5V
    1500;
#elif defined(VCODEC_1P6V)
    1600;
#elif defined(VCODEC_2P5V)
    2500;
#elif defined(VCODEC_3P5V)
    3500;
#else // VCODEC_1P8V
    1800;
#endif

#if 0
static OPT_TYPE bool low_power_adc =
#ifdef LOW_POWER_ADC
    true;
#else
    false;
#endif
#endif

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static CODEC_USER_MAP adc_map[MAX_ANA_MIC_CH_NUM];
static CODEC_USER_MAP vmic_map[MAX_VMIC_NUM];
static CODEC_USER_MAP codec_common_map;
static CODEC_USER_MAP adda_common_map;
static CODEC_USER_MAP vcodec_map;
//static CODEC_USER_MAP rx_cfg_map;

static uint8_t ana_aud_pll_map;
STATIC_ASSERT(ANA_AUD_PLL_USER_QTY <= sizeof(ana_aud_pll_map) * 8, "Too many ANA AUD PLL users");

static uint8_t dac_gain;

#ifdef ANC_APP
#ifndef DYN_ADC_GAIN
#define DYN_ADC_GAIN
#endif
#endif

#ifdef DYN_ADC_GAIN
static int8_t adc_gain_offset[MAX_ANA_MIC_CH_NUM];
#endif

static const int8_t adc_db[] = { -9, -6, -3, 0, 3, 6, 9, 12, };
static const int8_t tgt_adc_db[MAX_ANA_MIC_CH_NUM] = {
    ANALOG_ADC_A_GAIN_DB, ANALOG_ADC_B_GAIN_DB,
    ANALOG_ADC_C_GAIN_DB, ANALOG_ADC_D_GAIN_DB,
    ANALOG_ADC_E_GAIN_DB,
};

#if 0
static const uint8_t pga2_qdb[] = { 0, 1.5*4, 3*4, 4.5*4, 6*4, 12*4, 18*4, 24*4, };
static const uint8_t pga1_qdb[] = { -1, 0, 6*4, 12*4, 18*4, 24*4, 30*4, 36*4, };

static const uint8_t tgt_adc_qdb[MAX_ANA_MIC_CH_NUM] = {
    ANALOG_ADC_A_GAIN_DB * 4, ANALOG_ADC_B_GAIN_DB * 4,
};
#endif

// Max allowed total tune ratio (5000ppm)
#define MAX_TOTAL_TUNE_RATIO                0.005000

//static struct ANALOG_PLL_CFG_T ana_pll_cfg[2];
//static int pll_cfg_idx;

static void analog_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev)
{
    uint32_t lock;
    CODEC_USER_MAP old_map;
    bool set = false;
    int i;
    uint32_t pmu_map = 0;

    lock = int_lock();
    for (i = 0; i < MAX_VMIC_NUM; i++) {
        if (dev & (AUD_VMIC_MAP_VMIC1<<i)) {
            if (vmic_map[i] == 0) {
                set = true;
            }
            vmic_map[i] |= (1<<user);
        } else {
            old_map = vmic_map[i];
            vmic_map[i] &= ~(1<<user);
            if ((old_map != 0) && (vmic_map[i] == 0)) {
                set = true;
            }
        }

        if (vmic_map[i]) {
            pmu_map |= (AUD_VMIC_MAP_VMIC1<<i);
        }
    }
    int_unlock(lock);

    if (set) {
        pmu_codec_mic_bias_enable(pmu_map);
        if (pmu_map) {
            osDelay(1);
        }
    }
}

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div)
{
    usbphy_pll_config(freq, div);
}

void analog_aud_pll_tune(float ratio)
{
#ifdef CHIP_HAS_DCO
    if (dco_enabled()) {
        return;
    }
#endif

    TRACE(2,"%s: %f", __func__, (double)ratio);
    usbphy_pll_tune(ratio);
}

void analog_aud_pll_set_dig_div(uint32_t div)
{
    usbphy_pll_set_dig_div(div);
}

void analog_aud_osc_clk_enable(bool enable)
{
    uint16_t val;

    analog_read(ANA_REG_63, &val);
    if (enable) {
        val |= (REG_CRYSTAL_SEL_LV|REG_PU_OSC);
    } else {
        val &= ~(REG_CRYSTAL_SEL_LV|REG_PU_OSC);
    }
    analog_write(ANA_REG_63, val);

}

void analog_codec_pll_enable(bool enable)
{
    if (enable) {
        hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_AUD);
        usbphy_pll_codec_enable(true);
        analog_aud_osc_clk_enable(false);
    } else {
        analog_aud_osc_clk_enable(true);
        usbphy_pll_codec_enable(false);
        hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_AUD);
    }

}

/****************************************************************

  Function: analog_aud_pll_open
  Description:
    - To open the analog clocking source of CODEC
  ChangeLog:
    191117 binbinguo, initial version

*****************************************************************/
void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    if (user == ANA_AUD_PLL_USER_CODEC) {
        if (hal_cmu_get_audio_resample_status()) {
            analog_aud_osc_clk_enable(true);
        } else {
            hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
            analog_codec_pll_enable(true);
        }
    }
}

/*******************************************************************************************************************

  Function: analog_aud_pll_close
  Description:
    - To close the analog clocking source of CODEC
  ChangeLog:
    191117 binbinguo, initial version

*******************************************************************************************************************/
void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    if (user == ANA_AUD_PLL_USER_CODEC) {
        if (hal_cmu_get_audio_resample_status()) {
            analog_aud_osc_clk_enable(false);
        } else {
            analog_codec_pll_enable(false);
            hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
        }
    }
}

static void analog_aud_enable_dac(uint32_t dac)
{

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        uint16_t val;

        analog_read(ANA_REG_86, &val);
        val |= REG_CODEC_TX_EN_LDAC;
        val |= REG_CODEC_TX_EN_RDAC;
        analog_write(ANA_REG_86, val);

#ifdef AUDIO_OUTPUT_DIFF
        uint16_t val274, val273, val85;

        analog_read(ANA_REG_274, &val274);
        val274 |= REG_CODEC_TX_DIFF_EAR_ENBIAS;
        val274 &= ~REG_CODEC_TX_DIFF_EAR_DR_EN;
        val274 &= ~(REG_CODEC_TX_DIFF_EN_LCLK|REG_CODEC_TX_DIFF_EN_RCLK);
        val274 &= ~(REG_CODEC_TX_DIFF_EN_S1PA|REG_CODEC_TX_DIFF_EN_S2PA);
        val274 &= ~(REG_CODEC_TX_DIFF_EN_S3PA|REG_CODEC_TX_DIFF_EN_S4PA|REG_CODEC_TX_DIFF_EN_S5PA);
        analog_write(ANA_REG_274, val274);

        analog_read(ANA_REG_170, &val);
        val |= CFG_TX_TREE_EN;
        analog_write(ANA_REG_170, val);

        analog_read(ANA_REG_273, &val273);
        val273 &= ~(REG_CODEC_TX_DIFF_EN_EARPA_L|REG_CODEC_TX_DIFF_EN_EARPA_R);
        analog_write(ANA_REG_273, val273);

        analog_read(ANA_REG_85, &val85);
        val85 &= ~(REG_CODEC_TX_DIFF_EN_LDAC|REG_CODEC_TX_DIFF_EN_RDAC);
        analog_write(ANA_REG_85, val);

        if (dac & AUD_CHANNEL_MAP_CH0) {
            val274 |= REG_CODEC_TX_DIFF_EN_LCLK;
            val273 |= REG_CODEC_TX_DIFF_EN_EARPA_L;
            val85 |= REG_CODEC_TX_DIFF_EN_LDAC;

        }

        if (dac & AUD_CHANNEL_MAP_CH1) {
            val274 |= REG_CODEC_TX_DIFF_EN_RCLK;
            val273 |= REG_CODEC_TX_DIFF_EN_EARPA_R;
            val85 |= REG_CODEC_TX_DIFF_EN_RDAC;
        }

        val274 |= REG_CODEC_TX_DIFF_EN_DACLDO|REG_CODEC_TX_DIFF_EAR_DR_EN;

        analog_write(ANA_REG_274, val274);
        analog_write(ANA_REG_85, val85);

        analog_write(ANA_REG_273, val273);
        osDelay(1);

        val274 |= REG_CODEC_TX_DIFF_EN_S1PA;
        analog_write(ANA_REG_274, val274);
        osDelay(1);

        val274 |= REG_CODEC_TX_DIFF_EN_S2PA;
        analog_write(ANA_REG_274, val274);
        osDelay(1);

        val274 |= REG_CODEC_TX_DIFF_EN_S2PA;
        analog_write(ANA_REG_274, val274);
        osDelay(1);

        val274 |= REG_CODEC_TX_DIFF_EN_S3PA;
        analog_write(ANA_REG_274, val274);
        osDelay(1);

        val274 |= REG_CODEC_TX_DIFF_EN_S4PA;
        analog_write(ANA_REG_274, val274);

#else   // AUDIO_OUTPUT_DIFF

        // TODO: for single-ended output

#endif  // AUDIO_OUTPUT_DIFF
    }
    else {
        uint16_t val;

#ifdef AUDIO_OUTPUT_DIFF
        analog_read(ANA_REG_274, &val);
        val &= ~(REG_CODEC_TX_DIFF_EN_S1PA|REG_CODEC_TX_DIFF_EN_S2PA|REG_CODEC_TX_DIFF_EN_S3PA|REG_CODEC_TX_DIFF_EN_S4PA);
        val &= ~(REG_CODEC_TX_DIFF_EN_LCLK|REG_CODEC_TX_DIFF_EN_RCLK|REG_CODEC_TX_DIFF_EAR_DR_EN);
        val &= ~(REG_CODEC_TX_DIFF_EN_DACLDO|REG_CODEC_TX_DIFF_EAR_ENBIAS|REG_CODEC_TX_DIFF_EN_LPPA);
        analog_write(ANA_REG_274, val);

        analog_read(ANA_REG_273, &val);
        val &= ~(REG_CODEC_TX_DIFF_EN_EARPA_L|REG_CODEC_TX_DIFF_EN_EARPA_R);
        analog_write(ANA_REG_273, val);

        analog_read(ANA_REG_170, &val);
        val &= ~CFG_TX_TREE_EN;
        analog_write(ANA_REG_170, val);

#else   // AUDIO_OUTPUT_DIFF

        // TODO: for single-ended output

#endif  // AUDIO_OUTPUT_DIFF

        analog_read(ANA_REG_86, &val);
        val &= ~(REG_CODEC_TX_EN_LDAC|REG_CODEC_TX_EN_RDAC);
        analog_write(ANA_REG_86, val);

    }
}

/*******************************************************************************************************************

  Function: analog_aud_enable_dac_pa
  Description:
    - To enable/disable DAC PA
    - This function should be simple and fast to run to avoid any buffer conflict when noise gating is on
  ChangeLog:
    191117 binbinguo, initial version

*******************************************************************************************************************/
static void analog_aud_enable_dac_pa(uint32_t dac)
{
#ifdef AUDIO_OUTPUT_DIFF
    uint16_t val;

    analog_read(ANA_REG_274, &val);
    if (dac) {
        val |= REG_CODEC_TX_DIFF_EN_S4PA;
    } else {
        val &= ~REG_CODEC_TX_DIFF_EN_S4PA;
    }
    analog_write(ANA_REG_274, val);

#else   // AUDIO_OUTPUT_DIFF

    // TODO: for single-ended output

#endif  // AUDIO_OUTPUT_DIFF
}

static void analog_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    int i;
    CODEC_USER_MAP old_map;
    bool set = false;

    uint16_t val288, val167;

    analog_read(ANA_REG_288, &val288);
    analog_read(ANA_REG_167, &val167);

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (en) {
                if (adc_map[i] == 0) {
                    set = true;
                }
                adc_map[i] |= (1 << user);
            } else {
                old_map = adc_map[i];
                adc_map[i] &= ~(1 << user);
                if ((old_map != 0) && (adc_map[i] == 0)) {
                    set = true;
                }
            }

            if (set) {
                if (i >= 2) {
                    if (adc_map[i]) {
                        val288 |= REG_CODEC_EN_ADCC<<(i-2);
                    } else {
                        val288 &= ~(REG_CODEC_EN_ADCC<<(i-2));
                    }
                } else {
                    if (adc_map[i]) {
                        val167 |= REG_CODEC_EN_ADCA<<i;
                    } else {
                        val167 &= ~(REG_CODEC_EN_ADCA<<i);                    }
                }
            }
        }
    }

    analog_write(ANA_REG_288, val288);
    analog_write(ANA_REG_167, val167);
}

#if 0
static uint32_t qdb_to_adc_gain(uint32_t qdb)
{
    // TODO:

    return 0;
}
#endif

static uint32_t db_to_adc_gain(int db)
{
    int i = 0;
    uint8_t cnt;
    const int8_t *list;

    list = adc_db;
    cnt = ARRAY_SIZE(adc_db);

    for (i = 0; i < cnt - 1; i++) {
        if (db < list[i + 1]) {
            break;
        }
    }

    if (i == cnt - 1) {
        return i;
    }
    else if (db * 2 < list[i] + list[i + 1]) {
        return i;
    } else {
        return i + 1;
    }
}

static int8_t get_chan_adc_gain(uint32_t i)
{
    int8_t gain = 0;

    gain = tgt_adc_db[i];

#ifdef DYN_ADC_GAIN
    if (adc_gain_offset[i] < 0 && -adc_gain_offset[i] > gain) {
        gain = 0;
    } else {
        gain += adc_gain_offset[i];
    }
#endif

    return gain;
}

static void analog_aud_set_adc_gain(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map)
{
    int i = 0, gain = 0;
//    uint16_t gain_val, val16b, val170, val171, val287, val289;
    uint16_t gain_val, val171, val287;

//    analog_read(ANA_REG_16B, &val16b);
//    analog_read(ANA_REG_170, &val170);
    analog_read(ANA_REG_171, &val171);
    analog_read(ANA_REG_287, &val287);
//    analog_read(ANA_REG_289, &val289);

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (0) {
            }
#ifdef ANC_APP
#ifdef ANC_FF_ENABLED
            else if ((ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R) & (AUD_CHANNEL_MAP_CH0 << i)) {
                gain = get_chan_adc_gain(i);
            }
#endif
#ifdef ANC_FB_ENABLED
            else if ((ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R) & (AUD_CHANNEL_MAP_CH0 << i)) {
                gain = get_chan_adc_gain(i);
            }
#endif
#endif
            else if (input_path == AUD_INPUT_PATH_LINEIN) {
                gain = LINEIN_ADC_GAIN_DB;
            } else if (input_path == AUD_INPUT_PATH_USBAUDIO) {
                gain = get_chan_adc_gain(i);
            } else if (input_path == AUD_INPUT_PATH_MAINMIC) {
                gain = get_chan_adc_gain(i);
            }

            gain_val = db_to_adc_gain(gain);

            if (i < 2) {
                val171 &= ~(REG_CODEC_ADCA_GAIN_BIT_MASK<<(3*i));
                val171 |= (gain_val<<(REG_CODEC_ADCA_GAIN_BIT_SHIFT+3*i) & (REG_CODEC_ADCA_GAIN_BIT_MASK<<(3*i)));

#if 0
                if (i == 0) {
                    val16b |= REG_CODEC_ADCA_GAIN_UPDATE;
                } else {
                    val170 |= REG_CODEC_ADCB_GAIN_UPDATE;
                }
#endif
            } else {
                val287 &= ~(REG_CODEC_ADCC_GAIN_BIT_MASK<<(3*(i-2)));
                val287 |= (gain_val<<(REG_CODEC_ADCC_GAIN_BIT_SHIFT+3*(i-2)) & (REG_CODEC_ADCC_GAIN_BIT_MASK<<(3*(i-2))));

                //val289 |= (REG_CODEC_ADCC_GAIN_UPDATE<<(i-2));
            }
        }
    }

//    analog_write(ANA_REG_16B, val16b);
//    analog_write(ANA_REG_170, val170);
    analog_write(ANA_REG_171, val171);
    analog_write(ANA_REG_287, val287);
//    analog_write(ANA_REG_289, val289);

}

#ifdef ANC_APP
void analog_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r)
{
    enum AUD_CHANNEL_MAP_T ch_map;
    uint32_t l, r;
    uint8_t index;
    int8_t org, adj;

    // qdb to db
    offset_l /= 4;
    offset_r /= 4;

    ch_map = 0;
    l = r = 32;
    if (type == ANC_FEEDFORWARD) {
#ifdef ANC_FF_ENABLED
        ch_map = ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R;
        l = get_msb_pos(ANC_FF_MIC_CH_L);
        r = get_msb_pos(ANC_FF_MIC_CH_R);
#endif
    } else {
#ifdef ANC_FB_ENABLED
        ch_map = ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R;
        l = get_msb_pos(ANC_FB_MIC_CH_L);
        r = get_msb_pos(ANC_FB_MIC_CH_R);
#endif
    }

    if ((l >= MAX_ANA_MIC_CH_NUM || adc_gain_offset[l] == offset_l) &&
            (r >= MAX_ANA_MIC_CH_NUM || adc_gain_offset[r] == offset_r)) {
        return;
    }

    TRACE(3,"ana: apply anc adc gain offset: type=%d offset=%d/%d", type, offset_l, offset_r);

    if (l < MAX_ANA_MIC_CH_NUM) {
        index = (type == ANC_FEEDFORWARD) ? 0 : 2;
        if (offset_l == 0) {
            adc_gain_offset[l] = 0;
            hal_codec_apply_anc_adc_gain_offset(index, 0);
        } else {
            adc_gain_offset[l] = 0;
            org = adc_db[db_to_adc_gain(get_chan_adc_gain(l))];
            adc_gain_offset[l] = offset_l;
            adj = adc_db[db_to_adc_gain(get_chan_adc_gain(l))];
            hal_codec_apply_anc_adc_gain_offset(index, (org - adj));
        }
    }
    if (r < MAX_ANA_MIC_CH_NUM) {
        index = (type == ANC_FEEDFORWARD) ? 1 : 3;
        if (offset_r == 0) {
            adc_gain_offset[r] = 0;
            hal_codec_apply_anc_adc_gain_offset(index, 0);
        } else {
            adc_gain_offset[r] = 0;
            org = adc_db[db_to_adc_gain(get_chan_adc_gain(r))];
            adc_gain_offset[r] = offset_r;
            adj = adc_db[db_to_adc_gain(get_chan_adc_gain(r))];
            hal_codec_apply_anc_adc_gain_offset(index, (org - adj));
        }
    }

    analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
}
#endif

#ifdef DYN_ADC_GAIN
void analog_aud_apply_adc_gain_offset(enum AUD_CHANNEL_MAP_T ch_map, int16_t offset)
{
    enum AUD_CHANNEL_MAP_T map;
    int i;

#ifdef ANC_APP
#ifdef ANC_FF_ENABLED
    ch_map &= ~(ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R);
#endif
#ifdef ANC_FB_ENABLED
    ch_map &= ~(ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R);
#endif
#endif

    if (ch_map) {
        map = ch_map;

        while (map) {
            i = get_msb_pos(map);
            map &= ~(1 << i);
            if (i < MAX_ANA_MIC_CH_NUM) {
                adc_gain_offset[i] = offset;
            }
        }

        TRACE(2,"ana: apply adc gain offset: ch_map=0x%X offset=%d", ch_map, offset);

        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
    }
}
#endif

static void analog_codec_tx_pa_gain_sel(uint32_t v)
{
    uint16_t val;

#ifdef AUDIO_OUTPUT_DIFF

    analog_read(ANA_REG_273, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_DIFF_EAR_GAIN, v);
    analog_write(ANA_REG_273, val);

#else   // AUDIO_OUTPUT_DIFF

    analog_read(ANA_REG_27B, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_GAIN, v);
    analog_write(ANA_REG_27B, val);

#endif  // AUDIO_OUTPUT_DIFF
}

void analog_aud_set_dac_gain(int32_t v)
{
    if (v < 0) {
        v = DEFAULT_TX_PA_GAIN;
    }

    dac_gain = v;
    analog_codec_tx_pa_gain_sel(v);
}

uint32_t analog_codec_get_dac_gain(void)
{
    // TODO:

    return 0;
}

uint32_t analog_codec_dac_gain_to_db(int32_t gain)
{
    // TODO:

    return 0;
}

int32_t analog_codec_dac_max_attn_db(void)
{
    //return -(DAC_GAIN_TO_QDB(tx_pa_gain_0db) + 2) / 4;
    return 0;
}

static int POSSIBLY_UNUSED dc_calib_checksum_valid(uint16_t efuse)
{
    int i;
    uint32_t cnt = 0;

    for (i = 0; i < 12; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & 0xF) == ((efuse >> 12) & 0xF));
}

void analog_aud_get_dc_calib_value(int16_t *dc_l, int16_t *dc_r)
{
    // no DC calib temporarily

    *dc_l = *dc_r = 0;
}

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA

static void analog_aud_dc_calib_init(void)
{
    uint16_t val;
    int16_t dc_l, dc_r;

    analog_aud_get_dc_calib_value(&dc_l, &dc_r);

#ifdef AUDIO_OUTPUT_DIFF

    analog_read(ANA_REG_280, &val);
    val &= ~REG_TX_DIFF_EAR_OFF_BITL_MASK;
    val |= REG_TX_DIFF_EAR_OFF_BITL(dc_l);
    analog_write(ANA_REG_280, val);

    analog_read(ANA_REG_281, &val);
    val &= ~REG_TX_DIFF_EAR_OFF_BITR_MASK;
    val |= REG_TX_DIFF_EAR_OFF_BITR(dc_r);
    analog_write(ANA_REG_281, val);

#else   // AUDIO_OUTPUT_DIFF

    analog_read(ANA_REG_68, &val);
    val &= ~REG_CODEC_TX_EAR_OFF_BITR_MASK;
    val |= REG_CODEC_TX_EAR_OFF_BITR(dc_r);
    analog_write(ANA_REG_68, val);

    analog_read(ANA_REG_69, &val);
    val &= ~REG_CODEC_TX_EAR_OFF_BITL_MASK;
    val |= REG_CODEC_TX_EAR_OFF_BITL(dc_l);
    analog_write(ANA_REG_69, val);

#endif  // AUDIO_OUTPUT_DIFF
}

static void analog_aud_dc_calib_enable(bool en)
{
    uint16_t val;

#ifdef AUDIO_OUTPUT_DIFF

    analog_read(ANA_REG_282, &val);
    if (en) {
        val |= REG_TX_DIFF_EAR_OFFEN;
    } else {
        val &= ~REG_TX_DIFF_EAR_OFFEN;
    }
    analog_write(ANA_REG_282, val);

#else   // AUDIO_OUTPUT_DIFF

    analog_read(ANA_REG_62, &val);
    if (en) {
        val |= REG_CODEC_TX_EAR_OFFTEN;
    } else {
        val &= ~REG_CODEC_TX_EAR_OFFTEN;
    }
    analog_write(ANA_REG_62, val);

#endif  // AUDIO_OUTPUT_DIFF
}

#endif // AUDIO_OUTPUT_DC_CALIB_ANA

static void analog_aud_vcodec_enable(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (vcodec_map == 0) {
            set = true;
        }
        vcodec_map |= (1 << user);
    } else {
        vcodec_map &= ~(1 << user);
        if (vcodec_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        pmu_codec_config(!!vcodec_map);
    }
}

static void analog_aud_enable_common_internal(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val;
    bool set = false;
    static bool s_first_up = true;

    lock = int_lock();
    if (en) {
        if (codec_common_map == 0) {
            set = true;
        }
        codec_common_map |= (1<<user);
    } else {
        codec_common_map &= ~(1<<user);
        if (codec_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {

        if (codec_common_map) {

            analog_read(ANA_REG_63, &val);
            val &= ~REG_CODEC_VCM_LOW_VCM_MASK;
            val |= REG_CODEC_VCM_LOW_VCM(7);
            analog_write(ANA_REG_63, val);

            analog_read(ANA_REG_61, &val);
            val |= REG_CODEC_EN_VCM;
            analog_write(ANA_REG_61, val);

            if (s_first_up) {
                osDelay(10);
                s_first_up = false;
            }

            {
                uint16_t val62;

                analog_read(ANA_REG_62, &val62);

#ifdef AUDIO_OUTPUT_DIFF
                analog_read(ANA_REG_27F, &val);
                val &= ~REG_TX_DIFF_EAR_IBSEL_MASK;
                val |= REG_TX_DIFF_EAR_IBSEL(1);
                val &= ~(REG_TX_DIFF_EAR_DR_ST_L_MASK|REG_TX_DIFF_EAR_DR_ST_R_MASK);
                val |= REG_TX_DIFF_EAR_DR_ST_L(3)|REG_TX_DIFF_EAR_DR_ST_R(3);
                val &= ~REG_TX_DIFF_EAR_FBCAP_MASK;
                val |= REG_TX_DIFF_EAR_FBCAP(3);
                val &= ~(REG_TX_DIFF_EAR_LCAL|REG_TX_DIFF_EAR_RCAL);
                analog_write(ANA_REG_27F, val);

#else   // AUDIO_OUTPUT_DIFF
                val62 &= ~REG_CODEC_TX_EAR_IBSEL_MASK;
                val62 |= REG_CODEC_TX_EAR_IBSEL(1);

#endif  // AUDIO_OUTPUT_DIFF

                val62 |= REG_CODEC_VCM_EN_LPF|REG_CODEC_EN_BIAS|REG_CODEC_EN_VCMBUFFER;
                analog_write(ANA_REG_62, val62);
            }

        } else {

            analog_read(ANA_REG_62, &val);
            val &= ~(REG_CODEC_EN_BIAS|REG_CODEC_EN_TX_EXT|REG_CODEC_EN_VCMBUFFER);
            analog_write(ANA_REG_62, val);
        }

    }
}

static void analog_aud_enable_codec_common(enum ANA_CODEC_USER_T user, bool en)
{
#ifndef VCM_ON
    analog_aud_enable_common_internal(user, en);
#endif
}

static void analog_aud_enable_adda_common(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (adda_common_map == 0) {
            set = true;
        }
        adda_common_map |= (1 << user);
    } else {
        adda_common_map &= ~(1 << user);
        if (adda_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set == false) {
        return;
    }

    if (adda_common_map) {
#ifdef PU_TX_REGULATOR
#ifdef AUDIO_OUTPUT_DIFF
        analog_read(ANA_REG_274, &val);
        val |= REG_CODEC_TX_DIFF_PU_TX_REGULATOR;
#ifdef BYPASS_TX_REGULATOR
        val |= REG_CODEC_TX_DIFF_BYPASS_TX_REGULATOR;
#else   // BYPASS_TX_REGULATOR
        val &= ~REG_CODEC_TX_DIFF_BYPASS_TX_REGULATOR;
#endif  // BYPASS_TX_REGULATOR
        analog_write(ANA_REG_274, val);
#else   // AUDIO_OUTPUT_DIFF
        analog_read(ANA_REG_66, &val);
        val |= REG_PU_TX_REGULATOR;
        analog_write(ANA_REG_66, val);
#ifdef BYPASS_TX_REGULATOR
        analog_read(ANA_REG_68, &val);
        val |= REG_BYPASS_TX_REGULATOR;
        analog_write(ANA_REG_68, val);
#else   // BYPASS_TX_REGULATOR
        analog_read(ANA_REG_68, &val);
        val &= ~REG_BYPASS_TX_REGULATOR;
        analog_write(ANA_REG_68, val);
#endif  // BYPASS_TX_REGULATOR
#endif  // AUDIO_OUTPUT_DIFF
#endif  // PU_TX_REGULATOR

        analog_read(ANA_REG_62, &val);
        val |= REG_CODEC_EN_BIAS;
        analog_write(ANA_REG_62, val);
    } else {
        analog_read(ANA_REG_62, &val);
        val &= ~REG_CODEC_EN_BIAS;
        analog_write(ANA_REG_62, val);

#ifdef PU_TX_REGULATOR
#ifdef AUDIO_OUTPUT_DIFF
        analog_read(ANA_REG_274, &val);
        val &= ~REG_CODEC_TX_DIFF_PU_TX_REGULATOR;
        analog_write(ANA_REG_274, val);
#else
        analog_read(ANA_REG_66, &val);
        val &= ~REG_PU_TX_REGULATOR;
        analog_write(ANA_REG_66, val);
#endif  // AUDIO_OUTPUT_DIFF
#endif  // PU_TX_REGULATOR
    }
}

uint32_t analog_aud_get_max_dre_gain(void)
{
    uint32_t dre_gain = 0;

    if (vcodec_mv >= 2500) {
        dre_gain = 0xF;
    } else {
        dre_gain = 0xE;
    }

    return dre_gain;
}

void analog_open(void)
{
    uint16_t val;

    //----------------------------------------------------------------------
    //                  DAC initialization
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    //          Common for differential and single-ended output
    //----------------------------------------------------------------------

    analog_read(ANA_REG_6C, &val);
    val &= ~REG_CODEC_BIAS_IBSEL_MASK;
    val &= ~REG_CODEC_BIAS_IBSEL_TX_MASK;
    val &= ~REG_CODEC_BIAS_IBSEL_VOICE_MASK;
    val |= REG_CODEC_BIAS_IBSEL_VOICE(8);
    if (vcodec_mv >= 2500) {
        // At this time, vcodec can be either 2.5v or 1.8v
        // Here is the setting for vcodec=2.5v
        val |= REG_CODEC_BIAS_IBSEL(8)|REG_CODEC_BIAS_IBSEL_TX(7);
    } else {
        // Here is the setting for vcodec=1.8v
        val |= REG_CODEC_BIAS_IBSEL(8)|REG_CODEC_BIAS_IBSEL_TX(0xC);
    }
    analog_write(ANA_REG_6C, val);

    analog_read(ANA_REG_63, &val);
    val &= ~REG_CODEC_VCM_LOW_VCM_MASK;
    val &= ~REG_CODEC_VCM_LOW_VCM_LPF_MASK;
    val &= ~REG_CODEC_VCM_LOW_VCM_LP_MASK;
    if (vcodec_mv >= 1800) {
        val |= REG_CODEC_VCM_LOW_VCM(7)|REG_CODEC_VCM_LOW_VCM_LPF(7)|REG_CODEC_VCM_LOW_VCM_LP(7);
    } else {
        val |= REG_CODEC_VCM_LOW_VCM(7)|REG_CODEC_VCM_LOW_VCM_LPF(7)|REG_CODEC_VCM_LOW_VCM_LP(7);
    }
    val |= REG_CRYSTAL_SEL_LV|CFG_TX_CLK_INV;
    analog_write(ANA_REG_63, val);

    {
        uint16_t dre_gain = analog_aud_get_max_dre_gain();

        analog_read(ANA_REG_87, &val);
        val &= ~REG_CODEC_TX_EAR_DRE_GAIN_L_MASK;
        val &= ~REG_CODEC_TX_EAR_DRE_GAIN_R_MASK;
        val |= REG_CODEC_TX_EAR_DRE_GAIN_L(dre_gain)|REG_CODEC_TX_EAR_DRE_GAIN_R(dre_gain);
#ifdef DAC_DRE_ENABLE
        val |= DRE_GAIN_SEL_L|DRE_GAIN_SEL_R;
#else   // DAC_DRE_ENABLE
        val &= ~(DRE_GAIN_SEL_L|DRE_GAIN_SEL_R);
#endif  // DAC_DRE_ENABLE
        analog_write(ANA_REG_87, val);
    }

#ifdef VCM_ON
    analog_aud_enable_common_internal(ANA_CODEC_USER_DAC, true);
#endif

    analog_read(ANA_REG_67, &val);
    val &= ~REG_CODEC_TX_VREFBUFN_I_BIT_MASK;
    val |= REG_CODEC_TX_VREFBUFN_I_BIT(0);
    val &= ~REG_CODEC_TX_VREFBUFP_I_BIT_MASK;
    val |= REG_CODEC_TX_VREFBUFP_I_BIT(0);
    analog_write(ANA_REG_67, val);

    analog_read(ANA_REG_6A, &val);
    val &= ~REG_CODEC_BUF_LOWVCM_MASK;
    val |= REG_CODEC_BUF_LOWVCM(4);
    val &= ~REG_CODEC_TX_COMP_PA_12_MASK;
    val |= REG_CODEC_TX_COMP_PA_12(0xb);
    analog_write(ANA_REG_6A, val);


#ifdef AUDIO_OUTPUT_DIFF
    //----------------------------------------------------------------------
    //          for differential output
    //----------------------------------------------------------------------

    analog_read(ANA_REG_27E, &val);
    val &= ~REG_TX_DIFF_DAC_VREF_R_MASK;
    val &= ~REG_TX_DIFF_DAC_VREF_L_MASK;
    if (vcodec_mv >= 2500) {
        val |= REG_TX_DIFF_DAC_VREF_L(9)|REG_TX_DIFF_DAC_VREF_R(9);
    } else {
        val |= REG_TX_DIFF_DAC_VREF_L(8)|REG_TX_DIFF_DAC_VREF_R(8);
    }
    analog_write(ANA_REG_27E, val);

    analog_read(ANA_REG_27A, &val);
    val &= ~REG_TX_DIFF_EAR_COMP_L_MASK;
    val &= ~REG_TX_DIFF_EAR_COMP_R_MASK;
    if (vcodec_mv >= 2500) {
        //val |= REG_TX_DIFF_EAR_COMP_L(0x7)|REG_TX_DIFF_EAR_COMP_R(0x7);
        val |= REG_TX_DIFF_EAR_COMP_L(0x5)|REG_TX_DIFF_EAR_COMP_R(0x5);
    } else {
        val |= REG_TX_DIFF_EAR_COMP_L(0x18)|REG_TX_DIFF_EAR_COMP_R(0x18);
    }
    analog_write(ANA_REG_27A, val);

    analog_read(ANA_REG_27F, &val);
    val &= ~REG_TX_DIFF_EAR_FBCAP_MASK;
    val &= ~REG_TX_DIFF_EAR_IBSEL_MASK;
    val |= REG_TX_DIFF_EAR_FBCAP(3);
    val |= REG_TX_DIFF_EAR_IBSEL(3);
    analog_write(ANA_REG_27F, val);

    analog_read(ANA_REG_65, &val);
    val &= ~REG_CODEC_TX_EAR_DR_ST_MASK;
    val |= REG_CODEC_TX_EAR_DR_ST(4);
    val |= REG_CODEC_TX_EAR_DR_EN;
    analog_write(ANA_REG_65, val);

    analog_read(ANA_REG_273, &val);
#ifdef DAC_DRE_GAIN_DC_UPDATE
    val |= REG_CODEC_TX_DIFF_EAR_DRE_GAIN_L_UPDATE|REG_CODEC_TX_DIFF_EAR_DRE_GAIN_R_UPDATE;
#else   // DAC_DRE_GAIN_DC_UPDATE
    val &= ~(REG_CODEC_TX_DIFF_EAR_DRE_GAIN_L_UPDATE|REG_CODEC_TX_DIFF_EAR_DRE_GAIN_R_UPDATE);
#endif  //DAC_DRE_GAIN_DC_UPDATE
#ifdef VBAT_DET
    val |= REG_ZERO_DETECT_POWER_DOWN_DIRECT_DIFF;
#endif
    analog_write(ANA_REG_273, val);

    analog_read(ANA_REG_282, &val);
    val &= ~REG_TX_DIFF_EAR_SOFTSTART_MASK;
    val &= ~REG_TX_DIFF_EAR_VCM_BIT_MASK;
    val |= REG_TX_DIFF_EAR_SOFTSTART(8);
    val |= REG_TX_DIFF_EAR_VCM_BIT(3);
    val |= REG_TX_DIFF_EAR_VCM_SEL;
    val |= REG_TX_DIFF_EN_CM_COMP;
    val &= ~REG_TX_DIFF_EAR_OUTPUTSEL_MASK;
    if (vcodec_mv >= 2500) {
        //val |= REG_TX_DIFF_EAR_OUTPUTSEL(8);
		val |= REG_TX_DIFF_EAR_OUTPUTSEL(4);
    } else {
        val |= REG_TX_DIFF_EAR_OUTPUTSEL(1);
    }
    analog_write(ANA_REG_282, val);

    analog_read(ANA_REG_274, &val);
    val |= REG_CODEC_TX_DIFF_EAR_OCEN;
    val |= REG_CODEC_TX_DIFF_BYPASS_TX_REGULATOR;
    analog_write(ANA_REG_274, val);

    analog_read(ANA_REG_27C, &val);
    val &= ~REG_TX_DIFF_DAC_LDO0P9_VSEL_MASK;
    val |= REG_TX_DIFF_DAC_LDO0P9_VSEL(2);
    analog_write(ANA_REG_27C, val);

    analog_read(ANA_REG_283, &val);
    val &= ~REG_TX_DIFF_TX_REGULATOR_BIT_MASK;
    val |= REG_TX_DIFF_TX_REGULATOR_BIT(5);
    analog_write(ANA_REG_283, val);

    analog_read(ANA_REG_161, &val);
    val &= ~REG_CODEC_TX_DIFF_CLK_MODE;
    analog_write(ANA_REG_161, val);

    analog_read(ANA_REG_84, &val);
    val &= ~REG_TX_DIFF_LP_VREFBUF_L_MASK;
    val &= ~REG_TX_DIFF_LP_VREFBUF_R_MASK;
    //val |= REG_TX_DIFF_LP_VREFBUF_L(7);
    //val |= REG_TX_DIFF_LP_VREFBUF_R(7);
	val |= REG_TX_DIFF_LP_VREFBUF_L(5);
    analog_write(ANA_REG_84, val);

    analog_read(ANA_REG_28B, &val);
    val &= ~REG_TX_DIFF_COMP_1STAGE_MASK;
    val |= REG_TX_DIFF_COMP_1STAGE(5);
    val &= ~REG_TX_DIFF_EAR_LOWGAINL_MASK;
    //val |= REG_TX_DIFF_EAR_LOWGAINL(4);
	val |= REG_TX_DIFF_EAR_LOWGAINL(0xe);
    val &= ~REG_TX_DIFF_EAR_LOWGAINR_MASK;
    //val |= REG_TX_DIFF_EAR_LOWGAINR(4);
	val |= REG_TX_DIFF_EAR_LOWGAINR(0xe);
    analog_write(ANA_REG_28B, val);

    analog_read(ANA_REG_27E, &val);
    val &= ~REG_TX_DIFF_EAR_DIS_MASK;
    val |= REG_TX_DIFF_EAR_DIS(0);
    val &= ~REG_TX_DIFF_VSEL_VCMO;
    val &= ~REG_TX_DIFF_DAC_VREF_MASK;
    val |= REG_TX_DIFF_DAC_VREF(7);
    val &= ~REG_TX_DIFF_EAR_CASP_R_MASK;
    val |= REG_TX_DIFF_EAR_CASP_R(3);
    val &= ~REG_TX_DIFF_EAR_CASP_L_MASK;
    val |= REG_TX_DIFF_EAR_CASP_L(3);
    analog_write(ANA_REG_27E, val);

#else   // AUDIO_OUTPUT_DIFF
    //----------------------------------------------------------------------
    //              for single-ended output
    //----------------------------------------------------------------------

    // TODO:

#endif  // AUDIO_OUTPUT_DIFF

    //----------------------------------------------------------------------
    //              ADC initializaiton
    //----------------------------------------------------------------------

    analog_read(ANA_REG_165, &val);
    val &= ~REG_CODEC_ADCA_CH_SEL_MASK;
    val &= ~REG_CODEC_ADCB_CH_SEL_MASK;
    val &= ~REG_CODEC_ADCC_CH_SEL_MASK;
    val &= ~REG_CODEC_ADCD_CH_SEL_MASK;
    val &= ~REG_CODEC_ADCE_CH_SEL_MASK;
    val |= REG_CODEC_ADCA_CH_SEL(1)|REG_CODEC_ADCB_CH_SEL(1)    \
        |REG_CODEC_ADCC_CH_SEL(1)|REG_CODEC_ADCD_CH_SEL(1)|REG_CODEC_ADCE_CH_SEL(1);
    analog_write(ANA_REG_165, val);

    analog_read(ANA_REG_16A, &val);
    val &= ~REG_CODEC_ADC_OP1_IBIT_MASK;
    val |= REG_CODEC_ADC_OP1_IBIT(2);
    analog_write(ANA_REG_16A, val);

    analog_read(ANA_REG_64, &val);
    val &= ~REG_CODEC_ADC_VREF_SEL_MASK;
    val |= REG_CODEC_ADC_VREF_SEL(5);
    val |= CFG_RESET_ADC_A_DR|CFG_RESET_ADC_B_DR;
    val &= ~(CFG_RESET_ADC_A|CFG_RESET_ADC_B);
    analog_write(ANA_REG_64, val);

    analog_read(ANA_REG_288, &val);
    val |= CFG_RESET_ADC_C_DR|CFG_RESET_ADC_D_DR|CFG_RESET_ADC_E_DR;
    val &= ~(CFG_RESET_ADC_C|CFG_RESET_ADC_D|CFG_RESET_ADC_E);
    analog_write(ANA_REG_288, val);

    analog_read(ANA_REG_167, &val);
    val &= ~REG_CODEC_ADC_REG_VSEL_MASK;
    val &= ~REG_CODEC_ADC_RES_SEL_MASK;
    val |= REG_CODEC_ADC_RES_SEL(0)|REG_CODEC_ADC_REG_VSEL(4);
    analog_write(ANA_REG_167, val);

    //----------------------------------------------------------------------
    //              Other initializaiton
    //----------------------------------------------------------------------

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    analog_aud_dc_calib_init();
    analog_aud_dc_calib_enable(true);
#endif

#ifdef CHIP_HAS_DCO
    dco_open();
#endif
}

void analog_sleep(void)
{
}

void analog_wakeup(void)
{
}

void analog_aud_codec_anc_enable(enum ANC_TYPE_T type, bool en)
{
    enum ANA_CODEC_USER_T user;
    enum AUD_CHANNEL_MAP_T ch_map;

    ch_map = 0;
    if (type == ANC_FEEDFORWARD) {
        user = ANA_CODEC_USER_ANC_FF;
#ifdef ANC_FF_ENABLED
        ch_map = ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R;
#endif
    } else {
        user = ANA_CODEC_USER_ANC_FB;
#ifdef ANC_FB_ENABLED
        ch_map = ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R;
#endif
    }

    ANALOG_TRACE(4,"%s: type=%d en=%d ch_map=0x%x", __func__, type, en, ch_map);

    if (en) {
        analog_aud_enable_vmic(user, ANC_VMIC_CFG);
        analog_aud_enable_codec_common(user, true);
        analog_aud_enable_adda_common(user, true);
        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
        analog_aud_enable_adc(user, ch_map, true);
    } else {
        analog_aud_apply_anc_adc_gain_offset(type, 0, 0);
        analog_aud_enable_adc(user, ch_map, false);
        analog_aud_enable_adda_common(user, false);
        analog_aud_enable_codec_common(user, false);
        analog_aud_enable_vmic(user, 0);
    }
}

void analog_aud_codec_anc_init_close(void)
{

}

#ifdef CFG_MIC_KEY
void analog_aud_mickey_enable(bool en)
{
    if (en) {
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV);
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, true);
    } else {
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, 0);
    }
}
#endif  // CFG_MIC_KEY

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    if (en) {
        uint32_t dev = hal_codec_get_input_path_cfg(input_path);

        analog_aud_enable_vmic(ANA_CODEC_USER_ADC, dev);
        analog_aud_enable_codec_common(ANA_CODEC_USER_ADC, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_ADC, true);
        analog_aud_set_adc_gain(input_path, ch_map);
        analog_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, true);
    } else {
        analog_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, false);
        analog_aud_enable_adda_common(ANA_CODEC_USER_ADC, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_ADC, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_ADC, 0);
    }
}

static void analog_aud_codec_config_speaker(void)
{
    bool en;

    if (ana_spk_req && !ana_spk_muted) {
        en = true;
    } else {
        en = false;
    }

    if (ana_spk_enabled != en) {
        ana_spk_enabled = en;
        if (en) {
            analog_aud_enable_dac_pa(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);
        } else {
            analog_aud_enable_dac_pa(0);
        }
    }
}

void analog_aud_codec_speaker_enable(bool en)
{
    ana_spk_req = en;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_dac_enable(bool en)
{
    if (en) {
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, true);
        analog_aud_enable_dac(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        osDelay(1);
        analog_aud_codec_speaker_enable(true);
#endif
    } else {
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        analog_aud_codec_speaker_enable(false);
        osDelay(1);
#endif
        analog_aud_enable_dac(0);
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, false);
    }
}

void analog_aud_codec_open(void)
{
    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, true);
}

void analog_aud_codec_close(void)
{
    static const enum AUD_CHANNEL_MAP_T all_ch = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 |
        AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4;

    analog_aud_codec_speaker_enable(false);
    osDelay(1);
    analog_aud_codec_dac_enable(false);

    analog_aud_codec_adc_enable(AUD_IO_PATH_NULL, all_ch, false);
    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, false);
}

void analog_aud_codec_mute(void)
{
    ana_spk_muted = true;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_nomute(void)
{
    ana_spk_muted = false;
    analog_aud_codec_config_speaker();
}

int analog_debug_config_audio_output(bool diff)
{
    return 0;
}

int analog_debug_config_codec(uint16_t mv)
{

    return 0;
}

int analog_debug_config_low_power_adc(bool enable)
{
    return 0;
}

void analog_aud_set_sar_adc_sample_rate (enum AUD_SAMPRATE_T sample_rate)
{
    sample_rate = sample_rate;
}

//--------------------------------------------------------------
// DCXO
//--------------------------------------------------------------

/**********************************************************************************************************************

    FUNCTION NAME:      analog_osc_clk_default_enable
    INPUT PARAMETERS:
        - void
    RETURN VALUE:
        - void
    DESCRIPTION:
            This function is to enable the output of OSCX2 or OSCX4 clock.
    CHANGELOG:
        1) 2019/10/23 binbinguo@bestechnic.com
            Initial version

**********************************************************************************************************************/

void BOOT_TEXT_FLASH_LOC analog_osc_clk_default_enable (void)
{
    uint16_t val;

    // to disable and reset the doubler of OSC
    analog_read(ANA_REG_70, &val);
    val &= ~DIG_DBL_EN;
    val |= DIG_DBL_RST;
    analog_write(ANA_REG_70, val);

    analog_read(ANA_REG_71, &val);
#ifdef ANA_26M_X4_ENABLE
    // to output OSCX4 clock
    val |= REG_DBL_FREQ_SEL;
#else
    // to output OSCX2 clock
    val &= ~REG_DBL_FREQ_SEL;
#endif
    analog_write(ANA_REG_71, val);

    // to enable the doubler of OSC
    analog_read(ANA_REG_70, &val);
    val |= DIG_DBL_EN;
    analog_write(ANA_REG_70, val);

    hal_sys_timer_delay(US_TO_TICKS(100));

    // to clear the reset of the doubler of OSC
    val &= ~DIG_DBL_RST;
    analog_write(ANA_REG_70, val);

    // to rise the drv strength of DCO clock, especially for OSCx2 and OSCx4 clock
    analog_read(ANA_REG_76, &val);
    val &= ~REG_XTAL_CLKBUF_DRV_17_16_MASK;
    val |= REG_XTAL_CLKBUF_DRV_17_16(1);
    analog_write(ANA_REG_76, val);
}

/**********************************************************************************************************************

    FUNCTION NAME:      analog_codec_clk_default_sel
    INPUT PARAMETERS:
        - void
    RETURN VALUE:
        - void
    DESCRIPTION:
            This function is to set the clocking source of CODEC.
            The clocking source of CODEC is set to OSC in default.
    CHANGELOG:
        1) 2019/10/23 binbinguo@bestechnic.com
            Initial version

**********************************************************************************************************************/
void BOOT_TEXT_FLASH_LOC analog_codec_clk_default_sel (void)
{
    uint16_t val;
    analog_read(ANA_REG_63, &val);
    // choose OSC clock as default clock of CODEC
    val |= REG_CRYSTAL_SEL_LV;
    analog_write(ANA_REG_63, val);
}

