#ifdef __APP_BATTERY_USER__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt_hardware.h"
#include "battery_hal_if.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "battery_zt203.h"
#include "battery_zt102.h"
#include "zowee_config.h"

extern const hal_battery_if_t battery_ctrl;

bool battery_hal_if_plug_out(void)
{
	if(battery_ctrl.plug_out != NULL)
	{
		return battery_ctrl.plug_out();
	}

	return false;
}


void battery_hal_if_plug_in(void)
{
	if(battery_ctrl.plug_in != NULL)
	{
		battery_ctrl.plug_in();
	}
}

void battery_hal_if_init(void)
{
	if(battery_ctrl.init != NULL)
	{
		battery_ctrl.init();
	}
}

void battery_hal_if_uninit(void)
{
	if(battery_ctrl.uninit != NULL)
	{
		battery_ctrl.uninit();
	}
}

uint8_t battery_hal_if_charging_status(void)
{
	if(battery_ctrl.charging_status != NULL)
	{
		return battery_ctrl.charging_status();
	}

	return 0xff;
}

#endif//__APP_WEAR_ENABLE__

