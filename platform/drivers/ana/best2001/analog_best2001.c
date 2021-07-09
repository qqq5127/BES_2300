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
#include CHIP_SPECIFIC_HDR(pll)

#define ANALOG_TRACE(n, s, ...)             //TRACE(n, s, ##__VA_ARGS__)

#define VCM_ON

//DAC_HIGH_OUTPUT is defined at Makefile when VCODEC=1.9V

// Not using 1uF
#define VCM_CAP_100NF

#define LOW_CODEC_BIAS

#define DAC_DC_CALIB_BIT_WIDTH              14

#define DEFAULT_VOICE_ADC_GAIN_DB           0

#ifndef ANALOG_ADC_A_GAIN_DB
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif

#ifndef ANALOG_ADC_B_GAIN_DB
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif

#ifndef ANALOG_ADC_C_GAIN_DB
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif

#ifndef LINEIN_ADC_GAIN_DB
#define LINEIN_ADC_GAIN_DB                  0
#endif

#ifndef CFG_HW_AUD_MICKEY_DEV
#define CFG_HW_AUD_MICKEY_DEV               (AUD_VMIC_MAP_VMIC1)
#endif

// REG_60
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_61
#define CFG_TX_CLK_INV                      (1 << 0)
#define REG_CODEC_TX_EN_LDAC                (1 << 1)
#define REG_CODEC_TX_EN_RDAC                (1 << 2)
#define CFG_TX_TREE_EN                      (1 << 3)
#define REG_CODEC_TX_EN_LPPA                (1 << 4)
#define REG_CODEC_TX_EN_EARPA_L             (1 << 5)
#define REG_CODEC_TX_EN_EARPA_R             (1 << 6)
#define CFG_TX_PEAK_OFF_ADC_EN              (1 << 7)
#define CFG_TX_PEAK_OFF_DAC_EN              (1 << 8)
#define CFG_TX_CH0_MUTE                     (1 << 9)
#define CFG_TX_CH1_MUTE                     (1 << 10)
#define REG_CODEC_TX_PU_ZERO_DET_L          (1 << 11)
#define REG_CODEC_TX_PU_ZERO_DET_R          (1 << 12)
#define REG_IDETLEAR_EN                     (1 << 13)
#define REG_IDETREAR_EN                     (1 << 14)
#define RESERVED_ANA_0                      (1 << 15)

// REG_62
#define CODEC_DIN_L_HI_RSTVAL_SHIFT         0
#define CODEC_DIN_L_HI_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_HI_RSTVAL_SHIFT)
#define CODEC_DIN_L_HI_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_HI_RSTVAL, n)

// REG_63
#define CODEC_DIN_L_LO_RSTVAL_SHIFT         0
#define CODEC_DIN_L_LO_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_LO_RSTVAL_SHIFT)
#define CODEC_DIN_L_LO_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_LO_RSTVAL, n)

// REG_64
#define CODEC_DIN_R_HI_RSTVAL_SHIFT         0
#define CODEC_DIN_R_HI_RSTVAL_MASK          (0xFFFF << CODEC_DIN_R_HI_RSTVAL_SHIFT)
#define CODEC_DIN_R_HI_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_R_HI_RSTVAL, n)

// REG_65
#define CODEC_DIN_R_LO_RSTVAL_SHIFT         0
#define CODEC_DIN_R_LO_RSTVAL_MASK          (0xFFFF << CODEC_DIN_R_LO_RSTVAL_SHIFT)
#define CODEC_DIN_R_LO_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_R_LO_RSTVAL, n)

// REG_66
#define DRE_GAIN_SEL_L                      (1 << 0)
#define DRE_GAIN_SEL_R                      (1 << 1)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT   2
#define REG_CODEC_TX_EAR_DRE_GAIN_L_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L, n)
#define REG_CODEC_TX_EAR_DRE_GAIN_R_SHIFT   7
#define REG_CODEC_TX_EAR_DRE_GAIN_R_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_R_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_R(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_R, n)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE  (1 << 12)
#define REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE  (1 << 13)
#define REG_ZERO_DETECT_POWER_DOWN_DIRECT   (1 << 14)
#define REG_ZERO_DETECT_POWER_DOWN          (1 << 15)

// REG_67
#define REG_ZERO_DETECT_CHANGE              (1 << 0)
#define REG_CLOSE_PA                        (1 << 1)
#define REG_CLOSE_PA_DIRECT                 (1 << 2)
#define REG_CLOSE_SPA                       (1 << 3)
#define REG_CODEC_TX_SW_MODE                (1 << 4)
#define REG_CODEC_TX_EAR_GAIN_SHIFT         5
#define REG_CODEC_TX_EAR_GAIN_MASK          (0x3 << REG_CODEC_TX_EAR_GAIN_SHIFT)
#define REG_CODEC_TX_EAR_GAIN(n)            BITFIELD_VAL(REG_CODEC_TX_EAR_GAIN, n)
#define REG_CODEC_TX_EAR_IBSEL_SHIFT        7
#define REG_CODEC_TX_EAR_IBSEL_MASK         (0x3 << REG_CODEC_TX_EAR_IBSEL_SHIFT)
#define REG_CODEC_TX_EAR_IBSEL(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_IBSEL, n)
#define CFG_PEAK_DET_DB_DELAY_SHIFT         9
#define CFG_PEAK_DET_DB_DELAY_MASK          (0x7 << CFG_PEAK_DET_DB_DELAY_SHIFT)
#define CFG_PEAK_DET_DB_DELAY(n)            BITFIELD_VAL(CFG_PEAK_DET_DB_DELAY, n)
#define RESERVED_ANA_4_1_SHIFT              12
#define RESERVED_ANA_4_1_MASK               (0xF << RESERVED_ANA_4_1_SHIFT)
#define RESERVED_ANA_4_1(n)                 BITFIELD_VAL(RESERVED_ANA_4_1, n)

// REG_68
#define CFG_PEAK_DET_DR                     (1 << 0)
#define CFG_TX_PEAK_OFF_ADC                 (1 << 1)
#define CFG_TX_PEAK_OFF_DAC                 (1 << 2)
#define REG_CODEC_TX_PEAK_NL_EN             (1 << 3)
#define REG_CODEC_TX_PEAK_NR_EN             (1 << 4)
#define REG_CODEC_TX_PEAK_PL_EN             (1 << 5)
#define REG_CODEC_TX_PEAK_PR_EN             (1 << 6)
#define TX_PEAK_DET_STATUS                  (1 << 7)
#define TX_PEAK_DET_NL_STATUS               (1 << 8)
#define TX_PEAK_DET_NR_STATUS               (1 << 9)
#define TX_PEAK_DET_PL_STATUS               (1 << 10)
#define TX_PEAK_DET_PR_STATUS               (1 << 11)
#define REG_CODEC_TX_EN_DACLDO              (1 << 12)
#define REG_CODEC_TX_EN_LCLK                (1 << 13)
#define REG_CODEC_TX_EN_RCLK                (1 << 14)
#define RESERVED_ANA_5                      (1 << 15)

// REG_69
#define REG_CODEC_TX_EN_S1PA                (1 << 0)
#define REG_CODEC_TX_EN_S2PA                (1 << 1)
#define REG_CODEC_TX_EN_S3PA                (1 << 2)
#define REG_CODEC_TX_EN_S4PA                (1 << 3)
#define REG_CODEC_TX_EN_S5PA                (1 << 4)
#define REG_CODEC_TX_EAR_OCEN               (1 << 5)
#define REG_CODEC_TX_EAR_LPBIAS             (1 << 6)
#define REG_CODEC_TX_EAR_ENBIAS             (1 << 7)
#define REG_CODEC_TX_EAR_DR_EN              (1 << 8)
#define REG_PU_TX_REGULATOR                 (1 << 9)
#define REG_BYPASS_TX_REGULATOR             (1 << 10)
#define REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT    11
#define REG_CODEC_TX_EAR_OUTPUTSEL_MASK     (0xF << REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT)
#define REG_CODEC_TX_EAR_OUTPUTSEL(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_OUTPUTSEL, n)
#define REG_CODEC_TX_EAR_DOUBLEBIAS         (1 << 15)

// REG_6A
#define REG_CODEC_DAC_CLK_EDGE_SEL          (1 << 0)
#define REG_CODEC_TX_DAC_SWR_SHIFT          1
#define REG_CODEC_TX_DAC_SWR_MASK           (0x3 << REG_CODEC_TX_DAC_SWR_SHIFT)
#define REG_CODEC_TX_DAC_SWR(n)             BITFIELD_VAL(REG_CODEC_TX_DAC_SWR, n)
#define REG_CODEC_TX_EAR_OFFEN              (1 << 3)
#define REG_CODEC_TEST_SEL_SHIFT            4
#define REG_CODEC_TEST_SEL_MASK             (0x7 << REG_CODEC_TEST_SEL_SHIFT)
#define REG_CODEC_TEST_SEL(n)               BITFIELD_VAL(REG_CODEC_TEST_SEL, n)
#define REG_DAC_LDO0P9_VSEL_SHIFT           7
#define REG_DAC_LDO0P9_VSEL_MASK            (0x1F << REG_DAC_LDO0P9_VSEL_SHIFT)
#define REG_DAC_LDO0P9_VSEL(n)              BITFIELD_VAL(REG_DAC_LDO0P9_VSEL, n)
#define REG_CODEC_TX_REG_CAP_BIT            (1 << 12)
#define REG_CODEC_TX_DAC_MUTEL              (1 << 13)
#define REG_CODEC_TX_DAC_MUTER              (1 << 14)
#define RESERVED_ANA_6                      (1 << 15)

// REG_6B
#define REG_CODEC_TX_VREF_CAP_BIT           (1 << 0)
#define REG_CODEC_TX_VREFBUF_R_LP_SHIFT     1
#define REG_CODEC_TX_VREFBUF_R_LP_MASK      (0x7 << REG_CODEC_TX_VREFBUF_R_LP_SHIFT)
#define REG_CODEC_TX_VREFBUF_R_LP(n)        BITFIELD_VAL(REG_CODEC_TX_VREFBUF_R_LP, n)
#define REG_CODEC_TX_VREFBUF_L_LP_SHIFT     4
#define REG_CODEC_TX_VREFBUF_L_LP_MASK      (0x7 << REG_CODEC_TX_VREFBUF_L_LP_SHIFT)
#define REG_CODEC_TX_VREFBUF_L_LP(n)        BITFIELD_VAL(REG_CODEC_TX_VREFBUF_L_LP, n)
#define REG_CODEC_TX_EAR_SOFTSTART_SHIFT    7
#define REG_CODEC_TX_EAR_SOFTSTART_MASK     (0x3F << REG_CODEC_TX_EAR_SOFTSTART_SHIFT)
#define REG_CODEC_TX_EAR_SOFTSTART(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_SOFTSTART, n)
#define REG_CODEC_TX_EAR_LCAL               (1 << 13)
#define REG_CODEC_TX_EAR_RCAL               (1 << 14)
#define RESERVED_ANA_7                      (1 << 15)

// REG_6C
#define REG_CODEC_TX_EAR_DR_ST_R_SHIFT      0
#define REG_CODEC_TX_EAR_DR_ST_R_MASK       (0x7 << REG_CODEC_TX_EAR_DR_ST_R_SHIFT)
#define REG_CODEC_TX_EAR_DR_ST_R(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_DR_ST_R, n)
#define REG_CODEC_TX_EAR_DR_ST_L_SHIFT      3
#define REG_CODEC_TX_EAR_DR_ST_L_MASK       (0x7 << REG_CODEC_TX_EAR_DR_ST_L_SHIFT)
#define REG_CODEC_TX_EAR_DR_ST_L(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_DR_ST_L, n)
#define REG_CODEC_TX_EAR_FBCAP_SHIFT        6
#define REG_CODEC_TX_EAR_FBCAP_MASK         (0x3 << REG_CODEC_TX_EAR_FBCAP_SHIFT)
#define REG_CODEC_TX_EAR_FBCAP(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_FBCAP, n)
#define REG_CODEC_TX_EAR_LOWGAINL_SHIFT     8
#define REG_CODEC_TX_EAR_LOWGAINL_MASK      (0x3 << REG_CODEC_TX_EAR_LOWGAINL_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINL, n)
#define REG_CODEC_TX_EAR_LOWGAINR_SHIFT     10
#define REG_CODEC_TX_EAR_LOWGAINR_MASK      (0x3 << REG_CODEC_TX_EAR_LOWGAINR_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINR, n)
#define REG_CODEC_TX_EAR_DIS_SHIFT          12
#define REG_CODEC_TX_EAR_DIS_MASK           (0x3 << REG_CODEC_TX_EAR_DIS_SHIFT)
#define REG_CODEC_TX_EAR_DIS(n)             BITFIELD_VAL(REG_CODEC_TX_EAR_DIS, n)
#define REG_CODEC_TX_EAR_VCM_BIT_SHIFT      14
#define REG_CODEC_TX_EAR_VCM_BIT_MASK       (0x3 << REG_CODEC_TX_EAR_VCM_BIT_SHIFT)
#define REG_CODEC_TX_EAR_VCM_BIT(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_VCM_BIT, n)

// REG_6D
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define CFG_DIV_100K_SHIFT                  7
#define CFG_DIV_100K_MASK                   (0xFF << CFG_DIV_100K_SHIFT)
#define CFG_DIV_100K(n)                     BITFIELD_VAL(CFG_DIV_100K, n)
#define RESERVED_ANA_8                      (1 << 15)

// REG_6E
#define TX_TIMER0_SHIFT                     0
#define TX_TIMER0_MASK                      (0xFFFF << TX_TIMER0_SHIFT)
#define TX_TIMER0(n)                        BITFIELD_VAL(TX_TIMER0, n)

// REG_6F
#define TX_TIMER1_SHIFT                     0
#define TX_TIMER1_MASK                      (0xFFFF << TX_TIMER1_SHIFT)
#define TX_TIMER1(n)                        BITFIELD_VAL(TX_TIMER1, n)

// REG_70
#define REG_CODEC_TX_EAR_VCM_SEL            (1 << 0)
#define REG_CODEC_TX_PEAK_DET_BIT           (1 << 1)
#define REG_CODEC_TX_EAR_CASN_L_SHIFT       2
#define REG_CODEC_TX_EAR_CASN_L_MASK        (0x7 << REG_CODEC_TX_EAR_CASN_L_SHIFT)
#define REG_CODEC_TX_EAR_CASN_L(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_CASN_L, n)
#define REG_CODEC_TX_EAR_CASN_R_SHIFT       5
#define REG_CODEC_TX_EAR_CASN_R_MASK        (0x7 << REG_CODEC_TX_EAR_CASN_R_SHIFT)
#define REG_CODEC_TX_EAR_CASN_R(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_CASN_R, n)
#define REG_CODEC_TX_EAR_CASP_L_SHIFT       8
#define REG_CODEC_TX_EAR_CASP_L_MASK        (0x7 << REG_CODEC_TX_EAR_CASP_L_SHIFT)
#define REG_CODEC_TX_EAR_CASP_L(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_CASP_L, n)
#define REG_CODEC_TX_EAR_CASP_R_SHIFT       11
#define REG_CODEC_TX_EAR_CASP_R_MASK        (0x7 << REG_CODEC_TX_EAR_CASP_R_SHIFT)
#define REG_CODEC_TX_EAR_CASP_R(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_CASP_R, n)
#define RESERVED_ANA_10_9_SHIFT             14
#define RESERVED_ANA_10_9_MASK              (0x3 << RESERVED_ANA_10_9_SHIFT)
#define RESERVED_ANA_10_9(n)                BITFIELD_VAL(RESERVED_ANA_10_9, n)

// REG_71
#define REG_CODEC_TX_EAR_COMP_L_SHIFT       0
#define REG_CODEC_TX_EAR_COMP_L_MASK        (0x7F << REG_CODEC_TX_EAR_COMP_L_SHIFT)
#define REG_CODEC_TX_EAR_COMP_L(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_COMP_L, n)
#define REG_CODEC_TX_EAR_COMP_R_SHIFT       7
#define REG_CODEC_TX_EAR_COMP_R_MASK        (0x7F << REG_CODEC_TX_EAR_COMP_R_SHIFT)
#define REG_CODEC_TX_EAR_COMP_R(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_COMP_R, n)
#define REG_CODEC_TX_EAR_EN_CM_COMP         (1 << 14)
#define RESERVED_ANA_11                     (1 << 15)

// REG_72
#define CFG_CODEC_DIN_L_RST                 (1 << 0)
#define CFG_CODEC_DIN_R_RST                 (1 << 1)
#define RESERVED_ANA_25_12_SHIFT            2
#define RESERVED_ANA_25_12_MASK             (0x3FFF << RESERVED_ANA_25_12_SHIFT)
#define RESERVED_ANA_25_12(n)               BITFIELD_VAL(RESERVED_ANA_25_12, n)

// REG_73
#define REG_CLKMUX_DVDD0P9_VSEL_SHIFT       0
#define REG_CLKMUX_DVDD0P9_VSEL_MASK        (0x1F << REG_CLKMUX_DVDD0P9_VSEL_SHIFT)
#define REG_CLKMUX_DVDD0P9_VSEL(n)          BITFIELD_VAL(REG_CLKMUX_DVDD0P9_VSEL, n)
#define REG_CLKMUX_DVDD_SEL                 (1 << 5)
#define REG_CLKMUX_PU_DVDD0P9               (1 << 6)
#define REG_CRYSTAL_SEL_LV                  (1 << 7)
#define REG_PU_OSC                          (1 << 8)
#define REG_CODEC_TX_VREFBUF_LOAD_SHIFT     9
#define REG_CODEC_TX_VREFBUF_LOAD_MASK      (0x7 << REG_CODEC_TX_VREFBUF_LOAD_SHIFT)
#define REG_CODEC_TX_VREFBUF_LOAD(n)        BITFIELD_VAL(REG_CODEC_TX_VREFBUF_LOAD, n)
#define RESERVED_ANA_29_26_SHIFT            12
#define RESERVED_ANA_29_26_MASK             (0xF << RESERVED_ANA_29_26_SHIFT)
#define RESERVED_ANA_29_26(n)               BITFIELD_VAL(RESERVED_ANA_29_26, n)

// REG_74
#define REG_TX_REGULATOR_BIT_SHIFT          0
#define REG_TX_REGULATOR_BIT_MASK           (0xF << REG_TX_REGULATOR_BIT_SHIFT)
#define REG_TX_REGULATOR_BIT(n)             BITFIELD_VAL(REG_TX_REGULATOR_BIT, n)
#define REG_CODEC_TX_DAC_VREF_L_SHIFT       4
#define REG_CODEC_TX_DAC_VREF_L_MASK        (0xF << REG_CODEC_TX_DAC_VREF_L_SHIFT)
#define REG_CODEC_TX_DAC_VREF_L(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_L, n)
#define REG_CODEC_TX_DAC_VREF_R_SHIFT       8
#define REG_CODEC_TX_DAC_VREF_R_MASK        (0xF << REG_CODEC_TX_DAC_VREF_R_SHIFT)
#define REG_CODEC_TX_DAC_VREF_R(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_R, n)
#define CALOUT_LEAR                         (1 << 12)
#define CALOUT_REAR                         (1 << 13)
#define RESERVED_ANA_31_30_SHIFT            14
#define RESERVED_ANA_31_30_MASK             (0x3 << RESERVED_ANA_31_30_SHIFT)
#define RESERVED_ANA_31_30(n)               BITFIELD_VAL(RESERVED_ANA_31_30, n)

// REG_75
#define REG_CODEC_TX_EAR_OFF_BITL_SHIFT     0
#define REG_CODEC_TX_EAR_OFF_BITL_MASK      (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL, n)
#define RESERVED_ANA_33_32_SHIFT            14
#define RESERVED_ANA_33_32_MASK             (0x3 << RESERVED_ANA_33_32_SHIFT)
#define RESERVED_ANA_33_32(n)               BITFIELD_VAL(RESERVED_ANA_33_32, n)

// REG_76
#define REG_CODEC_TX_EAR_OFF_BITR_SHIFT     0
#define REG_CODEC_TX_EAR_OFF_BITR_MASK      (0x3FFF << REG_CODEC_TX_EAR_OFF_BITR_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITR, n)
#define RESERVED_ANA_35_34_SHIFT            14
#define RESERVED_ANA_35_34_MASK             (0x3 << RESERVED_ANA_35_34_SHIFT)
#define RESERVED_ANA_35_34(n)               BITFIELD_VAL(RESERVED_ANA_35_34, n)

// REG_77
#define REG_VOICE_HIGH_TUNE_A_SHIFT         0
#define REG_VOICE_HIGH_TUNE_A_MASK          (0xF << REG_VOICE_HIGH_TUNE_A_SHIFT)
#define REG_VOICE_HIGH_TUNE_A(n)            BITFIELD_VAL(REG_VOICE_HIGH_TUNE_A, n)
#define REG_VOICE_HIGH_TUNE_B_SHIFT         4
#define REG_VOICE_HIGH_TUNE_B_MASK          (0xF << REG_VOICE_HIGH_TUNE_B_SHIFT)
#define REG_VOICE_HIGH_TUNE_B(n)            BITFIELD_VAL(REG_VOICE_HIGH_TUNE_B, n)
#define CFG_VOICE_RESET                     (1 << 8)
#define CFG_VOICE_TRIG_ENABLE               (1 << 9)
#define CFG_VOICE_POWER_ON                  (1 << 10)
#define CFG_VOICE_PU_DELAY_SHIFT            11
#define CFG_VOICE_PU_DELAY_MASK             (0x7 << CFG_VOICE_PU_DELAY_SHIFT)
#define CFG_VOICE_PU_DELAY(n)               BITFIELD_VAL(CFG_VOICE_PU_DELAY, n)
#define CFG_VOICE_DR                        (1 << 14)
#define RESERVED_ANA_36                     (1 << 15)

// REG_78
#define CFG_VOICE_PRECH_DELAY_SHIFT         0
#define CFG_VOICE_PRECH_DELAY_MASK          (0xFF << CFG_VOICE_PRECH_DELAY_SHIFT)
#define CFG_VOICE_PRECH_DELAY(n)            BITFIELD_VAL(CFG_VOICE_PRECH_DELAY, n)
#define CFG_VOICE_PRECH_DONE_DELAY_SHIFT    8
#define CFG_VOICE_PRECH_DONE_DELAY_MASK     (0x1F << CFG_VOICE_PRECH_DONE_DELAY_SHIFT)
#define CFG_VOICE_PRECH_DONE_DELAY(n)       BITFIELD_VAL(CFG_VOICE_PRECH_DONE_DELAY, n)
#define BT_PU_OSC                           (1 << 13)
#define RESERVED_ANA_38_37_SHIFT            14
#define RESERVED_ANA_38_37_MASK             (0x3 << RESERVED_ANA_38_37_SHIFT)
#define RESERVED_ANA_38_37(n)               BITFIELD_VAL(RESERVED_ANA_38_37, n)

// REG_79
#define CFG_VOICE_DET_DELAY_SHIFT           0
#define CFG_VOICE_DET_DELAY_MASK            (0xFF << CFG_VOICE_DET_DELAY_SHIFT)
#define CFG_VOICE_DET_DELAY(n)              BITFIELD_VAL(CFG_VOICE_DET_DELAY, n)
#define BT_PU_OSC_DELAY_SHIFT               8
#define BT_PU_OSC_DELAY_MASK                (0xFF << BT_PU_OSC_DELAY_SHIFT)
#define BT_PU_OSC_DELAY(n)                  BITFIELD_VAL(BT_PU_OSC_DELAY, n)

// REG_7A
#define VOICE_OUT_MASK_MODE                 (1 << 0)
#define VOICE_OUT_MASK_EN                   (1 << 1)
#define REG_VOICE_PU                        (1 << 2)
#define REG_VOICE_GAIN_BASE_SHIFT           3
#define REG_VOICE_GAIN_BASE_MASK            (0x7 << REG_VOICE_GAIN_BASE_SHIFT)
#define REG_VOICE_GAIN_BASE(n)              BITFIELD_VAL(REG_VOICE_GAIN_BASE, n)
#define REG_VOICE_GAIN_A_SHIFT              6
#define REG_VOICE_GAIN_A_MASK               (0x7 << REG_VOICE_GAIN_A_SHIFT)
#define REG_VOICE_GAIN_A(n)                 BITFIELD_VAL(REG_VOICE_GAIN_A, n)
#define REG_VOICE_GAIN_B_SHIFT              9
#define REG_VOICE_GAIN_B_MASK               (0x7 << REG_VOICE_GAIN_B_SHIFT)
#define REG_VOICE_GAIN_B(n)                 BITFIELD_VAL(REG_VOICE_GAIN_B, n)
#define REG_VOICE_LDO_VBIT_SHIFT            12
#define REG_VOICE_LDO_VBIT_MASK             (0x7 << REG_VOICE_LDO_VBIT_SHIFT)
#define REG_VOICE_LDO_VBIT(n)               BITFIELD_VAL(REG_VOICE_LDO_VBIT, n)
#define REG_VOICE_CLK_EDGE_SEL              (1 << 15)

// REG_7B
#define REG_VOICE_PRECH                     (1 << 0)
#define REG_VOICE_PU_LDO                    (1 << 1)
#define REG_VOICE_PU_OSC                    (1 << 2)
#define REG_VOICE_OSC_FR_SHIFT              3
#define REG_VOICE_OSC_FR_MASK               (0x7 << REG_VOICE_OSC_FR_SHIFT)
#define REG_VOICE_OSC_FR(n)                 BITFIELD_VAL(REG_VOICE_OSC_FR, n)
#define REG_VOICE_OSC_PWR_SHIFT             6
#define REG_VOICE_OSC_PWR_MASK              (0x3 << REG_VOICE_OSC_PWR_SHIFT)
#define REG_VOICE_OSC_PWR(n)                BITFIELD_VAL(REG_VOICE_OSC_PWR, n)
#define REG_VOICE_SEL_VCMIN_BIN_SHIFT       8
#define REG_VOICE_SEL_VCMIN_BIN_MASK        (0xF << REG_VOICE_SEL_VCMIN_BIN_SHIFT)
#define REG_VOICE_SEL_VCMIN_BIN(n)          BITFIELD_VAL(REG_VOICE_SEL_VCMIN_BIN, n)
#define REG_VOICE_SEL_VCM0P3_BIN_SHIFT      12
#define REG_VOICE_SEL_VCM0P3_BIN_MASK       (0x7 << REG_VOICE_SEL_VCM0P3_BIN_SHIFT)
#define REG_VOICE_SEL_VCM0P3_BIN(n)         BITFIELD_VAL(REG_VOICE_SEL_VCM0P3_BIN, n)
#define RESERVED_ANA_39                     (1 << 15)

// REG_7C
#define REG_VOICE_SEL_VCM_BIN_SHIFT         0
#define REG_VOICE_SEL_VCM_BIN_MASK          (0xF << REG_VOICE_SEL_VCM_BIN_SHIFT)
#define REG_VOICE_SEL_VCM_BIN(n)            BITFIELD_VAL(REG_VOICE_SEL_VCM_BIN, n)
#define REG_VOICE_LP_COMP_SHIFT             4
#define REG_VOICE_LP_COMP_MASK              (0x7 << REG_VOICE_LP_COMP_SHIFT)
#define REG_VOICE_LP_COMP(n)                BITFIELD_VAL(REG_VOICE_LP_COMP, n)
#define REG_VOICE_SEL_VREF_BIN_SHIFT        7
#define REG_VOICE_SEL_VREF_BIN_MASK         (0xF << REG_VOICE_SEL_VREF_BIN_SHIFT)
#define REG_VOICE_SEL_VREF_BIN(n)           BITFIELD_VAL(REG_VOICE_SEL_VREF_BIN, n)
#define VOICE_OUTP                          (1 << 11)
#define VOICE_OUTN                          (1 << 12)
#define RESERVED_ANA_42_40_SHIFT            13
#define RESERVED_ANA_42_40_MASK             (0x7 << RESERVED_ANA_42_40_SHIFT)
#define RESERVED_ANA_42_40(n)               BITFIELD_VAL(RESERVED_ANA_42_40, n)

// REG_7D
#define REG_VOICE_LP_AMPA_SHIFT             0
#define REG_VOICE_LP_AMPA_MASK              (0x7 << REG_VOICE_LP_AMPA_SHIFT)
#define REG_VOICE_LP_AMPA(n)                BITFIELD_VAL(REG_VOICE_LP_AMPA, n)
#define REG_VOICE_LP_AMPB_SHIFT             3
#define REG_VOICE_LP_AMPB_MASK              (0x7 << REG_VOICE_LP_AMPB_SHIFT)
#define REG_VOICE_LP_AMPB(n)                BITFIELD_VAL(REG_VOICE_LP_AMPB, n)
#define REG_VOICE_LP_LDO_SHIFT              6
#define REG_VOICE_LP_LDO_MASK               (0x7 << REG_VOICE_LP_LDO_SHIFT)
#define REG_VOICE_LP_LDO(n)                 BITFIELD_VAL(REG_VOICE_LP_LDO, n)
#define REG_VOICE_LP_VREF_SHIFT             9
#define REG_VOICE_LP_VREF_MASK              (0x7 << REG_VOICE_LP_VREF_SHIFT)
#define REG_VOICE_LP_VREF(n)                BITFIELD_VAL(REG_VOICE_LP_VREF, n)
#define REG_VOICE_LOW_TUNE_SHIFT            12
#define REG_VOICE_LOW_TUNE_MASK             (0x3 << REG_VOICE_LOW_TUNE_SHIFT)
#define REG_VOICE_LOW_TUNE(n)               BITFIELD_VAL(REG_VOICE_LOW_TUNE, n)
#define REG_VOICE_EN_AGPIO_TEST             (1 << 14)
#define REG_VOICE_EN_REF_TEST               (1 << 15)

// REG_7E
#define REG_EN_OFFSET_CALIB_A               (1 << 0)
#define REG_OFFSET_CALIB_MODE_A             (1 << 1)
#define REG_EN_CALIB_A_DR                   (1 << 2)
#define REG_CLR_MODE_A_DONE                 (1 << 3)
#define REG_CALIB_A_CNT_SHIFT               4
#define REG_CALIB_A_CNT_MASK                (0x3FF << REG_CALIB_A_CNT_SHIFT)
#define REG_CALIB_A_CNT(n)                  BITFIELD_VAL(REG_CALIB_A_CNT, n)
#define REG_CALIB_START                     (1 << 14)
#define CALIB_A_DONE                        (1 << 15)

// REG_7F
#define REG_EN_OFFSET_CALIB_B               (1 << 0)
#define REG_OFFSET_CALIB_MODE_B             (1 << 1)
#define REG_EN_CALIB_B_DR                   (1 << 2)
#define REG_CLR_MODE_B_DONE                 (1 << 3)
#define REG_CALIB_B_CNT_SHIFT               4
#define REG_CALIB_B_CNT_MASK                (0x3FF << REG_CALIB_B_CNT_SHIFT)
#define REG_CALIB_B_CNT(n)                  BITFIELD_VAL(REG_CALIB_B_CNT, n)
#define CALIB_B_DONE                        (1 << 14)
#define RESERVED_ANA_43                     (1 << 15)

// REG_80
#define OFFSET_BIT_B_SHIFT                  0
#define OFFSET_BIT_B_MASK                   (0x3F << OFFSET_BIT_B_SHIFT)
#define OFFSET_BIT_B(n)                     BITFIELD_VAL(OFFSET_BIT_B, n)
#define OFFSET_BIT_A_SHIFT                  6
#define OFFSET_BIT_A_MASK                   (0x3F << OFFSET_BIT_A_SHIFT)
#define OFFSET_BIT_A(n)                     BITFIELD_VAL(OFFSET_BIT_A, n)
#define RESERVED_ANA_47_44_SHIFT            12
#define RESERVED_ANA_47_44_MASK             (0xF << RESERVED_ANA_47_44_SHIFT)
#define RESERVED_ANA_47_44(n)               BITFIELD_VAL(RESERVED_ANA_47_44, n)

// REG_81
#define REG_XTAL_FREQ_LLC_SFT_RSTN          (1 << 0)
#define REG_CLK_GOAL_DIV_NUM_SHIFT          1
#define REG_CLK_GOAL_DIV_NUM_MASK           (0x1FF << REG_CLK_GOAL_DIV_NUM_SHIFT)
#define REG_CLK_GOAL_DIV_NUM(n)             BITFIELD_VAL(REG_CLK_GOAL_DIV_NUM, n)
#define REG_COARSE_EN                       (1 << 10)
#define REG_COARSE_TUN_CODE_DR              (1 << 11)
#define RESERVED_ANA_51_48_SHIFT            12
#define RESERVED_ANA_51_48_MASK             (0xF << RESERVED_ANA_51_48_SHIFT)
#define RESERVED_ANA_51_48(n)               BITFIELD_VAL(RESERVED_ANA_51_48, n)

// REG_82
#define REG_COARSE_TUN_CODE_SHIFT           0
#define REG_COARSE_TUN_CODE_MASK            (0x3F << REG_COARSE_TUN_CODE_SHIFT)
#define REG_COARSE_TUN_CODE(n)              BITFIELD_VAL(REG_COARSE_TUN_CODE, n)
#define REG_COARSE_POLAR_SEL                (1 << 6)
#define RESERVED_ANA_60_52_SHIFT            7
#define RESERVED_ANA_60_52_MASK             (0x1FF << RESERVED_ANA_60_52_SHIFT)
#define RESERVED_ANA_60_52(n)               BITFIELD_VAL(RESERVED_ANA_60_52, n)

// REG_83
#define REG_CNT_COARSE_MARK_SHIFT           0
#define REG_CNT_COARSE_MARK_MASK            (0xFFFF << REG_CNT_COARSE_MARK_SHIFT)
#define REG_CNT_COARSE_MARK(n)              BITFIELD_VAL(REG_CNT_COARSE_MARK, n)

// REG_84
#define DBG_GOAL_CNT_SHIFT                  0
#define DBG_GOAL_CNT_MASK                   (0x1FF << DBG_GOAL_CNT_SHIFT)
#define DBG_GOAL_CNT(n)                     BITFIELD_VAL(DBG_GOAL_CNT, n)
#define RESERVED_ANA_67_61_SHIFT            9
#define RESERVED_ANA_67_61_MASK             (0x7F << RESERVED_ANA_67_61_SHIFT)
#define RESERVED_ANA_67_61(n)               BITFIELD_VAL(RESERVED_ANA_67_61, n)

// REG_85
#define DBG_REF_CNT_SHIFT                   0
#define DBG_REF_CNT_MASK                    (0xFFFF << DBG_REF_CNT_SHIFT)
#define DBG_REF_CNT(n)                      BITFIELD_VAL(DBG_REF_CNT, n)

// REG_86
#define DBG_STATE_SHIFT                     0
#define DBG_STATE_MASK                      (0x3 << DBG_STATE_SHIFT)
#define DBG_STATE(n)                        BITFIELD_VAL(DBG_STATE, n)
#define DBG_COARSE_TUN_CODE_SHIFT           2
#define DBG_COARSE_TUN_CODE_MASK            (0x7F << DBG_COARSE_TUN_CODE_SHIFT)
#define DBG_COARSE_TUN_CODE(n)              BITFIELD_VAL(DBG_COARSE_TUN_CODE, n)
#define RESERVED_ANA_74_68_SHIFT            9
#define RESERVED_ANA_74_68_MASK             (0x7F << RESERVED_ANA_74_68_SHIFT)
#define RESERVED_ANA_74_68(n)               BITFIELD_VAL(RESERVED_ANA_74_68, n)

// REG_160
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_161
#define RX_TIMER_RSTN_DLY_SHIFT             0
#define RX_TIMER_RSTN_DLY_MASK              (0x3F << RX_TIMER_RSTN_DLY_SHIFT)
#define RX_TIMER_RSTN_DLY(n)                BITFIELD_VAL(RX_TIMER_RSTN_DLY, n)
#define ADCC_RX_TIMER_RSTN_DLY_SHIFT        6
#define ADCC_RX_TIMER_RSTN_DLY_MASK         (0x3F << ADCC_RX_TIMER_RSTN_DLY_SHIFT)
#define ADCC_RX_TIMER_RSTN_DLY(n)           BITFIELD_VAL(ADCC_RX_TIMER_RSTN_DLY, n)
#define RESERVED_ANA_78_75_SHIFT            12
#define RESERVED_ANA_78_75_MASK             (0xF << RESERVED_ANA_78_75_SHIFT)
#define RESERVED_ANA_78_75(n)               BITFIELD_VAL(RESERVED_ANA_78_75, n)

// REG_162
#define REG_CODEC_EN_BIAS_LP                (1 << 0)
#define REG_CODEC_EN_BIAS                   (1 << 1)
#define REG_CODEC_EN_VCM                    (1 << 2)
#define REG_CODEC_RX_EN_VTOI                (1 << 3)
#define REG_CODEC_EN_VCMBUFFER              (1 << 4)
#define REG_CODEC_VCM_EN_LPF                (1 << 5)
#define REG_CODEC_BIAS_LOWV_LP              (1 << 6)
#define REG_CODEC_LP_VCM                    (1 << 7)
#define REG_CODEC_VCM_BIAS_HIGHV            (1 << 8)
#define REG_CODEC_RX_VTOI_I_DAC2_SHIFT      9
#define REG_CODEC_RX_VTOI_I_DAC2_MASK       (0x7 << REG_CODEC_RX_VTOI_I_DAC2_SHIFT)
#define REG_CODEC_RX_VTOI_I_DAC2(n)         BITFIELD_VAL(REG_CODEC_RX_VTOI_I_DAC2, n)
#define REG_CODEC_RX_VTOI_IDAC_SEL_SHIFT    12
#define REG_CODEC_RX_VTOI_IDAC_SEL_MASK     (0xF << REG_CODEC_RX_VTOI_IDAC_SEL_SHIFT)
#define REG_CODEC_RX_VTOI_IDAC_SEL(n)       BITFIELD_VAL(REG_CODEC_RX_VTOI_IDAC_SEL, n)

// REG_163
#define REG_CODEC_RX_VTOI_VCS_SEL_SHIFT     0
#define REG_CODEC_RX_VTOI_VCS_SEL_MASK      (0x1F << REG_CODEC_RX_VTOI_VCS_SEL_SHIFT)
#define REG_CODEC_RX_VTOI_VCS_SEL(n)        BITFIELD_VAL(REG_CODEC_RX_VTOI_VCS_SEL, n)
#define REG_CODEC_EN_TX_EXT                 (1 << 5)
#define REG_CODEC_BIAS_IBSEL_SHIFT          6
#define REG_CODEC_BIAS_IBSEL_MASK           (0xF << REG_CODEC_BIAS_IBSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL(n)             BITFIELD_VAL(REG_CODEC_BIAS_IBSEL, n)
#define REG_CODEC_BIAS_LOWV                 (1 << 10)
#define REG_CODEC_BUF_LOWPOWER              (1 << 11)
#define REG_CODEC_BUF_LOWPOWER2             (1 << 12)
#define REG_CODEC_BUF_LOWVCM_SHIFT          13
#define REG_CODEC_BUF_LOWVCM_MASK           (0x7 << REG_CODEC_BUF_LOWVCM_SHIFT)
#define REG_CODEC_BUF_LOWVCM(n)             BITFIELD_VAL(REG_CODEC_BUF_LOWVCM, n)

// REG_164
#define REG_CODEC_BIAS_IBSEL_TX_SHIFT       0
#define REG_CODEC_BIAS_IBSEL_TX_MASK        (0xF << REG_CODEC_BIAS_IBSEL_TX_SHIFT)
#define REG_CODEC_BIAS_IBSEL_TX(n)          BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_TX, n)
#define REG_CODEC_BIAS_IBSEL_VOICE_SHIFT    4
#define REG_CODEC_BIAS_IBSEL_VOICE_MASK     (0xF << REG_CODEC_BIAS_IBSEL_VOICE_SHIFT)
#define REG_CODEC_BIAS_IBSEL_VOICE(n)       BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_VOICE, n)
#define REG_CODEC_VCM_LOW_VCM_SHIFT         8
#define REG_CODEC_VCM_LOW_VCM_MASK          (0xF << REG_CODEC_VCM_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_LOW_VCM(n)            BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM, n)
#define REG_CODEC_VCM_LOW_VCM_LPF_SHIFT     12
#define REG_CODEC_VCM_LOW_VCM_LPF_MASK      (0xF << REG_CODEC_VCM_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LPF(n)        BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LPF, n)

// REG_165
#define REG_CODEC_VCM_LOW_VCM_LP_SHIFT      0
#define REG_CODEC_VCM_LOW_VCM_LP_MASK       (0xF << REG_CODEC_VCM_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LP(n)         BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LP, n)
#define REG_CODEC_ADC_LPFVCM_SW_SHIFT       4
#define REG_CODEC_ADC_LPFVCM_SW_MASK        (0xF << REG_CODEC_ADC_LPFVCM_SW_SHIFT)
#define REG_CODEC_ADC_LPFVCM_SW(n)          BITFIELD_VAL(REG_CODEC_ADC_LPFVCM_SW, n)
#define CFG_EN_RX_DR                        (1 << 8)
#define CFG_EN_RX                           (1 << 9)
#define REG_RX_PGAA_DACGAIN_SHIFT           10
#define REG_RX_PGAA_DACGAIN_MASK            (0x3 << REG_RX_PGAA_DACGAIN_SHIFT)
#define REG_RX_PGAA_DACGAIN(n)              BITFIELD_VAL(REG_RX_PGAA_DACGAIN, n)
#define REG_RX_PGAB_DACGAIN_SHIFT           12
#define REG_RX_PGAB_DACGAIN_MASK            (0x3 << REG_RX_PGAB_DACGAIN_SHIFT)
#define REG_RX_PGAB_DACGAIN(n)              BITFIELD_VAL(REG_RX_PGAB_DACGAIN, n)
#define REG_RX_PGAC_DACGAIN_SHIFT           14
#define REG_RX_PGAC_DACGAIN_MASK            (0x3 << REG_RX_PGAC_DACGAIN_SHIFT)
#define REG_RX_PGAC_DACGAIN(n)              BITFIELD_VAL(REG_RX_PGAC_DACGAIN, n)

// REG_166
#define CFG_RESET_ADC_A_DR                  (1 << 0)
#define CFG_RESET_ADC_A                     (1 << 1)
#define REG_CODEC_RX_EN_ADCA                (1 << 2)
#define REG_CODEC_EN_ADCA                   (1 << 3)
#define REG_CODEC_ADCA_GAIN_UPDATE          (1 << 4)
#define REG_CODEC_ADCA_GAIN_BIT_SHIFT       5
#define REG_CODEC_ADCA_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCA_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCA_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_GAIN_BIT, n)
#define REG_CODEC_ADCA_RES_2P5K_UPDATE      (1 << 8)
#define REG_CODEC_ADCA_RES_2P5K             (1 << 9)
#define REG_CODEC_ADCA_CH_SEL_SHIFT         10
#define REG_CODEC_ADCA_CH_SEL_MASK          (0x3 << REG_CODEC_ADCA_CH_SEL_SHIFT)
#define REG_CODEC_ADCA_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCA_CH_SEL, n)
#define REG_CODEC_ADCA_DITHER_EN            (1 << 12)
#define RESERVED_ANA_81_79_SHIFT            13
#define RESERVED_ANA_81_79_MASK             (0x7 << RESERVED_ANA_81_79_SHIFT)
#define RESERVED_ANA_81_79(n)               BITFIELD_VAL(RESERVED_ANA_81_79, n)

// REG_167
#define REG_CODEC_ADCA_OFFSET_CAL_S_L_SHIFT 0
#define REG_CODEC_ADCA_OFFSET_CAL_S_L_MASK  (0x1F << REG_CODEC_ADCA_OFFSET_CAL_S_L_SHIFT)
#define REG_CODEC_ADCA_OFFSET_CAL_S_L(n)    BITFIELD_VAL(REG_CODEC_ADCA_OFFSET_CAL_S_L, n)
#define REG_CODEC_ADCA_OFFSET_CAL_S_R_SHIFT 5
#define REG_CODEC_ADCA_OFFSET_CAL_S_R_MASK  (0x1F << REG_CODEC_ADCA_OFFSET_CAL_S_R_SHIFT)
#define REG_CODEC_ADCA_OFFSET_CAL_S_R(n)    BITFIELD_VAL(REG_CODEC_ADCA_OFFSET_CAL_S_R, n)
#define REG_CODEC_IDETA_EN                  (1 << 10)
#define RESERVED_ANA_86_82_SHIFT            11
#define RESERVED_ANA_86_82_MASK             (0x1F << RESERVED_ANA_86_82_SHIFT)
#define RESERVED_ANA_86_82(n)               BITFIELD_VAL(RESERVED_ANA_86_82, n)

// REG_168
#define CFG_PGAA_RESETN_DR                  (1 << 0)
#define CFG_PGAA_RESETN                     (1 << 1)
#define REG_RX_PGAA_EN                      (1 << 2)
#define REG_RX_PGAA_DRE_DR                  (1 << 3)
#define REG_RX_PGAA_DRE_SHIFT               4
#define REG_RX_PGAA_DRE_MASK                (0xF << REG_RX_PGAA_DRE_SHIFT)
#define REG_RX_PGAA_DRE(n)                  BITFIELD_VAL(REG_RX_PGAA_DRE, n)
#define REG_RX_PGAA_GAIN_UPDATE             (1 << 8)
#define REG_RX_PGAA_GAIN_SHIFT              9
#define REG_RX_PGAA_GAIN_MASK               (0x7 << REG_RX_PGAA_GAIN_SHIFT)
#define REG_RX_PGAA_GAIN(n)                 BITFIELD_VAL(REG_RX_PGAA_GAIN, n)
#define REG_RX_PGAA_LARGEGAIN_UPDATE        (1 << 12)
#define REG_RX_PGAA_LARGEGAIN_SHIFT         13
#define REG_RX_PGAA_LARGEGAIN_MASK          (0x7 << REG_RX_PGAA_LARGEGAIN_SHIFT)
#define REG_RX_PGAA_LARGEGAIN(n)            BITFIELD_VAL(REG_RX_PGAA_LARGEGAIN, n)

// REG_169
#define CFG_ADCA_DITHER_CLK_INV             (1 << 0)
#define REG_RX_PGAA_2P5K_SEL                (1 << 1)
#define REG_RX_PGAA_7P5K_SEL                (1 << 2)
#define REG_RX_PGAA_10K_SEL                 (1 << 3)
#define REG_RX_PGAA_CAPMODE                 (1 << 4)
#define REG_RX_PGAA_FASTSETTLE              (1 << 5)
#define REG_RX_PGAA_IBIT_OPA_SHIFT          6
#define REG_RX_PGAA_IBIT_OPA_MASK           (0x3 << REG_RX_PGAA_IBIT_OPA_SHIFT)
#define REG_RX_PGAA_IBIT_OPA(n)             BITFIELD_VAL(REG_RX_PGAA_IBIT_OPA, n)
#define REG_RX_PGAA_OP_C_SEL                (1 << 8)
#define REG_RX_PGAA_OP_GM_SEL               (1 << 9)
#define REG_RX_PGAA_OP_R_SEL                (1 << 10)
#define REG_RX_PGAA_RFB_EN                  (1 << 11)
#define REG_RX_PGAA_OFFSET_CAL_EN           (1 << 12)
#define REG_RX_PGAA_ZERO_DET_EN             (1 << 13)
#define REG_RX_PGAA_DRE_UPDATE              (1 << 14)
#define RESERVED_ANA_87                     (1 << 15)


// REG_16A
#define CFG_RESET_ADC_B_DR                  (1 << 0)
#define CFG_RESET_ADC_B                     (1 << 1)
#define REG_CODEC_RX_EN_ADCB                (1 << 2)
#define REG_CODEC_EN_ADCB                   (1 << 3)
#define REG_CODEC_ADCB_GAIN_UPDATE          (1 << 4)
#define REG_CODEC_ADCB_GAIN_BIT_SHIFT       5
#define REG_CODEC_ADCB_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCB_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCB_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_GAIN_BIT, n)
#define REG_CODEC_ADCB_RES_2P5K_UPDATE      (1 << 8)
#define REG_CODEC_ADCB_RES_2P5K             (1 << 9)
#define REG_CODEC_ADCB_CH_SEL_SHIFT         10
#define REG_CODEC_ADCB_CH_SEL_MASK          (0x3 << REG_CODEC_ADCB_CH_SEL_SHIFT)
#define REG_CODEC_ADCB_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCB_CH_SEL, n)
#define REG_CODEC_ADCB_DITHER_EN            (1 << 12)
#define RESERVED_ANA_90_88_SHIFT            13
#define RESERVED_ANA_90_88_MASK             (0x7 << RESERVED_ANA_90_88_SHIFT)
#define RESERVED_ANA_90_88(n)               BITFIELD_VAL(RESERVED_ANA_90_88, n)

// REG_16B
#define REG_CODEC_ADCB_OFFSET_CAL_S_L_SHIFT 0
#define REG_CODEC_ADCB_OFFSET_CAL_S_L_MASK  (0x1F << REG_CODEC_ADCB_OFFSET_CAL_S_L_SHIFT)
#define REG_CODEC_ADCB_OFFSET_CAL_S_L(n)    BITFIELD_VAL(REG_CODEC_ADCB_OFFSET_CAL_S_L, n)
#define REG_CODEC_ADCB_OFFSET_CAL_S_R_SHIFT 5
#define REG_CODEC_ADCB_OFFSET_CAL_S_R_MASK  (0x1F << REG_CODEC_ADCB_OFFSET_CAL_S_R_SHIFT)
#define REG_CODEC_ADCB_OFFSET_CAL_S_R(n)    BITFIELD_VAL(REG_CODEC_ADCB_OFFSET_CAL_S_R, n)
#define REG_CODEC_IDETB_EN                  (1 << 10)
#define RESERVED_ANA_95_91_SHIFT            11
#define RESERVED_ANA_95_91_MASK             (0x1F << RESERVED_ANA_95_91_SHIFT)
#define RESERVED_ANA_95_91(n)               BITFIELD_VAL(RESERVED_ANA_95_91, n)

// REG_16C
#define ADCB_OPEN_TIMER_DLY_SHIFT           0
#define ADCB_OPEN_TIMER_DLY_MASK            (0xF << ADCB_OPEN_TIMER_DLY_SHIFT)
#define ADCB_OPEN_TIMER_DLY(n)              BITFIELD_VAL(ADCB_OPEN_TIMER_DLY, n)
#define ADCB_PRE_CHARGE_TIMER_DLY_SHIFT     4
#define ADCB_PRE_CHARGE_TIMER_DLY_MASK      (0xFF << ADCB_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCB_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCB_PRE_CHARGE_TIMER_DLY, n)
#define VOITRIG_ADC_SEL                     (1 << 12)
#define REG_CODEC_ADCB_VREFBUF_CAS_SHIFT    13
#define REG_CODEC_ADCB_VREFBUF_CAS_MASK     (0x3 << REG_CODEC_ADCB_VREFBUF_CAS_SHIFT)
#define REG_CODEC_ADCB_VREFBUF_CAS(n)       BITFIELD_VAL(REG_CODEC_ADCB_VREFBUF_CAS, n)
#define RESERVED_ANA_96                     (1 << 15)

// REG_16D
#define REG_CODEC_ADCB_IBSEL_IDAC2_IN_SHIFT 0
#define REG_CODEC_ADCB_IBSEL_IDAC2_IN_MASK  (0xF << REG_CODEC_ADCB_IBSEL_IDAC2_IN_SHIFT)
#define REG_CODEC_ADCB_IBSEL_IDAC2_IN(n)    BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_IDAC2_IN, n)
#define REG_CODEC_ADCB_IBSEL_VREFBUF_IN_SHIFT 4
#define REG_CODEC_ADCB_IBSEL_VREFBUF_IN_MASK (0xF << REG_CODEC_ADCB_IBSEL_VREFBUF_IN_SHIFT)
#define REG_CODEC_ADCB_IBSEL_VREFBUF_IN(n)  BITFIELD_VAL(REG_CODEC_ADCB_IBSEL_VREFBUF_IN, n)
#define REG_CODEC_ADCB_LP_VREFBUF_SHIFT     8
#define REG_CODEC_ADCB_LP_VREFBUF_MASK      (0x7 << REG_CODEC_ADCB_LP_VREFBUF_SHIFT)
#define REG_CODEC_ADCB_LP_VREFBUF(n)        BITFIELD_VAL(REG_CODEC_ADCB_LP_VREFBUF, n)
#define REG_CODEC_ADCB_VREFBUF_BIT_IN_SHIFT 11
#define REG_CODEC_ADCB_VREFBUF_BIT_IN_MASK  (0xF << REG_CODEC_ADCB_VREFBUF_BIT_IN_SHIFT)
#define REG_CODEC_ADCB_VREFBUF_BIT_IN(n)    BITFIELD_VAL(REG_CODEC_ADCB_VREFBUF_BIT_IN, n)
#define RESERVED_ANA_97                     (1 << 15)

// REG_16E
#define CFG_PGAB_RESETN_DR                  (1 << 0)
#define CFG_PGAB_RESETN                     (1 << 1)
#define REG_RX_PGAB_EN                      (1 << 2)
#define REG_RX_PGAB_DRE_DR                  (1 << 3)
#define REG_RX_PGAB_DRE_SHIFT               4
#define REG_RX_PGAB_DRE_MASK                (0xF << REG_RX_PGAB_DRE_SHIFT)
#define REG_RX_PGAB_DRE(n)                  BITFIELD_VAL(REG_RX_PGAB_DRE, n)
#define REG_RX_PGAB_GAIN_UPDATE             (1 << 8)
#define REG_RX_PGAB_GAIN_SHIFT              9
#define REG_RX_PGAB_GAIN_MASK               (0x7 << REG_RX_PGAB_GAIN_SHIFT)
#define REG_RX_PGAB_GAIN(n)                 BITFIELD_VAL(REG_RX_PGAB_GAIN, n)
#define REG_RX_PGAB_LARGEGAIN_UPDATE        (1 << 12)
#define REG_RX_PGAB_LARGEGAIN_SHIFT         13
#define REG_RX_PGAB_LARGEGAIN_MASK          (0x7 << REG_RX_PGAB_LARGEGAIN_SHIFT)
#define REG_RX_PGAB_LARGEGAIN(n)            BITFIELD_VAL(REG_RX_PGAB_LARGEGAIN, n)

// REG_16F
#define CFG_ADCB_DITHER_CLK_INV             (1 << 0)
#define REG_RX_PGAB_2P5K_SEL                (1 << 1)
#define REG_RX_PGAB_7P5K_SEL                (1 << 2)
#define REG_RX_PGAB_10K_SEL                 (1 << 3)
#define REG_RX_PGAB_CAPMODE                 (1 << 4)
#define REG_RX_PGAB_FASTSETTLE              (1 << 5)
#define REG_RX_PGAB_IBIT_OPA_SHIFT          6
#define REG_RX_PGAB_IBIT_OPA_MASK           (0x3 << REG_RX_PGAB_IBIT_OPA_SHIFT)
#define REG_RX_PGAB_IBIT_OPA(n)             BITFIELD_VAL(REG_RX_PGAB_IBIT_OPA, n)
#define REG_RX_PGAB_OP_C_SEL                (1 << 8)
#define REG_RX_PGAB_OP_GM_SEL               (1 << 9)
#define REG_RX_PGAB_OP_R_SEL                (1 << 10)
#define REG_RX_PGAB_RFB_EN                  (1 << 11)
#define REG_RX_PGAB_OFFSET_CAL_EN           (1 << 12)
#define REG_RX_PGAB_ZERO_DET_EN             (1 << 13)
#define REG_RX_PGAB_DRE_UPDATE              (1 << 14)
#define RESERVED_ANA_98                     (1 << 15)

// REG_170
#define CFG_RESET_ADC_C_DR                  (1 << 0)
#define CFG_RESET_ADC_C                     (1 << 1)
#define REG_CODEC_RX_EN_ADCC                (1 << 2)
#define REG_CODEC_EN_ADCC                   (1 << 3)
#define REG_CODEC_ADCC_GAIN_UPDATE          (1 << 4)
#define REG_CODEC_ADCC_GAIN_BIT_SHIFT       5
#define REG_CODEC_ADCC_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCC_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCC_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_GAIN_BIT, n)
#define REG_CODEC_ADCC_RES_2P5K_UPDATE      (1 << 8)
#define REG_CODEC_ADCC_RES_2P5K             (1 << 9)
#define REG_CODEC_ADCC_CH_SEL_SHIFT         10
#define REG_CODEC_ADCC_CH_SEL_MASK          (0x3 << REG_CODEC_ADCC_CH_SEL_SHIFT)
#define REG_CODEC_ADCC_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCC_CH_SEL, n)
#define REG_CODEC_ADCC_DITHER_EN            (1 << 12)
#define RESERVED_ANA_101_99_SHIFT           13
#define RESERVED_ANA_101_99_MASK            (0x7 << RESERVED_ANA_101_99_SHIFT)
#define RESERVED_ANA_101_99(n)              BITFIELD_VAL(RESERVED_ANA_101_99, n)

// REG_171
#define REG_CODEC_ADCC_OFFSET_CAL_S_L_SHIFT 0
#define REG_CODEC_ADCC_OFFSET_CAL_S_L_MASK  (0x1F << REG_CODEC_ADCC_OFFSET_CAL_S_L_SHIFT)
#define REG_CODEC_ADCC_OFFSET_CAL_S_L(n)    BITFIELD_VAL(REG_CODEC_ADCC_OFFSET_CAL_S_L, n)
#define REG_CODEC_ADCC_OFFSET_CAL_S_R_SHIFT 5
#define REG_CODEC_ADCC_OFFSET_CAL_S_R_MASK  (0x1F << REG_CODEC_ADCC_OFFSET_CAL_S_R_SHIFT)
#define REG_CODEC_ADCC_OFFSET_CAL_S_R(n)    BITFIELD_VAL(REG_CODEC_ADCC_OFFSET_CAL_S_R, n)
#define REG_CODEC_IDETC_EN                  (1 << 10)
#define RESERVED_ANA_106_102_SHIFT          11
#define RESERVED_ANA_106_102_MASK           (0x1F << RESERVED_ANA_106_102_SHIFT)
#define RESERVED_ANA_106_102(n)             BITFIELD_VAL(RESERVED_ANA_106_102, n)

// REG_172
#define CFG_PGAC_RESETN_DR                  (1 << 0)
#define CFG_PGAC_RESETN                     (1 << 1)
#define REG_RX_PGAC_EN                      (1 << 2)
#define REG_RX_PGAC_DRE_DR                  (1 << 3)
#define REG_RX_PGAC_DRE_SHIFT               4
#define REG_RX_PGAC_DRE_MASK                (0xF << REG_RX_PGAC_DRE_SHIFT)
#define REG_RX_PGAC_DRE(n)                  BITFIELD_VAL(REG_RX_PGAC_DRE, n)
#define REG_RX_PGAC_GAIN_UPDATE             (1 << 8)
#define REG_RX_PGAC_GAIN_SHIFT              9
#define REG_RX_PGAC_GAIN_MASK               (0x7 << REG_RX_PGAC_GAIN_SHIFT)
#define REG_RX_PGAC_GAIN(n)                 BITFIELD_VAL(REG_RX_PGAC_GAIN, n)
#define REG_RX_PGAC_LARGEGAIN_UPDATE        (1 << 12)
#define REG_RX_PGAC_LARGEGAIN_SHIFT         13
#define REG_RX_PGAC_LARGEGAIN_MASK          (0x7 << REG_RX_PGAC_LARGEGAIN_SHIFT)
#define REG_RX_PGAC_LARGEGAIN(n)            BITFIELD_VAL(REG_RX_PGAC_LARGEGAIN, n)


// REG_173
#define CFG_ADCC_DITHER_CLK_INV             (1 << 0)
#define REG_RX_PGAC_2P5K_SEL                (1 << 1)
#define REG_RX_PGAC_7P5K_SEL                (1 << 2)
#define REG_RX_PGAC_10K_SEL                 (1 << 3)
#define REG_RX_PGAC_CAPMODE                 (1 << 4)
#define REG_RX_PGAC_FASTSETTLE              (1 << 5)
#define REG_RX_PGAC_IBIT_OPA_SHIFT          6
#define REG_RX_PGAC_IBIT_OPA_MASK           (0x3 << REG_RX_PGAC_IBIT_OPA_SHIFT)
#define REG_RX_PGAC_IBIT_OPA(n)             BITFIELD_VAL(REG_RX_PGAC_IBIT_OPA, n)
#define REG_RX_PGAC_OP_C_SEL                (1 << 8)
#define REG_RX_PGAC_OP_GM_SEL               (1 << 9)
#define REG_RX_PGAC_OP_R_SEL                (1 << 10)
#define REG_RX_PGAC_RFB_EN                  (1 << 11)
#define REG_RX_PGAC_OFFSET_CAL_EN           (1 << 12)
#define REG_RX_PGAC_ZERO_DET_EN             (1 << 13)
#define REG_RX_PGAC_DRE_UPDATE              (1 << 14)
#define RESERVED_ANA_107                    (1 << 15)

// REG_174
#define REG_RX_PGAA_CHANSEL_SHIFT           0
#define REG_RX_PGAA_CHANSEL_MASK            (0x3 << REG_RX_PGAA_CHANSEL_SHIFT)
#define REG_RX_PGAA_CHANSEL(n)              BITFIELD_VAL(REG_RX_PGAA_CHANSEL, n)
#define REG_RX_PGAB_CHANSEL_SHIFT           2
#define REG_RX_PGAB_CHANSEL_MASK            (0x3 << REG_RX_PGAB_CHANSEL_SHIFT)
#define REG_RX_PGAB_CHANSEL(n)              BITFIELD_VAL(REG_RX_PGAB_CHANSEL, n)
#define REG_RX_PGAC_CHANSEL_SHIFT           4
#define REG_RX_PGAC_CHANSEL_MASK            (0x3 << REG_RX_PGAC_CHANSEL_SHIFT)
#define REG_RX_PGAC_CHANSEL(n)              BITFIELD_VAL(REG_RX_PGAC_CHANSEL, n)
#define RESERVED_ANA_117_108_SHIFT          6
#define RESERVED_ANA_117_108_MASK           (0x3FF << RESERVED_ANA_117_108_SHIFT)
#define RESERVED_ANA_117_108(n)             BITFIELD_VAL(RESERVED_ANA_117_108, n)

// REG_175
#define ADCC_OPEN_TIMER_DLY_SHIFT           0
#define ADCC_OPEN_TIMER_DLY_MASK            (0xF << ADCC_OPEN_TIMER_DLY_SHIFT)
#define ADCC_OPEN_TIMER_DLY(n)              BITFIELD_VAL(ADCC_OPEN_TIMER_DLY, n)
#define ADCC_PRE_CHARGE_TIMER_DLY_SHIFT     4
#define ADCC_PRE_CHARGE_TIMER_DLY_MASK      (0xFF << ADCC_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCC_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCC_PRE_CHARGE_TIMER_DLY, n)
#define CFG_PRE_CHARGE_ADCC_DR              (1 << 12)
#define CFG_VOICE_TRIG_ADCC_EN              (1 << 13)
#define CFG_VOICE2ADCC                      (1 << 14)
#define CFG_ADC_START                       (1 << 15)

// REG_176
#define REG_CODEC_ADCC_IBSEL_IDAC2_IN_SHIFT 0
#define REG_CODEC_ADCC_IBSEL_IDAC2_IN_MASK  (0xF << REG_CODEC_ADCC_IBSEL_IDAC2_IN_SHIFT)
#define REG_CODEC_ADCC_IBSEL_IDAC2_IN(n)    BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_IDAC2_IN, n)
#define REG_CODEC_ADCC_IBSEL_VREFBUF_IN_SHIFT 4
#define REG_CODEC_ADCC_IBSEL_VREFBUF_IN_MASK (0xF << REG_CODEC_ADCC_IBSEL_VREFBUF_IN_SHIFT)
#define REG_CODEC_ADCC_IBSEL_VREFBUF_IN(n)  BITFIELD_VAL(REG_CODEC_ADCC_IBSEL_VREFBUF_IN, n)
#define REG_CODEC_ADCC_LP_VREFBUF_SHIFT     8
#define REG_CODEC_ADCC_LP_VREFBUF_MASK      (0x7 << REG_CODEC_ADCC_LP_VREFBUF_SHIFT)
#define REG_CODEC_ADCC_LP_VREFBUF(n)        BITFIELD_VAL(REG_CODEC_ADCC_LP_VREFBUF, n)
#define REG_CODEC_ADCC_VREFBUF_BIT_IN_SHIFT 11
#define REG_CODEC_ADCC_VREFBUF_BIT_IN_MASK  (0xF << REG_CODEC_ADCC_VREFBUF_BIT_IN_SHIFT)
#define REG_CODEC_ADCC_VREFBUF_BIT_IN(n)    BITFIELD_VAL(REG_CODEC_ADCC_VREFBUF_BIT_IN, n)
#define RESERVED_ANA_118                    (1 << 15)

// REG_177
#define REG_CODEC_ADCC_VREFBUF_CAS_SHIFT    0
#define REG_CODEC_ADCC_VREFBUF_CAS_MASK     (0x3 << REG_CODEC_ADCC_VREFBUF_CAS_SHIFT)
#define REG_CODEC_ADCC_VREFBUF_CAS(n)       BITFIELD_VAL(REG_CODEC_ADCC_VREFBUF_CAS, n)
#define RESERVED_ANA_132_119_SHIFT          2
#define RESERVED_ANA_132_119_MASK           (0x3FFF << RESERVED_ANA_132_119_SHIFT)
#define RESERVED_ANA_132_119(n)             BITFIELD_VAL(RESERVED_ANA_132_119, n)

// REG_178
#define REG_CODEC_ADC_CAP_BIT1_SHIFT        0
#define REG_CODEC_ADC_CAP_BIT1_MASK         (0x1F << REG_CODEC_ADC_CAP_BIT1_SHIFT)
#define REG_CODEC_ADC_CAP_BIT1(n)           BITFIELD_VAL(REG_CODEC_ADC_CAP_BIT1, n)
#define REG_CODEC_ADC_CAP_BIT2_SHIFT        5
#define REG_CODEC_ADC_CAP_BIT2_MASK         (0x1F << REG_CODEC_ADC_CAP_BIT2_SHIFT)
#define REG_CODEC_ADC_CAP_BIT2(n)           BITFIELD_VAL(REG_CODEC_ADC_CAP_BIT2, n)
#define REG_CODEC_ADC_CAP_BIT3_SHIFT        10
#define REG_CODEC_ADC_CAP_BIT3_MASK         (0x1F << REG_CODEC_ADC_CAP_BIT3_SHIFT)
#define REG_CODEC_ADC_CAP_BIT3(n)           BITFIELD_VAL(REG_CODEC_ADC_CAP_BIT3, n)
#define RESERVED_ANA_133                    (1 << 15)

// REG_179
#define REG_CODEC_ADC_DITHER0P5_EN          (1 << 0)
#define REG_CODEC_ADC_DITHER0P25_EN         (1 << 1)
#define REG_CODEC_ADC_DITHER1_EN            (1 << 2)
#define REG_CODEC_ADC_DITHER2_EN            (1 << 3)
#define REG_CODEC_ADC_DITHER_PHASE_SEL      (1 << 4)
#define RESERVED_ANA_144_134_SHIFT          5
#define RESERVED_ANA_144_134_MASK           (0x7FF << RESERVED_ANA_144_134_SHIFT)
#define RESERVED_ANA_144_134(n)             BITFIELD_VAL(RESERVED_ANA_144_134, n)

// REG_17A
#define REG_CODEC_ADC_IBSEL_OP1_SHIFT       0
#define REG_CODEC_ADC_IBSEL_OP1_MASK        (0xF << REG_CODEC_ADC_IBSEL_OP1_SHIFT)
#define REG_CODEC_ADC_IBSEL_OP1(n)          BITFIELD_VAL(REG_CODEC_ADC_IBSEL_OP1, n)
#define REG_CODEC_ADC_IBSEL_OP2_SHIFT       4
#define REG_CODEC_ADC_IBSEL_OP2_MASK        (0xF << REG_CODEC_ADC_IBSEL_OP2_SHIFT)
#define REG_CODEC_ADC_IBSEL_OP2(n)          BITFIELD_VAL(REG_CODEC_ADC_IBSEL_OP2, n)
#define REG_CODEC_ADC_IBSEL_OP3_SHIFT       8
#define REG_CODEC_ADC_IBSEL_OP3_MASK        (0xF << REG_CODEC_ADC_IBSEL_OP3_SHIFT)
#define REG_CODEC_ADC_IBSEL_OP3(n)          BITFIELD_VAL(REG_CODEC_ADC_IBSEL_OP3, n)
#define REG_CODEC_ADC_IBSEL_OP4_SHIFT       12
#define REG_CODEC_ADC_IBSEL_OP4_MASK        (0xF << REG_CODEC_ADC_IBSEL_OP4_SHIFT)
#define REG_CODEC_ADC_IBSEL_OP4(n)          BITFIELD_VAL(REG_CODEC_ADC_IBSEL_OP4, n)

// REG_17B
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
#define RESERVED_ANA_148_145_SHIFT          12
#define RESERVED_ANA_148_145_MASK           (0xF << RESERVED_ANA_148_145_SHIFT)
#define RESERVED_ANA_148_145(n)             BITFIELD_VAL(RESERVED_ANA_148_145, n)

// REG_17C
#define REG_CODEC_ADC_REG_VSEL_SHIFT        0
#define REG_CODEC_ADC_REG_VSEL_MASK         (0x7 << REG_CODEC_ADC_REG_VSEL_SHIFT)
#define REG_CODEC_ADC_REG_VSEL(n)           BITFIELD_VAL(REG_CODEC_ADC_REG_VSEL, n)
#define REG_CODEC_ADC_RES_SEL_SHIFT         3
#define REG_CODEC_ADC_RES_SEL_MASK          (0x7 << REG_CODEC_ADC_RES_SEL_SHIFT)
#define REG_CODEC_ADC_RES_SEL(n)            BITFIELD_VAL(REG_CODEC_ADC_RES_SEL, n)
#define REG_CODEC_ADC_CLK_SEL               (1 << 6)
#define REG_CODEC_ADC_IBSEL_REG_SHIFT       7
#define REG_CODEC_ADC_IBSEL_REG_MASK        (0xF << REG_CODEC_ADC_IBSEL_REG_SHIFT)
#define REG_CODEC_ADC_IBSEL_REG(n)          BITFIELD_VAL(REG_CODEC_ADC_IBSEL_REG, n)
#define RESERVED_ANA_153_149_SHIFT          11
#define RESERVED_ANA_153_149_MASK           (0x1F << RESERVED_ANA_153_149_SHIFT)
#define RESERVED_ANA_153_149(n)             BITFIELD_VAL(RESERVED_ANA_153_149, n)

// REG_17D
#define REG_CODEC_ADCA_VREF_SEL_SHIFT       0
#define REG_CODEC_ADCA_VREF_SEL_MASK        (0xF << REG_CODEC_ADCA_VREF_SEL_SHIFT)
#define REG_CODEC_ADCA_VREF_SEL(n)          BITFIELD_VAL(REG_CODEC_ADCA_VREF_SEL, n)
#define REG_CODEC_ADCB_VREF_SEL_SHIFT       4
#define REG_CODEC_ADCB_VREF_SEL_MASK        (0xF << REG_CODEC_ADCB_VREF_SEL_SHIFT)
#define REG_CODEC_ADCB_VREF_SEL(n)          BITFIELD_VAL(REG_CODEC_ADCB_VREF_SEL, n)
#define REG_CODEC_ADCC_VREF_SEL_SHIFT       8
#define REG_CODEC_ADCC_VREF_SEL_MASK        (0xF << REG_CODEC_ADCC_VREF_SEL_SHIFT)
#define REG_CODEC_ADCC_VREF_SEL(n)          BITFIELD_VAL(REG_CODEC_ADCC_VREF_SEL, n)
#define RESERVED_ANA_157_154_SHIFT          12
#define RESERVED_ANA_157_154_MASK           (0xF << RESERVED_ANA_157_154_SHIFT)
#define RESERVED_ANA_157_154(n)             BITFIELD_VAL(RESERVED_ANA_157_154, n)


// REG_17E
#define REG_CODEC_ADC_IBSEL_VCOMP_SHIFT     0
#define REG_CODEC_ADC_IBSEL_VCOMP_MASK      (0xF << REG_CODEC_ADC_IBSEL_VCOMP_SHIFT)
#define REG_CODEC_ADC_IBSEL_VCOMP(n)        BITFIELD_VAL(REG_CODEC_ADC_IBSEL_VCOMP, n)
#define REG_CODEC_ADC_IBSEL_VREF_SHIFT      4
#define REG_CODEC_ADC_IBSEL_VREF_MASK       (0xF << REG_CODEC_ADC_IBSEL_VREF_SHIFT)
#define REG_CODEC_ADC_IBSEL_VREF(n)         BITFIELD_VAL(REG_CODEC_ADC_IBSEL_VREF, n)
#define RESERVED_ANA_165_158_SHIFT          8
#define RESERVED_ANA_165_158_MASK           (0xFF << RESERVED_ANA_165_158_SHIFT)
#define RESERVED_ANA_165_158(n)             BITFIELD_VAL(RESERVED_ANA_165_158, n)

// REG_17F
#define ADCB_RX_TIMER_RSTN_DLY_SHIFT        0
#define ADCB_RX_TIMER_RSTN_DLY_MASK         (0x3F << ADCB_RX_TIMER_RSTN_DLY_SHIFT)
#define ADCB_RX_TIMER_RSTN_DLY(n)           BITFIELD_VAL(ADCB_RX_TIMER_RSTN_DLY, n)
#define RESERVED_ANA_175_166_SHIFT          6
#define RESERVED_ANA_175_166_MASK           (0x3FF << RESERVED_ANA_175_166_SHIFT)
#define RESERVED_ANA_175_166(n)             BITFIELD_VAL(RESERVED_ANA_175_166, n)

// REG_180
#define CFG_CODEC_LP_VCM_DR                 (1 << 0)
#define REG_CODEC_ADCB_PRE_CHARGE           (1 << 1)
#define REG_CODEC_ADCC_PRE_CHARGE           (1 << 2)
#define ADCA_PRE_CHARGE_TIMER_DLY_SHIFT     3
#define ADCA_PRE_CHARGE_TIMER_DLY_MASK      (0xFF << ADCA_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCA_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCA_PRE_CHARGE_TIMER_DLY, n)
#define REG_VOICE_OSC_VOUT_SEL_SHIFT        11
#define REG_VOICE_OSC_VOUT_SEL_MASK         (0x7 << REG_VOICE_OSC_VOUT_SEL_SHIFT)
#define REG_VOICE_OSC_VOUT_SEL(n)           BITFIELD_VAL(REG_VOICE_OSC_VOUT_SEL, n)
#define RESERVED_ANA_177_176_SHIFT          14
#define RESERVED_ANA_177_176_MASK           (0x3 << RESERVED_ANA_177_176_SHIFT)
#define RESERVED_ANA_177_176(n)             BITFIELD_VAL(RESERVED_ANA_177_176, n)

enum ANA_REG_T {
    ANA_REG_60 = 0x60,
    ANA_REG_61,
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
    ANA_REG_75,
    ANA_REG_76,
    ANA_REG_77,
    ANA_REG_78,
    ANA_REG_79,
    ANA_REG_7A,
    ANA_REG_7B,
    ANA_REG_7C,
    ANA_REG_7D,
    ANA_REG_7E,
    ANA_REG_7F,
    ANA_REG_80,
    ANA_REG_81,
    ANA_REG_82,
    ANA_REG_83,
    ANA_REG_84,
    ANA_REG_85,
    ANA_REG_86,

    ANA_REG_160 = 0x160,
    ANA_REG_161,
    ANA_REG_162,
    ANA_REG_163,
    ANA_REG_164,
    ANA_REG_165,
    ANA_REG_166,
    ANA_REG_167,
    ANA_REG_168,
    ANA_REG_169,
    ANA_REG_16A,
    ANA_REG_16B,
    ANA_REG_16C,
    ANA_REG_16D,
    ANA_REG_16E,
    ANA_REG_16F,
    ANA_REG_170,
    ANA_REG_171,
    ANA_REG_172,
    ANA_REG_173,
    ANA_REG_174,
    ANA_REG_175,
    ANA_REG_176,
    ANA_REG_177,
    ANA_REG_178,
    ANA_REG_179,
    ANA_REG_17A,
    ANA_REG_17B,
    ANA_REG_17C,
    ANA_REG_17D,
    ANA_REG_17E,
    ANA_REG_17F,
    ANA_REG_180,
};

enum ANA_CODEC_USER_T {
    ANA_CODEC_USER_DAC,
    ANA_CODEC_USER_ADC,

    ANA_CODEC_USER_CODEC,
    ANA_CODEC_USER_MICKEY,

    ANA_CODEC_USER_VAD,

    ANA_CODEC_USER_QTY
};

typedef uint8_t CODEC_USER_MAP;
STATIC_ASSERT(sizeof(CODEC_USER_MAP) * 8 >= ANA_CODEC_USER_QTY, "Invalid codec user map type size");

struct ANALOG_PLL_CFG_T {
    uint32_t freq;
    uint8_t div;
};

#define OPT_TYPE                        const

static OPT_TYPE uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static CODEC_USER_MAP adc_map[MAX_ANA_MIC_CH_NUM];
static CODEC_USER_MAP vmic_map[MAX_ANA_MIC_CH_NUM];
static CODEC_USER_MAP codec_common_map;
static CODEC_USER_MAP adda_common_map;
static CODEC_USER_MAP vcodec_map;

static uint8_t ana_aud_pll_map;
STATIC_ASSERT(ANA_AUD_PLL_USER_QTY <= sizeof(ana_aud_pll_map) * 8, "Too many ANA AUD PLL users");

#ifdef DYN_ADC_GAIN
static int8_t adc_gain_offset[MAX_ANA_MIC_CH_NUM];
#endif

static const int8_t adc_db[] = { -9, -6, -3, 0, 3, 6, 9, 12, };

static const int8_t tgt_adc_db[MAX_ANA_MIC_CH_NUM] = {
    ANALOG_ADC_A_GAIN_DB, ANALOG_ADC_B_GAIN_DB,
    ANALOG_ADC_C_GAIN_DB,
};

static const int16_t adc_reg[MAX_ANA_MIC_CH_NUM] = {ANA_REG_166, ANA_REG_16A, ANA_REG_170};

// Max allowed total tune ratio (5000ppm)
#define MAX_TOTAL_TUNE_RATIO                0.005000

static struct ANALOG_PLL_CFG_T ana_pll_cfg;

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div)
{
    if (ana_pll_cfg.freq == freq && ana_pll_cfg.div == div)
        return ;

    ana_pll_cfg.freq = freq;
    ana_pll_cfg.div = div;

    pmu_pll_div_set(HAL_CMU_PLL_AUD, PMU_PLL_DIV_CODEC, div);

    usb_pll_config(freq*div);
}

void analog_aud_pll_tune(float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    if (hal_cmu_get_audio_resample_status()) {
        return;
    }
#endif

    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)
    // Delta = ((SampleDiff / Fs) / TimeDiff) * PLL_cfg_val

    int64_t delta, new_pll;

    if (ana_pll_cfg.freq == 0) {
        TRACE(1,"%s: WARNING: aud pll config cache invalid. Skip tuning", __FUNCTION__);
        return;
    }

    if (ABS(ratio) > MAX_TOTAL_TUNE_RATIO) {
        TRACE(1,"\n------\nWARNING: TUNE: ratio=%d is too large and will be cut\n------\n", FLOAT_TO_PPB_INT(ratio));
        if (ratio > 0) {
            ratio = MAX_TOTAL_TUNE_RATIO;
        } else {
            ratio = -MAX_TOTAL_TUNE_RATIO;
        }
    }

    TRACE(2,"%s: ratio=%d", __FUNCTION__, FLOAT_TO_PPB_INT(ratio));

    new_pll = (int64_t)ana_pll_cfg.freq * ana_pll_cfg.div;
    delta = (int64_t)(new_pll * ratio);

    new_pll += delta;

    usb_pll_config(new_pll);
}

void analog_aud_osc_clk_enable(bool enable)
{
    uint16_t val;

    if (enable) {
        analog_read(ANA_REG_73, &val);
        val |= REG_CRYSTAL_SEL_LV;
        analog_write(ANA_REG_73, val);
    } else {
        analog_read(ANA_REG_73, &val);
        val &= ~REG_CRYSTAL_SEL_LV;
        analog_write(ANA_REG_73, val);
    }
}

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {

        analog_aud_osc_clk_enable(true);
        return;
    }
#endif

    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
    ana_aud_pll_map |= (1 << user);
}

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {

        analog_aud_osc_clk_enable(false);
        return;
    }
#endif

    ana_aud_pll_map &= ~(1 << user);
    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
}

static void analog_aud_enable_dac(uint32_t dac)
{
    uint16_t val_69;
    uint16_t val_61;
    uint16_t val_68;

    analog_read(ANA_REG_69, &val_69);
    analog_read(ANA_REG_61, &val_61);
    analog_read(ANA_REG_68, &val_68);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        //val_69 |= REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS;
        val_69 |= REG_CODEC_TX_EAR_ENBIAS;
        analog_write(ANA_REG_69, val_69);
        osDelay(1);
        val_61 |= CFG_TX_TREE_EN;
        analog_write(ANA_REG_61, val_61);
        osDelay(1);

        if (dac & AUD_CHANNEL_MAP_CH0) {
            val_68 |= REG_CODEC_TX_EN_LCLK;
            val_61 |= REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_LDAC;
        }
        if (dac & AUD_CHANNEL_MAP_CH1) {
            val_68 |= REG_CODEC_TX_EN_RCLK;
            val_61 |= REG_CODEC_TX_EN_EARPA_R | REG_CODEC_TX_EN_RDAC
                    | REG_CODEC_TX_EN_LDAC;
        }
        val_68 |= REG_CODEC_TX_EN_DACLDO;
        val_61 |= REG_CODEC_TX_EN_LPPA;
        analog_write(ANA_REG_68, val_68);
        analog_write(ANA_REG_61, val_61);
        osDelay(1);
        val_69 |= REG_CODEC_TX_EN_S1PA;
        analog_write(ANA_REG_69, val_69);
        // Ensure 1ms delay before enabling dac_pa
        osDelay(1);
    } else {
        // Ensure 1ms delay after disabling dac_pa
        osDelay(1);
        analog_read(ANA_REG_69, &val_69);
        val_69 &= ~REG_CODEC_TX_EN_S1PA;
        analog_write(ANA_REG_69, val_69);
        osDelay(1);
        val_61 &= ~(REG_CODEC_TX_EN_LPPA |
            REG_CODEC_TX_EN_EARPA_R | REG_CODEC_TX_EN_RDAC | REG_CODEC_TX_EN_LDAC |
            REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_RDAC);
        val_68 &= ~(REG_CODEC_TX_EN_DACLDO |
            REG_CODEC_TX_EN_RCLK | REG_CODEC_TX_EN_LCLK);
        analog_write(ANA_REG_61, val_61);
        analog_write(ANA_REG_68, val_68);
        osDelay(1);

        val_61 &= ~CFG_TX_TREE_EN;
        analog_write(ANA_REG_61, val_61);
        osDelay(1);

        //val_69 &= ~(REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS);
        val_69 &= ~(REG_CODEC_TX_EAR_ENBIAS);
        analog_write(ANA_REG_69, val_69);
    }
}

static void analog_aud_enable_dac_pa(uint32_t dac)
{
    uint16_t val_69;

    analog_read(ANA_REG_69, &val_69);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_69 |= REG_CODEC_TX_EN_S4PA;
        analog_write(ANA_REG_69, val_69);

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_clear();
#endif
    } else {
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_set();
#endif

        val_69 &= ~REG_CODEC_TX_EN_S4PA;
        analog_write(ANA_REG_69, val_69);
    }
}

static void analog_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    int i;
    uint16_t val;
    CODEC_USER_MAP old_map;
    bool set;
    bool global_update;

    ANALOG_TRACE(3,"[%s] user=%d ch_map=0x%x", __func__, user, ch_map);

    global_update = false;

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            set = false;
            if (en) {
                if (adc_map[i] == 0) {
                    set = true;
                }
                adc_map[i] |= (1 << user);
            } else {
                old_map = adc_map[i];
                adc_map[i] &= ~(1 << user);
                if (old_map != 0 && adc_map[i] == 0) {
                    set = true;
                }
            }
            if (set) {
                uint16_t reg = adc_reg[i];
                global_update = true;

                analog_read(reg, &val);
                if (adc_map[i])
                    val |= REG_CODEC_EN_ADCA;
                else
                    val &= ~REG_CODEC_EN_ADCA;
                analog_write(reg, val);
            }
        }
    }

    if (global_update) {
        for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
            if (adc_map[i]) {
                break;
            }
        }
        analog_read(ANA_REG_162, &val);
        if (i < MAX_ANA_MIC_CH_NUM) {
            val |= REG_CODEC_RX_EN_VTOI;
        } else {
            val &= ~REG_CODEC_RX_EN_VTOI;
        }
        analog_write(ANA_REG_162, val);
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
            uint16_t reg = adc_reg[i];
            if (0) {
            } else if (input_path == AUD_INPUT_PATH_LINEIN) {
                gain = LINEIN_ADC_GAIN_DB;
            } else {
                gain = get_chan_adc_gain(i);
            }
            gain_val = db_to_adc_gain(gain);
            analog_read(reg, &val);
            val = SET_BITFIELD(val, REG_CODEC_ADCA_GAIN_BIT, gain_val);
            analog_write(reg, val);
        }
    }

}


#ifdef DYN_ADC_GAIN
void analog_aud_apply_adc_gain_offset(enum AUD_CHANNEL_MAP_T ch_map, int16_t offset)
{
    enum AUD_CHANNEL_MAP_T map;
    int i;

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
        TRACE(1,"Dc calib L OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_l = dc.val.dc;
    } else {
        TRACE(1,"Warning: Bad dc calib efuse L: 0x%04x", efuse);
        *dc_l = 0;
    }

    pmu_get_efuse(page[1], &efuse);
    if (dc_calib_checksum_valid(efuse)) {
        TRACE(1,"Dc calib R OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_r = dc.val.dc;
    } else {
        TRACE(1,"Warning: Bad dc calib efuse R: 0x%04x", efuse);
        *dc_r = 0;
    }

    TRACE(2,"ANA: DC CALIB L=%d R=%d", *dc_l, *dc_r);

#if defined(ANA_DC_CALIB_L) || defined(ANA_DC_CALIB_R)
#ifdef ANA_DC_CALIB_L
    *dc_l = ANA_DC_CALIB_L;
#endif
#ifdef ANA_DC_CALIB_R
    *dc_r = ANA_DC_CALIB_R;
#endif
    TRACE(2,"ANA: OVERRIDE DC CALIB L=%d R=%d", *dc_l, *dc_r);
#endif

    return;
}

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
static void analog_aud_dc_calib_init(void)
{
    uint16_t val;
    int16_t dc_l, dc_r;

    analog_aud_get_dc_calib_value(&dc_l, &dc_r);

    analog_read(ANA_REG_75, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL, dc_l);
    analog_write(ANA_REG_75, val);

    analog_read(ANA_REG_76, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITR, dc_r);
    analog_write(ANA_REG_76, val);
}

static void analog_aud_dc_calib_enable(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_6A, &val);
    val |= REG_CODEC_TX_EAR_OFFEN;
    analog_write(ANA_REG_6A, val);
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
    uint16_t val_162, val_164;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (codec_common_map == 0) {
            set = true;
        }
        codec_common_map |= (1 << user);
    } else {
        codec_common_map &= ~(1 << user);
        if (codec_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_162, &val_162);
        analog_read(ANA_REG_164, &val_164);
        if (codec_common_map) {
            val_162 |= REG_CODEC_EN_VCM;
        } else {
            val_162 &= ~(REG_CODEC_EN_VCM | REG_CODEC_VCM_EN_LPF);
        }
        if (codec_common_map) {
            // RTOS application startup time is long enougth for VCM charging
#if !(defined(VCM_ON) && defined(RTOS))
            // VCM fully stable time is about 60ms/1.95V or 150ms/1.7V
            // Quick startup:
            // 1) Disable VCM LPF and target to a higher voltage than the required one
            // 2) Wait for a short time when VCM is in quick charge (high voltage)
            // 3) Enable VCM LPF and target to the required VCM LPF voltage
            analog_write(ANA_REG_164, SET_BITFIELD(val_164, REG_CODEC_VCM_LOW_VCM, 0));
            uint32_t delay;

#if defined(VCM_CAP_100NF)
            if (vcodec_mv >= 1900) {
                delay = 6;
            } else {
                delay = 10;
            }
#else
            if (vcodec_mv >= 1900) {
                delay = 25;
            } else {
                delay = 100;
            }
#endif
            osDelay(delay);
#if 0
            // Target to a voltage near the required one
            analog_write(ANA_REG_6B, val_6b);
            osDelay(10);
#endif
#endif // !(VCM_ON && RTOS)
            val_162 |= REG_CODEC_VCM_EN_LPF;
        }
        analog_write(ANA_REG_162, val_162);
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
    uint16_t val_162, val_6b, val_69, val_7e;
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

    if (set) {
        analog_read(ANA_REG_162, &val_162);
        analog_read(ANA_REG_6B, &val_6b);
        analog_read(ANA_REG_69, &val_69);
        analog_read(ANA_REG_7E, &val_7e);
        if (adda_common_map) {
            val_162 |= REG_CODEC_EN_BIAS;
            analog_write(ANA_REG_162, val_162);
            val_162 |= REG_CODEC_EN_VCMBUFFER;
        } else {
            val_162 &= ~REG_CODEC_EN_VCMBUFFER;
            val_69 |= REG_BYPASS_TX_REGULATOR;
            analog_write(ANA_REG_69, val_69);
            val_69 &= ~REG_PU_TX_REGULATOR;
            analog_write(ANA_REG_69, val_69);
            val_162 &= ~REG_CODEC_EN_BIAS;
        }
        analog_write(ANA_REG_162, val_162);
        analog_write(ANA_REG_6B, val_6b);
        analog_write(ANA_REG_7E, val_7e);
    }
}

static void analog_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev)
{
    uint32_t lock;
    CODEC_USER_MAP old_map;
    bool set = false;
    int i;
    uint32_t pmu_map = 0;

    lock = int_lock();

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (dev & (AUD_VMIC_MAP_VMIC1 << i)) {
            if (vmic_map[i] == 0) {
                set = true;
            }
            vmic_map[i] |= (1 << user);
        } else {
            old_map = vmic_map[i];
            vmic_map[i] &= ~(1 << user);
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
#ifdef VOICE_DETECTOR_EN
        //pmu_codec_mic_bias_lowpower_mode(pmu_map);
#endif
        if (pmu_map) {
            osDelay(1);
        }
    }
}

uint32_t analog_aud_get_max_dre_gain(void)
{
    if (vcodec_mv >= 1900) {
        return 0x11;
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
    int i;
    uint16_t val;
    uint16_t vcm, vcm_lpf;
    uint16_t vref_l, vref_r, ear_cas;

    //if (vcodec_mv >= 1900) {
    //    vcm = 7;
    //    vcm_lpf = 0xA;
    //} else if (vcodec_mv >= 1800) {
        vcm = vcm_lpf = 7;
    //} else {
    //    vcm = vcm_lpf = 7;
    //}

    //if (vcodec_mv > 1900) {
    //    vref_l = vref_r = 9;
    //    ear_cas = 3;
    //} else if (vcodec_mv == 1900) {
    //    vref_l = vref_r = 0xa;
    //    ear_cas = 3;
    //} else {
#ifdef DAC_HIGH_OUTPUT
        vref_l = vref_r = 7;
#else
        vref_l = vref_r = 2;
#endif
        ear_cas = 1;
    //}

    //0x166, 0x16a, 0x170
    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        analog_read(adc_reg[i], &val);
        val = SET_BITFIELD(val, REG_CODEC_ADCA_CH_SEL, 1);
        val = SET_BITFIELD(val, REG_CODEC_ADCA_GAIN_BIT, 0);
        val &= ~REG_CODEC_ADCA_GAIN_UPDATE;
        val |= CFG_RESET_ADC_A_DR;
        val &= ~CFG_RESET_ADC_A;
        val |= REG_CODEC_RX_EN_ADCA;
        analog_write(adc_reg[i], val);
    }

    //val = CFG_RESET_PGAA_DR | CFG_RESET_PGAB_DR | CFG_RESET_PGAC_DR | CFG_RESET_PGAD_DR;
    //analog_write(ANA_REG_62, val);

    analog_read(ANA_REG_66, &val);
#ifdef DAC_HIGH_OUTPUT
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DRE_GAIN_L, 0x11);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DRE_GAIN_R, 0x11);
#else
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DRE_GAIN_L, 0xF);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DRE_GAIN_R, 0xF);
#endif
    //val |= DRE_GAIN_SEL_L;
    //val |= DRE_GAIN_SEL_R;
#ifdef DAC_DRE_GAIN_DC_UPDATE
    val |= REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE | REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE;
#endif
    analog_write(ANA_REG_66, val);

    analog_read(ANA_REG_67, &val);
#ifdef LOW_CODEC_BIAS
    //if (vcodec_mv < 1900)
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_IBSEL, 3);
#endif
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_GAIN, 1);
    analog_write(ANA_REG_67, val);

    //val = REG_CODEC_ADC_VREF_SEL(4);
    //analog_write(ANA_REG_68, val);

    analog_read(ANA_REG_69, &val);
#ifdef DAC_HIGH_OUTPUT
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OUTPUTSEL, 0x4);
#else
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OUTPUTSEL, 0xF);
    //val |= REG_CODEC_TX_EAR_OCEN;
    //val |= REG_CODEC_TX_EAR_LPBIAS;
    val &= ~(REG_CODEC_TX_EAR_DR_EN);
#endif
    val |= REG_BYPASS_TX_REGULATOR;
    analog_write(ANA_REG_69, val);

    //val = REG_CODEC_EN_RX_EXT
    //analog_write(ANA_REG_6A, val);

    analog_read(ANA_REG_6A, &val);
    val = SET_BITFIELD(val, REG_DAC_LDO0P9_VSEL, 2);
    val |= REG_CODEC_DAC_CLK_EDGE_SEL;
    analog_write(ANA_REG_6A, val);

    analog_read(ANA_REG_6B, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_SOFTSTART, 8);
    //if (vcodec_mv >= 1900) {
    //    val |= REG_CODEC_TX_VREF_CAP_BIT;
    //}
    val |= RESERVED_ANA_7;
#ifdef DAC_HIGH_OUTPUT
    // dac l&r are controlled by TX_VREFBUF_L, and TX_VREFBUF_R is used for other function
    val = SET_BITFIELD(val, REG_CODEC_TX_VREFBUF_L_LP, 2);
#else
    val = SET_BITFIELD(val, REG_CODEC_TX_VREFBUF_L_LP, 0);
#endif
    val = SET_BITFIELD(val, REG_CODEC_TX_VREFBUF_R_LP, 0);
    analog_write(ANA_REG_6B, val);

    analog_read(ANA_REG_6C, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_FBCAP, 3);
#ifdef DAC_HIGH_OUTPUT
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_VCM_BIT, 2);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_LOWGAINL, 2);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_LOWGAINR, 2);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DR_ST_L, 2);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DR_ST_R, 2);
#else
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_VCM_BIT, 0);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_LOWGAINL, 3);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_LOWGAINR, 3);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DR_ST_L, 0);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_DR_ST_R, 0);
#endif
    analog_write(ANA_REG_6C, val);

    //REG_CODEC_TX_EAR_DR_ST(1)
    //analog_write(ANA_REG_6E, val);

    analog_read(ANA_REG_70, &val);
    val |= REG_CODEC_TX_EAR_VCM_SEL;
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_CASN_L, ear_cas);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_CASN_R, ear_cas);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_CASP_L, 0);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_CASP_R, 0);
    analog_write(ANA_REG_70, val);

    analog_read(ANA_REG_71, &val);
#ifdef DAC_HIGH_OUTPUT
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_COMP_L, 0x1B);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_COMP_R, 0x1B);
#else
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_COMP_L, 9);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_COMP_R, 9);
#endif
    analog_write(ANA_REG_71, val);

    //analog_read(ANA_REG_73, &val);
    //val |= REG_CLKMUX_DVDD_SEL;
    //analog_write(ANA_REG_73, val);

    //val = REG_CLKMUX_LDO0P9_VSEL(2) | REG_AUDPLL_LDO_VREF(4) | REG_AUDPLL_LPF_BW_SEL;
    //analog_write(ANA_REG_7E, val);

    analog_read(ANA_REG_73, &val);
    val |= REG_CLKMUX_DVDD_SEL;
    val |= REG_CLKMUX_PU_DVDD0P9;
    val = SET_BITFIELD(val, REG_CLKMUX_DVDD0P9_VSEL, 0x1f);
    val = SET_BITFIELD(val, REG_CODEC_TX_VREFBUF_LOAD, 0x7);
    analog_write(ANA_REG_73, val);

    analog_read(ANA_REG_74, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_DAC_VREF_L, vref_l);
    val = SET_BITFIELD(val, REG_CODEC_TX_DAC_VREF_R, vref_l);
    //if (vcodec_mv >= 1900)
    //    val = SET_BITFIELD(val, REG_TX_REGULATOR_BIT, 0xD);
    //else
        val = SET_BITFIELD(val, REG_TX_REGULATOR_BIT, 4);
    analog_write(ANA_REG_74, val);

    analog_read(ANA_REG_162, &val);
    val |= REG_CODEC_EN_BIAS_LP;
    val = SET_BITFIELD(val, REG_CODEC_RX_VTOI_I_DAC2, 4);
    val = SET_BITFIELD(val, REG_CODEC_RX_VTOI_IDAC_SEL, 7);
    analog_write(ANA_REG_162, val);

    analog_read(ANA_REG_163, &val);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL, 8);
    val = SET_BITFIELD(val, REG_CODEC_BUF_LOWVCM, 4);
    val = SET_BITFIELD(val, REG_CODEC_RX_VTOI_VCS_SEL, 0x10);
    val |= REG_CODEC_EN_TX_EXT;
    analog_write(ANA_REG_163, val);

    analog_read(ANA_REG_164, &val);
    val = SET_BITFIELD(val, REG_CODEC_BIAS_IBSEL_VOICE, 8);
    //if (vcodec_mv >= 1900) {
    //    val |= REG_CODEC_BIAS_IBSEL_TX(5);
    //} else {
#ifdef LOW_CODEC_BIAS
        val |= REG_CODEC_BIAS_IBSEL_TX(1);
#else
        val |= REG_CODEC_BIAS_IBSEL_TX(3);
#endif
    //}
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM, vcm);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM_LPF, vcm_lpf);
    analog_write(ANA_REG_164, val);

    analog_read(ANA_REG_165, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADC_LPFVCM_SW, 0xF);
    val = SET_BITFIELD(val, REG_CODEC_VCM_LOW_VCM_LP, vcm);
    analog_write(ANA_REG_165, val);

    analog_read(ANA_REG_16D, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADCB_VREFBUF_BIT_IN, 8);
    analog_write(ANA_REG_16D, val);

    analog_read(ANA_REG_176, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADCC_VREFBUF_BIT_IN, 8);
    analog_write(ANA_REG_176, val);

    analog_read(ANA_REG_17B, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADC_OP1_IBIT, 2);
    analog_write(ANA_REG_17B, val);

    analog_read(ANA_REG_17C, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADC_IBSEL_REG, 8);
    val = SET_BITFIELD(val, REG_CODEC_ADC_REG_VSEL, 6);
    analog_write(ANA_REG_17C, val);

    analog_read(ANA_REG_17D, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADCA_VREF_SEL, 5);
    val = SET_BITFIELD(val, REG_CODEC_ADCB_VREF_SEL, 5);
    val = SET_BITFIELD(val, REG_CODEC_ADCC_VREF_SEL, 5);
    analog_write(ANA_REG_17D, val);

    analog_read(ANA_REG_17E, &val);
    val = SET_BITFIELD(val, REG_CODEC_ADC_IBSEL_VCOMP, 8);
    val = SET_BITFIELD(val, REG_CODEC_ADC_IBSEL_VREF, 8);
    analog_write(ANA_REG_17E, val);

    analog_aud_osc_clk_enable(false);

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
#ifdef VOICE_DETECTOR_EN
    if ((adda_common_map & (1 << ANA_CODEC_USER_VAD)) && (adc_map[2] & (1 << ANA_CODEC_USER_VAD)) == 0) {
        uint16_t v;

        analog_read(ANA_REG_162, &v);
        v = (v & ~REG_CODEC_EN_VCMBUFFER) | REG_CODEC_LP_VCM;
        analog_write(ANA_REG_162, v);
    }
#endif
}

void analog_wakeup(void)
{
#ifdef VOICE_DETECTOR_EN
    if ((adda_common_map & (1 << ANA_CODEC_USER_VAD)) && (adc_map[2] & (1 << ANA_CODEC_USER_VAD)) == 0) {
        uint16_t v;

        analog_read(ANA_REG_162, &v);
        v = (v & ~REG_CODEC_LP_VCM) | REG_CODEC_EN_VCMBUFFER;
        analog_write(ANA_REG_162, v);
    }
#endif
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
        AUD_CHANNEL_MAP_CH2;

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
    return 0;
}

int analog_debug_config_low_power_adc(bool enable)
{
    return 0;
}

int analog_debug_config_vad_mic(bool enable)
{
    return 0;
}

static void analog_aud_codec_vad_ini(void)
{
#if 1
    unsigned short v;

#if 0
    analog_read(ANA_REG_6B, &v);
    v &= ~REG_CODEC_TX_VREF_CAP_BIT;
    v |= REG_BYPASS_TX_REGULATOR;
    v &= ~REG_PU_TX_REGULATOR;
    analog_write(ANA_REG_6B, v);

    analog_read(ANA_REG_74, &v);
    v = SET_BITFIELD(v, REG_TX_REGULATOR_BIT, 0x5);
    analog_write(ANA_REG_74, v);

    analog_read(ANA_REG_162, &v);
    v &= ~REG_CODEC_EN_VCMBUFFER;
    analog_write(ANA_REG_162, v);

    analog_read(ANA_REG_164, &v);
    v = SET_BITFIELD(v, REG_CODEC_VCM_LOW_VCM, 0x7);
    v = SET_BITFIELD(v, REG_CODEC_VCM_LOW_VCM_LPF, 0x7);
    analog_write(ANA_REG_164, v);

    analog_read(ANA_REG_165, &v);
    v = SET_BITFIELD(v, REG_CODEC_ADC_LPFVCM_SW, 0xF);
    v = SET_BITFIELD(v, REG_CODEC_VCM_LOW_VCM_LP, 0x7);
    analog_write(ANA_REG_165, v);
#endif

    analog_read(ANA_REG_77, &v);
    v |= CFG_VOICE_RESET;
    v = SET_BITFIELD(v, CFG_VOICE_PU_DELAY, 0x4);
    v = SET_BITFIELD(v, REG_VOICE_HIGH_TUNE_A, 0xa);
    v = SET_BITFIELD(v, REG_VOICE_HIGH_TUNE_B, 0x4);
    analog_write(ANA_REG_77, v);

    analog_read(ANA_REG_78, &v);
    v = SET_BITFIELD(v, CFG_VOICE_PRECH_DELAY, 0x7f);
    analog_write(ANA_REG_78, v);

    analog_read(ANA_REG_79, &v);
    v = SET_BITFIELD(v, CFG_VOICE_DET_DELAY, 0x8);
    analog_write(ANA_REG_79, v);

    analog_read(ANA_REG_7A, &v);
    v = SET_BITFIELD(v, REG_VOICE_LDO_VBIT, 0x5);
    v = SET_BITFIELD(v, REG_VOICE_GAIN_A, 0x3);
    v = SET_BITFIELD(v, REG_VOICE_GAIN_B, 0x3);
    analog_write(ANA_REG_7A, v);

    analog_read(ANA_REG_7D, &v);
    v = SET_BITFIELD(v, REG_VOICE_LOW_TUNE, 0x3);
    analog_write(ANA_REG_7D, v);

    analog_read(ANA_REG_81, &v);
    v = REG_CLK_GOAL_DIV_NUM(0x17f)
        | REG_COARSE_TUN_CODE_DR;
    analog_write(ANA_REG_81, v);

    osDelay(1);
#endif
}

static void analog_aud_codec_vad_trig_en(bool en)
{
#if 1
    unsigned short v;
    uint32_t lock;

    lock = int_lock();
    analog_read(ANA_REG_77, &v);
    if (en)
        v |= CFG_VOICE_TRIG_ENABLE;
    else
        v &= ~CFG_VOICE_TRIG_ENABLE;

    analog_write(ANA_REG_77, v);

    int_unlock(lock);
#endif
}

static void analog_aud_codec_vad_pu(bool en)
{
#if 1
    unsigned short v;
    uint32_t lock;

    lock = int_lock();
    analog_read(ANA_REG_77, &v);
    if (en)
        v |= CFG_VOICE_POWER_ON;
    else
        v &= ~CFG_VOICE_POWER_ON;

    analog_write(ANA_REG_77, v);
    int_unlock(lock);
#endif
}

void analog_aud_vad_enable(enum AUD_VAD_TYPE_T type, bool en)
{
    if (type == AUD_VAD_TYPE_DIG) {
        uint32_t dev;
        enum AUD_CHANNEL_MAP_T ch_map;

        dev = hal_codec_get_input_path_cfg(AUD_INPUT_PATH_VADMIC);
        ch_map = dev & AUD_CHANNEL_MAP_ALL;
        analog_aud_enable_adc(ANA_CODEC_USER_VAD, ch_map, en);
    } else if (type == AUD_VAD_TYPE_MIX || type == AUD_VAD_TYPE_ANA) {
        if (en) {
            analog_aud_codec_vad_ini();
            analog_aud_codec_vad_pu(true);
            osDelay(1);
            analog_aud_codec_vad_trig_en(true);
            //pmu_codec_vad_save_power();
        } else {
            //pmu_codec_vad_restore_power();
            analog_aud_codec_vad_pu(false);
            osDelay(1);
            analog_aud_codec_vad_trig_en(false);
        }
    }
}

void analog_aud_vad_adc_enable(bool en)
{
    uint32_t dev;
    enum AUD_CHANNEL_MAP_T ch_map;

    dev = hal_codec_get_input_path_cfg(AUD_INPUT_PATH_VADMIC);
    ch_map = dev & AUD_CHANNEL_MAP_ALL;

    analog_aud_osc_clk_enable(true);

    if (en) {
        // Enable vmic first to overlap vmic stable time with codec vcm stable time
        analog_aud_enable_vmic(ANA_CODEC_USER_VAD, dev);
        analog_aud_enable_codec_common(ANA_CODEC_USER_VAD, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_VAD, true);
        analog_aud_set_adc_gain(AUD_INPUT_PATH_VADMIC, ch_map);
    } else {
        analog_aud_enable_adda_common(ANA_CODEC_USER_VAD, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_VAD, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_VAD, 0);
    }
}

