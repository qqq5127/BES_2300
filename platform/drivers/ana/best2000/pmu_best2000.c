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
#include "cmsis_nvic.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "pmu.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_chipid.h"
#include "hal_location.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_spi.h"
#include "hal_sysfreq.h"
#include "tgt_hardware.h"

// Avoid GPIO out of control during vcore LDO soft start interval
#define VCORE_LDO_ON

// LDO soft start interval is about 1000 us
#define PMU_LDO_PU_STABLE_TIME_US       1800
#define PMU_DCDC_PU_STABLE_TIME_US      100
#define PMU_VANA_STABLE_TIME_US         10
#define PMU_VCORE_STABLE_TIME_US        10

#ifdef __PMU_VIO_DYNAMIC_CTRL_MODE__
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_2_2V
#else
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_2_6V
#endif
#ifdef DIGMIC_HIGH_VOLT
#define IO_VOLT_ACTIVE_RISE             PMU_IO_3_0V
#else
#define IO_VOLT_ACTIVE_RISE             PMU_IO_2_6V
#endif
#define IO_VOLT_SLEEP                   PMU_IO_2_6V

#define PMU_DCDC_ANA_2_3V               0xF
#define PMU_DCDC_ANA_2_2V               0xE
#define PMU_DCDC_ANA_2_1V               0xD
#define PMU_DCDC_ANA_2_0V               0xC
#define PMU_DCDC_ANA_1_9V               0xB
#define PMU_DCDC_ANA_1_8V               0xA
#define PMU_DCDC_ANA_1_7V               0x9
#define PMU_DCDC_ANA_1_6V               0x8
#define PMU_DCDC_ANA_1_5V               0x7
#define PMU_DCDC_ANA_1_4V               0x6

#define PMU_DCDC_ANA_SLEEP_1_6V         0xF
#define PMU_DCDC_ANA_SLEEP_1_5V         0xE
#define PMU_DCDC_ANA_SLEEP_1_4V         0xD
#define PMU_DCDC_ANA_SLEEP_1_3V         0xC
#define PMU_DCDC_ANA_SLEEP_1_2V         0xA
#define PMU_DCDC_ANA_SLEEP_1_1V         0x8

#define PMU_DCDC_ANA2_FLAG              (1 << 7)

enum PMU_DCDC_ANA2_VOLT_T {
    PMU_DCDC_ANA2_2_2V = 0x8 | PMU_DCDC_ANA2_FLAG,
    PMU_DCDC_ANA2_2_4V,
    PMU_DCDC_ANA2_2_6V,
    PMU_DCDC_ANA2_2_7V,
    PMU_DCDC_ANA2_2_9V,
    PMU_DCDC_ANA2_3_1V,
    PMU_DCDC_ANA2_3_3V,
    PMU_DCDC_ANA2_3_5V,
};

enum PMU_DCDC_ANA2_SLEEP_VOLT_T {
    PMU_DCDC_ANA2_SLEEP_1_5V = 0x8 | PMU_DCDC_ANA2_FLAG,
    PMU_DCDC_ANA2_SLEEP_1_xV,
    PMU_DCDC_ANA2_SLEEP_1_7V,
    PMU_DCDC_ANA2_SLEEP_1_8V,
    PMU_DCDC_ANA2_SLEEP_2_0V,
    PMU_DCDC_ANA2_SLEEP_2_1V,
    PMU_DCDC_ANA2_SLEEP_2_2V,
    PMU_DCDC_ANA2_SLEEP_2_5V,
};

#define PMU_DCDC_DIG_1_4V               0x6
#define PMU_DCDC_DIG_1_3V               0x5
#define PMU_DCDC_DIG_1_2V               0x4
#define PMU_DCDC_DIG_1_1V               0x3
#define PMU_DCDC_DIG_1_0V               0x2
#define PMU_DCDC_DIG_0_9V               0x1
#define PMU_DCDC_DIG_0_8V               0x0

#ifdef PMU_DCDC_DIG2_SUPPORT
#define PMU_DCDC_DIG2_FLAG              (1 << 7)

#define PMU_DCDC_DIG2_1_25V             (0x8 | PMU_DCDC_DIG2_FLAG)
#define PMU_DCDC_DIG2_1_15V             (0x6 | PMU_DCDC_DIG2_FLAG)
#define PMU_DCDC_DIG2_1_05V             (0x5 | PMU_DCDC_DIG2_FLAG)
#endif

#define PMU_VDIG_1_35V                  0xF
#define PMU_VDIG_1_3V                   0xE
#define PMU_VDIG_1_25V                  0xD
#define PMU_VDIG_1_2V                   0xC
#define PMU_VDIG_1_15V                  0xB
#define PMU_VDIG_1_1V                   0xA
#define PMU_VDIG_1_05V                  0x9
#define PMU_VDIG_1_0V                   0x8
#define PMU_VDIG_0_95V                  0x7
#define PMU_VDIG_0_9V                   0x6
#define PMU_VDIG_0_85V                  0x5
#define PMU_VDIG_0_8V                   0x4
#define PMU_VDIG_0_75V                  0x3
#define PMU_VDIG_0_7V                   0x2
#define PMU_VDIG_0_65V                  0x1
#define PMU_VDIG_0_6V                   0x0

#define PMU_IO_3_3V                     0x15
#define PMU_IO_3_2V                     0x14
#define PMU_IO_3_1V                     0x13
#define PMU_IO_3_0V                     0x12
#define PMU_IO_2_9V                     0x11
#define PMU_IO_2_8V                     0x10
#define PMU_IO_2_7V                     0xF
#define PMU_IO_2_6V                     0xE
#define PMU_IO_2_5V                     0xD
#define PMU_IO_2_4V                     0xC
#define PMU_IO_2_3V                     0xB
#define PMU_IO_2_2V                     0xA
#define PMU_IO_2_1V                     0x9
#define PMU_IO_2_0V                     0x8
#define PMU_IO_1_9V                     0x7
#define PMU_IO_1_8V                     0x6
#define PMU_IO_1_7V                     0x5

#define PMU_VMEM_2_8V                   0x10
#define PMU_VMEM_2_0V                   0x8
#define PMU_VMEM_1_9V                   0x7
#define PMU_VMEM_1_8V                   0x6

#define PMU_CODEC_2_7V                  0xF
#define PMU_CODEC_2_6V                  0xE
#define PMU_CODEC_2_5V                  0xD
#define PMU_CODEC_2_4V                  0xC
#define PMU_CODEC_2_3V                  0xB
#define PMU_CODEC_2_2V                  0xA
#define PMU_CODEC_2_1V                  0x9
#define PMU_CODEC_2_0V                  0x8
#define PMU_CODEC_1_9V                  0x7
#define PMU_CODEC_1_8V                  0x6
#define PMU_CODEC_1_7V                  0x5
#define PMU_CODEC_1_6V                  0x4
#define PMU_CODEC_1_5V                  0x3

#define PMU_USB_3_3V                    0xC
#define PMU_USB_3_2V                    0xB
#define PMU_USB_3_1V                    0xA
#define PMU_USB_3_0V                    0x9
#define PMU_USB_2_9V                    0x8
#define PMU_USB_2_8V                    0x7
#define PMU_USB_2_7V                    0x6
#define PMU_USB_2_6V                    0x5
#define PMU_USB_2_5V                    0x4
#define PMU_USB_2_4V                    0x3

// 00   PMU_REG_METAL_ID
#define PMU_METAL_ID_SHIFT              0
#define PMU_METAL_ID_MASK               (0xF << PMU_METAL_ID_SHIFT)
#define PMU_METAL_ID(n)                 (((n) & 0xF) << PMU_METAL_ID_SHIFT)

// 02   PMU_REG_POWER_KEY_CFG
#define PMU_PU_LPO_DR                   (1 << 14)
#define PMU_PU_LPO_REG                  (1 << 13)
#define PMU_POWERKEY_WAKEUP_OSC_EN      (1 << 12)
#define PMU_RTC_POWER_ON_EN             (1 << 11)

// 05   PMU_REG_CHARGER_CFG
#define PMU_CHARGE_OUT_INTR_MSK         (1 << 12)
#define PMU_CHARGE_IN_INTR_MSK          (1 << 11)
#define PMU_AC_ON_OUT_EN                (1 << 10)
#define PMU_AC_ON_IN_EN                 (1 << 9)
#define PMU_CHARGE_INTR_EN              (1 << 8)
#define PMU_AC_ON_DB_VALUE_SHIFT        0
#define PMU_AC_ON_DB_VALUE_MASK         (0xFF << PMU_AC_ON_DB_VALUE_SHIFT)
#define PMU_AC_ON_DB_VALUE(n)           BITFIELD_VAL(PMU_AC_ON_DB_VALUE, n)

// 06   PMU_REG_USB11_CFG
#define PMU_PU_USB11_DR                 (1 << 11)
#define PMU_PU_USB11_REG                (1 << 10)

// 07   ana setting
#define PMU_ANA_LP_MODE_EN              (1<<13)
#define PMU_ANA_LP_EN_DR                (1<<12)
#define PMU_ANA_LP_EN_REG               (1<<11)
#define PMU_ANA_DSLEEP_ON               (1<<10)
#define PMU_ANA_VANA_DR                 (1<<9)
#define PMU_ANA_VANA_EN                 (1<<8)
#define PMU_ANA_DSLEEP_VOLT(n)          (((n) & 0xF) << 4)
#define PMU_ANA_DSLEEP_VOLT_MASK        (0xF << 4)
#define PMU_ANA_DSLEEP_VOLT_SHIFT       (4)
#define PMU_ANA_NORMAL_VOLT(n)          (((n) & 0xF) << 0)
#define PMU_ANA_NORMAL_VOLT_MASK        (0xF << 0)
#define PMU_ANA_NORMAL_VOLT_SHIFT       (0)

// 08   dig setting
#define PMU_DIG_LP_MODE_EN              (1<<13)
#define PMU_DIG_LP_EN_DR                (1<<12)
#define PMU_DIG_LP_EN_REG               (1<<11)
#define PMU_DIG_DSLEEP_ON               (1<<10)
#define PMU_DIG_VDIG_DR                 (1<<9)
#define PMU_DIG_VDIG_EN                 (1<<8)
#define PMU_DIG_DSLEEP_VOLT(n)          (((n) & 0xF) << 4)
#define PMU_DIG_DSLEEP_VOLT_MASK        (0xF << 4)
#define PMU_DIG_DSLEEP_VOLT_SHIFT       (4)
#define PMU_DIG_NORMAL_VOLT(n)          (((n) & 0xF) << 0)
#define PMU_DIG_NORMAL_VOLT_MASK        (0xF << 0)
#define PMU_DIG_NORMAL_VOLT_SHIFT       (0)

// 09   IO SETTING
#define PMU_IO_LP_MODE_EN               (1<<15)
#define PMU_IO_LP_EN_DR                 (1<<14)
#define PMU_IO_LP_EN_REG                (1<<13)
#define PMU_IO_DSLEEP_ON                (1<<12)
#define PMU_IO_VIO_DR                   (1<<11)
#define PMU_IO_VIO_EN                   (1<<10)
#define PMU_IO_NORMAL_VOLT(n)           (((n) & 0x1F) << 5)
#define PMU_IO_NORMAL_VOLT_MASK         (0x1F << 5)
#define PMU_IO_NORMAL_VOLT_SHIFT        (5)
#define PMU_IO_DSLEEP_VOLT(n)           (((n) & 0x1F) << 0)
#define PMU_IO_DSLEEP_VOLT_MASK         (0x1F << 0)
#define PMU_IO_DSLEEP_VOLT_SHIFT        (0)

// 0A   VMEM SETTING
#define PMU_MEM_LP_MODE_EN              (1<<15)
#define PMU_MEM_LP_EN_DR                (1<<14)
#define PMU_MEM_LP_EN_REG               (1<<13)
#define PMU_MEM_DSLEEP_ON               (1<<12)
#define PMU_MEM_VMEM_DR                 (1<<11)
#define PMU_MEM_VMEM_EN                 (1<<10)
#define PMU_MEM_NORMAL_VOLT(n)          (((n) & 0x1F) << 5)
#define PMU_MEM_NORMAL_VOLT_MASK        (0x1F << 5)
#define PMU_MEM_NORMAL_VOLT_SHIFT       (5)
#define PMU_MEM_DSLEEP_VOLT(n)          (((n) & 0x1F) << 0)
#define PMU_MEM_DSLEEP_VOLT_MASK        (0x1F << 0)
#define PMU_MEM_DSLEEP_VOLT_SHIFT       (0)

// 0B   GP SETTING
#define PMU_GP_LP_MODE_EN               (1<<15)
#define PMU_GP_LP_EN_DR                 (1<<14)
#define PMU_GP_LP_EN_REG                (1<<13)
#define PMU_GP_DSLEEP_ON                (1<<12)
#define PMU_GP_VGP_DR                   (1<<11)
#define PMU_GP_VGP_EN                   (1<<10)
#define PMU_GP_NORMAL_VOLT(n)           (((n) & 0x1F) << 5)
#define PMU_GP_NORMAL_VOLT_MASK         (0x1F << 5)
#define PMU_GP_NORMAL_VOLT_SHIFT        (5)
#define PMU_GP_DSLEEP_VOLT(n)           (((n) & 0x1F) << 0)
#define PMU_GP_DSLEEP_VOLT_MASK         (0x1F << 0)
#define PMU_GP_DSLEEP_VOLT_SHIFT        (0)

// 0C   USB SETTING
#define PMU_USB_LP_MODE_EN              (1<<13)
#define PMU_USB_LP_EN_DR                (1<<12)
#define PMU_USB_LP_EN_REG               (1<<11)
#define PMU_USB_DSLEEP_ON               (1<<10)
#define PMU_USB_VUSB_DR                 (1<<9)
#define PMU_USB_VUSB_EN                 (1<<8)
#define PMU_USB_NORMAL_VOLT(n)          (((n) & 0xF) << 4)
#define PMU_USB_NORMAL_VOLT_MASK        (0xF << 4)
#define PMU_USB_NORMAL_VOLT_SHIFT       (4)
#define PMU_USB_DSLEEP_VOLT(n)          (((n) & 0xF) << 0)
#define PMU_USB_DSLEEP_VOLT_MASK        (0xF << 0)
#define PMU_USB_DSLEEP_VOLT_SHIFT       (0)

// 0D   CRYSTAL SETTING
#define PMU_CRYSTAL_DSLEEP_ON           (1<<13)
#define PMU_CRYSTAL_VCRYSTAL_DR         (1<<12)
#define PMU_CRYSTAL_VCRYSTAL_EN         (1<<11)
#define PMU_CRYSTAL_LP_MODE_EN          (1<<10)
#define PMU_CRYSTAL_LP_EN_DR            (1<<9)
#define PMU_CRYSTAL_LP_EN_REG           (1<<8)
#define PMU_CRYSTAL_NORMAL_VOLT(n)      (((n) & 0x7) << 4)
#define PMU_CRYSTAL_NORMAL_VOLT_MASK    (0x7 << 4)
#define PMU_CRYSTAL_NORMAL_VOLT_SHIFT   (3)
#define PMU_CRYSTAL_DSLEEP_VOLT(n)      (((n) & 0x7) << 0)
#define PMU_CRYSTAL_DSLEEP_VOLT_MASK    (0x7 << 0)
#define PMU_CRYSTAL_DSLEEP_VOLT_SHIFT   (0)

// 10   CODEC SETTING
#define PMU_CODEC_DSLEEP_ON             (1<<14)
#define PMU_CODEC_VCODEC_DR             (1<<13)
#define PMU_CODEC_VCODEC_EN             (1<<12)
#define PMU_CODEC_LP_MODE_EN            (1<<11)
#define PMU_CODEC_LP_EN_DR              (1<<10)
#define PMU_CODEC_LP_EN_REG             (1<<9)
#define PMU_CODEC_PULLDOWN_VCODEC       (1 << 8)
#define PMU_CODEC_NORMAL_VOLT(n)        (((n) & 0xF) << 4)
#define PMU_CODEC_NORMAL_VOLT_MASK      (0xF << 4)
#define PMU_CODEC_NORMAL_VOLT_SHIFT     (4)
#define PMU_CODEC_DSLEEP_VOLT(n)        (((n) & 0xF) << 0)
#define PMU_CODEC_DSLEEP_VOLT_MASK      (0xF << 0)
#define PMU_CODEC_DSLEEP_VOLT_SHIFT     (0)

// 15   BUCK VOLT SETTING
#define PMU_BUCK_VCORE_NORMAL(n)        (((n) & 0xF) << 12)
#define PMU_BUCK_VCORE_NORMAL_MASK      (0xF << 12)
#define PMU_BUCK_VCORE_NORMAL_SHIFT     (12)
#define PMU_BUCK_VCORE_DSLEEP(n)        (((n) & 0xF) << 8)
#define PMU_BUCK_VCORE_DSLEEP_MASK      (0xF << 8)
#define PMU_BUCK_VCORE_DSLEEP_SHIFT     (8)
#define PMU_BUCK_VANA_NORMAL(n)         (((n) & 0xF) << 4)
#define PMU_BUCK_VANA_NORMAL_MASK       (0xF << 4)
#define PMU_BUCK_VANA_NORMAL_SHIFT      (4)
#define PMU_BUCK_VANA_DSLEEP(n)         (((n) & 0xF) << 0)
#define PMU_BUCK_VANA_DSLEEP_MASK       (0xF << 0)
#define PMU_BUCK_VANA_DSLEEP_SHIFT      (0)

// 16   DCDC SETTING
#define PMU_TEST_MODE_SHIFT             13
#define PMU_TEST_MODE_MASK              (0x7 << PMU_TEST_MODE_SHIFT)
#define PMU_TEST_MODE(n)                BITFIELD_VAL(PMU_TEST_MODE, n)
#define PMU_BUCK_CC_MODE_EN             (1<<12)
#define PMU_DCDC_ANA_LP_MODE_EN         (1<<11)
#define PMU_DCDC_ANA_DR                 (1<<10)
#define PMU_DCDC_ANA_EN                 (1<<9)
#define PMU_DCDC_ANA_DSLEEP_ON          (1<<6)
#define PMU_DCDC_DIG_LP_MODE_EN         (1<<5)
#define PMU_DCDC_DIG_DR                 (1<<4)
#define PMU_DCDC_DIG_EN                 (1<<3)
#define PMU_DCDC_DIG_DSLEEP_ON          (1<<0)

// 19   PMU_REG_BUCK_VCOMP
#define PMU_BUCK_HV_ANA_NORMAL          (1 << 5)
#define PMU_BUCK_HV_ANA_DSLEEP          (1 << 4)

// 1E   PMU_REG_THERM
#define REG_SS_VCORE_EN                     (1 << 15)
#define REG_PMU_VSEL2_SHIFT                 12
#define REG_PMU_VSEL2_MASK                  (0x7 << REG_PMU_VSEL2_SHIFT)
#define REG_PMU_VSEL2(n)                    BITFIELD_VAL(REG_PMU_VSEL2, n)
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

// 21   PMU_REG_PWR_SEL
#define PMU_PMU_VSEL1_SHIFT             13
#define PMU_PMU_VSEL1_MASK              (0x7 << PMU_PMU_VSEL1_SHIFT)
#define PMU_PMU_VSEL1(n)                BITFIELD_VAL(PMU_PMU_VSEL1, n)
#define PMU_POWER_SEL_CNT_SHIFT         8
#define PMU_POWER_SEL_CNT_MASK          (0x1F << PMU_POWER_SEL_CNT_SHIFT)
#define PMU_POWER_SEL_CNT(n)            BITFIELD_VAL(PMU_POWER_SEL_CNT, n)
#define PMU_PWR_SEL_DR                  (1 << 7)
#define PMU_PWR_SEL                     (1 << 6)
#define PMU_CLK_BG_EN                   (1 << 5)
#define PMU_CLK_BG_DIV_VALUE_SHIFT      0
#define PMU_CLK_BG_DIV_VALUE_MASK       (0x1F << PMU_CLK_BG_DIV_VALUE_SHIFT)
#define PMU_CLK_BG_DIV_VALUE(n)         BITFIELD_VAL(PMU_CLK_BG_DIV_VALUE, n)

// 24   SLEEP ENABLE
#define PMU_SLEEP_EN                    (1 << 15)

// 26   PMU_REG_INT_MASK
#define PMU_INT_MASK_USB_INSERT         (1 << 15)
#define PMU_INT_MASK_RTC1               (1 << 14)
#define PMU_INT_MASK_RTC0               (1 << 13)

// 27   PMU_REG_INT_EN
#define PMU_INT_EN_USB_INSERT           (1 << 15)
#define PMU_INT_EN_RTC1                 (1 << 14)
#define PMU_INT_EN_RTC0                 (1 << 13)

// 28   PMU_REG_USBPHY
#define PMU_EN_USBDIGPHY_CLK            (1 << 11)
#define PMU_RESETN_USBPHY_DR            (1 << 10)
#define PMU_RESETN_USBPHY_REG           (1 << 9)

// 39   PMU_REG_TRNG
#define RESERVED_DIG_27_26_SHIFT            14
#define RESERVED_DIG_27_26_MASK             (0x3 << RESERVED_DIG_27_26_SHIFT)
#define RESERVED_DIG_27_26(n)               BITFIELD_VAL(RESERVED_DIG_27_26, n)
#define REG_LED_IO1_RX_EN                   (1 << 13)
#define REG_LED_IO2_RX_EN                   (1 << 12)
#define REG_PU_TRNG                         (1 << 11)
#define REG_TRNG_CORE_SEL                   (1 << 10)
#define REG_TRNG_SEL_DIV                    (1 << 9)
#define REG_TRNG_SEL_TEST                   (1 << 8)
#define REG_TRNG_TEST_EN                    (1 << 7)
#define REG_TRNG_RES_SEL_SHIFT              4
#define REG_TRNG_RES_SEL_MASK               (0x7 << REG_TRNG_RES_SEL_SHIFT)
#define REG_TRNG_RES_SEL(n)                 BITFIELD_VAL(REG_TRNG_RES_SEL, n)
#define REG_TRNG_VSEL_DR                    (1 << 3)
#define REG_TRNG_VSEL_SHIFT                 0
#define REG_TRNG_VSEL_MASK                  (0x7 << REG_TRNG_VSEL_SHIFT)
#define REG_TRNG_VSEL(n)                    BITFIELD_VAL(REG_TRNG_VSEL, n)

// 3A   PMU_REG_LED_CFG
#define PMU_LED_IO1_IBIT_SHIFT          14
#define PMU_LED_IO1_IBIT_MASK           (0x3 << PMU_LED_IO1_IBIT_SHIFT)
#define PMU_LED_IO1_IBIT(n)             BITFIELD_VAL(PMU_LED_IO1_IBIT, n)
#define PMU_LED_IO1_OENB                (1 << 13)
#define PMU_LED_IO1_PDEN                (1 << 12)
#define PMU_LED_IO1_PU                  (1 << 11)
#define PMU_LED_IO1_PUEN                (1 << 10)
#define PMU_LED_IO1_SEL_SHIFT           8
#define PMU_LED_IO1_SEL_MASK            (0x3 << PMU_LED_IO1_SEL_SHIFT)
#define PMU_LED_IO1_SEL(n)              BITFIELD_VAL(PMU_LED_IO1_SEL, n)
#define PMU_LED_IO2_IBIT_SHIFT          6
#define PMU_LED_IO2_IBIT_MASK           (0x3 << PMU_LED_IO2_IBIT_SHIFT)
#define PMU_LED_IO2_IBIT(n)             BITFIELD_VAL(PMU_LED_IO2_IBIT, n)
#define PMU_LED_IO2_OENB                (1 << 5)
#define PMU_LED_IO2_PDEN                (1 << 4)
#define PMU_LED_IO2_PU                  (1 << 3)
#define PMU_LED_IO2_PUEN                (1 << 2)
#define PMU_LED_IO2_SEL_SHIFT           0
#define PMU_LED_IO2_SEL_MASK            (0x3 << PMU_LED_IO2_SEL_SHIFT)
#define PMU_LED_IO2_SEL(n)              BITFIELD_VAL(PMU_LED_IO2_SEL, n)

// 3B   PMU_REG_MIC_BIAS_A
#define PMU_MIC_BIASA_CHANSEL_SHIFT     14
#define PMU_MIC_BIASA_CHANSEL_MASK      (0x3 << PMU_MIC_BIASA_CHANSEL_SHIFT)
#define PMU_MIC_BIASA_CHANSEL(n)        BITFIELD_VAL(PMU_MIC_BIASA_CHANSEL, n)
#define PMU_MIC_BIASA_EN                (1 << 13)
#define PMU_MIC_BIASA_ENLPF             (1 << 12)
#define PMU_MIC_BIASA_LPFSEL_SHIFT      10
#define PMU_MIC_BIASA_LPFSEL_MASK       (0x3 << PMU_MIC_BIASA_LPFSEL_SHIFT)
#define PMU_MIC_BIASA_LPFSEL(n)         BITFIELD_VAL(PMU_MIC_BIASA_LPFSEL, n)
#define PMU_MIC_BIASA_VSEL_SHIFT        6
#define PMU_MIC_BIASA_VSEL_MASK         (0xF << PMU_MIC_BIASA_VSEL_SHIFT)
#define PMU_MIC_BIASA_VSEL(n)           BITFIELD_VAL(PMU_MIC_BIASA_VSEL, n)
#define PMU_MIC_LDO_RES_SHIFT           2
#define PMU_MIC_LDO_RES_MASK            (0xF << PMU_MIC_LDO_RES_SHIFT)
#define PMU_MIC_LDO_RES(n)              BITFIELD_VAL(PMU_MIC_LDO_RES, n)
#define PMU_LED_IO2_AIO_EN              (1 << 1)
#define PMU_LED_IO1_AIO_EN              (1 << 0)

// 3C   PMU_REG_MIC_BIAS_B
#define PMU_MIC_BIASB_CHANSEL_SHIFT     14
#define PMU_MIC_BIASB_CHANSEL_MASK      (0x3 << PMU_MIC_BIASB_CHANSEL_SHIFT)
#define PMU_MIC_BIASB_CHANSEL(n)        BITFIELD_VAL(PMU_MIC_BIASB_CHANSEL, n)
#define PMU_MIC_BIASB_EN                (1 << 13)
#define PMU_MIC_BIASB_ENLPF             (1 << 12)
#define PMU_MIC_BIASB_LPFSEL_SHIFT      10
#define PMU_MIC_BIASB_LPFSEL_MASK       (0x3 << PMU_MIC_BIASB_LPFSEL_SHIFT)
#define PMU_MIC_BIASB_LPFSEL(n)         BITFIELD_VAL(PMU_MIC_BIASB_LPFSEL, n)
#define PMU_MIC_BIASB_VSEL_SHIFT        6
#define PMU_MIC_BIASB_VSEL_MASK         (0xF << PMU_MIC_BIASB_VSEL_SHIFT)
#define PMU_MIC_BIASB_VSEL(n)           BITFIELD_VAL(PMU_MIC_BIASB_VSEL, n)
#define PMU_MIC_LDO_EN                  (1 << 5)
#define PMU_MIC_LDO_LOOPCTRL            (1 << 4)
#define PMU_MIC_LDO_PULLDOWN            (1 << 3)

// 3E   PMU_REG_USB_PIN_CFG
#define PMU_USB_DEBOUNCE_EN             (1 << 15)
#define PMU_USB_NOLS_MODE               (1 << 14)
#define PMU_USB_INSERT_DET_EN           (1 << 13)

// 3F   PMU_REG_USB_PIN_POL
#define PMU_USB_POL_RX_DM               (1 << 15)
#define PMU_USB_POL_RX_DP               (1 << 14)

// 3F   PMU_REG_EFUSE_CTRL
#define PMU_EFUSE_LOAD                  (1 << 13)
#define PMU_EFUSE_PGENB                 (1 << 12)
#define PMU_EFUSE_CSB                   (1 << 11)
#define PMU_EFUSE_PS                    (1 << 10)
#define PMU_EFUSE_PD                    (1 << 9)
#define PMU_EFUSE_CLK_EN                (1 << 8)
#define PMU_EFUSE_WADDR_SHIFT           0
#define PMU_EFUSE_WADDR_MASK            (0xFF << PMU_EFUSE_WADDR_SHIFT)
#define PMU_EFUSE_WADDR(n)              BITFIELD_VAL(PMU_EFUSE_WADDR, n)

// 40   PMU_REG_EFUSE_STROBE
#define PMU_EFUSE_STROBE_WIDTH_SHIFT    (1)
#define PMU_EFUSE_STROBE_WIDTH_MASK     (0x7FFF << PMU_EFUSE_STROBE_WIDTH_SHIFT)
#define PMU_EFUSE_STROBE_WIDTH(n)       BITFIELD_VAL(PMU_EFUSE_STROBE_WIDTH, n)
#define PMU_EFUSE_STROBE_TRIGGER        (1 << 0)

// 41   PMU_REG_RESERVED_ANA
#define PMU_RESERVED_CODEC_VOLT_HIGH    (1 << 0)
#define PMU_RESERVED_ANA_VOLT_DEC_50MV  (1 << 3)
#define PMU_RESERVED_DIG_VOLT_BIT5      (1 << 5)
#define PMU_RESERVED_DIG_VOLT_BIT6      (1 << 6)
#define PMU_RESERVED_BUCK_BIT7          (1 << 7)
#define PMU_RESERVED_BUCK_BIT8          (1 << 8)

// 4F   PMU_REG_POWER_OFF
#define PMU_SOFT_POWER_OFF              (1 << 0)

// 50   PMU_REG_INT_STATUS
#define PMU_INT_STATUS_USB_INSERT       (1 << 15)
#define PMU_INT_STATUS_RTC1             (1 << 14)
#define PMU_INT_STATUS_RTC0             (1 << 13)

// 51   PMU_REG_INT_MSKED_STATUS
// 51   PMU_REG_INT_CLR
#define PMU_INT_CLR_USB_INSERT          (1 << 15)
#define PMU_INT_CLR_RTC1                (1 << 14)
#define PMU_INT_CLR_RTC0                (1 << 13)

// 53   PMU_REG_EFUSE_VAL
#define PMU_EFUSE_VAL_SHIFT             8
#define PMU_EFUSE_VAL_MASK              (0xFF << PMU_EFUSE_VAL_SHIFT)
#define PMU_EFUSE_VAL(n)                BITFIELD_VAL(PMU_EFUSE_VAL, n)
#define PMU_EFUSE_RADDR_SHIFT           0
#define PMU_EFUSE_RADDR_MASK            (0xFF << PMU_EFUSE_RADDR_SHIFT)
#define PMU_EFUSE_RADDR(n)              BITFIELD_VAL(PMU_EFUSE_RADDR, n)

// 5E   PMU_REG_CHARGER_STATUS
#define PMU_AC_ON_DET_OUT_MASKED        (1 << 4)
#define PMU_AC_ON_DET_IN_MASKED         (1 << 3)
#define PMU_AC_ON                       (1 << 2)
#define PMU_AC_ON_DET_OUT               (1 << 1)
#define PMU_AC_ON_DET_IN                (1 << 0)

// 5F   PMU_REG_USB_PIN_STATUS
#define PMU_USB_STATUS_RX_DM            (1 << 10)
#define PMU_USB_STATUS_RX_DP            (1 << 9)

// 64   PMU_REG_ANA_64
#define REG_CODEC_TX_PA_GAIN_SHIFT          0
#define REG_CODEC_TX_PA_GAIN_MASK           (0x3 << REG_CODEC_TX_PA_GAIN_SHIFT)
#define REG_CODEC_TX_PA_GAIN(n)             BITFIELD_VAL(REG_CODEC_TX_PA_GAIN, n)
#define REG_CODEC_TX_EAR_RSTB               (1 << 2)
#define REG_CODEC_TX_PA_RSTB                (1 << 3)
#define REG_CODEC_TX_DAC_LPFVCM_SHIFT       4
#define REG_CODEC_TX_DAC_LPFVCM_MASK        (0xF << REG_CODEC_TX_DAC_LPFVCM_SHIFT)
#define REG_CODEC_TX_DAC_LPFVCM(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_LPFVCM, n)
#define CFG_EN_RX                           (1 << 8)
#define CFG_EN_RX_DR                        (1 << 9)
#define REG_AUDPLL_RSTN                     (1 << 10)
#define REG_AUDPLL_RSTN_DR                  (1 << 11)
#define REG_AUDPLL_PU                       (1 << 12)
#define REG_AUDPLL_PU_DR                    (1 << 13)
#define REG_AUDPLL_POSTDIV_RST              (1 << 14)
#define REG_AUDPLL_POSTDIV_RST_DR           (1 << 15)

// 65   PMU_REG_ANA_65
#define REG_AUDPLL_DIVN_320M_SHIFT          0
#define REG_AUDPLL_DIVN_320M_MASK           (0x7 << REG_AUDPLL_DIVN_320M_SHIFT)
#define REG_AUDPLL_DIVN_320M(n)             BITFIELD_VAL(REG_AUDPLL_DIVN_320M, n)
#define REG_AUDPLL_DIVN_480M_SHIFT          3
#define REG_AUDPLL_DIVN_480M_MASK           (0x7 << REG_AUDPLL_DIVN_480M_SHIFT)
#define REG_AUDPLL_DIVN_480M(n)             BITFIELD_VAL(REG_AUDPLL_DIVN_480M, n)
#define REG_AUDPLL_POST_DIVN_SHIFT          6
#define REG_AUDPLL_POST_DIVN_MASK           (0x3F << REG_AUDPLL_POST_DIVN_SHIFT)
#define REG_AUDPLL_POST_DIVN(n)             BITFIELD_VAL(REG_AUDPLL_POST_DIVN, n)
#define REG_AUDPLL_FDIV_SEL_SHIFT           12
#define REG_AUDPLL_FDIV_SEL_MASK            (0x3 << REG_AUDPLL_FDIV_SEL_SHIFT)
#define REG_AUDPLL_FDIV_SEL(n)              BITFIELD_VAL(REG_AUDPLL_FDIV_SEL, n)
#define REG_AUDPLL_FANA960M_DIV2_EN         (1 << 14)
#define REG_AUDPLL_FANA960M_SEL             (1 << 15)

// 1EF  PMU_REG_RF_1EF
#define REG_WIFI_BBPLL_BW_SEL_BT_SHIFT      12
#define REG_WIFI_BBPLL_BW_SEL_BT_MASK       (0xF << REG_WIFI_BBPLL_BW_SEL_BT_SHIFT)
#define REG_WIFI_BBPLL_BW_SEL_BT(n)         BITFIELD_VAL(REG_WIFI_BBPLL_BW_SEL_BT, n)
#define REG_WIFI_BBPLL_SPEED_BT_SHIFT       9
#define REG_WIFI_BBPLL_SPEED_BT_MASK        (0x7 << REG_WIFI_BBPLL_SPEED_BT_SHIFT)
#define REG_WIFI_BBPLL_SPEED_BT(n)          BITFIELD_VAL(REG_WIFI_BBPLL_SPEED_BT, n)
#define REG_WIFI_BBPLL_CPMODE_BT            (1 << 8)
#define REG_WIFI_BBPLL_REFCLK_SEL_BT        (1 << 7)
#define REG_BBPLL_DIVN_480M_BT_SHIFT        4
#define REG_BBPLL_DIVN_480M_BT_MASK         (0x7 << REG_BBPLL_DIVN_480M_BT_SHIFT)
#define REG_BBPLL_DIVN_480M_BT(n)           BITFIELD_VAL(REG_BBPLL_DIVN_480M_BT, n)
#define REG_BBPLL_DIVN_320M_BT_SHIFT        1
#define REG_BBPLL_DIVN_320M_BT_MASK         (0x7 << REG_BBPLL_DIVN_320M_BT_SHIFT)
#define REG_BBPLL_DIVN_320M_BT(n)           BITFIELD_VAL(REG_BBPLL_DIVN_320M_BT, n)
#define REG_BBPLL_CODEC_CLKDIV2_EN_BT       (1 << 0)

// 1FA  PMU_REG_RF_1FA
#define REG_BBPLL_CODEC_DIVN_SHIFT          0
#define REG_BBPLL_CODEC_DIVN_MASK           (0x3F << REG_BBPLL_CODEC_DIVN_SHIFT)
#define REG_BBPLL_CODEC_DIVN(n)             BITFIELD_VAL(REG_BBPLL_CODEC_DIVN, n)
#define REG_BBPLL_CODEC_CLK25M_EN           (1 << 6)
#define REG_BBPLL_CODECDIV_RST              (1 << 9)

enum PMU_REG_T {
    PMU_REG_METAL_ID            = 0x00,
    PMU_REG_POWER_KEY_CFG       = 0x02,
    PMU_REG_BIAS_CFG            = 0x03,
    PMU_REG_CHARGER_CFG         = 0x05,
    PMU_REG_USB11_CFG           = 0x06,
    PMU_REG_ANA_CFG             = 0x07,
    PMU_REG_DIG_CFG             = 0x08,
    PMU_REG_IO_CFG              = 0x09,
    PMU_REG_MEM_CFG             = 0x0A,
    PMU_REG_GP_CFG              = 0x0B,
    PMU_REG_USB_CFG             = 0x0C,
    PMU_REG_CRYSTAL_CFG         = 0x0D,
    PMU_REG_VPA_CFG             = 0x0F,
    PMU_REG_CODEC_CFG           = 0x10,
    PMU_REG_BUCK_VOLT           = 0x15,
    PMU_REG_DCDC_CFG            = 0x16,
    PMU_REG_BUCK_FREQ           = 0x17,
    PMU_REG_BUCK_VCOMP          = 0x19,
    PMU_REG_HPPA_CFG            = 0x1A,
    PMU_REG_HPPA_FREQ           = 0x1B,
    PMU_REG_CP_PU               = 0x1D,
    PMU_REG_THERM               = 0x1E,
    PMU_REG_PWR_SEL             = 0x21,
    PMU_REG_SLEEP_CFG           = 0x24,
    PMU_REG_INT_MASK            = 0x26,
    PMU_REG_INT_EN              = 0x27,
    PMU_REG_USBPHY              = 0x28,
    PMU_REG_RTC_DIV_1HZ         = 0x2C,
    PMU_REG_RTC_LOAD_LOW        = 0x2D,
    PMU_REG_RTC_LOAD_HIGH       = 0x2E,
    PMU_REG_RTC_MATCH1_LOW      = 0x31,
    PMU_REG_RTC_MATCH1_HIGH     = 0x32,
    PMU_REG_HPPA_BUCK0          = 0x33,
    PMU_REG_HPPA_BUCK1          = 0x34,
    PMU_REG_HPPA_BUCK2          = 0x35,
    PMU_REG_HPPA_BUCK3          = 0x36,
    PMU_REG_HPPA_BUCK4          = 0x37,
    PMU_REG_HPPA_BUCK5          = 0x38,
    PMU_REG_TRNG                = 0x39,
    PMU_REG_LED_CFG             = 0x3A,
    PMU_REG_MIC_BIAS_A          = 0x3B,
    PMU_REG_MIC_BIAS_B          = 0x3C,
    PMU_REG_USB_PIN_CFG         = 0x3E,
    PMU_REG_USB_PIN_POL         = 0x3F,
    PMU_REG_EFUSE_CTRL          = PMU_REG_USB_PIN_POL,
    PMU_REG_EFUSE_STROBE        = 0x40,
    PMU_REG_RESERVED_ANA        = 0x41,
    PMU_REG_POWER_OFF           = 0x4F,
    PMU_REG_INT_STATUS          = 0x50,
    PMU_REG_INT_MSKED_STATUS    = 0x51,
    PMU_REG_INT_CLR             = 0x51,
    PMU_REG_EFUSE_VAL           = 0x53,
    PMU_REG_RTC_VAL_LOW         = 0x54,
    PMU_REG_RTC_VAL_HIGH        = 0x55,
    PMU_REG_CHARGER_STATUS      = 0x5E,
    PMU_REG_USB_PIN_STATUS      = 0x5F,

    PMU_REG_MODULE_START        = PMU_REG_ANA_CFG,

    // ANA registers
    PMU_REG_ANA_60              = 0x60,
    PMU_REG_ANA_64              = 0x64,
    PMU_REG_ANA_65              = 0x65,
    PMU_REG_ANA_167             = 0x167,

    // RF registers
    PMU_REG_RF_C0               = 0xC0,
    PMU_REG_RF_9D               = 0x9D,
    PMU_REG_RF_1EF              = 0x1EF,
    PMU_REG_RF_1FA              = 0x1FA,
};

enum PMU_VCORE_REQ_T {
    PMU_VCORE_FLASH_WRITE_ENABLED   = (1 << 0),
    PMU_VCORE_FLASH_FREQ_HIGH       = (1 << 1),
    PMU_VCORE_FLASH_FREQ_MEDIUM     = (1 << 2),
    PMU_VCORE_PSRAM_FREQ_HIGH       = (1 << 3),
    PMU_VCORE_USB_ENABLED           = (1 << 4),
    PMU_VCORE_IIR_FREQ_MEDIUM       = (1 << 5),
    PMU_VCORE_IIR_FREQ_HIGH         = (1 << 6),
    PMU_VCORE_SYS_FREQ_MEDIUM       = (1 << 7),
    PMU_VCORE_SYS_FREQ_HIGH         = (1 << 8),
};

enum PMU_MODUAL_T {
    PMU_ANA,
    PMU_DIG,
    PMU_IO,
    PMU_MEM,
    PMU_GP,
    PMU_USB,
    PMU_CRYSTAL,
    PMU_CODEC,
};

struct PMU_MODULE_CFG_T {
    unsigned short manual_bit;
    unsigned short ldo_en;
    unsigned short lp_mode;
    unsigned short dsleep_mode;
    unsigned short dsleep_v;
    unsigned short dsleep_v_shift;
    unsigned short normal_v;
    unsigned short normal_v_shift;
};

#define PMU_MOD_CFG_VAL(m)              { \
    PMU_##m##_V##m##_DR, PMU_##m##_V##m##_EN, \
    PMU_##m##_LP_MODE_EN, PMU_##m##_DSLEEP_ON, \
    PMU_##m##_DSLEEP_VOLT_MASK, PMU_##m##_DSLEEP_VOLT_SHIFT,\
    PMU_##m##_NORMAL_VOLT_MASK, PMU_##m##_NORMAL_VOLT_SHIFT }

static const struct PMU_MODULE_CFG_T pmu_module_cfg[] = {
    PMU_MOD_CFG_VAL(ANA),
    PMU_MOD_CFG_VAL(DIG),
    PMU_MOD_CFG_VAL(IO),
    PMU_MOD_CFG_VAL(MEM),
    PMU_MOD_CFG_VAL(GP),
    PMU_MOD_CFG_VAL(USB),
    PMU_MOD_CFG_VAL(CRYSTAL),
    PMU_MOD_CFG_VAL(CODEC),
};

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE bool audio_output_diff =
#ifdef AUDIO_OUTPUT_DIFF
    true;
#else
    false;
#endif
static OPT_TYPE bool vcodec_off =
#ifdef VCODEC_OFF
    true;
#else
    false;
#endif
static OPT_TYPE uint8_t ana_act_dcdc =
#ifdef VANA_1P5V
    PMU_DCDC_ANA_1_5V;
#elif defined(VANA_1P6V)
    PMU_DCDC_ANA_1_6V;
#else
    PMU_DCDC_ANA_1_8V;
#endif
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
static OPT_TYPE bool POSSIBLY_UNUSED vcrystal_off =
#ifdef VCRYSTAL_OFF
    true;
#else
    false;
#endif

static enum PMU_POWER_MODE_T BOOT_BSS_LOC pmu_power_mode = PMU_POWER_MODE_NONE;
static enum PMU_POWER_MODE_T pmu_active_power_mode = PMU_POWER_MODE_NONE;

static enum PMU_VCORE_REQ_T BOOT_BSS_LOC pmu_vcore_req;

static enum PMU_USB_PIN_CHK_STATUS_T usb_pin_status;

static PMU_USB_PIN_CHK_CALLBACK usb_pin_callback;

static PMU_CHARGER_IRQ_HANDLER_T charger_irq_handler;

static uint8_t SRAM_BSS_DEF(vio_risereq_map);
STATIC_ASSERT(sizeof(vio_risereq_map) * 8 >= PMU_VIORISE_REQ_USER_QTY, "vio_risereq_map size too small");

static uint8_t SRAM_DATA_DEF(vio_act_normal) = IO_VOLT_ACTIVE_NORMAL;
static uint8_t SRAM_DATA_DEF(vio_act_rise) = IO_VOLT_ACTIVE_RISE;
static uint8_t SRAM_DATA_DEF(vio_lp) = IO_VOLT_SLEEP;

static const uint8_t ana_lp_dcdc = PMU_DCDC_ANA_SLEEP_1_4V;

// Move all the data/bss invovled in pmu_open() to .sram_data/.sram_bss,
// so that pmu_open() can be called at the end of BootInit(),
// for data/bss is initialized after BootInit().
#ifdef VCORE_LDO_ON
static bool SRAM_BSS_DEF(vcore_ldo_off);
static uint16_t SRAM_BSS_DEF(dig_act_ldo);
#endif

static const uint8_t dig_lp_ldo = PMU_VDIG_0_8V;
static const uint8_t dig_lp_dcdc = PMU_DCDC_DIG_0_8V;

static uint8_t BOOT_DATA_DEF(bbpll_dig_div) = 4;
static uint8_t BOOT_DATA_DEF(bbpll_codec_div) = 32;
static uint8_t BOOT_DATA_DEF(audpll_dig_div) = 4;
static uint8_t BOOT_DATA_DEF(audpll_codec_div) = 32;

#if defined(MCU_HIGH_PERFORMANCE_MODE)
static const uint16_t high_perf_freq_mhz =
#if defined(MTEST_ENABLED) && defined(MTEST_CLK_MHZ)
    MTEST_CLK_MHZ;
#else
    400;
#endif
static bool high_perf_on;
#endif

#if defined(_AUTO_TEST_)
static bool at_skip_shutdown = false;
void pmu_at_skip_shutdown(bool enable)
{
    at_skip_shutdown = enable;
}
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
    int ret;
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

#if defined(PMU_FULL_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    // Reset RF
    pmu_write(PMU_REG_RF_C0, 0x0000);
    pmu_write(PMU_REG_RF_C0, 0x0001);
    hal_sys_timer_delay(US_TO_TICKS(500));

    // Restore BBPLL settings in ROM
    pmu_write(0x9d, 0x68a6);
    pmu_write(0x99, 0x6d42);
    pmu_write(0x9b, 0x089c);
    pmu_write(0x9c, 0x005c);
    pmu_write(0x188, 0x7890);
    pmu_write(0x1EF, 0x2840);
    pmu_write(0x9d, 0x68a4);

    // Reset ANA
    pmu_write(PMU_REG_ANA_60, 0xCAFE);
    pmu_write(PMU_REG_ANA_60, 0x5FEE);
    hal_sys_timer_delay(US_TO_TICKS(500));

#ifdef OSC_FAST_WAKEUP_RC_OSC
    // CAUTION:
    // This option should be consistent with the RF setting in BT driver
    extern unsigned int hal_analogif_get_padding_cycles(void);
    register uint32_t padding;

    padding = hal_analogif_get_padding_cycles();

    // Set RF reg 0x194 from 0x0000 to 0x8000 (which will impact OSC)
    *(volatile uint32_t *)0x4000D008 = 0xC08000 << padding;

    hal_cmu_mem_set_freq(HAL_CMU_FREQ_32K);
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_32K);
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_32K);

    *(volatile uint32_t *)0x4000D008 = 0x948000 << padding;

    hal_sys_timer_delay(MS_TO_TICKS(3));

    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_26M);
    hal_ispi_close(0);
    hal_analogif_open();

    *(volatile uint32_t *)0x4000D008 = 0xC00000 << padding;
#endif
#endif

    // Enable SWD debug mode
    pmu_read(PMU_REG_DCDC_CFG, &val);
    val = SET_BITFIELD(val, PMU_TEST_MODE, 4);
    pmu_write(PMU_REG_DCDC_CFG, val);

    // Enable 26M doubler (52M)
#ifdef ANA_26M_X4_ENABLE
    pmu_write(0xA4, 0x5000);
#else
    pmu_write(0xA4, 0x1000);
#endif
    pmu_read(0xC5, &val);
    val = (val & ~((0xF << 0) | (0xF << 4))) | (4 << 0) | (4 << 4) | (1 << 8) | (1 << 9);
    pmu_write(0xC5, val);
    hal_sys_timer_delay(2);
    val &= ~(1 << 8);
    pmu_write(0xC5, val);

#ifdef CLK_32K_FROM_OSC
    // reg_vtoi_5g_en
    pmu_read(0xC1, &val);
    val |= (1 << 2) | (1 << 3);
    pmu_write(0xC1, val);
#ifdef NO_LP_OSC
    // reg_bt_xtal_sel_mode
    pmu_read(0xC4, &val);
    val = (val & ~(0x3F << 5)) | (0x2D << 5) | (1 << 11);
    pmu_write(0xC4, val);
    // reg_wifi_xtal_pu
    pmu_read(0x1EB, &val);
    val |= (1 << 12) | (1 << 13);
    pmu_write(0x1EB, val);
#else
    // Clear reg_bt_xtal_sel_mode_dr and reg_wifi_xtal_pu_lpm_dr, and
    // set reg_bt_xtal_rstb_ldo and reg_bt_xtal_rstb_ldo_dr
    pmu_read(0xC4, &val);
    val &= ~((1 << 11) | (1 << 15));
    val |= (1 << 12 | (1 << 13));
    pmu_write(0xC4, val);
    // reg_pu_bt_xtal_dsleep
    pmu_read(0x1EA, &val);
    val |= (1 << 15);
    pmu_write(0x1EA, val);
    //pmu_write(0xC5, 0x8244);
    //pmu_write(0xC3, 0xA848);
    //pmu_write(0xEF, 0x3900);
#endif
    // reg_xtal_pu_clkbuf_slp[7] and reg_bt_xtal_pu_clkbuf[7]
    pmu_read(0xDC, &val);
    val |= (1 << 1) | (1 << 4);
    pmu_write(0xDC, val);
    // clk_32k_sel
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    val = (val & ~(3 << 8)) | (1 << 8);
    pmu_write(PMU_REG_POWER_KEY_CFG, val);
#else
    pmu_write(0xC4, 0x45B0);
#endif

    pmu_write(0x0E, 0x0380);
    //pmu_write(0x0E, 0x1f80);

    // Update BBPLL cfg
    // bbpll_dig_rc
    pmu_read(0x99, &val);
    val = (val & ~(0x7 << 7)) | (0x7 << 7);
    pmu_write(0x99, val);
    // bbpll_post_rst_time
    pmu_read(0xD1, &val);
    val = (val & ~(0x7 << 3)) | (0x7 << 3);
    pmu_write(0xD1, val);
    // pu_prechar_value_us
    pmu_read(0x1E3, &val);
    val = (val & ~(0xF << 8)) | (0x7 << 8);
    pmu_write(0x1E3, val);

    pmu_write(0x9e, 0x000a);
    //pmu_write(0xc6, 0xf000);
    // Enable audio PLL
    // RF
    pmu_write(0xee, 0x8020);
    pmu_write(0xdc, 0x0016);
    // ANA
    pmu_write(0x64, 0x8080);
    pmu_write(0x66, 0x0082);
    pmu_write(0x6d, 0x5000);
    pmu_write(0x75, 0x6f00);
    pmu_write(0x76, 0x5200);
#ifdef USB_HIGH_SPEED
    // For 960M
    pmu_write(0x77, 0x2270);
    pmu_write(0x78, 0x7248);
#else
    // For 760M
    pmu_write(0x77, 0x2271);
    pmu_write(0x78, 0x7208);
#endif
    pmu_write(0x79, 0x6DB4);
    pmu_write(0x66, 0x9082);
#ifdef OSC_26M_X4_AUD2BB
    pmu_write(0x65, 0x38c0);
#else
#ifdef AUD_PLL_SYS_122M
    // sys_div = 7
    pmu_write(0x65, 0x38ff);
#else
    // sys_div = 4
    pmu_write(0x65, 0x3827);
#endif
#endif
#ifdef USB_HIGH_SPEED
    // PLL 960M
    // [15:0]
    pmu_write(0x67, 0xEC4F);
    // [31:16]
    pmu_write(0x68, 0x4EC4);
    // [34:32] and enable
    pmu_write(0x69, 0x5488);
#else
#ifdef AUD_PLL_SYS_122M
    // PLL 860M / 7 -> AUD 122M
    // [15:0]
    pmu_write(0x67, 0x4878);
    // [31:16]
    pmu_write(0x68, 0x1154);
    // [34:32] and enable
    pmu_write(0x69, 0x5488);
#else
    // PLL 812M / 4 -> AUD 203M
    // [15:0]
    pmu_write(0x67, 0x4992);
    // [31:16]
    pmu_write(0x68, 0xE3F5);
    // [34:32] and enable
    pmu_write(0x69, 0x3883);
#endif
#endif

    ret = pmu_read(PMU_REG_METAL_ID, &val);
    if (ret) {
        return 0;
    }

    metal_id = GET_BITFIELD(val, PMU_METAL_ID);
    if (metal_id == HAL_CHIP_METAL_ID_2 && hal_cmu_get_aon_chip_id() == 0x00040002) {
        // Version E
        metal_id = HAL_CHIP_METAL_ID_3;
    }

#if 0 //ndef PROGRAMMER
    if (metal_id == 0) {
        uint16_t boot;

        ret = pmu_get_efuse(PMU_EFUSE_PAGE_BOOT, &boot);
        // Check for UART download mode
        if (ret == 0 && (boot & (1 << 1))) {
            // Fix BBPLL for metal 0 chips
            pmu_write(0x9d, 0x68a6);
            pmu_write(0x99, 0x6d42);
            pmu_write(0x9b, 0x089c);
            pmu_write(0x9c, 0x005c);
            pmu_write(0x188, 0x7890);
            pmu_write(0x1EF, 0x2840);
            pmu_write(0x9d, 0x68a4);
        }
        // Otherwise BBPLL is in use
    }
#endif

    // Init pll dividers
    pmu_read(PMU_REG_RF_1EF, &val);
    val = SET_BITFIELD(val, REG_BBPLL_DIVN_480M_BT, bbpll_dig_div);
    pmu_write(PMU_REG_RF_1EF, val);
    pmu_read(PMU_REG_RF_1FA, &val);
    val = SET_BITFIELD(val, REG_BBPLL_CODEC_DIVN, bbpll_codec_div);
    pmu_write(PMU_REG_RF_1FA, val);
    pmu_read(PMU_REG_ANA_65, &val);
    val = (val & ~(REG_AUDPLL_DIVN_480M_MASK | REG_AUDPLL_POST_DIVN_MASK)) |
        REG_AUDPLL_DIVN_480M(audpll_dig_div) | REG_AUDPLL_POST_DIVN(audpll_codec_div);
    pmu_write(PMU_REG_ANA_65, val);

#ifdef AUDIO_USE_BBPLL
    pmu_read(PMU_REG_RF_1FA, &val);
    val |= REG_BBPLL_CODEC_CLK25M_EN;
    pmu_write(PMU_REG_RF_1FA, val);
#endif

    return metal_id;
}

void BOOT_TEXT_SRAM_LOC pmu_pll_div_reset_set(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_USB) {
        pmu_read(PMU_REG_RF_1EF, &val);
        val |= REG_BBPLL_DIVN_480M_BT_MASK;
        pmu_write(PMU_REG_RF_1EF, val);
        pmu_read(PMU_REG_RF_1FA, &val);
        val |= REG_BBPLL_CODEC_DIVN_MASK | REG_BBPLL_CODECDIV_RST;
        pmu_write(PMU_REG_RF_1FA, val);
    } else {
        pmu_read(PMU_REG_ANA_65, &val);
        val |= REG_AUDPLL_DIVN_480M_MASK | REG_AUDPLL_POST_DIVN_MASK;
        pmu_write(PMU_REG_ANA_65, val);
        pmu_read(PMU_REG_ANA_64, &val);
        val |= REG_AUDPLL_POSTDIV_RST_DR | REG_AUDPLL_POSTDIV_RST;
        pmu_write(PMU_REG_ANA_64, val);
    }
    int_unlock(lock);
}

void BOOT_TEXT_SRAM_LOC pmu_pll_div_reset_clear(enum HAL_CMU_PLL_T pll)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_USB) {
        pmu_read(PMU_REG_RF_1EF, &val);
        val = SET_BITFIELD(val, REG_BBPLL_DIVN_480M_BT, bbpll_dig_div);
        pmu_write(PMU_REG_RF_1EF, val);
        pmu_read(PMU_REG_RF_1FA, &val);
        val &= ~REG_BBPLL_CODECDIV_RST;
        pmu_write(PMU_REG_RF_1FA, val);
        val = SET_BITFIELD(val, REG_BBPLL_CODEC_DIVN, bbpll_codec_div);
        pmu_write(PMU_REG_RF_1FA, val);
    } else {
        pmu_read(PMU_REG_ANA_64, &val);
        val = (val & ~REG_AUDPLL_POSTDIV_RST) | REG_AUDPLL_POSTDIV_RST_DR;
        pmu_write(PMU_REG_ANA_64, val);
        pmu_read(PMU_REG_ANA_65, &val);
        val = (val & ~(REG_AUDPLL_DIVN_480M_MASK | REG_AUDPLL_POST_DIVN_MASK)) |
            REG_AUDPLL_DIVN_480M(audpll_dig_div) | REG_AUDPLL_POST_DIVN(audpll_codec_div);
        pmu_write(PMU_REG_ANA_65, val);
    }
    int_unlock(lock);
}

void pmu_pll_div_set(enum HAL_CMU_PLL_T pll, enum PMU_PLL_DIV_TYPE_T type, uint32_t div)
{
    uint32_t lock;
    uint16_t val;

    lock = int_lock();
    if (pll == HAL_CMU_PLL_USB) {
        if (type == PMU_PLL_DIV_DIG) {
            if (div != bbpll_dig_div) {
                bbpll_dig_div = div;
                pmu_read(PMU_REG_RF_1EF, &val);
                val |= REG_BBPLL_DIVN_480M_BT_MASK;
                pmu_write(PMU_REG_RF_1EF, val);
                if (div != (REG_BBPLL_DIVN_480M_BT_MASK >> REG_BBPLL_DIVN_480M_BT_SHIFT)) {
                    val = SET_BITFIELD(val, REG_BBPLL_DIVN_480M_BT, div);
                    pmu_write(PMU_REG_RF_1EF, val);
                }
            }
        } else {
            if (div != bbpll_codec_div) {
                bbpll_codec_div = div;
                pmu_read(PMU_REG_RF_1FA, &val);
                val |= REG_BBPLL_CODEC_DIVN_MASK;
                pmu_write(PMU_REG_RF_1FA, val);
                if (div != (REG_BBPLL_CODEC_DIVN_MASK >> REG_BBPLL_CODEC_DIVN_SHIFT)) {
                    val = SET_BITFIELD(val, REG_BBPLL_CODEC_DIVN, div);
                    pmu_write(PMU_REG_RF_1FA, val);
                }
            }
        }
    } else {
        if (type == PMU_PLL_DIV_DIG) {
            if (div != audpll_dig_div) {
                audpll_dig_div = div;
                pmu_read(PMU_REG_ANA_65, &val);
                val |= REG_AUDPLL_DIVN_480M_MASK;
                pmu_write(PMU_REG_ANA_65, val);
                if (div != (REG_AUDPLL_DIVN_480M_MASK >> REG_AUDPLL_DIVN_480M_SHIFT)) {
                    val = SET_BITFIELD(val, REG_AUDPLL_DIVN_480M, div);
                    pmu_write(PMU_REG_ANA_65, val);
                }
            }
        } else {
            if (div != audpll_codec_div) {
                audpll_codec_div = div;
                pmu_read(PMU_REG_ANA_65, &val);
                val |= REG_AUDPLL_POST_DIVN_MASK;
                pmu_write(PMU_REG_ANA_65, val);
                if (div != (REG_AUDPLL_POST_DIVN_MASK >> REG_AUDPLL_POST_DIVN_SHIFT)) {
                    val = SET_BITFIELD(val, REG_AUDPLL_POST_DIVN, div);
                    pmu_write(PMU_REG_ANA_65, val);
                }
            }
        }
    }
    int_unlock(lock);
}


int BOOT_TEXT_SRAM_LOC pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
    static const uint16_t ctrl_open[3] = {
        PMU_EFUSE_CSB | PMU_EFUSE_PD | PMU_EFUSE_CLK_EN,
        PMU_EFUSE_CSB | PMU_EFUSE_CLK_EN,
        PMU_EFUSE_LOAD | PMU_EFUSE_PGENB | PMU_EFUSE_CLK_EN,
    };
    static const uint16_t ctrl_close[3] = {
        PMU_EFUSE_CSB | PMU_EFUSE_CLK_EN,
        PMU_EFUSE_CSB | PMU_EFUSE_PD | PMU_EFUSE_CLK_EN,
        PMU_EFUSE_CSB | PMU_EFUSE_PD,
    };
    static const unsigned char strobe_off = PMU_EFUSE_STROBE_WIDTH(1);
    static const unsigned char strobe_on = PMU_EFUSE_STROBE_WIDTH(1) | PMU_EFUSE_STROBE_TRIGGER;

    int ret;
    unsigned short val;
    unsigned char bytes[2];
    int i;

    ret = pmu_write(PMU_REG_EFUSE_STROBE, strobe_off);
    if (ret) {
        goto _exit;
    }

    for (i = 0; i < ARRAY_SIZE(ctrl_open); i++) {
        ret = pmu_write(PMU_REG_EFUSE_CTRL, ctrl_open[i]);
        if (ret) {
            goto _exit;
        }
    }

    for (i = 0; i < 2; i++) {
        val = SET_BITFIELD(ctrl_open[ARRAY_SIZE(ctrl_open) - 1], PMU_EFUSE_WADDR, (page << 1) + i);
        ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
        if (ret) {
            goto _exit;
        }

        ret = pmu_write(PMU_REG_EFUSE_STROBE, strobe_on);
        if (ret) {
            goto _exit;
        }

        ret = pmu_read(PMU_REG_EFUSE_VAL, &val);
        if (ret) {
            goto _exit;
        }
        bytes[i] = GET_BITFIELD(val, PMU_EFUSE_VAL);

        ret = pmu_write(PMU_REG_EFUSE_STROBE, strobe_off);
        if (ret) {
            goto _exit;
        }
    }

_exit:
    for (i = 0; i < ARRAY_SIZE(ctrl_close); i++) {
        pmu_write(PMU_REG_EFUSE_CTRL, ctrl_close[i]);
    }

    if (ret == 0) {
        *efuse = (bytes[1] << 8) | bytes[0];
    }

    return ret;
}

static unsigned int pmu_count_zeros(unsigned int val, unsigned int bits)
{
    int cnt = 0;
    int i;

    for (i = 0; i < bits; i++) {
        if ((val & (1 << i)) == 0) {
            cnt++;
        }
    }

    return cnt;
}

int pmu_get_security_value(union SECURITY_VALUE_T *val)
{
    int ret;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &val->reg);
    if (ret) {
        // Error
        goto _no_security;
    }

    if (!val->security_en) {
        // OK
        goto _no_security;
    }
    ret = 1;
    if (pmu_count_zeros(val->key_id, 3) != val->key_chksum) {
        // Error
        goto _no_security;
    }
    if (pmu_count_zeros(val->vendor_id, 6) != val->vendor_chksum) {
        // Error
        goto _no_security;
    }
    if ((pmu_count_zeros(val->reg, 15) & 1) != val->chksum) {
        // Error
        goto _no_security;
    }

    // OK
    return 0;

_no_security:
    val->reg = 0;

    return ret;
}

void pmu_shutdown(void)
{
    uint16_t val;
    uint32_t lock = int_lock();

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    // Default vcore might not be high enough to support high performance mode
    pmu_high_performance_mode_enable(false);
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
#endif
    pmu_mode_change(PMU_POWER_MODE_LDO);
    hal_sys_timer_delay(MS_TO_TICKS(1));
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

    pmu_read(PMU_REG_PWR_SEL, &val);
    val |= PMU_PWR_SEL_DR | PMU_PWR_SEL;
    pmu_write(PMU_REG_PWR_SEL, val);

#if defined(_AUTO_TEST_)
    if (at_skip_shutdown) {
        hal_cmu_sys_reboot();
        return;
    }
#endif

    // Power off
    pmu_read(PMU_REG_POWER_OFF,&val);
    val |= PMU_SOFT_POWER_OFF;
    for (int i = 0; i < 100; i++) {
        pmu_write(PMU_REG_POWER_OFF,val);
        hal_sys_timer_delay(MS_TO_TICKS(5));
    }

    hal_sys_timer_delay(MS_TO_TICKS(50));

    //can't reach here
    TRACE_IMM(0,"\nError: pmu_shutdown failed!\n");
    hal_sys_timer_delay(MS_TO_TICKS(5));
    hal_cmu_sys_reboot();

    int_unlock(lock);
}

static inline uint16_t pmu_get_module_addr(enum PMU_MODUAL_T module)
{
    if (module == PMU_CODEC) {
        return module + PMU_REG_MODULE_START + 2;
    } else {
        return module + PMU_REG_MODULE_START;
    }
}

void pmu_module_config(enum PMU_MODUAL_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode)
{
    int ret;
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

#if 0
    if (vcodec_off) {
        if (module == PMU_CODEC) {
            ldo_on = PMU_LDO_OFF;
        }
    }
#endif

    module_address = pmu_get_module_addr(module);

    ret = pmu_read(module_address, &val);
    if(ret == 0)
    {
#ifdef VCORE_LDO_ON
        if (module == PMU_DIG /*&& hal_get_chip_metal_id() == 0*/) {
            // VCORE should NEVER be disabled
            if (ldo_on == PMU_LDO_OFF) {
                vcore_ldo_off = true;
                // Force to enter LP mode and set voltage to min
                val |= PMU_DIG_LP_MODE_EN | PMU_DIG_LP_EN_DR | PMU_DIG_LP_EN_REG | PMU_DIG_DSLEEP_ON | PMU_DIG_VDIG_DR | PMU_DIG_VDIG_EN;
                val = (val & ~(PMU_DIG_DSLEEP_VOLT_MASK | PMU_DIG_NORMAL_VOLT_MASK)) |
                    PMU_DIG_DSLEEP_VOLT(0) | PMU_DIG_NORMAL_VOLT(0);
            } else {
                vcore_ldo_off = false;
                // Cancel LP mode direct-reg and restore voltage
                val |= PMU_DIG_LP_MODE_EN | PMU_DIG_DSLEEP_ON | PMU_DIG_VDIG_DR | PMU_DIG_VDIG_EN;
                val &= ~(PMU_DIG_LP_EN_DR | PMU_DIG_LP_EN_REG);
                val = (val & ~(PMU_DIG_DSLEEP_VOLT_MASK | PMU_DIG_NORMAL_VOLT_MASK)) |
                    PMU_DIG_DSLEEP_VOLT(dig_lp_ldo) | PMU_DIG_NORMAL_VOLT(dig_act_ldo);
            }
        } else
#endif
        {
            if (module == PMU_USB) {
                uint16_t usb11;

                pmu_read(PMU_REG_USB11_CFG, &usb11);
                usb11 |= PMU_PU_USB11_DR;
                if (ldo_on) {
                    usb11 |= PMU_PU_USB11_REG;
                } else {
                    usb11 &= ~PMU_PU_USB11_REG;
                }
                pmu_write(PMU_REG_USB11_CFG, usb11);
            }

            if(is_manual)
            {
                val |= module_cfg_p->manual_bit;
            }
            else
            {
                val &= ~module_cfg_p->manual_bit;
            }
            if(ldo_on)
            {
                val |= module_cfg_p->ldo_en;
            }
            else
            {
                val &= ~module_cfg_p->ldo_en;
            }
            if(lp_mode)
            {
                val |= module_cfg_p->lp_mode;
            }
            else
            {
                val &= ~module_cfg_p->lp_mode;
            }
            if(dpmode)
            {
                val |= module_cfg_p->dsleep_mode;
            }
            else
            {
                val &= ~module_cfg_p->dsleep_mode;
            }
        }
        pmu_write(module_address, val);
    }
}

void pmu_module_set_volt(unsigned char module, unsigned short sleep_v,unsigned short normal_v)
{
    int ret;
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

#ifdef VCORE_LDO_ON
    if (module == PMU_DIG /*&& hal_get_chip_metal_id() == 0*/) {
        if (vcore_ldo_off) {
            // Never change vcore ldo voltage when it is switched off nominally
            return;
        }
    }
#endif

    module_address = pmu_get_module_addr(module);

    ret = pmu_read(module_address, &val);
    if(ret == 0)
    {
        val &= ~module_cfg_p->normal_v;
        val |= (normal_v << module_cfg_p->normal_v_shift) & module_cfg_p->normal_v;
        val &= ~module_cfg_p->dsleep_v;
        val |= (sleep_v << module_cfg_p->dsleep_v_shift) & module_cfg_p->dsleep_v;
        pmu_write(module_address, val);
    }
}

int pmu_module_get_volt(unsigned char module, unsigned short *sleep_vp,unsigned short *normal_vp)
{
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    pmu_read(module_address, &val);
    if (normal_vp) {
        *normal_vp = (val & module_cfg_p->normal_v) >> module_cfg_p->normal_v_shift;
    }
    if (sleep_vp) {
        *sleep_vp = (val & module_cfg_p->dsleep_v) >> module_cfg_p->dsleep_v_shift;
    }

    return 0;
}

static void pmu_dcdc_ana_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_BUCK_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, PMU_BUCK_VANA_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, PMU_BUCK_VANA_DSLEEP);
    }
}

void pmu_dcdc_ana_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    int ret;
    unsigned short val;

#ifdef PMU_DCDC_ANA2_SUPPORT
    pmu_read(PMU_REG_BUCK_VCOMP, &val);
    if (normal_v & PMU_DCDC_ANA2_FLAG) {
        normal_v &= ~PMU_DCDC_ANA2_FLAG;
        val |= PMU_BUCK_HV_ANA_NORMAL;
    } else {
        val &= ~PMU_BUCK_HV_ANA_NORMAL;
    }
    if (dsleep_v & PMU_DCDC_ANA2_FLAG) {
        dsleep_v &= ~PMU_DCDC_ANA2_FLAG;
        val |= PMU_BUCK_HV_ANA_DSLEEP;
    } else {
        val &= ~PMU_BUCK_HV_ANA_DSLEEP;
    }
    pmu_write(PMU_REG_BUCK_VCOMP, val);
#endif

    ret = pmu_read(PMU_REG_BUCK_VOLT, &val);
    if(ret == 0)
    {
        val &= ~PMU_BUCK_VANA_DSLEEP_MASK;
        val &= ~PMU_BUCK_VANA_NORMAL_MASK;
        val |= PMU_BUCK_VANA_DSLEEP(dsleep_v);
        val |= PMU_BUCK_VANA_NORMAL(normal_v);
        pmu_write(PMU_REG_BUCK_VOLT, val);
    }
}

static void pmu_ana_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    if (mode == PMU_POWER_MODE_ANA_DCDC || mode == PMU_POWER_MODE_DIG_DCDC) {
#ifdef PMU_DCDC_ANA2_SUPPORT
        pmu_dcdc_ana_set_volt(ana_act_dcdc, ana_lp_dcdc);
        hal_sys_timer_delay_us(PMU_VANA_STABLE_TIME_US);
#else
        uint16_t old_act_dcdc;
        uint16_t old_lp_dcdc;

        pmu_dcdc_ana_get_volt(&old_act_dcdc, &old_lp_dcdc);
        if (old_act_dcdc < ana_act_dcdc) {
            while (old_act_dcdc++ < ana_act_dcdc) {
                pmu_dcdc_ana_set_volt(old_act_dcdc, ana_lp_dcdc);
            }
            hal_sys_timer_delay_us(PMU_VANA_STABLE_TIME_US);
        } else if (old_act_dcdc != ana_act_dcdc || old_lp_dcdc != ana_lp_dcdc) {
            pmu_dcdc_ana_set_volt(ana_act_dcdc, ana_lp_dcdc);
        }
#endif
    }
}

static void pmu_dcdc_dig_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_BUCK_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, PMU_BUCK_VCORE_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, PMU_BUCK_VCORE_DSLEEP);
    }
}

static void pmu_dcdc_dig_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    int ret;
    unsigned short val;

#ifdef PMU_DCDC_DIG2_SUPPORT
    uint32_t metal_id;
    bool dig2_en = false;

    metal_id = hal_get_chip_metal_id();

    if (normal_v & PMU_DCDC_DIG2_FLAG) {
        if (metal_id >= HAL_CHIP_METAL_ID_2) {
            normal_v &= ~PMU_DCDC_DIG2_FLAG;
            dig2_en = true;
        } else {
            ASSERT(false, "Bad vcore dcdc2 cfg");
        }
    }
#endif

    ret = pmu_read(PMU_REG_BUCK_VOLT, &val);
    if(ret == 0)
    {
        val &= ~PMU_BUCK_VCORE_NORMAL_MASK;
        val &= ~PMU_BUCK_VCORE_DSLEEP_MASK;
        val |= PMU_BUCK_VCORE_NORMAL(normal_v);
        val |= PMU_BUCK_VCORE_DSLEEP(dsleep_v);
        pmu_write(PMU_REG_BUCK_VOLT, val);
    }

#ifdef PMU_DCDC_DIG2_SUPPORT
    if (metal_id >= HAL_CHIP_METAL_ID_2) {
        ret = pmu_read(PMU_REG_RESERVED_ANA, &val);
        if (dig2_en) {
            val |= PMU_RESERVED_DIG_VOLT_BIT5 | PMU_RESERVED_DIG_VOLT_BIT6;
        } else {
            val &= ~(PMU_RESERVED_DIG_VOLT_BIT5 | PMU_RESERVED_DIG_VOLT_BIT6);
        }
        pmu_write(PMU_REG_RESERVED_ANA, val);
    }
#endif
}

static void BOOT_TEXT_SRAM_LOC pmu_dig_get_target_volt(uint16_t *ldo, uint16_t *dcdc)
{
    uint16_t ldo_volt;
    uint16_t dcdc_volt;

    if (0) {
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    } else if (pmu_vcore_req & PMU_VCORE_SYS_FREQ_HIGH) {
        ldo_volt = PMU_VDIG_1_35V;
        dcdc_volt = PMU_DCDC_DIG_1_4V;
#endif
    } else if (pmu_vcore_req & (PMU_VCORE_IIR_FREQ_HIGH | PMU_VCORE_USB_ENABLED)) {
        ldo_volt = PMU_VDIG_1_2V;
        dcdc_volt = PMU_DCDC_DIG_1_2V;
    } else if (pmu_vcore_req & (PMU_VCORE_FLASH_WRITE_ENABLED | PMU_VCORE_FLASH_FREQ_HIGH |
            PMU_VCORE_IIR_FREQ_MEDIUM | PMU_VCORE_SYS_FREQ_MEDIUM)) {
        ldo_volt = PMU_VDIG_1_1V;
        dcdc_volt = PMU_DCDC_DIG_1_1V;
    } else if (pmu_vcore_req & (PMU_VCORE_FLASH_FREQ_MEDIUM | PMU_VCORE_PSRAM_FREQ_HIGH)) {
        ldo_volt = PMU_VDIG_1_0V;
        dcdc_volt = PMU_DCDC_DIG_1_0V;
    } else {
        // Common cases

        // CAUTION:
        // 1) Some chips cannot work when vcore is below 1.0V
        // 2) Vcore DCDC -> LDO, or LDO 0.95V -> 0.9V, might occasionaly cause a negative
        //    glitch (about 50mV) . Flash controller is sensitive to vcore, and might not
        //    work below 0.9V.
        ldo_volt = PMU_VDIG_1_0V;
        dcdc_volt = PMU_DCDC_DIG_1_0V;
    }

#if defined(PROGRAMMER) && !defined(PMU_FULL_INIT)
    // Try to keep the same vcore voltage as ROM (hardware default)
    if (ldo_volt < PMU_VDIG_1_1V) {
        ldo_volt = PMU_VDIG_1_1V;
    }
    if (dcdc_volt < PMU_DCDC_DIG_1_1V) {
        dcdc_volt = PMU_DCDC_DIG_1_1V;
    }
#endif

#if defined(MTEST_ENABLED) && defined(MTEST_VOLT)
#ifdef DIG_DCDC_MODE
    dcdc_volt = MTEST_VOLT;
#else
    ldo_volt = MTEST_VOLT;
#endif
#endif

    if (ldo) {
        *ldo = ldo_volt;
    }
    if (dcdc) {
        *dcdc = dcdc_volt;
    }
}

static void pmu_dig_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    uint16_t dcdc_volt;
    uint16_t ldo_volt;

    lock = int_lock();

    if (mode == PMU_POWER_MODE_NONE) {
        mode = pmu_power_mode;
    }

    pmu_dig_get_target_volt(&ldo_volt, &dcdc_volt);
#ifdef VCORE_LDO_ON
    dig_act_ldo = ldo_volt;
#endif

#ifdef PMU_DCDC_DIG2_SUPPORT
    pmu_module_set_volt(PMU_DIG, dig_lp_ldo, ldo_volt);
    pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
    hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
#else
    uint16_t old_act_dcdc, old_lp_dcdc;
    uint16_t old_act_ldo, old_lp_ldo;
    bool volt_inc = false;

    pmu_module_get_volt(PMU_DIG, &old_lp_ldo, &old_act_ldo);
    pmu_dcdc_dig_get_volt(&old_act_dcdc, &old_lp_dcdc);

    if (mode_change || mode == PMU_POWER_MODE_DIG_DCDC) {
        if (old_act_dcdc < dcdc_volt) {
            volt_inc = true;
            while (old_act_dcdc++ < dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_act_dcdc, dig_lp_dcdc);
            }
        } else if (old_act_dcdc != dcdc_volt || old_lp_dcdc != dig_lp_dcdc) {
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
    }

    if (mode_change || mode != PMU_POWER_MODE_DIG_DCDC) {
        if (old_act_ldo < ldo_volt) {
            volt_inc = true;
            while (old_act_ldo++ < ldo_volt) {
                pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_act_ldo);
            }
        } else if (old_act_ldo != ldo_volt || old_lp_ldo != dig_lp_ldo) {
            pmu_module_set_volt(PMU_DIG, dig_lp_ldo, ldo_volt);
        }
    }

    if (mode_change == 0) {
        // Update the voltage of the other mode
        if (mode == PMU_POWER_MODE_DIG_DCDC) {
            pmu_module_set_volt(PMU_DIG, dig_lp_ldo, ldo_volt);
        } else {
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
    }

    if (volt_inc) {
        hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US);
    }
#endif

    int_unlock(lock);
}

static void pmu_ldo_mode_en(void)
{
    unsigned short val;

    // Enable vana ldo
    pmu_module_config(PMU_ANA,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    // Enable vcore ldo
    pmu_module_config(PMU_DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

    pmu_ana_set_volt(1, PMU_POWER_MODE_LDO);
    pmu_dig_set_volt(1, PMU_POWER_MODE_LDO);

    hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);

    // Disable vana dcdc and vcore dcdc
    pmu_read(PMU_REG_DCDC_CFG, &val);
    val = val & PMU_TEST_MODE_MASK;
    pmu_write(PMU_REG_DCDC_CFG, val);
}

static void pmu_dcdc_ana_mode_en(void)
{
    unsigned short val;

    if (pmu_power_mode == PMU_POWER_MODE_DIG_DCDC) {
        // Enable vcore ldo
        pmu_module_config(PMU_DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

        pmu_ana_set_volt(1, PMU_POWER_MODE_ANA_DCDC);
        pmu_dig_set_volt(1, PMU_POWER_MODE_ANA_DCDC);

#ifdef VCORE_LDO_ON
        hal_sys_timer_delay_us(PMU_VCORE_STABLE_TIME_US * 4);
#else
        hal_sys_timer_delay_us(PMU_LDO_PU_STABLE_TIME_US);
#endif
    }

    // Enable vana dcdc & disable vcore dcdc
    pmu_read(PMU_REG_DCDC_CFG, &val);
    val |= PMU_DCDC_ANA_LP_MODE_EN | PMU_DCDC_ANA_DSLEEP_ON;
    val |= PMU_DCDC_ANA_DR | PMU_DCDC_ANA_EN;
    val &= ~(PMU_BUCK_CC_MODE_EN | PMU_DCDC_DIG_DR | PMU_DCDC_DIG_EN);
    pmu_write(PMU_REG_DCDC_CFG, val);

    if (pmu_power_mode != PMU_POWER_MODE_DIG_DCDC) {
        pmu_ana_set_volt(1, PMU_POWER_MODE_ANA_DCDC);
        pmu_dig_set_volt(1, PMU_POWER_MODE_ANA_DCDC);

        hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

        // Disable vana ldo
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    }
}

static void pmu_dcdc_dual_mode_en(void)
{
    unsigned short val;

    // Enable vana dcdc and vcore dcdc
    pmu_read(PMU_REG_DCDC_CFG, &val);
    val |= PMU_BUCK_CC_MODE_EN;
    val |= PMU_DCDC_DIG_DR | PMU_DCDC_DIG_EN;
    val |= PMU_DCDC_ANA_DR | PMU_DCDC_ANA_EN;
    // Note that vcore dcdc has no lp mode,
    // so the power mode must be changed to ANA_DCDC before sleep
    val &= ~(PMU_DCDC_ANA_LP_MODE_EN | PMU_DCDC_ANA_DSLEEP_ON);
    pmu_write(PMU_REG_DCDC_CFG, val);

    pmu_ana_set_volt(1, PMU_POWER_MODE_DIG_DCDC);
    pmu_dig_set_volt(1, PMU_POWER_MODE_DIG_DCDC);

    hal_sys_timer_delay_us(PMU_DCDC_PU_STABLE_TIME_US);

    // Disable vana ldo
    pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    // Disable vcore ldo
    pmu_module_config(PMU_DIG,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
}

void pmu_mode_change(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    bool update_mode;

    if (pmu_power_mode == mode)
        return;

    update_mode = true;

    lock = int_lock();

    if (mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_dual_mode_en();
    } else if (mode == PMU_POWER_MODE_ANA_DCDC) {
        pmu_dcdc_ana_mode_en();
    } else if (mode == PMU_POWER_MODE_LDO) {
        pmu_ldo_mode_en();
    } else {
        update_mode = false;
    }

    if (update_mode) {
        pmu_power_mode = mode;
    }

    int_unlock(lock);
}

void pmu_sleep_en(unsigned char sleep_en)
{
    int ret;
    unsigned short val;

    ret = pmu_read(PMU_REG_SLEEP_CFG, &val);
    if(ret == 0)
    {
        if(sleep_en)
        {
            val |= PMU_SLEEP_EN;
        }
        else
        {
            val &= ~PMU_SLEEP_EN;
        }
        pmu_write(PMU_REG_SLEEP_CFG, val);
    }
}

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
static uint32_t pmu_vcodec_mv_to_val(uint16_t mv)
{
    return (mv == 2500 || mv == 3500) ? PMU_CODEC_2_5V : PMU_CODEC_1_8V;
}

static void BOOT_TEXT_FLASH_LOC pmu_dig_init_volt(void)
{
    uint16_t ldo_volt;
    uint16_t val;

    pmu_dig_get_target_volt(&ldo_volt, NULL);

    pmu_read(PMU_REG_DIG_CFG, &val);
    if (GET_BITFIELD(val, PMU_DIG_NORMAL_VOLT) < ldo_volt) {
        val = SET_BITFIELD(val, PMU_DIG_NORMAL_VOLT, ldo_volt);
        pmu_write(PMU_REG_DIG_CFG, val);
    }
}
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE)
void BOOT_TEXT_FLASH_LOC bbpll_freq_pll_config(uint32_t freq)
{
    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)

    uint64_t PLL_cfg_val;
    uint16_t v[3];
    uint16_t val;

    PLL_cfg_val = ((uint64_t)(1 << 28) * freq + 26000000 / 2) / 26000000;
    v[0] = PLL_cfg_val & 0xFFFF;
    v[1] = (PLL_cfg_val >> 16) & 0xFFFF;
    v[2] = (PLL_cfg_val >> 32) & 0xFFFF;

    pmu_write(0xd7, v[0]);
    pmu_write(0xd6, v[1]);
    pmu_read(0xd5, &val);
    val &= ~(0x7000);
    val |= v[2]<<13;
    pmu_write(0xd5, val);

    pmu_read(0xd9, &val);
    val &= ~0x1;
    pmu_write(0xd9, val);
    val |= 0x1;
    pmu_write(0xd9, val);

    hal_sys_timer_delay(US_TO_TICKS(100));
}
#endif

int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))

    int ret;
    uint16_t val;
    enum PMU_POWER_MODE_T mode;
    enum HAL_CHIP_METAL_ID_T metal_id;

    ASSERT(vcodec_mv >= 2500 || vcodec_off, "Invalid vcodec cfg: vcodec_mv=%u vcodec_off=%d", vcodec_mv, vcodec_off);
    ASSERT(vcodec_mv >= 2500 ||
            (vcodec_mv == 1500 && ana_act_dcdc == PMU_DCDC_ANA_1_5V) ||
            (vcodec_mv == 1600 && ana_act_dcdc == PMU_DCDC_ANA_1_6V) ||
            (vcodec_mv == 1800 && ana_act_dcdc == PMU_DCDC_ANA_1_8V) ||
            (vcodec_mv == 2000 && ana_act_dcdc == PMU_DCDC_ANA_2_0V),
        "Invalid vcodec/vana cfg: vcodec_mv=%u ana_act_dcdc=%u", vcodec_mv, ana_act_dcdc);

    metal_id = hal_get_chip_metal_id();

#if defined(__SBC_FUNC_IN_ROM_VBEST2000__) || defined(__SBC_FUNC_IN_ROM_VBEST2000_ONLYSBC__)
    if (metal_id < HAL_CHIP_METAL_ID_2 || metal_id == HAL_CHIP_METAL_ID_3) {
        ASSERT(false, "SBC_FUNC_IN_ROM is NOT supported on version A/C/E chips");
    }
#endif

#ifdef USB_HIGH_SPEED
    if (hal_cmu_get_pll_status(HAL_CMU_PLL_USB) == 0) {
#ifdef __AUDIO_RESAMPLE__
        // RF: Disable BBPLL
        pmu_write(PMU_REG_RF_9D, 0x0002);
#else
        // RF: 0x1640 for BBPLL 760M. The old value is 0x2840 for 960M
        pmu_read(PMU_REG_RF_1EF, &val);
        val = (val & ~(REG_WIFI_BBPLL_SPEED_BT_MASK | REG_WIFI_BBPLL_BW_SEL_BT_MASK)) |
            REG_WIFI_BBPLL_SPEED_BT(3) | REG_WIFI_BBPLL_BW_SEL_BT(1);
        pmu_write(PMU_REG_RF_1EF, val);
#endif
    }
#endif

    // Disable and clear all PMU irqs by default
    pmu_write(PMU_REG_INT_MASK, 0);
    pmu_write(PMU_REG_INT_EN, 0);
    // PMU irqs cannot be cleared by PMU soft reset
    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    pmu_write(PMU_REG_CHARGER_STATUS, val);
    pmu_read(PMU_REG_INT_STATUS, &val);
    pmu_write(PMU_REG_INT_CLR, val);

    // Allow PMU to sleep when power key is pressed
    ret = pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    if (ret == 0) {
        val &= ~PMU_POWERKEY_WAKEUP_OSC_EN;
        pmu_write(PMU_REG_POWER_KEY_CFG, val);
    }

    // Init DCDC settings
    pmu_write(PMU_REG_BIAS_CFG, 0x3155);
    if (metal_id >= HAL_CHIP_METAL_ID_2) {
        pmu_write(PMU_REG_BUCK_FREQ, 0xD800);
    } else {
        pmu_write(PMU_REG_BUCK_FREQ, 0xD400);
    }
    pmu_write(PMU_REG_BUCK_VCOMP, 0x44C0);

    val = 0;
#ifdef VANA_DEC_50MV
    if (metal_id >= HAL_CHIP_METAL_ID_1) {
        val |= PMU_RESERVED_ANA_VOLT_DEC_50MV;
    }
#endif
    if (metal_id >= HAL_CHIP_METAL_ID_2) {
        val |= PMU_RESERVED_BUCK_BIT7 | PMU_RESERVED_BUCK_BIT8;
    }
    pmu_write(PMU_REG_RESERVED_ANA, val);

#if 0
    pmu_read(PMU_REG_IO_CFG, &val);
    val |= PMU_IO_LP_EN_DR | PMU_IO_LP_EN_REG;
    pmu_write(PMU_REG_IO_CFG, val);
    pmu_read(PMU_REG_GP_CFG, &val);
    val |= PMU_GP_LP_EN_DR | PMU_GP_LP_EN_REG;
    pmu_write(PMU_REG_GP_CFG, val);
    pmu_read(PMU_REG_USB_CFG, &val);
    val |= PMU_USB_LP_EN_DR | PMU_USB_LP_EN_REG;
    pmu_write(PMU_REG_USB_CFG, val);
    pmu_read(PMU_REG_CRYSTAL_CFG, &val);
    val |= PMU_CRYSTAL_LP_EN_DR | PMU_CRYSTAL_LP_EN_REG;
    pmu_write(PMU_REG_CRYSTAL_CFG, val);
#endif

#ifndef NO_SLEEP
    pmu_sleep_en(1);  //enable sleep
#endif

    if (vcrystal_off) {
        pmu_read(PMU_REG_CRYSTAL_CFG, &val);
        val |= PMU_CRYSTAL_LP_EN_DR | PMU_CRYSTAL_LP_EN_REG;
        pmu_write(PMU_REG_CRYSTAL_CFG, val);

        if (metal_id >= HAL_CHIP_METAL_ID_2) {
            pmu_module_config(PMU_CRYSTAL,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable 26m ldo (short with vana)
        } else {
            pmu_module_config(PMU_CRYSTAL,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable 26m ldo in sleep
            pmu_module_set_volt(PMU_CRYSTAL, 0, 0);
        }
    } else {
#ifdef CLK_32K_FROM_OSC
        pmu_module_config(PMU_CRYSTAL,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#else
        pmu_module_config(PMU_CRYSTAL,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable 26m ldo in sleep
#endif
    }

#ifdef VMEM_ON
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON); //enable VMEM
    pmu_module_set_volt(PMU_MEM, PMU_VMEM_1_8V, PMU_VMEM_1_8V);
#else
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VMEM
#endif

    pmu_module_config(PMU_GP,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VGP

#if !defined(PROGRAMMER) && !defined(VUSB_ON)
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VUSB
#endif

#ifdef __BEST_FLASH_VIA_ANA__
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VIO in sleep
#else
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON); //enable VIO in sleep
#endif
    pmu_module_set_volt(PMU_IO, vio_lp, vio_act_normal);

    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VCODEC
    val = pmu_vcodec_mv_to_val(vcodec_mv);
    pmu_module_set_volt(PMU_CODEC, val, val);
    if (vcodec_mv == 3500) {
        pmu_read(PMU_REG_RESERVED_ANA, &val);
        val |= PMU_RESERVED_CODEC_VOLT_HIGH;
        pmu_write(PMU_REG_RESERVED_ANA, val);
    }

#ifdef VCORE_LDO_ON
    // Init dig_act_ldo
    pmu_dig_get_target_volt(&dig_act_ldo, NULL);
#endif

#ifdef DIG_DCDC_MODE
    mode = PMU_POWER_MODE_DIG_DCDC;
#elif defined(ANA_DCDC_MODE)
    mode = PMU_POWER_MODE_ANA_DCDC;
#else // LDO_MODE
    mode = PMU_POWER_MODE_LDO;
#endif

    pmu_mode_change(mode);

#ifdef RTC_ENABLE
    pmu_rtc_restore_context();
#endif

#if defined(MCU_HIGH_PERFORMANCE_MODE)
    // bbpll_dig_rc
    pmu_read(0x99, &val);
    val = (val & ~(0x7 << 7)) | (0x7 << 7);
    pmu_write(0x99, val);

    pmu_high_performance_mode_enable(true);
#endif

    //enable VPA LDO
    if (metal_id >= HAL_CHIP_METAL_ID_5) {
        pmu_read(PMU_REG_VPA_CFG, &val);
        val = (val & ~(0x1 << 5)) | (0x1 << 5);
        pmu_write(PMU_REG_VPA_CFG, val);
    }

#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

    return 0;
}

void pmu_sleep(void)
{
    if (pmu_power_mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_active_power_mode = pmu_power_mode;
        pmu_mode_change(PMU_POWER_MODE_ANA_DCDC);
    }

#if defined(CLK_32K_FROM_OSC) && defined(ANA_26M_X4_ENABLE)
    // Disable X4
    pmu_write(0xA4, 0x1000);
#endif
}

void pmu_wakeup(void)
{
#if defined(CLK_32K_FROM_OSC) && defined(ANA_26M_X4_ENABLE)
    // Enable X4
    pmu_write(0xA4, 0x5000);
#endif

    if (pmu_active_power_mode != PMU_POWER_MODE_NONE) {
        pmu_mode_change(pmu_active_power_mode);
        pmu_active_power_mode = PMU_POWER_MODE_NONE;
    }
}

void pmu_codec_config(int enable)
{
    if (!vcodec_off) {
        if (enable) {
            pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        } else {
            pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        }
    }
}

void pmu_codec_hppa_enable(int enable)
{
    enum HAL_CHIP_METAL_ID_T metal_id;
    uint16_t val;
    uint32_t hppa_volt;

    metal_id = hal_get_chip_metal_id();

    if (enable) {
        if (audio_output_diff) {
            pmu_write(PMU_REG_HPPA_FREQ, 0x4859);
            if (vcodec_mv == 1500) {
                // 1.6v DCDC_HPPA
                val = 0xC010;
            } else if (vcodec_mv == 1600) {
                // 1.7v DCDC_HPPA
                val = 0xC110;
            } else if (vcodec_mv == 1800) {
                // 1.9v DCDC_HPPA
                val = 0xC410;
			} else if (vcodec_mv == 2000) {
                //
                val = 0xC810;
            } else if (vcodec_mv == 2500) {
                // 2.6v DCDC_HPPA
                val = 0xCD10;
            } else if (vcodec_mv == 3500) {
                // 3.6v DCDC_HPPA
                val = 0xDB10;
            } else {
                ASSERT(false, "Invalid vcodec volt: %u", vcodec_mv);
            }
            pmu_write(PMU_REG_HPPA_BUCK1, val);
            if (metal_id >= HAL_CHIP_METAL_ID_2) {
                val = 0xE000;
            } else {
                val = 0x6000;
            }
            pmu_write(PMU_REG_HPPA_CFG, val);
        } else {
            pmu_write(PMU_REG_CP_PU, 0xFF40);
            pmu_write(PMU_REG_HPPA_FREQ, 0x6060);
            pmu_write(PMU_REG_HPPA_BUCK0, 0xC000);
            // Set HPPA volt to lowest to reduce the pop when enabling HPPA
            pmu_write(PMU_REG_HPPA_BUCK1, 0xC030);
            pmu_write(PMU_REG_HPPA_BUCK2, 0xFD00);
            pmu_write(PMU_REG_HPPA_BUCK3, 0xDF20);
            pmu_write(PMU_REG_HPPA_BUCK4, 0xC100);
            pmu_write(PMU_REG_HPPA_BUCK5, 0xDF20);
            if (metal_id >= HAL_CHIP_METAL_ID_2) {
                val = 0xE440;
            } else {
                val = 0x6440;
            }
            pmu_write(PMU_REG_HPPA_CFG, val);
            osDelay(2);
            // Ramp up HPPA volt to normal
            val = 0xC030;
#ifdef DAC_CLASSG_ENABLE
            hppa_volt = 0x1F;
#else
            hppa_volt = 7;
#endif
            for (uint32_t i = 1; i <= hppa_volt; i++) {
                val = (val & ~(0x1F << 8)) | (i << 8);
                pmu_write(PMU_REG_HPPA_BUCK1, val);
                hal_sys_timer_delay_us(20);
            }
#ifdef DAC_CLASSG_ENABLE
            // Clear hppa volt dr
            pmu_read(PMU_REG_HPPA_BUCK1, &val);
            val &= ~(1 << 4);
            pmu_write(PMU_REG_HPPA_BUCK1, val);
#endif
        }
    } else {
        if (metal_id >= HAL_CHIP_METAL_ID_2) {
            val = 0x8000;
        } else {
            val = 0;
        }
        pmu_write(PMU_REG_HPPA_CFG, val);
        if (!audio_output_diff) {
            pmu_write(PMU_REG_CP_PU, 0);
        }
    }
}

void pmu_codec_mic_bias_enable(uint32_t map)
{
    uint16_t val_a, val_b;
#ifdef __KNOWLES
	val_a = PMU_MIC_LDO_RES(0xF);
	val_b = 0;
	val_a |= PMU_MIC_BIASA_EN | PMU_MIC_BIASA_VSEL(0xA);
	val_b |= PMU_MIC_LDO_EN;
#else
    // Increase LDO_RES if higher vmic is required
#ifdef DIGMIC_HIGH_VOLT
    val_a = PMU_MIC_LDO_RES(0xF);
#else
    val_a = PMU_MIC_LDO_RES(7);
#endif
    val_b = 0;
    if (map & AUD_VMIC_MAP_VMIC1) {
        val_a |= PMU_MIC_BIASA_EN | PMU_MIC_BIASA_ENLPF | PMU_MIC_BIASA_VSEL(0xF);
        val_b |= PMU_MIC_LDO_EN;
    }
    if (map & AUD_VMIC_MAP_VMIC2) {
        val_b |= PMU_MIC_BIASB_EN | PMU_MIC_BIASB_ENLPF | PMU_MIC_BIASB_VSEL(0xF) | PMU_MIC_LDO_EN;
    }
#endif
    pmu_write(PMU_REG_MIC_BIAS_A, val_a);
    pmu_write(PMU_REG_MIC_BIAS_B, val_b);
}

void pmu_codec_adc_pre_start(void)
{
}

void pmu_codec_adc_post_start(void (*delay_ms)(uint32_t))
{
}

void pmu_codec_dac_pre_start(void)
{
}

void pmu_codec_dac_post_start(void (*delay_ms)(uint32_t))
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

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
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

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_flash_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;
    uint16_t val;
    bool flash_volt_high;

    pmu_get_efuse(PMU_EFUSE_PAGE_BONDING, &val);
    if (val & 0x0001) {
        flash_volt_high = true;
    } else {
        flash_volt_high = false;
    }

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_FLASH_FREQ_HIGH | PMU_VCORE_FLASH_FREQ_MEDIUM);
    if (flash_volt_high) {
        if (freq > 52000000) {
            pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_HIGH;
        } else if (freq == 52000000) {
            pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_MEDIUM;
        }
    } else {
#ifndef OSC_26M_X4_AUD2BB
        if (freq > 52000000) {
#ifdef USB_HIGH_SPEED
            // The real max freq is 120M
            pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_HIGH;
#else
            // The real max freq is about 90~100M. It seems to work at VCORE 0.9V.
            //pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_MEDIUM;
#endif
        }
#endif
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_psram_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;

    lock = int_lock();
    if (freq > 52000000) {
        pmu_vcore_req |= PMU_VCORE_PSRAM_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_PSRAM_FREQ_HIGH;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
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
    uint32_t lock;

    lock = int_lock();
    pmu_vcore_req &= ~(PMU_VCORE_IIR_FREQ_HIGH | PMU_VCORE_IIR_FREQ_MEDIUM);
    if (freq >= 52000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_FREQ_HIGH;
    } else if (freq > 26000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_FREQ_MEDIUM;
    }
    int_unlock(lock);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
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
            // The real freq is 400M
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_HIGH;
        } else {
            pmu_vcore_req |= PMU_VCORE_SYS_FREQ_MEDIUM;
        }
    } else {
#ifndef OSC_26M_X4_AUD2BB
        if (freq == HAL_CMU_FREQ_104M && high_perf_on) {
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

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        pmu_dig_init_volt();
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
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
        // Restore the default div
        pmu_pll_div_set(HAL_CMU_PLL_USB, PMU_PLL_DIV_DIG, 4);
        // Restore the default PLL freq (960M)
        bbpll_freq_pll_config(480 * 1000000 * 2);
    }

    pmu_sys_freq_config(hal_sysfreq_get_hw_freq());

    if (enable) {
        // Change freq first, and then change divider.
        // Otherwise there will be an instant 480M freq sent to digital domain.

        bbpll_freq_pll_config(high_perf_freq_mhz * 1000000 * 2);
        // bbpll_divn_480m_bt
        pmu_pll_div_set(HAL_CMU_PLL_USB, PMU_PLL_DIV_DIG, 2);
    }
#endif
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{
    unsigned short ldo_on, deep_sleep_on;
#ifdef USB_HIGH_SPEED
    uint16_t val;
    uint32_t lock;
#endif

    if (type == PMU_USB_CONFIG_TYPE_NONE) {
#ifdef USB_HIGH_SPEED
        pmu_read(PMU_REG_USBPHY, &val);
        val = (val & ~PMU_RESETN_USBPHY_REG) | PMU_RESETN_USBPHY_DR;
        pmu_write(PMU_REG_USBPHY, val);

        pmu_read(PMU_REG_ANA_167, &val);
        val &= ~(1 << 0);
        pmu_write(PMU_REG_ANA_167, val);
#endif

        ldo_on = PMU_LDO_OFF;
        deep_sleep_on = PMU_DSLEEP_MODE_OFF;
    } else {
#ifdef USB_HIGH_SPEED
        pmu_read(PMU_REG_USBPHY, &val);
        val = (val & ~PMU_RESETN_USBPHY_REG) | PMU_RESETN_USBPHY_DR;
        pmu_write(PMU_REG_USBPHY, val);
        val |= PMU_RESETN_USBPHY_REG | PMU_RESETN_USBPHY_DR | PMU_EN_USBDIGPHY_CLK;
        pmu_write(PMU_REG_USBPHY, val);

        pmu_read(PMU_REG_ANA_167, &val);
        val |= (1 << 0);
        pmu_write(PMU_REG_ANA_167, val);
#endif

        ldo_on = PMU_LDO_ON;
        deep_sleep_on = PMU_DSLEEP_MODE_ON;
    }

    pmu_module_config(PMU_USB, PMU_MANUAL_MODE, ldo_on, PMU_LP_MODE_ON, deep_sleep_on);

#ifdef USB_HIGH_SPEED
    lock = int_lock();
    if (type == PMU_USB_CONFIG_TYPE_NONE) {
        pmu_vcore_req &= ~PMU_VCORE_USB_ENABLED;
    } else {
        pmu_vcore_req |= PMU_VCORE_USB_ENABLED;
    }
    int_unlock(lock);
#endif

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

static int pmu_usb_check_pin_status(enum PMU_USB_PIN_CHK_STATUS_T status)
{
    int dp, dm;

    pmu_usb_get_pin_status(&dp, &dm);

    //TRACE(5,"[%X] %s: status=%d dp=%d dm=%d", hal_sys_timer_get(), __FUNCTION__, status, dp, dm);

    // HOST_RESUME: (resume) dp == 0 && dm == 1, (reset) dp == 0 && dm == 0

    if ( (status == PMU_USB_PIN_CHK_DEV_CONN && (dp == 1 && dm == 0)) ||
            (status == PMU_USB_PIN_CHK_DEV_DISCONN && (dp == 0 && dm == 0)) ||
            (status == PMU_USB_PIN_CHK_HOST_RESUME && dp == 0) ) {
        return 1;
    }

    return 0;
}

static void pmu_usb_pin_irq_handler(void)
{
    //TRACE(2,"[%X] %s", hal_sys_timer_get(), __FUNCTION__);

    pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_USB_INSERT);

    if (usb_pin_callback) {
        if (pmu_usb_check_pin_status(usb_pin_status)) {
            pmu_usb_disable_pin_status_check();
            usb_pin_callback(usb_pin_status);
        }
    }
}

int pmu_usb_config_pin_status_check(enum PMU_USB_PIN_CHK_STATUS_T status, PMU_USB_PIN_CHK_CALLBACK callback, int enable)
{
    uint16_t val;
    uint32_t lock;

    //TRACE(3,"[%X] %s: status=%d", hal_sys_timer_get(), __FUNCTION__, status);

    if (status >= PMU_USB_PIN_CHK_STATUS_QTY) {
        return 1;
    }

    NVIC_DisableIRQ(USB_PIN_IRQn);

    lock = int_lock();

    usb_pin_status = status;
    usb_pin_callback = callback;

    // Mask the irq
    pmu_read(PMU_REG_INT_MASK, &val);
    val &= ~PMU_INT_MASK_USB_INSERT;
    pmu_write(PMU_REG_INT_MASK, val);

    // Config pin check
    pmu_read(PMU_REG_USB_PIN_CFG, &val);
    val |= PMU_USB_DEBOUNCE_EN | PMU_USB_NOLS_MODE | PMU_USB_INSERT_DET_EN;
    pmu_write(PMU_REG_USB_PIN_CFG, val);

    pmu_read(PMU_REG_USB_PIN_POL, &val);
    val &= ~(PMU_USB_POL_RX_DP | PMU_USB_POL_RX_DM);
    if (status == PMU_USB_PIN_CHK_DEV_CONN) {
        // Check dp 0->1, dm x->0
    } else if (status == PMU_USB_PIN_CHK_DEV_DISCONN) {
        // Check dp 1->0, dm x->0
        val |= PMU_USB_POL_RX_DP;
    } else if (status == PMU_USB_PIN_CHK_HOST_RESUME) {
        // Check dp 1->0, dm 0->1 (resume) or dm 0->0 (reset)
        val |= PMU_USB_POL_RX_DP;
    }
    pmu_write(PMU_REG_USB_PIN_POL, val);

    if (status != PMU_USB_PIN_CHK_NONE && callback) {
        pmu_read(PMU_REG_INT_EN, &val);
        val |= PMU_INT_EN_USB_INSERT;
        pmu_write(PMU_REG_INT_EN, val);

        pmu_read(PMU_REG_INT_MASK, &val);
        val |= PMU_INT_MASK_USB_INSERT;
        pmu_write(PMU_REG_INT_MASK, val);

        pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_USB_INSERT);
    }

    int_unlock(lock);

    if (enable) {
        // Wait at least 10 cycles of 32K clock for the new status when signal checking polarity is changed
        hal_sys_timer_delay(5);
        pmu_usb_enable_pin_status_check();
    }

    return 0;
}

void pmu_usb_enable_pin_status_check(void)
{
    if (usb_pin_status != PMU_USB_PIN_CHK_NONE && usb_pin_callback) {
        pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_USB_INSERT);
        NVIC_ClearPendingIRQ(USB_PIN_IRQn);

        if (pmu_usb_check_pin_status(usb_pin_status)) {
            pmu_usb_disable_pin_status_check();
            usb_pin_callback(usb_pin_status);
            return;
        }

        NVIC_SetVector(USB_PIN_IRQn, (uint32_t)pmu_usb_pin_irq_handler);
        NVIC_SetPriority(USB_PIN_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(USB_PIN_IRQn);
    }
}

void pmu_usb_disable_pin_status_check(void)
{
    uint16_t val;
    uint32_t lock;

    NVIC_DisableIRQ(USB_PIN_IRQn);

    lock = int_lock();

    pmu_read(PMU_REG_INT_EN, &val);
    val &= ~PMU_INT_EN_USB_INSERT;
    pmu_write(PMU_REG_INT_EN, val);

    pmu_read(PMU_REG_USB_PIN_CFG, &val);
    val &= ~PMU_USB_INSERT_DET_EN;
    pmu_write(PMU_REG_USB_PIN_CFG, val);

    int_unlock(lock);
}

void pmu_usb_get_pin_status(int *dp, int *dm)
{
    uint16_t pol, val;

    pmu_read(PMU_REG_USB_PIN_POL, &pol);
    pmu_read(PMU_REG_USB_PIN_STATUS, &val);

    *dp = (!(pol & PMU_USB_POL_RX_DP)) ^ (!(val & PMU_USB_STATUS_RX_DP));
    *dm = (!(pol & PMU_USB_POL_RX_DM)) ^ (!(val & PMU_USB_STATUS_RX_DM));
}

void pmu_charger_init(void)
{
    unsigned short readval_cfg;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg &= ~(PMU_CHARGE_IN_INTR_MSK | PMU_CHARGE_OUT_INTR_MSK |
        PMU_AC_ON_OUT_EN | PMU_AC_ON_IN_EN | PMU_CHARGE_INTR_EN);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg |= PMU_AC_ON_OUT_EN | PMU_AC_ON_IN_EN | PMU_CHARGE_INTR_EN;
    readval_cfg = SET_BITFIELD(readval_cfg, PMU_AC_ON_DB_VALUE, 8);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);
}

static void pmu_charger_irq_handler(void)
{
    enum PMU_CHARGER_STATUS_T status = PMU_CHARGER_UNKNOWN;
    unsigned short readval;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    pmu_write(PMU_REG_CHARGER_STATUS ,readval);
    int_unlock(lock);
    TRACE(3,"%s REG_%02X=0x%04X", __func__, PMU_REG_CHARGER_STATUS, readval);

    if ((readval & (PMU_AC_ON_DET_IN_MASKED | PMU_AC_ON_DET_OUT_MASKED)) == 0){
        TRACE(1,"%s SKIP", __func__);
        return;
    } else if ((readval & (PMU_AC_ON_DET_IN_MASKED | PMU_AC_ON_DET_OUT_MASKED)) ==
            (PMU_AC_ON_DET_IN_MASKED | PMU_AC_ON_DET_OUT_MASKED)) {
        TRACE(1,"%s DITHERING", __func__);
        hal_sys_timer_delay(2);
    } else {
        TRACE(1,"%s NORMAL", __func__);
    }

    status = pmu_charger_get_status();

    if (charger_irq_handler) {
        charger_irq_handler(status);
    }
}

void pmu_charger_set_irq_handler(PMU_CHARGER_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    charger_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &val);
    val |= PMU_CHARGE_IN_INTR_MSK | PMU_CHARGE_OUT_INTR_MSK;
    pmu_write(PMU_REG_CHARGER_CFG, val);
    int_unlock(lock);

    if (handler) {
        NVIC_SetVector(CHARGER_IRQn, (uint32_t)pmu_charger_irq_handler);
        NVIC_SetPriority(CHARGER_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(CHARGER_IRQn);
        NVIC_EnableIRQ(CHARGER_IRQn);
    } else {
        NVIC_DisableIRQ(CHARGER_IRQn);
    }
}

void pmu_charger_plugin_config(void)
{
    if (IO_VOLT_ACTIVE_RISE < PMU_IO_3_1V) {
        vio_act_rise = PMU_IO_3_1V;
    }
    if (IO_VOLT_SLEEP < PMU_IO_3_2V) {
        vio_lp = PMU_IO_3_2V;
    }
    pmu_viorise_req(PMU_VIORISE_REQ_USER_CHARGER, true);
}

void pmu_charger_plugout_config(void)
{
    vio_act_rise = IO_VOLT_ACTIVE_RISE;
    vio_lp = IO_VOLT_SLEEP;
    pmu_viorise_req(PMU_VIORISE_REQ_USER_CHARGER, false);
}

enum PMU_CHARGER_STATUS_T pmu_charger_get_status(void)
{
    unsigned short readval;
    enum PMU_CHARGER_STATUS_T status;

    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    if (readval & PMU_AC_ON)
        status = PMU_CHARGER_PLUGIN;
    else
        status = PMU_CHARGER_PLUGOUT;

    return status;
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
    pmu_write(PMU_REG_RTC_DIV_1HZ, 32000 - 2);
#endif

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval |= PMU_RTC_POWER_ON_EN | PMU_PU_LPO_DR | PMU_PU_LPO_REG;
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
    readval &= ~(PMU_RTC_POWER_ON_EN | PMU_PU_LPO_DR);
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_enabled(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);

    return !!(readval & PMU_RTC_POWER_ON_EN);
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

    pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_RTC1);

    pmu_write(PMU_REG_RTC_MATCH1_LOW, low);
    pmu_write(PMU_REG_RTC_MATCH1_HIGH, high);

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval |= PMU_INT_EN_RTC1;
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
    readval &= ~PMU_INT_EN_RTC1;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);

    pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_RTC1);
}

int BOOT_TEXT_SRAM_LOC pmu_rtc_alarm_status_set(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_EN, &readval);

    return !!(readval & PMU_INT_EN_RTC1);
}

int pmu_rtc_alarm_alerted()
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_STATUS, &readval);

    return !!(readval & PMU_INT_STATUS_RTC1);
}

static void pmu_rtc_irq_handler(void)
{
    uint32_t seconds;

    if (pmu_rtc_alarm_alerted()) {
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
        readval |= PMU_INT_MASK_RTC1;
    } else {
        readval &= ~PMU_INT_MASK_RTC1;
    }
    pmu_write(PMU_REG_INT_MASK, readval);
    int_unlock(lock);

    if (handler) {
        NVIC_SetVector(RTC_IRQn, (uint32_t)pmu_rtc_irq_handler);
        NVIC_SetPriority(RTC_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(RTC_IRQn);
        NVIC_EnableIRQ(RTC_IRQn);
    } else {
        NVIC_DisableIRQ(RTC_IRQn);
    }
}
#endif

void pmu_viorise_req(enum PMU_VIORISE_REQ_USER_T user, bool rise)
{
    uint32_t lock;

    lock = int_lock();
    if (rise) {
        if (vio_risereq_map == 0) {
            pmu_module_set_volt(PMU_IO,vio_lp,vio_act_rise);
        }
        vio_risereq_map |= (1 << user);
    } else {
        vio_risereq_map &= ~(1 << user);
        if (vio_risereq_map == 0) {
            pmu_module_set_volt(PMU_IO,vio_lp,vio_act_normal);
        }
    }
    int_unlock(lock);
}

int pmu_debug_config_ana(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_codec(uint16_t volt)
{
#ifdef ANC_PROD_TEST
    if (volt == 1500 || volt == 1600 || volt == 1800 || volt == 2500 || volt == 3500) {
        vcodec_mv = volt;
    } else {
        return 1;
    }

    if (volt <= 1800) {
        if (volt == 1500) {
            ana_act_dcdc = PMU_DCDC_ANA_1_5V;
        } else if (volt == 1600) {
            ana_act_dcdc = PMU_DCDC_ANA_1_6V;
        } else if (volt == 1800) {
            ana_act_dcdc = PMU_DCDC_ANA_1_8V;
        }
        vcodec_off = true;
    } else {
        vcodec_off = false;
    }
#endif
    return 0;
}

int pmu_debug_config_vcrystal(bool on)
{
#ifdef ANC_PROD_TEST
    vcrystal_off = !on;
#endif
    return 0;
}

int pmu_debug_config_audio_output(bool diff)
{
#ifdef ANC_PROD_TEST
    audio_output_diff = diff;
#endif
    return 0;
}

void pmu_debug_reliability_test(int stage)
{
    uint16_t volt;

    if (stage == 0) {
        volt = PMU_DCDC_ANA_1_8V;
    } else {
        volt = PMU_DCDC_ANA_1_9V;
    }
    pmu_dcdc_ana_set_volt(volt, ana_lp_dcdc);
}

void pmu_led_set_direction(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_DIR_T dir)
{
    uint16_t val;
    uint16_t val_rx;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        pmu_read(PMU_REG_TRNG, &val_rx);
        if (pin == HAL_GPIO_PIN_LED1) {
            if (dir == HAL_GPIO_DIR_IN) {
                val |= PMU_LED_IO1_OENB;
                val_rx |= REG_LED_IO1_RX_EN;
            } else {
                val &= ~PMU_LED_IO1_OENB;
                val_rx &= ~REG_LED_IO1_RX_EN;
            }
        } else {
            if (dir == HAL_GPIO_DIR_IN) {
                val |= PMU_LED_IO2_OENB;
                val_rx |= REG_LED_IO2_RX_EN;
            } else {
                val &= ~PMU_LED_IO2_OENB;
                val_rx &= ~REG_LED_IO2_RX_EN;
            }
        }
        pmu_write(PMU_REG_LED_CFG, val);
        pmu_write(PMU_REG_TRNG, val_rx);
        int_unlock(lock);
    }
}

enum HAL_GPIO_DIR_T pmu_led_get_direction(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            return (val & PMU_LED_IO1_OENB) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        } else {
            return (val & PMU_LED_IO2_OENB) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        }
    } else {
        return HAL_GPIO_DIR_IN;
    }
}

void pmu_led_set_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    enum PMU_LED_VOLT_T {
        PMU_LED_VOLT_ITF, // Internal 3.3V
        PMU_LED_VOLT_VMEM,
        PMU_LED_VOLT_VIO,
    };
    enum PMU_LED_VOLT_T sel;
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        if (volt == HAL_IOMUX_PIN_VOLTAGE_VIO) {
            sel = PMU_LED_VOLT_VIO;
        } else if (volt == HAL_IOMUX_PIN_VOLTAGE_MEM) {
            sel = PMU_LED_VOLT_VMEM;
        } else {
            sel = PMU_LED_VOLT_ITF;
        }

        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_IOMUX_PIN_LED1) {
            val = SET_BITFIELD(val, PMU_LED_IO1_SEL, sel);
        } else {
            val = SET_BITFIELD(val, PMU_LED_IO2_SEL, sel);
        }
        pmu_write(PMU_REG_LED_CFG, val);
        int_unlock(lock);
    }
}

void pmu_led_set_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_IOMUX_PIN_LED1) {
            val &= ~(PMU_LED_IO1_PDEN | PMU_LED_IO1_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= PMU_LED_IO1_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= PMU_LED_IO1_PDEN;
            }
        } else {
            val &= ~(PMU_LED_IO2_PDEN | PMU_LED_IO2_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENABLE) {
                val |= PMU_LED_IO2_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENABLE) {
                val |= PMU_LED_IO2_PDEN;
            }
        }
        pmu_write(PMU_REG_LED_CFG, val);
        int_unlock(lock);
    }
}

void pmu_ntc_capture_enable(void)
{
    unsigned short val;

    pmu_read(PMU_REG_THERM, &val);
    val = SET_BITFIELD(val, SAR_THERM_GAIN, 2);
    pmu_write(PMU_REG_THERM, val);
}

void pmu_ntc_capture_disable(void)
{
    unsigned short val;

    pmu_read(PMU_REG_THERM, &val);
    val = SET_BITFIELD(val, SAR_THERM_GAIN, 0);
    pmu_write(PMU_REG_THERM, val);
}

void pmu_wdt_set_irq_handler(PMU_WDT_IRQ_HANDLER_T handler)
{
}
