#ifdef __APP_GESTURE_ENABLE__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt_hardware.h"
#include "gesture_hal_if.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "touch_iqs269a.h"
#include "zowee_config.h"
#include "touch_gpiote.h"

extern const hal_gesture_if_t gesture_ctrl;

static hal_gesture_if_t *p_gesture_ctrl = NULL;

void gesture_hal_if_int_handler(void)
{
	if (p_gesture_ctrl != NULL && p_gesture_ctrl->int_handle != NULL)
	{
		p_gesture_ctrl->int_handle();
		//TOUCH_HAL_TRACE();
	}
}

void gesture_hal_if_init(void)
{
	GESTURE_HAL_TRACE();
	p_gesture_ctrl = (hal_gesture_if_t *)&gesture_ctrl;
	if(p_gesture_ctrl->init != NULL)
	{
		p_gesture_ctrl->init();
	}
}

void gesture_hal_if_disable(void)
{
	GESTURE_HAL_TRACE();
	p_gesture_ctrl = (hal_gesture_if_t *)&gesture_ctrl;
	if(p_gesture_ctrl->disable_gesture != NULL)
	{
		p_gesture_ctrl->disable_gesture();
	}
}

void gesture_hal_if_enable(void)
{
	GESTURE_HAL_TRACE();
	p_gesture_ctrl = (hal_gesture_if_t *)&gesture_ctrl;
	if(p_gesture_ctrl->enable_gesture != NULL)
	{
		p_gesture_ctrl->enable_gesture();
	}
}



//void iqs_wear_int_handler(uint8_t flag)
//{
//	if (p_gesture_ctrl != NULL && p_gesture_ctrl->iqs_wear_handler != NULL)
//	{
//		p_gesture_ctrl->iqs_wear_handler(flag);
//		//TOUCH_HAL_TRACE();
//	}
//}




#endif//__APP_GESTURE_ENABLE__

