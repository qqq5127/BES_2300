#ifndef _REG_DCO_BEST3003_H_
#define _REG_DCO_BEST3003_H_


/************************************************************************* 
                        PAGE-0 START 
************************************************************************ */

// REG_060
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_061
#define DCO_PU_XO_BUFF                      (1 << 0)
#define DCO_PU_LDO                          (1 << 1)
#define DCO_PU_CORE                         (1 << 2)
#define DCO_PU_BUFFER_PRES                  (1 << 3)
#define DCO_PU_DIVN                         (1 << 4)
#define DCO_XO_BUFFER_SEL_MODE              (1 << 5)
#define DCO_XO_PWUP                         (1 << 6)
#define DCO_TST_BUFFER_RCTRL_SHIFT          7
#define DCO_TST_BUFFER_RCTRL_MASK           (0xF << DCO_TST_BUFFER_RCTRL_SHIFT)
#define DCO_TST_BUFFER_RCTRL(n)             BITFIELD_VAL(DCO_TST_BUFFER_RCTRL, n)
#define DCO_XO_BUFFER_PWUP_SHIFT            11
#define DCO_XO_BUFFER_PWUP_MASK             (0xF << DCO_XO_BUFFER_PWUP_SHIFT)
#define DCO_XO_BUFFER_PWUP(n)               BITFIELD_VAL(DCO_XO_BUFFER_PWUP, n)
#define DCO_FINE_TUN_RST                    (1 << 15)

// REG_062
#define DCO_FINE_CLK_SEL                    (1 << 0)
#define DCO_PU_XO_BUFF_DR                   (1 << 1)
#define DCO_PU_LDO_DR                       (1 << 2)
#define DCO_PU_CORE_DR                      (1 << 3)
#define DCO_PU_BUFFER_PRES_DR               (1 << 4)
#define DCO_PU_DIVN_DR                      (1 << 5)
#define DCO_XO_BUFFER_SEL_MODE_DR           (1 << 6)
#define DCO_XO_PWUP_DR                      (1 << 7)
#define DCO_TST_BUFFER_RCTRL_DR             (1 << 8)
#define DCO_XO_BUFFER_PWUP_DR               (1 << 9)
#define DCO_FINE_TUN_RST_DR                 (1 << 10)
#define RESERVED_DCO_4_0_SHIFT              11
#define RESERVED_DCO_4_0_MASK               (0x1F << RESERVED_DCO_4_0_SHIFT)
#define RESERVED_DCO_4_0(n)                 BITFIELD_VAL(RESERVED_DCO_4_0, n)

// REG_063
#define DCO_LOOP_DIV_RATIO_SHIFT            0
#define DCO_LOOP_DIV_RATIO_MASK             (0x3F << DCO_LOOP_DIV_RATIO_SHIFT)
#define DCO_LOOP_DIV_RATIO(n)               BITFIELD_VAL(DCO_LOOP_DIV_RATIO, n)
#define DCO_XO_DIV_RATIO_SHIFT              6
#define DCO_XO_DIV_RATIO_MASK               (0x3F << DCO_XO_DIV_RATIO_SHIFT)
#define DCO_XO_DIV_RATIO(n)                 BITFIELD_VAL(DCO_XO_DIV_RATIO, n)
#define DCO_BUFF_VRES_SHIFT                 12
#define DCO_BUFF_VRES_MASK                  (0x7 << DCO_BUFF_VRES_SHIFT)
#define DCO_BUFF_VRES(n)                    BITFIELD_VAL(DCO_BUFF_VRES, n)
#define REG_PU_OSC                          (1 << 15)

// REG_064
#define DCO_CB_TUN_SHIFT                    0
#define DCO_CB_TUN_MASK                     (0xFF << DCO_CB_TUN_SHIFT)
#define DCO_CB_TUN(n)                       BITFIELD_VAL(DCO_CB_TUN, n)
#define DCO_RBP_TUN_SHIFT                   8
#define DCO_RBP_TUN_MASK                    (0x7 << DCO_RBP_TUN_SHIFT)
#define DCO_RBP_TUN(n)                      BITFIELD_VAL(DCO_RBP_TUN, n)
#define DCO_RBN_TUN_SHIFT                   11
#define DCO_RBN_TUN_MASK                    (0x7 << DCO_RBN_TUN_SHIFT)
#define DCO_RBN_TUN(n)                      BITFIELD_VAL(DCO_RBN_TUN, n)
#define RESERVED_DCO_6_5_SHIFT              14
#define RESERVED_DCO_6_5_MASK               (0x3 << RESERVED_DCO_6_5_SHIFT)
#define RESERVED_DCO_6_5(n)                 BITFIELD_VAL(RESERVED_DCO_6_5, n)

// REG_065
#define DCO_LDO_BUFFER_RCTRL_SHIFT          0
#define DCO_LDO_BUFFER_RCTRL_MASK           (0xF << DCO_LDO_BUFFER_RCTRL_SHIFT)
#define DCO_LDO_BUFFER_RCTRL(n)             BITFIELD_VAL(DCO_LDO_BUFFER_RCTRL, n)
#define DCO_LDO_XO_BUFF_RCTRL_SHIFT         4
#define DCO_LDO_XO_BUFF_RCTRL_MASK          (0xF << DCO_LDO_XO_BUFF_RCTRL_SHIFT)
#define DCO_LDO_XO_BUFF_RCTRL(n)            BITFIELD_VAL(DCO_LDO_XO_BUFF_RCTRL, n)
#define DCO_LDO_DIVN_RCTRL_SHIFT            8
#define DCO_LDO_DIVN_RCTRL_MASK             (0xF << DCO_LDO_DIVN_RCTRL_SHIFT)
#define DCO_LDO_DIVN_RCTRL(n)               BITFIELD_VAL(DCO_LDO_DIVN_RCTRL, n)
#define RESERVED_DCO_10_7_SHIFT             12
#define RESERVED_DCO_10_7_MASK              (0xF << RESERVED_DCO_10_7_SHIFT)
#define RESERVED_DCO_10_7(n)                BITFIELD_VAL(RESERVED_DCO_10_7, n)

// REG_066
#define DCO_XO_BUFFER_RC_SHIFT              0
#define DCO_XO_BUFFER_RC_MASK               (0xF << DCO_XO_BUFFER_RC_SHIFT)
#define DCO_XO_BUFFER_RC(n)                 BITFIELD_VAL(DCO_XO_BUFFER_RC, n)
#define DCO_XO_BUFFER_DRV_SHIFT             4
#define DCO_XO_BUFFER_DRV_MASK              (0xFF << DCO_XO_BUFFER_DRV_SHIFT)
#define DCO_XO_BUFFER_DRV(n)                BITFIELD_VAL(DCO_XO_BUFFER_DRV, n)
#define RESERVED_DCO_14_11_SHIFT            12
#define RESERVED_DCO_14_11_MASK             (0xF << RESERVED_DCO_14_11_SHIFT)
#define RESERVED_DCO_14_11(n)               BITFIELD_VAL(RESERVED_DCO_14_11, n)

// REG_067
#define DCO_LDO_VD1P2REF_SEL_SHIFT          0
#define DCO_LDO_VD1P2REF_SEL_MASK           (0xF << DCO_LDO_VD1P2REF_SEL_SHIFT)
#define DCO_LDO_VD1P2REF_SEL(n)             BITFIELD_VAL(DCO_LDO_VD1P2REF_SEL, n)
#define DCO_LDO_VD1P5REF_SEL_SHIFT          4
#define DCO_LDO_VD1P5REF_SEL_MASK           (0x7 << DCO_LDO_VD1P5REF_SEL_SHIFT)
#define DCO_LDO_VD1P5REF_SEL(n)             BITFIELD_VAL(DCO_LDO_VD1P5REF_SEL, n)
#define DCO_LDO_VD1P2REF_SEL2_SHIFT         7
#define DCO_LDO_VD1P2REF_SEL2_MASK          (0xF << DCO_LDO_VD1P2REF_SEL2_SHIFT)
#define DCO_LDO_VD1P2REF_SEL2(n)            BITFIELD_VAL(DCO_LDO_VD1P2REF_SEL2, n)
#define REG_DCO_PU_DLY_SHIFT                11
#define REG_DCO_PU_DLY_MASK                 (0x1F << REG_DCO_PU_DLY_SHIFT)
#define REG_DCO_PU_DLY(n)                   BITFIELD_VAL(REG_DCO_PU_DLY, n)

// REG_068
#define REG_CLK_120M_NEG_EN                 (1 << 0)
#define REG_XTAL_DCXO_EN_OUT                (1 << 1)
#define DCXO_26M_EN                         (1 << 2)
#define DCXO_GPIO_SEL                       (1 << 3)
#define DCXO_DIGITAL_RESET                  (1 << 4)
#define DIG_XTAL_DCXO_EN                    (1 << 5)
#define RESERVED_DCO_24_15_SHIFT            6
#define RESERVED_DCO_24_15_MASK             (0x3FF << RESERVED_DCO_24_15_SHIFT)
#define RESERVED_DCO_24_15(n)               BITFIELD_VAL(RESERVED_DCO_24_15, n)

// REG_069
#define DCXO_26M_DIV_SHIFT                  0
#define DCXO_26M_DIV_MASK                   (0xFFFF << DCXO_26M_DIV_SHIFT)
#define DCXO_26M_DIV(n)                     BITFIELD_VAL(DCXO_26M_DIV, n)

// REG_06A
#define DCO_LDO_PRES_RCTRL_SHIFT            0
#define DCO_LDO_PRES_RCTRL_MASK             (0xF << DCO_LDO_PRES_RCTRL_SHIFT)
#define DCO_LDO_PRES_RCTRL(n)               BITFIELD_VAL(DCO_LDO_PRES_RCTRL, n)
#define DCO_LDO_CORE_RCTRL_SHIFT            4
#define DCO_LDO_CORE_RCTRL_MASK             (0xF << DCO_LDO_CORE_RCTRL_SHIFT)
#define DCO_LDO_CORE_RCTRL(n)               BITFIELD_VAL(DCO_LDO_CORE_RCTRL, n)
#define DCO_LDO_VD1P5_ISEL_SHIFT            8
#define DCO_LDO_VD1P5_ISEL_MASK             (0x1F << DCO_LDO_VD1P5_ISEL_SHIFT)
#define DCO_LDO_VD1P5_ISEL(n)               BITFIELD_VAL(DCO_LDO_VD1P5_ISEL, n)
#define RESERVED_DCO_27_25_SHIFT            13
#define RESERVED_DCO_27_25_MASK             (0x7 << RESERVED_DCO_27_25_SHIFT)
#define RESERVED_DCO_27_25(n)               BITFIELD_VAL(RESERVED_DCO_27_25, n)

// REG_06B
#define DCO_LDO_CAPC_RCTRL_SHIFT            0
#define DCO_LDO_CAPC_RCTRL_MASK             (0xF << DCO_LDO_CAPC_RCTRL_SHIFT)
#define DCO_LDO_CAPC_RCTRL(n)               BITFIELD_VAL(DCO_LDO_CAPC_RCTRL, n)
#define DCO_LDO_CAPF_RCTRL_SHIFT            4
#define DCO_LDO_CAPF_RCTRL_MASK             (0xF << DCO_LDO_CAPF_RCTRL_SHIFT)
#define DCO_LDO_CAPF_RCTRL(n)               BITFIELD_VAL(DCO_LDO_CAPF_RCTRL, n)
#define REG_DCO_LDO_PRECH                   (1 << 8)
#define REG_DCO_LDO_PRECH_DR                (1 << 8)

// REG_06D
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define RESERVED_DCO_36_28_SHIFT            7
#define RESERVED_DCO_36_28_MASK             (0x1FF << RESERVED_DCO_36_28_SHIFT)
#define RESERVED_DCO_36_28(n)               BITFIELD_VAL(RESERVED_DCO_36_28, n)

/************************************************************************* 
                        PAGE-0 END 
************************************************************************ */


/************************************************************************* 
                        PAGE-1 START 
************************************************************************ */

// REG_160
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_161
#define REG_COARSE_EN_DR                    (1 << 0)
#define REG_COARSE_EN                       (1 << 1)
#define REG_DCO_COARSE_TUN_CODE_DR          (1 << 2)
#define REG_DCO_COARSE_TUN_CODE_SHIFT       3
#define REG_DCO_COARSE_TUN_CODE_MASK        (0x3FF << REG_DCO_COARSE_TUN_CODE_SHIFT)
#define REG_DCO_COARSE_TUN_CODE(n)          BITFIELD_VAL(REG_DCO_COARSE_TUN_CODE, n)
#define REG_COARSE_POLAR_SEL                (1 << 13)
#define RESERVED_DIG_1_0_SHIFT              14
#define RESERVED_DIG_1_0_MASK               (0x3 << RESERVED_DIG_1_0_SHIFT)
#define RESERVED_DIG_1_0(n)                 BITFIELD_VAL(RESERVED_DIG_1_0, n)

// REG_162
#define REG_CLK_REF_COARSE_DIV_NUM_SHIFT    0
#define REG_CLK_REF_COARSE_DIV_NUM_MASK     (0xFF << REG_CLK_REF_COARSE_DIV_NUM_SHIFT)
#define REG_CLK_REF_COARSE_DIV_NUM(n)       BITFIELD_VAL(REG_CLK_REF_COARSE_DIV_NUM, n)
#define RESERVED_DIG_9_2_SHIFT              8
#define RESERVED_DIG_9_2_MASK               (0xFF << RESERVED_DIG_9_2_SHIFT)
#define RESERVED_DIG_9_2(n)                 BITFIELD_VAL(RESERVED_DIG_9_2, n)
/*New REGISTER Since METAL C (metal_id = 1)*/
#define REG_SOF_CLK_SOURCE_SEL              (1 << 9)    
    // set: external clock source
    // not set: internal SOF clock source
#define REG_SOF_CLK_USER_SEL                (1 << 8)    
    // set: SOF clock used by SDM tuning
    // not set: SOF clock used by Fine tuning

// REG_163
#define REG_CNT_COARSE_MARK_15_0_SHIFT      0
#define REG_CNT_COARSE_MARK_15_0_MASK       (0xFFFF << REG_CNT_COARSE_MARK_15_0_SHIFT)
#define REG_CNT_COARSE_MARK_15_0(n)         BITFIELD_VAL(REG_CNT_COARSE_MARK_15_0, n)

// REG_164
#define REG_CNT_COARSE_MARK_24_16_SHIFT     0
#define REG_CNT_COARSE_MARK_24_16_MASK      (0x1FF << REG_CNT_COARSE_MARK_24_16_SHIFT)
#define REG_CNT_COARSE_MARK_24_16(n)        BITFIELD_VAL(REG_CNT_COARSE_MARK_24_16, n)

// REG_165
#define DBG_COARSE_TUN_CODE_SHIFT           0
#define DBG_COARSE_TUN_CODE_MASK            (0x7FF << DBG_COARSE_TUN_CODE_SHIFT)
#define DBG_COARSE_TUN_CODE(n)              BITFIELD_VAL(DBG_COARSE_TUN_CODE, n)
#define RESERVED_DIG_14_10_SHIFT            11
#define RESERVED_DIG_14_10_MASK             (0x1F << RESERVED_DIG_14_10_SHIFT)
#define RESERVED_DIG_14_10(n)               BITFIELD_VAL(RESERVED_DIG_14_10, n)

// REG_166
#define REG_FINE_EN_DR                      (1 << 0)
#define REG_FINE_EN                         (1 << 1)
#define REG_FINE_TUN_CODE_LOAD_VAL_SHIFT    2
#define REG_FINE_TUN_CODE_LOAD_VAL_MASK     (0x7FF << REG_FINE_TUN_CODE_LOAD_VAL_SHIFT)
#define REG_FINE_TUN_CODE_LOAD_VAL(n)       BITFIELD_VAL(REG_FINE_TUN_CODE_LOAD_VAL, n)
#define REG_FINE_TUN_CODE_LOAD_EN           (1 << 13)
#define RESERVED_DIG_16_15_SHIFT            14
#define RESERVED_DIG_16_15_MASK             (0x3 << RESERVED_DIG_16_15_SHIFT)
#define RESERVED_DIG_16_15(n)               BITFIELD_VAL(RESERVED_DIG_16_15, n)

// REG_167
#define REG_CLK_REF_FINE_DIV_NUM_SHIFT      0
#define REG_CLK_REF_FINE_DIV_NUM_MASK       (0x3FF << REG_CLK_REF_FINE_DIV_NUM_SHIFT)
#define REG_CLK_REF_FINE_DIV_NUM(n)         BITFIELD_VAL(REG_CLK_REF_FINE_DIV_NUM, n)
#define REG_FINE_TUN_STEP_SEL_SHIFT         10
#define REG_FINE_TUN_STEP_SEL_MASK          (0x7 << REG_FINE_TUN_STEP_SEL_SHIFT)
#define REG_FINE_TUN_STEP_SEL(n)            BITFIELD_VAL(REG_FINE_TUN_STEP_SEL, n)
#define REG_FINE_POLAR_SEL                  (1 << 13)
#define REG_FINE_TUN_STEP_SHRINK            (1 << 14)
#define RESERVED_DIG_17                     (1 << 15)

// REG_168
#define REG_FINE_CNT_TIME_OUT_OFF_SHIFT     0
#define REG_FINE_CNT_TIME_OUT_OFF_MASK      (0x3FFF << REG_FINE_CNT_TIME_OUT_OFF_SHIFT)
#define REG_FINE_CNT_TIME_OUT_OFF(n)        BITFIELD_VAL(REG_FINE_CNT_TIME_OUT_OFF, n)
#define RESERVED_DIG_19_18_SHIFT            14
#define RESERVED_DIG_19_18_MASK             (0x3 << RESERVED_DIG_19_18_SHIFT)
#define RESERVED_DIG_19_18(n)               BITFIELD_VAL(RESERVED_DIG_19_18, n)

// REG_169
#define REG_CNT_FINE_MARK_15_0_SHIFT        0
#define REG_CNT_FINE_MARK_15_0_MASK         (0xFFFF << REG_CNT_FINE_MARK_15_0_SHIFT)
#define REG_CNT_FINE_MARK_15_0(n)           BITFIELD_VAL(REG_CNT_FINE_MARK_15_0, n)

// REG_16A
#define REG_CNT_FINE_MARK_24_16_SHIFT       0
#define REG_CNT_FINE_MARK_24_16_MASK        (0x1FF << REG_CNT_FINE_MARK_24_16_SHIFT)
#define REG_CNT_FINE_MARK_24_16(n)          BITFIELD_VAL(REG_CNT_FINE_MARK_24_16, n)
#define REG_DCO_FINE_TUN_CODE_DR            (1 << 9)
#define RESERVED_DIG_25_20_SHIFT            10
#define RESERVED_DIG_25_20_MASK             (0x3F << RESERVED_DIG_25_20_SHIFT)
#define RESERVED_DIG_25_20(n)               BITFIELD_VAL(RESERVED_DIG_25_20, n)

// REG_16B
#define REG_DCO_FINE_TUN_CODE_SHIFT         0
#define REG_DCO_FINE_TUN_CODE_MASK          (0x7FF << REG_DCO_FINE_TUN_CODE_SHIFT)
#define REG_DCO_FINE_TUN_CODE(n)            BITFIELD_VAL(REG_DCO_FINE_TUN_CODE, n)
#define RESERVED_DIG_30_26_SHIFT            11
#define RESERVED_DIG_30_26_MASK             (0x1F << RESERVED_DIG_30_26_SHIFT)
#define RESERVED_DIG_30_26(n)               BITFIELD_VAL(RESERVED_DIG_30_26, n)

// REG_16C
#define DCXO_COEFF_UPDATE                   (1 << 0)
#define REG_COEFF_SEL_SHIFT                 1
#define REG_COEFF_SEL_MASK                  (0x7 << REG_COEFF_SEL_SHIFT)
#define REG_COEFF_SEL(n)                    BITFIELD_VAL(REG_COEFF_SEL, n)
#define REG_FILTER_BYPASS                   (1 << 4)
#define REG_PRBS_ABS_SHIFT                  5
#define REG_PRBS_ABS_MASK                   (0x3 << REG_PRBS_ABS_SHIFT)
#define REG_PRBS_ABS(n)                     BITFIELD_VAL(REG_PRBS_ABS, n)
#define REG_SDM_BYPASS                      (1 << 7)
#define RESERVED_DIG_38_31_SHIFT            8
#define RESERVED_DIG_38_31_MASK             (0xFF << RESERVED_DIG_38_31_SHIFT)
#define RESERVED_DIG_38_31(n)               BITFIELD_VAL(RESERVED_DIG_38_31, n)

// REG_16D
#define DBG_FINE_TUN_STEP_SHIFT             0
#define DBG_FINE_TUN_STEP_MASK              (0xFF << DBG_FINE_TUN_STEP_SHIFT)
#define DBG_FINE_TUN_STEP(n)                BITFIELD_VAL(DBG_FINE_TUN_STEP, n)
#define DBG_FINE_TUN_STABLE                 (1 << 8)
#define DBG_STATE_SHIFT                     9
#define DBG_STATE_MASK                      (0x7 << DBG_STATE_SHIFT)
#define DBG_STATE(n)                        BITFIELD_VAL(DBG_STATE, n)
#define RESERVED_DIG_42_39_SHIFT            12
#define RESERVED_DIG_42_39_MASK             (0xF << RESERVED_DIG_42_39_SHIFT)
#define RESERVED_DIG_42_39(n)               BITFIELD_VAL(RESERVED_DIG_42_39, n)

// REG_16E
#define DBG_FINE_TUN_CODE_SHIFT             0
#define DBG_FINE_TUN_CODE_MASK              (0x7FF << DBG_FINE_TUN_CODE_SHIFT)
#define DBG_FINE_TUN_CODE(n)                BITFIELD_VAL(DBG_FINE_TUN_CODE, n)
#define RESERVED_DIG_47_43_SHIFT            11
#define RESERVED_DIG_47_43_MASK             (0x1F << RESERVED_DIG_47_43_SHIFT)
#define RESERVED_DIG_47_43(n)               BITFIELD_VAL(RESERVED_DIG_47_43, n)

// REG_16F
#define DBG_GOAL_CNT_15_0_SHIFT             0
#define DBG_GOAL_CNT_15_0_MASK              (0xFFFF << DBG_GOAL_CNT_15_0_SHIFT)
#define DBG_GOAL_CNT_15_0(n)                BITFIELD_VAL(DBG_GOAL_CNT_15_0, n)

// REG_170
#define DBG_GOAL_CNT_24_16_SHIFT            0
#define DBG_GOAL_CNT_24_16_MASK             (0x1FF << DBG_GOAL_CNT_24_16_SHIFT)
#define DBG_GOAL_CNT_24_16(n)               BITFIELD_VAL(DBG_GOAL_CNT_24_16, n)
#define RESERVED_DIG_54_48_SHIFT            9
#define RESERVED_DIG_54_48_MASK             (0x7F << RESERVED_DIG_54_48_SHIFT)
#define RESERVED_DIG_54_48(n)               BITFIELD_VAL(RESERVED_DIG_54_48, n)

// REG_171
#define DBG_REF_CNT_SHIFT                   0
#define DBG_REF_CNT_MASK                    (0x3FF << DBG_REF_CNT_SHIFT)
#define DBG_REF_CNT(n)                      BITFIELD_VAL(DBG_REF_CNT, n)
#define RESERVED_DIG_60_55_SHIFT            10
#define RESERVED_DIG_60_55_MASK             (0x3F << RESERVED_DIG_60_55_SHIFT)
#define RESERVED_DIG_60_55(n)               BITFIELD_VAL(RESERVED_DIG_60_55, n)

/************************************************************************* 
                        PAGE-1 END 
************************************************************************ */


/************************************************************************* 
                        PAGE-2 START 
************************************************************************ */

// REG_260
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_261
#define REG_SDM_OUT_EN_DR                   (1 << 0)
#define REG_SDM_OUT_EN                      (1 << 1)
#define REG_SDM_RSTB                        (1 << 2)
#define REG_SDM_OUT_TUN_CODE_LOAD_EN        (1 << 3)
#define REG_SDM_OUT_TUN_CODE_LOAD_VAL_11_0_SHIFT 4
#define REG_SDM_OUT_TUN_CODE_LOAD_VAL_11_0_MASK (0xFFF << REG_SDM_OUT_TUN_CODE_LOAD_VAL_11_0_SHIFT)
#define REG_SDM_OUT_TUN_CODE_LOAD_VAL_11_0(n) BITFIELD_VAL(REG_SDM_OUT_TUN_CODE_LOAD_VAL_11_0, n)

// REG_262
#define REG_SDM_OUT_TUN_CODE_LOAD_VAL_19_12_SHIFT 0
#define REG_SDM_OUT_TUN_CODE_LOAD_VAL_19_12_MASK (0xFF << REG_SDM_OUT_TUN_CODE_LOAD_VAL_19_12_SHIFT)
#define REG_SDM_OUT_TUN_CODE_LOAD_VAL_19_12(n) BITFIELD_VAL(REG_SDM_OUT_TUN_CODE_LOAD_VAL_19_12, n)
#define RESERVED_DIG_68_61_SHIFT            8
#define RESERVED_DIG_68_61_MASK             (0xFF << RESERVED_DIG_68_61_SHIFT)
#define RESERVED_DIG_68_61(n)               BITFIELD_VAL(RESERVED_DIG_68_61, n)

// REG_263
#define REG_CLK_REF_SDM_OUT_DIV_NUM_SHIFT   0
#define REG_CLK_REF_SDM_OUT_DIV_NUM_MASK    (0xFF << REG_CLK_REF_SDM_OUT_DIV_NUM_SHIFT)
#define REG_CLK_REF_SDM_OUT_DIV_NUM(n)      BITFIELD_VAL(REG_CLK_REF_SDM_OUT_DIV_NUM, n)
#define REG_SDM_OUT_TUN_STEP_SEL_SHIFT      8
#define REG_SDM_OUT_TUN_STEP_SEL_MASK       (0xF << REG_SDM_OUT_TUN_STEP_SEL_SHIFT)
#define REG_SDM_OUT_TUN_STEP_SEL(n)         BITFIELD_VAL(REG_SDM_OUT_TUN_STEP_SEL, n)
#define REG_SDM_OUT_POLAR_SEL               (1 << 12)
#define RESERVED_DIG_71_69_SHIFT            13
#define RESERVED_DIG_71_69_MASK             (0x7 << RESERVED_DIG_71_69_SHIFT)
#define RESERVED_DIG_71_69(n)               BITFIELD_VAL(RESERVED_DIG_71_69, n)

// REG_264
#define REG_SDM_OUT_TUN_STEP_SHRINK         (1 << 0)
#define REG_SDM_OUT_CNT_TIME_OUT_OFF_SHIFT  1
#define REG_SDM_OUT_CNT_TIME_OUT_OFF_MASK   (0x3FFF << REG_SDM_OUT_CNT_TIME_OUT_OFF_SHIFT)
#define REG_SDM_OUT_CNT_TIME_OUT_OFF(n)     BITFIELD_VAL(REG_SDM_OUT_CNT_TIME_OUT_OFF, n)
#define RESERVED_DIG_72                     (1 << 15)

// REG_265
#define REG_CNT_SDM_OUT_MARK_15_0_SHIFT     0
#define REG_CNT_SDM_OUT_MARK_15_0_MASK      (0xFFFF << REG_CNT_SDM_OUT_MARK_15_0_SHIFT)
#define REG_CNT_SDM_OUT_MARK_15_0(n)        BITFIELD_VAL(REG_CNT_SDM_OUT_MARK_15_0, n)

// REG_266
#define REG_CNT_SDM_OUT_MARK_24_16_SHIFT    0
#define REG_CNT_SDM_OUT_MARK_24_16_MASK     (0x1FF << REG_CNT_SDM_OUT_MARK_24_16_SHIFT)
#define REG_CNT_SDM_OUT_MARK_24_16(n)       BITFIELD_VAL(REG_CNT_SDM_OUT_MARK_24_16, n)
#define RESERVED_DIG_79_73_SHIFT            9
#define RESERVED_DIG_79_73_MASK             (0x7F << RESERVED_DIG_79_73_SHIFT)
#define RESERVED_DIG_79_73(n)               BITFIELD_VAL(RESERVED_DIG_79_73, n)

// REG_267
#define REG_SDM_OUT_TUN_CODE_DR             (1 << 0)
#define REG_SDM_OUT_TUN_CODE_11_0_SHIFT     1
#define REG_SDM_OUT_TUN_CODE_11_0_MASK      (0xFFF << REG_SDM_OUT_TUN_CODE_11_0_SHIFT)
#define REG_SDM_OUT_TUN_CODE_11_0(n)        BITFIELD_VAL(REG_SDM_OUT_TUN_CODE_11_0, n)
#define RESERVED_DIG_82_80_SHIFT            13
#define RESERVED_DIG_82_80_MASK             (0x7 << RESERVED_DIG_82_80_SHIFT)
#define RESERVED_DIG_82_80(n)               BITFIELD_VAL(RESERVED_DIG_82_80, n)

// REG_268
#define REG_SDM_OUT_TUN_CODE_19_12_SHIFT    0
#define REG_SDM_OUT_TUN_CODE_19_12_MASK     (0xFF << REG_SDM_OUT_TUN_CODE_19_12_SHIFT)
#define REG_SDM_OUT_TUN_CODE_19_12(n)       BITFIELD_VAL(REG_SDM_OUT_TUN_CODE_19_12, n)
#define RESERVED_DIG_90_83_SHIFT            8
#define RESERVED_DIG_90_83_MASK             (0xFF << RESERVED_DIG_90_83_SHIFT)
#define RESERVED_DIG_90_83(n)               BITFIELD_VAL(RESERVED_DIG_90_83, n)

// REG_269
#define REG_COEFF_UPDATE4SDM_OUT_CODE       (1 << 0)
#define REG_COEFF_SEL4SDM_OUT_CODE_SHIFT    1
#define REG_COEFF_SEL4SDM_OUT_CODE_MASK     (0x7 << REG_COEFF_SEL4SDM_OUT_CODE_SHIFT)
#define REG_COEFF_SEL4SDM_OUT_CODE(n)       BITFIELD_VAL(REG_COEFF_SEL4SDM_OUT_CODE, n)
#define REG_PRBS_ABS4SDM_OUT_CODE_SHIFT     4
#define REG_PRBS_ABS4SDM_OUT_CODE_MASK      (0x3 << REG_PRBS_ABS4SDM_OUT_CODE_SHIFT)
#define REG_PRBS_ABS4SDM_OUT_CODE(n)        BITFIELD_VAL(REG_PRBS_ABS4SDM_OUT_CODE, n)
#define REG_FILTER_BYPASS4SDM_OUT_CODE      (1 << 6)
#define REG_SDM_BYPASS4SDM_OUT_CODE         (1 << 7)
#define CFG_BYPASS_SDM_OUT_DLY              (1 << 8)
#define REG_WAIT_SDM_DONE_DLY_SHIFT         9
#define REG_WAIT_SDM_DONE_DLY_MASK          (0xF << REG_WAIT_SDM_DONE_DLY_SHIFT)
#define REG_WAIT_SDM_DONE_DLY(n)            BITFIELD_VAL(REG_WAIT_SDM_DONE_DLY, n)
#define RESERVED_DIG_93_91_SHIFT            13
#define RESERVED_DIG_93_91_MASK             (0x7 << RESERVED_DIG_93_91_SHIFT)
#define RESERVED_DIG_93_91(n)               BITFIELD_VAL(RESERVED_DIG_93_91, n)

// REG_26A
#define DBG_SDM_OUT_TUN_STEP_SHIFT          0
#define DBG_SDM_OUT_TUN_STEP_MASK           (0xFFFF << DBG_SDM_OUT_TUN_STEP_SHIFT)
#define DBG_SDM_OUT_TUN_STEP(n)             BITFIELD_VAL(DBG_SDM_OUT_TUN_STEP, n)

// REG_26B
#define DBG_SDM_OUT_TUN_CODE_15_0_SHIFT     0
#define DBG_SDM_OUT_TUN_CODE_15_0_MASK      (0xFFFF << DBG_SDM_OUT_TUN_CODE_15_0_SHIFT)
#define DBG_SDM_OUT_TUN_CODE_15_0(n)        BITFIELD_VAL(DBG_SDM_OUT_TUN_CODE_15_0, n)

// REG_26C
#define DBG_SDM_OUT_TUN_CODE_19_16_SHIFT    0
#define DBG_SDM_OUT_TUN_CODE_19_16_MASK     (0xF << DBG_SDM_OUT_TUN_CODE_19_16_SHIFT)
#define DBG_SDM_OUT_TUN_CODE_19_16(n)       BITFIELD_VAL(DBG_SDM_OUT_TUN_CODE_19_16, n)
#define DBG_SDM_OUT_TUN_STABLE              (1 << 4)
#define RESERVED_DIG_104_94_SHIFT           5
#define RESERVED_DIG_104_94_MASK            (0x7FF << RESERVED_DIG_104_94_SHIFT)
#define RESERVED_DIG_104_94(n)              BITFIELD_VAL(RESERVED_DIG_104_94, n)

// REG_26D
#define DBG_GOAL_CNT1_15_0_SHIFT            0
#define DBG_GOAL_CNT1_15_0_MASK             (0xFFFF << DBG_GOAL_CNT1_15_0_SHIFT)
#define DBG_GOAL_CNT1_15_0(n)               BITFIELD_VAL(DBG_GOAL_CNT1_15_0, n)

// REG_26E
#define DBG_GOAL_CNT1_24_16_SHIFT           0
#define DBG_GOAL_CNT1_24_16_MASK            (0x1FF << DBG_GOAL_CNT1_24_16_SHIFT)
#define DBG_GOAL_CNT1_24_16(n)              BITFIELD_VAL(DBG_GOAL_CNT1_24_16, n)
#define RESERVED_DIG_111_105_SHIFT          9
#define RESERVED_DIG_111_105_MASK           (0x7F << RESERVED_DIG_111_105_SHIFT)
#define RESERVED_DIG_111_105(n)             BITFIELD_VAL(RESERVED_DIG_111_105, n)

// REG_26F
#define DBG_REF_CNT1_SHIFT                  0
#define DBG_REF_CNT1_MASK                   (0x3FF << DBG_REF_CNT1_SHIFT)
#define DBG_REF_CNT1(n)                     BITFIELD_VAL(DBG_REF_CNT1, n)
#define RESERVED_DIG_117_112_SHIFT          10
#define RESERVED_DIG_117_112_MASK           (0x3F << RESERVED_DIG_117_112_SHIFT)
#define RESERVED_DIG_117_112(n)             BITFIELD_VAL(RESERVED_DIG_117_112, n)

// REG_270
#define DBG_STATE1_SHIFT                    0
#define DBG_STATE1_MASK                     (0x7 << DBG_STATE1_SHIFT)
#define DBG_STATE1(n)                       BITFIELD_VAL(DBG_STATE1, n)
#define RESERVED_DIG_130_118_SHIFT          3
#define RESERVED_DIG_130_118_MASK           (0x1FFF << RESERVED_DIG_130_118_SHIFT)
#define RESERVED_DIG_130_118(n)             BITFIELD_VAL(RESERVED_DIG_130_118, n)

// REG_271
#define REG_LOAD_DWC_IN                     (1 << 0)
#define REG_LOAD_DWC_OUT                    (1 << 1)
#define REG_LOAD_DWC_OUT_HI_VAL_SHIFT       2
#define REG_LOAD_DWC_OUT_HI_VAL_MASK        (0xFF << REG_LOAD_DWC_OUT_HI_VAL_SHIFT)
#define REG_LOAD_DWC_OUT_HI_VAL(n)          BITFIELD_VAL(REG_LOAD_DWC_OUT_HI_VAL, n)
#define REG_LOAD_DWC_IN_VAL_3_0_SHIFT       10
#define REG_LOAD_DWC_IN_VAL_3_0_MASK        (0xF << REG_LOAD_DWC_IN_VAL_3_0_SHIFT)
#define REG_LOAD_DWC_IN_VAL_3_0(n)          BITFIELD_VAL(REG_LOAD_DWC_IN_VAL_3_0, n)

// REG_272
#define REG_LOAD_DWC_IN_VAL_19_4_SHIFT      0
#define REG_LOAD_DWC_IN_VAL_19_4_MASK       (0xFFFF << REG_LOAD_DWC_IN_VAL_19_4_SHIFT)
#define REG_LOAD_DWC_IN_VAL_19_4(n)         BITFIELD_VAL(REG_LOAD_DWC_IN_VAL_19_4, n)

// REG_273
#define REG_LOAD_DWC_OUT_LO_VAL_SHIFT       0
#define REG_LOAD_DWC_OUT_LO_VAL_MASK        (0xFFFF << REG_LOAD_DWC_OUT_LO_VAL_SHIFT)
#define REG_LOAD_DWC_OUT_LO_VAL(n)          BITFIELD_VAL(REG_LOAD_DWC_OUT_LO_VAL, n)

// REG_274
#define FILTER_DATA_OUT4SDM_OUT_CODE_SHIFT  0
#define FILTER_DATA_OUT4SDM_OUT_CODE_MASK   (0x3FFF << FILTER_DATA_OUT4SDM_OUT_CODE_SHIFT)
#define FILTER_DATA_OUT4SDM_OUT_CODE(n)     BITFIELD_VAL(FILTER_DATA_OUT4SDM_OUT_CODE, n)

/************************************************************************* 
                        PAGE-2 END 
************************************************************************ */

/************************************************************************* 
                        REGISTER LIST 
************************************************************************ */

enum {
    // PAGE-0 START
    DCO_REG_060 = 0x060,
    DCO_REG_061,
    DCO_REG_062,
    DCO_REG_063,
    DCO_REG_064,
    DCO_REG_065,
    DCO_REG_066,
    DCO_REG_067,
    DCO_REG_068,
    DCO_REG_069,
    DCO_REG_06A,
    DCO_REG_06B,
    DCO_REG_06C,
    DCO_REG_06D,
    // PAGE-0 END

    // PAGE-1 START
    DCO_REG_160 = 0x160,
    DCO_REG_161,
    DCO_REG_162,
    DCO_REG_163,
    DCO_REG_164,
    DCO_REG_165,
    DCO_REG_166,
    DCO_REG_167,
    DCO_REG_168,
    DCO_REG_169,
    DCO_REG_16A,
    DCO_REG_16B,
    DCO_REG_16C,
    DCO_REG_16D,
    DCO_REG_16E,
    DCO_REG_16F,
    DCO_REG_170,
    DCO_REG_171,
    // PAGE-1 END

    // PAGE-2 START
    DCO_REG_260 = 0x260,
    DCO_REG_261,
    DCO_REG_262,
    DCO_REG_263,
    DCO_REG_264,
    DCO_REG_265,
    DCO_REG_266,
    DCO_REG_267,
    DCO_REG_268,
    DCO_REG_269,
    DCO_REG_26A,
    DCO_REG_26B,
    DCO_REG_26C,
    DCO_REG_26D,
    DCO_REG_26E,
    DCO_REG_26F,
    DCO_REG_270,
    DCO_REG_271,
    DCO_REG_272,
    DCO_REG_273,
    DCO_REG_274,
    // PAGE-2 END
}; 

#endif // _REG_DCO_BEST3003_H
