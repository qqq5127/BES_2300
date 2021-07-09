#ifndef __TOUCH_IQS269__
#define	__TOUCH_IQS269__

#include "hal_gpio.h"
#include "zowee_config.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "../gesture_hal_if.h"
#include "../IR/wear_hal_if.h"
#include "zowee_config.h"


#if __TOUCH_IQS269A__



#define	TOUCH_IQS269A_TRACE_ENABLE		1

#if TOUCH_IQS269A_TRACE_ENABLE
#define APP_IQS269A_TRACE(s,...)		TRACE(1, "[TOUCH_IQS269A]%s " s, __func__, ##__VA_ARGS__)
#else
#define APP_IQS269A_TRACE(s,...)
#endif


/*! I2C communication addresses */
#define TOUCH_IQS_ADDR                        0x44

#define IQS269_REG_CHIP_ID                    0x00
#define IQS269_REG_I2C_SETTING                0xF2

#define IQS269_PRODUCT_NUMBER                 0x4F  // representative IQS269A;
#define IQS269_SOFTWARE_NUMBER                0x03  // the newest IC version firmware�?
#define IQS269_HARDWARE_NUMBER                0x0D  //

#define IQS269_MM_SYSTEM_FLAGS                0x02
#define IQS269_MM_PMU_SETTINGS                0x80
#define IQS269_MM_PXS_GLBL_SETTINGS_0         0x86
#define IQS269_MM_CH0_SELECT_CRX              0x8C
#define IQS269_MM_CH1_SELECT_CRX              0x93
#define IQS269_MM_CH2_SELECT_CRX              0x9A
#define IQS269_MM_CH3_SELECT_CRX              0xA1
#define IQS269_MM_CH4_SELECT_CRX              0xA8
#define IQS269_MM_CH5_SELECT_CRX              0xAF
#define IQS269_MM_CH6_SELECT_CRX              0xB6
#define IQS269_MM_CH7_SELECT_CRX              0xBD

#define IQS268_MM_RH_EVENT_SET 				 0x82

#define GESTURE_MODE                          0x08
#define CHANNEL_2                             0x04  //touch CX2
#define CHANNEL_0                             0x01  //Wear on/off CX6
#define ENENT_MODE							  0x02	
#define SHOW_RESET							  0x80
#define TOUCH_EVENT							  0x02
#define PROX_EVENT							  0x01 //接近感应


#define IQS269A_AUDO_ATI_CHANNAL			  0x8B

#if 0
/* Change the Channel select, Power mode & System Settings */
/* Memory Map Position 0x80 - 0x82 */
#define PMU_GENERAL_SETTINGS                     0x23
#define I2C_GENERAL_SETTINGS                     0x20
#define SYS_CHB_ACTIVE                           0x07
#define ACF_LTA_FILTER_SETTINGS                  0x40
#define LTA_CHB_RESEED_ENABLED                   0xFC
#define UIS_GLOBAL_EVENTS_MASK                   0xAC

/* Change the Report Rates and Timing */
/* Memory Map Position 0x83 - 0x85 */
#define PMU_REPORT_RATE_NP                       0x10
#define PMU_REPORT_RATE_LP                       0xA0
#define PMU_REPORT_RATE_ULP                      0x0A
#define PMU_MODE_TIMOUT                          0x38
#define I2C_WINDOW_TIMEOUT                       0x14
#define LTA_HALT_TIMEOUT                         0x40

/* Change the Global Settings */
/* Memory Map Position 0x86 - 0x8B */
#define PXS_GENERAL_SETTINGS0                    0x00
#define PXS_GENERAL_SETTINGS1                    0x00
#define UIS_ABSOLUTE_CAPACITANCE                 0x00
#define UIS_DCF_GENERAL_SETTINGS                 0x51
#define GEM_CHB_BLOCK_NFOLLOW                    0x00
#define MOV_CHB_MOVEMENT_CHANNEL                 0x00
#define UIS_CHB_SLIDER0                          0x00
#define UIS_CHB_SLIDER1                          0x14// 0x00
#define UIS_GESTURE_TAP_TIMEOUT                  0x19
#define UIS_GESTURE_SWIPE_TIMEOUT                0x7D
#define UIS_GESTURE_THRESHOLD                    0x80
#define LTA_CHB_RESEED                           0x07//0x07// 0x00

/* Change the CH0 Settings */
/* Memory Map Position 0x8C - 0x92 */
#define PXS_CRXSEL_CH0                           0x40
#define PXS_CTXSEL_CH0                           0xFF
#define PXS_PROXCTRL_CH0                         0xA3
#define PXS_PROXCFG0_CH0                         0x40
#define PXS_PROXCFG1_TESTREG0_CH0                0x06
#define ATI_BASE_AND_TARGET_CH0                  0x59
#define ATI_MIRROR_CH0                           0x4D
#define ATI_PCC_CH0                              0x66
#define PXS_PROX_THRESHOLD_CH0                   0x04
#define PXS_TOUCH_THRESHOLD_CH0                  0x09
#define PXS_DEEP_THRESHOLD_CH0                   0x60
#define PXS_HYSTERESIS_CH0                       0xFF
#define DCF_CHB_ASSOCIATION_CH0                  0x00
#define DCF_WEIGHT_CH0                           0x4D

/* Change the CH1 Settings */
/* Memory Map Position 0x93 - 0x99 */
#define PXS_CRXSEL_CH1                           0x10
#define PXS_CTXSEL_CH1                           0xFF
#define PXS_PROXCTRL_CH1                         0xA3
#define PXS_PROXCFG0_CH1                         0x40
#define PXS_PROXCFG1_TESTREG0_CH1                0x06
#define ATI_BASE_AND_TARGET_CH1                  0x59
#define ATI_MIRROR_CH1                           0x4A
#define ATI_PCC_CH1                              0x5B
#define PXS_PROX_THRESHOLD_CH1                   0x04
#define PXS_TOUCH_THRESHOLD_CH1                  0x09
#define PXS_DEEP_THRESHOLD_CH1                   0x60
#define PXS_HYSTERESIS_CH1                       0xFF
#define DCF_CHB_ASSOCIATION_CH1                  0x01
#define DCF_WEIGHT_CH1                           0x00

/* Change the CH2 Settings */
/* Memory Map Position 0x9A - 0xA0 */
#define PXS_CRXSEL_CH2                           0x04
#define PXS_CTXSEL_CH2                           0xFF
#define PXS_PROXCTRL_CH2                         0xA3
#define PXS_PROXCFG0_CH2                         0x40
#define PXS_PROXCFG1_TESTREG0_CH2                0x06
#define ATI_BASE_AND_TARGET_CH2                  0x59
#define ATI_MIRROR_CH2                           0x49
#define ATI_PCC_CH2                              0x63
#define PXS_PROX_THRESHOLD_CH2                   0x05
#define PXS_TOUCH_THRESHOLD_CH2                  0x10
#define PXS_DEEP_THRESHOLD_CH2                   0x1A
#define PXS_HYSTERESIS_CH2                       0x44
#define DCF_CHB_ASSOCIATION_CH2                  0x00
#define DCF_WEIGHT_CH2                           0x00

/* Change the CH3 Settings */
/* Memory Map Position 0xA1 - 0xA7 */
#define PXS_CRXSEL_CH3                           0x01
#define PXS_CTXSEL_CH3                           0xFF
#define PXS_PROXCTRL_CH3                         0xA3
#define PXS_PROXCFG0_CH3                         0x40
#define PXS_PROXCFG1_TESTREG0_CH3                0x02
#define ATI_BASE_AND_TARGET_CH3                  0x59
#define ATI_MIRROR_CH3                           0x47
#define ATI_PCC_CH3                              0x82
#define PXS_PROX_THRESHOLD_CH3                   0x0A
#define PXS_TOUCH_THRESHOLD_CH3                  0x07
#define PXS_DEEP_THRESHOLD_CH3                   0x04
#define PXS_HYSTERESIS_CH3                       0x04
#define DCF_CHB_ASSOCIATION_CH3                  0x00
#define DCF_WEIGHT_CH3                           0x00

/* Change the CH4 Settings */
/* Memory Map Position 0xA8 - 0xAE */
#define PXS_CRXSEL_CH4                           0x10
#define PXS_CTXSEL_CH4                           0xFF
#define PXS_PROXCTRL_CH4                         0xA3
#define PXS_PROXCFG0_CH4                         0x40
#define PXS_PROXCFG1_TESTREG0_CH4                0x02
#define ATI_BASE_AND_TARGET_CH4                  0x50
#define ATI_MIRROR_CH4                           0x42
#define ATI_PCC_CH4                              0x52
#define PXS_PROX_THRESHOLD_CH4                   0x0A
#define PXS_TOUCH_THRESHOLD_CH4                  0x08
#define PXS_DEEP_THRESHOLD_CH4                   0x1A
#define PXS_HYSTERESIS_CH4                       0x04
#define DCF_CHB_ASSOCIATION_CH4                  0x00
#define DCF_WEIGHT_CH4                           0x00

/* Change the CH5 Settings */
/* Memory Map Position 0xAF - 0xB5 */
#define PXS_CRXSEL_CH5                           0x20
#define PXS_CTXSEL_CH5                           0xFF
#define PXS_PROXCTRL_CH5                         0xA3
#define PXS_PROXCFG0_CH5                         0x40
#define PXS_PROXCFG1_TESTREG0_CH5                0x02
#define ATI_BASE_AND_TARGET_CH5                  0x50
#define ATI_MIRROR_CH5                           0x42
#define ATI_PCC_CH5                              0x62
#define PXS_PROX_THRESHOLD_CH5                   0x0A
#define PXS_TOUCH_THRESHOLD_CH5                  0x08
#define PXS_DEEP_THRESHOLD_CH5                   0x1A
#define PXS_HYSTERESIS_CH5                       0x04
#define DCF_CHB_ASSOCIATION_CH5                  0x00
#define DCF_WEIGHT_CH5                           0x00

/* Change the CH6 Settings */
/* Memory Map Position 0xB6 - 0xBC */
#define PXS_CRXSEL_CH6                           0x40
#define PXS_CTXSEL_CH6                           0xFF
#define PXS_PROXCTRL_CH6                         0xA3
#define PXS_PROXCFG0_CH6                         0x40
#define PXS_PROXCFG1_TESTREG0_CH6                0x02
#define ATI_BASE_AND_TARGET_CH6                  0x50
#define ATI_MIRROR_CH6                           0x42
#define ATI_PCC_CH6                              0x69
#define PXS_PROX_THRESHOLD_CH6                   0x0A
#define PXS_TOUCH_THRESHOLD_CH6                  0x08
#define PXS_DEEP_THRESHOLD_CH6                   0x1A
#define PXS_HYSTERESIS_CH6                       0x04
#define DCF_CHB_ASSOCIATION_CH6                  0x00
#define DCF_WEIGHT_CH6                           0x00

/* Change the CH7 Settings */
/* Memory Map Position 0xBD - 0xC3 */
#define PXS_CRXSEL_CH7                           0x80
#define PXS_CTXSEL_CH7                           0xFF
#define PXS_PROXCTRL_CH7                         0xA3
#define PXS_PROXCFG0_CH7                         0x40
#define PXS_PROXCFG1_TESTREG0_CH7                0x02
#define ATI_BASE_AND_TARGET_CH7                  0x50
#define ATI_MIRROR_CH7                           0x42
#define ATI_PCC_CH7                              0x82
#define PXS_PROX_THRESHOLD_CH7                   0x0A
#define PXS_TOUCH_THRESHOLD_CH7                  0x08
#define PXS_DEEP_THRESHOLD_CH7                   0x1A
#define PXS_HYSTERESIS_CH7                       0x04
#define DCF_CHB_ASSOCIATION_CH7                  0x00
#define DCF_WEIGHT_CH7                           0x00

/* Change the I2C Control Settings */
/* Memory Map Position 0xF2 - 0xF2 */
#define I2C_CONTROL_SETTINGS                     0x01

/* Change the Hall UI */
/* Memory Map Position 0xF5 - 0xF5 */
#define HALL_UI_ENABLE                           0x8E
#endif
/*
* This file contains all the necessary settings for the IQS269A and this file can
* be changed from the GUI or edited here
* File:   IQS269A_init.h
* Author: Azoteq
*/ 

#ifndef IQS269A_INIT_H
#define IQS269A_INIT_H

/* Change the Channel select, Power mode & System Settings */
/* Memory Map Position 0x80 - 0x82 */
#define PMU_GENERAL_SETTINGS                     0x23
#define I2C_GENERAL_SETTINGS                     0x20
#define SYS_CHB_ACTIVE                           0x07
#define ACF_LTA_FILTER_SETTINGS                  0x40
#define LTA_CHB_RESEED_ENABLED                   0xFC
#define UIS_GLOBAL_EVENTS_MASK                   0xEC

/* Change the Report Rates and Timing */
/* Memory Map Position 0x83 - 0x85 */
#define PMU_REPORT_RATE_NP                       0x10
#define PMU_REPORT_RATE_LP                       0xA0
#define PMU_REPORT_RATE_ULP                      0x0A
#define PMU_MODE_TIMOUT                          0x38
#define I2C_WINDOW_TIMEOUT                       0x14
#define LTA_HALT_TIMEOUT                         0x40

/* Change the Global Settings */
/* Memory Map Position 0x86 - 0x8B */
#define PXS_GENERAL_SETTINGS0                    0x00
#define PXS_GENERAL_SETTINGS1                    0x00
#define UIS_ABSOLUTE_CAPACITANCE                 0x00
#define UIS_DCF_GENERAL_SETTINGS                 0x51
#define GEM_CHB_BLOCK_NFOLLOW                    0x00
#define MOV_CHB_MOVEMENT_CHANNEL                 0x00
#define UIS_CHB_SLIDER0                          0x00
#define UIS_CHB_SLIDER1                          0x14// 0x00
#define UIS_GESTURE_TAP_TIMEOUT                  0x19
#define UIS_GESTURE_SWIPE_TIMEOUT                0x7D
#define UIS_GESTURE_THRESHOLD                    0x80
#define LTA_CHB_RESEED                           0x07//0x07// 0x00

/* Change the CH0 Settings */
/* Memory Map Position 0x8C - 0x92 */
#define PXS_CRXSEL_CH0                           0x40
#define PXS_CTXSEL_CH0                           0xFF
#define PXS_PROXCTRL_CH0                         0xA3
#define PXS_PROXCFG0_CH0                         0x40
#define PXS_PROXCFG1_TESTREG0_CH0                0x06
#define ATI_BASE_AND_TARGET_CH0                  0x59
#define ATI_MIRROR_CH0                           0x4D
#define ATI_PCC_CH0                              0x6D
#define PXS_PROX_THRESHOLD_CH0                   0x04
#define PXS_TOUCH_THRESHOLD_CH0                  0x09
#define PXS_DEEP_THRESHOLD_CH0                   0x60
#define PXS_HYSTERESIS_CH0                       0xFF
#define DCF_CHB_ASSOCIATION_CH0                  0x00
#define DCF_WEIGHT_CH0                           0x4D

/* Change the CH1 Settings */
/* Memory Map Position 0x93 - 0x99 */
#define PXS_CRXSEL_CH1                           0x10
#define PXS_CTXSEL_CH1                           0xFF
#define PXS_PROXCTRL_CH1                         0xA3
#define PXS_PROXCFG0_CH1                         0x40
#define PXS_PROXCFG1_TESTREG0_CH1                0x06
#define ATI_BASE_AND_TARGET_CH1                  0x59
#define ATI_MIRROR_CH1                           0x4A
#define ATI_PCC_CH1                              0x62
#define PXS_PROX_THRESHOLD_CH1                   0x04
#define PXS_TOUCH_THRESHOLD_CH1                  0x09
#define PXS_DEEP_THRESHOLD_CH1                   0x60
#define PXS_HYSTERESIS_CH1                       0xFF
#define DCF_CHB_ASSOCIATION_CH1                  0x01
#define DCF_WEIGHT_CH1                           0x00

/* Change the CH2 Settings */
/* Memory Map Position 0x9A - 0xA0 */
#define PXS_CRXSEL_CH2                           0x04
#define PXS_CTXSEL_CH2                           0xFF
#define PXS_PROXCTRL_CH2                         0xA3
#define PXS_PROXCFG0_CH2                         0x40
#define PXS_PROXCFG1_TESTREG0_CH2                0x06
#define ATI_BASE_AND_TARGET_CH2                  0x59
#define ATI_MIRROR_CH2                           0x4A
#define ATI_PCC_CH2                              0x55
#define PXS_PROX_THRESHOLD_CH2                   0x05
#define PXS_TOUCH_THRESHOLD_CH2                  0x10
#define PXS_DEEP_THRESHOLD_CH2                   0x1A
#define PXS_HYSTERESIS_CH2                       0x44
#define DCF_CHB_ASSOCIATION_CH2                  0x00
#define DCF_WEIGHT_CH2                           0x00

/* Change the CH3 Settings */
/* Memory Map Position 0xA1 - 0xA7 */
#define PXS_CRXSEL_CH3                           0x01
#define PXS_CTXSEL_CH3                           0xFF
#define PXS_PROXCTRL_CH3                         0xA3
#define PXS_PROXCFG0_CH3                         0x40
#define PXS_PROXCFG1_TESTREG0_CH3                0x02
#define ATI_BASE_AND_TARGET_CH3                  0x59
#define ATI_MIRROR_CH3                           0x47
#define ATI_PCC_CH3                              0x82
#define PXS_PROX_THRESHOLD_CH3                   0x0A
#define PXS_TOUCH_THRESHOLD_CH3                  0x07
#define PXS_DEEP_THRESHOLD_CH3                   0x04
#define PXS_HYSTERESIS_CH3                       0x04
#define DCF_CHB_ASSOCIATION_CH3                  0x00
#define DCF_WEIGHT_CH3                           0x00

/* Change the CH4 Settings */
/* Memory Map Position 0xA8 - 0xAE */
#define PXS_CRXSEL_CH4                           0x10
#define PXS_CTXSEL_CH4                           0xFF
#define PXS_PROXCTRL_CH4                         0xA3
#define PXS_PROXCFG0_CH4                         0x40
#define PXS_PROXCFG1_TESTREG0_CH4                0x02
#define ATI_BASE_AND_TARGET_CH4                  0x50
#define ATI_MIRROR_CH4                           0x42
#define ATI_PCC_CH4                              0x52
#define PXS_PROX_THRESHOLD_CH4                   0x0A
#define PXS_TOUCH_THRESHOLD_CH4                  0x08
#define PXS_DEEP_THRESHOLD_CH4                   0x1A
#define PXS_HYSTERESIS_CH4                       0x04
#define DCF_CHB_ASSOCIATION_CH4                  0x00
#define DCF_WEIGHT_CH4                           0x00

/* Change the CH5 Settings */
/* Memory Map Position 0xAF - 0xB5 */
#define PXS_CRXSEL_CH5                           0x20
#define PXS_CTXSEL_CH5                           0xFF
#define PXS_PROXCTRL_CH5                         0xA3
#define PXS_PROXCFG0_CH5                         0x40
#define PXS_PROXCFG1_TESTREG0_CH5                0x02
#define ATI_BASE_AND_TARGET_CH5                  0x50
#define ATI_MIRROR_CH5                           0x42
#define ATI_PCC_CH5                              0x62
#define PXS_PROX_THRESHOLD_CH5                   0x0A
#define PXS_TOUCH_THRESHOLD_CH5                  0x08
#define PXS_DEEP_THRESHOLD_CH5                   0x1A
#define PXS_HYSTERESIS_CH5                       0x04
#define DCF_CHB_ASSOCIATION_CH5                  0x00
#define DCF_WEIGHT_CH5                           0x00

/* Change the CH6 Settings */
/* Memory Map Position 0xB6 - 0xBC */
#define PXS_CRXSEL_CH6                           0x40
#define PXS_CTXSEL_CH6                           0xFF
#define PXS_PROXCTRL_CH6                         0xA3
#define PXS_PROXCFG0_CH6                         0x40
#define PXS_PROXCFG1_TESTREG0_CH6                0x02
#define ATI_BASE_AND_TARGET_CH6                  0x50
#define ATI_MIRROR_CH6                           0x42
#define ATI_PCC_CH6                              0x69
#define PXS_PROX_THRESHOLD_CH6                   0x0A
#define PXS_TOUCH_THRESHOLD_CH6                  0x08
#define PXS_DEEP_THRESHOLD_CH6                   0x1A
#define PXS_HYSTERESIS_CH6                       0x04
#define DCF_CHB_ASSOCIATION_CH6                  0x00
#define DCF_WEIGHT_CH6                           0x00

/* Change the CH7 Settings */
/* Memory Map Position 0xBD - 0xC3 */
#define PXS_CRXSEL_CH7                           0x80
#define PXS_CTXSEL_CH7                           0xFF
#define PXS_PROXCTRL_CH7                         0xA3
#define PXS_PROXCFG0_CH7                         0x40
#define PXS_PROXCFG1_TESTREG0_CH7                0x02
#define ATI_BASE_AND_TARGET_CH7                  0x50
#define ATI_MIRROR_CH7                           0x42
#define ATI_PCC_CH7                              0x82
#define PXS_PROX_THRESHOLD_CH7                   0x0A
#define PXS_TOUCH_THRESHOLD_CH7                  0x08
#define PXS_DEEP_THRESHOLD_CH7                   0x1A
#define PXS_HYSTERESIS_CH7                       0x04
#define DCF_CHB_ASSOCIATION_CH7                  0x00
#define DCF_WEIGHT_CH7                           0x00

/* Change the I2C Control Settings */
/* Memory Map Position 0xF2 - 0xF2 */
#define I2C_CONTROL_SETTINGS                     0x01

/* Change the Hall UI */
/* Memory Map Position 0xF5 - 0xF5 */
#define HALL_UI_ENABLE                           0xCF

#endif	/* IQS269A_INIT_H */




typedef struct{

	uint8_t  wear_state;
	uint8_t  touch_state;
	uint8_t  double_click;//双击
	uint8_t  trible_click;//三击
	uint8_t  long_click;//长按
//	uint8_t  i2c_stop_bit;
	uint8_t  prox_flag; //接近感应
	uint8_t  first_wear_on;//第一次佩戴
	uint8_t  iqs269a_wear_flag;//佩戴标志位
	uint8_t  iqs_reset_count;//iqs重新初始化计�?
	uint16_t touch_count;//按键次数计数
	uint16_t time_count;//按键事件计数
	uint16_t timeout_count;//超时计数
	uint16_t wear_delay_count;//佩戴之后按键检测处理延�?

}iqs269a_state_t;



const struct HAL_IOMUX_PIN_FUNCTION_MAP app_touch_int_cfg = {
    TOUCH_INT_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_ldo_en_cfg = {
	TOUCH_LDO_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE
};


#ifdef __cplusplus
extern "C" {
#endif



int iqs269a_init(void);
void iqs269a_int_handler(void);
void iqs269a_touch_recog_handler(uint8_t *data, uint8_t len);
int iqs269a_get_wear_state(void);
void iqs269a_set_wear_state(uint8_t flag);
void iqs269a_touch_control(uint8_t flag);
void iqs269a_touch_timer_init(void);
void iqs269a_count_timehandler(void const *param);
void iqs269a_ldo_enable(void);
void iqs269a_ldo_disable(void);





#ifdef __cplusplus
}
#endif

#endif

#endif

