#ifdef __APP_BATTERY_USER__
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "tgt_hardware.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "app_thread.h"
#include "hwtimer_list.h"
#include "app_thread.h"
#include "pmu.h"
#include "apps.h"
#include "zowee_section.h"
#include "app_ibrt_ui.h"
#include "ui_event_manage.h"
#include "zowee_config.h"

#include "battery_hal_if.h"
#include "ui_event_battery.h"
#include "app_battery.h"
#include "battery_zt203.h"
#include "battery_zt102.h"
#include "ui_event_bt.h"
#include "app_tws_ibrt.h"
#include "ui_event_product_test.h"
#include "app_ibrt_if.h"


#define BAT_CHARGING_TIME 			5	// 5 min



static uint8_t bat_charging_count = 0;

#ifdef __BATTERY_LOWPOWER_PROMPT_FIXED__
static uint8_t lowpower_prompt_count = 0;
#endif


static uint8_t low_bat_check_count = LOW_BAT_CHECK_TIME;

static uint16_t battery_buff[UI_BATTERY_BUFF_LENGTH] = {0};

static void low_power_off_timehandler(void const *param);
osTimerDef(LOW_POWER_OFF_EVENT_TIMER, low_power_off_timehandler);
static osTimerId low_power_off_timer = NULL;


static void low_power_off_timehandler(void const *param)
{
	UI_BATTERY_TRACE("%d %d %d", app_tws_ibrt_tws_link_connected(),app_tws_ibrt_mobile_link_connected(),app_tws_ibrt_slave_ibrt_link_connected());
	if(app_ibrt_ui_can_tws_switch())
	{
		app_ibrt_ui_tws_switch();
	}
	else
	{
		app_shutdown();
		osTimerStop(low_power_off_timer); //
	}

}

static void ui_manage_battery_reload_buff(void)
{
	uint16_t current_voltage,i;
	
	current_voltage = app_battery_get_current_voltage();
	UI_BATTERY_TRACE("%d", current_voltage);

	for(i=0;i<UI_BATTERY_BUFF_LENGTH;i++)
	{
		battery_buff[i] = current_voltage;
	}
}

static void ui_manage_battery_refresh_table(void)
{
	uint16_t current_voltage,i;
	
	current_voltage = app_battery_get_current_voltage();
	UI_BATTERY_TRACE("%d", current_voltage);

	for(i=0;i<UI_BATTERY_BUFF_LENGTH - 1;i++)
	{
		battery_buff[i] = battery_buff[i+1];
	}
	battery_buff[i] = current_voltage;

}

static void ui_mamage_battery_report_level(void)
{
	uint8_t battery_level = 0;
	
	if(app_tws_ibrt_tws_link_connected())
	{
		if(ui_device_status.local_status.battery_level > ui_device_status.peer_status.battery_level)
		{
			battery_level = ui_device_status.peer_status.battery_level;
		}
		else
		{
			battery_level = ui_device_status.local_status.battery_level;
		}
	}
	else
	{
		battery_level = ui_device_status.local_status.battery_level;
		
	}

	app_status_battery_report(battery_level);

	UI_BATTERY_TRACE("local_status.battery_level:%d %d %d",app_tws_ibrt_tws_link_connected(), ui_device_status.local_status.battery_level,ui_device_status.peer_status.battery_level);
}


static void ui_manage_battery_set_level_init(void)
{
	uint16_t average_voltage, i;
	uint32_t total_voltage = 0;
	uint8_t current_level, calcualte_level = 9;

	for(i=0; i<UI_BATTERY_BUFF_LENGTH; i++)
	{
		total_voltage += battery_buff[i];
	}

	average_voltage = total_voltage/UI_BATTERY_BUFF_LENGTH;
	current_level = ui_device_status.local_status.battery_level;

	UI_BATTERY_TRACE("ear_status: %d, current_level: %d, average_voltage: %d !!!!!!", ui_device_status.local_status.ear_status, current_level, average_voltage);
	if((ui_device_status.local_status.ear_status == UI_DEVICE_BOX_OPEN) || (ui_device_status.local_status.ear_status == UI_DEVICE_BOX_CLOSED))
	{
		for(i=0; i<10; i++)
		{
			if(average_voltage < battery_charging_level_table[i])
			{
				calcualte_level = i;
				break;
			}
		}

		UI_BATTERY_TRACE("charging init, calcualte_level: %d !!!!!!", calcualte_level);
		if(current_level == 255)
		{
			current_level = calcualte_level;
			app_battery_set_level(current_level);
		}

		low_bat_check_count = LOW_BAT_CHECK_TIME;
	}
	else
	{
		for(i=0; i<10; i++)
		{
			if(average_voltage < battery_discharging_level_table[i])
			{
				calcualte_level = i;
				break;
			}
		}

		UI_BATTERY_TRACE("discharging init, calcualte_level: %d !!!!!!", calcualte_level);
		if(current_level == 255)
		{
			current_level = calcualte_level;
			app_battery_set_level(current_level);
		}
		
		if((current_level == 0) && (low_bat_check_count == LOW_BAT_CHECK_TIME))
		{	
			low_bat_check_count = 1;
			UI_BATTERY_TRACE("APP_STATUS_INDICATION_CHARGENEED !!!");
			app_voice_report(APP_STATUS_INDICATION_CHARGENEED, 0);
		}	
	}

	UI_BATTERY_TRACE("current_level: %d !!!!!!", current_level);
	ui_device_status.local_status.battery_level = current_level;
	
	if(app_tws_ibrt_tws_link_connected())
	{
		zowee_tws_send_local_info_to_peer();
	}

	ui_mamage_battery_report_level();
}

static void ui_manage_battery_calcualte_level(void)
{
	uint16_t average_voltage, i;
	uint32_t total_voltage = 0;
	uint8_t current_level, calcualte_level = 9;

	for(i=0; i<UI_BATTERY_BUFF_LENGTH; i++)
	{
		total_voltage += battery_buff[i];
	}
	
	average_voltage = total_voltage/UI_BATTERY_BUFF_LENGTH;
	current_level = app_battery_current_level();

	UI_BATTERY_TRACE("ear_status: %d, current_level: %d, average_voltage: %d !!!!!!", ui_device_status.local_status.ear_status, current_level, average_voltage);
	if((ui_device_status.local_status.ear_status == UI_DEVICE_BOX_OPEN) || (ui_device_status.local_status.ear_status == UI_DEVICE_BOX_CLOSED))
	{
		for(i=0; i<10; i++)
		{
			if(average_voltage < battery_charging_level_table[i])
			{
				calcualte_level = i;
				break;
			}
		}

		UI_BATTERY_TRACE("charging calcualte, calcualte_level: %d !!!!!!", calcualte_level);
		if(calcualte_level > current_level)
		{
			bat_charging_count++;
			UI_BATTERY_TRACE("charging calcualte, count: %d ", bat_charging_count);
			if(bat_charging_count >= BAT_CHARGING_TIME)
			{
				bat_charging_count = 0;
				current_level++;
				app_battery_set_level(current_level);
				ui_device_status.local_status.battery_level = current_level;
				if(app_tws_ibrt_tws_link_connected())
				{
					zowee_tws_send_local_info_to_peer();
				}
			}
		}

		low_bat_check_count = LOW_BAT_CHECK_TIME;
	}
	else
	{
		for(i=0; i<10; i++)
		{
			if(average_voltage < battery_discharging_level_table[i])
			{
				calcualte_level = i;
				break;
			}
		}

		UI_BATTERY_TRACE("discharging calcualte, calcualte_level: %d !!!!!!", calcualte_level);

		if(calcualte_level < current_level)
		{
			current_level--;
			app_battery_set_level(current_level);
			ui_device_status.local_status.battery_level = current_level;
			if(app_tws_ibrt_tws_link_connected())
			{
				zowee_tws_send_local_info_to_peer();
			}
		}

		bat_charging_count = 0; 
	}

	UI_BATTERY_TRACE("current_level: %d !!!!!!", current_level);

	ui_mamage_battery_report_level();
}

/*
static void ui_manage_battery_switch_tws_check(void)
{
	if((ui_device_status.local_status.battery_level != 0xff) && (ui_device_status.peer_status.battery_level != 0xff))
	{
		if(app_tws_ibrt_tws_link_connected() && app_tws_ibrt_mobile_link_connected())
		{
			if((ui_device_status.local_status.battery_level) < ui_device_status.peer_status.battery_level)
			{
				app_ui_event_process(UI_EVENT_BT,0,ZOWEE_BT_TWS_SWITCH,0,0);
			}
		}
	}
}
*/

static void ui_manage_battery_low_power_check(void)
{
	uint8_t check_level = 0;
	uint8_t status = PMU_CHARGER_PLUGOUT;
	
	ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();
	
	status = pmu_charger_get_status();
	if(status == PMU_CHARGER_PLUGIN)
	{
		return ;
	}

	if(app_tws_ibrt_tws_link_connected())
	{
		if((ui_device_status.local_status.battery_level > 0) && (ui_device_status.peer_status.battery_level > 0))
		{
			return ;
		}
		else if((ui_device_status.local_status.battery_level == 0) && (ui_device_status.peer_status.battery_level == 0))
		{
			if(p_ibrt_ctrl->current_role == IBRT_MASTER)
			{
				return ;
			}
		}		
	}
	else
	{
		if(ui_device_status.local_status.battery_level > 0)
		{
			return ;
		}
	}
	
	check_level = ui_device_status.local_status.battery_level;

	UI_BATTERY_TRACE("check_level:%d, low_bat_check_count:%d !!!", check_level, low_bat_check_count);
#ifdef __BATTERY_LOWPOWER_PROMPT_FIXED__
	if((battery_buff[UI_BATTERY_BUFF_LENGTH - 1] < BATTERY_POWER_DOWN_VOLTAGE) && (lowpower_prompt_count >= BATTERY_LOWPOWER_PROMPT_MAX))
#else
	if(battery_buff[UI_BATTERY_BUFF_LENGTH - 1] < BATTERY_POWER_DOWN_VOLTAGE)
#endif
	{
		UI_BATTERY_TRACE("UI_EVENT_SHUT_DOWN_POWER !!! %d ", battery_buff[UI_BATTERY_BUFF_LENGTH - 1]);
		app_ui_event_process(UI_EVENT_CHARGING, 0, UI_EVENT_SHUT_DOWN_POWER, 0, 0);
		return; // power off
	}

	if(check_level == 0)	
	{	
		low_bat_check_count++;
		if(low_bat_check_count > LOW_BAT_CHECK_TIME)	// µÕµÁ10∑÷÷”Ã·–—“ª¥Œ
		{	
			low_bat_check_count = 1;
			UI_BATTERY_TRACE("APP_STATUS_INDICATION_CHARGENEED !!!");
			app_voice_report(APP_STATUS_INDICATION_CHARGENEED, 0);
#ifdef __BATTERY_LOWPOWER_PROMPT_FIXED__
			lowpower_prompt_count++;
#endif			
		}
	}
	else
	{
		low_bat_check_count = LOW_BAT_CHECK_TIME;
#ifdef __BATTERY_LOWPOWER_PROMPT_FIXED__
		lowpower_prompt_count = 0;
#endif
	}
	
}

static void battery_monitor_timehandler(void)
{
	UI_BATTERY_TRACE("battery_buff[0] %d !!!",battery_buff[0]);
	if(battery_buff[0] == 0x00)
	{//init
		ui_manage_battery_reload_buff();
		ui_manage_battery_set_level_init();
		app_battery_start(APP_BATTERY_MEASURE_PERIODIC_NORMAL);
		return;
	}

	ui_manage_battery_refresh_table();
	ui_manage_battery_calcualte_level();
	//ui_manage_battery_switch_tws_check();
	ui_manage_battery_low_power_check();
	app_battery_start(APP_BATTERY_MEASURE_PERIODIC_NORMAL);
}

void ui_manage_battery_init(void)
{
	uint8_t cnt = 0, status;

	app_ibrt_if_ui_t* ibrt_if_ui_ctx = app_ibrt_if_ui_get_ctx();
	
	ui_device_status.local_status.battery_level = 0xff;
	ui_device_status.peer_status.battery_level = 0xff;

	if(low_power_off_timer == NULL)
		low_power_off_timer = osTimerCreate(osTimer(LOW_POWER_OFF_EVENT_TIMER), osTimerPeriodic, NULL);

    do
    {
        status = pmu_charger_get_status();
        if (status == PMU_CHARGER_PLUGIN)
        {
            break;
        }
        osDelay(20);
    }
    while(cnt++ < 5);

	//ui_device_status.local_status.battery_level = (UI_DEVICE_STATUS_E)zowee_section_get_battery_level_state();
	//UI_BATTERY_TRACE("battery current level: %d", ui_device_status.local_status.battery_level);
	//if(ui_device_status.local_status.battery_level != 255)
	//{
	//	app_battery_set_level(ui_device_status.local_status.battery_level);
	//	zowee_section_set_battery_level_state(0xFF);
	//}

	UI_BATTERY_TRACE("box state %d", ibrt_if_ui_ctx->box_state);
	ui_device_status.local_status.ear_status = (UI_DEVICE_STATUS_E)ibrt_if_ui_ctx->box_state;
	app_ibrt_init_ear_status((uint8_t )(ui_device_status.local_status.ear_status));
	UI_BATTERY_TRACE("status: %s %d", status == PMU_CHARGER_PLUGOUT ? "PLUGOUT" : "PLUGIN",ui_device_status.local_status.ear_status);
	if(status == PMU_CHARGER_PLUGIN)
	{
		UI_DEVICE_STATUS_E flash_ear_status;
		
		flash_ear_status = (UI_DEVICE_STATUS_E)zowee_section_get_box_state();;
		if(flash_ear_status==UI_DEVICE_BOX_CLOSED)//ÂÖÖÁîµÁõíÂÖ≥Èó≠‰∏îÂ§ç‰ΩçÊó∂ÔºåËÄ≥Êú∫Ê≤°Ê£ÄÊµãÂà∞5v
		{
			app_ui_event_process(UI_EVENT_STATUS, 0, UI_EVENT_STATUS_CLOSE_BOX, 0, 0);	
			ui_device_status.local_status.ear_status = UI_DEVICE_BOX_CLOSED;
			battery_hal_if_plug_in();
 		}
		else
		{
			app_ui_event_process(UI_EVENT_STATUS, 0, UI_EVENT_STATUS_OPEN_BOX, 0, 0);	
			ui_device_status.local_status.ear_status = UI_DEVICE_BOX_OPEN;
			battery_hal_if_plug_in();
		}
	}
	else
	{
		app_ibrt_if_ui_t* ibrt_if_ui_ctx = app_ibrt_if_ui_get_ctx();

		
		UI_BATTERY_TRACE("ear status: %d",ibrt_if_ui_ctx->box_state); 
		
		if(ibrt_if_ui_ctx->box_state == IBRT_IN_BOX_CLOSED)
		{
			app_ibrt_if_event_entry(IBRT_OPEN_BOX_EVENT);	
		}
		app_ui_event_process(UI_EVENT_CHARGING, 0, UI_EVENT_CHARGING_PLUG_OUT, 0, 0);		
	}
	
	app_battery_enable_pmu_irq(status);

}

void ui_manage_battery_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	UI_BATTERY_TRACE("%d",msg_body->id);
	switch(msg_body->id)
	{
		case UI_EVENT_CHARGING_PLUG_IN:
		{
			if(product_test_stauts.dut_flag == true)
			{
				break;
			}
			if((ui_device_status.local_status.ear_status != UI_DEVICE_BOX_OPEN) 
				&& (ui_device_status.local_status.ear_status != UI_DEVICE_BOX_CLOSED))
			{
				UI_BATTERY_TRACE("now earphone in box");
				app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_PUT_IN,0,0);
				//app_battery_start(APP_BATTERY_MEASURE_PERIODIC_FAST);
				//battery_buff[0] = 0;
			}

			if((battery_hal_if_charging_status() != CHARGING_PLUG_IN) && (battery_hal_if_charging_status() != CHARGING_FULL))
				battery_hal_if_plug_in();
		}
		break;

		case UI_EVENT_CHARGING_PLUG_OUT:
		{
			if(product_test_stauts.dut_flag == true)
			{
				break;
			}
			if((battery_hal_if_charging_status() != CHARGING_PLUG_OUT))
			{
				if(battery_hal_if_plug_out())
				{
					// shut down, do not need reconnect to phone
					return;
				}
			}

			if((ui_device_status.local_status.ear_status != UI_DEVICE_OUT_BOX))
			{

				ui_start_disconncet_shutdown_timer();
				UI_BATTERY_TRACE("now earphone out box");
				app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_FETCH_OUT,0,0);
				app_battery_start(APP_BATTERY_MEASURE_PERIODIC_FAST);
				//battery_buff[0] = 0;
 			}

		}
		break;

		case UI_EVENT_BATTERY_LEVEL_CHECK:
		{			
			battery_monitor_timehandler();
		}
		break;

		case UI_EVENT_BATTERY_LEVEL_REPROT:
		{
			if(battery_buff[0] != 0)
			{
				ui_manage_battery_calcualte_level();
			}
			
			if(app_tws_ibrt_tws_link_connected())
			{
				zowee_tws_send_local_info_to_peer();
			}
		}
		break;
		
		case UI_EVENT_SHUT_DOWN_POWER:
		{
			osTimerStop(low_power_off_timer); //
			osTimerStart(low_power_off_timer, 1000);	//200ms
		}
		break;
		default:

		break;
	}
}

#endif
