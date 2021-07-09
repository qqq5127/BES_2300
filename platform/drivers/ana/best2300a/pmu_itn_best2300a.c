/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#include "pmu.h"
#include "analog.h"
#include "cmsis.h"
#include "cmsis_nvic.h"
#include "hal_aud.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "tgt_hardware.h"
#include "usbphy.h"

#define PMU_VCORE_STABLE_TIME_US        10

//#define PMU_VDIG_1_2V                   0x1E
#define PMU_VDIG_1_1V                   0x1C
#define PMU_VDIG_1_05V                  0x1A
#define PMU_VDIG_1_0V                   0x18
#define PMU_VDIG_0_95V                  0x16
#define PMU_VDIG_0_9V                   0x14
#define PMU_VDIG_0_85V                  0x12
#define PMU_VDIG_0_8V                   0x10
#define PMU_VDIG_0_75V                  0x0E
#define PMU_VDIG_0_7V                   0x0C
#define PMU_VDIG_0_65V                  0x0A
#define PMU_VDIG_0_6V                   0x08
//#define PMU_VDIG_MAX                    PMU_VDIG_1_2V

// REG_00
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_01
#define LPO_OFF_CNT_SHIFT                   12
#define LPO_OFF_CNT_MASK                    (0xF << LPO_OFF_CNT_SHIFT)
#define LPO_OFF_CNT(n)                      BITFIELD_VAL(LPO_OFF_CNT, n)
#define POWER_ON_DB_CNT_SHIFT               8
#define POWER_ON_DB_CNT_MASK                (0xF << POWER_ON_DB_CNT_SHIFT)
#define POWER_ON_DB_CNT(n)                  BITFIELD_VAL(POWER_ON_DB_CNT, n)
#define POWER_OFF_CNT_SHIFT                 4
#define POWER_OFF_CNT_MASK                  (0xF << POWER_OFF_CNT_SHIFT)
#define POWER_OFF_CNT(n)                    BITFIELD_VAL(POWER_OFF_CNT, n)
#define CLK_STABLE_CNT_SHIFT                0
#define CLK_STABLE_CNT_MASK                 (0xF << CLK_STABLE_CNT_SHIFT)
#define CLK_STABLE_CNT(n)                   BITFIELD_VAL(CLK_STABLE_CNT, n)

// REG_02
#define REG_PU_VBAT_DIV                     (1 << 15)
#define PU_LPO_DR                           (1 << 14)
#define PU_LPO_REG                          (1 << 13)
#define POWERKEY_WAKEUP_OSC_EN              (1 << 12)
#define RTC_POWER_ON_EN                     (1 << 11)
#define PU_ALL_REG                          (1 << 10)
#define RESERVED_ANA_16                     (1 << 9)
#define REG_CLK_32K_SEL                     (1 << 8)
#define DEEPSLEEP_MODE_DIGI_DR              (1 << 7)
#define DEEPSLEEP_MODE_DIGI_REG             (1 << 6)
#define RESERVED_ANA_18_17_SHIFT            4
#define RESERVED_ANA_18_17_MASK             (0x3 << RESERVED_ANA_18_17_SHIFT)
#define RESERVED_ANA_18_17(n)               BITFIELD_VAL(RESERVED_ANA_18_17, n)
#define RESETN_ANA_DR                       (1 << 3)
#define RESETN_ANA_REG                      (1 << 2)
#define RESETN_DIG_DR                       (1 << 1)
#define RESETN_DIG_REG                      (1 << 0)

// REG_03
#define RESERVED_ANA_19                     (1 << 15)
#define PU_LP_BIAS_LDO_DSLEEP_MSK           (1 << 14)
#define PU_LP_BIAS_LDO_DR                   (1 << 13)
#define PU_LP_BIAS_LDO_REG                  (1 << 12)
#define PU_BIAS_LDO_DR                      (1 << 11)
#define PU_BIAS_LDO_REG                     (1 << 10)
#define BG_VBG_SEL_DR                       (1 << 9)
#define BG_VBG_SEL_REG                      (1 << 8)
#define BG_CONSTANT_GM_BIAS_DR              (1 << 7)
#define BG_CONSTANT_GM_BIAS_REG             (1 << 6)
#define BG_CORE_EN_DR                       (1 << 5)
#define BG_CORE_EN_REG                      (1 << 4)
#define BG_VTOI_EN_DR                       (1 << 3)
#define BG_VTOI_EN_REG                      (1 << 2)
#define BG_NOTCH_EN_DR                      (1 << 1)
#define BG_NOTCH_EN_REG                     (1 << 0)

// REG_04
#define BG_NOTCH_LPF_LOW_BW                 (1 << 15)
#define BG_OPX2                             (1 << 14)
#define BG_PTATX2                           (1 << 13)
#define BG_VBG_RES_SHIFT                    9
#define BG_VBG_RES_MASK                     (0xF << BG_VBG_RES_SHIFT)
#define BG_VBG_RES(n)                       BITFIELD_VAL(BG_VBG_RES, n)
#define BG_CONSTANT_GM_BIAS_BIT_SHIFT       7
#define BG_CONSTANT_GM_BIAS_BIT_MASK        (0x3 << BG_CONSTANT_GM_BIAS_BIT_SHIFT)
#define BG_CONSTANT_GM_BIAS_BIT(n)          BITFIELD_VAL(BG_CONSTANT_GM_BIAS_BIT, n)
#define BG_VTOI_IABS_BIT_SHIFT              2
#define BG_VTOI_IABS_BIT_MASK               (0x1F << BG_VTOI_IABS_BIT_SHIFT)
#define BG_VTOI_IABS_BIT(n)                 BITFIELD_VAL(BG_VTOI_IABS_BIT, n)
#define BG_VTOI_VCAS_BIT_SHIFT              0
#define BG_VTOI_VCAS_BIT_MASK               (0x3 << BG_VTOI_VCAS_BIT_SHIFT)
#define BG_VTOI_VCAS_BIT(n)                 BITFIELD_VAL(BG_VTOI_VCAS_BIT, n)

// REG_06
#define REG_BYPASS_VBG_RF_BUFFER_DR         (1 << 15)
#define REG_BYPASS_VBG_RF_BUFFER_REG        (1 << 14)
#define PU_VBG_RF_BUFFER_DR                 (1 << 13)
#define PU_VBG_RF_BUFFER_REG                (1 << 12)
#define RESERVED_ANA_21_20_SHIFT            10
#define RESERVED_ANA_21_20_MASK             (0x3 << RESERVED_ANA_21_20_SHIFT)
#define RESERVED_ANA_21_20(n)               BITFIELD_VAL(RESERVED_ANA_21_20, n)
#define REG_LPO_KTRIM_SHIFT                 4
#define REG_LPO_KTRIM_MASK                  (0x3F << REG_LPO_KTRIM_SHIFT)
#define REG_LPO_KTRIM(n)                    BITFIELD_VAL(REG_LPO_KTRIM, n)
#define REG_LPO_ITRIM_SHIFT                 0
#define REG_LPO_ITRIM_MASK                  (0xF << REG_LPO_ITRIM_SHIFT)
#define REG_LPO_ITRIM(n)                    BITFIELD_VAL(REG_LPO_ITRIM, n)

// REG_07
#define BG_TRIM_VBG_SHIFT                   13
#define BG_TRIM_VBG_MASK                    (0x7 << BG_TRIM_VBG_SHIFT)
#define BG_TRIM_VBG(n)                      BITFIELD_VAL(BG_TRIM_VBG, n)
#define BG_R_TEMP_SHIFT                     10
#define BG_R_TEMP_MASK                      (0x7 << BG_R_TEMP_SHIFT)
#define BG_R_TEMP(n)                        BITFIELD_VAL(BG_R_TEMP, n)
#define REG_LPO_FREQ_TRIM_SHIFT             5
#define REG_LPO_FREQ_TRIM_MASK              (0x1F << REG_LPO_FREQ_TRIM_SHIFT)
#define REG_LPO_FREQ_TRIM(n)                BITFIELD_VAL(REG_LPO_FREQ_TRIM, n)
#define REG_LIGHT_LOAD_OPTION_VCORE2        (1 << 4)
#define REG_LOOP_CTRL_VCORE1_LDO            (1 << 3)
#define REG_PULLDOWN_VCORE1                 (1 << 2)
#define REG_LOOP_CTRL_VCORE2_LDO            (1 << 1)
#define REG_PULLDOWN_VCORE2                 (1 << 0)

// REG_08
#define LP_EN_VCORE1_LDO_DSLEEP_EN          (1 << 15)
#define LP_EN_VCORE1_LDO_DR                 (1 << 14)
#define LP_EN_VCORE1_LDO_REG                (1 << 13)
#define REG_PU_VCORE1_LDO_DSLEEP_MSK        (1 << 12)
#define REG_PU_VCORE1_LDO_DR                (1 << 11)
#define REG_PU_VCORE1_LDO_REG               (1 << 10)
#define LDO_VCORE1_VBIT_DSLEEP_SHIFT        5
#define LDO_VCORE1_VBIT_DSLEEP_MASK         (0x1F << LDO_VCORE1_VBIT_DSLEEP_SHIFT)
#define LDO_VCORE1_VBIT_DSLEEP(n)           BITFIELD_VAL(LDO_VCORE1_VBIT_DSLEEP, n)
#define LDO_VCORE1_VBIT_NORMAL_SHIFT        0
#define LDO_VCORE1_VBIT_NORMAL_MASK         (0x1F << LDO_VCORE1_VBIT_NORMAL_SHIFT)
#define LDO_VCORE1_VBIT_NORMAL(n)           BITFIELD_VAL(LDO_VCORE1_VBIT_NORMAL, n)

// REG_09
#define LP_EN_VCORE2_LDO_DSLEEP_EN          (1 << 15)
#define LP_EN_VCORE2_LDO_DR                 (1 << 14)
#define LP_EN_VCORE2_LDO_REG                (1 << 13)
#define REG_PU_VCORE2_LDO_DSLEEP_MSK        (1 << 12)
#define REG_PU_VCORE2_LDO_DR                (1 << 11)
#define REG_PU_VCORE2_LDO_REG               (1 << 10)
#define LDO_VCORE2_VBIT_DSLEEP_SHIFT        5
#define LDO_VCORE2_VBIT_DSLEEP_MASK         (0x1F << LDO_VCORE2_VBIT_DSLEEP_SHIFT)
#define LDO_VCORE2_VBIT_DSLEEP(n)           BITFIELD_VAL(LDO_VCORE2_VBIT_DSLEEP, n)
#define LDO_VCORE2_VBIT_NORMAL_SHIFT        0
#define LDO_VCORE2_VBIT_NORMAL_MASK         (0x1F << LDO_VCORE2_VBIT_NORMAL_SHIFT)
#define LDO_VCORE2_VBIT_NORMAL(n)           BITFIELD_VAL(LDO_VCORE2_VBIT_NORMAL, n)

// REG_0A
#define LP_EN_MIC_LDO_DSLEEP_EN             (1 << 15)
#define LP_EN_MIC_LDO_DR                    (1 << 14)
#define LP_EN_MIC_LDO_REG                   (1 << 13)
#define REG_PU_MIC_LDO_DSLEEP_MSK           (1 << 12)
#define REG_PU_MIC_LDO_DR                   (1 << 11)
#define REG_PU_MIC_LDO_REG                  (1 << 10)

// REG_0B
#define LP_EN_MIC_BIASA_DSLEEP_EN           (1 << 15)
#define LP_EN_MIC_BIASA_DR                  (1 << 14)
#define LP_EN_MIC_BIASA_REG                 (1 << 13)
#define REG_PU_MIC_BIASA_DSLEEP_MSK         (1 << 12)
#define REG_PU_MIC_BIASA_DR                 (1 << 11)
#define REG_PU_MIC_BIASA_REG                (1 << 10)
#define LP_EN_MIC_BIASB_DSLEEP_EN           (1 << 9)
#define LP_EN_MIC_BIASB_DR                  (1 << 8)
#define LP_EN_MIC_BIASB_REG                 (1 << 7)
#define REG_PU_MIC_BIASB_DSLEEP_MSK         (1 << 6)
#define REG_PU_MIC_BIASB_DR                 (1 << 5)
#define REG_PU_MIC_BIASB_REG                (1 << 4)

// REG_0C
#define REG_VCORE_SSTIME_MODE_SHIFT         14
#define REG_VCORE_SSTIME_MODE_MASK          (0x3 << REG_VCORE_SSTIME_MODE_SHIFT)
#define REG_VCORE_SSTIME_MODE(n)            BITFIELD_VAL(REG_VCORE_SSTIME_MODE, n)
#define REG_STON_SW_VBG_TIME_SHIFT          6
#define REG_STON_SW_VBG_TIME_MASK           (0x1F << REG_STON_SW_VBG_TIME_SHIFT)
#define REG_STON_SW_VBG_TIME(n)             BITFIELD_VAL(REG_STON_SW_VBG_TIME, n)

#define LP_EN_VTOI_DSLEEP_EN                (1 << 5)
#define LP_EN_VTOI_DR                       (1 << 4)
#define LP_EN_VTOI_REG                      (1 << 3)
#define REG_PU_VTOI_DSLEEP_MSK              (1 << 2)
#define REG_PU_VTOI_DR                      (1 << 1)
#define REG_PU_VTOI_REG                     (1 << 0)

// REG_0D
#define REG_BG_SLEEP_MSK                    (1 << 12)
#define REG_MIC_BIASA_PULL_DOWN_DR          (1 << 11)
#define REG_MIC_BIASA_PULL_DOWN             (1 << 10)
#define REG_MIC_BIASB_PULL_DOWN_DR          (1 << 9)
#define REG_MIC_BIASB_PULL_DOWN             (1 << 8)
#define REG_LP_BIAS_SEL_LDO_SHIFT           6
#define REG_LP_BIAS_SEL_LDO_MASK            (0x3 << REG_LP_BIAS_SEL_LDO_SHIFT)
#define REG_LP_BIAS_SEL_LDO(n)              BITFIELD_VAL(REG_LP_BIAS_SEL_LDO, n)
#define REG_VTOI_VCAS_BIT_SHIFT             4
#define REG_VTOI_VCAS_BIT_MASK              (0x3 << REG_VTOI_VCAS_BIT_SHIFT)
#define REG_VTOI_VCAS_BIT(n)                BITFIELD_VAL(REG_VTOI_VCAS_BIT, n)
#define EN_VBG_TEST                         (1 << 3)
#define REG_PU_AVDD25_ANA                   (1 << 2)
#define REG_BYPASS_VCORE2                   (1 << 1)
#define REG_BYPASS_VCORE1                   (1 << 0)

// REG_0F
#define RESERVED_ANA_22                     (1 << 15)
#define REG_UVLO_SEL_SHIFT                  12
#define REG_UVLO_SEL_MASK                   (0x3 << REG_UVLO_SEL_SHIFT)
#define REG_UVLO_SEL(n)                     BITFIELD_VAL(REG_UVLO_SEL, n)
#define POWER_UP_SOFT_CNT_SHIFT             8
#define POWER_UP_SOFT_CNT_MASK              (0xF << POWER_UP_SOFT_CNT_SHIFT)
#define POWER_UP_SOFT_CNT(n)                BITFIELD_VAL(POWER_UP_SOFT_CNT, n)
#define POWER_UP_BIAS_CNT_SHIFT             4
#define POWER_UP_BIAS_CNT_MASK              (0xF << POWER_UP_BIAS_CNT_SHIFT)
#define POWER_UP_BIAS_CNT(n)                BITFIELD_VAL(POWER_UP_BIAS_CNT, n)

// REG_10
#define POWER_UP_MOD1_CNT_SHIFT             0
#define POWER_UP_MOD1_CNT_MASK              (0xFF << POWER_UP_MOD1_CNT_SHIFT)
#define POWER_UP_MOD1_CNT(n)                BITFIELD_VAL(POWER_UP_MOD1_CNT, n)
#define POWER_UP_MOD2_CNT_SHIFT             8
#define POWER_UP_MOD2_CNT_MASK              (0xFF << POWER_UP_MOD2_CNT_SHIFT)
#define POWER_UP_MOD2_CNT(n)                BITFIELD_VAL(POWER_UP_MOD2_CNT, n)

// REG_11
#define POWER_UP_MOD3_CNT_SHIFT             0
#define POWER_UP_MOD3_CNT_MASK              (0xFF << POWER_UP_MOD3_CNT_SHIFT)
#define POWER_UP_MOD3_CNT(n)                BITFIELD_VAL(POWER_UP_MOD3_CNT, n)
#define POWER_UP_MOD4_CNT_SHIFT             8
#define POWER_UP_MOD4_CNT_MASK              (0xFF << POWER_UP_MOD4_CNT_SHIFT)
#define POWER_UP_MOD4_CNT(n)                BITFIELD_VAL(POWER_UP_MOD4_CNT, n)

// REG_12
#define POWER_UP_MOD5_CNT_SHIFT             0
#define POWER_UP_MOD5_CNT_MASK              (0xFF << POWER_UP_MOD5_CNT_SHIFT)
#define POWER_UP_MOD5_CNT(n)                BITFIELD_VAL(POWER_UP_MOD5_CNT, n)
#define POWER_UP_MOD6_CNT_SHIFT             8
#define POWER_UP_MOD6_CNT_MASK              (0xFF << POWER_UP_MOD6_CNT_SHIFT)
#define POWER_UP_MOD6_CNT(n)                BITFIELD_VAL(POWER_UP_MOD6_CNT, n)

// REG_13
#define POWER_UP_MOD7_CNT_SHIFT             0
#define POWER_UP_MOD7_CNT_MASK              (0xFF << POWER_UP_MOD7_CNT_SHIFT)
#define POWER_UP_MOD7_CNT(n)                BITFIELD_VAL(POWER_UP_MOD7_CNT, n)
#define POWER_UP_MOD8_CNT_SHIFT             8
#define POWER_UP_MOD8_CNT_MASK              (0xFF << POWER_UP_MOD8_CNT_SHIFT)
#define POWER_UP_MOD8_CNT(n)                BITFIELD_VAL(POWER_UP_MOD8_CNT, n)

// REG_14
#define TEST_MODE_SHIFT                     13
#define TEST_MODE_MASK                      (0x7 << TEST_MODE_SHIFT)
#define TEST_MODE(n)                        BITFIELD_VAL(TEST_MODE, n)
#define REG_EFUSE_REDUNDANCY_DATA_OUT_SHIFT 2
#define REG_EFUSE_REDUNDANCY_DATA_OUT_MASK  (0xF << REG_EFUSE_REDUNDANCY_DATA_OUT_SHIFT)
#define REG_EFUSE_REDUNDANCY_DATA_OUT(n)    BITFIELD_VAL(REG_EFUSE_REDUNDANCY_DATA_OUT, n)
#define REG_EFUSE_REDUNDANCY_DATA_OUT_DR    (1 << 1)
#define REG_EFUSE_DATA_OUT_DR               (1 << 0)

// REG_15
#define REG_EFUSE_DATA_OUT_HI_SHIFT         0
#define REG_EFUSE_DATA_OUT_HI_MASK          (0xFFFF << REG_EFUSE_DATA_OUT_HI_SHIFT)
#define REG_EFUSE_DATA_OUT_HI(n)            BITFIELD_VAL(REG_EFUSE_DATA_OUT_HI, n)

// REG_16
#define REG_EFUSE_DATA_OUT_LO_SHIFT         0
#define REG_EFUSE_DATA_OUT_LO_MASK          (0xFFFF << REG_EFUSE_DATA_OUT_LO_SHIFT)
#define REG_EFUSE_DATA_OUT_LO(n)            BITFIELD_VAL(REG_EFUSE_DATA_OUT_LO, n)

// REG_17
#define REG_SS_VCORE_EN                     (1 << 15)
#define REG_PWR_SEL_REG                     (1 << 14)

// REG_1A
#define REG_PMU_VSEL1_SHIFT                 13
#define REG_PMU_VSEL1_MASK                  (0x7 << REG_PMU_VSEL1_SHIFT)
#define REG_PMU_VSEL1(n)                    BITFIELD_VAL(REG_PMU_VSEL1, n)
#define REG_POWER_SEL_CNT_SHIFT             8
#define REG_POWER_SEL_CNT_MASK              (0x1F << REG_POWER_SEL_CNT_SHIFT)
#define REG_POWER_SEL_CNT(n)                BITFIELD_VAL(REG_POWER_SEL_CNT, n)
#define REG_PWR_SEL_DR                      (1 << 7)
#define REG_PWR_SEL                         (1 << 6)
#define CLK_BG_EN                           (1 << 5)
#define CLK_BG_DIV_VALUE_SHIFT              0
#define CLK_BG_DIV_VALUE_MASK               (0x1F << CLK_BG_DIV_VALUE_SHIFT)
#define CLK_BG_DIV_VALUE(n)                 BITFIELD_VAL(CLK_BG_DIV_VALUE, n)

// REG_1D
#define CHIP_ADDR_I2C_SHIFT                 8
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define SLEEP_ALLOW                         (1 << 15)

// REG_1E
#define RESERVED_ANA_24                     (1 << 15)

// REG_1F
#define EXT_INTR_MSK                        (1 << 15)
#define RTC_INT1_MSK                        (1 << 14)
#define RTC_INT0_MSK                        (1 << 13)

// REG_20
#define EXT_INTR_EN                         (1 << 15)
#define RTC_INT_EN_1                        (1 << 14)
#define RTC_INT_EN_0                        (1 << 13)

// REG_21
#define RESERVED_ANA_26_25_SHIFT            14
#define RESERVED_ANA_26_25_MASK             (0x3 << RESERVED_ANA_26_25_SHIFT)
#define RESERVED_ANA_26_25(n)               BITFIELD_VAL(RESERVED_ANA_26_25, n)
#define REG_VCORE2_RAMP_EN                  (1 << 9)
#define REG_VCORE1_RAMP_EN                  (1 << 8)

// REG_22
#define CFG_DIV_RTC_1HZ_SHIFT               0
#define CFG_DIV_RTC_1HZ_MASK                (0xFFFF << CFG_DIV_RTC_1HZ_SHIFT)
#define CFG_DIV_RTC_1HZ(n)                  BITFIELD_VAL(CFG_DIV_RTC_1HZ, n)

// REG_23
#define RTC_LOAD_VALUE_15_0_SHIFT           0
#define RTC_LOAD_VALUE_15_0_MASK            (0xFFFF << RTC_LOAD_VALUE_15_0_SHIFT)
#define RTC_LOAD_VALUE_15_0(n)              BITFIELD_VAL(RTC_LOAD_VALUE_15_0, n)

// REG_24
#define RTC_LOAD_VALUE_31_16_SHIFT          0
#define RTC_LOAD_VALUE_31_16_MASK           (0xFFFF << RTC_LOAD_VALUE_31_16_SHIFT)
#define RTC_LOAD_VALUE_31_16(n)             BITFIELD_VAL(RTC_LOAD_VALUE_31_16, n)

// REG_25
#define RTC_MATCH_VALUE_0_15_0_SHIFT        0
#define RTC_MATCH_VALUE_0_15_0_MASK         (0xFFFF << RTC_MATCH_VALUE_0_15_0_SHIFT)
#define RTC_MATCH_VALUE_0_15_0(n)           BITFIELD_VAL(RTC_MATCH_VALUE_0_15_0, n)

// REG_26
#define RTC_MATCH_VALUE_0_31_16_SHIFT       0
#define RTC_MATCH_VALUE_0_31_16_MASK        (0xFFFF << RTC_MATCH_VALUE_0_31_16_SHIFT)
#define RTC_MATCH_VALUE_0_31_16(n)          BITFIELD_VAL(RTC_MATCH_VALUE_0_31_16, n)

// REG_27
#define RTC_MATCH_VALUE_1_15_0_SHIFT        0
#define RTC_MATCH_VALUE_1_15_0_MASK         (0xFFFF << RTC_MATCH_VALUE_1_15_0_SHIFT)
#define RTC_MATCH_VALUE_1_15_0(n)           BITFIELD_VAL(RTC_MATCH_VALUE_1_15_0, n)

// REG_28
#define RTC_MATCH_VALUE_1_31_16_SHIFT       0
#define RTC_MATCH_VALUE_1_31_16_MASK        (0xFFFF << RTC_MATCH_VALUE_1_31_16_SHIFT)
#define RTC_MATCH_VALUE_1_31_16(n)          BITFIELD_VAL(RTC_MATCH_VALUE_1_31_16, n)

// REG_29
#define REG_MIC_BIASA_CHANSEL_SHIFT         14
#define REG_MIC_BIASA_CHANSEL_MASK          (0x3 << REG_MIC_BIASA_CHANSEL_SHIFT)
#define REG_MIC_BIASA_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASA_CHANSEL, n)
#define REG_MIC_BIASA_EN                    (1 << 13)
#define REG_MIC_BIASA_ENLPF                 (1 << 12)
#define REG_MIC_BIASA_LPFSEL_SHIFT          9
#define REG_MIC_BIASA_LPFSEL_MASK           (0x7 << REG_MIC_BIASA_LPFSEL_SHIFT)
#define REG_MIC_BIASA_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASA_LPFSEL, n)
#define REG_MIC_BIASA_VSEL_SHIFT            3
#define REG_MIC_BIASA_VSEL_MASK             (0x3F << REG_MIC_BIASA_VSEL_SHIFT)
#define REG_MIC_BIASA_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASA_VSEL, n)
#define RESERVED_ANA_28_27_SHIFT            1
#define RESERVED_ANA_28_27_MASK             (0x3 << RESERVED_ANA_28_27_SHIFT)
#define RESERVED_ANA_28_27(n)               BITFIELD_VAL(RESERVED_ANA_28_27, n)
#define RESERVED_ANA_23                     (1 << 0)

// REG_2A
#define REG_MIC_BIASB_CHANSEL_SHIFT         14
#define REG_MIC_BIASB_CHANSEL_MASK          (0x3 << REG_MIC_BIASB_CHANSEL_SHIFT)
#define REG_MIC_BIASB_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASB_CHANSEL, n)
#define REG_MIC_BIASB_EN                    (1 << 13)
#define REG_MIC_BIASB_ENLPF                 (1 << 12)
#define REG_MIC_BIASB_LPFSEL_SHIFT          9
#define REG_MIC_BIASB_LPFSEL_MASK           (0x7 << REG_MIC_BIASB_LPFSEL_SHIFT)
#define REG_MIC_BIASB_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASB_LPFSEL, n)
#define REG_MIC_BIASB_VSEL_SHIFT            3
#define REG_MIC_BIASB_VSEL_MASK             (0x3F << REG_MIC_BIASB_VSEL_SHIFT)
#define REG_MIC_BIASB_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASB_VSEL, n)
#define REG_MIC_LDO_EN                      (1 << 2)
#define REG_MIC_LDO_PULLDOWN                (1 << 1)
#define REG_MIC_LDO_LOOPCTRL                (1 << 0)

// REG_2B
#define REG_CLK_26M_DIV_SHIFT               0
#define REG_CLK_26M_DIV_MASK                (0x3FF << REG_CLK_26M_DIV_SHIFT)
#define REG_CLK_26M_DIV(n)                  BITFIELD_VAL(REG_CLK_26M_DIV, n)

// REG_30
#define RESERVED_ANA_29                     (1 << 15)
#define RESETN_DB_NUMBER_SHIFT              11
#define RESETN_DB_NUMBER_MASK               (0xF << RESETN_DB_NUMBER_SHIFT)
#define RESETN_DB_NUMBER(n)                 BITFIELD_VAL(RESETN_DB_NUMBER, n)
#define RESETN_DB_EN                        (1 << 10)
#define REG_MIC_LDO_RES_SHIFT               0
#define REG_MIC_LDO_RES_MASK                (0x1F << REG_MIC_LDO_RES_SHIFT)
#define REG_MIC_LDO_RES(n)                  BITFIELD_VAL(REG_MIC_LDO_RES, n)

// REG_34
#define RESERVED_ANA_30                     (1 << 15)
#define CLK_32K_COUNT_NUM_SHIFT             11
#define CLK_32K_COUNT_NUM_MASK              (0xF << CLK_32K_COUNT_NUM_SHIFT)
#define CLK_32K_COUNT_NUM(n)                BITFIELD_VAL(CLK_32K_COUNT_NUM, n)

// REG_35
#define REG_DIV_HW_RESET_SHIFT              0
#define REG_DIV_HW_RESET_MASK               (0xFFFF << REG_DIV_HW_RESET_SHIFT)
#define REG_DIV_HW_RESET(n)                 BITFIELD_VAL(REG_DIV_HW_RESET, n)

// REG_36
#define REG_WDT_TIMER_SHIFT                 0
#define REG_WDT_TIMER_MASK                  (0xFFFF << REG_WDT_TIMER_SHIFT)
#define REG_WDT_TIMER(n)                    BITFIELD_VAL(REG_WDT_TIMER, n)

// REG_37
#define RESERVED_ANA_32_31_SHIFT            14
#define RESERVED_ANA_32_31_MASK             (0x3 << RESERVED_ANA_32_31_SHIFT)
#define RESERVED_ANA_32_31(n)               BITFIELD_VAL(RESERVED_ANA_32_31, n)
#define CLK_32K_COUNT_EN_TRIGGER            (1 << 13)
#define CLK_32K_COUNT_EN                    (1 << 12)
#define CLK_32K_COUNT_CLOCK_EN              (1 << 11)
#define POWERON_DETECT_EN                   (1 << 10)
#define REG_WDT_EN                          (1 << 8)
#define REG_WDT_RESET_EN                    (1 << 7)
#define REG_HW_RESET_TIME_SHIFT             1
#define REG_HW_RESET_TIME_MASK              (0x3F << REG_HW_RESET_TIME_SHIFT)
#define REG_HW_RESET_TIME(n)                BITFIELD_VAL(REG_HW_RESET_TIME, n)
#define REG_HW_RESET_EN                     (1 << 0)

// REG_38
#define RESERVED_ANA_15_0_SHIFT             0
#define RESERVED_ANA_15_0_MASK              (0xFFFF << RESERVED_ANA_15_0_SHIFT)
#define RESERVED_ANA_15_0(n)                BITFIELD_VAL(RESERVED_ANA_15_0, n)

// REG_39
#define RESERVED_DIG_15_0_SHIFT             0
#define RESERVED_DIG_15_0_MASK              (0xFFFF << RESERVED_DIG_15_0_SHIFT)
#define RESERVED_DIG_15_0(n)                BITFIELD_VAL(RESERVED_DIG_15_0, n)

// REG_3A
#define SOFT_MODE_SHIFT                     0
#define SOFT_MODE_MASK                      (0xFFFF << SOFT_MODE_SHIFT)
#define SOFT_MODE(n)                        BITFIELD_VAL(SOFT_MODE, n)

// REG_3B
#define EFUSE_REDUNDANCY_DATA_OUT_SHIFT     0
#define EFUSE_REDUNDANCY_DATA_OUT_MASK      (0xF << EFUSE_REDUNDANCY_DATA_OUT_SHIFT)
#define EFUSE_REDUNDANCY_DATA_OUT(n)        BITFIELD_VAL(EFUSE_REDUNDANCY_DATA_OUT, n)

// REG_3F
#define REG_SLEEP                           (1 << 1)
#define REG_POWER_ON                        (1 << 0)

// REG_43
#define EFUSE_REDUNDANCY_INFO_ROW_SEL_DR    (1 << 15)
#define EFUSE_REDUNDANCY_INFO_ROW_SEL_REG   (1 << 14)

// REG_45
#define EFUSE_CHIP_SEL_ENB_DR               (1 << 15)
#define EFUSE_CHIP_SEL_ENB_REG              (1 << 14)
#define EFUSE_STROBE_DR                     (1 << 13)
#define EFUSE_STROBE_REG                    (1 << 12)
#define EFUSE_LOAD_DR                       (1 << 11)
#define EFUSE_LOAD_REG                      (1 << 10)
#define EFUSE_PGM_ENB_DR                    (1 << 9)
#define EFUSE_PGM_ENB_REG                   (1 << 8)
#define EFUSE_PGM_SEL_DR                    (1 << 7)
#define EFUSE_PGM_SEL_REG                   (1 << 6)
#define EFUSE_READ_MODE_DR                  (1 << 5)
#define EFUSE_READ_MODE_REG                 (1 << 4)
#define EFUSE_PWR_DN_ENB_DR                 (1 << 3)
#define EFUSE_PWR_DN_ENB_REG                (1 << 2)
#define EFUSE_REDUNDANCY_EN_DR              (1 << 1)
#define EFUSE_REDUNDANCY_EN_REG             (1 << 0)

// REG_46
#define RESERVED_ANA_33                     (1 << 15)
#define REG_EFUSE_ADDRESS_SHIFT             6
#define REG_EFUSE_ADDRESS_MASK              (0x1FF << REG_EFUSE_ADDRESS_SHIFT)
#define REG_EFUSE_ADDRESS(n)                BITFIELD_VAL(REG_EFUSE_ADDRESS, n)
#define REG_EFUSE_STROBE_TRIGGER            (1 << 5)
#define REG_EFUSE_TURN_ON                   (1 << 4)
#define REG_EFUSE_CLK_EN                    (1 << 3)
#define REG_EFUSE_READ_MODE                 (1 << 2)
#define REG_EFUSE_PGM_MODE                  (1 << 1)
#define REG_EFUSE_PGM_READ_SEL              (1 << 0)

// REG_47
#define REG_EFUSE_TIME_CSB_ADDR_VALUE_SHIFT 10
#define REG_EFUSE_TIME_CSB_ADDR_VALUE_MASK  (0xF << REG_EFUSE_TIME_CSB_ADDR_VALUE_SHIFT)
#define REG_EFUSE_TIME_CSB_ADDR_VALUE(n)    BITFIELD_VAL(REG_EFUSE_TIME_CSB_ADDR_VALUE, n)
#define REG_EFUSE_TIME_PS_CSB_VALUE_SHIFT   6
#define REG_EFUSE_TIME_PS_CSB_VALUE_MASK    (0xF << REG_EFUSE_TIME_PS_CSB_VALUE_SHIFT)
#define REG_EFUSE_TIME_PS_CSB_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_PS_CSB_VALUE, n)
#define REG_EFUSE_TIME_PD_PS_VALUE_SHIFT    0
#define REG_EFUSE_TIME_PD_PS_VALUE_MASK     (0x3F << REG_EFUSE_TIME_PD_PS_VALUE_SHIFT)
#define REG_EFUSE_TIME_PD_PS_VALUE(n)       BITFIELD_VAL(REG_EFUSE_TIME_PD_PS_VALUE, n)

// REG_48
#define REG_EFUSE_TIME_PGM_STROBING_VALUE_SHIFT 4
#define REG_EFUSE_TIME_PGM_STROBING_VALUE_MASK (0x1FF << REG_EFUSE_TIME_PGM_STROBING_VALUE_SHIFT)
#define REG_EFUSE_TIME_PGM_STROBING_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_PGM_STROBING_VALUE, n)
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE_SHIFT 0
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE_MASK (0xF << REG_EFUSE_TIME_ADDR_STROBE_VALUE_SHIFT)
#define REG_EFUSE_TIME_ADDR_STROBE_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_ADDR_STROBE_VALUE, n)

// REG_49
#define REG_EFUSE_TIME_READ_STROBING_VALUE_SHIFT 0
#define REG_EFUSE_TIME_READ_STROBING_VALUE_MASK (0x1FF << REG_EFUSE_TIME_READ_STROBING_VALUE_SHIFT)
#define REG_EFUSE_TIME_READ_STROBING_VALUE(n) BITFIELD_VAL(REG_EFUSE_TIME_READ_STROBING_VALUE, n)

// REG_4A
#define REG_EFUSE_TIME_PS_PD_VALUE_SHIFT    10
#define REG_EFUSE_TIME_PS_PD_VALUE_MASK     (0x3F << REG_EFUSE_TIME_PS_PD_VALUE_SHIFT)
#define REG_EFUSE_TIME_PS_PD_VALUE(n)       BITFIELD_VAL(REG_EFUSE_TIME_PS_PD_VALUE, n)
#define REG_EFUSE_TIME_CSB_PS_VALUE_SHIFT   6
#define REG_EFUSE_TIME_CSB_PS_VALUE_MASK    (0xF << REG_EFUSE_TIME_CSB_PS_VALUE_SHIFT)
#define REG_EFUSE_TIME_CSB_PS_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_CSB_PS_VALUE, n)
#define REG_EFUSE_TIME_STROBE_CSB_VALUE_SHIFT 0
#define REG_EFUSE_TIME_STROBE_CSB_VALUE_MASK (0x3F << REG_EFUSE_TIME_STROBE_CSB_VALUE_SHIFT)
#define REG_EFUSE_TIME_STROBE_CSB_VALUE(n)  BITFIELD_VAL(REG_EFUSE_TIME_STROBE_CSB_VALUE, n)

// REG_4B
#define REG_EFUSE_TIME_PD_OFF_VALUE_SHIFT   0
#define REG_EFUSE_TIME_PD_OFF_VALUE_MASK    (0x3F << REG_EFUSE_TIME_PD_OFF_VALUE_SHIFT)
#define REG_EFUSE_TIME_PD_OFF_VALUE(n)      BITFIELD_VAL(REG_EFUSE_TIME_PD_OFF_VALUE, n)

// REG_4C
#define EFUSE_DATA_OUT_HI_SHIFT             0
#define EFUSE_DATA_OUT_HI_MASK              (0xFFFF << EFUSE_DATA_OUT_HI_SHIFT)
#define EFUSE_DATA_OUT_HI(n)                BITFIELD_VAL(EFUSE_DATA_OUT_HI, n)

// REG_4D
#define EFUSE_DATA_OUT_LO_SHIFT             0
#define EFUSE_DATA_OUT_LO_MASK              (0xFFFF << EFUSE_DATA_OUT_LO_SHIFT)
#define EFUSE_DATA_OUT_LO(n)                BITFIELD_VAL(EFUSE_DATA_OUT_LO, n)

// REG_4E
#define CLK_32K_COUNTER_26M_SHIFT           0
#define CLK_32K_COUNTER_26M_MASK            (0x7FFF << CLK_32K_COUNTER_26M_SHIFT)
#define CLK_32K_COUNTER_26M(n)              BITFIELD_VAL(CLK_32K_COUNTER_26M, n)
#define CLK_32K_COUNTER_26M_READY           (1 << 15)

// REG_4F
#define EXT_INTR_IN                         (1 << 15)
#define HARDWARE_POWER_OFF_EN               (1 << 1)
#define SOFT_POWER_OFF                      (1 << 0)

// REG_50
#define EXT_INTR                            (1 << 15)
#define RTC_INT_1                           (1 << 14)
#define RTC_INT_0                           (1 << 13)

// REG_51
#define EXT_INTR_MSKED                      (1 << 15)
#define RTC_INT1_MSKED                      (1 << 14)
#define RTC_INT0_MSKED                      (1 << 13)

#define EXT_INTR_CLR                        (1 << 15)
#define RTC_INT_CLR_1                       (1 << 14)
#define RTC_INT_CLR_0                       (1 << 13)

// REG_52
#define PMU_LDO_ON_SOURCE_SHIFT             4
#define PMU_LDO_ON_SOURCE_MASK              (0x3 << PMU_LDO_ON_SOURCE_SHIFT)
#define PMU_LDO_ON_SOURCE(n)                BITFIELD_VAL(PMU_LDO_ON_SOURCE, n)
#define RTC_LOAD                            (1 << 3)
#define WDT_LOAD                            (1 << 2)
#define POWER_ON                            (1 << 0)

// REG_53
#define REG_VTOI_POUT_IDLE_15_0_SHIFT       0
#define REG_VTOI_POUT_IDLE_15_0_MASK        (0xFFFF << REG_VTOI_POUT_IDLE_15_0_SHIFT)
#define REG_VTOI_POUT_IDLE_15_0(n)          BITFIELD_VAL(REG_VTOI_POUT_IDLE_15_0, n)

// REG_54
#define REG_VTOI_POUT_IDLE_31_16_SHIFT      0
#define REG_VTOI_POUT_IDLE_31_16_MASK       (0xFFFF << REG_VTOI_POUT_IDLE_31_16_SHIFT)
#define REG_VTOI_POUT_IDLE_31_16(n)         BITFIELD_VAL(REG_VTOI_POUT_IDLE_31_16, n)

// REG_55
#define REG_VTOI_POUT_IDLE_39_32_SHIFT      0
#define REG_VTOI_POUT_IDLE_39_32_MASK       (0xFF << REG_VTOI_POUT_IDLE_39_32_SHIFT)
#define REG_VTOI_POUT_IDLE_39_32(n)         BITFIELD_VAL(REG_VTOI_POUT_IDLE_39_32, n)

// REG_56
#define REG_VTOI_POUT_IDLE_49_40_SHIFT      0
#define REG_VTOI_POUT_IDLE_49_40_MASK       (0x3FF << REG_VTOI_POUT_IDLE_49_40_SHIFT)
#define REG_VTOI_POUT_IDLE_49_40(n)         BITFIELD_VAL(REG_VTOI_POUT_IDLE_49_40, n)

// REG_57
#define REG_VTOI_POUT_NORMAL_15_0_SHIFT     0
#define REG_VTOI_POUT_NORMAL_15_0_MASK      (0xFFFF << REG_VTOI_POUT_NORMAL_15_0_SHIFT)
#define REG_VTOI_POUT_NORMAL_15_0(n)        BITFIELD_VAL(REG_VTOI_POUT_NORMAL_15_0, n)

// REG_58
#define REG_VTOI_POUT_NORMAL_31_16_SHIFT    0
#define REG_VTOI_POUT_NORMAL_31_16_MASK     (0xFFFF << REG_VTOI_POUT_NORMAL_31_16_SHIFT)
#define REG_VTOI_POUT_NORMAL_31_16(n)       BITFIELD_VAL(REG_VTOI_POUT_NORMAL_31_16, n)

// REG_59
#define REG_VTOI_POUT_NORMAL_39_32_SHIFT    0
#define REG_VTOI_POUT_NORMAL_39_32_MASK     (0xFF << REG_VTOI_POUT_NORMAL_39_32_SHIFT)
#define REG_VTOI_POUT_NORMAL_39_32(n)       BITFIELD_VAL(REG_VTOI_POUT_NORMAL_39_32, n)

// REG_5A
#define REG_VTOI_POUT_NORMAL_49_40_SHIFT    0
#define REG_VTOI_POUT_NORMAL_49_40_MASK     (0x3FF << REG_VTOI_POUT_NORMAL_49_40_SHIFT)
#define REG_VTOI_POUT_NORMAL_49_40(n)       BITFIELD_VAL(REG_VTOI_POUT_NORMAL_49_40, n)

// REG_5B
#define REG_RTC_EXT_INTR_SEL                (1 << 9)
#define REG_VTOI_IABS_POUT_SHIFT            6
#define REG_VTOI_IABS_POUT_MASK             (0x7 << REG_VTOI_IABS_POUT_SHIFT)
#define REG_VTOI_IABS_POUT(n)               BITFIELD_VAL(REG_VTOI_IABS_POUT, n)
#define REG_VTOI_IABS_SHIFT                 1
#define REG_VTOI_IABS_MASK                  (0x1F << REG_VTOI_IABS_SHIFT)
#define REG_VTOI_IABS(n)                    BITFIELD_VAL(REG_VTOI_IABS, n)
#define REG_VTOI_IABS_EN                    (1 << 0)

// REG_5C
#define RTC_VALUE_31_16_SHIFT               0
#define RTC_VALUE_31_16_MASK                (0xFFFF << RTC_VALUE_31_16_SHIFT)
#define RTC_VALUE_31_16(n)                  BITFIELD_VAL(RTC_VALUE_31_16, n)

// REG_5D
#define RTC_VALUE_15_0_SHIFT                0
#define RTC_VALUE_15_0_MASK                 (0xFFFF << RTC_VALUE_15_0_SHIFT)
#define RTC_VALUE_15_0(n)                   BITFIELD_VAL(RTC_VALUE_15_0, n)

// REG_5E
#define POWER_ON_RELEASE                    (1 << 15)
#define POWER_ON_PRESS                      (1 << 14)
#define PU_OSC_OUT                          (1 << 13)
#define DIG_PU_VCORE1_LDO                   (1 << 12)
#define DIG_LP_EN_VCORE1_LDO                (1 << 11)
#define DIG_PU_VCORE2_LDO                   (1 << 10)
#define DIG_LP_EN_VCORE2_LDO                (1 << 9)
#define DIG_MIC_LDO_EN                      (1 << 8)
#define DIG_MIC_LP_ENABLE                   (1 << 7)
#define DIG_MIC_BIASA_EN                    (1 << 6)
#define DIG_MIC_BIASB_EN                    (1 << 5)
#define DIG_VTOI_EN                         (1 << 4)
#define DIG_VTOI_LP_EN                      (1 << 3)
#define LDO_BIAS_ENABLE                     (1 << 2)
#define LDO_LP_BIAS_ENABLE                  (1 << 1)
#define LP_MODE_RTC                         (1 << 0)

// REG_5F
#define DEEPSLEEP_MODE                      (1 << 13)
#define UVLO                                (1 << 11)
#define BG_VBG_SEL                          (1 << 4)
#define PMU_LDO_ON_BIT                      (1 << 3)
#define BG_NOTCH_EN                         (1 << 2)
#define BG_CORE_EN                          (1 << 1)// Efuse Registers

// 75
#define REG_CRYSTAL_SEL_LV                  (1 << 0)
#define REG_EXTPLL_SEL                      (1 << 1)
#define REG_AUDPLL_CAL_EN                   (1 << 2)
#define REG_AUDPLL_CP_IEN_SHIFT             3
#define REG_AUDPLL_CP_IEN_MASK              (0xF << REG_AUDPLL_CP_IEN_SHIFT)
#define REG_AUDPLL_CP_IEN(n)                BITFIELD_VAL(REG_AUDPLL_CP_IEN, n)
#define REG_AUDPLL_CP_SWRC_SHIFT            7
#define REG_AUDPLL_CP_SWRC_MASK             (0x3 << REG_AUDPLL_CP_SWRC_SHIFT)
#define REG_AUDPLL_CP_SWRC(n)               BITFIELD_VAL(REG_AUDPLL_CP_SWRC, n)
#define REG_AUDPLL_DIG_SWRC_SHIFT           9
#define REG_AUDPLL_DIG_SWRC_MASK            (0x3 << REG_AUDPLL_DIG_SWRC_SHIFT)
#define REG_AUDPLL_DIG_SWRC(n)              BITFIELD_VAL(REG_AUDPLL_DIG_SWRC, n)
#define REG_AUDPLL_VCO_SPD_SHIFT            11
#define REG_AUDPLL_VCO_SPD_MASK             (0x7 << REG_AUDPLL_VCO_SPD_SHIFT)
#define REG_AUDPLL_VCO_SPD(n)               BITFIELD_VAL(REG_AUDPLL_VCO_SPD, n)
#define REG_AUDPLL_VCO_SWRC_SHIFT           14
#define REG_AUDPLL_VCO_SWRC_MASK            (0x3 << REG_AUDPLL_VCO_SWRC_SHIFT)
#define REG_AUDPLL_VCO_SWRC(n)              BITFIELD_VAL(REG_AUDPLL_VCO_SWRC, n)

// REG_26E
#define REG_OSC_48M_PU_LDO                  (1 << 0)
#define REG_OSC_48M_EN                      (1 << 1)
#define REG_OSC_48M_EN_AGPIO_TEST           (1 << 2)
#define REG_OSC_48M_EN_CLK_DIG              (1 << 3)
#define REG_OSC_48M_EN_CLK_RX_ADC           (1 << 4)
#define REG_OSC_48M_EN_CLK_SAR_ADC          (1 << 5)
#define REG_OSC_48M_MUX_SEL_SAR_CLK_SHIFT   6
#define REG_OSC_48M_MUX_SEL_SAR_CLK_MASK    (0x7 << REG_OSC_48M_MUX_SEL_SAR_CLK_SHIFT)
#define REG_OSC_48M_MUX_SEL_SAR_CLK(n)      BITFIELD_VAL(REG_OSC_48M_MUX_SEL_SAR_CLK, n)
#define REG_OSC_48M_MUX_SEL_VAD_CLK         (1 << 9)
#define REG_OSC_48M_PWR_BIT_SHIFT           10
#define REG_OSC_48M_PWR_BIT_MASK            (0x3 << REG_OSC_48M_PWR_BIT_SHIFT)
#define REG_OSC_48M_PWR_BIT(n)              BITFIELD_VAL(REG_OSC_48M_PWR_BIT, n)
#define RESERVED_DIG_41_38_SHIFT            12
#define RESERVED_DIG_41_38_MASK             (0xF << RESERVED_DIG_41_38_SHIFT)
#define RESERVED_DIG_41_38(n)               BITFIELD_VAL(RESERVED_DIG_41_38, n)

// REG_272
#define REG_BT_XTAL_FX2_EN                  (1 << 0)
#define REG_BT_XTAL_RSTB_DR                 (1 << 1)
#define REG_BT_XTAL_RSTB                    (1 << 2)
#define REG_BT_XTAL_RCOSC_CD_SHIFT          3
#define REG_BT_XTAL_RCOSC_CD_MASK           (0x7 << REG_BT_XTAL_RCOSC_CD_SHIFT)
#define REG_BT_XTAL_RCOSC_CD(n)             BITFIELD_VAL(REG_BT_XTAL_RCOSC_CD, n)
#define REG_BT_XTAL_RCOSC_DITHER_SHIFT      6
#define REG_BT_XTAL_RCOSC_DITHER_MASK       (0x7 << REG_BT_XTAL_RCOSC_DITHER_SHIFT)
#define REG_BT_XTAL_RCOSC_DITHER(n)         BITFIELD_VAL(REG_BT_XTAL_RCOSC_DITHER, n)
#define REG_BT_XTAL_RCOSC_DITHER_FREQ_SHIFT 9
#define REG_BT_XTAL_RCOSC_DITHER_FREQ_MASK  (0x7 << REG_BT_XTAL_RCOSC_DITHER_FREQ_SHIFT)
#define REG_BT_XTAL_RCOSC_DITHER_FREQ(n)    BITFIELD_VAL(REG_BT_XTAL_RCOSC_DITHER_FREQ, n)
#define REG_BT_XTAL_LDO_VSEL_SHIFT          12
#define REG_BT_XTAL_LDO_VSEL_MASK           (0x7 << REG_BT_XTAL_LDO_VSEL_SHIFT)
#define REG_BT_XTAL_LDO_VSEL(n)             BITFIELD_VAL(REG_BT_XTAL_LDO_VSEL, n)
#define REG_CODEC_LOWF_IN                   (1 << 15)

// REG_273
#define REG_BT_XTAL_RCOSC_CS_SHIFT          0
#define REG_BT_XTAL_RCOSC_CS_MASK           (0xFF << REG_BT_XTAL_RCOSC_CS_SHIFT)
#define REG_BT_XTAL_RCOSC_CS(n)             BITFIELD_VAL(REG_BT_XTAL_RCOSC_CS, n)
#define REG_BT_XTAL_RCOSC_ICMP_SHIFT        8
#define REG_BT_XTAL_RCOSC_ICMP_MASK         (0x3 << REG_BT_XTAL_RCOSC_ICMP_SHIFT)
#define REG_BT_XTAL_RCOSC_ICMP(n)           BITFIELD_VAL(REG_BT_XTAL_RCOSC_ICMP, n)
#define REG_BT_XTAL_BUF_RC_SHIFT            10
#define REG_BT_XTAL_BUF_RC_MASK             (0xF << REG_BT_XTAL_BUF_RC_SHIFT)
#define REG_BT_XTAL_BUF_RC(n)               BITFIELD_VAL(REG_BT_XTAL_BUF_RC, n)
#define REG_BT_XTAL_MANKICK_EN              (1 << 14)
#define REG_BT_XTAL_RCOSC_TST_EN            (1 << 15)

// REG_274
#define REG_BT_XTAL_FX2_CAP1_SHIFT          0
#define REG_BT_XTAL_FX2_CAP1_MASK           (0xF << REG_BT_XTAL_FX2_CAP1_SHIFT)
#define REG_BT_XTAL_FX2_CAP1(n)             BITFIELD_VAL(REG_BT_XTAL_FX2_CAP1, n)
#define REG_BT_XTAL_FX2_CAP2_SHIFT          4
#define REG_BT_XTAL_FX2_CAP2_MASK           (0xF << REG_BT_XTAL_FX2_CAP2_SHIFT)
#define REG_BT_XTAL_FX2_CAP2(n)             BITFIELD_VAL(REG_BT_XTAL_FX2_CAP2, n)
#define REG_BT_XTAL_IBIAS_SHIFT             8
#define REG_BT_XTAL_IBIAS_MASK              (0xF << REG_BT_XTAL_IBIAS_SHIFT)
#define REG_BT_XTAL_IBIAS(n)                BITFIELD_VAL(REG_BT_XTAL_IBIAS, n)
#define REG_BT_XTAL_RCOSC_VREF_RSEL_SHIFT   12
#define REG_BT_XTAL_RCOSC_VREF_RSEL_MASK    (0xF << REG_BT_XTAL_RCOSC_VREF_RSEL_SHIFT)
#define REG_BT_XTAL_RCOSC_VREF_RSEL(n)      BITFIELD_VAL(REG_BT_XTAL_RCOSC_VREF_RSEL, n)

// REG_275
#define REG_BT_XTAL_PU_DR                   (1 << 0)
#define REG_BT_XTAL_PU                      (1 << 1)
#define REG_BT_XTAL_PU_DIV32K_DR            (1 << 2)
#define REG_BT_XTAL_PU_DIV32K               (1 << 3)
#define REG_BT_XTAL_PU_RCOSC_DR             (1 << 4)
#define REG_BT_XTAL_PU_RCOSC                (1 << 5)
#define REG_BT_XTAL_FX4_EN                  (1 << 6)
#define REG_BT_XTAL_FX4_CAP1_SHIFT          7
#define REG_BT_XTAL_FX4_CAP1_MASK           (0x3 << REG_BT_XTAL_FX4_CAP1_SHIFT)
#define REG_BT_XTAL_FX4_CAP1(n)             BITFIELD_VAL(REG_BT_XTAL_FX4_CAP1, n)
#define REG_BT_XTAL_FX4_CAP2_SHIFT          9
#define REG_BT_XTAL_FX4_CAP2_MASK           (0x3 << REG_BT_XTAL_FX4_CAP2_SHIFT)
#define REG_BT_XTAL_FX4_CAP2(n)             BITFIELD_VAL(REG_BT_XTAL_FX4_CAP2, n)
#define REG_XTAL_SEL_MODE                   (1 << 11)
#define REG_OSC_SEL_RC                      (1 << 12)
#define REG_OSC_VOUT_SEL_SHIFT              13
#define REG_OSC_VOUT_SEL_MASK               (0x7 << REG_OSC_VOUT_SEL_SHIFT)
#define REG_OSC_VOUT_SEL(n)                 BITFIELD_VAL(REG_OSC_VOUT_SEL, n)

// REG_276
#define REG_PU_BT_XTAL_DLY_SHIFT            0
#define REG_PU_BT_XTAL_DLY_MASK             (0x1F << REG_PU_BT_XTAL_DLY_SHIFT)
#define REG_PU_BT_XTAL_DLY(n)               BITFIELD_VAL(REG_PU_BT_XTAL_DLY, n)
#define REG_BT_XTAL_XOINJ_RC_SHIFT          5
#define REG_BT_XTAL_XOINJ_RC_MASK           (0xF << REG_BT_XTAL_XOINJ_RC_SHIFT)
#define REG_BT_XTAL_XOINJ_RC(n)             BITFIELD_VAL(REG_BT_XTAL_XOINJ_RC, n)
#define REG_BT_XTAL_PU_TST                  (1 << 9)
#define REG_BT_XTAL_STABLE_DR               (1 << 10)
#define REG_BT_XTAL_STABLE                  (1 << 11)
#define REG_XTAL_INJ_EN                     (1 << 12)
#define REG_CODEC_ADCE_DVDD_SEL             (1 << 13)
#define REG_CODEC_ADCE_PU_REG               (1 << 14)
#define CFG_OSC_SEL_RC_DR                   (1 << 15)

// REG_277
#define CFG_CLK26M_DIG_RSTB_DR              (1 << 0)
#define REG_CLK26M_DIG_RSTB                 (1 << 1)
#define REG_BT_BBPLL_FREF_SEL               (1 << 2)
#define REG_BT_BBPLL_LDO_CP_SHIFT           3
#define REG_BT_BBPLL_LDO_CP_MASK            (0x7 << REG_BT_BBPLL_LDO_CP_SHIFT)
#define REG_BT_BBPLL_LDO_CP(n)              BITFIELD_VAL(REG_BT_BBPLL_LDO_CP, n)
#define REG_BT_BBPLL_LDO_DIG_SHIFT          6
#define REG_BT_BBPLL_LDO_DIG_MASK           (0x7 << REG_BT_BBPLL_LDO_DIG_SHIFT)
#define REG_BT_BBPLL_LDO_DIG(n)             BITFIELD_VAL(REG_BT_BBPLL_LDO_DIG, n)
#define REG_BT_BBPLL_LDO_VCO_SHIFT          9
#define REG_BT_BBPLL_LDO_VCO_MASK           (0x7 << REG_BT_BBPLL_LDO_VCO_SHIFT)
#define REG_BT_BBPLL_LDO_VCO(n)             BITFIELD_VAL(REG_BT_BBPLL_LDO_VCO, n)
#define REG_BT_BBPLL_EN_CLK_BBADC           (1 << 12)
#define REG_BT_BBPLL_EN_CLK_CODEC           (1 << 13)
#define REG_BT_BBPLL_EN_CLK_DIG             (1 << 14)
#define REG_BT_BBPLL_EN_CLK_NFMI            (1 << 15)

enum PMU_REG_T {
    PMU_REG_METAL_ID            = ITNPMU_REG(0x00),
    PMU_REG_POWER_KEY_CFG       = ITNPMU_REG(0x02),
    PMU_REG_VBG_CFG             = ITNPMU_REG(0x03),
    PMU_REG_DIG1_CFG            = ITNPMU_REG(0x08),
    PMU_REG_DIG2_CFG            = ITNPMU_REG(0x09),
    PMU_REG_MIC_LDO_EN          = ITNPMU_REG(0x0A),
    PMU_REG_MIC_BIAS_EN         = ITNPMU_REG(0x0B),
    PMU_REG_VTOI_EN             = ITNPMU_REG(0x0C),
    PMU_REG_PWR_SEL             = ITNPMU_REG(0x1A),
    PMU_REG_SLEEP_CFG           = ITNPMU_REG(0x1D),
    PMU_REG_INT_MASK            = ITNPMU_REG(0x1F),
    PMU_REG_INT_EN              = ITNPMU_REG(0x20),
    PMU_REG_RTC_DIV_1HZ         = ITNPMU_REG(0x22),
    PMU_REG_RTC_LOAD_LOW        = ITNPMU_REG(0x23),
    PMU_REG_RTC_LOAD_HIGH       = ITNPMU_REG(0x24),
    PMU_REG_RTC_MATCH1_LOW      = ITNPMU_REG(0x27),
    PMU_REG_RTC_MATCH1_HIGH     = ITNPMU_REG(0x28),
    PMU_REG_MIC_BIAS_A          = ITNPMU_REG(0x29),
    PMU_REG_MIC_BIAS_B          = ITNPMU_REG(0x2A),
    PMU_REG_MIC_LDO_RES         = ITNPMU_REG(0x30),
    PMU_REG_WDT_TIMER           = ITNPMU_REG(0x36),
    PMU_REG_WDT_CFG             = ITNPMU_REG(0x37),
    PMU_REG_EFUSE_CTRL          = ITNPMU_REG(0x46),
    PMU_REG_EFUSE_DATA_HIGH     = ITNPMU_REG(0x4C),
    PMU_REG_EFUSE_DATA_LOW      = ITNPMU_REG(0x4D),
    PMU_REG_POWER_OFF           = ITNPMU_REG(0x4F),
    PMU_REG_INT_STATUS          = ITNPMU_REG(0x50),
    PMU_REG_INT_MSKED_STATUS    = ITNPMU_REG(0x51),
    PMU_REG_INT_CLR             = ITNPMU_REG(0x51),
    PMU_REG_RTC_VAL_HIGH        = ITNPMU_REG(0x5C),
    PMU_REG_RTC_VAL_LOW         = ITNPMU_REG(0x5D),

    PMU_REG_ANA_60              = NORMAL_REG(0x60),
    PMU_REG_ANA_75              = NORMAL_REG(0x75),
    PMU_REG_ANA_26E             = NORMAL_REG(0x26E),
    PMU_REG_ANA_271             = NORMAL_REG(0x271),
    PMU_REG_ANA_272             = NORMAL_REG(0x272),
    PMU_REG_ANA_273             = NORMAL_REG(0x273),
    PMU_REG_ANA_274             = NORMAL_REG(0x274),
    PMU_REG_ANA_275             = NORMAL_REG(0x275),
    PMU_REG_ANA_276             = NORMAL_REG(0x276),
    PMU_REG_ANA_277             = NORMAL_REG(0x277),
};

enum PMU_VCORE_REQ_T {
    PMU_VCORE_FLASH_WRITE_ENABLED   = (1 << 0),
    PMU_VCORE_FLASH_FREQ_HIGH       = (1 << 1),
    PMU_VCORE_PSRAM_FREQ_HIGH       = (1 << 2),
    PMU_VCORE_USB_HS_ENABLED        = (1 << 3),
    PMU_VCORE_RS_FREQ_HIGH          = (1 << 4),
    PMU_VCORE_SYS_FREQ_MEDIUM       = (1 << 5),
    PMU_VCORE_SYS_FREQ_HIGH         = (1 << 6),
};

union BOOT_SETTINGS_T {
    struct {
        unsigned short usb_dld_dis      :1;
        unsigned short uart_dld_en      :1;
        unsigned short uart_trace_en    :1;
        unsigned short pll_dis          :1;
        unsigned short uart_baud_div2   :1;
        unsigned short sec_freq_div2    :1;
        unsigned short crystal_freq     :2;
        unsigned short reserved         :4;
        unsigned short chksum           :4;
    };
    unsigned short reg;
};

enum PMU_MODUAL_T {
    PMU_DIG1,
    PMU_DIG2,
};

#ifdef NO_EXT_PMU

static enum HAL_CHIP_METAL_ID_T BOOT_BSS_LOC pmu_metal_id;

static enum PMU_VCORE_REQ_T BOOT_BSS_LOC pmu_vcore_req;

static PMU_IRQ_UNIFIED_HANDLER_T pmu_irq_hdlrs[PMU_IRQ_TYPE_QTY];

static const uint8_t dig_lp_ldo = PMU_VDIG_0_8V;

static uint16_t wdt_timer;

#if defined(MCU_HIGH_PERFORMANCE_MODE)
static const uint16_t high_perf_freq_mhz =
#if defined(MTEST_ENABLED) && defined(MTEST_CLK_MHZ)
    MTEST_CLK_MHZ;
#else
    300;
#endif
static bool high_perf_on;
#endif

#ifdef RTC_ENABLE
struct PMU_RTC_CTX_T {
    bool enabled;
    bool alarm_set;
    uint32_t alarm_val;
};

static struct PMU_RTC_CTX_T BOOT_BSS_LOC rtc_ctx;

static PMU_RTC_IRQ_HANDLER_T rtc_irq_handler;

static void BOOT_TEXT_SRAM_LOC pmu_rtc_save_context(void)
{
    if (pmu_rtc_enabled()) {
        rtc_ctx.enabled = true;
        if (pmu_rtc_alarm_status_set()) {
            rtc_ctx.alarm_set = true;
            rtc_ctx.alarm_val = pmu_rtc_get_alarm();
        }
    } else {
        rtc_ctx.enabled = false;
    }
}

static void pmu_rtc_restore_context(void)
{
    uint32_t rtc_val;

    if (rtc_ctx.enabled) {
        pmu_rtc_enable();
        if (rtc_ctx.alarm_set) {
            rtc_val = pmu_rtc_get();
            if (rtc_val - rtc_ctx.alarm_val <= 1 || rtc_ctx.alarm_val - rtc_val < 5) {
                rtc_ctx.alarm_val = rtc_val + 5;
            }
            pmu_rtc_set_alarm(rtc_ctx.alarm_val);
        }
    }
}
#endif

uint32_t BOOT_TEXT_FLASH_LOC read_hw_metal_id(void)
{
    uint16_t val;
    uint32_t metal_id;

#ifdef RTC_ENABLE
    // RTC will be restored in pmu_open()
    pmu_rtc_save_context();
#endif

    // Reset PMU (to recover from a possible insane state, e.g., ESD reset)
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

    pmu_rf_ana_init();

    pmu_read(PMU_REG_METAL_ID, &val);
    pmu_metal_id = GET_BITFIELD(val, REVID);

    metal_id = hal_cmu_get_aon_revision_id();

    return metal_id;
}

#endif // NO_EXT_PMU

int BOOT_TEXT_SRAM_LOC pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
    int ret;
    unsigned short val;
    unsigned short tmp[2];

#ifdef NO_EXT_PMU
    //hal_cmu_pmu_fast_clock_enable();
#endif

    // Enable CLK_EN
    val = REG_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Enable TURN_ON
    val |= REG_EFUSE_TURN_ON;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Write Address
#ifdef PMU_EFUSE_NO_REDUNDANCY
    val |= REG_EFUSE_ADDRESS(page / 2);
#else
    val |= REG_EFUSE_ADDRESS(page); //redundancy
#endif
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Set Strobe Trigger = 1
    val |= REG_EFUSE_STROBE_TRIGGER;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // set Strobe Trigger = 0
    val &= ~REG_EFUSE_STROBE_TRIGGER;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        goto _exit;
    }

    // Read Efuse High 16 bits
    ret = pmu_read(PMU_REG_EFUSE_DATA_LOW, &tmp[0]);
    if (ret) {
        goto _exit;
    }

    // Read Efuse Low 16 bits
    ret = pmu_read(PMU_REG_EFUSE_DATA_HIGH, &tmp[1]);
    if (ret) {
        goto _exit;
    }
#ifdef PMU_EFUSE_NO_REDUNDANCY
    *efuse = tmp[page % 2];
#else
    *efuse = (tmp[0] | tmp[1]); //redundancy
#endif

_exit:
    // Disable TURN_ON
    val &= ~(REG_EFUSE_TURN_ON | REG_EFUSE_ADDRESS_MASK);
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

    // Disable CLK_EN
    val &= ~REG_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);

#ifdef NO_EXT_PMU
    //hal_cmu_pmu_fast_clock_disable();
#endif

    return ret;
}

void BOOT_TEXT_FLASH_LOC pmu_itn_init(void)
{
    uint16_t val;

    // Reset PMU (to recover from a possible insane state, e.g., ESD reset)
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

#ifdef NO_EXT_PMU
    // Set vcore voltage to lowest
    pmu_read(PMU_REG_DIG1_CFG, &val);
    val = (val & ~(LDO_VCORE1_VBIT_DSLEEP_MASK | LDO_VCORE1_VBIT_NORMAL_MASK)) |
        LDO_VCORE1_VBIT_DSLEEP(0) | LDO_VCORE1_VBIT_NORMAL(0);
    pmu_write(PMU_REG_DIG1_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Force LP mode
    pmu_read(PMU_REG_VBG_CFG, &val);
    val = (val & ~BG_VBG_SEL_REG) | BG_VBG_SEL_DR;
    pmu_write(PMU_REG_VBG_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    val = (val & ~(PU_BIAS_LDO_REG | BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_VTOI_EN_REG | BG_NOTCH_EN_REG)) |
        PU_BIAS_LDO_DR | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_VTOI_EN_DR | BG_NOTCH_EN_DR;
    pmu_write(PMU_REG_VBG_CFG, val);

    // Allow sleep
    pmu_read(PMU_REG_SLEEP_CFG, &val);
    val |= SLEEP_ALLOW;
    pmu_write(PMU_REG_SLEEP_CFG, val);
#else
    // Close MIC LDO 0x0A
    pmu_read(PMU_REG_MIC_LDO_EN, &val);
    val = (val & ~(REG_PU_MIC_LDO_REG)) | REG_PU_MIC_LDO_DR;
    pmu_write(PMU_REG_MIC_LDO_EN, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Close MIC BIAS 0x0B
    pmu_read(PMU_REG_MIC_BIAS_EN, &val);
    val = (val & ~(REG_PU_MIC_BIASA_REG | REG_PU_MIC_BIASB_REG)) | REG_PU_MIC_BIASA_DR | REG_PU_MIC_BIASB_DR;
    pmu_write(PMU_REG_MIC_BIAS_EN, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Close Vtoi 0x0C
    pmu_read(PMU_REG_VTOI_EN, &val);
    val = (val & ~(REG_PU_VTOI_REG)) | REG_PU_VTOI_DR;
    pmu_write(PMU_REG_VTOI_EN, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Set vcore voltage to lowest
    pmu_read(PMU_REG_DIG1_CFG, &val);
    val = (val & ~(LDO_VCORE1_VBIT_DSLEEP_MASK | LDO_VCORE1_VBIT_NORMAL_MASK | REG_PU_VCORE1_LDO_REG)) | REG_PU_VCORE1_LDO_DR |
        LDO_VCORE1_VBIT_DSLEEP(20) | LDO_VCORE1_VBIT_NORMAL(20);
    pmu_write(PMU_REG_DIG1_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Force LP mode
    pmu_read(PMU_REG_VBG_CFG, &val);
    val = (val & ~BG_VBG_SEL_REG) | BG_VBG_SEL_DR;
    pmu_write(PMU_REG_VBG_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    val = (val & ~(PU_LP_BIAS_LDO_REG | PU_BIAS_LDO_REG | BG_VBG_SEL_DR | BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_VTOI_EN_REG | BG_NOTCH_EN_REG)) |
        PU_LP_BIAS_LDO_DR | PU_BIAS_LDO_DR | BG_VBG_SEL_REG | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_VTOI_EN_DR | BG_NOTCH_EN_DR;

    pmu_write(PMU_REG_VBG_CFG, val);

    pmu_read(PMU_REG_PWR_SEL, &val);
    val = (val & ~REG_PWR_SEL) | REG_PWR_SEL_DR;
    pmu_write(PMU_REG_PWR_SEL, val);

    // Allow sleep
    pmu_read(PMU_REG_SLEEP_CFG, &val);
    val |= SLEEP_ALLOW;
    pmu_write(PMU_REG_SLEEP_CFG, val);
#endif
}

#ifdef NO_EXT_PMU

static void pmu_sys_ctrl(bool shutdown)
{
    uint16_t val;
    uint32_t lock = int_lock();

    PMU_INFO_TRACE_IMM(0, "Start pmu %s", shutdown ? "shutdown" : "reboot");

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    // Default vcore might not be high enough to support high performance mode
    pmu_high_performance_mode_enable(false);
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
#endif
#endif

#ifdef RTC_ENABLE
    pmu_rtc_save_context();
#endif

    // Reset PMU
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(4);

#ifdef RTC_ENABLE
    pmu_rtc_restore_context();
#endif

    if (shutdown) {
        // Power off
        pmu_read(PMU_REG_POWER_OFF,&val);
        val |= SOFT_POWER_OFF;
        for (int i = 0; i < 100; i++) {
            pmu_write(PMU_REG_POWER_OFF,val);
            hal_sys_timer_delay(MS_TO_TICKS(5));
        }

        hal_sys_timer_delay(MS_TO_TICKS(50));

        //can't reach here
        PMU_INFO_TRACE_IMM(0, "\nError: pmu shutdown failed!\n");
        hal_sys_timer_delay(MS_TO_TICKS(5));
    } else {
#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
        // CAUTION:
        // 1) Never reset RF because system or flash might be using X2/X4, which are off by default
        // 2) Never reset RF/ANA because system or flash might be using PLL, and the reset might cause clock glitch
        // TODO:
        // Restore BBPLL settings in RF
#endif
    }

    hal_cmu_sys_reboot();

    int_unlock(lock);
}

void pmu_shutdown(void)
{
    pmu_sys_ctrl(true);
}

void pmu_reboot(void)
{
    pmu_sys_ctrl(false);
}

void pmu_module_config(enum PMU_MODUAL_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode)
{
    unsigned short val;
    unsigned short module_address;

    if (module == PMU_DIG1) {
        module_address = PMU_REG_DIG1_CFG;
    } else {
        module_address = PMU_REG_DIG2_CFG;
    }

    pmu_read(module_address, &val);
    if (is_manual) {
        val |= REG_PU_VCORE1_LDO_DR;
    } else {
        val &= ~REG_PU_VCORE1_LDO_DR;
    }
    if (ldo_on) {
        val |= REG_PU_VCORE1_LDO_REG;
    } else {
        val &= ~REG_PU_VCORE1_LDO_REG;
    }
    if (lp_mode) {
        val &= ~LP_EN_VCORE1_LDO_DR;
    } else {
        val = (val & ~LP_EN_VCORE1_LDO_REG) | LP_EN_VCORE1_LDO_DR;
    }
    if (dpmode) {
        val |= LP_EN_VCORE1_LDO_DSLEEP_EN;
    } else {
        val &= ~LP_EN_VCORE1_LDO_DSLEEP_EN;
    }
    pmu_write(module_address, val);
}

void pmu_module_set_volt(unsigned char module, unsigned short sleep_v,unsigned short normal_v)
{
    unsigned short val;
    unsigned short module_address;

    if (module == PMU_DIG1) {
        module_address = PMU_REG_DIG1_CFG;
    } else {
        module_address = PMU_REG_DIG2_CFG;
    }

    pmu_read(module_address, &val);
    val = (val & ~(LDO_VCORE1_VBIT_DSLEEP_MASK | LDO_VCORE1_VBIT_NORMAL_MASK)) |
        LDO_VCORE1_VBIT_DSLEEP(sleep_v) | LDO_VCORE1_VBIT_NORMAL(normal_v);
    pmu_write(module_address, val);
}

int pmu_module_get_volt(unsigned char module, unsigned short *sleep_vp,unsigned short *normal_vp)
{
    unsigned short val;
    unsigned short module_address;

    if (module == PMU_DIG1) {
        module_address = PMU_REG_DIG1_CFG;
    } else {
        module_address = PMU_REG_DIG2_CFG;
    }

    pmu_read(module_address, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, LDO_VCORE1_VBIT_NORMAL);
    }
    if (sleep_vp) {
        *sleep_vp = GET_BITFIELD(val, LDO_VCORE1_VBIT_DSLEEP);
    }

    return 0;
}

static void pmu_module_ramp_volt(unsigned char module, unsigned short sleep_v, unsigned short normal_v)
{
    uint16_t old_normal_v;
    uint16_t old_sleep_v;

    pmu_module_get_volt(module, &old_sleep_v, &old_normal_v);

    if (old_normal_v < normal_v) {
        while (old_normal_v++ < normal_v) {
            pmu_module_set_volt(module, sleep_v, old_normal_v);
        }
    } else if (old_normal_v != normal_v || old_sleep_v != sleep_v) {
        pmu_module_set_volt(module, sleep_v, normal_v);
    }
}

static void BOOT_TEXT_SRAM_LOC pmu_dig_get_target_volt(uint16_t *ldo)
{
    uint16_t ldo_volt;

    if (0) {
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    } else if (pmu_vcore_req & (PMU_VCORE_SYS_FREQ_HIGH)) {
        if (high_perf_freq_mhz <= 300) {
            ldo_volt = PMU_VDIG_1_1V; //PMU_VDIG_1_05V;
        } else if (high_perf_freq_mhz <= 350) {
            ldo_volt = PMU_VDIG_1_1V;
        }
#endif
    } else if (pmu_vcore_req & (PMU_VCORE_USB_HS_ENABLED | PMU_VCORE_RS_FREQ_HIGH | PMU_VCORE_SYS_FREQ_MEDIUM)) {
        ldo_volt = PMU_VDIG_0_9V;
    } else if (pmu_vcore_req & (PMU_VCORE_FLASH_FREQ_HIGH | PMU_VCORE_PSRAM_FREQ_HIGH |
            PMU_VCORE_FLASH_WRITE_ENABLED)) {
        ldo_volt = PMU_VDIG_0_8V;
    } else {
        // Common cases
        ldo_volt = PMU_VDIG_0_8V;
    }

#if defined(MTEST_ENABLED) && defined(MTEST_VOLT)
    ldo_volt  = MTEST_VOLT;
#endif

    if (ldo) {
        *ldo = ldo_volt;
    }
}

static void pmu_dig_set_volt(void)
{
    uint32_t lock;
    uint16_t ldo_volt, old_act_ldo, old_lp_ldo;
    bool volt_inc = false;

    lock = int_lock();

    pmu_dig_get_target_volt(&ldo_volt);

    pmu_module_get_volt(PMU_DIG1, &old_lp_ldo, &old_act_ldo);

    if (old_act_ldo < ldo_volt) {
        volt_inc = true;
    }
    pmu_module_ramp_volt(PMU_DIG1, dig_lp_ldo, ldo_volt);
    if (volt_inc) {
        hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
    }

    int_unlock(lock);
}

void pmu_mode_change(enum PMU_POWER_MODE_T mode)
{
    pmu_module_config(PMU_DIG1,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    pmu_dig_set_volt();
    pmu_module_config(PMU_DIG2,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
}

void pmu_sleep_en(unsigned char sleep_en)
{
    unsigned short val;

    pmu_read(PMU_REG_SLEEP_CFG, &val);
    if (sleep_en) {
        val |= SLEEP_ALLOW;
    } else {
        val &= ~SLEEP_ALLOW;
    }
    pmu_write(PMU_REG_SLEEP_CFG, val);
}

int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))

    uint16_t val;

    // Disable and clear all PMU irqs by default
    pmu_write(PMU_REG_INT_MASK, 0);
    pmu_write(PMU_REG_INT_EN, 0);
    // PMU irqs cannot be cleared by PMU soft reset
    pmu_read(PMU_REG_INT_STATUS, &val);
    pmu_write(PMU_REG_INT_CLR, val);

#ifndef NO_SLEEP
    pmu_sleep_en(true);
#endif

    pmu_codec_mic_bias_enable(0);

    pmu_mode_change(PMU_POWER_MODE_LDO);

#ifdef FORCE_LP_MODE
    pmu_read(PMU_REG_DIG1_CFG, &val);
    val |= LP_EN_VCORE1_LDO_DR | LP_EN_VCORE1_LDO_REG;
    pmu_write(PMU_REG_DIG1_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    pmu_read(PMU_REG_VBG_CFG, &val);
    val = (val & ~BG_VBG_SEL_REG) | BG_VBG_SEL_DR;
    pmu_write(PMU_REG_VBG_CFG, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    val = (val & ~(PU_BIAS_LDO_REG | BG_CONSTANT_GM_BIAS_REG | BG_CORE_EN_REG | BG_VTOI_EN_REG | BG_NOTCH_EN_REG)) |
        PU_BIAS_LDO_DR | BG_CONSTANT_GM_BIAS_DR | BG_CORE_EN_DR | BG_VTOI_EN_DR | BG_NOTCH_EN_DR;
    pmu_write(PMU_REG_VBG_CFG, val);
#endif

#ifdef RTC_ENABLE
    pmu_rtc_restore_context();
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE)
    analog_read(PMU_REG_ANA_277, &val);
    val = (val & ~(REG_BT_BBPLL_LDO_CP_MASK | REG_BT_BBPLL_LDO_DIG_MASK | REG_BT_BBPLL_LDO_VCO_MASK)) |
        REG_BT_BBPLL_LDO_CP(7) | REG_BT_BBPLL_LDO_DIG(7) | REG_BT_BBPLL_LDO_VCO(7);
    analog_write(PMU_REG_ANA_277, val);
    analog_read(PMU_REG_ANA_75, &val);
    val = (val & ~(REG_AUDPLL_VCO_SPD_MASK | REG_AUDPLL_VCO_SWRC_MASK)) |
        REG_AUDPLL_VCO_SPD(7) | REG_AUDPLL_VCO_SWRC(3);
    analog_write(PMU_REG_ANA_75, val);

    pmu_high_performance_mode_enable(true);
#endif

#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

    return 0;
}

void pmu_sleep(void)
{
}

void pmu_wakeup(void)
{
}

void pmu_codec_mic_bias_enable(uint32_t map)
{
    uint16_t val;

    pmu_read(PMU_REG_MIC_BIAS_A, &val);
    if (map & AUD_VMIC_MAP_VMIC1) {
        val |= REG_MIC_BIASA_EN;
    } else {
        val &= ~REG_MIC_BIASA_EN;
    }
    pmu_write(PMU_REG_MIC_BIAS_A, val);

    pmu_read(PMU_REG_MIC_BIAS_B, &val);
    if (map & AUD_VMIC_MAP_VMIC2) {
        val |= REG_MIC_BIASB_EN;
    } else {
        val &= ~REG_MIC_BIASB_EN;
    }
    if (map & (AUD_VMIC_MAP_VMIC1 | AUD_VMIC_MAP_VMIC2)) {
        val |= REG_MIC_LDO_EN;
    } else {
        val &= ~REG_MIC_LDO_EN;
    }
    pmu_write(PMU_REG_MIC_BIAS_B, val);

    pmu_read(PMU_REG_MIC_BIAS_EN, &val);
    val |= REG_PU_MIC_BIASA_DR | REG_PU_MIC_BIASB_DR;
    if (map & AUD_VMIC_MAP_VMIC1) {
        val |= REG_PU_MIC_BIASA_REG;
    } else {
        val &= ~REG_PU_MIC_BIASA_REG;
    }
    if (map & AUD_VMIC_MAP_VMIC2) {
        val |= REG_PU_MIC_BIASB_REG;
    } else {
        val &= ~REG_PU_MIC_BIASB_REG;
    }
    pmu_write(PMU_REG_MIC_BIAS_EN, val);

    pmu_read(PMU_REG_MIC_LDO_EN, &val);
    val |= REG_PU_MIC_LDO_DR;
    if (map & (AUD_VMIC_MAP_VMIC1 | AUD_VMIC_MAP_VMIC2)) {
        val |= REG_PU_MIC_LDO_REG;
    } else {
        val &= ~REG_PU_MIC_LDO_REG;
    }
    pmu_write(PMU_REG_MIC_LDO_EN, val);
}

void pmu_codec_mic_bias_lowpower_mode(uint32_t map)
{
}

void pmu_flash_write_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if (pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req |= PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt();
#endif
}

void pmu_flash_read_config(void)
{
#ifdef FLASH_WRITE_AT_HIGH_VCORE
    uint32_t lock;

    if ((pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) == 0) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req &= ~PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt();
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_flash_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;

    lock = int_lock();
    if (freq > 52000000) {
        // The real max freq is 120M
        //pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_FLASH_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt();
#endif
}

void pmu_anc_config(int enable)
{
}

void pmu_fir_high_speed_config(int enable)
{
}

void pmu_iir_freq_config(uint32_t freq)
{
}

void pmu_rs_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 60000000) {
        pmu_vcore_req |= PMU_VCORE_RS_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_RS_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt();
}

void BOOT_TEXT_SRAM_LOC pmu_sys_freq_config(enum HAL_CMU_FREQ_T freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#if defined(MCU_HIGH_PERFORMANCE_MODE) || defined(ULTRA_LOW_POWER) || !defined(OSC_26M_X4_AUD2BB)
    uint32_t lock;
    enum PMU_VCORE_REQ_T old_req;
    bool update = false;

    lock = int_lock();
    old_req = pmu_vcore_req;
    pmu_vcore_req &= ~(PMU_VCORE_SYS_FREQ_HIGH | PMU_VCORE_SYS_FREQ_MEDIUM);
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    if (freq > HAL_CMU_FREQ_104M) {
        if (high_perf_on) {
            // The real freq is 350M
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_HIGH;
        } else {
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
        }
    } else {
#ifndef OSC_26M_X4_AUD2BB
        if (freq == HAL_CMU_FREQ_104M) {
            // The real freq is 200M
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
        }
#endif
    }
#else
    if (freq > HAL_CMU_FREQ_104M) {
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
    }
#endif
    if (old_req != pmu_vcore_req) {
        update = true;
    }
    int_unlock(lock);

    if (!update) {
        // Nothing changes
        return;
    }

    pmu_dig_set_volt();
#endif
#endif
}

void pmu_high_performance_mode_enable(bool enable)
{
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    if (high_perf_on == enable) {
        return;
    }
    high_perf_on = enable;

    if (!enable) {
        if (high_perf_freq_mhz > 300) {
#ifdef NO_X2_X4
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
#else
            // Switch to 52M to avoid using PLL
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
#endif
            // Restore the default div
            analog_aud_pll_set_dig_div(2);
            // Restore the sys freq
            hal_cmu_sys_set_freq(hal_sysfreq_get_hw_freq());
        }
        // Restore the default PLL freq (393M)
        analog_aud_freq_pll_config(CODEC_FREQ_24P576M, 16);
    }

    pmu_sys_freq_config(hal_sysfreq_get_hw_freq());

    if (enable) {
        uint32_t pll_freq;

        // Change freq first, and then change divider.
        // Otherwise there will be an instant very high freq sent to digital domain.

        if (high_perf_freq_mhz <= 300) {
            pll_freq = high_perf_freq_mhz * 1000000 * 2;
        } else {
            pll_freq = high_perf_freq_mhz * 1000000;
        }
        analog_aud_freq_pll_config(pll_freq / 16, 16);

        if (high_perf_freq_mhz > 300) {
#ifdef NO_X2_X4
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
#else
            // Switch to 52M to avoid using PLL
            hal_cmu_sys_set_freq(HAL_CMU_FREQ_52M);
#endif
            // Change the dig div
            analog_aud_pll_set_dig_div(1);
            // Restore the sys freq
            hal_cmu_sys_set_freq(hal_sysfreq_get_hw_freq());
        }
    }
#endif
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{
}

#ifdef RTC_ENABLE
void pmu_rtc_enable(void)
{
    uint16_t readval;
    uint32_t lock;

#ifdef SIMU
    // Set RTC counter to 1KHz
    pmu_write(PMU_REG_RTC_DIV_1HZ, 32 - 2);
#else
    // Set RTC counter to 1Hz
    pmu_write(PMU_REG_RTC_DIV_1HZ, CONFIG_SYSTICK_HZ * 2 - 2);
#endif

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval |= RTC_POWER_ON_EN | PU_LPO_DR | PU_LPO_REG;
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

void pmu_rtc_disable(void)
{
    uint16_t readval;
    uint32_t lock;

    pmu_rtc_clear_alarm();

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval &= ~(RTC_POWER_ON_EN | PU_LPO_DR);
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_enabled(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);

    return !!(readval & RTC_POWER_ON_EN);
}

void pmu_rtc_set(uint32_t seconds)
{
    uint16_t high, low;

    // Need 3 seconds to load a new value
    seconds += 3;

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_RTC_LOAD_LOW, low);
    pmu_write(PMU_REG_RTC_LOAD_HIGH, high);
}

uint32_t pmu_rtc_get(void)
{
    uint16_t high, low, high2;

    pmu_read(PMU_REG_RTC_VAL_HIGH, &high);
    pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    // Handle counter wrap
    pmu_read(PMU_REG_RTC_VAL_HIGH, &high2);
    if (high != high2) {
        high = high2;
        pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    }

    return (high << 16) | low;
}

void pmu_rtc_set_alarm(uint32_t seconds)
{
    uint16_t readval;
    uint16_t high, low;
    uint32_t lock;

    // Need 1 second to raise the interrupt
    if (seconds > 0) {
        seconds -= 1;
    }

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_INT_CLR, RTC_INT1_MSKED);

    pmu_write(PMU_REG_RTC_MATCH1_LOW, low);
    pmu_write(PMU_REG_RTC_MATCH1_HIGH, high);

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval |= RTC_INT_EN_1;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);
}

uint32_t BOOT_TEXT_SRAM_LOC pmu_rtc_get_alarm(void)
{
    uint16_t high, low;

    pmu_read(PMU_REG_RTC_MATCH1_LOW, &low);
    pmu_read(PMU_REG_RTC_MATCH1_HIGH, &high);

    // Compensate the alarm offset
    return (uint32_t)((high << 16) | low) + 1;
}

void pmu_rtc_clear_alarm(void)
{
    uint16_t readval;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval &= ~RTC_INT_EN_1;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);

    pmu_write(PMU_REG_INT_CLR, RTC_INT1_MSKED);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_alarm_status_set(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_EN, &readval);

    return !!(readval & RTC_INT_EN_1);
}

int pmu_rtc_alarm_alerted()
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_STATUS, &readval);

    return !!(readval & RTC_INT_1);
}

static void pmu_rtc_irq_handler(uint16_t irq_status)
{
    uint32_t seconds;

    if (irq_status & RTC_INT1_MSKED) {
        pmu_rtc_clear_alarm();

        if (rtc_irq_handler) {
            seconds = pmu_rtc_get();
            rtc_irq_handler(seconds);
        }
    }
}

void pmu_rtc_set_irq_handler(PMU_RTC_IRQ_HANDLER_T handler)
{
    uint16_t readval;
    uint32_t lock;

    rtc_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_INT_MASK, &readval);
    if (handler) {
        readval |= RTC_INT1_MSK;
    } else {
        readval &= ~RTC_INT1_MSK;
    }
    pmu_write(PMU_REG_INT_MASK, readval);
    pmu_set_irq_unified_handler(PMU_IRQ_TYPE_RTC, handler ? pmu_rtc_irq_handler : NULL);
    int_unlock(lock);
}
#endif

static void pmu_general_irq_handler(void)
{
    uint32_t lock;
    uint16_t val;
    bool rtc;

    rtc = false;

    lock = int_lock();
    pmu_read(PMU_REG_INT_MSKED_STATUS, &val);
    if (val & (RTC_INT1_MSKED | RTC_INT0_MSKED)) {
        rtc = true;
    }
    if (rtc) {
        pmu_write(PMU_REG_INT_CLR, val);
    }
    int_unlock(lock);

    if (rtc) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC](val);
        }
    }
}

int pmu_set_irq_unified_handler(enum PMU_IRQ_TYPE_T type, PMU_IRQ_UNIFIED_HANDLER_T hdlr)
{
    bool update;
    uint32_t lock;
    int i;

    if (type >= PMU_IRQ_TYPE_QTY) {
        return 1;
    }

    update = false;

    lock = int_lock();

    for (i = 0; i < PMU_IRQ_TYPE_QTY; i++) {
        if (pmu_irq_hdlrs[i]) {
            break;
        }
    }

    pmu_irq_hdlrs[type] = hdlr;

    if (hdlr) {
        update = (i >= PMU_IRQ_TYPE_QTY);
    } else {
        if (i == type) {
            for (; i < PMU_IRQ_TYPE_QTY; i++) {
                if (pmu_irq_hdlrs[i]) {
                    break;
                }
            }
            update = (i >= PMU_IRQ_TYPE_QTY);
        }
    }

    if (update) {
        if (hdlr) {
            NVIC_SetVector(PMU_IRQn, (uint32_t)pmu_general_irq_handler);
            NVIC_SetPriority(PMU_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_ClearPendingIRQ(PMU_IRQn);
            NVIC_EnableIRQ(PMU_IRQn);
        } else {
            NVIC_DisableIRQ(PMU_IRQn);
        }
    }

    int_unlock(lock);

    return 0;
}

int pmu_debug_config_ana(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_codec(uint16_t volt)
{
#ifdef ANC_PROD_TEST
    if (volt == 1600 || volt == 1700 || volt == 1800 || volt == 1900 || volt == 1950) {
        vhppa_mv = vcodec_mv = volt;
        vcodec_off = true;
    } else {
        vcodec_off = false;
        return 1;
    }
#endif
    return 0;
}

int pmu_debug_config_vcrystal(bool on)
{
    return 0;
}

int pmu_debug_config_audio_output(bool diff)
{
    return 0;
}

void pmu_debug_reliability_test(int stage)
{
}

int pmu_wdt_config(uint32_t irq_ms, uint32_t reset_ms)
{
    // No wdt irq on best3008
    if (irq_ms + reset_ms > 0xFFFF) {
        return 1;
    }
    wdt_timer = irq_ms + reset_ms;

    pmu_write(PMU_REG_WDT_TIMER, wdt_timer);

    return 0;
}

void pmu_wdt_start(void)
{
    uint16_t val;

    if (wdt_timer == 0) {
        return;
    }

    pmu_read(PMU_REG_WDT_CFG, &val);
    val |= (REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);
}

void pmu_wdt_stop(void)
{
    uint16_t val;

    if (wdt_timer == 0) {
        return;
    }

    pmu_read(PMU_REG_WDT_CFG, &val);
    val &= ~(REG_WDT_RESET_EN | REG_WDT_EN);
    pmu_write(PMU_REG_WDT_CFG, val);
}

void pmu_wdt_feed(void)
{
    if (wdt_timer == 0) {
        return;
    }

    pmu_write(PMU_REG_WDT_TIMER, wdt_timer);
}

#endif

