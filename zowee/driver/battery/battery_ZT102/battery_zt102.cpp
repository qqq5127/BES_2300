#include "battery_zt102.h"

#if __BATTERY_ZT102__
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "tgt_hardware.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "app_thread.h"
#include "hwtimer_list.h"
#include "ui_event_manage.h"
#include "app_battery.h"
#include "hal_gpadc.h"
#include "pmu.h"
#include "pmu_best2300p.h"
#include "apps.h"
#include "ui_event_bt.h"
//#include "wear_hal_if.h"
#include "app_ibrt_if.h"
#include "app_battery.h"
#include "ui_event_battery.h"



static void battery_check_timehandler(void const *param);
static CHARGING_BATTERY_STATUS charging_status = CHARGING_NONE;


osTimerDef(BATTERY_CHECK_TIMER, battery_check_timehandler);
static osTimerId battery_check_timer = NULL;

static uint8_t battery_charging_full_count = 0;



static void battery_zt102_charging_full_check(void)
{
	APP_BATTERY_MV_T vot_value;
	uint8_t current_level,charging_status_temp;

	if(charging_status == CHARGING_PLUG_IN)
	{
		app_battery_get_info(&vot_value,&current_level,(enum APP_BATTERY_STATUS_T *)&charging_status_temp);		
		BATTERY_ZT102_TRACE("current_level: %d, full gpio: %d !!!", current_level, hal_gpio_pin_get_val((HAL_GPIO_PIN_T)(battery_full_int_cfg.pin)));
		if(current_level >= 9)
		{
			if(hal_gpio_pin_get_val((HAL_GPIO_PIN_T )(battery_full_int_cfg.pin)))
			{
				BATTERY_ZT102_TRACE("CHARGING_FULL !!!");
				
				battery_charging_full_count = 0;
				charging_status = CHARGING_FULL;
			}
			else
			{
				battery_charging_full_count++;
				BATTERY_ZT102_TRACE("full check count: %d ", battery_charging_full_count);
				if(battery_charging_full_count > BATTERY_ZT102_FULL_TIMEROUT_COUNT)
				{
					BATTERY_ZT102_TRACE("full check over time : %d min, setting CHARGING_FULL !!!", BATTERY_ZT102_FULL_TIMEROUT_COUNT);

					battery_charging_full_count = BATTERY_ZT102_FULL_TIMEROUT_COUNT;
					charging_status = CHARGING_FULL;
				}
			}

			if(charging_status == CHARGING_FULL)
			{
				if(app_tws_ibrt_tws_link_connected())
				{
					zowee_tws_send_local_info_to_peer();
				}
			}
			
		}
		else
		{
			battery_charging_full_count = 0;
		}
	}
	else
	{
		battery_charging_full_count = 0;
	}
}
static void battery_check_timehandler(void const *param)
{
	BATTERY_ZT102_TRACE("%08x", hal_sys_timer_get());
	battery_zt102_charging_full_check();
	osTimerStart(battery_check_timer, 60000);	// 60s
}



static void battery_zt102_enable_full(void)
{   
	hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_full_int_cfg.pin, HAL_GPIO_DIR_IN, 0);
}

uint8_t battery_zt102_status(void)
{
	return charging_status;
}

void battery_zt102_plugin(void)
{
	charging_status = CHARGING_PLUG_IN;
	app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);

	BATTERY_ZT102_TRACE("charging_status: %s", charging_status == CHARGING_PLUG_OUT ? "PLUGOUT" : "PLUGIN");
}

bool battery_zt102_plugout(void)
{
	bool ret = false;

	app_ibrt_if_ui_t* ibrt_if_ui_ctx = app_ibrt_if_ui_get_ctx();

	BATTERY_ZT102_TRACE("box state %d", ibrt_if_ui_ctx->box_state);
	if((ibrt_if_ui_ctx->box_state == IBRT_IN_BOX_CLOSED) && (charging_status == CHARGING_FULL))
	{//charge full power off
		BATTERY_ZT102_TRACE("charging full power off");
		app_shutdown();	
		ret = true;
	}
	charging_status = CHARGING_PLUG_OUT;
	
	BATTERY_ZT102_TRACE("charging_status: %s", charging_status == CHARGING_PLUG_OUT ? "PLUGOUT" : "PLUGIN");

	battery_charging_full_count = 0;
	
	return ret;
}


void battery_zt102_init(void)
{
	BATTERY_ZT102_TRACE("!!!");
	
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&battery_full_int_cfg, 1);
	battery_zt102_enable_full();

	battery_check_timer = osTimerCreate(osTimer(BATTERY_CHECK_TIMER), osTimerOnce, NULL);

	osTimerStart(battery_check_timer, 60000);	//60s
}

void battery_zt102_uninit(void)
{
	app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
	app_battery_stop();
	app_battery_disable_pmu_irq();
	app_battery_close();
	ntc_capture_stop();

	osTimerStop(battery_check_timer);
}


#endif
