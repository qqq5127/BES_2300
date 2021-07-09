#ifndef __TOUCH_GPIOTE_H__
#define	__TOUCH_GPIOTE_H__

#include "hal_gpio.h"
#include "zowee_config.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "gesture_hal_if.h"


#if __TOUCH_GPIOTE__



#define	TOUCH_GPIOTE_TRACE_ENABLE	1

#if TOUCH_GPIOTE_TRACE_ENABLE
#define APP_GPIOTE_TRACE(s,...)		TRACE(1, "[GPIO TOUCH]%s " s, __func__, ##__VA_ARGS__)
#else
#define APP_GPIOTE_TRACE(s,...)
#endif

#define TOUCH_LLPRESS_THRESH_MS      1000  //5000 ms 
#define TOUCH_LPRESS_THRESH_MS       400  //2000 ms
#define TOUCH_TBLCLICK_THRESH_MS     100  //  500 ms
#define TOUCH_DBLCLICK_THRESH_MS     80  //400 ms
#define TOUCH_DOWN_THRESH_MS      	 40  //200 ms
#define TOUCH_CLICK_DEBOUNCE_MS      8  //40 ms


const struct HAL_IOMUX_PIN_FUNCTION_MAP touch_gpiote_int_cfg = {
	TOUCH_GPIOTE_INT_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE
};

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	TOUCH_GPIOTE_IDLE = 0,
	TOUCH_GPIOTE_HOLD,
	TOUCH_GPIOTE_RELEASE,
}touch_status_t;


typedef struct
{
	bool timer_flag;
	touch_status_t	status;
	uint16_t hold_cnt;
	uint16_t hold_cnt_back;
	uint16_t release_cnt;
	uint16_t beat_cnt;
	uint16_t touch_period;
}touch_gpiote_struct_t;

void touch_gpiote_handler(void);
int32_t touch_gpiote_init(void);


#ifdef __cplusplus
}
#endif

#endif

#endif
