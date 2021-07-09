#include "battery_zt203.h"

#if __BATTERY_ZT203__
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
#include "wear_hal_if.h"
#include "app_ibrt_if.h"
#include "app_battery.h"
#include "ui_event_battery.h"
#include "ui_event_manage.h"
#include "ui_event_product_test.h"


#if BATTERY_TEST_TEMPERATURE
const NTC_TALBE battery_ntc_table[] ={
	{1720 ,  "-20" },
	{1698 ,  "-19" },
	{1676 ,  "-18" },
	{1654 ,  "-17" },
	{1632 ,  "-16" },
	{1610 ,  "-15" },
	{1588 ,  "-14" },
	{1566 ,  "-13" },
	{1544 ,  "-12" },
	{1522 ,  "-11" },
	{1497 ,  "-10" },
	{1471 ,  "-9 " },
	{1445 ,  "-8 " },
	{1419 ,  "-7 " },
	{1393 ,  "-6 " },
	{1367 ,  "-5 " },
	{1341 ,  "-4 " },
	{1315 ,  "-3 " },
	{1289 ,  "-2 " },
	{1263 ,  "-1 " },
	{1236 ,  "0  " },
	{1212 ,  "1  " },
	{1188 ,  "2  " },
	{1164 ,  "3  " },
	{1140 ,  "4  " },
	{1115 ,  "5  " },
	{1088 ,  "6  " },
	{1060 ,  "7  " },
	{1033 ,  "8  " },
	{1005 ,  "9  " },
	{978  ,  "10 " },
	{953  , "11 "  },
	{928  , "12 "  },
	{902  , "13 "  },
	{877  , "14 "  },
	{852  , "15 "  },
	{831  , "16 "  },
	{810  , "17 "  },
	{789  , "18 "  },
	{768  , "19 "  },
	{747  , "20 "  },
	{725  , "21 "  },
	{704  , "22 "  },
	{683  , "23 "  },
	{662  , "24 "  },
	{641  , "25 "  },
	{622  , "26 "  },
	{603  , "27 "  },
	{585  , "28 "  },
	{566  , "29 "  },
	{547  , "30 "  },
	{531  , "31 "  },
	{516  , "32 "  },
	{500  , "33 "  },
	{485  , "34 "  },
	{469  , "35 "  },
	{455  , "36 "  },
	{442  , "37 "  },
	{428  , "38 "  },
	{415  , "39 "  },
	{401  , "40 "  },
	{389  , "41 "  },
	{377  , "42 "  },
	{365  , "43 "  },
	{353  , "44 "  },
	{341  , "45 "  },
	{333  , "46 "  },
	{325  , "47 "  },
	{317  , "48 "  },
	{309  , "49 "  },
	{301  , "50 "  },
	{293  , "51 "  },
	{285  , "52 "  },
	{277  , "53 "  },
	{269  , "54 "  },
	{261  , "55 "  },
	{253  , "56 "  },
	{245  , "57 "  },
	{237  , "58 "  },
	{229  , "59 "  },
	{221  , "60 "  }


};
#endif

const uint16_t battery_ntc_low_to_high_table[]=
{
	1497,	//-10 °C
	1115,	//5 °C
	852,	//15 °C
	301,	//50 °C
	0,		//never reach
};

const uint16_t battery_ntc_high_to_low_table[]=
{
	1610,	//-15 °C
	1236, //0 °C
	978, //10 °C
	341,	//45 °C
	0,		//never reach
};

static void battery_zt203_charging_control(HAL_GPADC_MV_T volt);
static void battery_read_ntc_timehandler(void const *param);
static void battery_check_timehandler(void const *param);
static void battery_zt203_enable_slowly_charging(void);

static BATTERY_NTC_STATUS ntc_status = NTC_TEMPERATURE_NORMAL;
static CHARGING_BATTERY_STATUS charging_status = CHARGING_NONE;	//

osTimerDef(BATTERY_NTC_TIMER, battery_read_ntc_timehandler);
static osTimerId battery_ntc_timer = NULL;

osTimerDef(BATTERY_CHECK_TIMER, battery_check_timehandler);
static osTimerId battery_check_timer = NULL;

static uint8_t battery_charging_full_count = 0;
static uint32_t battery_power_on_sys_timer = 0;


static void battery_zt203_enable_charging(void);
static void battery_zt203_disable_charging(void);
static void battery_zt203_enable_quick_charging(void);
static void battery_zt203_enable_slowly_charging(void);



static void battery_zt203_charging_full_check(void)
{
	APP_BATTERY_MV_T vot_value;
	uint8_t current_level,charging_status_temp;
	uint8_t full_battery_pin = 0;

	if(charging_status == CHARGING_PLUG_IN)
	{
		full_battery_pin = hal_gpio_pin_get_val((HAL_GPIO_PIN_T)(battery_full_int_cfg.pin));
		app_battery_get_info(&vot_value,&current_level,(enum APP_BATTERY_STATUS_T *)&charging_status_temp);		
		BATTERY_ZT203_TRACE("vol: %d, level: %d, status: %d, full gpio: %d !!!", 
			vot_value, current_level, charging_status_temp, full_battery_pin);

		if((vot_value < 4050) && (current_level < 9) && (full_battery_pin == 1))
		{
			BATTERY_ZT203_TRACE("full battery check gpio abnormal, charging reset !!!");
			battery_zt203_disable_charging();
			hal_sys_timer_delay_us(100);
			battery_zt203_enable_quick_charging();
			battery_zt203_enable_charging();
		}

		if(current_level >= 9)
		{
			if(hal_gpio_pin_get_val((HAL_GPIO_PIN_T )(battery_full_int_cfg.pin)))
			{
				BATTERY_ZT203_TRACE("CHARGING_FULL !!!");
				
				battery_charging_full_count = 0;
				charging_status = CHARGING_FULL;
			}
			else
			{
				battery_charging_full_count++;
				BATTERY_ZT203_TRACE("full check count: %d ", battery_charging_full_count);
				if(battery_charging_full_count > BATTERY_ZT203_FULL_TIMEROUT_COUNT)
				{
					BATTERY_ZT203_TRACE("full check over time : %d min, setting CHARGING_FULL !!!", BATTERY_ZT203_FULL_TIMEROUT_COUNT);

					battery_charging_full_count = BATTERY_ZT203_FULL_TIMEROUT_COUNT;
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

static void battery_ntc_capture_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
	static bool first_check = 0;
	uint32_t current_sys_timer;

	
	current_sys_timer = hal_sys_timer_get();
	BATTERY_ZT203_TRACE("sys_timer %d %d",battery_power_on_sys_timer,current_sys_timer);
	
	if(current_sys_timer > battery_power_on_sys_timer)
	{
		if((current_sys_timer - battery_power_on_sys_timer) < 540000)
		{
			// 18000 = 1 second
			//do not check ntc in 30s when first power on
			return;
		}
	}

    if (volt == HAL_GPADC_BAD_VALUE)
    {
        return;
    }
	if(ui_get_sys_status() == APP_UI_SYS_PRODUCT_TEST)
	{
		// test mode
		return;
	}
	if(first_check == 0)
	{
		uint8_t i;
		
		first_check = 1;
		
		if((volt < battery_ntc_high_to_low_table[3]) || (volt > battery_ntc_low_to_high_table[0]))
		{
			BATTERY_ZT203_TRACE("first temperature too high or too low %d!!!!!!",volt);
			app_shutdown();
			return;
		}
		for(i = 0; i < 4;i++)
		{
			if(volt > battery_ntc_low_to_high_table[i])
			{
				break;
			}
		}
		ntc_status = (BATTERY_NTC_STATUS )i;
		BATTERY_ZT203_TRACE("ntc_status %d %d",volt,ntc_status);
	}
#if BATTERY_TEST_TEMPERATURE
	{	
		const char* string;
		uint16_t i;
		
		for(i=0;i<sizeof(battery_ntc_table)/sizeof(NTC_TALBE);i++)
		{
			if(volt > battery_ntc_table[i].vot)
			{
				string = battery_ntc_table[i].temperature;
				break;
			}
			else
			{
				string = "error";
			}
		}
		
		BATTERY_ZT203_TRACE("%d %d %d %s^C", volt,ntc_status, charging_status, string);	

	}
#endif
	if((charging_status == CHARGING_PLUG_IN))
	{
		battery_zt203_charging_control(volt);
		app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
	}
	else if(charging_status == CHARGING_PLUG_OUT)
	{
		if((volt < battery_ntc_low_to_high_table[3]) || (volt > battery_ntc_high_to_low_table[0]))
		{
			BATTERY_ZT203_TRACE("temperature too high or too low %d!!!!!!",volt);
			app_shutdown();
			return;
		}
	}
	
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_ntc_enable_cfg.pin, HAL_GPIO_DIR_OUT, 0);
}

static void battery_read_ntc_timehandler(void const *param)
{
    hal_gpadc_open(HAL_GPADC_CHAN_2, HAL_GPADC_ATP_ONESHOT, battery_ntc_capture_irqhandler);
}

static void battery_ntc_capture_start(void)
{
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_ntc_enable_cfg.pin, HAL_GPIO_DIR_OUT, 1);
	osTimerStop(battery_ntc_timer);
	osTimerStart(battery_ntc_timer, 200);	//200ms
}


static void battery_zt203_enable_charging(void)
{
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_enable_cfg.pin, HAL_GPIO_DIR_OUT, 0);
}

static void battery_zt203_disable_charging(void)
{
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_enable_cfg.pin, HAL_GPIO_DIR_OUT, 1);
}

static void battery_zt203_enable_quick_charging(void)
{
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_control_cfg.pin, HAL_GPIO_DIR_OUT, 1);
}

static void battery_zt203_enable_slowly_charging(void)
{
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_control_cfg.pin, HAL_GPIO_DIR_OUT, 0);
}

static void battery_check_timehandler(void const *param)
{
	BATTERY_ZT203_TRACE(" %08x", hal_sys_timer_get());
	// 18000 = 1 second
	battery_ntc_capture_start();
	battery_zt203_charging_full_check();
	osTimerStart(battery_check_timer, 60000);	//1 min
}

static void battery_zt203_charging_control(HAL_GPADC_MV_T volt)
{
	/*charging ntc check*/
	
	if(ntc_status > NTC_TEMPERATURE_COLD && (volt > battery_ntc_low_to_high_table[ntc_status - 1]))
	{
		BATTERY_ZT203_TRACE(" 1 %d %d", volt,battery_ntc_high_to_low_table[ntc_status-1]);
		if(volt > battery_ntc_high_to_low_table[ntc_status-1])
		{
			uint8_t ntc_status_temp = ntc_status;

			ntc_status_temp--;
			
			ntc_status = (BATTERY_NTC_STATUS )ntc_status_temp;
			BATTERY_ZT203_TRACE(" 2 %d",ntc_status);
			goto battery_zt203_charging_excute;
		}
	}
	if(ntc_status < NTC_TEMPERATURE_HOT)
	{
		BATTERY_ZT203_TRACE(" 3 %d %d", volt,battery_ntc_low_to_high_table[ntc_status]);
		if(volt < battery_ntc_low_to_high_table[ntc_status])
		{
			uint8_t ntc_status_temp = ntc_status;

			ntc_status_temp++;
			
			ntc_status = (BATTERY_NTC_STATUS )ntc_status_temp;
			BATTERY_ZT203_TRACE(" 4 %d",ntc_status);
		}
	}

battery_zt203_charging_excute:

	BATTERY_ZT203_TRACE(" 5 %d",ntc_status);

	switch(ntc_status)
	{
		case NTC_TEMPERATURE_NORMAL:
		{
			if(volt < 341)
			{// more than 45° slow charging
				battery_zt203_enable_slowly_charging();
			}
			else
			{
				battery_zt203_enable_quick_charging();
			}
			battery_zt203_enable_charging();
		}
		break;

		case NTC_TEMPERATURE_LOW:
		{
			battery_zt203_enable_slowly_charging();
			battery_zt203_enable_charging();
		}
		break;
		
		default:
		{
			battery_zt203_disable_charging();
		}
		break;
	}
}


static void battery_zt203_enable_full(void)
{   
	hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_full_int_cfg.pin, HAL_GPIO_DIR_IN, 0);
}

uint8_t battery_zt203_status(void)
{
	return charging_status;
}

void battery_zt203_plugin(void)
{
	charging_status = CHARGING_PLUG_IN;
	app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
	battery_ntc_capture_start();	//read ntc value

	BATTERY_ZT203_TRACE("charging_status: %s", charging_status == CHARGING_PLUG_OUT ? "PLUGOUT" : "PLUGIN");
}

static bool battery_zt203_check_first_power_on(void)
{
	static bool first_power_on_flag = true;
	uint32_t current_sys_timer;

	if(first_power_on_flag == false)
	{
		return false;
	}
	
	if(battery_power_on_sys_timer == 0)
	{
		first_power_on_flag = false;
		return true;
	}
	
	current_sys_timer = hal_sys_timer_get();
	BATTERY_ZT203_TRACE("sys_timer %d %d",battery_power_on_sys_timer,current_sys_timer);
	
	if(current_sys_timer > battery_power_on_sys_timer)
	{
		if((current_sys_timer - battery_power_on_sys_timer) < 180000)
		{
			// 18000 = 1 second
			//do not power off in 10s when first power on
			first_power_on_flag = false;
			return true;
		}
	}
	first_power_on_flag = false;
	
	return false;
}

bool battery_zt203_plugout(void)
{
	bool ret = false;

	if(app_ibrt_current_era_status() == IBRT_CLOSE_BOX_EVENT)
	{//charge full power off
		if(!battery_zt203_check_first_power_on())
		{
			BATTERY_ZT203_TRACE("in box closed , shut down");
			app_shutdown(); 
			ui_device_status.ear_power_off_inbox_fg = true;
			ret = true;
		}
	}
	

	charging_status = CHARGING_PLUG_OUT;
	
	BATTERY_ZT203_TRACE("charging_status: %s %d", charging_status == CHARGING_PLUG_OUT ? "PLUGOUT" : "PLUGIN",app_ibrt_current_era_status());

	battery_charging_full_count = 0;

	return ret;
}


void battery_zt203_init(void)
{
	BATTERY_ZT203_TRACE("!!!");
	
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&battery_full_int_cfg, 1);
	battery_zt203_enable_full();

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&battery_enable_cfg, 1);
	battery_zt203_enable_charging();
	
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&battery_control_cfg, 1);
	battery_zt203_enable_quick_charging();

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&battery_ntc_enable_cfg, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)battery_ntc_enable_cfg.pin, HAL_GPIO_DIR_OUT, 0);

	battery_ntc_timer = osTimerCreate(osTimer(BATTERY_NTC_TIMER), osTimerOnce, NULL);
	battery_check_timer = osTimerCreate(osTimer(BATTERY_CHECK_TIMER), osTimerOnce, NULL);

	osTimerStart(battery_check_timer, 60000);	//1 min

	//battery_ntc_capture_start();

	battery_power_on_sys_timer = hal_sys_timer_get();
	BATTERY_ZT203_TRACE("sys_timer %d",battery_power_on_sys_timer);
}

void battery_zt203_uninit(void)
{
	app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
	app_battery_stop();
	app_battery_disable_pmu_irq();
	app_battery_close();
	ntc_capture_stop();

	osTimerStop(battery_ntc_timer);
	osTimerStop(battery_check_timer);
}


#endif
