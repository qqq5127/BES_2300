#ifndef _REG_PMU_BEST3003_H_
#define _REG_PMU_BEST3003_H_


// REG_00
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

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
#define CLK_32K_SEL                         (1 << 8)
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
#define PU_LP_BIAS_LDO_DSLEEP               (1 << 14)
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

// REG_05
#define REG_PU_LDO_VRTC_RF_DSLEEP           (1 << 15)
#define REG_PU_LDO_VRTC_RF_DR               (1 << 14)
#define REG_PU_LDO_VRTC_RF_REG              (1 << 13)
#define REG_CHARGE_OUT_INTR_MSK             (1 << 12)
#define REG_CHARGE_IN_INTR_MSK              (1 << 11)
#define REG_AC_ON_OUT_EN                    (1 << 10)
#define REG_AC_ON_IN_EN                     (1 << 9)
#define REG_CHARGE_INTR_EN                  (1 << 8)
#define REG_AC_ON_DB_VALUE_SHIFT            0
#define REG_AC_ON_DB_VALUE_MASK             (0xFF << REG_AC_ON_DB_VALUE_SHIFT)
#define REG_AC_ON_DB_VALUE(n)               BITFIELD_VAL(REG_AC_ON_DB_VALUE, n)

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
#define REG_LIGHT_LOAD_VDCDC_LDO            (1 << 15)
#define REG_PULLDOWN_VANA_LDO               (1 << 14)
#define LP_EN_ANA_LDO_DSLEEP               (1 << 13)
#define LP_EN_ANA_LDO_DR                   (1 << 12)
#define LP_EN_ANA_LDO_REG                  (1 << 11)
#define PU_LDO_ANA_DSLEEP              (1 << 10)
#define PU_LDO_ANA_DR                  (1 << 9)
#define PU_LDO_ANA_REG                 (1 << 8)
#define LDO_ANA_VBIT_DSLEEP_SHIFT           4
#define LDO_ANA_VBIT_DSLEEP_MASK            (0xF << 4)
#define LDO_ANA_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_ANA_VBIT_DSLEEP, n)
#define LDO_ANA_VBIT_NORMAL_SHIFT           0
#define LDO_ANA_VBIT_NORMAL_MASK            (0xF << 0)
#define LDO_ANA_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_ANA_VBIT_NORMAL, n)

// REG_08
#define REG_LOOP_CTL_VCORE_LDO              (1 << 15)
#define REG_PULLDOWN_VCORE                  (1 << 14)
#define LP_EN_DIG_LDO_DSLEEP              (1 << 13)
#define LP_EN_DIG_LDO_DR                  (1 << 12)
#define LP_EN_DIG_LDO_REG                 (1 << 11)
#define PU_LDO_DIG_DSLEEP               (1 << 10)
#define PU_LDO_DIG_DR                   (1 << 9)
#define PU_LDO_DIG_REG                  (1 << 8)
#define LDO_DIG_VBIT_DSLEEP_SHIFT           4
#define LDO_DIG_VBIT_DSLEEP_MASK            (0xF << LDO_DIG_VBIT_DSLEEP_SHIFT)
#define LDO_DIG_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_DIG_VBIT_DSLEEP, n)
#define LDO_DIG_VBIT_NORMAL_SHIFT           0
#define LDO_DIG_VBIT_NORMAL_MASK            (0xF << LDO_DIG_VBIT_NORMAL_SHIFT)
#define LDO_DIG_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_DIG_VBIT_NORMAL, n)

// REG_09
#define LP_EN_IO_LDO_DSLEEP                (1 << 15)
#define LP_EN_IO_LDO_DR                    (1 << 14)
#define LP_EN_IO_LDO_REG                   (1 << 13)
#define PU_LDO_IO_DSLEEP               (1 << 12)
#define PU_LDO_IO_DR                   (1 << 11)
#define PU_LDO_IO_REG                  (1 << 10)
#define LDO_IO_VBIT_NORMAL_SHIFT           5
#define LDO_IO_VBIT_NORMAL_MASK            (0x1F << LDO_IO_VBIT_NORMAL_SHIFT)
#define LDO_IO_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_IO_VBIT_NORMAL, n)
#define LDO_IO_VBIT_DSLEEP_SHIFT           0
#define LDO_IO_VBIT_DSLEEP_MASK            (0x1F << LDO_IO_VBIT_DSLEEP_SHIFT)
#define LDO_IO_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_IO_VBIT_DSLEEP, n)

// REG_0B
#define REG_PULLDOWN_VIO                    (1 << 15)
#define REG_PULLDOWN_VUSB                   (1 << 14)
#define LP_EN_USB_LDO_DSLEEP               (1 << 13)
#define LP_EN_USB_LDO_DR                   (1 << 12)
#define LP_EN_USB_LDO_REG                  (1 << 11)
#define PU_LDO_USB_DSLEEP                  (1 << 10)
#define PU_LDO_USB_DR                      (1 << 9)
#define PU_LDO_USB_REG                     (1 << 8)
#define LDO_USB_VBIT_NORMAL_SHIFT          4
#define LDO_USB_VBIT_NORMAL_MASK           (0xF << LDO_USB_VBIT_NORMAL_SHIFT)
#define LDO_USB_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_USB_VBIT_NORMAL, n)
#define LDO_USB_VBIT_DSLEEP_SHIFT          0
#define LDO_USB_VBIT_DSLEEP_MASK           (0xF << LDO_USB_VBIT_DSLEEP_SHIFT)
#define LDO_USB_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_USB_VBIT_DSLEEP, n)

// REG_0C
#define REG_VCORE_SSTIME_MODE_SHIFT         14
#define REG_VCORE_SSTIME_MODE_MASK          (0x3 << REG_VCORE_SSTIME_MODE_SHIFT)
#define REG_VCORE_SSTIME_MODE(n)            BITFIELD_VAL(REG_VCORE_SSTIME_MODE, n)
#define PU_LDO_CRYSTAL_DSLEEP              (1 << 13)
#define PU_LDO_CRYSTAL_DR                  (1 << 12)
#define PU_LDO_CRYSTAL_REG                 (1 << 11)
#define LP_EN_CRYSTAL_LDO_DSLEEP           (1 << 10)
#define LP_EN_CRYSTAL_LDO_DR               (1 << 9)
#define LP_EN_CRYSTAL_LDO_REG              (1 << 8)
#define REG_EXT_CAP_VCRYSTAL                (1 << 7)
#define REG_PULLDOWN_VCRYSTAL               (1 << 6)
#define LDO_CRYSTAL_VBIT_NORMAL_SHIFT      3
#define LDO_CRYSTAL_VBIT_NORMAL_MASK       (0x7 << LDO_CRYSTAL_VBIT_NORMAL_SHIFT)
#define LDO_CRYSTAL_VBIT_NORMAL(n)         BITFIELD_VAL(LDO_CRYSTAL_VBIT_NORMAL, n)
#define LDO_CRYSTAL_VBIT_DSLEEP_SHIFT      0
#define LDO_CRYSTAL_VBIT_DSLEEP_MASK       (0x7 << LDO_CRYSTAL_VBIT_DSLEEP_SHIFT)
#define LDO_CRYSTAL_VBIT_DSLEEP(n)         BITFIELD_VAL(LDO_CRYSTAL_VBIT_DSLEEP, n)

// REG_0D
#define REG_BUCK_CC_CAP_BIT_SHIFT           12
#define REG_BUCK_CC_CAP_BIT_MASK            (0xF << REG_BUCK_CC_CAP_BIT_SHIFT)
#define REG_BUCK_CC_CAP_BIT(n)              BITFIELD_VAL(REG_BUCK_CC_CAP_BIT, n)
#define REG_BUCK_CC_ILIMIT_SHIFT            8
#define REG_BUCK_CC_ILIMIT_MASK             (0xF << REG_BUCK_CC_ILIMIT_SHIFT)
#define REG_BUCK_CC_ILIMIT(n)               BITFIELD_VAL(REG_BUCK_CC_ILIMIT, n)
#define REG_LP_BIAS_SEL_LDO_SHIFT           6
#define REG_LP_BIAS_SEL_LDO_MASK            (0x3 << REG_LP_BIAS_SEL_LDO_SHIFT)
#define REG_LP_BIAS_SEL_LDO(n)              BITFIELD_VAL(REG_LP_BIAS_SEL_LDO, n)
#define REG_LP_EN_VUSB11                    (1 << 5)
#define REG_BUCK_IS_GAIN_DSLEEP_SHIFT       3
#define REG_BUCK_IS_GAIN_DSLEEP_MASK        (0x3 << REG_BUCK_IS_GAIN_DSLEEP_SHIFT)
#define REG_BUCK_IS_GAIN_DSLEEP(n)          BITFIELD_VAL(REG_BUCK_IS_GAIN_DSLEEP, n)
#define REG_PU_AVDD25_ANA                   (1 << 1)
#define REG_BYPASS_VCORE                    (1 << 0)

// REG_0E
#define PU_LDO_CODEC_DSLEEP                (1 << 15)
#define PU_LDO_CODEC_DR                    (1 << 14)
#define PU_LDO_CODEC_REG                   (1 << 13)
#define LP_EN_CODEC_LDO_DSLEEP             (1 << 12)
#define LP_EN_CODEC_LDO_DR                 (1 << 11)
#define LP_EN_CODEC_LDO_REG                (1 << 10)
#define LDO_CODEC_VBIT_NORMAL_SHIFT        5
#define LDO_CODEC_VBIT_NORMAL_MASK         (0x1F << LDO_CODEC_VBIT_NORMAL_SHIFT)
#define LDO_CODEC_VBIT_NORMAL(n)           BITFIELD_VAL(LDO_CODEC_VBIT_NORMAL, n)
#define LDO_CODEC_VBIT_DSLEEP_SHIFT        0
#define LDO_CODEC_VBIT_DSLEEP_MASK         (0x1F << LDO_CODEC_VBIT_DSLEEP_SHIFT)
#define LDO_CODEC_VBIT_DSLEEP(n)           BITFIELD_VAL(LDO_CODEC_VBIT_DSLEEP, n)

// REG_0F
#define RESERVED_ANA_22                     (1 << 15)
#define REG_PULLDOWN_VCODEC                 (1 << 14)
#define REG_UVLO_SEL_SHIFT                  12
#define REG_UVLO_SEL_MASK                   (0x3 << REG_UVLO_SEL_SHIFT)
#define REG_UVLO_SEL(n)                     BITFIELD_VAL(REG_UVLO_SEL, n)
#define POWER_UP_SOFT_CNT_SHIFT             8
#define POWER_UP_SOFT_CNT_MASK              (0xF << POWER_UP_SOFT_CNT_SHIFT)
#define POWER_UP_SOFT_CNT(n)                BITFIELD_VAL(POWER_UP_SOFT_CNT, n)
#define POWER_UP_BIAS_CNT_SHIFT             4
#define POWER_UP_BIAS_CNT_MASK              (0xF << POWER_UP_BIAS_CNT_SHIFT)
#define POWER_UP_BIAS_CNT(n)                BITFIELD_VAL(POWER_UP_BIAS_CNT, n)
#define RESETN_MOD2_CNT_SHIFT               0
#define RESETN_MOD2_CNT_MASK                (0xF << RESETN_MOD2_CNT_SHIFT)
#define RESETN_MOD2_CNT(n)                  BITFIELD_VAL(RESETN_MOD2_CNT, n)

// REG_10
#define POWER_UP_MOD2_CNT_SHIFT             8
#define POWER_UP_MOD2_CNT_MASK              (0xFF << POWER_UP_MOD2_CNT_SHIFT)
#define POWER_UP_MOD2_CNT(n)                BITFIELD_VAL(POWER_UP_MOD2_CNT, n)
#define POWER_UP_MOD1_CNT_SHIFT             0
#define POWER_UP_MOD1_CNT_MASK              (0xFF << POWER_UP_MOD1_CNT_SHIFT)
#define POWER_UP_MOD1_CNT(n)                BITFIELD_VAL(POWER_UP_MOD1_CNT, n)

// REG_11
#define POWER_UP_MOD4_CNT_SHIFT             8
#define POWER_UP_MOD4_CNT_MASK              (0xFF << POWER_UP_MOD4_CNT_SHIFT)
#define POWER_UP_MOD4_CNT(n)                BITFIELD_VAL(POWER_UP_MOD4_CNT, n)
#define POWER_UP_MOD3_CNT_SHIFT             0
#define POWER_UP_MOD3_CNT_MASK              (0xFF << POWER_UP_MOD3_CNT_SHIFT)
#define POWER_UP_MOD3_CNT(n)                BITFIELD_VAL(POWER_UP_MOD3_CNT, n)

// REG_12
#define POWER_UP_MOD6_CNT_SHIFT             8
#define POWER_UP_MOD6_CNT_MASK              (0xFF << POWER_UP_MOD6_CNT_SHIFT)
#define POWER_UP_MOD6_CNT(n)                BITFIELD_VAL(POWER_UP_MOD6_CNT, n)
#define POWER_UP_MOD5_CNT_SHIFT             0
#define POWER_UP_MOD5_CNT_MASK              (0xFF << POWER_UP_MOD5_CNT_SHIFT)
#define POWER_UP_MOD5_CNT(n)                BITFIELD_VAL(POWER_UP_MOD5_CNT, n)

// REG_13
#define BUCK_VCORE_BIT_NORMAL_SHIFT         8
#define BUCK_VCORE_BIT_NORMAL_MASK          (0xFF << BUCK_VCORE_BIT_NORMAL_SHIFT)
#define BUCK_VCORE_BIT_NORMAL(n)            BITFIELD_VAL(BUCK_VCORE_BIT_NORMAL, n)
#define BUCK_VCORE_BIT_DSLEEP_SHIFT         0
#define BUCK_VCORE_BIT_DSLEEP_MASK          (0xFF << BUCK_VCORE_BIT_DSLEEP_SHIFT)
#define BUCK_VCORE_BIT_DSLEEP(n)            BITFIELD_VAL(BUCK_VCORE_BIT_DSLEEP, n)

// REG_14
#define TEST_MODE_SHIFT                     13
#define TEST_MODE_MASK                      (0x7 << TEST_MODE_SHIFT)
#define TEST_MODE(n)                        BITFIELD_VAL(TEST_MODE, n)
#define REG_BUCK_CC_MODE                    (1 << 12)
#define DCDC_ANA_LP_EN_DSLEEP               (1 << 11)
#define PU_DCDC_ANA_DR                      (1 << 10)
#define PU_DCDC_ANA_REG                     (1 << 9)
#define DCDC_ANA_LP_EN_DR                   (1 << 8)
#define DCDC_ANA_LP_EN_REG                  (1 << 7)
#define PU_DCDC_ANA_DSLEEP                  (1 << 6)
#define PU_DCDC_DIG_DR                      (1 << 4)
#define PU_DCDC_DIG_REG                     (1 << 3)
#define PU_DCDC_DIG_DSLEEP                  (1 << 0)

// REG_15
#define REG_BUCK_PDRV_BIT_SHIFT             14
#define REG_BUCK_PDRV_BIT_MASK              (0x3 << REG_BUCK_PDRV_BIT_SHIFT)
#define REG_BUCK_PDRV_BIT(n)                BITFIELD_VAL(REG_BUCK_PDRV_BIT, n)
#define REG_BUCK_IS_GAIN_NORMAL_SHIFT       12
#define REG_BUCK_IS_GAIN_NORMAL_MASK        (0x3 << REG_BUCK_IS_GAIN_NORMAL_SHIFT)
#define REG_BUCK_IS_GAIN_NORMAL(n)          BITFIELD_VAL(REG_BUCK_IS_GAIN_NORMAL, n)
#define REG_BUCK_INTERNAL_FREQUENCY_SHIFT   10
#define REG_BUCK_INTERNAL_FREQUENCY_MASK    (0x3 << REG_BUCK_INTERNAL_FREQUENCY_SHIFT)
#define REG_BUCK_INTERNAL_FREQUENCY(n)      BITFIELD_VAL(REG_BUCK_INTERNAL_FREQUENCY, n)
#define REG_BUCK_LOW_VOLTAGE                (1 << 9)
#define REG_BUCK_DT_BIT                     (1 << 8)
#define REG_BUCK_PULLDOWN_EN                (1 << 7)
#define REG_BUCK_ANTI_RES_DISABLE           (1 << 6)
#define REG_BUCK_BURST_THRESHOLD_SHIFT      4
#define REG_BUCK_BURST_THRESHOLD_MASK       (0x3 << REG_BUCK_BURST_THRESHOLD_SHIFT)
#define REG_BUCK_BURST_THRESHOLD(n)         BITFIELD_VAL(REG_BUCK_BURST_THRESHOLD, n)
#define REG_BUCK_SYNC_DISABLE               (1 << 3)
#define REG_BUCK_SOFT_START_EN              (1 << 2)
#define REG_BUCK_RECOVER                    (1 << 1)
#define REG_BUCK_SLOPE_DOUBLE               (1 << 0)

// REG_16
#define REG_BUCK_CC_SENSE_BIT_SHIFT         14
#define REG_BUCK_CC_SENSE_BIT_MASK          (0x3 << REG_BUCK_CC_SENSE_BIT_SHIFT)
#define REG_BUCK_CC_SENSE_BIT(n)            BITFIELD_VAL(REG_BUCK_CC_SENSE_BIT, n)
#define REG_BUCK_CC_SLOPE_BIT_SHIFT         12
#define REG_BUCK_CC_SLOPE_BIT_MASK          (0x3 << REG_BUCK_CC_SLOPE_BIT_SHIFT)
#define REG_BUCK_CC_SLOPE_BIT(n)            BITFIELD_VAL(REG_BUCK_CC_SLOPE_BIT, n)
#define REG_BUCK_I2V_BIT2_SHIFT             9
#define REG_BUCK_I2V_BIT2_MASK              (0x7 << REG_BUCK_I2V_BIT2_SHIFT)
#define REG_BUCK_I2V_BIT2(n)                BITFIELD_VAL(REG_BUCK_I2V_BIT2, n)
#define REG_BUCK_I2V_BIT_SHIFT              6
#define REG_BUCK_I2V_BIT_MASK               (0x7 << REG_BUCK_I2V_BIT_SHIFT)
#define REG_BUCK_I2V_BIT(n)                 BITFIELD_VAL(REG_BUCK_I2V_BIT, n)
#define REG_BUCK_V2I_BIT1_SHIFT             3
#define REG_BUCK_V2I_BIT1_MASK              (0x7 << REG_BUCK_V2I_BIT1_SHIFT)
#define REG_BUCK_V2I_BIT1(n)                BITFIELD_VAL(REG_BUCK_V2I_BIT1, n)
#define REG_BUCK_V2I_BIT2_SHIFT             0
#define REG_BUCK_V2I_BIT2_MASK              (0x7 << REG_BUCK_V2I_BIT2_SHIFT)
#define REG_BUCK_V2I_BIT2(n)                BITFIELD_VAL(REG_BUCK_V2I_BIT2, n)

// REG_17
#define REG_SS_VCORE_EN                     (1 << 15)
#define RESERVED_ANA_23                     (1 << 14)
#define REG_GPADC_RESETN_DR                 (1 << 11)
#define REG_GPADC_RESETN                    (1 << 10)
#define SAR_PWR_BIT_SHIFT                   8
#define SAR_PWR_BIT_MASK                    (0x3 << SAR_PWR_BIT_SHIFT)
#define SAR_PWR_BIT(n)                      BITFIELD_VAL(SAR_PWR_BIT, n)
#define SAR_OP_IBIT_SHIFT                   5
#define SAR_OP_IBIT_MASK                    (0x7 << SAR_OP_IBIT_SHIFT)
#define SAR_OP_IBIT(n)                      BITFIELD_VAL(SAR_OP_IBIT, n)
#define SAR_THERM_GAIN_SHIFT                3
#define SAR_THERM_GAIN_MASK                 (0x3 << SAR_THERM_GAIN_SHIFT)
#define SAR_THERM_GAIN(n)                   BITFIELD_VAL(SAR_THERM_GAIN, n)
#define SAR_VREF_BIT_SHIFT                  0
#define SAR_VREF_BIT_MASK                   (0x7 << SAR_VREF_BIT_SHIFT)
#define SAR_VREF_BIT(n)                     BITFIELD_VAL(SAR_VREF_BIT, n)

// REG_18
#define SAR_MODE_SEL                        (1 << 15)
#define KEY_DB_DSB                          (1 << 14)
#define KEY_INTERVAL_MODE                   (1 << 13)
#define GPADC_INTERVAL_MODE                 (1 << 12)
#define SAR_ADC_MODE                        (1 << 10)
#define DELAY_BEFORE_SAMP_SHIFT             8
#define DELAY_BEFORE_SAMP_MASK              (0x3 << DELAY_BEFORE_SAMP_SHIFT)
#define DELAY_BEFORE_SAMP(n)                BITFIELD_VAL(DELAY_BEFORE_SAMP, n)
#define CONV_CLK_INV                        (1 << 7)
#define SAMP_CLK_INV                        (1 << 6)
#define TIME_SAMP_NEG                       (1 << 5)
#define TIME_SAMP_POS                       (1 << 4)
#define SAR_OUT_POLARITY                    (1 << 3)
#define TIMER_SAR_STABLE_SEL_SHIFT          0
#define TIMER_SAR_STABLE_SEL_MASK           (0x7 << TIMER_SAR_STABLE_SEL_SHIFT)
#define TIMER_SAR_STABLE_SEL(n)             BITFIELD_VAL(TIMER_SAR_STABLE_SEL, n)

// REG_19
#define REG_CLK_GPADC_DIV_SHIFT             11
#define REG_CLK_GPADC_DIV_MASK              (0x1F << REG_CLK_GPADC_DIV_SHIFT)
#define REG_CLK_GPADC_DIV(n)                BITFIELD_VAL(REG_CLK_GPADC_DIV, n)
#define REG_KEY_IN_DR                       (1 << 10)
#define REG_KEY_IN_REG                      (1 << 9)
#define REG_GPADC_EN_DR                     (1 << 8)
#define REG_GPADC_EN_REG_SHIFT              0
#define REG_GPADC_EN_REG_MASK               (0xFF << REG_GPADC_EN_REG_SHIFT)
#define REG_GPADC_EN_REG(n)                 BITFIELD_VAL(REG_GPADC_EN_REG, n)

// REG_1A
#define REG_PMU_VSEL1_SHIFT                 13
#define REG_PMU_VSEL1_MASK                  (0x7 << REG_PMU_VSEL1_SHIFT)
#define REG_PMU_VSEL1(n)                    BITFIELD_VAL(REG_PMU_VSEL1, n)
#define REG_POWER_SEL_CNT_SHIFT             8
#define REG_POWER_SEL_CNT_MASK              (0x1F << REG_POWER_SEL_CNT_SHIFT)
#define REG_POWER_SEL_CNT(n)                BITFIELD_VAL(REG_POWER_SEL_CNT, n)
#define REG_PWR_SEL                         (1 << 6)

// REG_1B
#define KEY_INTERVAL_SHIFT                  0
#define KEY_INTERVAL_MASK                   (0xFFFF << KEY_INTERVAL_SHIFT)
#define KEY_INTERVAL(n)                     BITFIELD_VAL(KEY_INTERVAL, n)

// REG_1C
#define GPADC_INTERVAL_SHIFT                0
#define GPADC_INTERVAL_MASK                 (0xFFFF << GPADC_INTERVAL_SHIFT)
#define GPADC_INTERVAL(n)                   BITFIELD_VAL(GPADC_INTERVAL, n)

// REG_1D
//#define CHIP_ADDR_I2C_SHIFT                 8
//#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
//#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define SLEEP_ALLOW                         (1 << 15)

// REG_1E
#define RESERVED_ANA_24                     (1 << 15)
#define REG_SAR_REF_MODE_SEL                (1 << 14)
#define REG_CLK_GPADC_EN                    (1 << 13)
#define REG_DR_PU_SAR                       (1 << 12)
#define REG_PU_SAR                          (1 << 11)
#define REG_DR_TSC_SAR_BIT                  (1 << 10)
#define REG_TSC_SAR_BIT_SHIFT               0
#define REG_TSC_SAR_BIT_MASK                (0x3FF << REG_TSC_SAR_BIT_SHIFT)
#define REG_TSC_SAR_BIT(n)                  BITFIELD_VAL(REG_TSC_SAR_BIT, n)

// REG_1F
#define USB_INSERT_INTR_MSK                 (1 << 13)
#define RTC_INT1_MSK                        (1 << 12)
#define RTC_INT0_MSK                        (1 << 11)
#define KEY_ERR1_INTR_MSK                   (1 << 10)
#define KEY_ERR0_INTR_MSK                   (1 << 9)
#define KEY_PRESS_INTR_MSK                  (1 << 8)
#define KEY_RELEASE_INTR_MSK                (1 << 7)
#define USB_INSERT_INTR_EN                  (1 << 6)
#define RTC_INT_EN_1                        (1 << 5)
#define RTC_INT_EN_0                        (1 << 4)
#define KEY_ERR1_INTR_EN                    (1 << 3)
#define KEY_ERR0_INTR_EN                    (1 << 2)
#define KEY_PRESS_INTR_EN                   (1 << 1)
#define KEY_RELEASE_INTR_EN                 (1 << 0)

// REG_20
#define RESERVED_ANA_26_25_SHIFT            14
#define RESERVED_ANA_26_25_MASK             (0x3 << RESERVED_ANA_26_25_SHIFT)
#define RESERVED_ANA_26_25(n)               BITFIELD_VAL(RESERVED_ANA_26_25, n)
#define GPADC_CHAN_DATA_INTR_EN_SHIFT       6
#define GPADC_CHAN_DATA_INTR_EN_MASK        (0xFF << GPADC_CHAN_DATA_INTR_EN_SHIFT)
#define GPADC_CHAN_DATA_INTR_EN(n)          BITFIELD_VAL(GPADC_CHAN_DATA_INTR_EN, n)
#define USB_POL_USB_RX_DP                   (1 << 5)
#define USB_POL_USB_RX_DM                   (1 << 4)
#define REG_EN_USBDIGPHY_CLK                (1 << 3)
#define RESETN_USBPHY_DR                    (1 << 2)
#define RESETN_USBPHY_REG                   (1 << 1)
#define REG_VCORE_RAMP_EN                   (1 << 0)

// REG_21
#define REG_MIC_BIASA_CHANSEL_SHIFT         13
#define REG_MIC_BIASA_CHANSEL_MASK          (0x3 << REG_MIC_BIASA_CHANSEL_SHIFT)
#define REG_MIC_BIASA_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASA_CHANSEL, n)
#define REG_MIC_BIASA_ENLPF                 (1 << 12)
#define REG_MIC_BIASA_VSEL_SHIFT            5
#define REG_MIC_BIASA_VSEL_MASK             (0x1F << REG_MIC_BIASA_VSEL_SHIFT)
#define REG_MIC_BIASA_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASA_VSEL, n)
#define REG_MIC_LDO_RES_SHIFT               1
#define REG_MIC_LDO_RES_MASK                (0xF << REG_MIC_LDO_RES_SHIFT)
#define REG_MIC_LDO_RES(n)                  BITFIELD_VAL(REG_MIC_LDO_RES, n)
#define REG_MIC_LDO_LOOPCTRL                (1 << 0)

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
#define REG_MIC_BIASB_CHANSEL_SHIFT         12
#define REG_MIC_BIASB_CHANSEL_MASK          (0x3 << REG_MIC_BIASB_CHANSEL_SHIFT)
#define REG_MIC_BIASB_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASB_CHANSEL, n)
#define REG_MIC_BIASB_EN                    (1 << 11)
#define REG_MIC_BIASB_ENLPF                 (1 << 10)
#define REG_MIC_BIASB_VSEL_SHIFT            5
#define REG_MIC_BIASB_VSEL_MASK             (0x1F << REG_MIC_BIASB_VSEL_SHIFT)
#define REG_MIC_BIASB_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASB_VSEL, n)
#define USB_DEBOUNCE_EN                     (1 << 2)
#define USB_NOLS_MODE                       (1 << 1)
#define USB_INSERT_DET_EN                   (1 << 0)

// REG_2A
#define REG_LED_IO1_IBIT_SHIFT              14
#define REG_LED_IO1_IBIT_MASK               (0x3 << REG_LED_IO1_IBIT_SHIFT)
#define REG_LED_IO1_IBIT(n)                 BITFIELD_VAL(REG_LED_IO1_IBIT, n)
#define REG_LED_IO1_OENB                    (1 << 13)
#define REG_LED_IO1_PDEN                    (1 << 12)
#define REG_LED_IO1_PU                      (1 << 11)
#define REG_LED_IO1_PUEN                    (1 << 10)
#define REG_LED_IO1_SEL_SHIFT               8
#define REG_LED_IO1_SEL_MASK                (0x3 << REG_LED_IO1_SEL_SHIFT)
#define REG_LED_IO1_SEL(n)                  BITFIELD_VAL(REG_LED_IO1_SEL, n)
#define REG_LED_IO1_RX_EN                   (1 << 7)
#define REG_LED_IO1_AIO_EN                  (1 << 6)
#define RESERVED_ANA_27                     (1 << 5)
#define VBATDET_RES_SEL_SHIFT               0
#define VBATDET_RES_SEL_MASK                (0x1F << VBATDET_RES_SEL_SHIFT)
#define VBATDET_RES_SEL(n)                  BITFIELD_VAL(VBATDET_RES_SEL, n)

// REG_2B
#define VBATDET_LP_MODE                     (1 << 15)
#define PU_VBATDET                          (1 << 14)
#define RESERVED_ANA_28                     (1 << 13)
#define PU_LDO_USB11_DSLEEP                (1 << 12)
#define PU_LDO_USB11_DR                    (1 << 11)
#define PU_LDO_USB11_REG                   (1 << 10)
#define REG_BYPASS_VUSB11                   (1 << 9)
#define REG_PULLDOWN_VUSB11                 (1 << 8)
#define LDO_USB11_VBIT_NORMAL_SHIFT        4
#define LDO_USB11_VBIT_NORMAL_MASK         (0xF << LDO_USB11_VBIT_NORMAL_SHIFT)
#define LDO_USB11_VBIT_NORMAL(n)           BITFIELD_VAL(LDO_USB11_VBIT_NORMAL, n)
#define LDO_USB11_VBIT_DSLEEP_SHIFT        0
#define LDO_USB11_VBIT_DSLEEP_MASK         (0xF << LDO_USB11_VBIT_DSLEEP_SHIFT)
#define LDO_USB11_VBIT_DSLEEP(n)           BITFIELD_VAL(LDO_USB11_VBIT_DSLEEP, n)

// REG_2C
#define R_PWM2_TOGGLE_SHIFT                 0
#define R_PWM2_TOGGLE_MASK                  (0xFFFF << R_PWM2_TOGGLE_SHIFT)
#define R_PWM2_TOGGLE(n)                    BITFIELD_VAL(R_PWM2_TOGGLE, n)

// REG_2D
#define REG_PWM2_ST1_SHIFT                  0
#define REG_PWM2_ST1_MASK                   (0xFFFF << REG_PWM2_ST1_SHIFT)
#define REG_PWM2_ST1(n)                     BITFIELD_VAL(REG_PWM2_ST1, n)

// REG_2E
#define SUBCNT_DATA2_SHIFT                  8
#define SUBCNT_DATA2_MASK                   (0xFF << SUBCNT_DATA2_SHIFT)
#define SUBCNT_DATA2(n)                     BITFIELD_VAL(SUBCNT_DATA2, n)
#define TG_SUBCNT_D2_ST_SHIFT               1
#define TG_SUBCNT_D2_ST_MASK                (0x7F << TG_SUBCNT_D2_ST_SHIFT)
#define TG_SUBCNT_D2_ST(n)                  BITFIELD_VAL(TG_SUBCNT_D2_ST, n)
#define REG_LED0_OUT                        (1 << 0)

// REG_2F
#define RESERVED_ANA_29                     (1 << 15)
#define RESETN_DB_NUMBER_SHIFT              11
#define RESETN_DB_NUMBER_MASK               (0xF << RESETN_DB_NUMBER_SHIFT)
#define RESETN_DB_NUMBER(n)                 BITFIELD_VAL(RESETN_DB_NUMBER, n)
#define RESETN_DB_EN                        (1 << 10)
#define REG_PWM_CLK_EN                      (1 << 9)
#define REG_CLK_PWM_DIV_SHIFT               4
#define REG_CLK_PWM_DIV_MASK                (0x1F << REG_CLK_PWM_DIV_SHIFT)
#define REG_CLK_PWM_DIV(n)                  BITFIELD_VAL(REG_CLK_PWM_DIV, n)
#define REG_PWM2_BR_EN                      (1 << 3)
#define PWM_SELECT_EN                       (1 << 2)
#define PWM_SELECT_INV                      (1 << 1)
#define LED_GPIO_SELECT                     (1 << 0)

// REG_30
#define REG_EFUSE_WRITE_COUNTER_SHIFT       8
#define REG_EFUSE_WRITE_COUNTER_MASK        (0xFF << REG_EFUSE_WRITE_COUNTER_SHIFT)
#define REG_EFUSE_WRITE_COUNTER(n)          BITFIELD_VAL(REG_EFUSE_WRITE_COUNTER, n)
#define REG_EFUSE_READ_COUNTER_SHIFT        4
#define REG_EFUSE_READ_COUNTER_MASK         (0xF << REG_EFUSE_READ_COUNTER_SHIFT)
#define REG_EFUSE_READ_COUNTER(n)           BITFIELD_VAL(REG_EFUSE_READ_COUNTER, n)
#define REG_EFUSE_PROGRAM_MODE_EN           (1 << 3)
#define REG_EFUSE_PU_AVDD                   (1 << 2)
#define REG_EFUSE_READ_MODE_EN              (1 << 1)
#define REG_EFUSE_CLK_EN                    (1 << 0)

// REG_31
#define REG_EFUSE_PGMEN_OFF_COUNTER_SHIFT   12
#define REG_EFUSE_PGMEN_OFF_COUNTER_MASK    (0xF << REG_EFUSE_PGMEN_OFF_COUNTER_SHIFT)
#define REG_EFUSE_PGMEN_OFF_COUNTER(n)      BITFIELD_VAL(REG_EFUSE_PGMEN_OFF_COUNTER, n)
#define REG_EFUSE_PGMEN_ON_COUNTER_SHIFT    8
#define REG_EFUSE_PGMEN_ON_COUNTER_MASK     (0xF << REG_EFUSE_PGMEN_ON_COUNTER_SHIFT)
#define REG_EFUSE_PGMEN_ON_COUNTER(n)       BITFIELD_VAL(REG_EFUSE_PGMEN_ON_COUNTER, n)
#define REG_EFUSE_ADDRESS_SHIFT             0
#define REG_EFUSE_ADDRESS_MASK              (0xFF << REG_EFUSE_ADDRESS_SHIFT)
#define REG_EFUSE_ADDRESS(n)                BITFIELD_VAL(REG_EFUSE_ADDRESS, n)

// REG_32
#define REG_EFUSE_AVDD_ON_COUNTER_SHIFT     7
#define REG_EFUSE_AVDD_ON_COUNTER_MASK      (0x1FF << REG_EFUSE_AVDD_ON_COUNTER_SHIFT)
#define REG_EFUSE_AVDD_ON_COUNTER(n)        BITFIELD_VAL(REG_EFUSE_AVDD_ON_COUNTER, n)
#define REG_EFUSE_AVDD_OFF_COUNTER_SHIFT    0
#define REG_EFUSE_AVDD_OFF_COUNTER_MASK     (0x7F << REG_EFUSE_AVDD_OFF_COUNTER_SHIFT)
#define REG_EFUSE_AVDD_OFF_COUNTER(n)       BITFIELD_VAL(REG_EFUSE_AVDD_OFF_COUNTER, n)

// REG_33
#define RESERVED_ANA_30                     (1 << 15)
#define CLK_32K_COUNT_NUM_SHIFT             11
#define CLK_32K_COUNT_NUM_MASK              (0xF << CLK_32K_COUNT_NUM_SHIFT)
#define CLK_32K_COUNT_NUM(n)                BITFIELD_VAL(CLK_32K_COUNT_NUM, n)
#define REG_IPTAT_CORE_I_SEL_SHIFT          5
#define REG_IPTAT_CORE_I_SEL_MASK           (0x3F << REG_IPTAT_CORE_I_SEL_SHIFT)
#define REG_IPTAT_CORE_I_SEL(n)             BITFIELD_VAL(REG_IPTAT_CORE_I_SEL, n)
#define DIG_IPTAT_CORE_EN                   (1 << 4)
#define REG_EFUSE_DATA_OUT_DR               (1 << 3)
#define REG_EFUSE_WRITE_AUTO_MODE           (1 << 2)
#define REG_EFUSE_TRIGGER_WRITE             (1 << 1)
#define REG_EFUSE_TRIGGER_READ              (1 << 0)

// REG_34
#define REG_DIV_HW_RESET_SHIFT              0
#define REG_DIV_HW_RESET_MASK               (0xFFFF << REG_DIV_HW_RESET_SHIFT)
#define REG_DIV_HW_RESET(n)                 BITFIELD_VAL(REG_DIV_HW_RESET, n)

// REG_35
#define REG_WDT_TIMER_SHIFT                 0
#define REG_WDT_TIMER_MASK                  (0xFFFF << REG_WDT_TIMER_SHIFT)
#define REG_WDT_TIMER(n)                    BITFIELD_VAL(REG_WDT_TIMER, n)

// REG_36
#define RESERVED_ANA_33_31_SHIFT            13
#define RESERVED_ANA_33_31_MASK             (0x7 << RESERVED_ANA_33_31_SHIFT)
#define RESERVED_ANA_33_31(n)               BITFIELD_VAL(RESERVED_ANA_33_31, n)
#define CLK_32K_COUNT_EN                    (1 << 12)
#define CLK_32K_COUNT_CLOCK_EN              (1 << 11)
#define POWERON_DETECT_EN                   (1 << 10)
#define MERGE_INTR                          (1 << 9)
#define REG_WDT_EN                          (1 << 8)
#define REG_WDT_RESET_EN                    (1 << 7)
#define REG_HW_RESET_TIME_SHIFT             1
#define REG_HW_RESET_TIME_MASK              (0x3F << REG_HW_RESET_TIME_SHIFT)
#define REG_HW_RESET_TIME(n)                BITFIELD_VAL(REG_HW_RESET_TIME, n)
#define REG_HW_RESET_EN                     (1 << 0)

// REG_37
#define RESERVED_ANA_15_0_SHIFT             0
#define RESERVED_ANA_15_0_MASK              (0xFFFF << RESERVED_ANA_15_0_SHIFT)
#define RESERVED_ANA_15_0(n)                BITFIELD_VAL(RESERVED_ANA_15_0, n)

// REG_38
#define RESERVED_DIG_15_0_SHIFT             0
#define RESERVED_DIG_15_0_MASK              (0xFFFF << RESERVED_DIG_15_0_SHIFT)
#define RESERVED_DIG_15_0(n)                BITFIELD_VAL(RESERVED_DIG_15_0, n)

// REG_39
#define REG_CLK_NCP_DIV_SHIFT               10
#define REG_CLK_NCP_DIV_MASK                (0x1F << REG_CLK_NCP_DIV_SHIFT)
#define REG_CLK_NCP_DIV(n)                  BITFIELD_VAL(REG_CLK_NCP_DIV, n)
#define REG_NCP_CLK_INV_EN                  (1 << 9)
#define REG_EAR_DETECT_RESET                (1 << 8)
#define REG_EAR_RES_RESET                   (1 << 7)
#define REG_EAR_RES_CLK_INV_EN              (1 << 6)
#define REG_BG_CLK_EN                       (1 << 5)
#define REG_BG_CLK_DIV_SHIFT                0
#define REG_BG_CLK_DIV_MASK                 (0x1F << REG_BG_CLK_DIV_SHIFT)
#define REG_BG_CLK_DIV(n)                   BITFIELD_VAL(REG_BG_CLK_DIV, n)

// REG_3A
#define REG_EAR_DETECT_CLK_INV_EN           (1 << 15)
#define REG_EAR_CLK_CP_DIV_SHIFT            10
#define REG_EAR_CLK_CP_DIV_MASK             (0x1F << REG_EAR_CLK_CP_DIV_SHIFT)
#define REG_EAR_CLK_CP_DIV(n)               BITFIELD_VAL(REG_EAR_CLK_CP_DIV, n)
#define REG_CLK_EAR_DETECT_DIV_SHIFT        0
#define REG_CLK_EAR_DETECT_DIV_MASK         (0x3FF << REG_CLK_EAR_DETECT_DIV_SHIFT)
#define REG_CLK_EAR_DETECT_DIV(n)           BITFIELD_VAL(REG_CLK_EAR_DETECT_DIV, n)

// REG_3B
#define REG_CLK_EAR_RES_DIV_SHIFT           0
#define REG_CLK_EAR_RES_DIV_MASK            (0xFFF << REG_CLK_EAR_RES_DIV_SHIFT)
#define REG_CLK_EAR_RES_DIV(n)              BITFIELD_VAL(REG_CLK_EAR_RES_DIV, n)

// REG_3C
#define REG_NCP_RESET                       (1 << 13)
#define REG_EAR_CLK_CP_EN                   (1 << 12)
#define REG_CLK_EAR_DETECT_EN               (1 << 11)
#define REG_CLK_EAR_RES_EN                  (1 << 10)
#define REG_NCP_CLK_EN                      (1 << 9)
#define GPADC_SAMPLE_DONE_INTR_EN           (1 << 8)
#define REG_GPADC_CHAN_EN_SHIFT             0
#define REG_GPADC_CHAN_EN_MASK              (0xFF << REG_GPADC_CHAN_EN_SHIFT)
#define REG_GPADC_CHAN_EN(n)                BITFIELD_VAL(REG_GPADC_CHAN_EN, n)

// REG_3D
#define RESERVED_DIG_31_16_SHIFT            0
#define RESERVED_DIG_31_16_MASK             (0xFFFF << RESERVED_DIG_31_16_SHIFT)
#define RESERVED_DIG_31_16(n)               BITFIELD_VAL(RESERVED_DIG_31_16, n)

// REG_3E
#define REG_EFUSE2_TRIGGER_READ             (1 << 15)
#define REG_EFUSE2_TRIGGER_WRITE            (1 << 14)
#define REG_EFUSE2_WRITE_AUTO_MODE          (1 << 13)
#define REG_EFUSE2_DATA_OUT_DR              (1 << 12)
#define REG_EFUSE2_ADDRESS_SHIFT            4
#define REG_EFUSE2_ADDRESS_MASK             (0xFF << REG_EFUSE2_ADDRESS_SHIFT)
#define REG_EFUSE2_ADDRESS(n)               BITFIELD_VAL(REG_EFUSE2_ADDRESS, n)
#define REG_EFUSE2_PROGRAM_MODE_EN          (1 << 3)
#define REG_EFUSE2_PU_AVDD                  (1 << 2)
#define REG_EFUSE2_READ_MODE_EN             (1 << 1)
#define REG_EFUSE2_CLK_EN                   (1 << 0)

// REG_3F
#define SMIC_EFUSE2_A_SHIFT                 8
#define SMIC_EFUSE2_A_MASK                  (0xFF << SMIC_EFUSE2_A_SHIFT)
#define SMIC_EFUSE2_A(n)                    BITFIELD_VAL(SMIC_EFUSE2_A, n)
#define SMIC_EFUSE2_Q_SHIFT                 0
#define SMIC_EFUSE2_Q_MASK                  (0xFF << SMIC_EFUSE2_Q_SHIFT)
#define SMIC_EFUSE2_Q(n)                    BITFIELD_VAL(SMIC_EFUSE2_Q, n)

// REG_40
#define REG_CP_AUX_CLK_EN                   (1 << 10)
#define REG_CLK_4M_EN                       (1 << 9)
#define REG_CLK_4M_EN_DR                    (1 << 8)
#define REG_CLK_4M_PU_LDO                   (1 << 7)
#define REG_CLK_4M_PU_LDO_DR                (1 << 6)
#define CP_AUX_VOUT_HIGHRES                 (1 << 5)
#define LP_EN_VHPPA_LDO_REG                 (1 << 4)
#define LP_EN_VHPPA_LDO_DR                  (1 << 3)
#define PU_VHPPA_LDO_REG                    (1 << 2)
#define PU_VHPPA_LDO_DR                     (1 << 1)
#define LP_EN_VHPPA_LDO_DSLEEP              (1 << 0)

// REG_41
#define SAR_VIN_GAIN_SEL_SHIFT              13
#define SAR_VIN_GAIN_SEL_MASK               (0x3 << SAR_VIN_GAIN_SEL_SHIFT)
#define SAR_VIN_GAIN_SEL(n)                 BITFIELD_VAL(SAR_VIN_GAIN_SEL, n)
#define GPADC_CHAN_DATA_INTR_MSK_SHIFT      2
#define GPADC_CHAN_DATA_INTR_MSK_MASK       (0xFF << GPADC_CHAN_DATA_INTR_MSK_SHIFT)
#define GPADC_CHAN_DATA_INTR_MSK(n)         BITFIELD_VAL(GPADC_CHAN_DATA_INTR_MSK, n)
#define GPADC_SAMPLE_DONE_INTR_MSK          (1 << 1)
#define REG_GPADC_STATE_RESET               (1 << 0)

// REG_42
#define NCP_VBIT_REG_SHIFT                  13
#define NCP_VBIT_REG_MASK                   (0x7 << NCP_VBIT_REG_SHIFT)
#define NCP_VBIT_REG(n)                     BITFIELD_VAL(NCP_VBIT_REG, n)
#define NCP_SOFT_START_SHIFT                10
#define NCP_SOFT_START_MASK                 (0x7 << NCP_SOFT_START_SHIFT)
#define NCP_SOFT_START(n)                   BITFIELD_VAL(NCP_SOFT_START, n)
#define NCP_RES_P_SHIFT                     8
#define NCP_RES_P_MASK                      (0x3 << NCP_RES_P_SHIFT)
#define NCP_RES_P(n)                        BITFIELD_VAL(NCP_RES_P, n)
#define NCP_RES_N_SHIFT                     6
#define NCP_RES_N_MASK                      (0x3 << NCP_RES_N_SHIFT)
#define NCP_RES_N(n)                        BITFIELD_VAL(NCP_RES_N, n)
#define NCP_RES_SHIFT                       3
#define NCP_RES_MASK                        (0x7 << NCP_RES_SHIFT)
#define NCP_RES(n)                          BITFIELD_VAL(NCP_RES, n)
#define NCP_REGULATION_POS                  (1 << 2)
#define NCP_REGULATION_NEG                  (1 << 1)
#define NCP_REG_MODE                        (1 << 0)

// REG_43
#define CLASSG_STEP_DIFF_HOLD_MAX_SHIFT     12
#define CLASSG_STEP_DIFF_HOLD_MAX_MASK      (0x7 << CLASSG_STEP_DIFF_HOLD_MAX_SHIFT)
#define CLASSG_STEP_DIFF_HOLD_MAX(n)        BITFIELD_VAL(CLASSG_STEP_DIFF_HOLD_MAX, n)
#define REG_PU_CP_REG                       (1 << 11)
#define REG_PU_CP_DR                        (1 << 10)
#define NCP_VSEL_MODE_DN_SHIFT              8
#define NCP_VSEL_MODE_DN_MASK               (0x3 << NCP_VSEL_MODE_DN_SHIFT)
#define NCP_VSEL_MODE_DN(n)                 BITFIELD_VAL(NCP_VSEL_MODE_DN, n)
#define NCP_VOUT_POS_SET_SHIFT              4
#define NCP_VOUT_POS_SET_MASK               (0xF << NCP_VOUT_POS_SET_SHIFT)
#define NCP_VOUT_POS_SET(n)                 BITFIELD_VAL(NCP_VOUT_POS_SET, n)
#define NCP_VOUT_NEG_SET_SHIFT              0
#define NCP_VOUT_NEG_SET_MASK               (0xF << NCP_VOUT_NEG_SET_SHIFT)
#define NCP_VOUT_NEG_SET(n)                 BITFIELD_VAL(NCP_VOUT_NEG_SET, n)

// REG_44
#define NCP_OCP_11_SHIFT                    12
#define NCP_OCP_11_MASK                     (0xF << NCP_OCP_11_SHIFT)
#define NCP_OCP_11(n)                       BITFIELD_VAL(NCP_OCP_11, n)
#define NCP_OCP_10_SHIFT                    8
#define NCP_OCP_10_MASK                     (0xF << NCP_OCP_10_SHIFT)
#define NCP_OCP_10(n)                       BITFIELD_VAL(NCP_OCP_10, n)
#define NCP_OCP_01_SHIFT                    4
#define NCP_OCP_01_MASK                     (0xF << NCP_OCP_01_SHIFT)
#define NCP_OCP_01(n)                       BITFIELD_VAL(NCP_OCP_01, n)
#define NCP_OCP_00_SHIFT                    0
#define NCP_OCP_00_MASK                     (0xF << NCP_OCP_00_SHIFT)
#define NCP_OCP_00(n)                       BITFIELD_VAL(NCP_OCP_00, n)

// REG_45
#define NCP_CP_MODE_SEL_11                  (1 << 15)
#define NCP_CP_MODE_SEL_10                  (1 << 14)
#define NCP_CP_MODE_SEL_01                  (1 << 13)
#define NCP_CP_MODE_SEL_00                  (1 << 12)
#define NCP_CLK_SEL_DEC_FAST                (1 << 11)
#define NCP_CLK_SEL_INC_FAST                (1 << 10)
#define NCP_OCP_DEC_FAST                    (1 << 9)
#define NCP_OCP_INC_FAST                    (1 << 8)
#define NCP_CLK_SEL_STEP_NUM_SHIFT          0
#define NCP_CLK_SEL_STEP_NUM_MASK           (0xFF << NCP_CLK_SEL_STEP_NUM_SHIFT)
#define NCP_CLK_SEL_STEP_NUM(n)             BITFIELD_VAL(NCP_CLK_SEL_STEP_NUM, n)

// REG_46
#define NCP_OCP_STEP_NUM_SHIFT              8
#define NCP_OCP_STEP_NUM_MASK               (0xFF << NCP_OCP_STEP_NUM_SHIFT)
#define NCP_OCP_STEP_NUM(n)                 BITFIELD_VAL(NCP_OCP_STEP_NUM, n)
#define EAR_RES_POINT_NUM_SHIFT             0
#define EAR_RES_POINT_NUM_MASK              (0xFF << EAR_RES_POINT_NUM_SHIFT)
#define EAR_RES_POINT_NUM(n)                BITFIELD_VAL(EAR_RES_POINT_NUM, n)

// REG_47
#define BUCK_VANA_BIT_NORMAL_SHIFT          8
#define BUCK_VANA_BIT_NORMAL_MASK           (0xFF << BUCK_VANA_BIT_NORMAL_SHIFT)
#define BUCK_VANA_BIT_NORMAL(n)             BITFIELD_VAL(BUCK_VANA_BIT_NORMAL, n)
#define BUCK_VANA_BIT_DSLEEP_SHIFT          0
#define BUCK_VANA_BIT_DSLEEP_MASK           (0xFF << BUCK_VANA_BIT_DSLEEP_SHIFT)
#define BUCK_VANA_BIT_DSLEEP(n)             BITFIELD_VAL(BUCK_VANA_BIT_DSLEEP, n)

// REG_48
#define MIC_LP_ENABLE_REG                   (1 << 14)
#define MIC_LP_ENABLE_DR                    (1 << 13)
#define REG_CP_AUX_RES_SHIFT                10
#define REG_CP_AUX_RES_MASK                 (0x7 << REG_CP_AUX_RES_SHIFT)
#define REG_CP_AUX_RES(n)                   BITFIELD_VAL(REG_CP_AUX_RES, n)
#define REG_CP_AUX_REGULATION               (1 << 9)
#define REG_MICBIASB_PULL_DOWN              (1 << 8)
#define REG_PULLDOWN_VHPPA                  (1 << 7)
#define REG_RES_SEL_VHPPA_SHIFT             2
#define REG_RES_SEL_VHPPA_MASK              (0x1F << REG_RES_SEL_VHPPA_SHIFT)
#define REG_RES_SEL_VHPPA(n)                BITFIELD_VAL(REG_RES_SEL_VHPPA, n)
#define REG_PU_LDO_VHPPA_DSLEEP             (1 << 1)

// REG_49
#define SOFT_POWER_OFF                      (1 << 13)
#define HARDWARE_POWER_OFF_EN               (1 << 12)
#define AC_ON_EN                            (1 << 11)
#define KEY_START                           (1 << 10)
#define POWER_INTR_MSK                      (1 << 9)
#define POWER_ON_BYPASS_EAR_GND             (1 << 8)
#define VCORE_LDO_ON_DELAY_REG              (1 << 7)
#define VCORE_LDO_ON_DELAY_DR               (1 << 6)
#define REG_CP_AUX_CLK_SEL_SHIFT            3
#define REG_CP_AUX_CLK_SEL_MASK             (0x7 << REG_CP_AUX_CLK_SEL_SHIFT)
#define REG_CP_AUX_CLK_SEL(n)               BITFIELD_VAL(REG_CP_AUX_CLK_SEL, n)
#define REG_CP_AUX_VOUT_SET_SHIFT           0
#define REG_CP_AUX_VOUT_SET_MASK            (0x7 << REG_CP_AUX_VOUT_SET_SHIFT)
#define REG_CP_AUX_VOUT_SET(n)              BITFIELD_VAL(REG_CP_AUX_VOUT_SET, n)

// REG_4A
#define USB_INSERT_INTR_MSKED               (1 << 15)
#define RTC_INT1_MSKED                      (1 << 14)
#define RTC_INT0_MSKED                      (1 << 13)
#define KEY_ERR1_INTR_MSKED                 (1 << 12)
#define KEY_ERR0_INTR_MSKED                 (1 << 11)
#define KEY_PRESS_INTR_MSKED                (1 << 10)
#define KEY_RELEASE_INTR_MSKED              (1 << 9)
#define GPADC_CHAN_DATA_INTR_MSKED_SHIFT    0
#define GPADC_CHAN_DATA_INTR_MSKED_MASK     (0xFF << GPADC_CHAN_DATA_INTR_MSKED_SHIFT)
#define GPADC_CHAN_DATA_INTR_MSKED(n)       BITFIELD_VAL(GPADC_CHAN_DATA_INTR_MSKED, n)

// REG_4B
#define CLK_32K_COUNTER_26M_SHIFT           0
#define CLK_32K_COUNTER_26M_MASK            (0x7FFF << CLK_32K_COUNTER_26M_SHIFT)
#define CLK_32K_COUNTER_26M(n)              BITFIELD_VAL(CLK_32K_COUNTER_26M, n)
#define CLK_32K_COUNTER_26M_READY           (1 << 15)

// REG_4C
#define POWER_ON                            (1 << 12)
#define CORE_GPIO_OUT                       (1 << 11)
#define WDT_LOAD                            (1 << 10)
#define RTC_LOAD                            (1 << 9)
#define PMU_LDO_ON_SOURCE_SHIFT             7
#define PMU_LDO_ON_SOURCE_MASK              (0x3 << PMU_LDO_ON_SOURCE_SHIFT)
#define PMU_LDO_ON_SOURCE(n)                BITFIELD_VAL(PMU_LDO_ON_SOURCE, n)
#define KEY_RELEASE_INTR                    (1 << 6)
#define KEY_PRESS_INTR                      (1 << 5)
#define KEY_ERR0_INTR                       (1 << 4)
#define KEY_ERR1_INTR                       (1 << 3)
#define RTC_INT_0                           (1 << 2)
#define RTC_INT_1                           (1 << 1)
#define USBINSERT_INTR2CPU                  (1 << 0)

// REG_4D
#define CLK_32K_COUNT_EN_TRIGGER            (1 << 11)
#define REG_POWER_ON_RELEASE_CLR            (1 << 10)
#define REG_POWER_ON_PRESS_CLR              (1 << 9)
#define REG_CHARGE_IN_INTR_CLR              (1 << 8)
#define REG_CHARGE_OUT_INTR_CLR             (1 << 7)
#define KEY_RELEASE_INTR_CLR                (1 << 6)
#define KEY_PRESS_INTR_CLR                  (1 << 5)
#define KEY_ERR0_INTR_CLR                   (1 << 4)
#define KEY_ERR1_INTR_CLR                   (1 << 3)
#define RTC_INT_CLR_0                       (1 << 2)
#define RTC_INT_CLR_1                       (1 << 1)
#define USB_INTR_CLR                        (1 << 0)

// REG_4E
#define SMIC_EFUSE_A_SHIFT                  8
#define SMIC_EFUSE_A_MASK                   (0xFF << SMIC_EFUSE_A_SHIFT)
#define SMIC_EFUSE_A(n)                     BITFIELD_VAL(SMIC_EFUSE_A, n)
#define SMIC_EFUSE_Q_SHIFT                  0
#define SMIC_EFUSE_Q_MASK                   (0xFF << SMIC_EFUSE_Q_SHIFT)
#define SMIC_EFUSE_Q(n)                     BITFIELD_VAL(SMIC_EFUSE_Q, n)

// REG_4F
#define RTC_VALUE_15_0_SHIFT                0
#define RTC_VALUE_15_0_MASK                 (0xFFFF << RTC_VALUE_15_0_SHIFT)
#define RTC_VALUE_15_0(n)                   BITFIELD_VAL(RTC_VALUE_15_0, n)

// REG_50
#define RTC_VALUE_31_16_SHIFT               0
#define RTC_VALUE_31_16_MASK                (0xFFFF << RTC_VALUE_31_16_SHIFT)
#define RTC_VALUE_31_16(n)                  BITFIELD_VAL(RTC_VALUE_31_16, n)

// REG_51
#define DATA_CHAN0_SHIFT                    0
#define DATA_CHAN0_MASK                     (0x3FF << DATA_CHAN0_SHIFT)
#define DATA_CHAN0(n)                       BITFIELD_VAL(DATA_CHAN0, n)

// REG_52
#define DATA_CHAN1_SHIFT                    0
#define DATA_CHAN1_MASK                     (0x3FF << DATA_CHAN1_SHIFT)
#define DATA_CHAN1(n)                       BITFIELD_VAL(DATA_CHAN1, n)

// REG_53
#define DATA_CHAN2_SHIFT                    0
#define DATA_CHAN2_MASK                     (0x3FF << DATA_CHAN2_SHIFT)
#define DATA_CHAN2(n)                       BITFIELD_VAL(DATA_CHAN2, n)

// REG_54
#define DATA_CHAN3_SHIFT                    0
#define DATA_CHAN3_MASK                     (0x3FF << DATA_CHAN3_SHIFT)
#define DATA_CHAN3(n)                       BITFIELD_VAL(DATA_CHAN3, n)

// REG_55
#define DATA_CHAN4_SHIFT                    0
#define DATA_CHAN4_MASK                     (0x3FF << DATA_CHAN4_SHIFT)
#define DATA_CHAN4(n)                       BITFIELD_VAL(DATA_CHAN4, n)

// REG_56
#define DATA_CHAN5_SHIFT                    0
#define DATA_CHAN5_MASK                     (0x3FF << DATA_CHAN5_SHIFT)
#define DATA_CHAN5(n)                       BITFIELD_VAL(DATA_CHAN5, n)

// REG_57
#define DATA_CHAN6_SHIFT                    0
#define DATA_CHAN6_MASK                     (0x3FF << DATA_CHAN6_SHIFT)
#define DATA_CHAN6(n)                       BITFIELD_VAL(DATA_CHAN6, n)

// REG_58
#define DATA_CHAN7_SHIFT                    0
#define DATA_CHAN7_MASK                     (0x3FF << DATA_CHAN7_SHIFT)
#define DATA_CHAN7(n)                       BITFIELD_VAL(DATA_CHAN7, n)

// REG_59
#define POWER_ON_RELEASE                    (1 << 15)
#define POWER_ON_PRESS                      (1 << 14)
#define DIG_PU_VCODEC                       (1 << 13)
#define DIG_PU_VCRYSTAL                     (1 << 12)
#define DIG_PU_VIO                          (1 << 11)
#define DIG_PU_VUSB                         (1 << 10)
#define DIG_PU_VUSB11                       (1 << 9)
#define DIG_PU_VRTC_RF                      (1 << 8)
#define DIG_PU_BIAS_LDO                     (1 << 7)
#define DIG_PU_LP_BIAS_LDO                  (1 << 6)
#define INTR_MSKED_CHARGE_SHIFT             3
#define INTR_MSKED_CHARGE_MASK              (0x3 << INTR_MSKED_CHARGE_SHIFT)
#define INTR_MSKED_CHARGE(n)                BITFIELD_VAL(INTR_MSKED_CHARGE, n)
#define AC_ON                               (1 << 2)
#define AC_ON_DET_OUT                       (1 << 1)
#define AC_ON_DET_IN                        (1 << 0)

// REG_5A
#define DEEPSLEEP_MODE                      (1 << 13)
#define BDOPTION_SPI_VSEL_RTC               (1 << 12)
#define UVLO_LV                             (1 << 11)
#define USB_STATUS_RX_DM                    (1 << 10)
#define USB_STATUS_RX_DP                    (1 << 9)
#define PMU_LDO_ON_STATUS                          (1 << 8)
#define DIG_PU_DCDC_VCORE                   (1 << 7)
#define DIG_PU_VCORE_LDO                    (1 << 6)
#define DIG_PU_VANA_LDO                     (1 << 5)
#define DIG_PU_DCDC_VANA                    (1 << 4)
#define DIG_PU_LPO                          (1 << 3)
#define DIG_PU_VHPPA                        (1 << 1)
#define DIG_PU_SAR                          (1 << 0)

// REG_5B
#define EAR_RES_FIRST_ORDER_STEP_SHIFT      0
#define EAR_RES_FIRST_ORDER_STEP_MASK       (0xFFFF << EAR_RES_FIRST_ORDER_STEP_SHIFT)
#define EAR_RES_FIRST_ORDER_STEP(n)         BITFIELD_VAL(EAR_RES_FIRST_ORDER_STEP, n)

// REG_5C
#define EAR_RES_SAMPLE_MODE_SHIFT           14
#define EAR_RES_SAMPLE_MODE_MASK            (0x3 << EAR_RES_SAMPLE_MODE_SHIFT)
#define EAR_RES_SAMPLE_MODE(n)              BITFIELD_VAL(EAR_RES_SAMPLE_MODE, n)
#define EAR_RES_SECOND_ORDER_STEP_SHIFT     0
#define EAR_RES_SECOND_ORDER_STEP_MASK      (0x3FFF << EAR_RES_SECOND_ORDER_STEP_SHIFT)
#define EAR_RES_SECOND_ORDER_STEP(n)        BITFIELD_VAL(EAR_RES_SECOND_ORDER_STEP, n)

// REG_5D
#define EAR_RES_SAMPLE_INTERVAL_SHIFT       10
#define EAR_RES_SAMPLE_INTERVAL_MASK        (0x3F << EAR_RES_SAMPLE_INTERVAL_SHIFT)
#define EAR_RES_SAMPLE_INTERVAL(n)          BITFIELD_VAL(EAR_RES_SAMPLE_INTERVAL, n)
#define EAR_RES_FIRST_RANGE0_MAX_SHIFT      0
#define EAR_RES_FIRST_RANGE0_MAX_MASK       (0x3FF << EAR_RES_FIRST_RANGE0_MAX_SHIFT)
#define EAR_RES_FIRST_RANGE0_MAX(n)         BITFIELD_VAL(EAR_RES_FIRST_RANGE0_MAX, n)

// REG_5E
#define EAR_RES_WAIT_CHANGE_NUM_SHIFT       10
#define EAR_RES_WAIT_CHANGE_NUM_MASK        (0xF << EAR_RES_WAIT_CHANGE_NUM_SHIFT)
#define EAR_RES_WAIT_CHANGE_NUM(n)          BITFIELD_VAL(EAR_RES_WAIT_CHANGE_NUM, n)
#define EAR_RES_FIRST_RANGE0_MIN_SHIFT      0
#define EAR_RES_FIRST_RANGE0_MIN_MASK       (0x3FF << EAR_RES_FIRST_RANGE0_MIN_SHIFT)
#define EAR_RES_FIRST_RANGE0_MIN(n)         BITFIELD_VAL(EAR_RES_FIRST_RANGE0_MIN, n)

// REG_5F
#define EAR_RES_SECOND_RANGE1_ENABLE        (1 << 15)
#define EAR_RES_SECOND_RANGE0_ENABLE        (1 << 14)
#define EAR_RES_FIRST_RANGE1_ENABLE         (1 << 13)
#define EAR_RES_FIRST_RANGE0_ENABLE         (1 << 12)
#define EAR_RES_USE_FIRST_ORDER             (1 << 11)
#define EAR_RES_USE_ONE_ROUND               (1 << 10)
#define EAR_RES_FIRST_RANGE1_MAX_SHIFT      0
#define EAR_RES_FIRST_RANGE1_MAX_MASK       (0x3FF << EAR_RES_FIRST_RANGE1_MAX_SHIFT)
#define EAR_RES_FIRST_RANGE1_MAX(n)         BITFIELD_VAL(EAR_RES_FIRST_RANGE1_MAX, n)

// REG_60
#define EAR_DETECT_SAMPLE_MODE_SHIFT        14
#define EAR_DETECT_SAMPLE_MODE_MASK         (0x3 << EAR_DETECT_SAMPLE_MODE_SHIFT)
#define EAR_DETECT_SAMPLE_MODE(n)           BITFIELD_VAL(EAR_DETECT_SAMPLE_MODE, n)
#define EAR_DETECT_STATE_RESET              (1 << 13)
#define EAR_RES_ISEL_L_EN                   (1 << 12)
#define EAR_RES_ISEL_R_EN                   (1 << 11)
#define EAR_RES_ENABLE                      (1 << 10)
#define EAR_RES_FIRST_RANGE1_MIN_SHIFT      0
#define EAR_RES_FIRST_RANGE1_MIN_MASK       (0x3FF << EAR_RES_FIRST_RANGE1_MIN_SHIFT)
#define EAR_RES_FIRST_RANGE1_MIN(n)         BITFIELD_VAL(EAR_RES_FIRST_RANGE1_MIN, n)

// REG_61
#define REG_EAR_EN_GND                      (1 << 15)
#define REG_EAR_EN_MIC                      (1 << 14)
#define REG_EAR_SEL_MIC                     (1 << 13)
#define REG_EAR_SEL_GND                     (1 << 12)
#define REG_EAR_SEL_DR                      (1 << 11)
#define REG_EAR_EN_RDAC                     (1 << 10)
#define EAR_RES_SECOND_RANGE0_MAX_SHIFT     0
#define EAR_RES_SECOND_RANGE0_MAX_MASK      (0x3FF << EAR_RES_SECOND_RANGE0_MAX_SHIFT)
#define EAR_RES_SECOND_RANGE0_MAX(n)        BITFIELD_VAL(EAR_RES_SECOND_RANGE0_MAX, n)

// REG_62
#define REG_EAR_MICBIASA_EN                 (1 << 13)
#define REG_EAR_MIC_DIN_DR                  (1 << 11)
#define REG_EAR_GND_DIN_DR                  (1 << 10)
#define EAR_RES_SECOND_RANGE0_MIN_SHIFT     0
#define EAR_RES_SECOND_RANGE0_MIN_MASK      (0x3FF << EAR_RES_SECOND_RANGE0_MIN_SHIFT)
#define EAR_RES_SECOND_RANGE0_MIN(n)        BITFIELD_VAL(EAR_RES_SECOND_RANGE0_MIN, n)

// REG_63
#define NCP_OCP_DR                          (1 << 15)
#define NCP_CP_MODE_SEL_DR                  (1 << 14)
#define NCP_CLK_SEL_DR                      (1 << 13)
#define IMPDET_DIN_DR                       (1 << 12)
#define IMPDET_ISEL_LEFT_RIGHT_DR           (1 << 11)
#define IMPDET_LEFT_RIGHT_DR                (1 << 10)
#define EAR_RES_SECOND_RANGE1_MAX_SHIFT     0
#define EAR_RES_SECOND_RANGE1_MAX_MASK      (0x3FF << EAR_RES_SECOND_RANGE1_MAX_SHIFT)
#define EAR_RES_SECOND_RANGE1_MAX(n)        BITFIELD_VAL(EAR_RES_SECOND_RANGE1_MAX, n)

// REG_64
#define SAR_EN_VIN_GAIN_REG                 (1 << 15)
#define SAR_EN_VIN_GAIN_DR                  (1 << 14)
#define EAR_DETECT_FLOW_DONE_INTR_MSK       (1 << 13)
#define EAR_DETECT_FLOW_DONE_INTR_EN        (1 << 12)
#define EAR_RES_FLOW_DONE_INTR_MSK          (1 << 11)
#define EAR_RES_FLOW_DONE_INTR_EN           (1 << 10)
#define EAR_RES_SECOND_RANGE1_MIN_SHIFT     0
#define EAR_RES_SECOND_RANGE1_MIN_MASK      (0x3FF << EAR_RES_SECOND_RANGE1_MIN_SHIFT)
#define EAR_RES_SECOND_RANGE1_MIN(n)        BITFIELD_VAL(EAR_RES_SECOND_RANGE1_MIN, n)

// REG_65
#define EAR_DETECT_GND_CHANGE_NUM_SHIFT     10
#define EAR_DETECT_GND_CHANGE_NUM_MASK      (0x3F << EAR_DETECT_GND_CHANGE_NUM_SHIFT)
#define EAR_DETECT_GND_CHANGE_NUM(n)        BITFIELD_VAL(EAR_DETECT_GND_CHANGE_NUM, n)
#define EAR_DETECT_SAMPLE_RANGE0_MIN_SHIFT  0
#define EAR_DETECT_SAMPLE_RANGE0_MIN_MASK   (0x3FF << EAR_DETECT_SAMPLE_RANGE0_MIN_SHIFT)
#define EAR_DETECT_SAMPLE_RANGE0_MIN(n)     BITFIELD_VAL(EAR_DETECT_SAMPLE_RANGE0_MIN, n)

// REG_66
#define EAR_DETECT_MIC_CHANGE_NUM_SHIFT     10
#define EAR_DETECT_MIC_CHANGE_NUM_MASK      (0x3F << EAR_DETECT_MIC_CHANGE_NUM_SHIFT)
#define EAR_DETECT_MIC_CHANGE_NUM(n)        BITFIELD_VAL(EAR_DETECT_MIC_CHANGE_NUM, n)
#define EAR_DETECT_SAMPLE_RANGE0_MAX_SHIFT  0
#define EAR_DETECT_SAMPLE_RANGE0_MAX_MASK   (0x3FF << EAR_DETECT_SAMPLE_RANGE0_MAX_SHIFT)
#define EAR_DETECT_SAMPLE_RANGE0_MAX(n)     BITFIELD_VAL(EAR_DETECT_SAMPLE_RANGE0_MAX, n)

// REG_67
#define EAR_DETECT_SAMPLE_CNT_NUM_SHIFT     10
#define EAR_DETECT_SAMPLE_CNT_NUM_MASK      (0x3F << EAR_DETECT_SAMPLE_CNT_NUM_SHIFT)
#define EAR_DETECT_SAMPLE_CNT_NUM(n)        BITFIELD_VAL(EAR_DETECT_SAMPLE_CNT_NUM, n)
#define EAR_DETECT_SAMPLE_RANGE1_MIN_SHIFT  0
#define EAR_DETECT_SAMPLE_RANGE1_MIN_MASK   (0x3FF << EAR_DETECT_SAMPLE_RANGE1_MIN_SHIFT)
#define EAR_DETECT_SAMPLE_RANGE1_MIN(n)     BITFIELD_VAL(EAR_DETECT_SAMPLE_RANGE1_MIN, n)

// REG_68
#define NCP_CLK_SEL_00_SHIFT                13
#define NCP_CLK_SEL_00_MASK                 (0x7 << NCP_CLK_SEL_00_SHIFT)
#define NCP_CLK_SEL_00(n)                   BITFIELD_VAL(NCP_CLK_SEL_00, n)
#define NCP_CLK_SEL_01_SHIFT                10
#define NCP_CLK_SEL_01_MASK                 (0x7 << NCP_CLK_SEL_01_SHIFT)
#define NCP_CLK_SEL_01(n)                   BITFIELD_VAL(NCP_CLK_SEL_01, n)
#define EAR_DETECT_SAMPLE_RANGE1_MAX_SHIFT  0
#define EAR_DETECT_SAMPLE_RANGE1_MAX_MASK   (0x3FF << EAR_DETECT_SAMPLE_RANGE1_MAX_SHIFT)
#define EAR_DETECT_SAMPLE_RANGE1_MAX(n)     BITFIELD_VAL(EAR_DETECT_SAMPLE_RANGE1_MAX, n)

// REG_69
#define NCP_CLK_SEL_10_SHIFT                13
#define NCP_CLK_SEL_10_MASK                 (0x7 << NCP_CLK_SEL_10_SHIFT)
#define NCP_CLK_SEL_10(n)                   BITFIELD_VAL(NCP_CLK_SEL_10, n)
#define NCP_CLK_SEL_11_SHIFT                10
#define NCP_CLK_SEL_11_MASK                 (0x7 << NCP_CLK_SEL_11_SHIFT)
#define NCP_CLK_SEL_11(n)                   BITFIELD_VAL(NCP_CLK_SEL_11, n)
#define EAR_DETECT_SAMPLE_MIC_MAX_SHIFT     0
#define EAR_DETECT_SAMPLE_MIC_MAX_MASK      (0x3FF << EAR_DETECT_SAMPLE_MIC_MAX_SHIFT)
#define EAR_DETECT_SAMPLE_MIC_MAX(n)        BITFIELD_VAL(EAR_DETECT_SAMPLE_MIC_MAX, n)

// REG_6A
#define EAR_RES_ISEL_L_SECOND_SHIFT         8
#define EAR_RES_ISEL_L_SECOND_MASK          (0xFF << EAR_RES_ISEL_L_SECOND_SHIFT)
#define EAR_RES_ISEL_L_SECOND(n)            BITFIELD_VAL(EAR_RES_ISEL_L_SECOND, n)
#define EAR_RES_ISEL_L_FIRST_SHIFT          0
#define EAR_RES_ISEL_L_FIRST_MASK           (0xFF << EAR_RES_ISEL_L_FIRST_SHIFT)
#define EAR_RES_ISEL_L_FIRST(n)             BITFIELD_VAL(EAR_RES_ISEL_L_FIRST, n)

// REG_6B
#define EAR_RES_ISEL_R_SECOND_SHIFT         8
#define EAR_RES_ISEL_R_SECOND_MASK          (0xFF << EAR_RES_ISEL_R_SECOND_SHIFT)
#define EAR_RES_ISEL_R_SECOND(n)            BITFIELD_VAL(EAR_RES_ISEL_R_SECOND, n)
#define EAR_RES_ISEL_R_FIRST_SHIFT          0
#define EAR_RES_ISEL_R_FIRST_MASK           (0xFF << EAR_RES_ISEL_R_FIRST_SHIFT)
#define EAR_RES_ISEL_R_FIRST(n)             BITFIELD_VAL(EAR_RES_ISEL_R_FIRST, n)

// REG_6C
#define EAR_DETECT_GND_STATE_CNT_NUM1_SHIFT 8
#define EAR_DETECT_GND_STATE_CNT_NUM1_MASK  (0xFF << EAR_DETECT_GND_STATE_CNT_NUM1_SHIFT)
#define EAR_DETECT_GND_STATE_CNT_NUM1(n)    BITFIELD_VAL(EAR_DETECT_GND_STATE_CNT_NUM1, n)
#define EAR_DETECT_GND_STATE_CNT_NUM0_SHIFT 0
#define EAR_DETECT_GND_STATE_CNT_NUM0_MASK  (0xFF << EAR_DETECT_GND_STATE_CNT_NUM0_SHIFT)
#define EAR_DETECT_GND_STATE_CNT_NUM0(n)    BITFIELD_VAL(EAR_DETECT_GND_STATE_CNT_NUM0, n)

// REG_6D
#define EAR_DETECT_MIC_STATE_CNT_NUM1_SHIFT 8
#define EAR_DETECT_MIC_STATE_CNT_NUM1_MASK  (0xFF << EAR_DETECT_MIC_STATE_CNT_NUM1_SHIFT)
#define EAR_DETECT_MIC_STATE_CNT_NUM1(n)    BITFIELD_VAL(EAR_DETECT_MIC_STATE_CNT_NUM1, n)
#define EAR_DETECT_MIC_STATE_CNT_NUM0_SHIFT 0
#define EAR_DETECT_MIC_STATE_CNT_NUM0_MASK  (0xFF << EAR_DETECT_MIC_STATE_CNT_NUM0_SHIFT)
#define EAR_DETECT_MIC_STATE_CNT_NUM0(n)    BITFIELD_VAL(EAR_DETECT_MIC_STATE_CNT_NUM0, n)

// REG_6E
#define POWER_ON_FEEDIN_OFF_DISABLE         (1 << 15)
#define EAR_DET_CP_CKSEL                    (1 << 14)
#define EAR_DETECT_MIC_BIAS_LPFSEL_NUM_SHIFT 8
#define EAR_DETECT_MIC_BIAS_LPFSEL_NUM_MASK (0x3F << EAR_DETECT_MIC_BIAS_LPFSEL_NUM_SHIFT)
#define EAR_DETECT_MIC_BIAS_LPFSEL_NUM(n)   BITFIELD_VAL(EAR_DETECT_MIC_BIAS_LPFSEL_NUM, n)
#define EAR_DETECT_MIC_BIAS_WAIT_NUM_SHIFT  0
#define EAR_DETECT_MIC_BIAS_WAIT_NUM_MASK   (0xFF << EAR_DETECT_MIC_BIAS_WAIT_NUM_SHIFT)
#define EAR_DETECT_MIC_BIAS_WAIT_NUM(n)     BITFIELD_VAL(EAR_DETECT_MIC_BIAS_WAIT_NUM, n)

// REG_6F
#define POWER_INTR_EN                       (1 << 12)
#define NCP_PD_SHOT_MODE_UP                 (1 << 11)
#define NCP_PD_SHOT_MODE_DN                 (1 << 10)
#define NCP_CAP_SHIFT                       7
#define NCP_CAP_MASK                        (0x7 << NCP_CAP_SHIFT)
#define NCP_CAP(n)                          BITFIELD_VAL(NCP_CAP, n)
#define NCP_NONOV_BIT_SHIFT                 5
#define NCP_NONOV_BIT_MASK                  (0x3 << NCP_NONOV_BIT_SHIFT)
#define NCP_NONOV_BIT(n)                    BITFIELD_VAL(NCP_NONOV_BIT, n)
#define EAR_ISEL_IDAC_SHIFT                 0
#define EAR_ISEL_IDAC_MASK                  (0x1F << EAR_ISEL_IDAC_SHIFT)
#define EAR_ISEL_IDAC(n)                    BITFIELD_VAL(EAR_ISEL_IDAC, n)

// REG_70
#define POWER_ON_FEEDIN_DB_NUM_SHIFT        11
#define POWER_ON_FEEDIN_DB_NUM_MASK         (0xF << POWER_ON_FEEDIN_DB_NUM_SHIFT)
#define POWER_ON_FEEDIN_DB_NUM(n)           BITFIELD_VAL(POWER_ON_FEEDIN_DB_NUM, n)
#define EAR_DETECT_GND_CHANGE_NUM_SECOND_SHIFT 5
#define EAR_DETECT_GND_CHANGE_NUM_SECOND_MASK (0x3F << EAR_DETECT_GND_CHANGE_NUM_SECOND_SHIFT)
#define EAR_DETECT_GND_CHANGE_NUM_SECOND(n) BITFIELD_VAL(EAR_DETECT_GND_CHANGE_NUM_SECOND, n)
#define REG_BG_CLK_DIV_BYPASS               (1 << 4)
#define REG_EAR_CLK_CP_DIV_BYPASS           (1 << 2)
#define REG_NCP_CLK_DIV_BYPASS              (1 << 1)
#define REG_GPADC_CLK_DIV_BYPASS            (1 << 0)

// REG_71
#define EAR_PLUG_RES_SEL_SHIFT              13
#define EAR_PLUG_RES_SEL_MASK               (0x7 << EAR_PLUG_RES_SEL_SHIFT)
#define EAR_PLUG_RES_SEL(n)                 BITFIELD_VAL(EAR_PLUG_RES_SEL, n)
#define EAR_PLUG_CHANGE_DB_NUM_SHIFT        2
#define EAR_PLUG_CHANGE_DB_NUM_MASK         (0x7FF << EAR_PLUG_CHANGE_DB_NUM_SHIFT)
#define EAR_PLUG_CHANGE_DB_NUM(n)           BITFIELD_VAL(EAR_PLUG_CHANGE_DB_NUM, n)
#define EAR_PLUG_CHANGE_INTR_MSK            (1 << 1)
#define EAR_PLUG_CHANGE_INTR_EN             (1 << 0)

// REG_72
#define EAR_PLUG_OUT_DB_BYPASS              (1 << 15)
#define EAR_PLUG_OUT_DB_FAST                (1 << 14)
#define HPPA_PU_VO2ADC_R_REG                (1 << 13)
#define HPPA_PU_VO2ADC_L_REG                (1 << 12)
#define HPPA_PU_VO2ADC_R_DR                 (1 << 11)
#define HPPA_PU_VO2ADC_L_DR                 (1 << 10)
#define EAR_DETECT_WITH_MIC                 (1 << 9)
#define EAR_PLUG_DETECT_INV                 (1 << 8)
#define EAR_PLUG_HYS_SEL_SHIFT              2
#define EAR_PLUG_HYS_SEL_MASK               (0xF << EAR_PLUG_HYS_SEL_SHIFT)
#define EAR_PLUG_HYS_SEL(n)                 BITFIELD_VAL(EAR_PLUG_HYS_SEL, n)
#define EAR_PLUG_DETECT_MODE_SEL_SHIFT      0
#define EAR_PLUG_DETECT_MODE_SEL_MASK       (0x3 << EAR_PLUG_DETECT_MODE_SEL_SHIFT)
#define EAR_PLUG_DETECT_MODE_SEL(n)         BITFIELD_VAL(EAR_PLUG_DETECT_MODE_SEL, n)

// REG_73
#define EAR_KEY_DETECT_EN                   (1 << 15)
#define EAR_KEY_DETECT_VTH_SHIFT            12
#define EAR_KEY_DETECT_VTH_MASK             (0x7 << EAR_KEY_DETECT_VTH_SHIFT)
#define EAR_KEY_DETECT_VTH(n)               BITFIELD_VAL(EAR_KEY_DETECT_VTH, n)
#define HPPA_PU_VO2IDAC_R_REG               (1 << 3)
#define HPPA_PU_VO2IDAC_L_REG               (1 << 2)
#define HPPA_PU_VO2IDAC_R_DR                (1 << 1)
#define HPPA_PU_VO2IDAC_L_DR                (1 << 0)

// REG_74
#define EAR_KEY_DB_NEG_NUM_SHIFT            11
#define EAR_KEY_DB_NEG_NUM_MASK             (0x1F << EAR_KEY_DB_NEG_NUM_SHIFT)
#define EAR_KEY_DB_NEG_NUM(n)               BITFIELD_VAL(EAR_KEY_DB_NEG_NUM, n)
#define EAR_KEY_DB_POS_NUM_SHIFT            4
#define EAR_KEY_DB_POS_NUM_MASK             (0x7F << EAR_KEY_DB_POS_NUM_SHIFT)
#define EAR_KEY_DB_POS_NUM(n)               BITFIELD_VAL(EAR_KEY_DB_POS_NUM, n)
#define EAR_KEY_DETECT_INV                  (1 << 3)
#define EAR_KEY_DB_NEG_BYPASS               (1 << 2)
#define EAR_KEY_CHANGE_INTR_MSK             (1 << 1)
#define EAR_KEY_CHANGE_INTR_EN              (1 << 0)

// REG_75
#define EAR_KEY_CHANGE_INTR_CLR             (1 << 13)
#define EAR_PLUG_CHANGE_INTR_CLR            (1 << 12)
#define POWER_INTR_CLR                      (1 << 11)
#define GPADC_CHAN_DATA_INTR_CLR_SHIFT      3
#define GPADC_CHAN_DATA_INTR_CLR_MASK       (0xFF << GPADC_CHAN_DATA_INTR_CLR_SHIFT)
#define GPADC_CHAN_DATA_INTR_CLR(n)         BITFIELD_VAL(GPADC_CHAN_DATA_INTR_CLR, n)
#define GPADC_SAMPLE_DONE_INTR_CLR          (1 << 2)
#define EAR_DETECT_FLOW_DONE_INTR_CLR       (1 << 1)
#define EAR_RES_FLOW_DONE_INTR_CLR          (1 << 0)

// REG_76
#define RSTN_CALIB                          (1 << 13)
#define START_CALIB                         (1 << 12)
#define CLK_OSC_SEL                         (1 << 11)
#define REG_GPADC_START_DIRECT              (1 << 10)
#define REG_AUTO_EAR_DETECT                 (1 << 9)
#define REG_DIRECT_CLOSE_MIC_B              (1 << 8)
#define REG_DIRECT_CLOSE_MIC_A              (1 << 7)
#define REG_DIRECT_OPEN_MIC_B               (1 << 6)
#define REG_DIRECT_OPEN_MIC_A               (1 << 5)
#define REG_DIRECT_SWITCH_GND               (1 << 4)
#define REG_DIRECT_CLOSE_GND_B              (1 << 3)
#define REG_DIRECT_CLOSE_GND_A              (1 << 2)
#define REG_DIRECT_OPEN_GND_B               (1 << 1)
#define REG_DIRECT_OPEN_GND_A               (1 << 0)

// REG_77
#define CODEC_CLASSG_STEP_SHIFT             14
#define CODEC_CLASSG_STEP_MASK              (0x3 << CODEC_CLASSG_STEP_SHIFT)
#define CODEC_CLASSG_STEP(n)                BITFIELD_VAL(CODEC_CLASSG_STEP, n)
#define UVLO                                (1 << 12)
#define EAR_KEY_DETECT_DB                   (1 << 11)
#define POWER_ON_FEEDIN_DB                  (1 << 10)
#define EAR_PLUG_DETECT_DB                  (1 << 9)
#define NCP_PU                              (1 << 8)
#define EAR_GND_ON                          (1 << 7)
#define BG_VBG_SEL                          (1 << 4)
#define DIG_PU_LDO_BG                       (1 << 3)
#define LP_MODE_RTC                         (1 << 2)
#define DEEPSLEEP_MODE_DLY                  (1 << 1)
#define DEEPSLEEP_MODE_DIG                  (1 << 0)

// REG_78
#define EAR_RES_SECOND_RANGE1_MATCH         (1 << 14)
#define EAR_RES_SECOND_RANGE0_MATCH         (1 << 13)
#define EAR_RES_FIRST_RANGE1_MATCH          (1 << 12)
#define EAR_RES_FIRST_RANGE0_MATCH          (1 << 11)
#define EAR_RES_FLOW_DONE                   (1 << 10)
#define EAR_KEY_DETECT                      (1 << 9)
#define EAR_PLUG_EDGE_SEL                   (1 << 8)
#define POWER_ON_FEEDIN                     (1 << 7)
#define EAR_PLUG_DETECT_SEL                 (1 << 6)
#define EAR_PLUG_DETECT_R                   (1 << 5)
#define EAR_PLUG_DETECT_L                   (1 << 4)
#define GPADC_SAMPLE_PERIOD_DONE_CHANGE     (1 << 3)
#define SAR_OUTN                            (1 << 2)
#define KEY_INT_OUT                         (1 << 1)

// REG_79
#define EAR_DETECT_FLOW_DONE                (1 << 15)
#define EAR_RES_STATE_SHIFT                 10
#define EAR_RES_STATE_MASK                  (0x1F << EAR_RES_STATE_SHIFT)
#define EAR_RES_STATE(n)                    BITFIELD_VAL(EAR_RES_STATE, n)
#define EAR_RES_FIRST_DATA_SHIFT            0
#define EAR_RES_FIRST_DATA_MASK             (0x3FF << EAR_RES_FIRST_DATA_SHIFT)
#define EAR_RES_FIRST_DATA(n)               BITFIELD_VAL(EAR_RES_FIRST_DATA, n)

// REG_7A
#define EAR_DETECT_FOUR_PORT_B              (1 << 15)
#define EAR_DETECT_FOUR_PORT_A              (1 << 14)
#define EAR_DETECT_MIC_B_ON                 (1 << 13)
#define EAR_DETECT_MIC_A_ON                 (1 << 12)
#define EAR_DETECT_GND_B_ON                 (1 << 11)
#define EAR_DETECT_GND_A_ON                 (1 << 10)
#define EAR_RES_SECOND_DATA_SHIFT           0
#define EAR_RES_SECOND_DATA_MASK            (0x3FF << EAR_RES_SECOND_DATA_SHIFT)
#define EAR_RES_SECOND_DATA(n)              BITFIELD_VAL(EAR_RES_SECOND_DATA, n)

// REG_7B
#define EAR_DETECT_MIC_STATE_SHIFT          12
#define EAR_DETECT_MIC_STATE_MASK           (0xF << EAR_DETECT_MIC_STATE_SHIFT)
#define EAR_DETECT_MIC_STATE(n)             BITFIELD_VAL(EAR_DETECT_MIC_STATE, n)
#define EAR_DETECT_GND_STATE_SHIFT          8
#define EAR_DETECT_GND_STATE_MASK           (0xF << EAR_DETECT_GND_STATE_SHIFT)
#define EAR_DETECT_GND_STATE(n)             BITFIELD_VAL(EAR_DETECT_GND_STATE, n)
#define EAR_DETECT_SAMPLE_STATE_SHIFT       4
#define EAR_DETECT_SAMPLE_STATE_MASK        (0xF << EAR_DETECT_SAMPLE_STATE_SHIFT)
#define EAR_DETECT_SAMPLE_STATE(n)          BITFIELD_VAL(EAR_DETECT_SAMPLE_STATE, n)
#define EAR_DETECT_MAIN_STATE_SHIFT         0
#define EAR_DETECT_MAIN_STATE_MASK          (0xF << EAR_DETECT_MAIN_STATE_SHIFT)
#define EAR_DETECT_MAIN_STATE(n)            BITFIELD_VAL(EAR_DETECT_MAIN_STATE, n)

// REG_7C
#define EAR_KEY_CHANGE_INTR                 (1 << 14)
#define EAR_PLUG_CHANGE_INTR                (1 << 13)
#define POWER_INTR                          (1 << 12)
#define GPADC_CHAN_DATA_INTR_SHIFT          4
#define GPADC_CHAN_DATA_INTR_MASK           (0xFF << GPADC_CHAN_DATA_INTR_SHIFT)
#define GPADC_CHAN_DATA_INTR(n)             BITFIELD_VAL(GPADC_CHAN_DATA_INTR, n)
#define GPADC_SAMPLE_DONE_INTR              (1 << 3)
#define EAR_DETECT_FLOW_DONE_INTR           (1 << 2)
#define EAR_RES_FLOW_DONE_INTR              (1 << 1)

// REG_7D
#define EAR_DETECT_A_DATA_L_SHIFT           0
#define EAR_DETECT_A_DATA_L_MASK            (0x3FF << EAR_DETECT_A_DATA_L_SHIFT)
#define EAR_DETECT_A_DATA_L(n)              BITFIELD_VAL(EAR_DETECT_A_DATA_L, n)

// REG_7E
#define EAR_DETECT_B_DATA_L_SHIFT           0
#define EAR_DETECT_B_DATA_L_MASK            (0x3FF << EAR_DETECT_B_DATA_L_SHIFT)
#define EAR_DETECT_B_DATA_L(n)              BITFIELD_VAL(EAR_DETECT_B_DATA_L, n)

// REG_80
#define EAR_DETECT_A_DATA_R_SHIFT           0
#define EAR_DETECT_A_DATA_R_MASK            (0x3FF << EAR_DETECT_A_DATA_R_SHIFT)
#define EAR_DETECT_A_DATA_R(n)              BITFIELD_VAL(EAR_DETECT_A_DATA_R, n)

// REG_81
#define EAR_DETECT_B_DATA_R_SHIFT           0
#define EAR_DETECT_B_DATA_R_MASK            (0x3FF << EAR_DETECT_B_DATA_R_SHIFT)
#define EAR_DETECT_B_DATA_R(n)              BITFIELD_VAL(EAR_DETECT_B_DATA_R, n)

// REG_82
#define EAR_DETECT_A_DATA_MIC_SHIFT         0
#define EAR_DETECT_A_DATA_MIC_MASK          (0x3FF << EAR_DETECT_A_DATA_MIC_SHIFT)
#define EAR_DETECT_A_DATA_MIC(n)            BITFIELD_VAL(EAR_DETECT_A_DATA_MIC, n)

// REG_83
#define EAR_DETECT_B_DATA_MIC_SHIFT         0
#define EAR_DETECT_B_DATA_MIC_MASK          (0x3FF << EAR_DETECT_B_DATA_MIC_SHIFT)
#define EAR_DETECT_B_DATA_MIC(n)            BITFIELD_VAL(EAR_DETECT_B_DATA_MIC, n)

// REG_84
#define EAR_DETECT_THREE_PORT               (1 << 14)
#define EAR_DETECT_PORT_ERROR               (1 << 13)
#define CNT_F_SHIFT                         1
#define CNT_F_MASK                          (0xFFF << CNT_F_SHIFT)
#define CNT_F(n)                            BITFIELD_VAL(CNT_F, n)
#define DONE_CALIB                          (1 << 0)

// REG_85
#define REG_COARSE_TUN_CODE_SHIFT           10
#define REG_COARSE_TUN_CODE_MASK            (0x3F << REG_COARSE_TUN_CODE_SHIFT)
#define REG_COARSE_TUN_CODE(n)              BITFIELD_VAL(REG_COARSE_TUN_CODE, n)
#define REG_CLK_GOAL_DIV_NUM_SHIFT          1
#define REG_CLK_GOAL_DIV_NUM_MASK           (0x1FF << REG_CLK_GOAL_DIV_NUM_SHIFT)
#define REG_CLK_GOAL_DIV_NUM(n)             BITFIELD_VAL(REG_CLK_GOAL_DIV_NUM, n)
#define REG_XTAL_FREQ_LLC_SFT_RSTN          (1 << 0)

// REG_86
#define REG_CNT_COARSE_MARK_15_0_SHIFT      0
#define REG_CNT_COARSE_MARK_15_0_MASK       (0xFFFF << REG_CNT_COARSE_MARK_15_0_SHIFT)
#define REG_CNT_COARSE_MARK_15_0(n)         BITFIELD_VAL(REG_CNT_COARSE_MARK_15_0, n)

// REG_87
#define REG_BUCK_I_DELTAV_X2                (1 << 15)
#define REG_BUCK_EN_ZCD_CAL                 (1 << 14)
#define REG_BUCK_BURST_RES                  (1 << 13)
#define REG_BUCK_BIAS_HALF                  (1 << 12)
#define REG_COARSE_POLAR_SEL                (1 << 11)
#define REG_COARSE_EN                       (1 << 10)
#define REG_COARSE_TUN_CODE_DR              (1 << 9)
#define REG_CNT_COARSE_MARK_24_16_SHIFT     0
#define REG_CNT_COARSE_MARK_24_16_MASK      (0x1FF << REG_CNT_COARSE_MARK_24_16_SHIFT)
#define REG_CNT_COARSE_MARK_24_16(n)        BITFIELD_VAL(REG_CNT_COARSE_MARK_24_16, n)

// REG_88
#define REG_BUCK_ZCD_CAP_BIT_SHIFT          12
#define REG_BUCK_ZCD_CAP_BIT_MASK           (0xF << REG_BUCK_ZCD_CAP_BIT_SHIFT)
#define REG_BUCK_ZCD_CAP_BIT(n)             BITFIELD_VAL(REG_BUCK_ZCD_CAP_BIT, n)
#define REG_BUCK_EDGE_CON_SHIFT             8
#define REG_BUCK_EDGE_CON_MASK              (0xF << REG_BUCK_EDGE_CON_SHIFT)
#define REG_BUCK_EDGE_CON(n)                BITFIELD_VAL(REG_BUCK_EDGE_CON, n)
#define REG_BUCK_CC_CAP_BIT2_SHIFT          6
#define REG_BUCK_CC_CAP_BIT2_MASK           (0x3 << REG_BUCK_CC_CAP_BIT2_SHIFT)
#define REG_BUCK_CC_CAP_BIT2(n)             BITFIELD_VAL(REG_BUCK_CC_CAP_BIT2, n)
#define REG_BUCK_CAL_DELTAV_SHIFT           0
#define REG_BUCK_CAL_DELTAV_MASK            (0x3F << REG_BUCK_CAL_DELTAV_SHIFT)
#define REG_BUCK_CAL_DELTAV(n)              BITFIELD_VAL(REG_BUCK_CAL_DELTAV, n)

// REG_89
#define REG_CP_AUX_PU                       (1 << 14)
#define REG_CP_AUX_PU_DR                    (1 << 13)
#define REG_CP_AUX_NONOV_BIT_SHIFT          11
#define REG_CP_AUX_NONOV_BIT_MASK           (0x3 << REG_CP_AUX_NONOV_BIT_SHIFT)
#define REG_CP_AUX_NONOV_BIT(n)             BITFIELD_VAL(REG_CP_AUX_NONOV_BIT, n)
#define REG_XTAL_FREQ_LLC_RST               (1 << 9)
#define REG_CLK_4M_PWR_BIT_SHIFT            7
#define REG_CLK_4M_PWR_BIT_MASK             (0x3 << REG_CLK_4M_PWR_BIT_SHIFT)
#define REG_CLK_4M_PWR_BIT(n)               BITFIELD_VAL(REG_CLK_4M_PWR_BIT, n)
#define REG_CLK_4M_EN_AGPIO_TEST            (1 << 6)
#define REG_BUCK_SLOPE_HALF_ENB             (1 << 5)
#define REG_BUCK_LP_ERR                     (1 << 4)
#define REG_BUCK_LP_VCOMP2_SHIFT            1
#define REG_BUCK_LP_VCOMP2_MASK             (0x3 << REG_BUCK_LP_VCOMP2_SHIFT)
#define REG_BUCK_LP_VCOMP2(n)               BITFIELD_VAL(REG_BUCK_LP_VCOMP2, n)
#define REG_BUCK_LP_VCOMP                   (1 << 0)

// REG_8A
#define EAR_LP_EN_REG_SW_MIC                (1 << 15)
#define EAR_LP_EN_REG_SW_GND                (1 << 14)
#define EAR_DET_LDO_MIC_VSEL_SHIFT          10
#define EAR_DET_LDO_MIC_VSEL_MASK           (0xF << EAR_DET_LDO_MIC_VSEL_SHIFT)
#define EAR_DET_LDO_MIC_VSEL(n)             BITFIELD_VAL(EAR_DET_LDO_MIC_VSEL, n)
#define EAR_DET_LDO_GND_VSEL_SHIFT          6
#define EAR_DET_LDO_GND_VSEL_MASK           (0xF << EAR_DET_LDO_GND_VSEL_SHIFT)
#define EAR_DET_LDO_GND_VSEL(n)             BITFIELD_VAL(EAR_DET_LDO_GND_VSEL, n)
#define REG_CP_AUX_CLK_DIV_SHIFT            0
#define REG_CP_AUX_CLK_DIV_MASK             (0x3F << REG_CP_AUX_CLK_DIV_SHIFT)
#define REG_CP_AUX_CLK_DIV(n)               BITFIELD_VAL(REG_CP_AUX_CLK_DIV, n)

// REG_8B
#define REG_MIC_BIASA_EN                    (1 << 15)
#define REG_MIC_BIASA_EN_DR                 (1 << 14)
#define REG_MIC_LDO_EN                      (1 << 13)
#define REG_MIC_LDO_EN_DR                   (1 << 12)
#define REG_MICBIASA_PULL_DOWN_DLY_SHIFT    6
#define REG_MICBIASA_PULL_DOWN_DLY_MASK     (0x3F << REG_MICBIASA_PULL_DOWN_DLY_SHIFT)
#define REG_MICBIASA_PULL_DOWN_DLY(n)       BITFIELD_VAL(REG_MICBIASA_PULL_DOWN_DLY, n)
#define REG_MICBIASA_LPFSEL_DLY_SHIFT       0
#define REG_MICBIASA_LPFSEL_DLY_MASK        (0x3F << REG_MICBIASA_LPFSEL_DLY_SHIFT)
#define REG_MICBIASA_LPFSEL_DLY(n)          BITFIELD_VAL(REG_MICBIASA_LPFSEL_DLY, n)

// REG_8C
#define REG_PU_LDO_FM                       (1 << 15)
#define REG_PU_LDO_FM_DR                    (1 << 14)
#define REG_PU_LDO_FM_DSLEEP                (1 << 13)
#define RESETN_FM_REG                       (1 << 12)
#define RESETN_FM_DR                        (1 << 11)
#define REG_SAR_AUTO_CAL                    (1 << 10)
#define REG_BYPASS_VANA2VFM                 (1 << 9)
#define REG_PULLDOWN_VANA2VFM               (1 << 8)
#define REG_MIC_BIASB_LPFSEL_SHIFT          5
#define REG_MIC_BIASB_LPFSEL_MASK           (0x7 << REG_MIC_BIASB_LPFSEL_SHIFT)
#define REG_MIC_BIASB_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASB_LPFSEL, n)
#define REG_CLK_OSC_PMU_DIV_SHIFT           0
#define REG_CLK_OSC_PMU_DIV_MASK            (0x1F << REG_CLK_OSC_PMU_DIV_SHIFT)
#define REG_CLK_OSC_PMU_DIV(n)              BITFIELD_VAL(REG_CLK_OSC_PMU_DIV, n)

// REG_8D
#define REG_MICBIASA_PULL_DOWN              (1 << 14)
#define REG_MICBIASA_PULL_DOWN_DR           (1 << 13)
#define RESETN_DCO_REG                      (1 << 12)
#define RESETN_DCO_DR                       (1 << 11)
#define LP_EN_FM_LDO_REG                    (1 << 10)
#define LP_EN_FM_LDO_DR                     (1 << 9)
#define LP_EN_FM_LDO_DSLEEP                 (1 << 8)
#define LDO_FM_VBIT_DSLEEP_SHIFT            4
#define LDO_FM_VBIT_DSLEEP_MASK             (0xF << LDO_FM_VBIT_DSLEEP_SHIFT)
#define LDO_FM_VBIT_DSLEEP(n)               BITFIELD_VAL(LDO_FM_VBIT_DSLEEP, n)
#define LDO_FM_VBIT_NORMAL_SHIFT            0
#define LDO_FM_VBIT_NORMAL_MASK             (0xF << LDO_FM_VBIT_NORMAL_SHIFT)
#define LDO_FM_VBIT_NORMAL(n)               BITFIELD_VAL(LDO_FM_VBIT_NORMAL, n)

// REG_8E
#define REG_MIC_LDO_PULLDOWN                (1 << 14)
#define REG_MIC_LDO_PULLDOWN_DR             (1 << 13)
#define CLK_4M_MUX_SEL_CLK_SAR_SHIFT        10
#define CLK_4M_MUX_SEL_CLK_SAR_MASK         (0x7 << CLK_4M_MUX_SEL_CLK_SAR_SHIFT)
#define CLK_4M_MUX_SEL_CLK_SAR(n)           BITFIELD_VAL(CLK_4M_MUX_SEL_CLK_SAR, n)
#define CLK_4M_EN_CLK_SAR_ADC_OUT           (1 << 9)
#define CLK_4M_EN_CLK_CP_OUT                (1 << 8)
#define POWER_UP_MOD7_CNT_SHIFT             0
#define POWER_UP_MOD7_CNT_MASK              (0xFF << POWER_UP_MOD7_CNT_SHIFT)
#define POWER_UP_MOD7_CNT(n)                BITFIELD_VAL(POWER_UP_MOD7_CNT, n)


enum PMU_REG_T {
    PMU_REG_METAL_ID            = 0x00,
    PMU_REG_POWER_KEY_CFG       = 0x02,
    PMU_REG_BIAS_CFG            = 0x03,
    PMU_REG_CHARGER_CFG         = 0x05,
    PMU_REG_ANA_CFG             = 0x07,
    PMU_REG_MODULE_START        = PMU_REG_ANA_CFG,
    PMU_REG_DIG_CFG             = 0x08,
    PMU_REG_IO_CFG              = 0x09,
    PMU_REG_USB_CFG             = 0x0B,
    PMU_REG_CRYSTAL_CFG         = 0x0C,
    PMU_REG_CODEC_CFG           = 0x0E,
    PMU_REG_DCDC_DIG_VOLT       = 0x13,
    PMU_REG_DCDC_CFG            = 0x14,
    PMU_REG_BUCK_FREQ           = 0x15,
    PMU_REG_PWR_SEL             = 0x1A,
    PMU_REG_SLEEP_CFG           = 0x1D,
    PMU_REG_INT_CTRL            = 0x1F,
    PMU_REG_USBPHY              = 0x20,
    PMU_REG_MIC_BIAS_A          = 0x21,
    PMU_REG_RTC_DIV_1HZ         = 0x22,
    PMU_REG_RTC_LOAD_LOW        = 0x23,
    PMU_REG_RTC_LOAD_HIGH       = 0x24,
    PMU_REG_RTC_MATCH0_LOW      = 0x25,
    PMU_REG_RTC_MATCH0_HIGH     = 0x26,
    PMU_REG_RTC_MATCH1_LOW      = 0x27,
    PMU_REG_RTC_MATCH1_HIGH     = 0x28,
    PMU_REG_MIC_BIAS_B          = 0x29,
    PMU_REG_LED_CFG             = 0x2A,
    PMU_REG_USB11_CFG           = 0x2B,
    PMU_REG_EFUSE_CTRL          = 0x30,
    PMU_REG_EFUSE_PGM_CTRL      = 0x31,
    PMU_REG_EFUSE_AVDD_COUNTER  = 0x32,
    PMU_REG_EFUSE_READ_TRIG     = 0x33,
    PMU_REG_EFUSE2_CTRL         = 0x3E,
    PMU_REG_EFUSE2_VAL          = 0x3F,
    PMU_REG_DCDC_ANA_VOLT       = 0x47,
    PMU_REG_POWER_OFF           = 0x49,
    PMU_REG_INTR_MSKED          = 0x4A,
    PMU_REG_INT_STATUS          = 0x4C,
    PMU_REG_INT_CLR             = 0x4D,
    PMU_REG_EFUSE_VAL           = 0x4E,
    PMU_REG_RTC_VAL_LOW         = 0x4F,
    PMU_REG_RTC_VAL_HIGH        = 0x50,
    PMU_REG_DBG_STATUS0         = 0x59,
    PMU_REG_DBG_STATUS1         = 0x5A,
    PMU_REG_MICBIAS_CTRL        = 0x8B,
};

#endif // _REG_PMU_BEST3003_H_
