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
#ifndef __REG_DDRPLL_BEST2001_H__
#define __REG_DDRPLL_BEST2001_H__

#include "plat_types.h"

enum DDRPLL_REG_T {
    DDRPLL_REG_30   = 0x30,
    DDRPLL_REG_31,
    DDRPLL_REG_32,
    DDRPLL_REG_33,
    DDRPLL_REG_34,
    DDRPLL_REG_35,
    DDRPLL_REG_36,
    DDRPLL_REG_37,
    DDRPLL_REG_38,
    DDRPLL_REG_39,
    DDRPLL_REG_3A,
    DDRPLL_REG_3B,
    DDRPLL_REG_3C,
    DDRPLL_REG_3D,
    DDRPLL_REG_3E,
    DDRPLL_REG_3F,

    DDRPLL_REG_50   = 0x50,
    DDRPLL_REG_51,
    DDRPLL_REG_52,

    DDRPLL_REG_60   = 0x60,
};

// reg_30
#define REG_DDRPLL_VCO_CAL_VT_SHIFT         0
#define REG_DDRPLL_VCO_CAL_VT_MASK          (0x3 << REG_DDRPLL_VCO_CAL_VT_SHIFT)
#define REG_DDRPLL_VCO_CAL_VT(n)            BITFIELD_VAL(REG_DDRPLL_VCO_CAL_VT, n)
#define REG_DDRPLL_1M_PULSE_MODE            (1 << 2)

// reg_31
#define REG_DDRPLL_CAL_CLK_SEL              (1 << 0)
#define REG_DDRPLL_CPMODE                   (1 << 1)
#define REG_DDRPLL_PATH_EN_SHIFT            2
#define REG_DDRPLL_PATH_EN_MASK             (0x3 << REG_DDRPLL_PATH_EN_SHIFT)
#define REG_DDRPLL_PATH_EN(n)               BITFIELD_VAL(REG_DDRPLL_PATH_EN, n)
#define REG_DDRPLL_PDIV_PSRAM_SHIFT         4
#define REG_DDRPLL_PDIV_PSRAM_MASK          (0x1F << REG_DDRPLL_PDIV_PSRAM_SHIFT)
#define REG_DDRPLL_PDIV_PSRAM(n)            BITFIELD_VAL(REG_DDRPLL_PDIV_PSRAM, n)
#define REG_DDRPLL_PDIV_PSRAM2_SHIFT        9
#define REG_DDRPLL_PDIV_PSRAM2_MASK         (0x1F << REG_DDRPLL_PDIV_PSRAM2_SHIFT)
#define REG_DDRPLL_PDIV_PSRAM2(n)           BITFIELD_VAL(REG_DDRPLL_PDIV_PSRAM2, n)

// reg_32
#define REG_DDRPLL_BUF_VRES_SHIFT           0
#define REG_DDRPLL_BUF_VRES_MASK            (0x7 << REG_DDRPLL_BUF_VRES_SHIFT)
#define REG_DDRPLL_BUF_VRES(n)              BITFIELD_VAL(REG_DDRPLL_BUF_VRES, n)
#define REG_DDRPLL_BW_SEL_SHIFT             3
#define REG_DDRPLL_BW_SEL_MASK              (0xF << REG_DDRPLL_BW_SEL_SHIFT)
#define REG_DDRPLL_BW_SEL(n)                BITFIELD_VAL(REG_DDRPLL_BW_SEL, n)
#define REG_DDRPLL_DIV_DLYCAP_SHIFT         7
#define REG_DDRPLL_DIV_DLYCAP_MASK          (0xF << REG_DDRPLL_DIV_DLYCAP_SHIFT)
#define REG_DDRPLL_DIV_DLYCAP(n)            BITFIELD_VAL(REG_DDRPLL_DIV_DLYCAP, n)

// reg_33
#define REG_DDRPLL_CP_VRES_SHIFT            0
#define REG_DDRPLL_CP_VRES_MASK             (0x7 << REG_DDRPLL_CP_VRES_SHIFT)
#define REG_DDRPLL_CP_VRES(n)               BITFIELD_VAL(REG_DDRPLL_CP_VRES, n)
#define REG_DDRPLL_DIG_VRES_SHIFT           3
#define REG_DDRPLL_DIG_VRES_MASK            (0x7 << REG_DDRPLL_DIG_VRES_SHIFT)
#define REG_DDRPLL_DIG_VRES(n)              BITFIELD_VAL(REG_DDRPLL_DIG_VRES, n)
#define REG_DDRPLL_DIV_FBC_SEL_SHIFT        6
#define REG_DDRPLL_DIV_FBC_SEL_MASK         (0x7 << REG_DDRPLL_DIV_FBC_SEL_SHIFT)
#define REG_DDRPLL_DIV_FBC_SEL(n)           BITFIELD_VAL(REG_DDRPLL_DIV_FBC_SEL, n)
#define REG_DDRPLL_FBRES_SHIFT              9
#define REG_DDRPLL_FBRES_MASK               (0x7 << REG_DDRPLL_FBRES_SHIFT)
#define REG_DDRPLL_FBRES(n)                 BITFIELD_VAL(REG_DDRPLL_FBRES, n)
#define REG_DDRPLL_ICP_SHIFT                12
#define REG_DDRPLL_ICP_MASK                 (0xF << REG_DDRPLL_ICP_SHIFT)
#define REG_DDRPLL_ICP(n)                   BITFIELD_VAL(REG_DDRPLL_ICP, n)

// reg_34
#define REG_DDRPLL_IOFST_SHIFT              0
#define REG_DDRPLL_IOFST_MASK               (0xF << REG_DDRPLL_IOFST_SHIFT)
#define REG_DDRPLL_IOFST(n)                 BITFIELD_VAL(REG_DDRPLL_IOFST, n)
#define REG_DDRPLL_LOCKDET_BYPASS           (1 << 4)
#define REG_DDRPLL_LOCKDET_WIN_SHIFT        5
#define REG_DDRPLL_LOCKDET_WIN_MASK         (0x7 << REG_DDRPLL_LOCKDET_WIN_SHIFT)
#define REG_DDRPLL_LOCKDET_WIN(n)           BITFIELD_VAL(REG_DDRPLL_LOCKDET_WIN, n)
#define REG_DDRPLL_VAR_BIAS_SHIFT           8
#define REG_DDRPLL_VAR_BIAS_MASK            (0x7 << REG_DDRPLL_VAR_BIAS_SHIFT)
#define REG_DDRPLL_VAR_BIAS(n)              BITFIELD_VAL(REG_DDRPLL_VAR_BIAS, n)
#define REG_DDRPLL_TST_VCTL_SHIFT           11
#define REG_DDRPLL_TST_VCTL_MASK            (0xF << REG_DDRPLL_TST_VCTL_SHIFT)
#define REG_DDRPLL_TST_VCTL(n)              BITFIELD_VAL(REG_DDRPLL_TST_VCTL, n)

// reg_35
#define REG_DDRPLL_VCO_ICTRL_SHIFT          0
#define REG_DDRPLL_VCO_ICTRL_MASK           (0xF << REG_DDRPLL_VCO_ICTRL_SHIFT)
#define REG_DDRPLL_VCO_ICTRL(n)             BITFIELD_VAL(REG_DDRPLL_VCO_ICTRL, n)
#define REG_DDRPLL_VCO_VRES_SHIFT           4
#define REG_DDRPLL_VCO_VRES_MASK            (0x7 << REG_DDRPLL_VCO_VRES_SHIFT)
#define REG_DDRPLL_VCO_VRES(n)              BITFIELD_VAL(REG_DDRPLL_VCO_VRES, n)
#define REG_DDRPLL_REFCLK_SEL_SHIFT         7
#define REG_DDRPLL_REFCLK_SEL_MASK          (0x3 << REG_DDRPLL_REFCLK_SEL_SHIFT)
#define REG_DDRPLL_REFCLK_SEL(n)            BITFIELD_VAL(REG_DDRPLL_REFCLK_SEL, n)

// reg_36
#define RESERVED_1_SHIFT                    0
#define RESERVED_1_MASK                     (0xFFFF << RESERVED_1_SHIFT)
#define RESERVED_1(n)                       BITFIELD_VAL(RESERVED_1, n)

// reg_37
#define REG_DDRPLL_CAL_OPT                  (1 << 0)
#define REG_DDRPLL_INIT_DELAY_SHIFT         1
#define REG_DDRPLL_INIT_DELAY_MASK          (0x7 << REG_DDRPLL_INIT_DELAY_SHIFT)
#define REG_DDRPLL_INIT_DELAY(n)            BITFIELD_VAL(REG_DDRPLL_INIT_DELAY, n)
#define REG_DDRPLL_CNT_TIME_SHIFT           4
#define REG_DDRPLL_CNT_TIME_MASK            (0x7 << REG_DDRPLL_CNT_TIME_SHIFT)
#define REG_DDRPLL_CNT_TIME(n)              BITFIELD_VAL(REG_DDRPLL_CNT_TIME, n)
#define REG_DDRPLL_WAIT_TIME_SHIFT          7
#define REG_DDRPLL_WAIT_TIME_MASK           (0x7 << REG_DDRPLL_WAIT_TIME_SHIFT)
#define REG_DDRPLL_WAIT_TIME(n)             BITFIELD_VAL(REG_DDRPLL_WAIT_TIME, n)
#define REG_DDRPLL_CAL_MANUAL               (1 << 10)
#define REG_DDRPLL_CAL_CNT_EN_DR            (1 << 11)
#define REG_DDRPLL_CAL_CNT_EN               (1 << 12)
#define REG_DDRPLL_CAL_EN_DR                (1 << 13)
#define REG_DDRPLL_CAL_EN                   (1 << 14)

// reg_38
#define REG_DDRPLL_VCO_CAPBANK_DR           (1 << 0)
#define REG_DDRPLL_VCO_CAPBANK_SHIFT        1
#define REG_DDRPLL_VCO_CAPBANK_MASK         (0x1FF << REG_DDRPLL_VCO_CAPBANK_SHIFT)
#define REG_DDRPLL_VCO_CAPBANK(n)           BITFIELD_VAL(REG_DDRPLL_VCO_CAPBANK, n)
#define REG_DDRPLL_MULT_SHIFT               10
#define REG_DDRPLL_MULT_MASK                (0x7 << REG_DDRPLL_MULT_SHIFT)
#define REG_DDRPLL_MULT(n)                  BITFIELD_VAL(REG_DDRPLL_MULT, n)
#define REG_DDRPLL_CAL_CLK_FREQ_SHIFT       13
#define REG_DDRPLL_CAL_CLK_FREQ_MASK        (0x3 << REG_DDRPLL_CAL_CLK_FREQ_SHIFT)
#define REG_DDRPLL_CAL_CLK_FREQ(n)          BITFIELD_VAL(REG_DDRPLL_CAL_CLK_FREQ, n)

// reg_39
#define REG_DDRPLL_SDM_FREQWORD_15_0_SHIFT  0
#define REG_DDRPLL_SDM_FREQWORD_15_0_MASK   (0xFFFF << REG_DDRPLL_SDM_FREQWORD_15_0_SHIFT)
#define REG_DDRPLL_SDM_FREQWORD_15_0(n)     BITFIELD_VAL(REG_DDRPLL_SDM_FREQWORD_15_0, n)

// reg_3a
#define REG_DDRPLL_SDM_FREQWORD_31_16_SHIFT 0
#define REG_DDRPLL_SDM_FREQWORD_31_16_MASK  (0xFFFF << REG_DDRPLL_SDM_FREQWORD_31_16_SHIFT)
#define REG_DDRPLL_SDM_FREQWORD_31_16(n)    BITFIELD_VAL(REG_DDRPLL_SDM_FREQWORD_31_16, n)

// reg_3b
#define REG_DDRPLL_PU_DR                    (1 << 0)
#define REG_DDRPLL_PU                       (1 << 1)
#define REG_DDRPLL_LDO_PU_DR                (1 << 2)
#define REG_DDRPLL_LDO_PU                   (1 << 3)
#define REG_DDRPLL_PRECHARGE_DR             (1 << 4)
#define REG_DDRPLL_PRECHARGE                (1 << 5)
#define REG_DDRPLL_LPF_PRECHARGE_DR         (1 << 6)
#define REG_DDRPLL_LPF_PRECHARGE            (1 << 7)
#define REG_DDRPLL_LOOP_RESETB_DR           (1 << 8)
#define REG_DDRPLL_LOOP_RESETB              (1 << 9)
#define REG_DDRPLL_CAPBANK_CAL_BYPASS       (1 << 10)
#define REG_DDRPLL_CAL_RESETN_DR            (1 << 11)
#define REG_DDRPLL_CAL_RESETN               (1 << 12)
#define REG_DDRPLL_SDM_RESETN_DR            (1 << 13)
#define REG_DDRPLL_SDM_RESETN               (1 << 14)

// reg_3c
#define REG_DDRPLL_CLK_FBC_EDGE             (1 << 0)
#define REG_DDRPLL_INT_DEC_SEL_SHIFT        1
#define REG_DDRPLL_INT_DEC_SEL_MASK         (0x7 << REG_DDRPLL_INT_DEC_SEL_SHIFT)
#define REG_DDRPLL_INT_DEC_SEL(n)           BITFIELD_VAL(REG_DDRPLL_INT_DEC_SEL, n)
#define REG_DDRPLL_PRESCALER_DEL_SHIFT      4
#define REG_DDRPLL_PRESCALER_DEL_MASK       (0xF << REG_DDRPLL_PRESCALER_DEL_SHIFT)
#define REG_DDRPLL_PRESCALER_DEL(n)         BITFIELD_VAL(REG_DDRPLL_PRESCALER_DEL, n)
#define REG_DDRPLL_DITHER_BYPASS            (1 << 8)
#define REG_DDRPLL_SDM_CLK_SEL_DR           (1 << 9)
#define REG_DDRPLL_SDM_CLK_SEL              (1 << 10)

// reg_3d
#define REG_DDRPLL_DIV_DR                   (1 << 0)
#define REG_DDRPLL_DIV_INT_SHIFT            1
#define REG_DDRPLL_DIV_INT_MASK             (0x7F << REG_DDRPLL_DIV_INT_SHIFT)
#define REG_DDRPLL_DIV_INT(n)               BITFIELD_VAL(REG_DDRPLL_DIV_INT, n)
#define REG_DDRPLL_DIV_FRAC_SHIFT           8
#define REG_DDRPLL_DIV_FRAC_MASK            (0x3 << REG_DDRPLL_DIV_FRAC_SHIFT)
#define REG_DDRPLL_DIV_FRAC(n)              BITFIELD_VAL(REG_DDRPLL_DIV_FRAC, n)
#define REG_DDRPLL_CAL_CLK_EN_DR            (1 << 10)
#define REG_DDRPLL_CAL_CLK_EN               (1 << 11)

// reg_3e
#define REG_DDRPLL_WAIT_T1_VALUE_SHIFT      0
#define REG_DDRPLL_WAIT_T1_VALUE_MASK       (0xFF << REG_DDRPLL_WAIT_T1_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T1_VALUE(n)         BITFIELD_VAL(REG_DDRPLL_WAIT_T1_VALUE, n)
#define REG_DDRPLL_WAIT_T2_VALUE_SHIFT      8
#define REG_DDRPLL_WAIT_T2_VALUE_MASK       (0xFF << REG_DDRPLL_WAIT_T2_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T2_VALUE(n)         BITFIELD_VAL(REG_DDRPLL_WAIT_T2_VALUE, n)

// reg_3f
#define REG_DDRPLL_WAIT_T3_VALUE_SHIFT      0
#define REG_DDRPLL_WAIT_T3_VALUE_MASK       (0xFF << REG_DDRPLL_WAIT_T3_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T3_VALUE(n)         BITFIELD_VAL(REG_DDRPLL_WAIT_T3_VALUE, n)
#define REG_DDRPLL_WAIT_T4_VALUE_SHIFT      8
#define REG_DDRPLL_WAIT_T4_VALUE_MASK       (0xFF << REG_DDRPLL_WAIT_T4_VALUE_SHIFT)
#define REG_DDRPLL_WAIT_T4_VALUE(n)         BITFIELD_VAL(REG_DDRPLL_WAIT_T4_VALUE, n)

// reg_50
#define REG_BIAS_SEL_LDO_VOUT_SHIFT         0
#define REG_BIAS_SEL_LDO_VOUT_MASK          (0x3 << REG_BIAS_SEL_LDO_VOUT_SHIFT)
#define REG_BIAS_SEL_LDO_VOUT(n)            BITFIELD_VAL(REG_BIAS_SEL_LDO_VOUT, n)
#define REG_BIAS_SEL_IBIAS_SHIFT            2
#define REG_BIAS_SEL_IBIAS_MASK             (0x7 << REG_BIAS_SEL_IBIAS_SHIFT)
#define REG_BIAS_SEL_IBIAS(n)               BITFIELD_VAL(REG_BIAS_SEL_IBIAS, n)
#define REG_BIAS_SEL_TEMP_SHIFT             5
#define REG_BIAS_SEL_TEMP_MASK              (0x7 << REG_BIAS_SEL_TEMP_SHIFT)
#define REG_BIAS_SEL_TEMP(n)                BITFIELD_VAL(REG_BIAS_SEL_TEMP, n)
#define REG_BIAS_BYPASS                     (1 << 8)
#define REG_BIAS_PRECHG                     (1 << 9)
#define REG_BIAS_PWUP                       (1 << 10)

// reg_51
#define REG_BIAS_EN_IOUT_SHIFT              0
#define REG_BIAS_EN_IOUT_MASK               (0xFFFF << REG_BIAS_EN_IOUT_SHIFT)
#define REG_BIAS_EN_IOUT(n)                 BITFIELD_VAL(REG_BIAS_EN_IOUT, n)

// reg_52
#define RESERVED_2_SHIFT                    0
#define RESERVED_2_MASK                     (0xFFFF << RESERVED_2_SHIFT)
#define RESERVED_2(n)                       BITFIELD_VAL(RESERVED_2, n)

// reg_60
#define DDRPLL_CHIP_ADDR_I2C_SHIFT          0
#define DDRPLL_CHIP_ADDR_I2C_MASK           (0x7F << DDRPLL_CHIP_ADDR_I2C_SHIFT)
#define DDRPLL_CHIP_ADDR_I2C(n)             BITFIELD_VAL(DDRPLL_CHIP_ADDR_I2C, n)

#endif
