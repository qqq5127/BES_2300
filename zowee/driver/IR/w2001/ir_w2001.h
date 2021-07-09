#ifndef	__IR_W2001_H__
#define	__IR_W2001_H__

#if __IR_W2001__

#include "hal_gpio.h"
#include "zowee_config.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "wear_hal_if.h"


#define	IR_W2001_TEST			0


#define	IR_W2001_TRACE_ENABLE		1

#define	IR_W2001_MAX_PS_DATA		10000
#define	IR_W2001_NEED_CALIBRATION_OFFSET	5000
#if IR_W2001_TRACE_ENABLE
#define IR_W2001_TRACE(s,...)		TRACE(1, "[IR_W2001]%s " s, __func__, ##__VA_ARGS__)
#else
#define IR_W2001_TRACE(s,...)
#endif

#define	IR_W2001_ADDR		    0x67	

/*ALSPS REGS*/
#define STK_STATE_REG 			0x00
#define STK_PSCTRL_REG 			0x01
#define STK_ALSCTRL_REG 		0x02
#define STK_LEDCTRL_REG 		0x03
#define STK_INT_REG 			0x04
#define STK_WAIT_REG 			0x05
#define STK_THDH1_PS_REG 		0x06
#define STK_THDH2_PS_REG 		0x07
#define STK_THDL1_PS_REG 		0x08
#define STK_THDL2_PS_REG 		0x09
#define STK_THDH1_ALS_REG 		0x0A
#define STK_THDH2_ALS_REG 		0x0B
#define STK_THDL1_ALS_REG 		0x0C
#define STK_THDL2_ALS_REG 		0x0D
#define STK_FLAG_REG 			0x10
#define STK_DATA1_PS_REG	 	0x11
#define STK_DATA2_PS_REG 		0x12
#define STK_DATA1_ALS_REG 		0x13
#define STK_DATA2_ALS_REG 		0x14
#define STK_DATA1_OFFSET_REG 	0x1D
#define STK_DATA2_OFFSET_REG 	0x1E
#define STK_PDT_ID_REG 			0x3E
#define STK_SW_RESET_REG		0x80

/* Define state reg */
#define STK_STATE_EN_WAIT_SHIFT  	2
#define STK_STATE_EN_ALS_SHIFT  	1
#define STK_STATE_EN_PS_SHIFT  	    0

#define STK_STATE_EN_WAIT_MASK	0x04
#define STK_STATE_EN_ALS_MASK	0x02
#define STK_STATE_EN_PS_MASK	0x01

#define STK_INT_PS_MODE1			0x01
#define STK_INT_PS_MODE3			0x03
#define STK_INT_PS_MODE4			0x04
#define STK_INT_ALS				    0x08

#define STK_FLG_ALSDR_MASK		0x80
#define STK_FLG_PSDR_MASK		0x40
#define STK_FLG_ALSINT_MASK		0x20
#define STK_FLG_PSINT_MASK		0x10
#define STK_FLG_NF_MASK			0x01

/***********************************************/

#define ALS_INTERRUPT_THD_DIFF 50 //20

#define PS_INTERRUPT_MODE 	1
#define ALS_INTERRUPT_MODE	0



#ifdef __cplusplus
extern "C" {
#endif

const struct HAL_IOMUX_PIN_FUNCTION_MAP ldo_en_cfg = {
	LDO_ENABLE_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP ir_w2001_int_cfg = {
	IR_W2001_INT_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE
};

#define	IR_W2001_READ_PS_DATA_TIME 		200	// 20 seconds

typedef struct
{
	bool refresh_flash_data_flag;
	bool wear_off_flag;
	uint16_t read_ps_data_times;
	uint32_t ps_data_min;
	uint32_t ps_data_max;
	uint32_t ps_actually_min;
	uint32_t ps_actually_max;
}ir_w2001_struct_t;

/*init interface*/
void STK_Init(void);

/*ps enable interface*/
uint32_t STK_enable_PS(void);
uint32_t STK_DisEnable_PS(void);

/*als enable interface*/
uint32_t STK_enable_ALS(void);
uint32_t STK_DisEnable_ALS(void);

/*polling mode interface*/
void read_als_data(void);
void get_ps_data(void);

/*interrupt mode interface*/
void eint_work_func(void);

uint16_t STK_Read_ALS(void);
uint16_t STK_Read_PS(void);
void STK_sw_reset(void);
void STK_Show_Allreg(void);
int ir_w2001_init(void);
void ir_w2001_int_handler(void);
void ir_w2001_uninit(void);


#ifdef __cplusplus
}
#endif


#endif

#endif
