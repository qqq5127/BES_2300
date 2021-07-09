#ifndef _REG_ANALOG_BEST3003_H_
#define _REG_ANALOG_BEST3003_H_

/* ---- PAGE0 START ---- */

// REG_060
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_061
#define REG_CODEC_VCM_NEG_EN                (1 << 0)
#define REG_CODEC_VCM_NEG_EN_LPF            (1 << 1)
#define REG_CODEC_VCM_NEG_LP                (1 << 2)
#define REG_CODEC_VCM_NEG_LOW_VCM_SHIFT     3
#define REG_CODEC_VCM_NEG_LOW_VCM_MASK      (0xF << REG_CODEC_VCM_NEG_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_NEG_LOW_VCM(n)        BITFIELD_VAL(REG_CODEC_VCM_NEG_LOW_VCM, n)
#define REG_CODEC_VCM_NEG_LOW_VCM_LP_SHIFT  7
#define REG_CODEC_VCM_NEG_LOW_VCM_LP_MASK   (0xF << REG_CODEC_VCM_NEG_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_NEG_LOW_VCM_LP(n)     BITFIELD_VAL(REG_CODEC_VCM_NEG_LOW_VCM_LP, n)
#define REG_CODEC_VCM_NEG_LOW_VCM_LPF_SHIFT 11
#define REG_CODEC_VCM_NEG_LOW_VCM_LPF_MASK  (0xF << REG_CODEC_VCM_NEG_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_NEG_LOW_VCM_LPF(n)    BITFIELD_VAL(REG_CODEC_VCM_NEG_LOW_VCM_LPF, n)
#define REG_CODEC_EN_VCM                    (1 << 15)

// REG_062
#define REG_CODEC_TX_EAR_DOUBLEBIAS         (1 << 0)
#define REG_CODEC_TX_EAR_IBSEL_SHIFT        1
#define REG_CODEC_TX_EAR_IBSEL_MASK         (0x3 << REG_CODEC_TX_EAR_IBSEL_SHIFT)
#define REG_CODEC_TX_EAR_IBSEL(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_IBSEL, n)
#define REG_CODEC_VCM_EN_LPF                (1 << 3)
#define REG_CODEC_LP_VCM                    (1 << 4)
#define REG_CODEC_EN_BIAS                   (1 << 5)
#define REG_CODEC_EN_VCMBUFFER              (1 << 6)
#define REG_CODEC_EN_TX_EXT                 (1 << 7)
#define REG_CODEC_DAC_CLK_EDGE_SEL          (1 << 8)
#define REG_CODEC_TX_DAC_SWR_SHIFT          9
#define REG_CODEC_TX_DAC_SWR_MASK           (0x3 << REG_CODEC_TX_DAC_SWR_SHIFT)
#define REG_CODEC_TX_DAC_SWR(n)             BITFIELD_VAL(REG_CODEC_TX_DAC_SWR, n)
#define REG_CODEC_TX_IB_SEL_HALF            (1 << 11)
#define REG_CODEC_TX_EAR_OFFTEN             (1 << 12)

// REG_063
#define REG_CODEC_VCM_LOW_VCM_SHIFT         0
#define REG_CODEC_VCM_LOW_VCM_MASK          (0xF << REG_CODEC_VCM_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_LOW_VCM(n)            BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM, n)
#define REG_CODEC_VCM_LOW_VCM_LPF_SHIFT     4
#define REG_CODEC_VCM_LOW_VCM_LPF_MASK      (0xF << REG_CODEC_VCM_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LPF(n)        BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LPF, n)
#define REG_CODEC_VCM_LOW_VCM_LP_SHIFT      8
#define REG_CODEC_VCM_LOW_VCM_LP_MASK       (0xF << REG_CODEC_VCM_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LP(n)         BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LP, n)
#define REG_CODEC_VCM_BIAS_HIGHV            (1 << 12)
#define REG_CRYSTAL_SEL_LV                  (1 << 13)
#define CFG_TX_CLK_INV                      (1 << 14)
#define REG_PU_OSC                          (1 << 15)

// REG_064
#define CFG_RESET_ADC_A                     (1 << 0)
#define CFG_RESET_ADC_A_DR                  (1 << 1)
#define CFG_RESET_ADC_B                     (1 << 2)
#define CFG_RESET_ADC_B_DR                  (1 << 3)
#define REG_CODEC_RX_EN_ADCA                (1 << 4)
#define REG_CODEC_RX_EN_ADCB                (1 << 5)
#define REG_CODEC_ADC_VREF_SEL_SHIFT        6
#define REG_CODEC_ADC_VREF_SEL_MASK         (0xF << REG_CODEC_ADC_VREF_SEL_SHIFT)
#define REG_CODEC_ADC_VREF_SEL(n)           BITFIELD_VAL(REG_CODEC_ADC_VREF_SEL, n)
#define RX_TIMER_RSTN_DLY_SHIFT             10
#define RX_TIMER_RSTN_DLY_MASK              (0x3F << RX_TIMER_RSTN_DLY_SHIFT)
#define RX_TIMER_RSTN_DLY(n)                BITFIELD_VAL(RX_TIMER_RSTN_DLY, n)

// REG_065
#define REG_CODEC_TX_EAR_DR_EN              (1 << 0)
#define REG_CODEC_TX_EAR_DR_ST_SHIFT        1
#define REG_CODEC_TX_EAR_DR_ST_MASK         (0x7 << REG_CODEC_TX_EAR_DR_ST_SHIFT)
#define REG_CODEC_TX_EAR_DR_ST(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_DR_ST, n)
#define REG_CODEC_TX_EN_EARPA_L             (1 << 4)
#define REG_CODEC_TX_EN_EARPA_R             (1 << 5)
#define REG_CODEC_TX_EN_LCLK                (1 << 6)
#define REG_CODEC_TX_EN_RCLK                (1 << 7)
#define REG_CODEC_TX_PA_LOWGAINL_SHIFT      8
#define REG_CODEC_TX_PA_LOWGAINL_MASK       (0xF << REG_CODEC_TX_PA_LOWGAINL_SHIFT)
#define REG_CODEC_TX_PA_LOWGAINL(n)         BITFIELD_VAL(REG_CODEC_TX_PA_LOWGAINL, n)
#define REG_CODEC_TX_PA_LOWGAINR_SHIFT      12
#define REG_CODEC_TX_PA_LOWGAINR_MASK       (0xF << REG_CODEC_TX_PA_LOWGAINR_SHIFT)
#define REG_CODEC_TX_PA_LOWGAINR(n)         BITFIELD_VAL(REG_CODEC_TX_PA_LOWGAINR, n)

// REG_066
#define REG_IDETLEAR_EN                     (1 << 0)
#define REG_IDETREAR_EN                     (1 << 1)
#define REG_TX_REGULATOR_BIT_SHIFT          2
#define REG_TX_REGULATOR_BIT_MASK           (0xF << REG_TX_REGULATOR_BIT_SHIFT)
#define REG_TX_REGULATOR_BIT(n)             BITFIELD_VAL(REG_TX_REGULATOR_BIT, n)
#define REG_PU_TX_REGULATOR                 (1 << 6)
#define REG_CODEC_TX_EN_DACLDO              (1 << 7)
#define REG_CODEC_TX_EN_S1PA                (1 << 8)
#define REG_CODEC_TX_EN_S2PA                (1 << 9)
#define REG_CODEC_TX_EN_S3PA                (1 << 10)
#define REG_CODEC_TEST_SEL_SHIFT            11
#define REG_CODEC_TEST_SEL_MASK             (0x7 << REG_CODEC_TEST_SEL_SHIFT)
#define REG_CODEC_TEST_SEL(n)               BITFIELD_VAL(REG_CODEC_TEST_SEL, n)
#define REG_CODEC_TX_EN_SWBUFFER            (1 << 14)
#define REG_CODEC_EN_BIAS_LP                (1 << 15)

// REG_067
#define REG_CODEC_TX_ISEL_SW_BUF_SHIFT      0
#define REG_CODEC_TX_ISEL_SW_BUF_MASK       (0xF << REG_CODEC_TX_ISEL_SW_BUF_SHIFT)
#define REG_CODEC_TX_ISEL_SW_BUF(n)         BITFIELD_VAL(REG_CODEC_TX_ISEL_SW_BUF, n)
#define REG_CODEC_TX_SEL_IN_SW              (1 << 4)
#define REG_CODEC_TX_VREFBUF_RZ             (1 << 5)
#define REG_CODEC_TX_VREFBUFN_I_BIT_SHIFT   6
#define REG_CODEC_TX_VREFBUFN_I_BIT_MASK    (0x3 << REG_CODEC_TX_VREFBUFN_I_BIT_SHIFT)
#define REG_CODEC_TX_VREFBUFN_I_BIT(n)      BITFIELD_VAL(REG_CODEC_TX_VREFBUFN_I_BIT, n)
#define REG_CODEC_TX_VREFBUFP_I_BIT_SHIFT   8
#define REG_CODEC_TX_VREFBUFP_I_BIT_MASK    (0x3 << REG_CODEC_TX_VREFBUFP_I_BIT_SHIFT)
#define REG_CODEC_TX_VREFBUFP_I_BIT(n)      BITFIELD_VAL(REG_CODEC_TX_VREFBUFP_I_BIT, n)
#define REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT    10
#define REG_CODEC_TX_EAR_OUTPUTSEL_MASK     (0xF << REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT)
#define REG_CODEC_TX_EAR_OUTPUTSEL(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_OUTPUTSEL, n)
#define REG_CODEC_TX_EN_S0PA                (1 << 14)

// REG_068
#define REG_CODEC_TX_EAR_OFF_BITR_SHIFT     0
#define REG_CODEC_TX_EAR_OFF_BITR_MASK      (0x3FF << REG_CODEC_TX_EAR_OFF_BITR_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITR, n)
#define REG_CODEC_TX_EAR_ENBIAS             (1 << 10)
#define REG_CODEC_TX_EAR_OCEN               (1 << 11)
#define REG_BYPASS_TX_REGULATOR             (1 << 12)
#define CFG_TX_CH0_MUTE                     (1 << 13)
#define CFG_TX_CH1_MUTE                     (1 << 14)

// REG_069
#define REG_CODEC_TX_EAR_OFF_BITL_SHIFT     0
#define REG_CODEC_TX_EAR_OFF_BITL_MASK      (0x3FF << REG_CODEC_TX_EAR_OFF_BITL_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL, n)
#define REG_CODEC_TX_EAR_LCAL               (1 << 10)
#define REG_CODEC_TX_EAR_RCAL               (1 << 11)
#define REG_CODEC_TX_VREFBUFN_OUT_I_SHIFT   12
#define REG_CODEC_TX_VREFBUFN_OUT_I_MASK    (0x3 << REG_CODEC_TX_VREFBUFN_OUT_I_SHIFT)
#define REG_CODEC_TX_VREFBUFN_OUT_I(n)      BITFIELD_VAL(REG_CODEC_TX_VREFBUFN_OUT_I, n)
#define REG_CODEC_TX_VREFBUFP_OUT_I_SHIFT   14
#define REG_CODEC_TX_VREFBUFP_OUT_I_MASK    (0x3 << REG_CODEC_TX_VREFBUFP_OUT_I_SHIFT)
#define REG_CODEC_TX_VREFBUFP_OUT_I(n)      BITFIELD_VAL(REG_CODEC_TX_VREFBUFP_OUT_I, n)

// REG_06A
#define REG_CODEC_TX_COMP_PA_12_SHIFT       0
#define REG_CODEC_TX_COMP_PA_12_MASK        (0xFF << REG_CODEC_TX_COMP_PA_12_SHIFT)
#define REG_CODEC_TX_COMP_PA_12(n)          BITFIELD_VAL(REG_CODEC_TX_COMP_PA_12, n)
#define REG_CODEC_BUF_LOWPOWER              (1 << 8)
#define REG_CODEC_BUF_LOWPOWER2             (1 << 9)
#define REG_CODEC_BUF_LOWVCM_SHIFT          10
#define REG_CODEC_BUF_LOWVCM_MASK           (0x7 << REG_CODEC_BUF_LOWVCM_SHIFT)
#define REG_CODEC_BUF_LOWVCM(n)             BITFIELD_VAL(REG_CODEC_BUF_LOWVCM, n)

// REG_06B
#define REG_TRI_MODE                        (1 << 0)
#define REG_DAC_ENBLE                       (1 << 1)
#define REG_SY_DACEN_MASK                   (1 << 2)
#define REG_SY_CLEAR                        (1 << 3)
#define REG_SE_DACEN_MASK                   (1 << 4)
#define REG_SE_CLEAR                        (1 << 5)
#define REG_SV_DACEN_MASK                   (1 << 6)
#define REG_SV_CLEAR                        (1 << 7)
#define REG_ALL_CLEAR                       (1 << 8)

// REG_06C
#define REG_CODEC_BIAS_LOWV                 (1 << 0)
#define REG_CODEC_BIAS_IBSEL_SHIFT          1
#define REG_CODEC_BIAS_IBSEL_MASK           (0xF << REG_CODEC_BIAS_IBSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL(n)             BITFIELD_VAL(REG_CODEC_BIAS_IBSEL, n)
#define REG_CODEC_BIAS_IBSEL_TX_SHIFT       5
#define REG_CODEC_BIAS_IBSEL_TX_MASK        (0xF << REG_CODEC_BIAS_IBSEL_TX_SHIFT)
#define REG_CODEC_BIAS_IBSEL_TX(n)          BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_TX, n)
#define REG_CODEC_BIAS_LOWV_LP              (1 << 9)
#define REG_CODEC_BIAS_IBSEL_VOICE_SHIFT    10
#define REG_CODEC_BIAS_IBSEL_VOICE_MASK     (0xF << REG_CODEC_BIAS_IBSEL_VOICE_SHIFT)
#define REG_CODEC_BIAS_IBSEL_VOICE(n)       BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_VOICE, n)

// REG_06D
#define CHIP_ADDR_I2C_SHIFT                 0
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)
#define CFG_DIV_100K_SHIFT                  7
#define CFG_DIV_100K_MASK                   (0xFF << CFG_DIV_100K_SHIFT)
#define CFG_DIV_100K(n)                     BITFIELD_VAL(CFG_DIV_100K, n)

// REG_06E
#define TX_TIMER0_SHIFT                     0
#define TX_TIMER0_MASK                      (0xFFFF << TX_TIMER0_SHIFT)
#define TX_TIMER0(n)                        BITFIELD_VAL(TX_TIMER0, n)

// REG_06F
#define TX_TIMER1_SHIFT                     0
#define TX_TIMER1_MASK                      (0xFFFF << TX_TIMER1_SHIFT)
#define TX_TIMER1(n)                        BITFIELD_VAL(TX_TIMER1, n)

// REG_070
#define REG_DBL_DL1_SHIFT                   0
#define REG_DBL_DL1_MASK                    (0xF << REG_DBL_DL1_SHIFT)
#define REG_DBL_DL1(n)                      BITFIELD_VAL(REG_DBL_DL1, n)
#define REG_DBL_DL2_SHIFT                   4
#define REG_DBL_DL2_MASK                    (0xF << REG_DBL_DL2_SHIFT)
#define REG_DBL_DL2(n)                      BITFIELD_VAL(REG_DBL_DL2, n)
#define DIG_DBL_EN                          (1 << 8)
#define DIG_DBL_RST                         (1 << 9)

// REG_071
#define REG_DBL_DCC1_CAP_STB_SHIFT          0
#define REG_DBL_DCC1_CAP_STB_MASK           (0x3 << REG_DBL_DCC1_CAP_STB_SHIFT)
#define REG_DBL_DCC1_CAP_STB(n)             BITFIELD_VAL(REG_DBL_DCC1_CAP_STB, n)
#define REG_DBL_DCC2_CAP_STB_SHIFT          2
#define REG_DBL_DCC2_CAP_STB_MASK           (0x3 << REG_DBL_DCC2_CAP_STB_SHIFT)
#define REG_DBL_DCC2_CAP_STB(n)             BITFIELD_VAL(REG_DBL_DCC2_CAP_STB, n)
#define REG_DBL_DCC3_CAP_STB_SHIFT          4
#define REG_DBL_DCC3_CAP_STB_MASK           (0x3 << REG_DBL_DCC3_CAP_STB_SHIFT)
#define REG_DBL_DCC3_CAP_STB(n)             BITFIELD_VAL(REG_DBL_DCC3_CAP_STB, n)
#define REG_DBL_DCC1_RES_SHIFT              6
#define REG_DBL_DCC1_RES_MASK               (0x3 << REG_DBL_DCC1_RES_SHIFT)
#define REG_DBL_DCC1_RES(n)                 BITFIELD_VAL(REG_DBL_DCC1_RES, n)
#define REG_DBL_DCC2_RES_SHIFT              8
#define REG_DBL_DCC2_RES_MASK               (0x3 << REG_DBL_DCC2_RES_SHIFT)
#define REG_DBL_DCC2_RES(n)                 BITFIELD_VAL(REG_DBL_DCC2_RES, n)
#define REG_DBL_DCC3_RES_SHIFT              10
#define REG_DBL_DCC3_RES_MASK               (0x3 << REG_DBL_DCC3_RES_SHIFT)
#define REG_DBL_DCC3_RES(n)                 BITFIELD_VAL(REG_DBL_DCC3_RES, n)
#define REG_DBL_IC_BOOST_SHIFT              12
#define REG_DBL_IC_BOOST_MASK               (0x3 << REG_DBL_IC_BOOST_SHIFT)
#define REG_DBL_IC_BOOST(n)                 BITFIELD_VAL(REG_DBL_IC_BOOST, n)
#define REG_DBL_FREQ_SEL                    (1 << 14)

// REG_072
#define REG_XO_TOP_PU_XTAL_DLY_SHIFT        0
#define REG_XO_TOP_PU_XTAL_DLY_MASK         (0x1F << REG_XO_TOP_PU_XTAL_DLY_SHIFT)
#define REG_XO_TOP_PU_XTAL_DLY(n)           BITFIELD_VAL(REG_XO_TOP_PU_XTAL_DLY, n)
#define REG_XO_TOP_XTAL_PU_DR               (1 << 5)
#define REG_XO_TOP_XTAL_PU                  (1 << 6)
#define REG_XO_TOP_PU_XTAL_DSLEEP           (1 << 7)
#define REG_XO_TOP_XTAL_PU_LPM_DR           (1 << 8)
#define REG_XO_TOP_XTAL_PU_LPM              (1 << 9)
#define REG_XO_TOP_XTAL_SEL_MODE_DR         (1 << 10)
#define REG_XO_TOP_XTAL_SEL_MODE            (1 << 11)
#define REG_XO_TOP_XTAL_RSTB_LDO_DR         (1 << 12)
#define REG_XO_TOP_XTAL_RSTB_LDO            (1 << 13)
#define REG_XO_TOP_XTAL_PU_CLKBUF_DR        (1 << 14)
#define REG_XTAL_EN_RCOSC                   (1 << 15)

// REG_073
#define REG_XO_TOP_XTAL_PU_CLKBUF_SLP_SHIFT 0
#define REG_XO_TOP_XTAL_PU_CLKBUF_SLP_MASK  (0xFF << REG_XO_TOP_XTAL_PU_CLKBUF_SLP_SHIFT)
#define REG_XO_TOP_XTAL_PU_CLKBUF_SLP(n)    BITFIELD_VAL(REG_XO_TOP_XTAL_PU_CLKBUF_SLP, n)
#define REG_XO_TOP_XTAL_PU_CLKBUF_SHIFT     8
#define REG_XO_TOP_XTAL_PU_CLKBUF_MASK      (0xFF << REG_XO_TOP_XTAL_PU_CLKBUF_SHIFT)
#define REG_XO_TOP_XTAL_PU_CLKBUF(n)        BITFIELD_VAL(REG_XO_TOP_XTAL_PU_CLKBUF, n)

// REG_074
#define REG_XO_TOP_XTAL_PU_CLKBUF_EXTRA_SHIFT 0
#define REG_XO_TOP_XTAL_PU_CLKBUF_EXTRA_MASK (0xFF << REG_XO_TOP_XTAL_PU_CLKBUF_EXTRA_SHIFT)
#define REG_XO_TOP_XTAL_PU_CLKBUF_EXTRA(n)  BITFIELD_VAL(REG_XO_TOP_XTAL_PU_CLKBUF_EXTRA, n)
#define REG_XO_TOP_XTAL_PU_CLKBUF_DSLEEP_SHIFT 8
#define REG_XO_TOP_XTAL_PU_CLKBUF_DSLEEP_MASK (0xFF << REG_XO_TOP_XTAL_PU_CLKBUF_DSLEEP_SHIFT)
#define REG_XO_TOP_XTAL_PU_CLKBUF_DSLEEP(n) BITFIELD_VAL(REG_XO_TOP_XTAL_PU_CLKBUF_DSLEEP, n)

// REG_075
#define REG_XTAL_CLKBUF_DRV_15_0_SHIFT      0
#define REG_XTAL_CLKBUF_DRV_15_0_MASK       (0xFFFF << REG_XTAL_CLKBUF_DRV_15_0_SHIFT)
#define REG_XTAL_CLKBUF_DRV_15_0(n)         BITFIELD_VAL(REG_XTAL_CLKBUF_DRV_15_0, n)

// REG_076
#define REG_XTAL_CLKBUF_DRV_17_16_SHIFT     0
#define REG_XTAL_CLKBUF_DRV_17_16_MASK      (0x3 << REG_XTAL_CLKBUF_DRV_17_16_SHIFT)
#define REG_XTAL_CLKBUF_DRV_17_16(n)        BITFIELD_VAL(REG_XTAL_CLKBUF_DRV_17_16, n)
#define REG_XTAL_RCOSC_TRIM_SHIFT           2
#define REG_XTAL_RCOSC_TRIM_MASK            (0xF << REG_XTAL_RCOSC_TRIM_SHIFT)
#define REG_XTAL_RCOSC_TRIM(n)              BITFIELD_VAL(REG_XTAL_RCOSC_TRIM, n)
#define REG_XTAL_VRDAC_SHIFT                6
#define REG_XTAL_VRDAC_MASK                 (0x3 << REG_XTAL_VRDAC_SHIFT)
#define REG_XTAL_VRDAC(n)                   BITFIELD_VAL(REG_XTAL_VRDAC, n)
#define REG_XTAL_SEL_TP_SHIFT               8
#define REG_XTAL_SEL_TP_MASK                (0x7 << REG_XTAL_SEL_TP_SHIFT)
#define REG_XTAL_SEL_TP(n)                  BITFIELD_VAL(REG_XTAL_SEL_TP, n)
#define REG_XTAL_MANUAL_KICK                (1 << 11)
#define REG_XTAL_LDO_HPM_VTRIM_SHIFT        12
#define REG_XTAL_LDO_HPM_VTRIM_MASK         (0x7 << REG_XTAL_LDO_HPM_VTRIM_SHIFT)
#define REG_XTAL_LDO_HPM_VTRIM(n)           BITFIELD_VAL(REG_XTAL_LDO_HPM_VTRIM, n)
#define REG_XTAL_EN_MANUAL_KICK             (1 << 15)

// REG_077
#define REG_XTAL_FVAR_SHIFT                 0
#define REG_XTAL_FVAR_MASK                  (0xFF << REG_XTAL_FVAR_SHIFT)
#define REG_XTAL_FVAR(n)                    BITFIELD_VAL(REG_XTAL_FVAR, n)
#define REG_XTAL_CORE_ITRIM_SHIFT           8
#define REG_XTAL_CORE_ITRIM_MASK            (0x3 << REG_XTAL_CORE_ITRIM_SHIFT)
#define REG_XTAL_CORE_ITRIM(n)              BITFIELD_VAL(REG_XTAL_CORE_ITRIM, n)
#define REG_XTAL_LDO_HPM_ITRIM_SHIFT        10
#define REG_XTAL_LDO_HPM_ITRIM_MASK         (0x3 << REG_XTAL_LDO_HPM_ITRIM_SHIFT)
#define REG_XTAL_LDO_HPM_ITRIM(n)           BITFIELD_VAL(REG_XTAL_LDO_HPM_ITRIM, n)
#define REG_XTAL_LDO_LPM_VTRIM_SHIFT        12
#define REG_XTAL_LDO_LPM_VTRIM_MASK         (0x3 << REG_XTAL_LDO_LPM_VTRIM_SHIFT)
#define REG_XTAL_LDO_LPM_VTRIM(n)           BITFIELD_VAL(REG_XTAL_LDO_LPM_VTRIM, n)
#define REG_XTAL_TP_CLK_DRV_SHIFT           14
#define REG_XTAL_TP_CLK_DRV_MASK            (0x3 << REG_XTAL_TP_CLK_DRV_SHIFT)
#define REG_XTAL_TP_CLK_DRV(n)              BITFIELD_VAL(REG_XTAL_TP_CLK_DRV, n)

// REG_078
#define REG_XTAL_AAC_SLEEP_EN               (1 << 0)
#define REG_XTAL_AAC_RESETN                 (1 << 1)
#define REG_XTAL_AAC_ICAL_DR                (1 << 2)
#define REG_XTAL_AAC_ICAL_SHIFT             3
#define REG_XTAL_AAC_ICAL_MASK              (0x1F << REG_XTAL_AAC_ICAL_SHIFT)
#define REG_XTAL_AAC_ICAL(n)                BITFIELD_VAL(REG_XTAL_AAC_ICAL, n)
#define REG_STABLE_VALUE_SHIFT              8
#define REG_STABLE_VALUE_MASK               (0x1F << REG_STABLE_VALUE_SHIFT)
#define REG_STABLE_VALUE(n)                 BITFIELD_VAL(REG_STABLE_VALUE, n)
#define REG_CRYCAL_DIV_MODE_SHIFT           13
#define REG_CRYCAL_DIV_MODE_MASK            (0x3 << REG_CRYCAL_DIV_MODE_SHIFT)
#define REG_CRYCAL_DIV_MODE(n)              BITFIELD_VAL(REG_CRYCAL_DIV_MODE, n)
#define REG_XTAL_DCXO_EN_OUT                (1 << 15)

// REG_079
#define REG_AMP_ABS_NORMAL_SHIFT            0
#define REG_AMP_ABS_NORMAL_MASK             (0x3 << REG_AMP_ABS_NORMAL_SHIFT)
#define REG_AMP_ABS_NORMAL(n)               BITFIELD_VAL(REG_AMP_ABS_NORMAL, n)
#define REG_AMP_ABS_DSLEEP_SHIFT            2
#define REG_AMP_ABS_DSLEEP_MASK             (0x3 << REG_AMP_ABS_DSLEEP_SHIFT)
#define REG_AMP_ABS_DSLEEP(n)               BITFIELD_VAL(REG_AMP_ABS_DSLEEP, n)
#define REG_AMP_RANGE_NORMAL_SHIFT          4
#define REG_AMP_RANGE_NORMAL_MASK           (0xF << REG_AMP_RANGE_NORMAL_SHIFT)
#define REG_AMP_RANGE_NORMAL(n)             BITFIELD_VAL(REG_AMP_RANGE_NORMAL, n)
#define REG_AMP_RANGE_DSLEEP_SHIFT          8
#define REG_AMP_RANGE_DSLEEP_MASK           (0xF << REG_AMP_RANGE_DSLEEP_SHIFT)
#define REG_AMP_RANGE_DSLEEP(n)             BITFIELD_VAL(REG_AMP_RANGE_DSLEEP, n)
#define REG_PU_OSC_DLY_CNT_SHIFT            12
#define REG_PU_OSC_DLY_CNT_MASK             (0xF << REG_PU_OSC_DLY_CNT_SHIFT)
#define REG_PU_OSC_DLY_CNT(n)               BITFIELD_VAL(REG_PU_OSC_DLY_CNT, n)

// REG_07A
#define REG_XTAL_FCAP_NORMAL_SHIFT          0
#define REG_XTAL_FCAP_NORMAL_MASK           (0xFF << REG_XTAL_FCAP_NORMAL_SHIFT)
#define REG_XTAL_FCAP_NORMAL(n)             BITFIELD_VAL(REG_XTAL_FCAP_NORMAL, n)
#define REG_XTAL_FCAP_DSLEEP_SHIFT          8
#define REG_XTAL_FCAP_DSLEEP_MASK           (0xFF << REG_XTAL_FCAP_DSLEEP_SHIFT)
#define REG_XTAL_FCAP_DSLEEP(n)             BITFIELD_VAL(REG_XTAL_FCAP_DSLEEP, n)

// REG_07B
#define CALOUT_LEAR                         (1 << 0)
#define CALOUT_REAR                         (1 << 1)
#define DBG_RCOSC48M_GOAL_CNT_13_0_SHIFT    2
#define DBG_RCOSC48M_GOAL_CNT_13_0_MASK     (0x3FFF << DBG_RCOSC48M_GOAL_CNT_13_0_SHIFT)
#define DBG_RCOSC48M_GOAL_CNT_13_0(n)       BITFIELD_VAL(DBG_RCOSC48M_GOAL_CNT_13_0, n)

// REG_07C
#define DBG_RCOSC48M_GOAL_CNT_16_14_SHIFT   0
#define DBG_RCOSC48M_GOAL_CNT_16_14_MASK    (0x7 << DBG_RCOSC48M_GOAL_CNT_16_14_SHIFT)
#define DBG_RCOSC48M_GOAL_CNT_16_14(n)      BITFIELD_VAL(DBG_RCOSC48M_GOAL_CNT_16_14, n)
#define RCOSC48M_FTRIM_SHIFT                3
#define RCOSC48M_FTRIM_MASK                 (0x3F << RCOSC48M_FTRIM_SHIFT)
#define RCOSC48M_FTRIM(n)                   BITFIELD_VAL(RCOSC48M_FTRIM, n)

// REG_07D
#define REG_REF_24M_DIV_SHIFT               0
#define REG_REF_24M_DIV_MASK                (0x3F << REG_REF_24M_DIV_SHIFT)
#define REG_REF_24M_DIV(n)                  BITFIELD_VAL(REG_REF_24M_DIV, n)
#define REG_RCOSC48M_LLC_SFT_RSTN           (1 << 6)
#define REG_RCOSC48M_CALIB_EN               (1 << 7)
#define REG_RCOSC48M_TUN_CODE_DR            (1 << 8)
#define REG_RCOSC48M_POLAR_SEL              (1 << 9)
#define DBG_RCOSC48M_STATE_SHIFT            10
#define DBG_RCOSC48M_STATE_MASK             (0x3 << DBG_RCOSC48M_STATE_SHIFT)
#define DBG_RCOSC48M_STATE(n)               BITFIELD_VAL(DBG_RCOSC48M_STATE, n)
#define DBG_RCOSC48M_TUN_DONE               (1 << 12)

// REG_07E
#define REG_RCOSC48M_REF_DIV_NUM_SHIFT      0
#define REG_RCOSC48M_REF_DIV_NUM_MASK       (0x3FF << REG_RCOSC48M_REF_DIV_NUM_SHIFT)
#define REG_RCOSC48M_REF_DIV_NUM(n)         BITFIELD_VAL(REG_RCOSC48M_REF_DIV_NUM, n)
#define REG_RCOSC48M_TUN_CODE_SHIFT         10
#define REG_RCOSC48M_TUN_CODE_MASK          (0x3F << REG_RCOSC48M_TUN_CODE_SHIFT)
#define REG_RCOSC48M_TUN_CODE(n)            BITFIELD_VAL(REG_RCOSC48M_TUN_CODE, n)

// REG_07F
#define REG_RCOSC48M_GOAL_CNT_MARK_15_0_SHIFT 0
#define REG_RCOSC48M_GOAL_CNT_MARK_15_0_MASK (0xFFFF << REG_RCOSC48M_GOAL_CNT_MARK_15_0_SHIFT)
#define REG_RCOSC48M_GOAL_CNT_MARK_15_0(n)  BITFIELD_VAL(REG_RCOSC48M_GOAL_CNT_MARK_15_0, n)

// REG_080
#define REG_RCOSC48M_GOAL_CNT_MARK_16       (1 << 0)
#define DBG_RCOSC48M_REF_CNT_SHIFT          1
#define DBG_RCOSC48M_REF_CNT_MASK           (0x3FF << DBG_RCOSC48M_REF_CNT_SHIFT)
#define DBG_RCOSC48M_REF_CNT(n)             BITFIELD_VAL(DBG_RCOSC48M_REF_CNT, n)

// REG_081
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

// REG_082
#define REG_CLKMUX_DVDD0P9_VSEL_SHIFT       0
#define REG_CLKMUX_DVDD0P9_VSEL_MASK        (0x1F << REG_CLKMUX_DVDD0P9_VSEL_SHIFT)
#define REG_CLKMUX_DVDD0P9_VSEL(n)          BITFIELD_VAL(REG_CLKMUX_DVDD0P9_VSEL, n)
#define REG_CLKMUX_DVDD_SEL                 (1 << 5)
#define REG_CLKMUX_PU_DVDD0P9               (1 << 6)

// REG_083
#define CFG_TX_PEAK_OFF_ADC_EN              (1 << 0)
#define CFG_TX_PEAK_OFF_DAC_EN              (1 << 1)
#define CFG_PEAK_DET_DR                     (1 << 2)
#define CFG_TX_PEAK_OFF_ADC                 (1 << 3)
#define CFG_TX_PEAK_OFF_DAC                 (1 << 4)
#define CFG_PEAK_DET_DB_DELAY_SHIFT         5
#define CFG_PEAK_DET_DB_DELAY_MASK          (0x7 << CFG_PEAK_DET_DB_DELAY_SHIFT)
#define CFG_PEAK_DET_DB_DELAY(n)            BITFIELD_VAL(CFG_PEAK_DET_DB_DELAY, n)
#define TX_DIFF_PEAK_DET_NL_STATUS          (1 << 8)
#define TX_DIFF_PEAK_DET_NR_STATUS          (1 << 9)
#define TX_DIFF_PEAK_DET_PL_STATUS          (1 << 10)
#define TX_DIFF_PEAK_DET_PR_STATUS          (1 << 11)
#define TX_DIFF_PEAK_DET_STATUS             (1 << 12)

// REG_084
#define REG_TX_DIFF_LP_VREFBUF_L_SHIFT      0
#define REG_TX_DIFF_LP_VREFBUF_L_MASK       (0x7 << REG_TX_DIFF_LP_VREFBUF_L_SHIFT)
#define REG_TX_DIFF_LP_VREFBUF_L(n)         BITFIELD_VAL(REG_TX_DIFF_LP_VREFBUF_L, n)
#define REG_TX_DIFF_LP_VREFBUF_R_SHIFT      3
#define REG_TX_DIFF_LP_VREFBUF_R_MASK       (0x7 << REG_TX_DIFF_LP_VREFBUF_R_SHIFT)
#define REG_TX_DIFF_LP_VREFBUF_R(n)         BITFIELD_VAL(REG_TX_DIFF_LP_VREFBUF_R, n)
#define REG_ZERO_DETECT_CHANGE              (1 << 6)
#define REG_ZERO_DETECT_POWER_DOWN          (1 << 7)
#define REG_ZERO_DETECT_POWER_DOWN_DIRECT   (1 << 8)
#define REG_CODEC_TX_SW_MODE                (1 << 9)
#define REG_CODEC_TX_PEAK_DET_BIT           (1 << 10)
#define REG_CODEC_TX_PEAK_NL_EN             (1 << 11)
#define REG_CODEC_TX_PEAK_NR_EN             (1 << 12)
#define REG_CODEC_TX_PEAK_PL_EN             (1 << 13)
#define REG_CODEC_TX_PEAK_PR_EN             (1 << 14)

// REG_085
#define REG_CODEC_ADCC_DITHER_EN            (1 << 0)
#define REG_CODEC_ADCD_DITHER_EN            (1 << 1)
#define REG_CODEC_ADCE_DITHER_EN            (1 << 2)
#define CFG_ADCC_DITHER_CLK_INV             (1 << 3)
#define CFG_ADCD_DITHER_CLK_INV             (1 << 4)
#define CFG_ADCE_DITHER_CLK_INV             (1 << 5)
#define CFG_TX_EN_DACLDO_DR                 (1 << 6)
#define CFG_TX_EN_DACLDO                    (1 << 7)
#define CFG_TX_EN_S1PA_DR                   (1 << 8)
#define CFG_TX_EN_S1PA                      (1 << 9)
#define CFG_TX_EN_S2PA_DR                   (1 << 10)
#define CFG_TX_EN_S2PA                      (1 << 11)
#define CFG_TX_EN_S3PA_DR                   (1 << 12)
#define CFG_TX_EN_S3PA                      (1 << 13)
#define REG_CODEC_TX_DIFF_EN_LDAC           (1 << 14)
#define REG_CODEC_TX_DIFF_EN_RDAC           (1 << 15)

// REG_086
#define REG_CODEC_TX_EAR_COMP_PA_OUT_L      (1 << 0)
#define REG_CODEC_TX_EAR_COMP_PA_OUT_R      (1 << 1)
#define REG_CODEC_TX_EN_LDAC                (1 << 2)
#define REG_CODEC_TX_EN_RDAC                (1 << 3)
#define REG_CODEC_TX_EAR_CAL_CH             (1 << 4)

// REG_087
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

/* ---- PAGE0 END ---- */


/* ---- PAGE1 START ---- */

// REG_160
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_161
#define REG_CODEC_TX_DIFF_CLK_MODE          (1 << 0)
#define REG_CODEC_TX_DIFF_CLK_EDGE_SEL      (1 << 1)
#define REG_CODEC_TX_DIFF_VSEL_SHIFT        2
#define REG_CODEC_TX_DIFF_VSEL_MASK         (0x7 << REG_CODEC_TX_DIFF_VSEL_SHIFT)
#define REG_CODEC_TX_DIFF_VSEL(n)           BITFIELD_VAL(REG_CODEC_TX_DIFF_VSEL, n)
#define CODEC_TX_DIFF_OFF_PATH_SEL          (1 << 5)

// REG_162
#define REG_RX_PGAA_CHANSEL_SHIFT           0
#define REG_RX_PGAA_CHANSEL_MASK            (0x3 << REG_RX_PGAA_CHANSEL_SHIFT)
#define REG_RX_PGAA_CHANSEL(n)              BITFIELD_VAL(REG_RX_PGAA_CHANSEL, n)
#define REG_RX_PGAB_CHANSEL_SHIFT           2
#define REG_RX_PGAB_CHANSEL_MASK            (0x3 << REG_RX_PGAB_CHANSEL_SHIFT)
#define REG_RX_PGAB_CHANSEL(n)              BITFIELD_VAL(REG_RX_PGAB_CHANSEL, n)
#define REG_RX_PGAA_EN                      (1 << 4)
#define REG_RX_PGAB_EN                      (1 << 5)
#define CFG_PGAA_RESETN                     (1 << 6)
#define CFG_PGAB_RESETN                     (1 << 7)
#define CFG_PGAA_RESETN_DR                  (1 << 8)
#define CFG_PGAB_RESETN_DR                  (1 << 9)
#define CFG_EN_RX_DR                        (1 << 10)
#define CFG_EN_RX                           (1 << 11)
#define RESERVED_DIG_2_0_SHIFT              13
#define RESERVED_DIG_2_0_MASK               (0x7 << RESERVED_DIG_2_0_SHIFT)
#define RESERVED_DIG_2_0(n)                 BITFIELD_VAL(RESERVED_DIG_2_0, n)

// REG_163
#define REG_RX_PGAA_2P5K_SEL                (1 << 0)
#define REG_RX_PGAA_7P5K_SEL                (1 << 1)
#define REG_RX_PGAA_10K_SEL                 (1 << 2)
#define REG_RX_PGAA_CAPMODE                 (1 << 3)
#define REG_RX_PGAA_DACGAIN_SHIFT           4
#define REG_RX_PGAA_DACGAIN_MASK            (0x3 << REG_RX_PGAA_DACGAIN_SHIFT)
#define REG_RX_PGAA_DACGAIN(n)              BITFIELD_VAL(REG_RX_PGAA_DACGAIN, n)
#define REG_RX_PGAA_FASTSETTLE              (1 << 6)
#define REG_RX_PGAA_IBIT_OPA_SHIFT          7
#define REG_RX_PGAA_IBIT_OPA_MASK           (0x3 << REG_RX_PGAA_IBIT_OPA_SHIFT)
#define REG_RX_PGAA_IBIT_OPA(n)             BITFIELD_VAL(REG_RX_PGAA_IBIT_OPA, n)
#define REG_RX_PGAA_OFFSET_CAL_EN           (1 << 9)
#define REG_RX_PGAA_OP_C_SEL                (1 << 10)
#define REG_RX_PGAA_OP_GM_SEL               (1 << 11)
#define REG_RX_PGAA_OP_R_SEL                (1 << 12)
#define REG_RX_PGAA_RFB_EN                  (1 << 13)
#define REG_RX_PGAA_ZERO_DET_EN             (1 << 14)
#define RESERVED_DIG_3                      (1 << 15)

// REG_164
#define REG_RX_PGAB_2P5K_SEL                (1 << 0)
#define REG_RX_PGAB_7P5K_SEL                (1 << 1)
#define REG_RX_PGAB_10K_SEL                 (1 << 2)
#define REG_RX_PGAB_CAPMODE                 (1 << 3)
#define REG_RX_PGAB_DACGAIN_SHIFT           4
#define REG_RX_PGAB_DACGAIN_MASK            (0x3 << REG_RX_PGAB_DACGAIN_SHIFT)
#define REG_RX_PGAB_DACGAIN(n)              BITFIELD_VAL(REG_RX_PGAB_DACGAIN, n)
#define REG_RX_PGAB_FASTSETTLE              (1 << 6)
#define REG_RX_PGAB_IBIT_OPA_SHIFT          7
#define REG_RX_PGAB_IBIT_OPA_MASK           (0x3 << REG_RX_PGAB_IBIT_OPA_SHIFT)
#define REG_RX_PGAB_IBIT_OPA(n)             BITFIELD_VAL(REG_RX_PGAB_IBIT_OPA, n)
#define REG_RX_PGAB_OFFSET_CAL_EN           (1 << 9)
#define REG_RX_PGAB_OP_C_SEL                (1 << 10)
#define REG_RX_PGAB_OP_GM_SEL               (1 << 11)
#define REG_RX_PGAB_OP_R_SEL                (1 << 12)
#define REG_RX_PGAB_RFB_EN                  (1 << 13)
#define REG_RX_PGAB_ZERO_DET_EN             (1 << 14)
#define RESERVED_DIG_4                      (1 << 15)

// REG_165
#define REG_CODEC_ADCA_CH_SEL_SHIFT         0
#define REG_CODEC_ADCA_CH_SEL_MASK          (0x3 << REG_CODEC_ADCA_CH_SEL_SHIFT)
#define REG_CODEC_ADCA_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCA_CH_SEL, n)
#define REG_CODEC_ADCB_CH_SEL_SHIFT         2
#define REG_CODEC_ADCB_CH_SEL_MASK          (0x3 << REG_CODEC_ADCB_CH_SEL_SHIFT)
#define REG_CODEC_ADCB_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCB_CH_SEL, n)
#define REG_CODEC_IDETA_EN                  (1 << 4)
#define REG_CODEC_IDETB_EN                  (1 << 5)
#define REG_CODEC_ADCA_DITHER_EN            (1 << 6)
#define REG_CODEC_ADCB_DITHER_EN            (1 << 7)
#define CFG_ADCA_DITHER_CLK_INV             (1 << 8)
#define CFG_ADCB_DITHER_CLK_INV             (1 << 9)
#define REG_CODEC_ADCC_CH_SEL_SHIFT         10
#define REG_CODEC_ADCC_CH_SEL_MASK          (0x3 << REG_CODEC_ADCC_CH_SEL_SHIFT)
#define REG_CODEC_ADCC_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCC_CH_SEL, n)
#define REG_CODEC_ADCD_CH_SEL_SHIFT         12
#define REG_CODEC_ADCD_CH_SEL_MASK          (0x3 << REG_CODEC_ADCD_CH_SEL_SHIFT)
#define REG_CODEC_ADCD_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCD_CH_SEL, n)
#define REG_CODEC_ADCE_CH_SEL_SHIFT         14
#define REG_CODEC_ADCE_CH_SEL_MASK          (0x3 << REG_CODEC_ADCE_CH_SEL_SHIFT)
#define REG_CODEC_ADCE_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCE_CH_SEL, n)

// REG_166
#define REG_CODEC_ADC_CAP_BIT1_SHIFT        0
#define REG_CODEC_ADC_CAP_BIT1_MASK         (0x1F << REG_CODEC_ADC_CAP_BIT1_SHIFT)
#define REG_CODEC_ADC_CAP_BIT1(n)           BITFIELD_VAL(REG_CODEC_ADC_CAP_BIT1, n)
#define REG_CODEC_ADC_CAP_BIT2_SHIFT        5
#define REG_CODEC_ADC_CAP_BIT2_MASK         (0x1F << REG_CODEC_ADC_CAP_BIT2_SHIFT)
#define REG_CODEC_ADC_CAP_BIT2(n)           BITFIELD_VAL(REG_CODEC_ADC_CAP_BIT2, n)
#define REG_CODEC_ADC_CAP_BIT3_SHIFT        10
#define REG_CODEC_ADC_CAP_BIT3_MASK         (0x1F << REG_CODEC_ADC_CAP_BIT3_SHIFT)
#define REG_CODEC_ADC_CAP_BIT3(n)           BITFIELD_VAL(REG_CODEC_ADC_CAP_BIT3, n)
#define REG_CODEC_ADC_CLK_SEL               (1 << 15)

// REG_167
#define REG_CODEC_ADC_DITHER0P5_EN          (1 << 0)
#define REG_CODEC_ADC_DITHER0P25_EN         (1 << 1)
#define REG_CODEC_ADC_DITHER1_EN            (1 << 2)
#define REG_CODEC_ADC_DITHER2_EN            (1 << 3)
#define REG_CODEC_ADC_DITHER_PHASE_SEL      (1 << 4)
#define REG_CODEC_ADC_REG_VSEL_SHIFT        5
#define REG_CODEC_ADC_REG_VSEL_MASK         (0x7 << REG_CODEC_ADC_REG_VSEL_SHIFT)
#define REG_CODEC_ADC_REG_VSEL(n)           BITFIELD_VAL(REG_CODEC_ADC_REG_VSEL, n)
#define REG_CODEC_ADC_RES_SEL_SHIFT         8
#define REG_CODEC_ADC_RES_SEL_MASK          (0x7 << REG_CODEC_ADC_RES_SEL_SHIFT)
#define REG_CODEC_ADC_RES_SEL(n)            BITFIELD_VAL(REG_CODEC_ADC_RES_SEL, n)
#define REG_CODEC_EN_ADCA                   (1 << 11)
#define REG_CODEC_EN_ADCB                   (1 << 12)
#define REG_CODEC_IDETC_EN                  (1 << 13)
#define REG_CODEC_IDETD_EN                  (1 << 14)
#define REG_CODEC_IDETE_EN                  (1 << 15)

// REG_168
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

// REG_169
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

// REG_16A
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
#define RESERVED_DIG_8_5_SHIFT              12
#define RESERVED_DIG_8_5_MASK               (0xF << RESERVED_DIG_8_5_SHIFT)
#define RESERVED_DIG_8_5(n)                 BITFIELD_VAL(RESERVED_DIG_8_5, n)

// REG_16B
#define REG_RX_PGAA_GAIN_SHIFT              0
#define REG_RX_PGAA_GAIN_MASK               (0x7 << REG_RX_PGAA_GAIN_SHIFT)
#define REG_RX_PGAA_GAIN(n)                 BITFIELD_VAL(REG_RX_PGAA_GAIN, n)
#define REG_RX_PGAA_LARGEGAIN_SHIFT         3
#define REG_RX_PGAA_LARGEGAIN_MASK          (0x7 << REG_RX_PGAA_LARGEGAIN_SHIFT)
#define REG_RX_PGAA_LARGEGAIN(n)            BITFIELD_VAL(REG_RX_PGAA_LARGEGAIN, n)
#define REG_RX_PGAB_GAIN_SHIFT              6
#define REG_RX_PGAB_GAIN_MASK               (0x7 << REG_RX_PGAB_GAIN_SHIFT)
#define REG_RX_PGAB_GAIN(n)                 BITFIELD_VAL(REG_RX_PGAB_GAIN, n)
#define REG_RX_PGAB_LARGEGAIN_SHIFT         9
#define REG_RX_PGAB_LARGEGAIN_MASK          (0x7 << REG_RX_PGAB_LARGEGAIN_SHIFT)
#define REG_RX_PGAB_LARGEGAIN(n)            BITFIELD_VAL(REG_RX_PGAB_LARGEGAIN, n)
#define REG_CODEC_ADCA_GAIN_UPDATE          (1 << 12)
#define REG_RX_PGAA_LARGEGAIN_UPDATE        (1 << 13)
#define REG_RX_PGAA_DRE_UPDATE              (1 << 14)
#define REG_RX_PGAA_GAIN_UPDATE             (1 << 15)

// REG_16C
#define CODEC_DIN_L_HI_RSTVAL_SHIFT         0
#define CODEC_DIN_L_HI_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_HI_RSTVAL_SHIFT)
#define CODEC_DIN_L_HI_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_HI_RSTVAL, n)

// REG_16D
#define CODEC_DIN_L_LO_RSTVAL_SHIFT         0
#define CODEC_DIN_L_LO_RSTVAL_MASK          (0xFFFF << CODEC_DIN_L_LO_RSTVAL_SHIFT)
#define CODEC_DIN_L_LO_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_L_LO_RSTVAL, n)

// REG_16E
#define CODEC_DIN_R_HI_RSTVAL_SHIFT         0
#define CODEC_DIN_R_HI_RSTVAL_MASK          (0xFFFF << CODEC_DIN_R_HI_RSTVAL_SHIFT)
#define CODEC_DIN_R_HI_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_R_HI_RSTVAL, n)

// REG_16F
#define CODEC_DIN_R_LO_RSTVAL_SHIFT         0
#define CODEC_DIN_R_LO_RSTVAL_MASK          (0xFFFF << CODEC_DIN_R_LO_RSTVAL_SHIFT)
#define CODEC_DIN_R_LO_RSTVAL(n)            BITFIELD_VAL(CODEC_DIN_R_LO_RSTVAL, n)

// REG_170
#define CFG_TX_TREE_EN                      (1 << 0)
#define CFG_CODEC_DIN_L_RST                 (1 << 1)
#define CFG_CODEC_DIN_R_RST                 (1 << 2)
#define REG_CODEC_ADCA_RES_2P5K             (1 << 3)
#define REG_CODEC_ADCA_RES_2P5K_UPDATE      (1 << 4)
#define REG_RX_PGAB_LARGEGAIN_UPDATE        (1 << 5)
#define REG_CODEC_ADCB_RES_2P5K             (1 << 6)
#define REG_CODEC_ADCB_RES_2P5K_UPDATE      (1 << 7)
#define REG_CODEC_ADCB_GAIN_UPDATE          (1 << 8)
#define REG_RX_PGAB_DRE_UPDATE              (1 << 9)
#define REG_RX_PGAB_GAIN_UPDATE             (1 << 10)
#define REG_CODEC_EN_CODEC_HPPA_LDO         (1 << 11)
#define REG_CODEC_LP_CODEC_HPPA_LDO         (1 << 12)
#define RESERVED_DIG_11_9_SHIFT             13
#define RESERVED_DIG_11_9_MASK              (0x7 << RESERVED_DIG_11_9_SHIFT)
#define RESERVED_DIG_11_9(n)                BITFIELD_VAL(RESERVED_DIG_11_9, n)

// REG_171
#define REG_RX_PGAA_DRE_DR                  (1 << 0)
#define REG_RX_PGAB_DRE_DR                  (1 << 1)
#define REG_RX_PGAA_DRE_SHIFT               2
#define REG_RX_PGAA_DRE_MASK                (0xF << REG_RX_PGAA_DRE_SHIFT)
#define REG_RX_PGAA_DRE(n)                  BITFIELD_VAL(REG_RX_PGAA_DRE, n)
#define REG_RX_PGAB_DRE_SHIFT               6
#define REG_RX_PGAB_DRE_MASK                (0xF << REG_RX_PGAB_DRE_SHIFT)
#define REG_RX_PGAB_DRE(n)                  BITFIELD_VAL(REG_RX_PGAB_DRE, n)
#define REG_CODEC_ADCA_GAIN_BIT_SHIFT       10
#define REG_CODEC_ADCA_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCA_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCA_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_GAIN_BIT, n)
#define REG_CODEC_ADCB_GAIN_BIT_SHIFT       13
#define REG_CODEC_ADCB_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCB_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCB_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_GAIN_BIT, n)

/* ---- PAGE1 END ---- */


/* ---- PAGE2 START ---- */

// REG_260
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_261
#define REG_SSC_DELAY_26M_SHIFT             0
#define REG_SSC_DELAY_26M_MASK              (0x7 << REG_SSC_DELAY_26M_SHIFT)
#define REG_SSC_DELAY_26M(n)                BITFIELD_VAL(REG_SSC_DELAY_26M, n)
#define REG_SSC_DELAY_52M_SHIFT             3
#define REG_SSC_DELAY_52M_MASK              (0x7 << REG_SSC_DELAY_52M_SHIFT)
#define REG_SSC_DELAY_52M(n)                BITFIELD_VAL(REG_SSC_DELAY_52M, n)
#define REG_SSC_DELAY_104M_SHIFT            6
#define REG_SSC_DELAY_104M_MASK             (0x7 << REG_SSC_DELAY_104M_SHIFT)
#define REG_SSC_DELAY_104M(n)               BITFIELD_VAL(REG_SSC_DELAY_104M, n)
#define REG_SSC_EN_26M                      (1 << 9)
#define REG_SSC_EN_52M                      (1 << 10)
#define REG_SSC_EN_104M                     (1 << 11)
#define RESERVED_DIG_15_12_SHIFT            12
#define RESERVED_DIG_15_12_MASK             (0xF << RESERVED_DIG_15_12_SHIFT)
#define RESERVED_DIG_15_12(n)               BITFIELD_VAL(RESERVED_DIG_15_12, n)

// REG_262
#define CFG_TX_DIFF_TREE_TRI_EN             (1 << 0)
#define CFG_TX_DIFF_CH0_MUTE                (1 << 1)
#define CFG_TX_DIFF_CH1_MUTE                (1 << 2)
#define CFG_CODEC_DIFF_DIN_L_RST            (1 << 3)
#define CFG_CODEC_DIFF_DIN_R_RST            (1 << 4)
#define RESERVED_DIG_26_16_SHIFT            5
#define RESERVED_DIG_26_16_MASK             (0x7FF << RESERVED_DIG_26_16_SHIFT)
#define RESERVED_DIG_26_16(n)               BITFIELD_VAL(RESERVED_DIG_26_16, n)

// REG_263
#define CODEC_DIFF_DIN_L_HI_RSTVAL_15_0_SHIFT 0
#define CODEC_DIFF_DIN_L_HI_RSTVAL_15_0_MASK (0xFFFF << CODEC_DIFF_DIN_L_HI_RSTVAL_15_0_SHIFT)
#define CODEC_DIFF_DIN_L_HI_RSTVAL_15_0(n)  BITFIELD_VAL(CODEC_DIFF_DIN_L_HI_RSTVAL_15_0, n)

// REG_264
#define CODEC_DIFF_DIN_L_HI_RSTVAL_31_16_SHIFT 0
#define CODEC_DIFF_DIN_L_HI_RSTVAL_31_16_MASK (0xFFFF << CODEC_DIFF_DIN_L_HI_RSTVAL_31_16_SHIFT)
#define CODEC_DIFF_DIN_L_HI_RSTVAL_31_16(n) BITFIELD_VAL(CODEC_DIFF_DIN_L_HI_RSTVAL_31_16, n)

// REG_265
#define CODEC_DIFF_DIN_L_HI_RSTVAL_47_32_SHIFT 0
#define CODEC_DIFF_DIN_L_HI_RSTVAL_47_32_MASK (0xFFFF << CODEC_DIFF_DIN_L_HI_RSTVAL_47_32_SHIFT)
#define CODEC_DIFF_DIN_L_HI_RSTVAL_47_32(n) BITFIELD_VAL(CODEC_DIFF_DIN_L_HI_RSTVAL_47_32, n)

// REG_266
#define CODEC_DIFF_DIN_L_HI_RSTVAL_63_48_SHIFT 0
#define CODEC_DIFF_DIN_L_HI_RSTVAL_63_48_MASK (0xFFFF << CODEC_DIFF_DIN_L_HI_RSTVAL_63_48_SHIFT)
#define CODEC_DIFF_DIN_L_HI_RSTVAL_63_48(n) BITFIELD_VAL(CODEC_DIFF_DIN_L_HI_RSTVAL_63_48, n)

// REG_267
#define CODEC_DIFF_DIN_L_LO_RSTVAL_15_0_SHIFT 0
#define CODEC_DIFF_DIN_L_LO_RSTVAL_15_0_MASK (0xFFFF << CODEC_DIFF_DIN_L_LO_RSTVAL_15_0_SHIFT)
#define CODEC_DIFF_DIN_L_LO_RSTVAL_15_0(n)  BITFIELD_VAL(CODEC_DIFF_DIN_L_LO_RSTVAL_15_0, n)

// REG_268
#define CODEC_DIFF_DIN_L_LO_RSTVAL_31_16_SHIFT 0
#define CODEC_DIFF_DIN_L_LO_RSTVAL_31_16_MASK (0xFFFF << CODEC_DIFF_DIN_L_LO_RSTVAL_31_16_SHIFT)
#define CODEC_DIFF_DIN_L_LO_RSTVAL_31_16(n) BITFIELD_VAL(CODEC_DIFF_DIN_L_LO_RSTVAL_31_16, n)

// REG_269
#define CODEC_DIFF_DIN_L_LO_RSTVAL_47_32_SHIFT 0
#define CODEC_DIFF_DIN_L_LO_RSTVAL_47_32_MASK (0xFFFF << CODEC_DIFF_DIN_L_LO_RSTVAL_47_32_SHIFT)
#define CODEC_DIFF_DIN_L_LO_RSTVAL_47_32(n) BITFIELD_VAL(CODEC_DIFF_DIN_L_LO_RSTVAL_47_32, n)

// REG_26A
#define CODEC_DIFF_DIN_L_LO_RSTVAL_63_48_SHIFT 0
#define CODEC_DIFF_DIN_L_LO_RSTVAL_63_48_MASK (0xFFFF << CODEC_DIFF_DIN_L_LO_RSTVAL_63_48_SHIFT)
#define CODEC_DIFF_DIN_L_LO_RSTVAL_63_48(n) BITFIELD_VAL(CODEC_DIFF_DIN_L_LO_RSTVAL_63_48, n)

// REG_26B
#define CODEC_DIFF_DIN_R_HI_RSTVAL_15_0_SHIFT 0
#define CODEC_DIFF_DIN_R_HI_RSTVAL_15_0_MASK (0xFFFF << CODEC_DIFF_DIN_R_HI_RSTVAL_15_0_SHIFT)
#define CODEC_DIFF_DIN_R_HI_RSTVAL_15_0(n)  BITFIELD_VAL(CODEC_DIFF_DIN_R_HI_RSTVAL_15_0, n)

// REG_26C
#define CODEC_DIFF_DIN_R_HI_RSTVAL_31_16_SHIFT 0
#define CODEC_DIFF_DIN_R_HI_RSTVAL_31_16_MASK (0xFFFF << CODEC_DIFF_DIN_R_HI_RSTVAL_31_16_SHIFT)
#define CODEC_DIFF_DIN_R_HI_RSTVAL_31_16(n) BITFIELD_VAL(CODEC_DIFF_DIN_R_HI_RSTVAL_31_16, n)

// REG_26D
#define CODEC_DIFF_DIN_R_HI_RSTVAL_47_32_SHIFT 0
#define CODEC_DIFF_DIN_R_HI_RSTVAL_47_32_MASK (0xFFFF << CODEC_DIFF_DIN_R_HI_RSTVAL_47_32_SHIFT)
#define CODEC_DIFF_DIN_R_HI_RSTVAL_47_32(n) BITFIELD_VAL(CODEC_DIFF_DIN_R_HI_RSTVAL_47_32, n)

// REG_20E
#define CODEC_DIFF_DIN_R_HI_RSTVAL_63_48_SHIFT 0
#define CODEC_DIFF_DIN_R_HI_RSTVAL_63_48_MASK (0xFFFF << CODEC_DIFF_DIN_R_HI_RSTVAL_63_48_SHIFT)
#define CODEC_DIFF_DIN_R_HI_RSTVAL_63_48(n) BITFIELD_VAL(CODEC_DIFF_DIN_R_HI_RSTVAL_63_48, n)

// REG_26F
#define CODEC_DIFF_DIN_R_LO_RSTVAL_15_0_SHIFT 0
#define CODEC_DIFF_DIN_R_LO_RSTVAL_15_0_MASK (0xFFFF << CODEC_DIFF_DIN_R_LO_RSTVAL_15_0_SHIFT)
#define CODEC_DIFF_DIN_R_LO_RSTVAL_15_0(n)  BITFIELD_VAL(CODEC_DIFF_DIN_R_LO_RSTVAL_15_0, n)

// REG_270
#define CODEC_DIFF_DIN_R_LO_RSTVAL_31_16_SHIFT 0
#define CODEC_DIFF_DIN_R_LO_RSTVAL_31_16_MASK (0xFFFF << CODEC_DIFF_DIN_R_LO_RSTVAL_31_16_SHIFT)
#define CODEC_DIFF_DIN_R_LO_RSTVAL_31_16(n) BITFIELD_VAL(CODEC_DIFF_DIN_R_LO_RSTVAL_31_16, n)

// REG_271
#define CODEC_DIFF_DIN_R_LO_RSTVAL_47_32_SHIFT 0
#define CODEC_DIFF_DIN_R_LO_RSTVAL_47_32_MASK (0xFFFF << CODEC_DIFF_DIN_R_LO_RSTVAL_47_32_SHIFT)
#define CODEC_DIFF_DIN_R_LO_RSTVAL_47_32(n) BITFIELD_VAL(CODEC_DIFF_DIN_R_LO_RSTVAL_47_32, n)

// REG_272
#define CODEC_DIFF_DIN_R_LO_RSTVAL_63_48_SHIFT 0
#define CODEC_DIFF_DIN_R_LO_RSTVAL_63_48_MASK (0xFFFF << CODEC_DIFF_DIN_R_LO_RSTVAL_63_48_SHIFT)
#define CODEC_DIFF_DIN_R_LO_RSTVAL_63_48(n) BITFIELD_VAL(CODEC_DIFF_DIN_R_LO_RSTVAL_63_48, n)

// REG_273
#define REG_CODEC_TX_DIFF_EAR_DRE_GAIN_L_UPDATE (1 << 0)
#define REG_CODEC_TX_DIFF_EAR_DRE_GAIN_R_UPDATE (1 << 1)
#define REG_ZERO_DETECT_POWER_DOWN_DIRECT_DIFF (1 << 2)
#define REG_ZERO_DETECT_POWER_DOWN_DIFF     (1 << 3)
#define REG_CODEC_TX_DIFF_EAR_GAIN_SHIFT    4
#define REG_CODEC_TX_DIFF_EAR_GAIN_MASK     (0x3 << REG_CODEC_TX_DIFF_EAR_GAIN_SHIFT)
#define REG_CODEC_TX_DIFF_EAR_GAIN(n)       BITFIELD_VAL(REG_CODEC_TX_DIFF_EAR_GAIN, n)
#define REG_CLOSE_PA_DIRECT                 (1 << 6)
#define REG_CLOSE_PA_DIRECT_DIFF            (1 << 7)
#define REG_ZERO_DETECT_CHANGE_DIFF         (1 << 8)
#define REG_CODEC_TX_DIFF_EN_EARPA_L        (1 << 9)
#define REG_CODEC_TX_DIFF_EN_EARPA_R        (1 << 10)
#define REG_CLOSE_SPA_DIFF                  (1 << 11)
#define REG_CLOSE_PA_DIFF                   (1 << 12)
#define REG_CLOSE_PA                        (1 << 13)
#define REG_CODEC_TX_DIFF_PU_ZERO_DET_L     (1 << 14)
#define REG_CODEC_TX_DIFF_PU_ZERO_DET_R     (1 << 15)

// REG_274
#define REG_CODEC_TX_DIFF_EN_DACLDO         (1 << 0)
#define REG_CODEC_TX_DIFF_EN_LPPA           (1 << 1)
#define REG_CODEC_TX_DIFF_EN_S1PA           (1 << 2)
#define REG_CODEC_TX_DIFF_EN_S2PA           (1 << 3)
#define REG_CODEC_TX_DIFF_EN_S3PA           (1 << 4)
#define REG_CODEC_TX_DIFF_EN_S4PA           (1 << 5)
#define REG_CODEC_TX_DIFF_EN_S5PA           (1 << 6)
#define REG_CODEC_TX_DIFF_EN_LCLK           (1 << 7)
#define REG_CODEC_TX_DIFF_EN_RCLK           (1 << 8)
#define REG_CODEC_TX_DIFF_EAR_OCEN          (1 << 9)
#define REG_CODEC_TX_DIFF_EAR_LPBIAS        (1 << 10)
#define REG_CODEC_TX_DIFF_EAR_ENBIAS        (1 << 11)
#define REG_CODEC_TX_DIFF_EAR_DR_EN         (1 << 12)
#define REG_CODEC_TX_DIFF_PU_TX_REGULATOR   (1 << 13)
#define REG_CODEC_TX_DIFF_BYPASS_TX_REGULATOR (1 << 14)
#define RESERVED_DIG_27                     (1 << 15)

// REG_275
#define CFG_PRE_CHARGE_ADCA_DR              (1 << 0)
#define REG_CODEC_ADCA_PRE_CHARGE           (1 << 1)
#define ADCA_PRE_CHARGE_TIMER_DLY_SHIFT     2
#define ADCA_PRE_CHARGE_TIMER_DLY_MASK      (0x7F << ADCA_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCA_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCA_PRE_CHARGE_TIMER_DLY, n)
#define RESERVED_DIG_34_28_SHIFT            9
#define RESERVED_DIG_34_28_MASK             (0x7F << RESERVED_DIG_34_28_SHIFT)
#define RESERVED_DIG_34_28(n)               BITFIELD_VAL(RESERVED_DIG_34_28, n)

// REG_276
#define CFG_PRE_CHARGE_ADCB_DR              (1 << 0)
#define REG_CODEC_ADCB_PRE_CHARGE           (1 << 1)
#define ADCB_PRE_CHARGE_TIMER_DLY_SHIFT     2
#define ADCB_PRE_CHARGE_TIMER_DLY_MASK      (0x7F << ADCB_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCB_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCB_PRE_CHARGE_TIMER_DLY, n)
#define RESERVED_DIG_41_35_SHIFT            9
#define RESERVED_DIG_41_35_MASK             (0x7F << RESERVED_DIG_41_35_SHIFT)
#define RESERVED_DIG_41_35(n)               BITFIELD_VAL(RESERVED_DIG_41_35, n)

// REG_277
#define CFG_PRE_CHARGE_ADCC_DR              (1 << 0)
#define REG_CODEC_ADCC_PRE_CHARGE           (1 << 1)
#define ADCC_PRE_CHARGE_TIMER_DLY_SHIFT     2
#define ADCC_PRE_CHARGE_TIMER_DLY_MASK      (0x7F << ADCC_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCC_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCC_PRE_CHARGE_TIMER_DLY, n)
#define RESERVED_DIG_48_42_SHIFT            9
#define RESERVED_DIG_48_42_MASK             (0x7F << RESERVED_DIG_48_42_SHIFT)
#define RESERVED_DIG_48_42(n)               BITFIELD_VAL(RESERVED_DIG_48_42, n)

// REG_278
#define CFG_PRE_CHARGE_ADCD_DR              (1 << 0)
#define REG_CODEC_ADCD_PRE_CHARGE           (1 << 1)
#define ADCD_PRE_CHARGE_TIMER_DLY_SHIFT     2
#define ADCD_PRE_CHARGE_TIMER_DLY_MASK      (0x7F << ADCD_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCD_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCD_PRE_CHARGE_TIMER_DLY, n)
#define RESERVED_DIG_55_49_SHIFT            9
#define RESERVED_DIG_55_49_MASK             (0x7F << RESERVED_DIG_55_49_SHIFT)
#define RESERVED_DIG_55_49(n)               BITFIELD_VAL(RESERVED_DIG_55_49, n)

// REG_279
#define CFG_PRE_CHARGE_ADCE_DR              (1 << 0)
#define REG_CODEC_ADCE_PRE_CHARGE           (1 << 1)
#define ADCE_PRE_CHARGE_TIMER_DLY_SHIFT     2
#define ADCE_PRE_CHARGE_TIMER_DLY_MASK      (0x7F << ADCE_PRE_CHARGE_TIMER_DLY_SHIFT)
#define ADCE_PRE_CHARGE_TIMER_DLY(n)        BITFIELD_VAL(ADCE_PRE_CHARGE_TIMER_DLY, n)
#define RESERVED_DIG_62_56_SHIFT            9
#define RESERVED_DIG_62_56_MASK             (0x7F << RESERVED_DIG_62_56_SHIFT)
#define RESERVED_DIG_62_56(n)               BITFIELD_VAL(RESERVED_DIG_62_56, n)

// REG_27A
#define REG_TX_DIFF_EAR_COMP_L_SHIFT        0
#define REG_TX_DIFF_EAR_COMP_L_MASK         (0x1F << REG_TX_DIFF_EAR_COMP_L_SHIFT)
#define REG_TX_DIFF_EAR_COMP_L(n)           BITFIELD_VAL(REG_TX_DIFF_EAR_COMP_L, n)
#define REG_TX_DIFF_EAR_COMP_R_SHIFT        5
#define REG_TX_DIFF_EAR_COMP_R_MASK         (0x1F << REG_TX_DIFF_EAR_COMP_R_SHIFT)
#define REG_TX_DIFF_EAR_COMP_R(n)           BITFIELD_VAL(REG_TX_DIFF_EAR_COMP_R, n)

// REG_27B
#define REG_TX_DIFF_DAC_VREF_CAS_L_SHIFT    0
#define REG_TX_DIFF_DAC_VREF_CAS_L_MASK     (0x3 << REG_TX_DIFF_DAC_VREF_CAS_L_SHIFT)
#define REG_TX_DIFF_DAC_VREF_CAS_L(n)       BITFIELD_VAL(REG_TX_DIFF_DAC_VREF_CAS_L, n)
#define REG_TX_DIFF_DAC_VREF_CAS_R_SHIFT    2
#define REG_TX_DIFF_DAC_VREF_CAS_R_MASK     (0x3 << REG_TX_DIFF_DAC_VREF_CAS_R_SHIFT)
#define REG_TX_DIFF_DAC_VREF_CAS_R(n)       BITFIELD_VAL(REG_TX_DIFF_DAC_VREF_CAS_R, n)
#define REG_TX_DIFF_EAR_CAS                 (1 << 4)
#define REG_CODEC_TX_EAR_GAIN_SHIFT         5
#define REG_CODEC_TX_EAR_GAIN_MASK          (0x3 << REG_CODEC_TX_EAR_GAIN_SHIFT)
#define REG_CODEC_TX_EAR_GAIN(n)            BITFIELD_VAL(REG_CODEC_TX_EAR_GAIN, n)
#define REG_TX_LP_VREFBUF_L_SHIFT           7
#define REG_TX_LP_VREFBUF_L_MASK            (0x7 << REG_TX_LP_VREFBUF_L_SHIFT)
#define REG_TX_LP_VREFBUF_L(n)              BITFIELD_VAL(REG_TX_LP_VREFBUF_L, n)
#define REG_TX_LP_VREFBUF_R_SHIFT           10
#define REG_TX_LP_VREFBUF_R_MASK            (0x7 << REG_TX_LP_VREFBUF_R_SHIFT)
#define REG_TX_LP_VREFBUF_R(n)              BITFIELD_VAL(REG_TX_LP_VREFBUF_R, n)
#define RESERVED_DIG_65_63_SHIFT            13
#define RESERVED_DIG_65_63_MASK             (0x7 << RESERVED_DIG_65_63_SHIFT)
#define RESERVED_DIG_65_63(n)               BITFIELD_VAL(RESERVED_DIG_65_63, n)

// REG_27C
#define REG_TX_DIFF_DAC_CLK_DELAY_SHIFT     0
#define REG_TX_DIFF_DAC_CLK_DELAY_MASK      (0x3 << REG_TX_DIFF_DAC_CLK_DELAY_SHIFT)
#define REG_TX_DIFF_DAC_CLK_DELAY(n)        BITFIELD_VAL(REG_TX_DIFF_DAC_CLK_DELAY, n)
#define REG_TX_DIFF_SW_DELAY_SHIFT          2
#define REG_TX_DIFF_SW_DELAY_MASK           (0x3 << REG_TX_DIFF_SW_DELAY_SHIFT)
#define REG_TX_DIFF_SW_DELAY(n)             BITFIELD_VAL(REG_TX_DIFF_SW_DELAY, n)
#define REG_TX_DIFF_CLK_12M                 (1 << 4)
#define REG_TX_DIFF_DAC_CLK_EDGE_SEL        (1 << 5)
#define REG_TX_DIFF_DAC_LDO0P9_VSEL_SHIFT   6
#define REG_TX_DIFF_DAC_LDO0P9_VSEL_MASK    (0x1F << REG_TX_DIFF_DAC_LDO0P9_VSEL_SHIFT)
#define REG_TX_DIFF_DAC_LDO0P9_VSEL(n)      BITFIELD_VAL(REG_TX_DIFF_DAC_LDO0P9_VSEL, n)
#define REG_TX_DIFF_DAC_MUTEL               (1 << 11)
#define REG_TX_DIFF_DAC_MUTER               (1 << 12)
#define REG_TX_DIFF_DAC_SWR_SHIFT           13
#define REG_TX_DIFF_DAC_SWR_MASK            (0x3 << REG_TX_DIFF_DAC_SWR_SHIFT)
#define REG_TX_DIFF_DAC_SWR(n)              BITFIELD_VAL(REG_TX_DIFF_DAC_SWR, n)
#define RESERVED_DIG_66                     (1 << 15)

// REG_27D
#define REG_TX_DIFF_DAC_VREF_L_SHIFT        0
#define REG_TX_DIFF_DAC_VREF_L_MASK         (0xF << REG_TX_DIFF_DAC_VREF_L_SHIFT)
#define REG_TX_DIFF_DAC_VREF_L(n)           BITFIELD_VAL(REG_TX_DIFF_DAC_VREF_L, n)
#define REG_TX_DIFF_DAC_VREF_R_SHIFT        4
#define REG_TX_DIFF_DAC_VREF_R_MASK         (0xF << REG_TX_DIFF_DAC_VREF_R_SHIFT)
#define REG_TX_DIFF_DAC_VREF_R(n)           BITFIELD_VAL(REG_TX_DIFF_DAC_VREF_R, n)
#define REG_TX_DIFF_EAR_CASN_L_SHIFT        8
#define REG_TX_DIFF_EAR_CASN_L_MASK         (0x7 << REG_TX_DIFF_EAR_CASN_L_SHIFT)
#define REG_TX_DIFF_EAR_CASN_L(n)           BITFIELD_VAL(REG_TX_DIFF_EAR_CASN_L, n)
#define REG_TX_DIFF_EAR_CASN_R_SHIFT        11
#define REG_TX_DIFF_EAR_CASN_R_MASK         (0x7 << REG_TX_DIFF_EAR_CASN_R_SHIFT)
#define REG_TX_DIFF_EAR_CASN_R(n)           BITFIELD_VAL(REG_TX_DIFF_EAR_CASN_R, n)
#define RESERVED_DIG_68_67_SHIFT            14
#define RESERVED_DIG_68_67_MASK             (0x3 << RESERVED_DIG_68_67_SHIFT)
#define RESERVED_DIG_68_67(n)               BITFIELD_VAL(RESERVED_DIG_68_67, n)

// REG_27E
#define REG_TX_DIFF_EAR_CASP_L_SHIFT        0
#define REG_TX_DIFF_EAR_CASP_L_MASK         (0x7 << REG_TX_DIFF_EAR_CASP_L_SHIFT)
#define REG_TX_DIFF_EAR_CASP_L(n)           BITFIELD_VAL(REG_TX_DIFF_EAR_CASP_L, n)
#define REG_TX_DIFF_EAR_CASP_R_SHIFT        3
#define REG_TX_DIFF_EAR_CASP_R_MASK         (0x7 << REG_TX_DIFF_EAR_CASP_R_SHIFT)
#define REG_TX_DIFF_EAR_CASP_R(n)           BITFIELD_VAL(REG_TX_DIFF_EAR_CASP_R, n)
#define REG_TX_DIFF_DAC_VREF_SHIFT          6
#define REG_TX_DIFF_DAC_VREF_MASK           (0xF << REG_TX_DIFF_DAC_VREF_SHIFT)
#define REG_TX_DIFF_DAC_VREF(n)             BITFIELD_VAL(REG_TX_DIFF_DAC_VREF, n)
#define REG_TX_DIFF_VSEL_VCMO               (1 << 10)
#define REG_TX_DIFF_EAR_DIS_SHIFT           11
#define REG_TX_DIFF_EAR_DIS_MASK            (0x3 << REG_TX_DIFF_EAR_DIS_SHIFT)
#define REG_TX_DIFF_EAR_DIS(n)              BITFIELD_VAL(REG_TX_DIFF_EAR_DIS, n)
#define RESERVED_DIG_71_69_SHIFT            13
#define RESERVED_DIG_71_69_MASK             (0x7 << RESERVED_DIG_71_69_SHIFT)
#define RESERVED_DIG_71_69(n)               BITFIELD_VAL(RESERVED_DIG_71_69, n)

// REG_27F
#define REG_TX_DIFF_EAR_DR_ST_L_SHIFT       0
#define REG_TX_DIFF_EAR_DR_ST_L_MASK        (0x7 << REG_TX_DIFF_EAR_DR_ST_L_SHIFT)
#define REG_TX_DIFF_EAR_DR_ST_L(n)          BITFIELD_VAL(REG_TX_DIFF_EAR_DR_ST_L, n)
#define REG_TX_DIFF_EAR_DR_ST_R_SHIFT       3
#define REG_TX_DIFF_EAR_DR_ST_R_MASK        (0x7 << REG_TX_DIFF_EAR_DR_ST_R_SHIFT)
#define REG_TX_DIFF_EAR_DR_ST_R(n)          BITFIELD_VAL(REG_TX_DIFF_EAR_DR_ST_R, n)
#define REG_TX_DIFF_EAR_FBCAP_SHIFT         6
#define REG_TX_DIFF_EAR_FBCAP_MASK          (0x3 << REG_TX_DIFF_EAR_FBCAP_SHIFT)
#define REG_TX_DIFF_EAR_FBCAP(n)            BITFIELD_VAL(REG_TX_DIFF_EAR_FBCAP, n)
#define REG_TX_DIFF_EAR_IBSEL_SHIFT         8
#define REG_TX_DIFF_EAR_IBSEL_MASK          (0x3 << REG_TX_DIFF_EAR_IBSEL_SHIFT)
#define REG_TX_DIFF_EAR_IBSEL(n)            BITFIELD_VAL(REG_TX_DIFF_EAR_IBSEL, n)
#define REG_TX_DIFF_EAR_LCAL                (1 << 10)
#define REG_TX_DIFF_EAR_RCAL                (1 << 11)

// REG_280
#define REG_TX_DIFF_EAR_OFF_BITL_SHIFT      0
#define REG_TX_DIFF_EAR_OFF_BITL_MASK       (0x3FFF << REG_TX_DIFF_EAR_OFF_BITL_SHIFT)
#define REG_TX_DIFF_EAR_OFF_BITL(n)         BITFIELD_VAL(REG_TX_DIFF_EAR_OFF_BITL, n)
#define RESERVED_DIG_73_72_SHIFT            14
#define RESERVED_DIG_73_72_MASK             (0x3 << RESERVED_DIG_73_72_SHIFT)
#define RESERVED_DIG_73_72(n)               BITFIELD_VAL(RESERVED_DIG_73_72, n)

// REG_281
#define REG_TX_DIFF_EAR_OFF_BITR_SHIFT      0
#define REG_TX_DIFF_EAR_OFF_BITR_MASK       (0x3FFF << REG_TX_DIFF_EAR_OFF_BITR_SHIFT)
#define REG_TX_DIFF_EAR_OFF_BITR(n)         BITFIELD_VAL(REG_TX_DIFF_EAR_OFF_BITR, n)
#define RESERVED_DIG_75_74_SHIFT            14
#define RESERVED_DIG_75_74_MASK             (0x3 << RESERVED_DIG_75_74_SHIFT)
#define RESERVED_DIG_75_74(n)               BITFIELD_VAL(RESERVED_DIG_75_74, n)

// REG_282
#define REG_TX_DIFF_EAR_OFFEN               (1 << 0)
#define REG_TX_DIFF_EAR_OUTPUTSEL_SHIFT     1
#define REG_TX_DIFF_EAR_OUTPUTSEL_MASK      (0xF << REG_TX_DIFF_EAR_OUTPUTSEL_SHIFT)
#define REG_TX_DIFF_EAR_OUTPUTSEL(n)        BITFIELD_VAL(REG_TX_DIFF_EAR_OUTPUTSEL, n)
#define REG_TX_DIFF_EAR_SOFTSTART_SHIFT     5
#define REG_TX_DIFF_EAR_SOFTSTART_MASK      (0x3F << REG_TX_DIFF_EAR_SOFTSTART_SHIFT)
#define REG_TX_DIFF_EAR_SOFTSTART(n)        BITFIELD_VAL(REG_TX_DIFF_EAR_SOFTSTART, n)
#define REG_TX_DIFF_EAR_VCM_BIT_SHIFT       11
#define REG_TX_DIFF_EAR_VCM_BIT_MASK        (0x3 << REG_TX_DIFF_EAR_VCM_BIT_SHIFT)
#define REG_TX_DIFF_EAR_VCM_BIT(n)          BITFIELD_VAL(REG_TX_DIFF_EAR_VCM_BIT, n)
#define REG_TX_DIFF_EAR_VCM_SEL             (1 << 13)
#define REG_TX_DIFF_EN_CM_COMP              (1 << 14)
#define RESERVED_DIG_76                     (1 << 15)

// REG_283
#define REG_TX_DIFF_TX_REGULATOR_BIT_SHIFT  0
#define REG_TX_DIFF_TX_REGULATOR_BIT_MASK   (0xF << REG_TX_DIFF_TX_REGULATOR_BIT_SHIFT)
#define REG_TX_DIFF_TX_REGULATOR_BIT(n)     BITFIELD_VAL(REG_TX_DIFF_TX_REGULATOR_BIT, n)
#define REG_TX_DIFF_IBSEL_INPUT_SHIFT       4
#define REG_TX_DIFF_IBSEL_INPUT_MASK        (0x3 << REG_TX_DIFF_IBSEL_INPUT_SHIFT)
#define REG_TX_DIFF_IBSEL_INPUT(n)          BITFIELD_VAL(REG_TX_DIFF_IBSEL_INPUT, n)
#define REG_TX_EAR_IBSEL_INPUT_SHIFT        6
#define REG_TX_EAR_IBSEL_INPUT_MASK         (0x3 << REG_TX_EAR_IBSEL_INPUT_SHIFT)
#define REG_TX_EAR_IBSEL_INPUT(n)           BITFIELD_VAL(REG_TX_EAR_IBSEL_INPUT, n)
#define REG_TX_VREFN_VSEL_SHIFT             8
#define REG_TX_VREFN_VSEL_MASK              (0x7 << REG_TX_VREFN_VSEL_SHIFT)
#define REG_TX_VREFN_VSEL(n)                BITFIELD_VAL(REG_TX_VREFN_VSEL, n)
#define REG_TX_VREFP_VSEL_SHIFT             11
#define REG_TX_VREFP_VSEL_MASK              (0x7 << REG_TX_VREFP_VSEL_SHIFT)
#define REG_TX_VREFP_VSEL(n)                BITFIELD_VAL(REG_TX_VREFP_VSEL, n)
#define RESERVED_DIG_78_77_SHIFT            14
#define RESERVED_DIG_78_77_MASK             (0x3 << RESERVED_DIG_78_77_SHIFT)
#define RESERVED_DIG_78_77(n)               BITFIELD_VAL(RESERVED_DIG_78_77, n)

// REG_284
#define REG_ADCA_OFFSET_CAL_S_SHIFT         0
#define REG_ADCA_OFFSET_CAL_S_MASK          (0x1F << REG_ADCA_OFFSET_CAL_S_SHIFT)
#define REG_ADCA_OFFSET_CAL_S(n)            BITFIELD_VAL(REG_ADCA_OFFSET_CAL_S, n)
#define REG_ADCB_OFFSET_CAL_S_SHIFT         5
#define REG_ADCB_OFFSET_CAL_S_MASK          (0x1F << REG_ADCB_OFFSET_CAL_S_SHIFT)
#define REG_ADCB_OFFSET_CAL_S(n)            BITFIELD_VAL(REG_ADCB_OFFSET_CAL_S, n)
#define REG_ADCC_OFFSET_CAL_S_SHIFT         10
#define REG_ADCC_OFFSET_CAL_S_MASK          (0x1F << REG_ADCC_OFFSET_CAL_S_SHIFT)
#define REG_ADCC_OFFSET_CAL_S(n)            BITFIELD_VAL(REG_ADCC_OFFSET_CAL_S, n)
#define RESERVED_DIG_79                     (1 << 15)

// REG_285
#define REG_ADCD_OFFSET_CAL_S_SHIFT         0
#define REG_ADCD_OFFSET_CAL_S_MASK          (0x1F << REG_ADCD_OFFSET_CAL_S_SHIFT)
#define REG_ADCD_OFFSET_CAL_S(n)            BITFIELD_VAL(REG_ADCD_OFFSET_CAL_S, n)
#define REG_ADCE_OFFSET_CAL_S_SHIFT         5
#define REG_ADCE_OFFSET_CAL_S_MASK          (0x1F << REG_ADCE_OFFSET_CAL_S_SHIFT)
#define REG_ADCE_OFFSET_CAL_S(n)            BITFIELD_VAL(REG_ADCE_OFFSET_CAL_S, n)
#define REG_RX_PGAC_ZERO_DET_EN             (1 << 10)
#define REG_RX_PGAD_ZERO_DET_EN             (1 << 11)
#define REG_RX_PGAE_ZERO_DET_EN             (1 << 12)
#define RESERVED_DIG_82_80_SHIFT            13
#define RESERVED_DIG_82_80_MASK             (0x7 << RESERVED_DIG_82_80_SHIFT)
#define RESERVED_DIG_82_80(n)               BITFIELD_VAL(RESERVED_DIG_82_80, n)

// REG_286
#define REG_RX_PGAC_DRE_DR                  (1 << 0)
#define REG_RX_PGAD_DRE_DR                  (1 << 1)
#define REG_RX_PGAE_DRE_DR                  (1 << 2)
#define REG_RX_PGAC_DRE_SHIFT               3
#define REG_RX_PGAC_DRE_MASK                (0xF << REG_RX_PGAC_DRE_SHIFT)
#define REG_RX_PGAC_DRE(n)                  BITFIELD_VAL(REG_RX_PGAC_DRE, n)
#define REG_RX_PGAD_DRE_SHIFT               7
#define REG_RX_PGAD_DRE_MASK                (0xF << REG_RX_PGAD_DRE_SHIFT)
#define REG_RX_PGAD_DRE(n)                  BITFIELD_VAL(REG_RX_PGAD_DRE, n)
#define REG_RX_PGAE_DRE_SHIFT               11
#define REG_RX_PGAE_DRE_MASK                (0xF << REG_RX_PGAE_DRE_SHIFT)
#define REG_RX_PGAE_DRE(n)                  BITFIELD_VAL(REG_RX_PGAE_DRE, n)
#define RESERVED_DIG_83                     (1 << 15)

// REG_287
#define REG_CODEC_ADCC_GAIN_BIT_SHIFT       0
#define REG_CODEC_ADCC_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCC_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCC_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_GAIN_BIT, n)
#define REG_CODEC_ADCD_GAIN_BIT_SHIFT       3
#define REG_CODEC_ADCD_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCD_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCD_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCD_GAIN_BIT, n)
#define REG_CODEC_ADCE_GAIN_BIT_SHIFT       6
#define REG_CODEC_ADCE_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCE_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCE_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCE_GAIN_BIT, n)
#define REG_CODEC_ADCC_RES_2P5K_UPDATE      (1 << 9)
#define REG_CODEC_ADCD_RES_2P5K_UPDATE      (1 << 10)
#define REG_CODEC_ADCE_RES_2P5K_UPDATE      (1 << 11)
#define REG_CODEC_ADCC_RES_2P5K             (1 << 12)
#define REG_CODEC_ADCD_RES_2P5K             (1 << 13)
#define REG_CODEC_ADCE_RES_2P5K             (1 << 14)
#define RESERVED_DIG_84                     (1 << 15)

// REG_288
#define CFG_RESET_ADC_C_DR                  (1 << 0)
#define CFG_RESET_ADC_D_DR                  (1 << 1)
#define CFG_RESET_ADC_E_DR                  (1 << 2)
#define CFG_RESET_ADC_C                     (1 << 3)
#define CFG_RESET_ADC_D                     (1 << 4)
#define CFG_RESET_ADC_E                     (1 << 5)
#define REG_CODEC_RX_EN_ADCC                (1 << 6)
#define REG_CODEC_RX_EN_ADCD                (1 << 7)
#define REG_CODEC_RX_EN_ADCE                (1 << 8)
#define REG_CODEC_EN_ADCC                   (1 << 9)
#define REG_CODEC_EN_ADCD                   (1 << 10)
#define REG_CODEC_EN_ADCE                   (1 << 11)
#define REG_ADC_VREFBUF_CAS_SHIFT           12
#define REG_ADC_VREFBUF_CAS_MASK            (0x3 << REG_ADC_VREFBUF_CAS_SHIFT)
#define REG_ADC_VREFBUF_CAS(n)              BITFIELD_VAL(REG_ADC_VREFBUF_CAS, n)
#define RESERVED_DIG_86_85_SHIFT            14
#define RESERVED_DIG_86_85_MASK             (0x3 << RESERVED_DIG_86_85_SHIFT)
#define RESERVED_DIG_86_85(n)               BITFIELD_VAL(RESERVED_DIG_86_85, n)

// REG_289
#define REG_CODEC_ADCC_GAIN_UPDATE          (1 << 0)
#define REG_CODEC_ADCD_GAIN_UPDATE          (1 << 1)
#define REG_CODEC_ADCE_GAIN_UPDATE          (1 << 2)
#define RESERVED_DIG_99_87_SHIFT            3
#define RESERVED_DIG_99_87_MASK             (0x1FFF << RESERVED_DIG_99_87_SHIFT)
#define RESERVED_DIG_99_87(n)               BITFIELD_VAL(RESERVED_DIG_99_87, n)

// REG_28A
#define REG_ADC_IBSEL_IDAC2_SHIFT           0
#define REG_ADC_IBSEL_IDAC2_MASK            (0xF << REG_ADC_IBSEL_IDAC2_SHIFT)
#define REG_ADC_IBSEL_IDAC2(n)              BITFIELD_VAL(REG_ADC_IBSEL_IDAC2, n)
#define REG_ADC_IBSEL_VREFBUF_SHIFT         4
#define REG_ADC_IBSEL_VREFBUF_MASK          (0xF << REG_ADC_IBSEL_VREFBUF_SHIFT)
#define REG_ADC_IBSEL_VREFBUF(n)            BITFIELD_VAL(REG_ADC_IBSEL_VREFBUF, n)
#define REG_ADC_LP_VREFBUF_SHIFT            8
#define REG_ADC_LP_VREFBUF_MASK             (0x7 << REG_ADC_LP_VREFBUF_SHIFT)
#define REG_ADC_LP_VREFBUF(n)               BITFIELD_VAL(REG_ADC_LP_VREFBUF, n)
#define REG_ADC_VREFBUF_BIT_SHIFT           11
#define REG_ADC_VREFBUF_BIT_MASK            (0xF << REG_ADC_VREFBUF_BIT_SHIFT)
#define REG_ADC_VREFBUF_BIT(n)              BITFIELD_VAL(REG_ADC_VREFBUF_BIT, n)
#define RESERVED_DIG_100                    (1 << 15)

// REG_28B
#define REG_TX_DIFF_EAR_LOWGAINL_SHIFT      0
#define REG_TX_DIFF_EAR_LOWGAINL_MASK       (0xF << REG_TX_DIFF_EAR_LOWGAINL_SHIFT)
#define REG_TX_DIFF_EAR_LOWGAINL(n)         BITFIELD_VAL(REG_TX_DIFF_EAR_LOWGAINL, n)
#define REG_TX_DIFF_EAR_LOWGAINR_SHIFT      4
#define REG_TX_DIFF_EAR_LOWGAINR_MASK       (0xF << REG_TX_DIFF_EAR_LOWGAINR_SHIFT)
#define REG_TX_DIFF_EAR_LOWGAINR(n)         BITFIELD_VAL(REG_TX_DIFF_EAR_LOWGAINR, n)
#define REG_TX_DIFF_COMP_1STAGE_SHIFT       8
#define REG_TX_DIFF_COMP_1STAGE_MASK        (0xFF << REG_TX_DIFF_COMP_1STAGE_SHIFT)
#define REG_TX_DIFF_COMP_1STAGE(n)          BITFIELD_VAL(REG_TX_DIFF_COMP_1STAGE, n)

/* ---- PAGE2 END ---- */


/* ---- PAGE3 START ---- */

// REG_360
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)

// REG_361
#define SAR_BIT00_WEIGHT_SHIFT              0
#define SAR_BIT00_WEIGHT_MASK               (0x1FFF << SAR_BIT00_WEIGHT_SHIFT)
#define SAR_BIT00_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT00_WEIGHT, n)
#define REG_SAR_ADC_ON                      (1 << 13)
#define REG_SAR_RESULT_SEL                  (1 << 14)
#define RESERVED_ANA_51                     (1 << 15)

// REG_362
#define SAR_BIT01_WEIGHT_SHIFT              0
#define SAR_BIT01_WEIGHT_MASK               (0x1FFF << SAR_BIT01_WEIGHT_SHIFT)
#define SAR_BIT01_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT01_WEIGHT, n)
#define RESERVED_ANA_55_52_SHIFT            13
#define RESERVED_ANA_55_52_MASK             (0x7 << RESERVED_ANA_55_52_SHIFT)
#define RESERVED_ANA_55_52(n)               BITFIELD_VAL(RESERVED_ANA_55_52, n)

// REG_363
#define SAR_BIT02_WEIGHT_SHIFT              0
#define SAR_BIT02_WEIGHT_MASK               (0x1FFF << SAR_BIT02_WEIGHT_SHIFT)
#define SAR_BIT02_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT02_WEIGHT, n)
#define RESERVED_ANA_58_56_SHIFT            13
#define RESERVED_ANA_58_56_MASK             (0x7 << RESERVED_ANA_58_56_SHIFT)
#define RESERVED_ANA_58_56(n)               BITFIELD_VAL(RESERVED_ANA_58_56, n)

// REG_364
#define SAR_BIT03_WEIGHT_SHIFT              0
#define SAR_BIT03_WEIGHT_MASK               (0x1FFF << SAR_BIT03_WEIGHT_SHIFT)
#define SAR_BIT03_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT03_WEIGHT, n)
#define RESERVED_ANA_61_59_SHIFT            13
#define RESERVED_ANA_61_59_MASK             (0x7 << RESERVED_ANA_61_59_SHIFT)
#define RESERVED_ANA_61_59(n)               BITFIELD_VAL(RESERVED_ANA_61_59, n)

// REG_365
#define SAR_BIT04_WEIGHT_SHIFT              0
#define SAR_BIT04_WEIGHT_MASK               (0x1FFF << SAR_BIT04_WEIGHT_SHIFT)
#define SAR_BIT04_WEIGHT(n)                 BITFIELD_VAL(SAR_BIT04_WEIGHT, n)
#define RESERVED_ANA_64_62_SHIFT            13
#define RESERVED_ANA_64_62_MASK             (0x7 << RESERVED_ANA_64_62_SHIFT)
#define RESERVED_ANA_64_62(n)               BITFIELD_VAL(RESERVED_ANA_64_62, n)

// REG_366
#define SAR_P_BIT05_WEIGHT_SHIFT            0
#define SAR_P_BIT05_WEIGHT_MASK             (0x1FFF << SAR_P_BIT05_WEIGHT_SHIFT)
#define SAR_P_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT05_WEIGHT, n)
#define RESERVED_ANA_67_65_SHIFT            13
#define RESERVED_ANA_67_65_MASK             (0x7 << RESERVED_ANA_67_65_SHIFT)
#define RESERVED_ANA_67_65(n)               BITFIELD_VAL(RESERVED_ANA_67_65, n)

// REG_367
#define SAR_P_BIT06_WEIGHT_SHIFT            0
#define SAR_P_BIT06_WEIGHT_MASK             (0x1FFF << SAR_P_BIT06_WEIGHT_SHIFT)
#define SAR_P_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT06_WEIGHT, n)
#define RESERVED_ANA_70_68_SHIFT            13
#define RESERVED_ANA_70_68_MASK             (0x7 << RESERVED_ANA_70_68_SHIFT)
#define RESERVED_ANA_70_68(n)               BITFIELD_VAL(RESERVED_ANA_70_68, n)

// REG_368
#define SAR_P_BIT07_WEIGHT_SHIFT            0
#define SAR_P_BIT07_WEIGHT_MASK             (0x1FFF << SAR_P_BIT07_WEIGHT_SHIFT)
#define SAR_P_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT07_WEIGHT, n)
#define RESERVED_ANA_73_71_SHIFT            13
#define RESERVED_ANA_73_71_MASK             (0x7 << RESERVED_ANA_73_71_SHIFT)
#define RESERVED_ANA_73_71(n)               BITFIELD_VAL(RESERVED_ANA_73_71, n)

// REG_369
#define SAR_P_BIT08_WEIGHT_SHIFT            0
#define SAR_P_BIT08_WEIGHT_MASK             (0x1FFF << SAR_P_BIT08_WEIGHT_SHIFT)
#define SAR_P_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT08_WEIGHT, n)
#define RESERVED_ANA_76_74_SHIFT            13
#define RESERVED_ANA_76_74_MASK             (0x7 << RESERVED_ANA_76_74_SHIFT)
#define RESERVED_ANA_76_74(n)               BITFIELD_VAL(RESERVED_ANA_76_74, n)

// REG_36A
#define SAR_P_BIT09_WEIGHT_SHIFT            0
#define SAR_P_BIT09_WEIGHT_MASK             (0x1FFF << SAR_P_BIT09_WEIGHT_SHIFT)
#define SAR_P_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT09_WEIGHT, n)
#define RESERVED_ANA_79_77_SHIFT            13
#define RESERVED_ANA_79_77_MASK             (0x7 << RESERVED_ANA_79_77_SHIFT)
#define RESERVED_ANA_79_77(n)               BITFIELD_VAL(RESERVED_ANA_79_77, n)

// REG_36B
#define SAR_P_BIT10_WEIGHT_SHIFT            0
#define SAR_P_BIT10_WEIGHT_MASK             (0x1FFF << SAR_P_BIT10_WEIGHT_SHIFT)
#define SAR_P_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT10_WEIGHT, n)
#define RESERVED_ANA_82_80_SHIFT            13
#define RESERVED_ANA_82_80_MASK             (0x7 << RESERVED_ANA_82_80_SHIFT)
#define RESERVED_ANA_82_80(n)               BITFIELD_VAL(RESERVED_ANA_82_80, n)

// REG_36C
#define SAR_P_BIT11_WEIGHT_SHIFT            0
#define SAR_P_BIT11_WEIGHT_MASK             (0x1FFF << SAR_P_BIT11_WEIGHT_SHIFT)
#define SAR_P_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT11_WEIGHT, n)
#define RESERVED_ANA_85_83_SHIFT            13
#define RESERVED_ANA_85_83_MASK             (0x7 << RESERVED_ANA_85_83_SHIFT)
#define RESERVED_ANA_85_83(n)               BITFIELD_VAL(RESERVED_ANA_85_83, n)

// REG_36D
#define SAR_P_BIT12_WEIGHT_SHIFT            0
#define SAR_P_BIT12_WEIGHT_MASK             (0x1FFF << SAR_P_BIT12_WEIGHT_SHIFT)
#define SAR_P_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT12_WEIGHT, n)
#define RESERVED_ANA_88_86_SHIFT            13
#define RESERVED_ANA_88_86_MASK             (0x7 << RESERVED_ANA_88_86_SHIFT)
#define RESERVED_ANA_88_86(n)               BITFIELD_VAL(RESERVED_ANA_88_86, n)

// REG_36E
#define SAR_P_BIT13_WEIGHT_SHIFT            0
#define SAR_P_BIT13_WEIGHT_MASK             (0x1FFF << SAR_P_BIT13_WEIGHT_SHIFT)
#define SAR_P_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT13_WEIGHT, n)
#define RESERVED_ANA_91_89_SHIFT            13
#define RESERVED_ANA_91_89_MASK             (0x7 << RESERVED_ANA_91_89_SHIFT)
#define RESERVED_ANA_91_89(n)               BITFIELD_VAL(RESERVED_ANA_91_89, n)

// REG_36F
#define SAR_P_BIT14_WEIGHT_SHIFT            0
#define SAR_P_BIT14_WEIGHT_MASK             (0x1FFF << SAR_P_BIT14_WEIGHT_SHIFT)
#define SAR_P_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT14_WEIGHT, n)
#define RESERVED_ANA_94_92_SHIFT            13
#define RESERVED_ANA_94_92_MASK             (0x7 << RESERVED_ANA_94_92_SHIFT)
#define RESERVED_ANA_94_92(n)               BITFIELD_VAL(RESERVED_ANA_94_92, n)

// REG_370
#define SAR_P_BIT15_WEIGHT_SHIFT            0
#define SAR_P_BIT15_WEIGHT_MASK             (0x1FFF << SAR_P_BIT15_WEIGHT_SHIFT)
#define SAR_P_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT15_WEIGHT, n)
#define RESERVED_ANA_97_95_SHIFT            13
#define RESERVED_ANA_97_95_MASK             (0x7 << RESERVED_ANA_97_95_SHIFT)
#define RESERVED_ANA_97_95(n)               BITFIELD_VAL(RESERVED_ANA_97_95, n)

// REG_371
#define SAR_P_BIT16_WEIGHT_SHIFT            0
#define SAR_P_BIT16_WEIGHT_MASK             (0x1FFF << SAR_P_BIT16_WEIGHT_SHIFT)
#define SAR_P_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_P_BIT16_WEIGHT, n)
#define RESERVED_ANA_100_98_SHIFT           13
#define RESERVED_ANA_100_98_MASK            (0x7 << RESERVED_ANA_100_98_SHIFT)
#define RESERVED_ANA_100_98(n)              BITFIELD_VAL(RESERVED_ANA_100_98, n)

// REG_372
#define SAR_N_BIT05_WEIGHT_SHIFT            0
#define SAR_N_BIT05_WEIGHT_MASK             (0x1FFF << SAR_N_BIT05_WEIGHT_SHIFT)
#define SAR_N_BIT05_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT05_WEIGHT, n)
#define RESERVED_ANA_103_101_SHIFT          13
#define RESERVED_ANA_103_101_MASK           (0x7 << RESERVED_ANA_103_101_SHIFT)
#define RESERVED_ANA_103_101(n)             BITFIELD_VAL(RESERVED_ANA_103_101, n)

// REG_373
#define SAR_N_BIT06_WEIGHT_SHIFT            0
#define SAR_N_BIT06_WEIGHT_MASK             (0x1FFF << SAR_N_BIT06_WEIGHT_SHIFT)
#define SAR_N_BIT06_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT06_WEIGHT, n)
#define RESERVED_ANA_106_104_SHIFT          13
#define RESERVED_ANA_106_104_MASK           (0x7 << RESERVED_ANA_106_104_SHIFT)
#define RESERVED_ANA_106_104(n)             BITFIELD_VAL(RESERVED_ANA_106_104, n)

// REG_374
#define SAR_N_BIT07_WEIGHT_SHIFT            0
#define SAR_N_BIT07_WEIGHT_MASK             (0x1FFF << SAR_N_BIT07_WEIGHT_SHIFT)
#define SAR_N_BIT07_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT07_WEIGHT, n)
#define RESERVED_ANA_109_107_SHIFT          13
#define RESERVED_ANA_109_107_MASK           (0x7 << RESERVED_ANA_109_107_SHIFT)
#define RESERVED_ANA_109_107(n)             BITFIELD_VAL(RESERVED_ANA_109_107, n)

// REG_375
#define SAR_N_BIT08_WEIGHT_SHIFT            0
#define SAR_N_BIT08_WEIGHT_MASK             (0x1FFF << SAR_N_BIT08_WEIGHT_SHIFT)
#define SAR_N_BIT08_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT08_WEIGHT, n)
#define RESERVED_ANA_112_110_SHIFT          13
#define RESERVED_ANA_112_110_MASK           (0x7 << RESERVED_ANA_112_110_SHIFT)
#define RESERVED_ANA_112_110(n)             BITFIELD_VAL(RESERVED_ANA_112_110, n)

// REG_376
#define SAR_N_BIT09_WEIGHT_SHIFT            0
#define SAR_N_BIT09_WEIGHT_MASK             (0x1FFF << SAR_N_BIT09_WEIGHT_SHIFT)
#define SAR_N_BIT09_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT09_WEIGHT, n)
#define RESERVED_ANA_115_113_SHIFT          13
#define RESERVED_ANA_115_113_MASK           (0x7 << RESERVED_ANA_115_113_SHIFT)
#define RESERVED_ANA_115_113(n)             BITFIELD_VAL(RESERVED_ANA_115_113, n)

// REG_377
#define SAR_N_BIT10_WEIGHT_SHIFT            0
#define SAR_N_BIT10_WEIGHT_MASK             (0x1FFF << SAR_N_BIT10_WEIGHT_SHIFT)
#define SAR_N_BIT10_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT10_WEIGHT, n)
#define RESERVED_ANA_118_116_SHIFT          13
#define RESERVED_ANA_118_116_MASK           (0x7 << RESERVED_ANA_118_116_SHIFT)
#define RESERVED_ANA_118_116(n)             BITFIELD_VAL(RESERVED_ANA_118_116, n)

// REG_378
#define SAR_N_BIT11_WEIGHT_SHIFT            0
#define SAR_N_BIT11_WEIGHT_MASK             (0x1FFF << SAR_N_BIT11_WEIGHT_SHIFT)
#define SAR_N_BIT11_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT11_WEIGHT, n)
#define RESERVED_ANA_121_119_SHIFT          13
#define RESERVED_ANA_121_119_MASK           (0x7 << RESERVED_ANA_121_119_SHIFT)
#define RESERVED_ANA_121_119(n)             BITFIELD_VAL(RESERVED_ANA_121_119, n)

// REG_379
#define SAR_N_BIT12_WEIGHT_SHIFT            0
#define SAR_N_BIT12_WEIGHT_MASK             (0x1FFF << SAR_N_BIT12_WEIGHT_SHIFT)
#define SAR_N_BIT12_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT12_WEIGHT, n)
#define RESERVED_ANA_124_122_SHIFT          13
#define RESERVED_ANA_124_122_MASK           (0x7 << RESERVED_ANA_124_122_SHIFT)
#define RESERVED_ANA_124_122(n)             BITFIELD_VAL(RESERVED_ANA_124_122, n)

// REG_37A
#define SAR_N_BIT13_WEIGHT_SHIFT            0
#define SAR_N_BIT13_WEIGHT_MASK             (0x1FFF << SAR_N_BIT13_WEIGHT_SHIFT)
#define SAR_N_BIT13_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT13_WEIGHT, n)
#define RESERVED_ANA_127_125_SHIFT          13
#define RESERVED_ANA_127_125_MASK           (0x7 << RESERVED_ANA_127_125_SHIFT)
#define RESERVED_ANA_127_125(n)             BITFIELD_VAL(RESERVED_ANA_127_125, n)

// REG_37B
#define SAR_N_BIT14_WEIGHT_SHIFT            0
#define SAR_N_BIT14_WEIGHT_MASK             (0x1FFF << SAR_N_BIT14_WEIGHT_SHIFT)
#define SAR_N_BIT14_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT14_WEIGHT, n)
#define RESERVED_ANA_130_128_SHIFT          13
#define RESERVED_ANA_130_128_MASK           (0x7 << RESERVED_ANA_130_128_SHIFT)
#define RESERVED_ANA_130_128(n)             BITFIELD_VAL(RESERVED_ANA_130_128, n)

// REG_37C
#define SAR_N_BIT15_WEIGHT_SHIFT            0
#define SAR_N_BIT15_WEIGHT_MASK             (0x1FFF << SAR_N_BIT15_WEIGHT_SHIFT)
#define SAR_N_BIT15_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT15_WEIGHT, n)
#define RESERVED_ANA_133_131_SHIFT          13
#define RESERVED_ANA_133_131_MASK           (0x7 << RESERVED_ANA_133_131_SHIFT)
#define RESERVED_ANA_133_131(n)             BITFIELD_VAL(RESERVED_ANA_133_131, n)

// REG_37D
#define SAR_N_BIT16_WEIGHT_SHIFT            0
#define SAR_N_BIT16_WEIGHT_MASK             (0x1FFF << SAR_N_BIT16_WEIGHT_SHIFT)
#define SAR_N_BIT16_WEIGHT(n)               BITFIELD_VAL(SAR_N_BIT16_WEIGHT, n)
#define RESERVED_ANA_136_134_SHIFT          13
#define RESERVED_ANA_136_134_MASK           (0x7 << RESERVED_ANA_136_134_SHIFT)
#define RESERVED_ANA_136_134(n)             BITFIELD_VAL(RESERVED_ANA_136_134, n)

// REG_37E
#define REG_CLK_SAR_MUX                     (1 << 0)

// REG_37F
#define REG_SAR_INIT_CALI_BIT_SHIFT         0
#define REG_SAR_INIT_CALI_BIT_MASK          (0x1F << REG_SAR_INIT_CALI_BIT_SHIFT)
#define REG_SAR_INIT_CALI_BIT(n)            BITFIELD_VAL(REG_SAR_INIT_CALI_BIT, n)
#define REG_SAR_CALI                        (1 << 5)
#define REG_SAR_CALI_CNT_SHIFT              6
#define REG_SAR_CALI_CNT_MASK               (0xF << REG_SAR_CALI_CNT_SHIFT)
#define REG_SAR_CALI_CNT(n)                 BITFIELD_VAL(REG_SAR_CALI_CNT, n)
#define REG_SAR_CALI_LSB_SCRN               (1 << 10)
#define REG_SAR_WEIGHT_DR                   (1 << 11)
#define REG_SAR_OFFSET_DR                   (1 << 12)
#define REG_SAR_ADC_OFFSET_DR               (1 << 13)
#define CLK_SARADC_CODEC_INV                (1 << 14)
#define RESERVED_ANA_137                    (1 << 15)

// REG_380
#define REG_SAR_OFFSET_P_SHIFT              0
#define REG_SAR_OFFSET_P_MASK               (0x1F << REG_SAR_OFFSET_P_SHIFT)
#define REG_SAR_OFFSET_P(n)                 BITFIELD_VAL(REG_SAR_OFFSET_P, n)
#define REG_SAR_OFFSET_N_SHIFT              5
#define REG_SAR_OFFSET_N_MASK               (0x1F << REG_SAR_OFFSET_N_SHIFT)
#define REG_SAR_OFFSET_N(n)                 BITFIELD_VAL(REG_SAR_OFFSET_N, n)
#define RESERVED_ANA_143_138_SHIFT          10
#define RESERVED_ANA_143_138_MASK           (0x3F << RESERVED_ANA_143_138_SHIFT)
#define RESERVED_ANA_143_138(n)             BITFIELD_VAL(RESERVED_ANA_143_138, n)

// REG_381
#define REG_SAR_ADC_OFFSET_SHIFT            0
#define REG_SAR_ADC_OFFSET_MASK             (0xFFFF << REG_SAR_ADC_OFFSET_SHIFT)
#define REG_SAR_ADC_OFFSET(n)               BITFIELD_VAL(REG_SAR_ADC_OFFSET, n)

// REG_382
#define REG_SAR_OFFSET_CALI                 (1 << 0)
#define REG_SAR_OFFSET_CALI_CNT_SHIFT       1
#define REG_SAR_OFFSET_CALI_CNT_MASK        (0x7 << REG_SAR_OFFSET_CALI_CNT_SHIFT)
#define REG_SAR_OFFSET_CALI_CNT(n)          BITFIELD_VAL(REG_SAR_OFFSET_CALI_CNT, n)
#define REG_SAR_CLK_OUT_SEL                 (1 << 4)
#define REG_SAR_CLK_INT_DIV_SHIFT           5
#define REG_SAR_CLK_INT_DIV_MASK            (0x7F << REG_SAR_CLK_INT_DIV_SHIFT)
#define REG_SAR_CLK_INT_DIV(n)              BITFIELD_VAL(REG_SAR_CLK_INT_DIV, n)
#define SAR_VOUT_CALIB_INV                  (1 << 12)
#define SAR_CONV_DONE_INV                   (1 << 13)
#define RESERVED_ANA_145_144_SHIFT          14
#define RESERVED_ANA_145_144_MASK           (0x3 << RESERVED_ANA_145_144_SHIFT)
#define RESERVED_ANA_145_144(n)             BITFIELD_VAL(RESERVED_ANA_145_144, n)

// REG_383
#define REG_SAR_CH_SEL_IN_SHIFT             0
#define REG_SAR_CH_SEL_IN_MASK              (0xFF << REG_SAR_CH_SEL_IN_SHIFT)
#define REG_SAR_CH_SEL_IN(n)                BITFIELD_VAL(REG_SAR_CH_SEL_IN, n)
#define REG_SAR_CLK_TRIM_SHIFT              8
#define REG_SAR_CLK_TRIM_MASK               (0x3 << REG_SAR_CLK_TRIM_SHIFT)
#define REG_SAR_CLK_TRIM(n)                 BITFIELD_VAL(REG_SAR_CLK_TRIM, n)
#define REG_SAR_CLK_MODE                    (1 << 10)
#define REG_SAR_DELAY_BIT_SHIFT             11
#define REG_SAR_DELAY_BIT_MASK              (0x7 << REG_SAR_DELAY_BIT_SHIFT)
#define REG_SAR_DELAY_BIT(n)                BITFIELD_VAL(REG_SAR_DELAY_BIT, n)
#define REG_SAR_EN_PREAMP                   (1 << 14)
#define RESERVED_ANA_146                    (1 << 15)

// REG_384
#define REG_SAR_PU_PIN_BIAS                 (1 << 0)
#define REG_SAR_PU_NIN_BIAS                 (1 << 1)
#define REG_SAR_SE_MODE                     (1 << 2)
#define REG_SAR_THERM_GAIN_IN_SHIFT         3
#define REG_SAR_THERM_GAIN_IN_MASK          (0x3 << REG_SAR_THERM_GAIN_IN_SHIFT)
#define REG_SAR_THERM_GAIN_IN(n)            BITFIELD_VAL(REG_SAR_THERM_GAIN_IN, n)
#define REG_SAR_VREG_IBIT_SHIFT             5
#define REG_SAR_VREG_IBIT_MASK              (0x3 << REG_SAR_VREG_IBIT_SHIFT)
#define REG_SAR_VREG_IBIT(n)                BITFIELD_VAL(REG_SAR_VREG_IBIT, n)
#define REG_SAR_VREG_OUTCAP_MODE            (1 << 7)
#define REG_SAR_VREG_SEL_SHIFT              8
#define REG_SAR_VREG_SEL_MASK               (0xF << REG_SAR_VREG_SEL_SHIFT)
#define REG_SAR_VREG_SEL(n)                 BITFIELD_VAL(REG_SAR_VREG_SEL, n)
#define RESERVED_ANA_150_147_SHIFT          12
#define RESERVED_ANA_150_147_MASK           (0xF << RESERVED_ANA_150_147_SHIFT)
#define RESERVED_ANA_150_147(n)             BITFIELD_VAL(RESERVED_ANA_150_147, n)

// REG_385
#define REG_SAR_CLK_OUT_DIV_SHIFT           0
#define REG_SAR_CLK_OUT_DIV_MASK            (0x1FFF << REG_SAR_CLK_OUT_DIV_SHIFT)
#define REG_SAR_CLK_OUT_DIV(n)              BITFIELD_VAL(REG_SAR_CLK_OUT_DIV, n)
#define RESERVED_ANA_153_151_SHIFT          13
#define RESERVED_ANA_153_151_MASK           (0x7 << RESERVED_ANA_153_151_SHIFT)
#define RESERVED_ANA_153_151(n)             BITFIELD_VAL(RESERVED_ANA_153_151, n)

// REG_386
#define REG_SAR_PU_VREF_DR                  (1 << 0)
#define REG_SAR_PU_VREF_REG                 (1 << 1)
#define REG_SAR_PU_VREF_IN                  (1 << 2)
#define REG_SAR_PU_DR                       (1 << 3)
#define REG_SAR_PU_REG                      (1 << 4)
#define REG_SAR_PU_PRECHARGE_DR             (1 << 5)
#define REG_SAR_PU_PRECHARGE_REG            (1 << 6)
#define REG_SAR_PU_RST_DR                   (1 << 7)
#define REG_SAR_PU_RST_REG                  (1 << 8)
#define REG_SAR_PU_START_DLY_CNT_SHIFT      9
#define REG_SAR_PU_START_DLY_CNT_MASK       (0x7F << REG_SAR_PU_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_START_DLY_CNT(n)         BITFIELD_VAL(REG_SAR_PU_START_DLY_CNT, n)

// REG_387
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT 0
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT_MASK (0x7F << REG_SAR_PU_PRECHARGE_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_START_DLY_CNT(n) BITFIELD_VAL(REG_SAR_PU_PRECHARGE_START_DLY_CNT, n)
#define REG_SAR_PU_RST_START_DLY_CNT_SHIFT  7
#define REG_SAR_PU_RST_START_DLY_CNT_MASK   (0x7F << REG_SAR_PU_RST_START_DLY_CNT_SHIFT)
#define REG_SAR_PU_RST_START_DLY_CNT(n)     BITFIELD_VAL(REG_SAR_PU_RST_START_DLY_CNT, n)
#define RESERVED_ANA_155_154_SHIFT          14
#define RESERVED_ANA_155_154_MASK           (0x3 << RESERVED_ANA_155_154_SHIFT)
#define RESERVED_ANA_155_154(n)             BITFIELD_VAL(RESERVED_ANA_155_154, n)

// REG_388
#define REG_SAR_PU_RST_LAST_CNT_SHIFT       0
#define REG_SAR_PU_RST_LAST_CNT_MASK        (0x7F << REG_SAR_PU_RST_LAST_CNT_SHIFT)
#define REG_SAR_PU_RST_LAST_CNT(n)          BITFIELD_VAL(REG_SAR_PU_RST_LAST_CNT, n)
#define REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT 7
#define REG_SAR_PU_PRECHARGE_LAST_CNT_MASK  (0x1FF << REG_SAR_PU_PRECHARGE_LAST_CNT_SHIFT)
#define REG_SAR_PU_PRECHARGE_LAST_CNT(n)    BITFIELD_VAL(REG_SAR_PU_PRECHARGE_LAST_CNT, n)

// REG_389
#define SAR_P_BIT05_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT05_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT05_WEIGHT_IN_SHIFT)
#define SAR_P_BIT05_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT05_WEIGHT_IN, n)

// REG_38A
#define SAR_P_BIT06_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT06_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT06_WEIGHT_IN_SHIFT)
#define SAR_P_BIT06_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT06_WEIGHT_IN, n)

// REG_38B
#define SAR_P_BIT07_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT07_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT07_WEIGHT_IN_SHIFT)
#define SAR_P_BIT07_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT07_WEIGHT_IN, n)

// REG_38C
#define SAR_P_BIT08_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT08_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT08_WEIGHT_IN_SHIFT)
#define SAR_P_BIT08_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT08_WEIGHT_IN, n)

// REG_38D
#define SAR_P_BIT09_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT09_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT09_WEIGHT_IN_SHIFT)
#define SAR_P_BIT09_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT09_WEIGHT_IN, n)

// REG_38E
#define SAR_P_BIT10_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT10_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT10_WEIGHT_IN_SHIFT)
#define SAR_P_BIT10_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT10_WEIGHT_IN, n)

// REG_38F
#define SAR_P_BIT11_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT11_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT11_WEIGHT_IN_SHIFT)
#define SAR_P_BIT11_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT11_WEIGHT_IN, n)

// REG_390
#define SAR_P_BIT12_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT12_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT12_WEIGHT_IN_SHIFT)
#define SAR_P_BIT12_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT12_WEIGHT_IN, n)

// REG_391
#define SAR_P_BIT13_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT13_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT13_WEIGHT_IN_SHIFT)
#define SAR_P_BIT13_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT13_WEIGHT_IN, n)

// REG_392
#define SAR_P_BIT14_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT14_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT14_WEIGHT_IN_SHIFT)
#define SAR_P_BIT14_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT14_WEIGHT_IN, n)

// REG_393
#define SAR_P_BIT15_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT15_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT15_WEIGHT_IN_SHIFT)
#define SAR_P_BIT15_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT15_WEIGHT_IN, n)

// REG_394
#define SAR_P_BIT16_WEIGHT_IN_SHIFT         0
#define SAR_P_BIT16_WEIGHT_IN_MASK          (0x1FFF << SAR_P_BIT16_WEIGHT_IN_SHIFT)
#define SAR_P_BIT16_WEIGHT_IN(n)            BITFIELD_VAL(SAR_P_BIT16_WEIGHT_IN, n)

// REG_395
#define SAR_N_BIT05_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT05_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT05_WEIGHT_IN_SHIFT)
#define SAR_N_BIT05_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT05_WEIGHT_IN, n)

// REG_396
#define SAR_N_BIT06_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT06_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT06_WEIGHT_IN_SHIFT)
#define SAR_N_BIT06_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT06_WEIGHT_IN, n)

// REG_397
#define SAR_N_BIT07_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT07_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT07_WEIGHT_IN_SHIFT)
#define SAR_N_BIT07_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT07_WEIGHT_IN, n)

// REG_398
#define SAR_N_BIT08_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT08_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT08_WEIGHT_IN_SHIFT)
#define SAR_N_BIT08_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT08_WEIGHT_IN, n)

// REG_399
#define SAR_N_BIT09_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT09_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT09_WEIGHT_IN_SHIFT)
#define SAR_N_BIT09_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT09_WEIGHT_IN, n)

// REG_39A
#define SAR_N_BIT10_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT10_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT10_WEIGHT_IN_SHIFT)
#define SAR_N_BIT10_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT10_WEIGHT_IN, n)

// REG_39B
#define SAR_N_BIT11_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT11_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT11_WEIGHT_IN_SHIFT)
#define SAR_N_BIT11_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT11_WEIGHT_IN, n)

// REG_39C
#define SAR_N_BIT12_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT12_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT12_WEIGHT_IN_SHIFT)
#define SAR_N_BIT12_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT12_WEIGHT_IN, n)

// REG_39D
#define SAR_N_BIT13_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT13_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT13_WEIGHT_IN_SHIFT)
#define SAR_N_BIT13_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT13_WEIGHT_IN, n)

// REG_39E
#define SAR_N_BIT14_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT14_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT14_WEIGHT_IN_SHIFT)
#define SAR_N_BIT14_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT14_WEIGHT_IN, n)

// REG_39F
#define SAR_N_BIT15_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT15_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT15_WEIGHT_IN_SHIFT)
#define SAR_N_BIT15_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT15_WEIGHT_IN, n)

// REG_3A0
#define SAR_N_BIT16_WEIGHT_IN_SHIFT         0
#define SAR_N_BIT16_WEIGHT_IN_MASK          (0x1FFF << SAR_N_BIT16_WEIGHT_IN_SHIFT)
#define SAR_N_BIT16_WEIGHT_IN(n)            BITFIELD_VAL(SAR_N_BIT16_WEIGHT_IN, n)

// REG_3A1
#define REG_SAR_OFFSET_P_IN_SHIFT           0
#define REG_SAR_OFFSET_P_IN_MASK            (0x1F << REG_SAR_OFFSET_P_IN_SHIFT)
#define REG_SAR_OFFSET_P_IN(n)              BITFIELD_VAL(REG_SAR_OFFSET_P_IN, n)
#define REG_SAR_OFFSET_N_IN_SHIFT           5
#define REG_SAR_OFFSET_N_IN_MASK            (0x1F << REG_SAR_OFFSET_N_IN_SHIFT)
#define REG_SAR_OFFSET_N_IN(n)              BITFIELD_VAL(REG_SAR_OFFSET_N_IN, n)

// REG_3A2
#define REG_SAR_ADC_OFFSET_IN_SHIFT         0
#define REG_SAR_ADC_OFFSET_IN_MASK          (0xFFFF << REG_SAR_ADC_OFFSET_IN_SHIFT)
#define REG_SAR_ADC_OFFSET_IN(n)            BITFIELD_VAL(REG_SAR_ADC_OFFSET_IN, n)

/* ---- PAGE3 END ---- */



enum ANA_REG_T {
    // PAGE-0
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
    ANA_REG_87,

    // PAGE-1
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

    // PAGE-2
    ANA_REG_260 = 0x260,
    ANA_REG_261,
    ANA_REG_262,
    ANA_REG_263,
    ANA_REG_264,
    ANA_REG_265,
    ANA_REG_266,
    ANA_REG_267,
    ANA_REG_268,
    ANA_REG_269,
    ANA_REG_26A,
    ANA_REG_26B,
    ANA_REG_26C,
    ANA_REG_26D,
    ANA_REG_26E,
    ANA_REG_26F,
    ANA_REG_270,
    ANA_REG_271,
    ANA_REG_272,
    ANA_REG_273,
    ANA_REG_274,
    ANA_REG_275,
    ANA_REG_276,
    ANA_REG_277,
    ANA_REG_278,
    ANA_REG_279,
    ANA_REG_27A,
    ANA_REG_27B,
    ANA_REG_27C,
    ANA_REG_27D,
    ANA_REG_27E,
    ANA_REG_27F,
    ANA_REG_280,
    ANA_REG_281,
    ANA_REG_282,
    ANA_REG_283,
    ANA_REG_284,
    ANA_REG_285,
    ANA_REG_286,
    ANA_REG_287,
    ANA_REG_288,
    ANA_REG_289,
    ANA_REG_28A,
    ANA_REG_28B,

    // PAGE-3
    ANA_REG_360 = 0x360,
    ANA_REG_361,
    ANA_REG_362,
    ANA_REG_363,
    ANA_REG_364,
    ANA_REG_365,
    ANA_REG_366,
    ANA_REG_367,
    ANA_REG_368,
    ANA_REG_369,
    ANA_REG_36A,
    ANA_REG_36B,
    ANA_REG_36C,
    ANA_REG_36D,
    ANA_REG_36E,
    ANA_REG_36F,
    ANA_REG_370,
    ANA_REG_371,
    ANA_REG_372,
    ANA_REG_373,
    ANA_REG_374,
    ANA_REG_375,
    ANA_REG_376,
    ANA_REG_377,
    ANA_REG_378,
    ANA_REG_379,
    ANA_REG_37A,
    ANA_REG_37B,
    ANA_REG_37C,
    ANA_REG_37D,
    ANA_REG_37E,
    ANA_REG_37F,
    ANA_REG_380,
    ANA_REG_381,
    ANA_REG_382,
    ANA_REG_383,
    ANA_REG_384,
    ANA_REG_385,
    ANA_REG_386,
    ANA_REG_387,
    ANA_REG_388,
    ANA_REG_389,
    ANA_REG_38A,
    ANA_REG_38B,
    ANA_REG_38C,
    ANA_REG_38D,
    ANA_REG_38E,
    ANA_REG_38F,
    ANA_REG_390,
    ANA_REG_391,
    ANA_REG_392,
    ANA_REG_393,
    ANA_REG_394,
    ANA_REG_395,
    ANA_REG_396,
    ANA_REG_397,
    ANA_REG_398,
    ANA_REG_399,
    ANA_REG_39A,
    ANA_REG_39B,
    ANA_REG_39C,
    ANA_REG_39D,
    ANA_REG_39E,
    ANA_REG_39F,
    ANA_REG_3A0,
    ANA_REG_3A1,
    ANA_REG_3A2,
};


#endif // _REG_ANALOG_BEST3003_H_
