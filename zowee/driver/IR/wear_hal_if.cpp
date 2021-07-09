#ifdef __APP_WEAR_ENABLE__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt_hardware.h"
#include "wear_hal_if.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "ir_w2001.h"
#include "touch_iqs269a.h"
#include "zowee_config.h"

extern const hal_wear_if_t wear_ctrl;

static hal_wear_if_t *p_wear_ctrl = NULL;

void wear_hal_if_int_handler(void)
{
	if (p_wear_ctrl != NULL && p_wear_ctrl->int_handle != NULL)
	{
		p_wear_ctrl->int_handle();
		//WEAR_HAL_TRACE();
	}
}

void wear_hal_if_init(void)
{
	WEAR_HAL_TRACE();
	p_wear_ctrl = (hal_wear_if_t *)&wear_ctrl;
	if(p_wear_ctrl->init != NULL)
	{
		p_wear_ctrl->init();
	}

}

void wear_hal_if_enable(void)
{
	WEAR_HAL_TRACE();
	p_wear_ctrl = (hal_wear_if_t *)&wear_ctrl;
	if(p_wear_ctrl->enable_wear != NULL)
	{
		p_wear_ctrl->enable_wear();
	}

}

void wear_hal_if_disable(void)
{
	WEAR_HAL_TRACE();
	p_wear_ctrl = (hal_wear_if_t *)&wear_ctrl;
	if(p_wear_ctrl->disable_wear != NULL)
	{
		p_wear_ctrl->disable_wear();
	}
}

void wear_hal_if_refresh_cali_data(void)
{
	WEAR_HAL_TRACE();
	p_wear_ctrl = (hal_wear_if_t *)&wear_ctrl;
	if(p_wear_ctrl->refresh_flash_data != NULL)
	{
		p_wear_ctrl->refresh_flash_data();
	}
}


#endif//__APP_WEAR_ENABLE__

