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
#include "app_utils.h"
#include "apps.h"	

#include "ui_event_manage.h"
#include "zowee_config.h"
#include "gesture_hal_if.h"
#include "wear_hal_if.h"
#ifdef __APP_VBUS_ENABLE__
#include "vbus_hal_if.h"
#include "ui_event_vbus.h"
#include "vbus_uart.h"
#endif
#include "battery_hal_if.h"
#include "ui_event_gesture.h"
#include "ui_event_wear_status.h"
#include "ui_event_scpi.h"
#include "app_spp_anc.h"
#include "ui_event_a2dp.h"
#include "ui_event_battery.h"
#include "ui_event_bt.h"
#include "ui_event_hfp.h"
#include "app_anc.h"
#include "zowee_section.h"
#include "nvrecord_env.h"

//bes paltform
#include "app_ibrt_if.h"
#include "ui_event_product_test.h"
#include "hal_bootmode.h"

//function define
static void ui_manage_thread(void const *argument);

uint32_t test_count_timer_op=0;

UI_DEVICE_TOTAL_STATUS_T ui_device_status;

extern "C" void app_anc_status_sync(uint8_t status);

//global variable define
#define	IBRT_EVENT_STATUS_COUNT		6
static void ibrt_status_timehandler(void const *param);
static const ibrt_event_type last_ibrt_event_group[IBRT_EVENT_STATUS_COUNT] = {IBRT_CLOSE_BOX_EVENT, IBRT_OPEN_BOX_EVENT,IBRT_FETCH_OUT_EVENT,IBRT_WEAR_UP_EVENT,IBRT_WEAR_DOWN_EVENT,IBRT_PUT_IN_EVENT};
static ibrt_event_type current_ibrt_status = IBRT_NONE_EVENT;
static ibrt_event_type purpose_ibrt_status = IBRT_NONE_EVENT;
osTimerDef(IBRT_STATUS_TIMER, ibrt_status_timehandler);
static osTimerId ibrt_status_timer = NULL;

UI_DEVICE_DISCONNCET_STATUS_T ui_disconncet_status;

static osTimerId ibrt_dis_shutdown_timer = NULL;
static void ibrt_dis_shutdown_timehandler(void const *param);
osTimerDef(IBRT_DIS_SHUTDOWN_TIMER, (void (*)(void const *))ibrt_dis_shutdown_timehandler);




#define	UI_MANAGE_MAILBOX_MAX	60

static osThreadId ui_manage_tid;
osThreadDef(ui_manage_thread, osPriorityHigh, 1, (1024 * 8), "ui_manage_thread");
osMailQDef (ui_manage_mailbox, UI_MANAGE_MAILBOX_MAX, UI_MANAGE_MESSAGE_BLOCK);
static osMailQId ui_manage_mailbox;
static uint8_t ui_manage_mailbox_cnt = 0;

static void ui_manage_product_test_init(void)
{
#ifdef	__SCPI_TEST__
	app_user_spp_scpi_init();
#endif
}

static void ui_device_status_init(void)
{
	memset((void *)&ui_device_status, 0x00, sizeof(ui_device_status));
	zowee_anc_status_init();
}

static void ui_manage_perpheral_init()
{
    APP_UI_TRACE();

	ui_device_status_init();

#ifdef __APP_GESTURE_ENABLE__

#ifndef __TOUCH_IQS269A__
	gesture_hal_if_init();
#endif

#endif

#ifdef __APP_WEAR_ENABLE__
	wear_hal_if_init();
#endif

#ifdef __APP_VBUS_ENABLE__
	ui_manage_vbus_init_timer();
	vbus_hal_if_init();
	//vbus rx done to analysis callback 
#endif

	ui_manage_battery_init();
}


static uint8_t ibrt_get_status_temp(ibrt_event_type status)
{
	uint8_t i;
	
	for(i=0;i<IBRT_EVENT_STATUS_COUNT;i++)
	{
		if(status == last_ibrt_event_group[i])
		{
			return i;
		}
	}

	return 0;
}

void app_ibrt_init_ear_status(uint8_t status)
{	
	switch(status)
	{
		case UI_DEVICE_BOX_CLOSED:
			purpose_ibrt_status = IBRT_CLOSE_BOX_EVENT;
			current_ibrt_status = IBRT_CLOSE_BOX_EVENT;
		break;

		case UI_DEVICE_BOX_OPEN:
			purpose_ibrt_status = IBRT_OPEN_BOX_EVENT;
			current_ibrt_status = IBRT_OPEN_BOX_EVENT;
			
		break;

		case UI_DEVICE_OUT_BOX:
			purpose_ibrt_status = IBRT_FETCH_OUT_EVENT;
			current_ibrt_status = IBRT_FETCH_OUT_EVENT;
			
		break;

		case UI_DEVICE_WARED:
			purpose_ibrt_status = IBRT_WEAR_UP_EVENT;
			current_ibrt_status = IBRT_WEAR_UP_EVENT;
			
		break;
		
		default:

		break;
	}
}

uint8_t app_ibrt_current_era_status(void)
{
	if(purpose_ibrt_status == current_ibrt_status)
	{
		return current_ibrt_status;
	}
	return IBRT_NONE_EVENT;
}

static bool app_ibrt_entry_status(void)  

{
	switch(purpose_ibrt_status)
	{
		case IBRT_CLOSE_BOX_EVENT:
		{
			if(current_ibrt_status == IBRT_OPEN_BOX_EVENT || current_ibrt_status == IBRT_PUT_IN_EVENT)
			{
				current_ibrt_status = purpose_ibrt_status;	
				return true;
			}
		}
		break;

		case IBRT_OPEN_BOX_EVENT:
		{
			if(current_ibrt_status == IBRT_CLOSE_BOX_EVENT)
			{
				current_ibrt_status = purpose_ibrt_status;	
				return true;
			}
		}
		break;

		case IBRT_FETCH_OUT_EVENT:
		{
			if(current_ibrt_status == IBRT_OPEN_BOX_EVENT || current_ibrt_status == IBRT_PUT_IN_EVENT)
			{
				current_ibrt_status = purpose_ibrt_status;	
				return true;
			}
		}
		break;

		case IBRT_WEAR_UP_EVENT:
		{
			if(current_ibrt_status == IBRT_WEAR_DOWN_EVENT || current_ibrt_status == IBRT_FETCH_OUT_EVENT)
			{
				current_ibrt_status = purpose_ibrt_status;	
				return true;
			}
		}
		break;
		
		case IBRT_WEAR_DOWN_EVENT:
		{
			if(current_ibrt_status == IBRT_WEAR_UP_EVENT)
			{
				current_ibrt_status = purpose_ibrt_status;	
				return true;
			}
		}
		break;

		case IBRT_PUT_IN_EVENT:
		{
			if(current_ibrt_status == IBRT_WEAR_DOWN_EVENT || current_ibrt_status == IBRT_FETCH_OUT_EVENT)
			{
				current_ibrt_status = purpose_ibrt_status;	
				return true;
			}
		}
		break;

		default:
		
		break;
	}

	return false;	
}

static void app_correction_ibrt_close_box(void)
{
	if(purpose_ibrt_status != IBRT_CLOSE_BOX_EVENT)
	{
		return;
	}
	
	switch(current_ibrt_status)
	{
		case IBRT_WEAR_UP_EVENT:
		{
			current_ibrt_status = IBRT_WEAR_DOWN_EVENT;
		}
		break;

		case IBRT_WEAR_DOWN_EVENT:
		{
			current_ibrt_status = IBRT_PUT_IN_EVENT;
		}
		break;

		case IBRT_PUT_IN_EVENT:
		{
			current_ibrt_status = IBRT_CLOSE_BOX_EVENT;
		}
		break;

		case IBRT_FETCH_OUT_EVENT:
		{
			current_ibrt_status = IBRT_PUT_IN_EVENT;
		}
		break;
		

		default:
		return;
	}	
	app_ibrt_if_event_entry(current_ibrt_status);
}

static void app_correction_ibrt_weared(void)
{
	if((purpose_ibrt_status != IBRT_WEAR_DOWN_EVENT) && (purpose_ibrt_status != IBRT_WEAR_UP_EVENT))
	{
		return;
	}
	
	switch(current_ibrt_status)
	{
		case IBRT_PUT_IN_EVENT:
		case IBRT_OPEN_BOX_EVENT:
		{
			current_ibrt_status = IBRT_FETCH_OUT_EVENT;
		}
		break;

		case IBRT_FETCH_OUT_EVENT:
		{
			current_ibrt_status = IBRT_WEAR_UP_EVENT;
		}
		break;
		
		case IBRT_CLOSE_BOX_EVENT:
		{
			current_ibrt_status = IBRT_OPEN_BOX_EVENT;
		}
		break;

		default:
		return;
	}	

	app_ibrt_if_event_entry(current_ibrt_status);
}

static void app_correction_box_open(void)
{
	if((purpose_ibrt_status != IBRT_PUT_IN_EVENT) && (purpose_ibrt_status != IBRT_OPEN_BOX_EVENT))
	{
		return;
	}
	
	switch(current_ibrt_status)
	{
		case IBRT_WEAR_UP_EVENT:
		{
			current_ibrt_status = IBRT_WEAR_DOWN_EVENT;
		}
		break;

		case IBRT_WEAR_DOWN_EVENT:
		{
			current_ibrt_status = IBRT_PUT_IN_EVENT;
		}
		break;
		default:
		return;
	}	
	app_ibrt_if_event_entry(current_ibrt_status);
}

static void app_correction_box_fetch_out(void)
{
	if(purpose_ibrt_status != IBRT_FETCH_OUT_EVENT)
	{
		return;
	}
	
	switch(current_ibrt_status)
	{
		case IBRT_WEAR_UP_EVENT:
		{
			current_ibrt_status = IBRT_WEAR_DOWN_EVENT;
		}
		break;

		case IBRT_WEAR_DOWN_EVENT:
		{
			current_ibrt_status = IBRT_PUT_IN_EVENT;
		}
		break;

		case IBRT_CLOSE_BOX_EVENT:
		{
			current_ibrt_status = IBRT_OPEN_BOX_EVENT;
		}
		break;
		
		default:
		return;
	}	
	
	app_ibrt_if_event_entry(current_ibrt_status);
}

static void app_correction_ibrt_status(void)
{
	app_correction_ibrt_close_box();
	app_correction_ibrt_weared();
	app_correction_box_open();
	app_correction_box_fetch_out();
}

static void app_ibrt_status_not_need_change(void)
{
	if((current_ibrt_status == IBRT_PUT_IN_EVENT) && (purpose_ibrt_status == IBRT_OPEN_BOX_EVENT))
	{
		osTimerStop(ibrt_status_timer); //
	}
	if((current_ibrt_status == IBRT_FETCH_OUT_EVENT) && (purpose_ibrt_status == IBRT_OPEN_BOX_EVENT))
	{
		osTimerStop(ibrt_status_timer); //
	}
}

static void ibrt_status_timehandler(void const *param)
{
	uint8_t current_status_temp,purpose_status_temp;

	
	APP_UI_TRACE(" %d %d",current_ibrt_status,purpose_ibrt_status);
	if(current_ibrt_status == IBRT_NONE_EVENT)
	{
		current_ibrt_status = purpose_ibrt_status;
		app_ibrt_if_event_entry(current_ibrt_status);
		return;
	}
	if(current_ibrt_status != purpose_ibrt_status)
	{
		if(app_ibrt_entry_status() == true)
		{
			app_ibrt_if_event_entry(current_ibrt_status);
			return;
		}
		
		current_status_temp = ibrt_get_status_temp(current_ibrt_status);
		purpose_status_temp = ibrt_get_status_temp(purpose_ibrt_status);

		if(current_status_temp < purpose_status_temp)
		{
			current_status_temp++;
			current_ibrt_status = last_ibrt_event_group[current_status_temp];
			app_ibrt_if_event_entry(last_ibrt_event_group[current_status_temp]);
			return;
		}
		else
		{
			app_correction_ibrt_status();
		}

		app_ibrt_status_not_need_change();
	}
	else
	{
		osTimerStop(ibrt_status_timer); //
	}
}

void ui_set_disconnect_init(void)
{
	ibrt_dis_shutdown_timer = osTimerCreate(osTimer(IBRT_DIS_SHUTDOWN_TIMER), osTimerOnce, NULL);	
	osTimerStart(ibrt_dis_shutdown_timer, 2000);
}

void ui_start_disconncet_shutdown_timer(void)
{
	APP_UI_TRACE();
	osTimerStop(ibrt_dis_shutdown_timer);
	osTimerStart(ibrt_dis_shutdown_timer, 200);
	ui_disconncet_status.ibrt_dis_timer_flag = 1;
	ui_disconncet_status.ibrt_dis_timer_count = 0;
}

void ui_stop_disconncet_shutdown_timer(void)
{
	APP_UI_TRACE();
	osTimerStop(ibrt_dis_shutdown_timer);
	ui_disconncet_status.ibrt_dis_timer_flag = 0;
	ui_disconncet_status.ibrt_dis_timer_count = 0;
}

//dis shutdown timer
static void ibrt_dis_shutdown_timehandler(void const *param)
{
	ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();
	//APP_UI_TRACE("current role:%d, count:%d, profile exchanged:%d, activeCons:%d ", p_ibrt_ctrl->current_role, ui_disconncet_status.ibrt_dis_timer_count, app_ibrt_ui_is_profile_exchanged(), btif_me_get_activeCons());

	if(ui_disconncet_status.ibrt_dis_timer_count>=AUTO_POWER_OFF_TIME)//	
	{
		osTimerStop(ibrt_dis_shutdown_timer);
		ui_disconncet_status.ibrt_shutdown_flag = true;

		APP_UI_TRACE("SYSTEM_SHUTDOWNXXXX");

		if(app_tws_ibrt_tws_link_connected())
			zowee_ibrt_tws_exchange_info_without_rsp(TWS_SHUTDOWN,NULL,0);
		app_shutdown();
	}
	else if (app_tws_ibrt_tws_link_connected() && (p_ibrt_ctrl->current_role == IBRT_SLAVE))
	{
		APP_UI_TRACE("TWS CONNECT: salve osTimerStop, shutdown_flag:%d", ui_disconncet_status.ibrt_dis_timer_flag);
		osTimerStop(ibrt_dis_shutdown_timer);
		ui_disconncet_status.ibrt_dis_timer_flag = 0;
	}
	else if (app_tws_ibrt_mobile_link_connected() || app_ibrt_ui_is_profile_exchanged() || (btif_me_get_activeCons() == 2))
	{
		APP_UI_TRACE("BT CONNECT osTimerStop, shutdown_flag:%d, profile exchanged:%d, activeCons:%d", ui_disconncet_status.ibrt_dis_timer_flag, app_ibrt_ui_is_profile_exchanged(), btif_me_get_activeCons());
		osTimerStop(ibrt_dis_shutdown_timer);
		ui_disconncet_status.ibrt_dis_timer_flag = 0;
	}
	else if((ui_device_status.local_status.ear_status == UI_DEVICE_BOX_OPEN) || (ui_device_status.local_status.ear_status == UI_DEVICE_BOX_CLOSED))
	{
		osTimerStop(ibrt_dis_shutdown_timer);
		ui_disconncet_status.ibrt_dis_timer_flag = 0;
		APP_UI_TRACE("charging osTimerStop, shutdown_flag:%d ", ui_disconncet_status.ibrt_dis_timer_flag);
	}
	else
	{
		ui_disconncet_status.ibrt_shutdown_flag = false;
		APP_UI_TRACE("count:%d", ui_disconncet_status.ibrt_dis_timer_count);
		ui_disconncet_status.ibrt_dis_timer_count++;
		osTimerStart(ibrt_dis_shutdown_timer, 10000);
	}
	
}


//bes ibrt sm enent change
static void app_zowee_ibrt_if_event_entry(ibrt_event_type event)
{
	APP_UI_TRACE("event:%d, current:%d, purpose:%d", event, current_ibrt_status, purpose_ibrt_status);
	purpose_ibrt_status = event; 
	if(current_ibrt_status == purpose_ibrt_status)
	{
		return;
	}
	osTimerStop(ibrt_status_timer);	//
	osTimerStart(ibrt_status_timer, 200);	//200ms
}

//zowee status event entry api
void ui_manage_status_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{	
	APP_UI_TRACE(": %d", msg_body->id);
	switch(msg_body->id)
	{
		case UI_EVENT_STATUS_OPEN_BOX:
		{
			app_zowee_ibrt_if_event_entry(IBRT_OPEN_BOX_EVENT);
			APP_UI_TRACE("open box !!!");
			APP_UI_TRACE("zowee_section_get_box_state = %d",zowee_section_get_box_state());
			zowee_section_set_box_state(UI_DEVICE_BOX_OPEN);
			ui_device_status.local_status.ear_status = UI_DEVICE_BOX_OPEN;
			if (app_tws_ibrt_tws_link_connected())
				zowee_tws_send_local_info_to_peer();
		}
		break;
		
		case UI_EVENT_STATUS_CLOSE_BOX:
		{
			//app_ui_event_process(UI_EVENT_BT,0,ZOWEE_BT_TWS_SWITCH,0,0);
			app_zowee_ibrt_if_event_entry(IBRT_CLOSE_BOX_EVENT);
			//app_ui_event_process(UI_EVENT_CHARGING, 0, UI_EVENT_CHARGING_ENTRY_LOW_POWER, 0, 0);
			APP_UI_TRACE("colse box !!!");
			APP_UI_TRACE("zowee_section_get_box_state = %d",zowee_section_get_box_state());
			zowee_section_set_box_state(UI_DEVICE_BOX_CLOSED);
			ui_device_status.local_status.ear_status = UI_DEVICE_BOX_CLOSED;
			if (app_tws_ibrt_tws_link_connected())
				zowee_tws_send_local_info_to_peer();

		}
		break;
		
		case UI_EVENT_STATUS_FETCH_OUT:
		{
			//enable gesture and wear	
			#ifdef __APP_WEAR_ENABLE__
			wear_hal_if_enable();
			#endif
			app_zowee_ibrt_if_event_entry(IBRT_FETCH_OUT_EVENT);
			ui_device_status.local_status.fetch_status=UI_EVENT_STATUS_FETCH_OUT;
			ui_device_status.local_status.ear_status = UI_DEVICE_OUT_BOX;	
			if (app_tws_ibrt_tws_link_connected())
				zowee_tws_send_local_info_to_peer();
		}
		break;
		
		case UI_EVENT_STATUS_PUT_IN:
		{
			//disable gesture and wear
			#ifdef __APP_GESTURE_ENABLE__
			#ifdef __TOUCH_GPIOTE__
			gesture_hal_if_disable();
			#endif
			#endif
			#ifdef __APP_WEAR_ENABLE__
			wear_hal_if_disable();
			#endif
			#ifdef __WEAR_CONTROL_ANC__
			zowee_anc_close_when_put_in_box();
			#endif
			app_zowee_ibrt_if_event_entry(IBRT_PUT_IN_EVENT);
			ui_device_status.local_status.fetch_status=UI_EVENT_STATUS_PUT_IN;
			ui_device_status.local_status.ear_status = UI_DEVICE_BOX_OPEN;
			ui_device_status.local_status.wear_status = UI_WEAR_OFF;
			app_ui_event_process(UI_EVENT_A2DP, 0, AVRCP_KEY_PAUSE, 0, 0);
			if (app_tws_ibrt_tws_link_connected())
				zowee_tws_send_local_info_to_peer();
		}
		break;

		case UI_EVENT_STATUS_WEAR_UP:
		{
#ifdef __APP_GESTURE_ENABLE__
#ifdef __TOUCH_GPIOTE__
			gesture_hal_if_enable();
#endif
#endif
		#if defined(__PROJ_NAME_ZT102__)
			if(app_tws_ibrt_mobile_link_connected() || (btif_me_get_activeCons() == 2))
		#endif
			{
				app_zowee_ibrt_if_event_entry(IBRT_WEAR_UP_EVENT);	
			}

			ui_device_status.local_status.wear_status = UI_WEAR_ON;
			if (app_tws_ibrt_tws_link_connected())
				zowee_tws_send_local_info_to_peer();
		}
		break;
		
		case UI_EVENT_STATUS_WEAR_DOWN:
		{
			app_zowee_ibrt_if_event_entry(IBRT_WEAR_DOWN_EVENT);			
			ui_device_status.local_status.wear_status = UI_WEAR_OFF;
			if (app_tws_ibrt_tws_link_connected())
				zowee_tws_send_local_info_to_peer();
		}
		break;
		
		default:

		break;
	}
}

const char *msg_id_str[] =
{
    "[UI_EVENT_STATUS]",
    "[UI_EVENT_GESTURE]",
    "[UI_EVENT_WEAR]",
    "[UI_EVENT_VBUS]",
    "[UI_EVENT_BT]",
    "[UI_EVENT_A2DP]",
    "[UI_EVENT_HFP]",
    "[UI_EVENT_CHARGING]",
    "[UI_EVENT_SCPI]",
};

const char *msg_id2str(UI_EVENT_MANAGE_T msg_id)
{
    const char *str = NULL;

    if (msg_id >= 0 && msg_id <= UI_EVENT_SCPI)
    {
        str = msg_id_str[msg_id];
    }
    else
    {
        str = "[UNKNOWN] MSG";
    }

    return str;
}

static int app_ui_event_handle(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	APP_UI_TRACE("msg_id:%d %s", msg_body->msg_id,msg_id2str(msg_body->msg_id));
	switch(msg_body->msg_id)
	{
		case UI_EVENT_STATUS:
		{
			ui_manage_status_event(msg_body);
		}
		break;
#ifdef __APP_GESTURE_ENABLE__	
		case UI_EVENT_GESTURE:
		{
			ui_manage_gesture_event(msg_body);
		}
		break;
#endif
#ifdef  __APP_WEAR_ENABLE__
		case UI_EVENT_WEAR:
		{
			ui_manage_wear_event(msg_body);
		}
		break;
#endif
#ifdef	__APP_VBUS_ENABLE__
		case UI_EVENT_VBUS:
		{
			ui_manage_vbus_event(msg_body);
		}
		break;
#endif
		case UI_EVENT_BT:
		{
			ui_manage_bt_event(msg_body);
		}
		break;
		case UI_EVENT_A2DP:
		{
			ui_manage_a2dp_event(msg_body);
		}
		break;
		
		case UI_EVENT_HFP:
		{
			ui_manage_hfp_event(msg_body);
		}
		break;
#ifdef __APP_BATTERY_USER__
		case UI_EVENT_CHARGING:
		{
			ui_manage_battery_event(msg_body);
		}
		break;
#endif
#ifdef	__SCPI_TEST__
		case UI_EVENT_SCPI:
		{
			ui_manage_scpi_event(msg_body);
		}
		break;
#endif
		default:
		break;
	}

    return 0;
}

int ui_manage_mailbox_free(UI_MANAGE_MESSAGE_BLOCK *msg_p)
{
    osStatus status;

    status = osMailFree(ui_manage_mailbox, msg_p);
    if (osOK == status) {
        ui_manage_mailbox_cnt--;
    }

    return (int)status;
}

int ui_manage_mailbox_get(UI_MANAGE_MESSAGE_BLOCK **msg_p)
{
    osEvent evt;
    evt = osMailGet(ui_manage_mailbox, osWaitForever);
    if (evt.status == osEventMail) {
        *msg_p = (UI_MANAGE_MESSAGE_BLOCK *)evt.value.p;
        return 0;
    }

    return -1;
}


static void ui_manage_thread(void const *argument)
{
    APP_UI_TRACE();

	app_sysfreq_req(APP_SYSFREQ_USER_UI, APP_SYSFREQ_52M);

    while (1) {
        UI_MANAGE_MESSAGE_BLOCK *msg_p = NULL;

        app_sysfreq_req(APP_SYSFREQ_USER_UI, APP_SYSFREQ_32K);
        if(ui_manage_mailbox_get(&msg_p) == 0)
        {
            app_sysfreq_req(APP_SYSFREQ_USER_UI, APP_SYSFREQ_52M);
			app_ui_event_handle(msg_p);	
        }
        ui_manage_mailbox_free(msg_p);
    }
}

static void ui_manage_mailbox_init(void)
{
    ui_manage_mailbox = osMailCreate(osMailQ(ui_manage_mailbox), NULL);

    if (ui_manage_mailbox == NULL) {
        APP_UI_TRACE("Failed to Create ui_manage_mailbox\n");
        return;
    }

    ui_manage_mailbox_cnt = 0;
    return;
}

static void ui_manage_thread_init(void)
{
    APP_UI_TRACE();

	ui_manage_mailbox_init();
    ui_manage_tid = osThreadCreate(osThread(ui_manage_thread), NULL);

    if (ui_manage_tid == NULL) {
        APP_UI_TRACE("Failed to Create ui_manage_thread\n");
    }
	
}

void zowee_thread_init(void)
{
	ui_manage_thread_init();

}

static void app_ui_massage_dump(void)
{
	osEvent evt;
	APP_UI_TRACE("osMailAlloc error dump");
	for (uint8_t i=0; i<UI_MANAGE_MAILBOX_MAX; i++){
		evt = osMailGet(ui_manage_mailbox, 0);
		if (evt.status == osEventMail) {
			APP_UI_TRACE("cnt:%d %d",i,((UI_MANAGE_MESSAGE_BLOCK *)(evt.value.p))->msg_id);
		}else{				  
			APP_UI_TRACE("cnt:%d %d", i, evt.status); 
		}
	}
	APP_UI_TRACE("osMailAlloc error dump end");
}

void app_ui_event_process(UI_EVENT_MANAGE_T msg_id, uint32_t ptr, uint32_t id, uint32_t param0, uint32_t param1)
{
    UI_MANAGE_MESSAGE_BLOCK *msg;
    osStatus status;

    
    msg = (UI_MANAGE_MESSAGE_BLOCK *)osMailAlloc(ui_manage_mailbox, 0);
    if (!msg) {
        APP_UI_TRACE("osMailAlloc ERROR \n");
        return;
    }

    if ((ui_manage_mailbox_cnt + 1) > UI_MANAGE_MAILBOX_MAX) {
        APP_UI_TRACE(">>>> error:app_ui_event_process dead in event. %d %d %d",msg_id,id,ui_manage_mailbox_cnt);		
		app_ui_massage_dump();
		ASSERT(1, "osMailAlloc error out of length");
        return;
    }
	ASSERT(msg, "osMailAlloc error");
    msg->msg_id = msg_id;
    msg->ptr = ptr;
    msg->id = id;
    msg->param0 = param0;
    msg->param1 = param1;
    status = osMailPut(ui_manage_mailbox, msg);
    if (osOK == status) {
        ui_manage_mailbox_cnt++;
    }
}

#ifdef __SET_EARSIDE_VIA_GPIO__

#define TWS_SIDE_ROLE_PIN			HAL_IOMUX_PIN_P2_5
void ui_set_earside_test(void)
{
	const struct HAL_IOMUX_PIN_FUNCTION_MAP app_tws_side_role_cfg = {
		TWS_SIDE_ROLE_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE,
	};
	
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_tws_side_role_cfg, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_tws_side_role_cfg.pin, HAL_GPIO_DIR_IN, 0);
	osDelay(1);
	if(!hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)app_tws_side_role_cfg.pin))
	{
		zowee_section_set_earside(0);//test set earside 0 yeild left 1 yeild right
	}
	else
	{
		zowee_section_set_earside(1);//test set earside 0 yeild left 1 yeild right
	}

	app_get_earside_from_zowee_section();
}

#endif

void ui_send_vbus_data(uint8_t *buff,uint8_t length)
{
	vbus_hal_if_send_data(buff, length);
}

void app_reboot()
{
	APP_UI_TRACE();
	hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
	hal_cmu_sys_reboot();
}

void ui_manage_init(void)
{	
#ifdef __SET_EARSIDE_VIA_GPIO__
	ui_set_earside_test();
#endif

	//create ibrt check timer
	ibrt_status_timer = osTimerCreate(osTimer(IBRT_STATUS_TIMER), osTimerPeriodic, NULL);
	//dis conncet 30min shutdown timer
	ui_set_disconnect_init();
	ui_manage_perpheral_init();
	ui_manage_product_test_init();
	APP_UI_TRACE("ui_manage_init end!");
	ui_set_sys_status(APP_UI_SYS_NORMAL);	
}

