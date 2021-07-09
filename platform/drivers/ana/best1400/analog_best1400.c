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
#include "plat_types.h"
#include "analog.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "tgt_hardware.h"


#define VCM_ON

#define DAC_DC_CALIB_BIT_WIDTH              14

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

#ifndef CFG_HW_AUD_MICKEY_DEV
#define CFG_HW_AUD_MICKEY_DEV               (AUD_VMIC_MAP_VMIC1)
#endif

#ifndef ANC_VMIC_CFG
#define ANC_VMIC_CFG                        (AUD_VMIC_MAP_VMIC1)
#endif

// 61
#define REG_CODEC_EN_ADCA                   (1 << 0)
#define REG_CODEC_ADCA_DITHER_EN            (1 << 1)
#define REG_CODEC_ADCA_CH_SEL_SHIFT         2
#define REG_CODEC_ADCA_CH_SEL_MASK          (0x3 << REG_CODEC_ADCA_CH_SEL_SHIFT)
#define REG_CODEC_ADCA_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCA_CH_SEL, n)
#define CFG_RESET_PGAA_DR                   (1 << 7)
#define REG_CODEC_ADCA_GAIN_BIT_SHIFT       8
#define REG_CODEC_ADCA_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCA_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCA_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_GAIN_BIT, n)
#define REG_CODEC_ADCA_GAIN_UPDATE          (1 << 11)


// 64
#define REG_CODEC_ADC_IBSEL_REG_SHIFT       0
#define REG_CODEC_ADC_IBSEL_REG_MASK        (0xF << REG_CODEC_ADC_IBSEL_REG_SHIFT)
#define REG_CODEC_ADC_IBSEL_REG(n)          BITFIELD_VAL(REG_CODEC_ADC_IBSEL_REG, n)
#define REG_CODEC_ADC_IBSEL_VCOMP_SHIFT     4
#define REG_CODEC_ADC_IBSEL_VCOMP_MASK      (0xF << REG_CODEC_ADC_IBSEL_VCOMP_SHIFT)
#define REG_CODEC_ADC_IBSEL_VCOMP(n)        BITFIELD_VAL(REG_CODEC_ADC_IBSEL_VCOMP, n)
#define REG_CODEC_ADC_IBSEL_VREF_SHIFT      8
#define REG_CODEC_ADC_IBSEL_VREF_MASK       (0xF << REG_CODEC_ADC_IBSEL_VREF_SHIFT)
#define REG_CODEC_ADC_IBSEL_VREF(n)         BITFIELD_VAL(REG_CODEC_ADC_IBSEL_VREF, n)
#define REG_CODEC_ADC_LPFVCM_SW_SHIFT       12
#define REG_CODEC_ADC_LPFVCM_SW_MASK        (0xF << REG_CODEC_ADC_LPFVCM_SW_SHIFT)
#define REG_CODEC_ADC_LPFVCM_SW(n)          BITFIELD_VAL(REG_CODEC_ADC_LPFVCM_SW, n)

// 65
#define REG_CODEC_ADC_OP1_IBIT_SHIFT        0
#define REG_CODEC_ADC_OP1_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP1_IBIT_SHIFT)
#define REG_CODEC_ADC_OP1_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP1_IBIT, n)
#define REG_CODEC_ADC_OP1_R_SEL             (1 << 2)
#define REG_CODEC_ADC_OP2_IBIT_SHIFT        3
#define REG_CODEC_ADC_OP2_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP2_IBIT_SHIFT)
#define REG_CODEC_ADC_OP2_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP2_IBIT, n)
#define REG_CODEC_ADC_OP2_R_SEL             (1 << 5)
#define REG_CODEC_ADC_OP3_IBIT_SHIFT        6
#define REG_CODEC_ADC_OP3_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP3_IBIT_SHIFT)
#define REG_CODEC_ADC_OP3_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP3_IBIT, n)
#define REG_CODEC_ADC_OP3_R_SEL             (1 << 8)
#define REG_CODEC_ADC_OP4_IBIT_SHIFT        9
#define REG_CODEC_ADC_OP4_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP4_IBIT_SHIFT)
#define REG_CODEC_ADC_OP4_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP4_IBIT, n)
#define REG_CODEC_ADC_OP4_R_SEL             (1 << 11)
#define REG_CODEC_ADC_VREF_SEL_SHIFT        12
#define REG_CODEC_ADC_VREF_SEL_MASK         (0xF << REG_CODEC_ADC_VREF_SEL_SHIFT)
#define REG_CODEC_ADC_VREF_SEL(n)           BITFIELD_VAL(REG_CODEC_ADC_VREF_SEL, n)

// 66
#define REG_CODEC_BIAS_IBSEL_SHIFT          0
#define REG_CODEC_BIAS_IBSEL_MASK           (0xF << REG_CODEC_BIAS_IBSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL(n)             BITFIELD_VAL(REG_CODEC_BIAS_IBSEL, n)
#define REG_CODEC_BIAS_IBSEL_TX_SHIFT       4
#define REG_CODEC_BIAS_IBSEL_TX_MASK        (0xF << REG_CODEC_BIAS_IBSEL_TX_SHIFT)
#define REG_CODEC_BIAS_IBSEL_TX(n)          BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_TX, n)
#define REG_CODEC_BIAS_IBSEL_VOICE_SHIFT    8
#define REG_CODEC_BIAS_IBSEL_VOICE_MASK     (0xF << REG_CODEC_BIAS_IBSEL_VOICE_SHIFT)
#define REG_CODEC_BIAS_IBSEL_VOICE(n)       BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_VOICE, n)
#define REG_CODEC_BIAS_LOWV                 (1 << 12)
#define REG_CODEC_BIAS_LOWV_LP              (1 << 13)
#define REG_CODEC_BUF_LOWPOWER              (1 << 14)
#define REG_CODEC_BUF_LOWPOWER2             (1 << 15)

// 67
#define REG_CODEC_ADC_REG_VSEL_SHIFT        0
#define REG_CODEC_ADC_REG_VSEL_MASK         (0x7 << REG_CODEC_ADC_REG_VSEL_SHIFT)
#define REG_CODEC_ADC_REG_VSEL(n)           BITFIELD_VAL(REG_CODEC_ADC_REG_VSEL, n)
#define REG_CODEC_ADC_RES_SEL_SHIFT         3
#define REG_CODEC_ADC_RES_SEL_MASK          (0x7 << REG_CODEC_ADC_RES_SEL_SHIFT)
#define REG_CODEC_ADC_RES_SEL(n)            BITFIELD_VAL(REG_CODEC_ADC_RES_SEL, n)
#define REG_CODEC_BUF_LOWVCM_SHIFT          6
#define REG_CODEC_BUF_LOWVCM_MASK           (0x7 << REG_CODEC_BUF_LOWVCM_SHIFT)
#define REG_CODEC_BUF_LOWVCM(n)             BITFIELD_VAL(REG_CODEC_BUF_LOWVCM, n)
#define REG_CODEC_EN_BIAS                   (1 << 9)
#define REG_CODEC_EN_BIAS_LP                (1 << 10)
#define REG_CODEC_EN_TX_EXT                 (1 << 11)
#define REG_CODEC_DAC_CLK_EDGE_SEL          (1 << 12)
#define CFG_TX_CH0_MUTE                     (1 << 13)
#define CFG_TX_CH1_MUTE                     (1 << 14)

// 68
#define REG_CODEC_EN_VCM                    (1 << 0)
#define REG_CODEC_VCM_EN_LPF                (1 << 1)
#define REG_CODEC_LP_VCM                    (1 << 2)
#define REG_CODEC_VCM_LOW_VCM_SHIFT         3
#define REG_CODEC_VCM_LOW_VCM_MASK          (0xF << REG_CODEC_VCM_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_LOW_VCM(n)            BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM, n)
#define REG_CODEC_VCM_LOW_VCM_LP_SHIFT      7
#define REG_CODEC_VCM_LOW_VCM_LP_MASK       (0xF << REG_CODEC_VCM_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LP(n)         BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LP, n)
#define REG_CODEC_VCM_LOW_VCM_LPF_SHIFT     11
#define REG_CODEC_VCM_LOW_VCM_LPF_MASK      (0xF << REG_CODEC_VCM_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LPF(n)        BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LPF, n)
#define REG_CODEC_EN_VCM_BUFFER             (1 << 15)

// 69
#define REG_CODEC_RX_EN_VTOI                (1 << 0)
#define REG_CODEC_RX_VTOI_I_DAC2_SHIFT      1
#define REG_CODEC_RX_VTOI_I_DAC2_MASK       (0x7 << REG_CODEC_RX_VTOI_I_DAC2_SHIFT)
#define REG_CODEC_RX_VTOI_I_DAC2(n)         BITFIELD_VAL(REG_CODEC_RX_VTOI_I_DAC2, n)
#define REG_CODEC_RX_VTOI_IDAC_SEL_SHIFT    4
#define REG_CODEC_RX_VTOI_IDAC_SEL_MASK     (0xF << REG_CODEC_RX_VTOI_IDAC_SEL_SHIFT)
#define REG_CODEC_RX_VTOI_IDAC_SEL(n)       BITFIELD_VAL(REG_CODEC_RX_VTOI_IDAC_SEL, n)
#define REG_CODEC_RX_VTOI_VCS_SEL_SHIFT     8
#define REG_CODEC_RX_VTOI_VCS_SEL_MASK      (0x1F << REG_CODEC_RX_VTOI_VCS_SEL_SHIFT)
#define REG_CODEC_RX_VTOI_VCS_SEL(n)        BITFIELD_VAL(REG_CODEC_RX_VTOI_VCS_SEL, n)
#define REG_CODEC_TEST_SEL_SHIFT            13
#define REG_CODEC_TEST_SEL_MASK             (0x7 << REG_CODEC_TEST_SEL_SHIFT)
#define REG_CODEC_TEST_SEL(n)               BITFIELD_VAL(REG_CODEC_TEST_SEL, n)

// 6A
#define REG_CODEC_TX_DAC_MUTEL              (1 << 0)
#define REG_CODEC_TX_DAC_MUTER              (1 << 1)
#define REG_CODEC_TX_DAC_SWR_SHIFT          2
#define REG_CODEC_TX_DAC_SWR_MASK           (0x3 << REG_CODEC_TX_DAC_SWR_SHIFT)
#define REG_CODEC_TX_DAC_SWR(n)             BITFIELD_VAL(REG_CODEC_TX_DAC_SWR, n)
#define REG_CODEC_TX_DAC_VREF_L_SHIFT       4
#define REG_CODEC_TX_DAC_VREF_L_MASK        (0xF << REG_CODEC_TX_DAC_VREF_L_SHIFT)
#define REG_CODEC_TX_DAC_VREF_L(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_L, n)
#define REG_CODEC_TX_DAC_VREF_R_SHIFT       8
#define REG_CODEC_TX_DAC_VREF_R_MASK        (0xF << REG_CODEC_TX_DAC_VREF_R_SHIFT)
#define REG_CODEC_TX_DAC_VREF_R(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_R, n)
#define REG_CODEC_TX_EAR_DIS_SHIFT          12
#define REG_CODEC_TX_EAR_DIS_MASK           (0x3 << REG_CODEC_TX_EAR_DIS_SHIFT)
#define REG_CODEC_TX_EAR_DIS(n)             BITFIELD_VAL(REG_CODEC_TX_EAR_DIS, n)

// 6B
#define REG_CODEC_TX_EAR_COMP_L_SHIFT       0
#define REG_CODEC_TX_EAR_COMP_L_MASK        (0x7F << REG_CODEC_TX_EAR_COMP_L_SHIFT)
#define REG_CODEC_TX_EAR_COMP_L(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_COMP_L, n)
#define REG_CODEC_TX_EAR_COMP_R_SHIFT       7
#define REG_CODEC_TX_EAR_COMP_R_MASK        (0x7F << REG_CODEC_TX_EAR_COMP_R_SHIFT)
#define REG_CODEC_TX_EAR_COMP_R(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_COMP_R, n)

//6C
#define REG_CODEC_TX_EAR_DOUBLEBIAS         (1 << 0)
#define REG_CODEC_TX_EAR_DR_EN              (1 << 1)
#define REG_CODEC_TX_EAR_DR_ST_SHIFT        2
#define REG_CODEC_TX_EAR_DR_ST_MASK         (0x7 << REG_CODEC_TX_EAR_DR_ST_SHIFT)
#define REG_CODEC_TX_EAR_DR_ST(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_DR_ST, n)
#define REG_CODEC_TX_EAR_ENBIAS             (1 << 5)
#define REG_CODEC_TX_EAR_FBCAP_SHIFT        6
#define REG_CODEC_TX_EAR_FBCAP_MASK         (0x3 << REG_CODEC_TX_EAR_FBCAP_SHIFT)
#define REG_CODEC_TX_EAR_FBCAP(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_FBCAP, n)
#define REG_CODEC_TX_EAR_IBSEL_SHIFT        8
#define REG_CODEC_TX_EAR_IBSEL_MASK         (0x3 << REG_CODEC_TX_EAR_IBSEL_SHIFT)
#define REG_CODEC_TX_EAR_IBSEL(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_IBSEL, n)

// 6D
#define REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT   0
#define REG_CODEC_TX_EAR_DRE_GAIN_L_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L, n)
#define REG_CODEC_TX_EAR_DRE_GAIN_R_SHIFT   5
#define REG_CODEC_TX_EAR_DRE_GAIN_R_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_R_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_R(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_R, n)
#define DRE_GAIN_SEL_L                      (1 << 10)
#define DRE_GAIN_SEL_R                      (1 << 11)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE  (1 << 12)
#define REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE  (1 << 13)
#define REG_CODEC_TX_EAR_GAIN_SHIFT         14
#define REG_CODEC_TX_EAR_GAIN_MASK          (0x3 << REG_CODEC_TX_EAR_GAIN_SHIFT)
#define REG_CODEC_TX_EAR_GAIN(n)            BITFIELD_VAL(REG_CODEC_TX_EAR_GAIN, n)

// 6E
#define REG_CODEC_TX_EAR_LCAL               (1 << 0)
#define REG_CODEC_TX_EAR_RCAL               (1 << 1)
#define REG_CODEC_TX_EAR_LPBIAS             (1 << 2)
#define REG_CODEC_TX_EAR_OCEN               (1 << 3)
#define REG_CODEC_TX_EAR_OFFEN              (1 << 4)
#define REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT    5
#define REG_CODEC_TX_EAR_OUTPUTSEL_MASK     (0xF << REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT)
#define REG_CODEC_TX_EAR_OUTPUTSEL(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_OUTPUTSEL, n)
#define REG_CODEC_TX_EAR_SOFTSTART_SHIFT    9
#define REG_CODEC_TX_EAR_SOFTSTART_MASK     (0x3F << REG_CODEC_TX_EAR_SOFTSTART_SHIFT)
#define REG_CODEC_TX_EAR_SOFTSTART(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_SOFTSTART, n)
#define CFG_TX_TREE_EN                      (1 << 15)

// 6F
#define REG_CODEC_TX_EAR_LOWGAINL_SHIFT     0
#define REG_CODEC_TX_EAR_LOWGAINL_MASK      (0x3 << REG_CODEC_TX_EAR_LOWGAINL_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINL, n)
#define REG_CODEC_TX_EAR_OFF_BITL_SHIFT     2
#define REG_CODEC_TX_EAR_OFF_BITL_MASK      (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL, n)

// 70
#define REG_CODEC_TX_EAR_LOWGAINR_SHIFT     0
#define REG_CODEC_TX_EAR_LOWGAINR_MASK      (0x3 << REG_CODEC_TX_EAR_LOWGAINR_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINR, n)
#define REG_CODEC_TX_EAR_OFF_BITR_SHIFT     2
#define REG_CODEC_TX_EAR_OFF_BITR_MASK      (0x3FFF << REG_CODEC_TX_EAR_OFF_BITR_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITR, n)

// 71
#define REG_CODEC_TX_EN_DACLDO              (1 << 0)
#define REG_CODEC_TX_EN_EARPA_L             (1 << 1)
#define REG_CODEC_TX_EN_EARPA_R             (1 << 2)
#define REG_CODEC_TX_EN_LCLK                (1 << 3)
#define REG_CODEC_TX_EN_RCLK                (1 << 4)
#define REG_CODEC_TX_EN_LDAC                (1 << 5)
#define REG_CODEC_TX_EN_RDAC                (1 << 6)
#define REG_CODEC_TX_EN_LPPA                (1 << 7)
#define REG_CODEC_TX_EN_S1PA                (1 << 8)
#define REG_CODEC_TX_EN_S2PA                (1 << 9)
#define REG_CODEC_TX_EN_S3PA                (1 << 10)
#define REG_CODEC_TX_EN_S4PA                (1 << 11)
#define REG_CODEC_TX_EN_S5PA                (1 << 12)
#define REG_IDETLEAR_EN                     (1 << 13)
#define REG_IDETREAR_EN                     (1 << 14)
#define REG_CODEC_TX_SW_MODE                (1 << 15)

// 72
#define REG_CODEC_TX_RVREF_CAP_BIT          (1 << 0)
#define REG_DAC_LDO0P9_VSEL_SHIFT           1
#define REG_DAC_LDO0P9_VSEL_MASK            (0x1F << REG_DAC_LDO0P9_VSEL_SHIFT)
#define REG_DAC_LDO0P9_VSEL(n)              BITFIELD_VAL(REG_DAC_LDO0P9_VSEL, n)
#define REG_BYPASS_TX_REGULATOR             (1 << 6)
#define REG_PU_TX_REGULATOR                 (1 << 7)
#define REG_TX_REGULATOR_BIT_SHIFT          8
#define REG_TX_REGULATOR_BIT_MASK           (0xF << REG_TX_REGULATOR_BIT_SHIFT)
#define REG_TX_REGULATOR_BIT(n)             BITFIELD_VAL(REG_TX_REGULATOR_BIT, n)
#define REG_TX_REG_CAP_BIT                  (1 << 12)
#define REG_CODEC_TX_EAR_VCM_BIT_SHIFT      13
#define REG_CODEC_TX_EAR_VCM_BIT_MASK       (0x3 << REG_CODEC_TX_EAR_VCM_BIT_SHIFT)
#define REG_CODEC_TX_EAR_VCM_BIT(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_VCM_BIT, n)
#define REG_CODEC_TX_EAR_VCM_SEL            (1 << 15)

// 73
#define REG_CODEC_ADC_DITHER0P5_EN          (1 << 0)
#define REG_CODEC_ADC_DITHER0P25_EN         (1 << 1)
#define REG_CODEC_ADC_DITHER1_EN            (1 << 2)
#define REG_CODEC_ADC_DITHER2_EN            (1 << 3)
#define REG_CODEC_ADC_DITHER_PHASE_SEL      (1 << 4)
#define RX_TIMER_RSTN_BIT_SHIFT             5
#define RX_TIMER_RSTN_BIT_MASK              (0x3F << RX_TIMER_RSTN_BIT_SHIFT)
#define RX_TIMER_RSTN_BIT(n)                BITFIELD_VAL(RX_TIMER_RSTN_BIT, n)
#define REG_PU_OSC                          (1 << 11)

#ifdef CHIP_BEST1402
// 75
#define REG_CODEC_EN_ADCB                   (1 << 0)
#define REG_CODEC_ADCB_DITHER_EN            (1 << 1)
#define REG_CODEC_ADCB_CH_SEL_SHIFT         2
#define REG_CODEC_ADCB_CH_SEL_MASK          (0x3 << REG_CODEC_ADCB_CH_SEL_SHIFT)
#define REG_CODEC_ADCB_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCB_CH_SEL, n)
#define REG_CODEC_RESET_ADCB                (1 << 4)
#define CFG_RESET_ADCB_DR                   (1 << 5)
#define REG_RX_PGAB_RESET                   (1 << 6)
#define CFG_RESET_PGAB_DR                   (1 << 7)
#define REG_CODEC_ADCB_GAIN_BIT_SHIFT       8
#define REG_CODEC_ADCB_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCB_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCB_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_GAIN_BIT, n)
#define REG_CODEC_ADCB_GAIN_UPDATE          (1 << 11)
#define REG_CODEC_ADCB_RES_2P5K             (1 << 12)
#define REG_CODEC_ADCB_RES_2P5K_UPDATE      (1 << 13)
#define REG_CODEC_IDETB_EN                  (1 << 14)
#define CFG_ADCB_DITHER_CLK_INV             (1 << 15)
#endif

// 164
#define CFG_TX_PEAK_OFF_ADC_EN              (1 << 0)
#define CFG_TX_PEAK_OFF_DAC_EN              (1 << 1)
#define CFG_PEAK_DET_DR                     (1 << 2)
#define CFG_TX_PEAK_OFF_ADC                 (1 << 3)
#define CFG_TX_PEAK_OFF_DAC                 (1 << 4)
#define CFG_PEAK_DET_DB_DELAY_SHIFT         5
#define CFG_PEAK_DET_DB_DELAY_MASK          (0x7 << CFG_PEAK_DET_DB_DELAY_SHIFT)
#define CFG_PEAK_DET_DB_DELAY(n)            BITFIELD_VAL(CFG_PEAK_DET_DB_DELAY, n)
#define REG_CODEC_TX_PEAK_DET_BIT           (1 << 8)
#define REG_CODEC_TX_PEAK_NL_EN             (1 << 9)
#define REG_CODEC_TX_PEAK_NR_EN             (1 << 10)
#define REG_CODEC_TX_PEAK_PL_EN             (1 << 11)
#define REG_CODEC_TX_PEAK_PR_EN             (1 << 12)
#define CFG_TX_CLK_INV                      (1 << 13)
#define CFG_CODEC_DIN_L_RST                 (1 << 14)
#define CFG_CODEC_DIN_R_RST                 (1 << 15)

// C4
#define CODEC_RESAMPLE_CLK_BUF_PU           (1 << 2)

// 16A
#define REG_CODEC_TX_EAR_CASN_BIT_SHIFT     0
#define REG_CODEC_TX_EAR_CASN_BIT_MASK      (0x7 << REG_CODEC_TX_EAR_CASN_BIT_SHIFT)
#define REG_CODEC_TX_EAR_CASN_BIT(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_CASN_BIT, n)
#define REG_CODEC_TX_EAR_CASP_BIT_SHIFT     3
#define REG_CODEC_TX_EAR_CASP_BIT_MASK      (0x7 << REG_CODEC_TX_EAR_CASP_BIT_SHIFT)
#define REG_CODEC_TX_EAR_CASP_BIT(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_CASP_BIT, n)
#define REG_CODEC_TX_EAR_EN_CM_COMP         (1 << 6)
#define REG_CODEC_TX_VREFBUF_LP_SHIFT       7
#define REG_CODEC_TX_VREFBUF_LP_MASK        (0x3 << REG_CODEC_TX_VREFBUF_LP_SHIFT)
#define REG_CODEC_TX_VREFBUF_LP(n)          BITFIELD_VAL(REG_CODEC_TX_VREFBUF_LP, n)
#define REG_CODEC_VCM_BIAS_HIGHV            (1 << 9)
#ifdef CHIP_BEST1402
#define REG_CODEC_TX_VREFBUF_LP_R_SHIFT     10
#define REG_CODEC_TX_VREFBUF_LP_R_MASK      (0x7 << REG_CODEC_TX_VREFBUF_LP_R_SHIFT)
#define REG_CODEC_TX_VREFBUF_LP_R(n)        BITFIELD_VAL(REG_CODEC_TX_VREFBUF_LP_R, n)
#define REG_CODEC_TX_VREFBUF_LP_L_SHIFT     13
#define REG_CODEC_TX_VREFBUF_LP_L_MASK      (0x7 << REG_CODEC_TX_VREFBUF_LP_L_SHIFT)
#define REG_CODEC_TX_VREFBUF_LP_L(n)        BITFIELD_VAL(REG_CODEC_TX_VREFBUF_LP_L, n)
#else
#define RESERVED_ANA_46_41_SHIFT            10
#define RESERVED_ANA_46_41_MASK             (0x3F << RESERVED_ANA_46_41_SHIFT)
#define RESERVED_ANA_46_41(n)               BITFIELD_VAL(RESERVED_ANA_46_41, n)
#endif

#ifdef CHIP_BEST1402
// 16B
#define REG_RX_PGAB_EN                      (1 << 0)
#define REG_RX_PGAB_FASTSETTLE              (1 << 1)
#define REG_RX_PGAB_CHANSEL_SHIFT           2
#define REG_RX_PGAB_CHANSEL_MASK            (0x3 << REG_RX_PGAB_CHANSEL_SHIFT)
#define REG_RX_PGAB_CHANSEL(n)              BITFIELD_VAL(REG_RX_PGAB_CHANSEL, n)
#define REG_RX_PGAB_2P5K_SEL                (1 << 4)
#define REG_RX_PGAB_7P5K_SEL                (1 << 5)
#define REG_RX_PGAB_10K_SEL                 (1 << 6)
#define REG_RX_PGAB_CAPMODE                 (1 << 7)
#define REG_RX_PGAB_DACGAIN_SHIFT           8
#define REG_RX_PGAB_DACGAIN_MASK            (0x3 << REG_RX_PGAB_DACGAIN_SHIFT)
#define REG_RX_PGAB_DACGAIN(n)              BITFIELD_VAL(REG_RX_PGAB_DACGAIN, n)
#define REG_RX_PGAB_IBIT_OPA_SHIFT          10
#define REG_RX_PGAB_IBIT_OPA_MASK           (0x3 << REG_RX_PGAB_IBIT_OPA_SHIFT)
#define REG_RX_PGAB_IBIT_OPA(n)             BITFIELD_VAL(REG_RX_PGAB_IBIT_OPA, n)
#define REG_RX_PGAB_DRE_SHIFT               12
#define REG_RX_PGAB_DRE_MASK                (0xF << REG_RX_PGAB_DRE_SHIFT)
#define REG_RX_PGAB_DRE(n)                  BITFIELD_VAL(REG_RX_PGAB_DRE, n)

// 16C
#define REG_RX_PGAB_DRE_DR                  (1 << 0)
#define REG_RX_PGAB_DRE_UPDATE              (1 << 1)
#define REG_RX_PGAB_GAIN_UPDATE             (1 << 2)
#define REG_RX_PGAB_LARGEGAIN_UPDATE        (1 << 3)
#define REG_RX_PGAB_GAIN_SHIFT              4
#define REG_RX_PGAB_GAIN_MASK               (0x7 << REG_RX_PGAB_GAIN_SHIFT)
#define REG_RX_PGAB_GAIN(n)                 BITFIELD_VAL(REG_RX_PGAB_GAIN, n)
#define REG_RX_PGAB_RFB_EN                  (1 << 7)
#define REG_RX_PGAB_LARGEGAIN_SHIFT         8
#define REG_RX_PGAB_LARGEGAIN_MASK          (0x7 << REG_RX_PGAB_LARGEGAIN_SHIFT)
#define REG_RX_PGAB_LARGEGAIN(n)            BITFIELD_VAL(REG_RX_PGAB_LARGEGAIN, n)
#define REG_RX_PGAB_ZERO_DET_EN             (1 << 11)
#define REG_RX_PGAB_OFFSET_CAL_EN           (1 << 12)
#define REG_RX_PGAB_OP_C_SEL                (1 << 13)
#define REG_RX_PGAB_OP_GM_SEL               (1 << 14)
#define REG_RX_PGAB_OP_R_SEL                (1 << 15)
#endif

enum ANA_REG_T {
    ANA_REG_61 = 0x61,
    ANA_REG_62,
    ANA_REG_63,
    ANA_REG_64,
    ANA_REG_65,
    ANA_REG_66,
    ANA_REG_67,
    ANA_REG_68,
    ANA_REG_69,
    ANA_REG_6A,
    ANA_REG_6B,
    ANA_REG_6C,
    ANA_REG_6D,
    ANA_REG_6E,
    ANA_REG_6F,
    ANA_REG_70,
    ANA_REG_71,
    ANA_REG_72,
    ANA_REG_73,
    ANA_REG_74,
#ifdef CHIP_BEST1402
    ANA_REG_75,
#endif

    ANA_REG_161 = 0x161,
    ANA_REG_162,
    ANA_REG_163,
    ANA_REG_164,
    ANA_REG_165,
    ANA_REG_166,
    ANA_REG_167,
    ANA_REG_168,
    ANA_REG_169,
    ANA_REG_16A,
#ifdef CHIP_BEST1402
    ANA_REG_16B,
    ANA_REG_16C,
#endif
};

enum ANA_CODEC_USER_T {
    ANA_CODEC_USER_DAC          = (1 << 0),
    ANA_CODEC_USER_ADC          = (1 << 1),

    ANA_CODEC_USER_CODEC        = (1 << 2),
    ANA_CODEC_USER_MICKEY       = (1 << 3),

    ANA_CODEC_USER_ANC_FF       = (1 << 4),

    ANA_CODEC_USER_VAD          = (1 << 5),
};

struct ANALOG_PLL_CFG_T {
    uint32_t freq;
    uint8_t div;
    uint16_t val[3];
    int32_t total_delta;
};

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static bool anc_calib_mode;

static enum ANA_CODEC_USER_T adc_map[MAX_ANA_MIC_CH_NUM];
static enum ANA_CODEC_USER_T vmic_map[MAX_ANA_MIC_CH_NUM];
static enum ANA_CODEC_USER_T codec_common_map;
static enum ANA_CODEC_USER_T adda_common_map;
static enum ANA_CODEC_USER_T vcodec_map;


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
    ANALOG_ADC_A_GAIN_DB,
#ifdef CHIP_BEST1402
    ANALOG_ADC_B_GAIN_DB,
#endif
};

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div)
{
}

void analog_aud_osc_clk_enable(bool enable)
{
    uint16_t val;

    if (enable) {
        analog_read(ANA_REG_73, &val);
        val |= REG_PU_OSC;
        analog_write(ANA_REG_73, val);
    } else {
        analog_read(ANA_REG_73, &val);
        val &= ~REG_PU_OSC;
        analog_write(ANA_REG_73, val);
    }
}

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_END) {
        return;
    }

    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {

        analog_aud_osc_clk_enable(true);
    }
}

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_END) {
        return;
    }

    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {

        analog_aud_osc_clk_enable(false);
    }
}

static void analog_aud_enable_dac(uint32_t dac)
{
    uint16_t val_6c;
    uint16_t val_6E;
    uint16_t val_71;

    analog_read(ANA_REG_6C, &val_6c);
    analog_read(ANA_REG_6E, &val_6E);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_6c |= REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS;
        analog_write(ANA_REG_6C, val_6c);
        osDelay(1);
        val_6E |= CFG_TX_TREE_EN;
        analog_write(ANA_REG_6E, val_6E);
        osDelay(1);

        val_71 = 0;
        if (vcodec_mv >= 1800) {
            val_71 |= REG_CODEC_TX_EN_S2PA | REG_CODEC_TX_EN_S3PA | REG_CODEC_TX_EN_S5PA | REG_CODEC_TX_SW_MODE;
        }
        if (dac & AUD_CHANNEL_MAP_CH0) {
            val_71 |= REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_LCLK | REG_CODEC_TX_EN_LDAC;
        }
        if (dac & AUD_CHANNEL_MAP_CH1) {
            val_71 |= REG_CODEC_TX_EN_EARPA_R | REG_CODEC_TX_EN_RCLK | REG_CODEC_TX_EN_RDAC
                   | REG_CODEC_TX_EN_LDAC;
        }
        val_71 |= REG_CODEC_TX_EN_DACLDO | REG_CODEC_TX_EN_LPPA;
        analog_write(ANA_REG_71, val_71);
        osDelay(1);
        val_71 |= REG_CODEC_TX_EN_S1PA;
        analog_write(ANA_REG_71, val_71);
        // Ensure 1ms delay before enabling dac_pa
        osDelay(1);
    } else {
        // Ensure 1ms delay after disabling dac_pa
        osDelay(1);
        analog_read(ANA_REG_71, &val_71);
        val_71 &= ~REG_CODEC_TX_EN_S1PA;
        analog_write(ANA_REG_71, val_71);
        osDelay(1);
        val_71 = 0;
        analog_write(ANA_REG_71, val_71);
        osDelay(1);

        val_6E &= ~CFG_TX_TREE_EN;
        analog_write(ANA_REG_6E, val_6E);
        osDelay(1);

        val_6c &= ~(REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS);
        analog_write(ANA_REG_6C, val_6c);
    }
}

static void analog_aud_enable_dac_pa(uint32_t dac)
{
    uint16_t val_71;

    analog_read(ANA_REG_71, &val_71);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_71 |= REG_CODEC_TX_EN_S4PA;
        analog_write(ANA_REG_71, val_71);

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_clear();
#endif
    } else {
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_set();
#endif

        val_71 &= ~REG_CODEC_TX_EN_S4PA;
        analog_write(ANA_REG_71, val_71);
    }
}

static void analog_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    int i;
    uint16_t val, val_69;
    enum ANA_CODEC_USER_T old_map;
    bool set;
    bool global_update;

    ANALOG_DEBUG_TRACE(3,"[%s] user=%d ch_map=0x%x", __func__, user, ch_map);

    global_update = false;

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            set = false;
            if (en) {
                if (adc_map[i] == 0) {
                    set = true;
                }
                adc_map[i] |= user;
            } else {
                old_map = adc_map[i];
                adc_map[i] &= ~user;
                if (old_map != 0 && adc_map[i] == 0) {
                    set = true;
                }
            }
            if (set) {
                global_update = true;
                if (i == 0) {
                    analog_read(ANA_REG_61, &val);
                    if (adc_map[i]) {
                        val |= REG_CODEC_EN_ADCA;
                    } else {
                        val &= ~REG_CODEC_EN_ADCA;
                    }
                    analog_write(ANA_REG_61, val);
#ifdef CHIP_BEST1402
                } else {
                    analog_read(ANA_REG_75, &val);
                    if (adc_map[i]) {
                        val |= REG_CODEC_EN_ADCB;
                    } else {
                        val &= ~REG_CODEC_EN_ADCB;
                    }
                    analog_write(ANA_REG_75, val);
#endif
                }
            }
        }
    }

    if (global_update) {
        for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
            if (adc_map[i]) {
                break;
            }
        }
        analog_read(ANA_REG_69, &val_69);
        if (i < MAX_ANA_MIC_CH_NUM) {
            val_69 |= REG_CODEC_RX_EN_VTOI;
        } else {
            val_69 &= ~REG_CODEC_RX_EN_VTOI;
        }
        analog_write(ANA_REG_69, val_69);
    }
}

static uint32_t db_to_adc_gain(int db)
{
    int i;
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
    int8_t gain;

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
    int i;
    int gain;
    uint16_t gain_val;
    uint16_t val;

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (0) {
            } else if (input_path == AUD_INPUT_PATH_LINEIN) {
                gain = LINEIN_ADC_GAIN_DB;
            } else {
                gain = get_chan_adc_gain(i);
            }
            gain_val = db_to_adc_gain(gain);
            if (i == 0) {
                analog_read(ANA_REG_61, &val);
                val = SET_BITFIELD(val, REG_CODEC_ADCA_GAIN_BIT, gain_val);
                analog_write(ANA_REG_61, val);
#ifdef CHIP_BEST1402
            } else {
                analog_read(ANA_REG_75, &val);
                val = SET_BITFIELD(val, REG_CODEC_ADCB_GAIN_BIT, gain_val);
                analog_write(ANA_REG_75, val);
#endif
            }
        }
    }
}

#ifdef ANC_APP
void analog_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r)
{
    enum ANC_TYPE_T single_type;
    enum AUD_CHANNEL_MAP_T ch_map;
    uint32_t l, r;
    int8_t org_l, adj_l;
    int8_t org_r, adj_r;

    // qdb to db
    offset_l /= 4;
    offset_r /= 4;

    while (type) {
        l = get_msb_pos(type);
        single_type = (1 << l);
        type &= ~single_type;

        ch_map = 0;
        l = r = 32;
        if (0) {
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
        } else if (single_type == ANC_FEEDFORWARD) {
            ch_map |= ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R;
            l = get_msb_pos(ANC_FF_MIC_CH_L);
            r = get_msb_pos(ANC_FF_MIC_CH_R);
#endif
        } else {
            continue;
        }

        if ((l >= MAX_ANA_MIC_CH_NUM || adc_gain_offset[l] == offset_l) &&
                (r >= MAX_ANA_MIC_CH_NUM || adc_gain_offset[r] == offset_r)) {
            continue;
        }

        ANALOG_INFO_TRACE(0, "ana: apply anc adc gain offset: type=%d offset=%d/%d", single_type, offset_l, offset_r);

        org_l = adj_l = 0;
        if (l < MAX_ANA_MIC_CH_NUM) {
            adc_gain_offset[l] = 0;
            if (offset_l) {
                org_l = adc_db[db_to_adc_gain(get_chan_adc_gain(l))];
                adc_gain_offset[l] = offset_l;
                adj_l = adc_db[db_to_adc_gain(get_chan_adc_gain(l))];
            }
        }

        org_r = adj_r = 0;
        if (r < MAX_ANA_MIC_CH_NUM) {
            adc_gain_offset[r] = 0;
            if (offset_r) {
                org_r = adc_db[db_to_adc_gain(get_chan_adc_gain(r))];
                adc_gain_offset[r] = offset_r;
                adj_r = adc_db[db_to_adc_gain(get_chan_adc_gain(r))];
            }
        }

        hal_codec_apply_anc_adc_gain_offset(single_type, (org_l - adj_l), (org_r - adj_r));
        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
    }
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

        ANALOG_INFO_TRACE(2,"ana: apply adc gain offset: ch_map=0x%X offset=%d", ch_map, offset);

        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
    }
}
#endif

void analog_aud_set_dac_gain(int32_t v)
{
}

uint32_t analog_codec_get_dac_gain(void)
{
    return 0;
}

uint32_t analog_codec_dac_gain_to_db(int32_t gain)
{
    return 0;
}

int32_t analog_codec_dac_max_attn_db(void)
{
    return 0;
}

static int POSSIBLY_UNUSED dc_calib_checksum_valid(uint32_t efuse)
{
    int i;
    uint32_t cnt = 0;
    uint32_t chksum_mask = (1 << (16 - DAC_DC_CALIB_BIT_WIDTH)) - 1;

    for (i = 0; i < DAC_DC_CALIB_BIT_WIDTH; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & chksum_mask) == ((efuse >> DAC_DC_CALIB_BIT_WIDTH) & chksum_mask));
}

static int16_t dc_calib_val_decode(int16_t val)
{
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    uint32_t sign_bit = (1 << (DAC_DC_CALIB_BIT_WIDTH - 1));
    uint32_t num_mask = sign_bit - 1;

    if (val & sign_bit) {
        val = -(val & num_mask);
    }
#endif
    return val;
}

void analog_aud_get_dc_calib_value(int16_t *dc_l, int16_t *dc_r)
{
    static const uint8_t EFUSE_PAGE_DIFF_1P7V[2] = { PMU_EFUSE_PAGE_DCCALIB2_L, PMU_EFUSE_PAGE_DCCALIB2_R, };
    static const uint8_t EFUSE_PAGE_DIFF_1P95V[2] = { PMU_EFUSE_PAGE_DCCALIB_L, PMU_EFUSE_PAGE_DCCALIB_R, };
    const uint8_t *page;
    uint16_t efuse;

    union DC_EFUSE_T {
        struct DC_VALUE_T {
            int16_t dc          : DAC_DC_CALIB_BIT_WIDTH;
            uint16_t checksum   : (16 - DAC_DC_CALIB_BIT_WIDTH);
        } val;
        uint16_t reg;
    };
    union DC_EFUSE_T dc;

    if (vcodec_mv >= 1900) {
        page = EFUSE_PAGE_DIFF_1P95V;
    } else {
        page = EFUSE_PAGE_DIFF_1P7V;
    }

    pmu_get_efuse(page[0], &efuse);
    if (dc_calib_checksum_valid(efuse)) {
        ANALOG_INFO_TRACE(1,"Dc calib L OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_l = dc.val.dc;
    } else {
        ANALOG_INFO_TRACE(1,"Warning: Bad dc calib efuse L: 0x%04x", efuse);
        *dc_l = 0;
    }

    pmu_get_efuse(page[1], &efuse);
    if (dc_calib_checksum_valid(efuse)) {
        ANALOG_INFO_TRACE(1,"Dc calib R OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_r = dc.val.dc;
    } else {
        ANALOG_INFO_TRACE(1,"Warning: Bad dc calib efuse R: 0x%04x", efuse);
        *dc_r = 0;
    }

    ANALOG_INFO_TRACE(2,"ANA: DC CALIB L=0x%04hX/%d R=0x%04hX/%d", *dc_l, dc_calib_val_decode(*dc_l), *dc_r, dc_calib_val_decode(*dc_r));

#if defined(ANA_DC_CALIB_L) || defined(ANA_DC_CALIB_R)
#ifdef ANA_DC_CALIB_L
    *dc_l = ANA_DC_CALIB_L;
#endif
#ifdef ANA_DC_CALIB_R
    *dc_r = ANA_DC_CALIB_R;
#endif
    ANALOG_INFO_TRACE(2,"ANA: OVERRIDE DC CALIB L=0x%04hX/%d R=0x%04hX/%d", *dc_l, dc_calib_val_decode(*dc_l), *dc_r, dc_calib_val_decode(*dc_r));
#endif

    return;
}

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
static void analog_aud_dc_calib_init(void)
{
    uint16_t val;
    int16_t dc_l, dc_r;

    analog_aud_get_dc_calib_value(&dc_l, &dc_r);

    analog_read(ANA_REG_6F, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL, dc_l);
    analog_write(ANA_REG_6F, val);

    analog_read(ANA_REG_70, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITR, dc_r);
    analog_write(ANA_REG_70, val);
}

static void analog_aud_dc_calib_enable(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_6E, &val);
    val |= REG_CODEC_TX_EAR_OFFEN;
    analog_write(ANA_REG_6E, val);
}
#endif

static void analog_aud_vcodec_enable(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (vcodec_map == 0) {
            set = true;
        }
        vcodec_map |= user;
    } else {
        vcodec_map &= ~user;
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
    uint16_t val_68;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (codec_common_map == 0) {
            set = true;
        }
        codec_common_map |= user;
    } else {
        codec_common_map &= ~user;
        if (codec_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_68, &val_68);
        if (codec_common_map) {
            val_68 |= REG_CODEC_EN_VCM;
        } else {
            val_68 &= ~(REG_CODEC_EN_VCM | REG_CODEC_VCM_EN_LPF);
        }
        if (codec_common_map) {
            // Quick startup:
            // 1) Disable VCM LPF and target to a higher voltage than the required one
            // 2) Wait for a short time when VCM is in quick charge (high voltage)
            // 3) Enable VCM LPF and target to the required VCM LPF voltage
            analog_write(ANA_REG_68, SET_BITFIELD(val_68, REG_CODEC_VCM_LOW_VCM, 0));
            uint32_t delay;

#if defined(VCM_CAP_100NF)
            if (vcodec_mv >= 2500) {
                delay = 6;
            } else {
                delay = 10;
            }
#else
            if (vcodec_mv >= 2500) {
                delay = 10;
            } else {
                delay = 50;
            }
#endif
            osDelay(delay);
#if 0
            // Target to a voltage near the required one
            analog_write(ANA_REG_68, val_68);
            osDelay(10);
#endif
            val_68 |= REG_CODEC_VCM_EN_LPF;
        }
        analog_write(ANA_REG_68, val_68);
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
#if PU_TX_REGULATOR
    uint16_t val_72;
#endif
    uint16_t val_67, val_68;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (adda_common_map == 0) {
            set = true;
        }
        adda_common_map |= user;
    } else {
        adda_common_map &= ~user;
        if (adda_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_67, &val_67);
        analog_read(ANA_REG_68, &val_68);
#if PU_TX_REGULATOR
        analog_read(ANA_REG_72, &val_72);
#endif
        if (adda_common_map) {
            val_67 |= REG_CODEC_EN_BIAS;
            analog_write(ANA_REG_67, val_67);
            val_68 |= REG_CODEC_EN_VCM_BUFFER;
#if PU_TX_REGULATOR
            val_72 |= REG_PU_TX_REGULATOR;
            analog_write(ANA_REG_72, val_72);
            hal_sys_timer_delay_us(10);
            val_72 &= ~REG_BYPASS_TX_REGULATOR;
            analog_write(ANA_REG_72, val_72);
#endif
        } else {
#if PU_TX_REGULATOR
            val_72 |= REG_BYPASS_TX_REGULATOR;
            analog_write(ANA_REG_72, val_72);
            val_72 &= ~REG_PU_TX_REGULATOR;
            analog_write(ANA_REG_72, val_72);
#endif
            val_68 &= ~(REG_CODEC_EN_VCM_BUFFER);
            val_67 &= ~REG_CODEC_EN_BIAS;
            analog_write(ANA_REG_67, val_67);
        }
        analog_write(ANA_REG_68, val_68);
    }
}

static void analog_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev)
{
    uint32_t lock;
    enum ANA_CODEC_USER_T old_map;
    bool set = false;
    int i;
    uint32_t pmu_map = 0;

    lock = int_lock();

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (dev & (AUD_VMIC_MAP_VMIC1 << i)) {
            if (vmic_map[i] == 0) {
                set = true;
            }
            vmic_map[i] |= user;
        } else {
            old_map = vmic_map[i];
            vmic_map[i] &= ~user;
            if (old_map != 0 && vmic_map[i] == 0) {
                set = true;
            }
        }

        if (vmic_map[i]) {
            pmu_map |= (AUD_VMIC_MAP_VMIC1 << i);
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

uint32_t analog_aud_get_max_dre_gain(void)
{
    if (vcodec_mv >= 2800) {
        return 0xC; // 6=-4dB
    } else if (vcodec_mv >= 2500) {
        return 0x11; // 0xB=-4dB
    } else if (vcodec_mv >= 1800) {
        return 0xE; // -4dB
    } else {
        return 0xF;
    }
}

int analog_reset(void)
{
    return 0;
}

void analog_open(void)
{
    uint16_t val;

    val = REG_CODEC_ADCA_GAIN_BIT(3)|REG_CODEC_ADCA_CH_SEL(1)|CFG_RESET_PGAA_DR;
    analog_write(ANA_REG_61, val);
#ifdef CHIP_BEST1402
    val = REG_CODEC_ADCB_GAIN_BIT(3)|REG_CODEC_ADCB_CH_SEL(1)|CFG_RESET_PGAB_DR;
    analog_write(ANA_REG_75, val);
#endif

#if 0
    val = REG_CODEC_ADC_IBSEL_REG(8) | REG_CODEC_ADC_IBSEL_VCOMP(8) | REG_CODEC_ADC_IBSEL_VREF(8) | REG_CODEC_ADC_LPFVCM_SW(7);
    analog_write(ANA_REG_64, val);
#endif

    val = REG_CODEC_ADC_OP1_IBIT(2) | REG_CODEC_ADC_VREF_SEL(7);
    analog_write(ANA_REG_65, val);

    val = REG_CODEC_BIAS_IBSEL_VOICE(8);
    if (vcodec_mv >= 2800) {
        val |= REG_CODEC_BIAS_IBSEL(6) | REG_CODEC_BIAS_IBSEL_TX(5);
    } else if (vcodec_mv >= 2500) {
        val |= REG_CODEC_BIAS_IBSEL(8) | REG_CODEC_BIAS_IBSEL_TX(8);
    } else if (vcodec_mv >= 1800) {
        val |= REG_CODEC_BIAS_IBSEL(8) | REG_CODEC_BIAS_IBSEL_TX(0xC);
    } else {
        val |= REG_CODEC_BIAS_IBSEL(8) | REG_CODEC_BIAS_IBSEL_TX(1);
    }
    analog_write(ANA_REG_66, val);

    if (vcodec_mv >= 1800) {
        val = REG_CODEC_ADC_REG_VSEL(3) | REG_CODEC_BUF_LOWVCM(4);
    } else {
        val = REG_CODEC_ADC_REG_VSEL(7) | REG_CODEC_BUF_LOWVCM(4)
                | REG_CODEC_EN_TX_EXT | REG_CODEC_DAC_CLK_EDGE_SEL;
    }
    analog_write(ANA_REG_67, val);

    uint16_t vcm, vcm_lpf;
    if (vcodec_mv >= 1800) {
        vcm = vcm_lpf = 7;
    } else {
        vcm = vcm_lpf = 7;
    }
    val = REG_CODEC_VCM_LOW_VCM(vcm) | REG_CODEC_VCM_LOW_VCM_LP(vcm) | REG_CODEC_VCM_LOW_VCM_LPF(vcm_lpf);
    analog_write(ANA_REG_68, val);

    val = REG_CODEC_RX_VTOI_I_DAC2(4);
    if (vcodec_mv >= 2500) {
        val |= REG_CODEC_RX_VTOI_IDAC_SEL(7) | REG_CODEC_RX_VTOI_VCS_SEL(0xC);
    } else if (vcodec_mv >= 1800) {
        val |= REG_CODEC_RX_VTOI_IDAC_SEL(7) | REG_CODEC_RX_VTOI_VCS_SEL(0xC);
    } else {
        val |= REG_CODEC_RX_VTOI_IDAC_SEL(7) | REG_CODEC_RX_VTOI_VCS_SEL(0x18);
    }
    analog_write(ANA_REG_69, val);

    if (vcodec_mv >= 2800) {
        val = REG_CODEC_TX_DAC_VREF_L(7) | REG_CODEC_TX_DAC_VREF_R(7);
    } else if (vcodec_mv >= 2500) {
        val = REG_CODEC_TX_DAC_VREF_L(1) | REG_CODEC_TX_DAC_VREF_R(1);
    } else if (vcodec_mv >= 1800) {
        val = REG_CODEC_TX_DAC_VREF_L(8) | REG_CODEC_TX_DAC_VREF_R(8);
    } else {
        val = REG_CODEC_TX_DAC_VREF_L(2) | REG_CODEC_TX_DAC_VREF_R(2);
    }
    analog_write(ANA_REG_6A, val);

    if (vcodec_mv >= 2500) {
        val = 0;
    } else if (vcodec_mv >= 1800) {
        val = REG_CODEC_TX_EAR_COMP_L(0x18) | REG_CODEC_TX_EAR_COMP_R(0x18);
    } else {
        val = 0;
    }
    analog_write(ANA_REG_6B, val);

    val = REG_CODEC_TX_EAR_FBCAP(3) | REG_CODEC_TX_EAR_IBSEL(1);
    if (vcodec_mv >= 1800) {
        val |= REG_CODEC_TX_EAR_DR_ST(2);
    } else {
#ifdef CHIP_BEST1402
        val |= REG_CODEC_TX_EAR_DR_ST(7);
#else
        val |= REG_CODEC_TX_EAR_DR_ST(1);
#endif
    }
    analog_write(ANA_REG_6C, val);

    uint16_t dre_gain = analog_aud_get_max_dre_gain();
    val = REG_CODEC_TX_EAR_DRE_GAIN_L(dre_gain) | REG_CODEC_TX_EAR_DRE_GAIN_R(dre_gain) | REG_CODEC_TX_EAR_GAIN(1) |
        DRE_GAIN_SEL_L | DRE_GAIN_SEL_R;
#ifdef DAC_DRE_GAIN_DC_UPDATE
    val |= REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE | REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE;
#endif
    analog_write(ANA_REG_6D, val);

    val = REG_CODEC_TX_EAR_SOFTSTART(8) | REG_CODEC_TX_EAR_OCEN;
    if (vcodec_mv >= 2500) {
        val |= REG_CODEC_TX_EAR_OUTPUTSEL(4);
    } else if (vcodec_mv >= 1800) {
        val |= REG_CODEC_TX_EAR_OUTPUTSEL(1);
    } else {
#ifdef CHIP_BEST1402
        val |= REG_CODEC_TX_EAR_OUTPUTSEL(1);
#else
        val |= REG_CODEC_TX_EAR_OUTPUTSEL(4);
#endif
    }
    analog_write(ANA_REG_6E, val);

    val = REG_DAC_LDO0P9_VSEL(2) | REG_BYPASS_TX_REGULATOR | REG_CODEC_TX_EAR_VCM_SEL;
    if (vcodec_mv >= 1800) {
        val |= REG_TX_REGULATOR_BIT(5) | REG_CODEC_TX_EAR_VCM_BIT(2);
    } else {
#ifdef CHIP_BEST1402
        val |= REG_TX_REGULATOR_BIT(4) | REG_CODEC_TX_EAR_VCM_BIT(2);
#else
        val |= REG_TX_REGULATOR_BIT(4);
        if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5){
            val |= REG_CODEC_TX_EAR_VCM_BIT(1);
        }else{
            val |= REG_CODEC_TX_EAR_VCM_BIT(3);
        }
#endif
    }
    analog_write(ANA_REG_72, val);

#ifdef CHIP_BEST1402
    if (hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_1) {
        analog_read(ANA_REG_16A, &val);
        val = (val & ~(REG_CODEC_TX_VREFBUF_LP_L_MASK | REG_CODEC_TX_VREFBUF_LP_R_MASK)) |
            REG_CODEC_TX_VREFBUF_LP_L(0) | REG_CODEC_TX_VREFBUF_LP_R(0);
        analog_write(ANA_REG_16A, val);
    }
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    analog_aud_dc_calib_init();
    analog_aud_dc_calib_enable(true);
#endif

#ifdef VCM_ON
    analog_aud_enable_common_internal(ANA_CODEC_USER_DAC, true);
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

    user = 0;
    ch_map = 0;
#if defined(ANC_FF_MIC_CH_L) || defined(ANC_FF_MIC_CH_R)
    if (type & ANC_FEEDFORWARD) {
        user |= ANA_CODEC_USER_ANC_FF;
        ch_map |= ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R;
    }
#endif

    ANALOG_DEBUG_TRACE(0, "%s: type=%d en=%d ch_map=0x%x", __func__, type, en, ch_map);

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

void analog_aud_mickey_enable(bool en)
{
    if (en) {
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV);
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, true);
    } else {
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, 0);
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, false);
    }
}

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    if (en) {
        dev = hal_codec_get_input_path_cfg(input_path);
        // Enable vmic first to overlap vmic stable time with codec vcm stable time
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

#ifdef _AUTO_SWITCH_POWER_MODE__
    //pmu_mode_change(PMU_POWER_MODE_DIG_DCDC);
#endif
}

void analog_aud_codec_close(void)
{
    static const enum AUD_CHANNEL_MAP_T all_ch = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 |
        AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4;

    analog_aud_codec_speaker_enable(false);
    osDelay(1);
    analog_aud_codec_dac_enable(false);

    analog_aud_codec_adc_enable(AUD_IO_PATH_NULL, all_ch, false);

#ifdef _AUTO_SWITCH_POWER_MODE__
    //pmu_mode_change(PMU_POWER_MODE_ANA_DCDC);
#endif

    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, false);
}

void analog_aud_codec_mute(void)
{
#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_codec_tx_pa_gain_sel(0);
#endif

    ana_spk_muted = true;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_nomute(void)
{
    ana_spk_muted = false;
    analog_aud_codec_config_speaker();

#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_aud_set_dac_gain(dac_gain);
#endif
}

int analog_debug_config_audio_output(bool diff)
{
    return 0;
}

int analog_debug_config_codec(uint16_t mv)
{
#ifdef ANC_PROD_TEST
    int ret;

    ret = pmu_debug_config_codec(mv);
    if (ret) {
        return ret;
    }

    vcodec_mv = mv;
#endif
    return 0;
}

int analog_debug_config_low_power_adc(bool enable)
{
    return 0;
}

void analog_debug_config_anc_calib_mode(bool enable)
{
    anc_calib_mode = enable;
}

bool analog_debug_get_anc_calib_mode(void)
{
    return anc_calib_mode;
}

void analog_aud_pll_tune(float ratio)
{
}

#define PMU_REG_DUMP(addr) do{ \
                                uint16_t pmu_reg_val = 0; \
                                pmu_read(addr, &pmu_reg_val); \
                                TRACE_IMM(2,"pmu reg addr:%04x val:%04x", addr, pmu_reg_val); \
                            }while(0);

#define ANALOG_REG_DUMP(addr) do{ \
                                uint16_t ana_reg_val = 0; \
                                analog_read(addr, &ana_reg_val); \
                                TRACE_IMM(2,"analog reg addr:%04x val:%04x", addr, ana_reg_val); \
                            }while(0);

#define DIG_DUMP(addr) do{ \
                            uint32_t dig_reg_val = 0; \
                            dig_reg_val = (*(volatile uint32_t *)(addr)); \
                            TRACE_IMM(2,"dig reg addr:%08x val:%08x", addr, dig_reg_val); \
                        }while(0);


void analog_productiontest_settings_checker(void)
{
#if 0
mail by haichengduan@bestechnic.com @ 2020/05/05/ 15:25
1402_reg_config_DAC_1K.txt

PMU
        ("write","<0x49>","<0xA088>");
        ("write","<0x44>","<0x8354>");//****//


ANA
        ("write","<0x6b>","<0x0000>");
        ("write","<0x6c>","<0x01ea>");
        ("write","<0x6e>","<0x9088>");
        ("write","<0x61>","<0x0385>");
        ("write","<0x75>","<0x0385>");
        ("write","<0x60>","<0xa010>");  // page 1
        ("write","<0x6a>","<0x48dc>");  // 20200323 by dhc; tianxin thd improve 0x00dc;20200425 return x048dc
        ("write","<0x60>","<0xa000>");  // page 0

DIG
        ("write","<0x40000010>","<0x19900000>");
        ("write","<0x40000074>","<0x00000011>");  //20200103 by dhc  zenghua dac fanyan
        ("write","<0x40000048>","<0x19900000>");
        ("write","<0x40300088>","<0x00870207>");
        ("write","<0x4030008c>","<0x00104000>");
        ("write","<0x40300090>","<0x00104000>");
        ("write","<0x40300094>","<0x00000001>");
/********************************************************************************************************/

1402_reg_config_ADDA_loop.txt
    mail by haichengduan@bestechnic.com @ 2020/05/05/ 15:25
/********************************************************************************************************/

    ANA
    "<0x68>","<0xbbbb>");//0xbbbb
    "<0x67>","<0x0300>");
    "<0x71>","<0x9fff>");
    "<0x6c>","<0x01ea>");
    "<0x73>","<0x0a00>");
    "<0x6e>","<0x9088>");
    "<0x75>","<0x0385>");
    "<0x60>","<0xa010>");  // page 1
    "<0x6a>","<0x48dc>");  // 20200323 by dhc; tianxin thd improve 0x00dc;20200425 return x048dc
    "<0x60>","<0xa000>");  // page 0



    DIG
    "<0x40000010>","<0x19900000>");
    "<0x40000048>","<0x19900000>");
    "<0x40300088>","<0x00870207>");
    "<0x4030008c>","<0x00104000>");
    "<0x40300090>","<0x00104000>");
    "<0x40300094>","<0x00000001>");

    DIG
    "<0x40000010>","<0x1f900000>");
    "<0x40000048>","<0x1f900000>");
    "<0x40300080>","<0x00040DFF>");
    "<0x40300084>","<0x00100008>");
    "<0x40300180>","<0x0010000A>");
    "<0x40300080>","<0x00046DFF>");
    "<0x40300088>","<0x00850207>");
    "<0x40000074>","<0x00000019>");
    "<0x40300094>","<0x00000000>");
    "<0x4030008c>","<0x00104000>");
    "<0x40300090>","<0x00104000>");
    "<0x40300094>","<0x00000001>");

    PMU
    "<0x29>","<0x216e>");
    "<0x2A>","<0x0174>");

    ANA
    ,"<0x69>","<0x0c79>");//0xbbbb
    ,"<0x61>","<0x0385>");
    ,"<0x67>","<0x0304>");
/********************************************************************************************************/
#endif

    ANALOG_REG_DUMP(0x6B);
    ANALOG_REG_DUMP(0x6C);
    ANALOG_REG_DUMP(0x6E);
    ANALOG_REG_DUMP(0x61);
    ANALOG_REG_DUMP(0x75);
    ANALOG_REG_DUMP(0x16A);

    DIG_DUMP(0x40000010);
    DIG_DUMP(0x40000074);
    DIG_DUMP(0x40000048);
    DIG_DUMP(0x40300088);
    DIG_DUMP(0x4030008c);
    DIG_DUMP(0x40300090);
    DIG_DUMP(0x40300094);

    ANALOG_REG_DUMP(0x68);
    ANALOG_REG_DUMP(0x67);
    ANALOG_REG_DUMP(0x71);
    ANALOG_REG_DUMP(0x6c);
    ANALOG_REG_DUMP(0x73);
    ANALOG_REG_DUMP(0x6E);
    ANALOG_REG_DUMP(0x75);
    ANALOG_REG_DUMP(0x16A);

    DIG_DUMP(0x40000010);
    DIG_DUMP(0x40000048);
    DIG_DUMP(0x40300088);
    DIG_DUMP(0x4030008c);
    DIG_DUMP(0x40300090);
    DIG_DUMP(0x40300094);

    DIG_DUMP(0x40000010);
    DIG_DUMP(0x40000048);
    DIG_DUMP(0x40300084);
    DIG_DUMP(0x40300080);
    DIG_DUMP(0x40300088);
    DIG_DUMP(0x40000074);
    DIG_DUMP(0x40300094);
    DIG_DUMP(0x4030008c);
    DIG_DUMP(0x40300090);
    DIG_DUMP(0x40300094);

    PMU_REG_DUMP(0x29);
    PMU_REG_DUMP(0x2A);

    ANALOG_REG_DUMP(0x69);
    ANALOG_REG_DUMP(0x61);
    ANALOG_REG_DUMP(0x67);
}
