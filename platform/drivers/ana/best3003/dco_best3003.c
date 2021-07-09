
#include <stdint.h>
#include "stdbool.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_location.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_chipid.h"
#include "analog.h"
#include "pmu.h"
#include "reg_dco_best3003.h"

#ifndef EFUSE_DCO_FINE_TUNE_CODE_REG
#define EFUSE_DCO_FINE_TUNE_CODE_REG    (0xE)
#endif

#ifndef DCO_CALIB_INTERVAL
#ifdef USB_HIGH_SPEED
#define DCO_CALIB_INTERVAL      (5<<3)
#else
#define DCO_CALIB_INTERVAL      (5)
#endif
#endif // DCO_CALIB_INTERVAL

#ifndef DCO_CALIB_CALIB_CLK_DIV
#define DCO_CALIB_CALIB_CLK_DIV     0x8
#endif // DCO_CALIB_CALIB_CLK_DIV

#ifdef USB_HIGH_SPEED
#define DCO_CALIB_REF_CLK_FREQ      (4000)    // 4kHz in HS mode
#else
#define DCO_CALIB_REF_CLK_FREQ      (500)    // 0.5kHz in FS mode
#endif

#define DCO_CALIB_MARK_CNT      (((960000000/DCO_CALIB_CALIB_CLK_DIV)/DCO_CALIB_REF_CLK_FREQ)*DCO_CALIB_INTERVAL)

#ifndef DCO_LDO_VD1P5_ISEL_SETTING
#define DCO_LDO_VD1P5_ISEL_SETTING       (0x7)
#endif  // DCO_LDO_VD1P5_ISEL_SETTING

#ifndef DCO_TST_BUF_RCTRL_SETTING
#define DCO_TST_BUF_RCTRL_SETTING       (0)
#endif  // DCO_TST_BUF_RCTRL_SETTING

#ifndef DCO_EXT_REF_CLK_FREQ
#define DCO_EXT_REF_CLK_FREQ    (13000000)   // 13MHz is the default external clock frequency
#endif  // DCO_EXT_REF_CLK_FREQ

#ifndef DCO_SDM_CNT_TIME_OUT
#define DCO_SDM_CNT_TIME_OUT    (0x3E8)
#endif // DCO_SDM_CNT_TIME_OUT

#ifndef DCO_SDM_SHRINK
#define DCO_SDM_SHRINK          (0)
#endif  // DCO_SDM_SHRINK

#ifndef DCO_SDM_TUN_INIT_CODE
#define DCO_SDM_TUN_INIT_CODE       (0x40000)
#endif // DCO_SDM_TUN_INIT_CODE

#ifndef DCO_SDM_TUN_DCW_IN_INIT_CODE
#define DCO_SDM_TUN_DCW_IN_INIT_CODE        (0x80000)
#endif // DCO_SDM_TUN_DCWI_INIT_CODE

#ifndef DCO_SDM_TUN_DCW_OUT_INIT_CODE
#define DCO_SDM_TUN_DCW_OUT_INIT_CODE       (0x808000)
#endif // DCO_SDM_TUN_DCWO_INIT_CODE

#ifndef DCO_FINE_TUNE_COEF
#define DCO_FINE_TUNE_COEF          (1)
#endif // DCO_FINE_TUNE_COEF

#ifdef CHIP_HAS_DCO

static uint8_t BOOT_DATA_LOC s_dco_enabled = 0xFF;

bool BOOT_TEXT_FLASH_LOC dco_enabled(void)
{
    if (s_dco_enabled == 0xFF) {
        uint16_t val;
        dco_read(DCO_REG_068, &val);
        s_dco_enabled = (uint8_t)((val&DIG_XTAL_DCXO_EN) == DIG_XTAL_DCXO_EN);
    }

    return (bool)s_dco_enabled;
}


void dco_open (void)
{
    if (!dco_enabled()) {
        return;
    }

    uint16_t val = 0;

    TRACE(1,"%s", __func__);

    //---------------------------------------------------
    //  Overall Setting
    //--------------------------------------------------

    // to set loop_div, then calibration clock = 3.84G/loop_div_n
    dco_read(DCO_REG_063, &val);
    val &= ~DCO_LOOP_DIV_RATIO_MASK;
    val |= DCO_LOOP_DIV_RATIO(DCO_CALIB_CALIB_CLK_DIV);
    dco_write(DCO_REG_063, val);

    // to set the current consumption of DCO core
    dco_read(DCO_REG_064, &val);
    val &= ~DCO_CB_TUN_MASK;
    val |= DCO_CB_TUN(0x8);
    dco_write(DCO_REG_064, val);
    // Note: RBP and RBN are obsoleted in 3003.

    // to set isel of DCO
    dco_read(DCO_REG_06A, &val);
    val &= ~DCO_LDO_VD1P5_ISEL_MASK;
    val |= DCO_LDO_VD1P5_ISEL(DCO_LDO_VD1P5_ISEL_SETTING);
    dco_write(DCO_REG_06A, val);

    // to set DIVN of external 13MHz reference clock source
    dco_write(DCO_REG_069, (DCO_EXT_REF_CLK_FREQ/DCO_CALIB_REF_CLK_FREQ)-2);

    // to set SOF clock as the reference of DCO tuning
    dco_set_tuning_reference_clk(DCO_TUNING_REF_SOF_CLK);

    //---------------------------------------------------
    //  COARSE TUNING Mode Initial Setting
    //--------------------------------------------------

    // !!COARSE TUNING initialization should have been done in ROM code!!

    // to disable COARSE TUNING
    dco_read(DCO_REG_161, &val);
    val |= REG_COARSE_EN_DR;
    val &= ~REG_COARSE_EN;
    dco_write(DCO_REG_161, val);

    //---------------------------------------------------
    //  FINE TUNING Mode Initial Setting
    //--------------------------------------------------

    // to disable FINE TUNING
    dco_read(DCO_REG_166, &val);
    val |= REG_FINE_EN_DR;
    val &= ~REG_FINE_EN;
    dco_write(DCO_REG_166, val);

    // to set DIVN of FINE TUNING
    dco_read(DCO_REG_167, &val);
    val &= ~REG_CLK_REF_FINE_DIV_NUM_MASK;
    val |= REG_FINE_POLAR_SEL;
    val |= REG_CLK_REF_FINE_DIV_NUM(DCO_CALIB_INTERVAL-1);
    dco_write(DCO_REG_167, val);

    // to set COUNT MARK of FINE TUNING
    dco_read(DCO_REG_169, &val);
    val &= (uint16_t)(~REG_CNT_FINE_MARK_15_0_MASK);
    val |= REG_CNT_FINE_MARK_15_0(DCO_CALIB_MARK_CNT-1);
    dco_write(DCO_REG_169, val);

    dco_read(DCO_REG_16A, &val);
    val &= ~REG_CNT_FINE_MARK_24_16_MASK;
    val |= REG_CNT_FINE_MARK_24_16((DCO_CALIB_MARK_CNT-1) >> 16);
    dco_write(DCO_REG_16A, val);

/*
    // to set initial FINE TUNING code
    pmu_get_efuse(EFUSE_DCO_FINE_TUNE_CODE_REG, &init_fine_code);
#ifdef CHIP_HAS_TEMP_SNSR
    {
        // TODO:
    }
#endif  // CHIP_HAS_TEMP_SNSR
#ifdef USR_DEF_INIT_DCO_FINE_TUNE_CODE
    init_fine_code = USR_DEF_INIT_DCO_FINE_TUNE_CODE;
#endif  // USR_DEF_INIT_DCO_FINE_TUNE_CODE
    dco_read(DCO_REG_166, &val);
    val &= ~REG_FINE_TUN_CODE_LOAD_VAL_MASK;
    val |= REG_FINE_TUN_CODE_LOAD_VAL(val) | REG_FINE_TUN_CODE_LOAD_EN;
    dco_write(DCO_REG_166, val);
*/
    // to disable SDM bypass and filter bypass for FINE TUNING
    dco_read(DCO_REG_16C, &val);
    val &= ~(REG_SDM_BYPASS | REG_FILTER_BYPASS);
    val &= ~REG_COEFF_SEL_MASK;
    val |= REG_COEFF_SEL(DCO_FINE_TUNE_COEF);
    dco_write(DCO_REG_16C, val);


    //---------------------------------------------------
    //  SDM TUNING Mode Initial Setting
    //--------------------------------------------------

    // to disable and reset SDM TUNING
    dco_read(DCO_REG_261, &val);
    val |= REG_SDM_OUT_EN_DR | REG_SDM_RSTB;
    val &= ~REG_SDM_OUT_EN;
    dco_write(DCO_REG_261, val);

    // to set DIVN of SDM TUNING
    dco_read(DCO_REG_263, &val);
    val |= RESERVED_DIG_71_69_MASK;
    val &= ~REG_CLK_REF_SDM_OUT_DIV_NUM_MASK;
    val |= REG_CLK_REF_SDM_OUT_DIV_NUM(DCO_CALIB_INTERVAL-1);
    dco_write(DCO_REG_263, val);

    // to set COUNT MARK of SDM TUNING
    dco_read(DCO_REG_265, &val);
    val &= (uint16_t)(~REG_CNT_SDM_OUT_MARK_15_0_MASK);
    val |= REG_CNT_SDM_OUT_MARK_15_0(DCO_CALIB_MARK_CNT-1);
    dco_write(DCO_REG_265, val);

    dco_read(DCO_REG_266, &val);
    val &= ~REG_CNT_SDM_OUT_MARK_24_16_MASK;
    val |= REG_CNT_SDM_OUT_MARK_15_0((DCO_CALIB_MARK_CNT-1) >> 16);
    dco_write(DCO_REG_266, val);

    // to set SDM TUNING polar and step
    dco_read(DCO_REG_263, &val);
    val &= ~REG_SDM_OUT_TUN_STEP_SEL_MASK;
    val |= REG_SDM_OUT_TUN_STEP_SEL(0x8) | REG_SDM_OUT_POLAR_SEL;
    dco_write(DCO_REG_263, val);

    // to set SDM TUNING timeout, and disable SDM TUNING shrink
    dco_read(DCO_REG_264, &val);
    val &= ~REG_SDM_OUT_CNT_TIME_OUT_OFF_MASK;
    val |= REG_SDM_OUT_CNT_TIME_OUT_OFF(DCO_SDM_CNT_TIME_OUT);
    if (DCO_SDM_SHRINK) {
        val |= REG_SDM_OUT_TUN_STEP_SHRINK;
    } else {
        val &= ~REG_SDM_OUT_TUN_STEP_SHRINK;
    }
    dco_write(DCO_REG_264, val);

    // to set initial SDM TUNING Code
    dco_write(DCO_REG_273, DCO_SDM_TUN_DCW_OUT_INIT_CODE&0xFFFF);
    dco_write(DCO_REG_272, (DCO_SDM_TUN_DCW_IN_INIT_CODE>>4)&0xFFFF);
    
    dco_read(DCO_REG_271, &val);
    val |= REG_LOAD_DWC_OUT | REG_LOAD_DWC_IN;
    val &= ~REG_LOAD_DWC_OUT_HI_VAL_MASK;
    val |= REG_LOAD_DWC_OUT_HI_VAL((DCO_SDM_TUN_DCW_OUT_INIT_CODE>>16)&0xFFFF);
    val &= ~REG_LOAD_DWC_IN_VAL_3_0_MASK;
    val |= REG_LOAD_DWC_IN_VAL_3_0(DCO_SDM_TUN_DCW_IN_INIT_CODE&0xFFFF);
    dco_write(DCO_REG_271, val);

    dco_read(DCO_REG_262, &val);
    val &= ~REG_SDM_OUT_TUN_CODE_19_12_MASK;
    val |= REG_SDM_OUT_TUN_CODE_19_12((DCO_SDM_TUN_INIT_CODE>>12)&0xFFFF);
    dco_write(DCO_REG_262, val);

    dco_read(DCO_REG_261, &val);
    val |= REG_SDM_OUT_TUN_CODE_LOAD_EN;
    val &= ~REG_SDM_OUT_TUN_CODE_11_0_MASK;
    val |= REG_SDM_OUT_TUN_CODE_11_0(DCO_SDM_TUN_INIT_CODE&0xFFFF);
    dco_write(DCO_REG_261, val);

    // to disable SDM bypass and filter bypass for SDM TUNING
    dco_read(DCO_REG_269, &val);
    val &= ~(REG_FILTER_BYPASS4SDM_OUT_CODE | REG_SDM_BYPASS4SDM_OUT_CODE);
    val |= CFG_BYPASS_SDM_OUT_DLY;
    dco_write(DCO_REG_269, val);

    // to make sure all settings have been applied
    dco_read(DCO_REG_068, &val);
    val |= DCXO_DIGITAL_RESET;
    dco_write(DCO_REG_068, val);
    osDelay(1);
    val &= ~DCXO_DIGITAL_RESET;
    dco_write(DCO_REG_068, val);

    //---------------------------------------------------
    //  power consumption saving setting
    //--------------------------------------------------
    dco_read(DCO_REG_061, &val);
    val &= ~DCO_XO_BUFFER_PWUP_MASK;
    val |= DCO_XO_BUFFER_PWUP(1);
    val &= ~DCO_TST_BUFFER_RCTRL_MASK;
    dco_write(DCO_REG_061, val);

    dco_read(DCO_REG_062, &val);
    val |= DCO_TST_BUFFER_RCTRL_DR;
    dco_write(DCO_REG_062, val);

    dco_read(DCO_REG_065, &val);
    val &= ~DCO_LDO_DIVN_RCTRL_MASK;
    val |= DCO_LDO_DIVN_RCTRL(1);
    val &= ~DCO_LDO_XO_BUFF_RCTRL_MASK;
    val |= DCO_LDO_XO_BUFF_RCTRL(1);
    val &= ~DCO_LDO_BUFFER_RCTRL_MASK;
    val |= DCO_LDO_BUFFER_RCTRL(1);
    dco_write(DCO_REG_065, val);

    dco_read(DCO_REG_067, &val);
    val &= ~DCO_LDO_VD1P2REF_SEL_MASK;
    val |= DCO_LDO_VD1P2REF_SEL(4);
    dco_write(DCO_REG_067, val);

    dco_read(DCO_REG_06A, &val);
    val &= ~DCO_LDO_VD1P5_ISEL_MASK;
    val |= DCO_LDO_VD1P5_ISEL(2);
    val &= ~DCO_LDO_PRES_RCTRL_MASK;
    val |= DCO_LDO_PRES_RCTRL(1);
    dco_write(DCO_REG_06A, val);

    //---------------------------------------------------
    //  to start FINE TUNING
    //--------------------------------------------------
    dco_set_tuning_mode(DCO_TUNING_MODE_FINE);
    
}


void dco_set_tuning_mode (uint8_t tuning_mode)
{
    static uint8_t s_dco_current_tuning_mode = 0xFF;
    uint16_t val = 0;

    TRACE(3,"%s: tuning_mode [%d -> %d]", __func__, s_dco_current_tuning_mode, tuning_mode);

    if (tuning_mode > DCO_TUNING_MODE_SDM) {
        return;
    }
    
    if (s_dco_current_tuning_mode == tuning_mode) {
        return;
    }

    if (s_dco_current_tuning_mode == DCO_TUNING_MODE_COARSE) {
        dco_read(DCO_REG_161, &val);
        val |= REG_COARSE_EN_DR;
        val &= ~REG_COARSE_EN;
        dco_write(DCO_REG_161, val);
    } else if (s_dco_current_tuning_mode == DCO_TUNING_MODE_FINE) {
        dco_read(DCO_REG_166, &val);
        val |= REG_FINE_EN_DR;
        val &= ~REG_FINE_EN;
        dco_write(DCO_REG_166, val);
    } else if (s_dco_current_tuning_mode == DCO_TUNING_MODE_SDM) {
        dco_read(DCO_REG_261, &val);
        val |= REG_SDM_OUT_EN_DR;
        val &= ~REG_SDM_OUT_EN;
        dco_write(DCO_REG_261, val);
    } 

    if (tuning_mode == DCO_TUNING_MODE_COARSE) {
        dco_read(DCO_REG_161, &val);
        val |= REG_COARSE_EN_DR | REG_COARSE_EN;
        dco_write(DCO_REG_161, val);
    }

    if (tuning_mode == DCO_TUNING_MODE_FINE) {
        dco_read(DCO_REG_261, &val);
        val |= REG_SDM_RSTB;
        dco_write(DCO_REG_261, val);
        
        dco_read(DCO_REG_162, &val);
        val &= ~REG_SOF_CLK_USER_SEL;
        dco_write(DCO_REG_162, val);
        
        dco_read(DCO_REG_166, &val);
        val |= REG_FINE_EN_DR | REG_FINE_EN;
        dco_write(DCO_REG_166, val);
    }

    if (tuning_mode == DCO_TUNING_MODE_SDM) {

        
        dco_read(DCO_REG_261, &val);
        val |= REG_SDM_OUT_EN_DR | REG_SDM_RSTB;
        val &= ~REG_SDM_OUT_EN;
        dco_write(DCO_REG_261, val);
        
        dco_read(DCO_REG_162, &val);
        val |= REG_SOF_CLK_USER_SEL;
        dco_write(DCO_REG_162, val);

        dco_read(DCO_REG_271, &val);
        val &= ~(REG_LOAD_DWC_OUT | REG_LOAD_DWC_IN);
        dco_write(DCO_REG_271, val);
        
        dco_read(DCO_REG_261, &val);
        val |= REG_SDM_OUT_EN;
        val &= ~REG_SDM_OUT_TUN_CODE_LOAD_EN;
        dco_write(DCO_REG_261, val);
    }
    
    s_dco_current_tuning_mode = tuning_mode;
}

void dco_set_tuning_reference_clk (uint8_t clk_src)
{
    if (hal_get_chip_metal_id() >= 1) {
        
        uint16_t val;
        static uint8_t s_current_ref_clk_src = 0xFF;

        TRACE(3,"%s: clk_src [%d -> %d]", __func__, s_current_ref_clk_src, clk_src);

        if (clk_src == s_current_ref_clk_src) {
            return;
        }

        dco_read(DCO_REG_162, &val);
        if (clk_src == DCO_TUNING_REF_SOF_CLK) {
            val &= ~REG_SOF_CLK_SOURCE_SEL;            
        } else if (clk_src == DCO_TUNING_REF_EXT_13MHz) {
            val |= REG_SOF_CLK_SOURCE_SEL;
        }
        dco_write(DCO_REG_162, val);

        s_current_ref_clk_src = clk_src;
    }
}

#endif  // CHIP_HAS_DCO

