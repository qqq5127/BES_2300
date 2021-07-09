#ifdef __APP_WEAR_ENABLE__
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

#include "ui_event_manage.h"
#include "zowee_config.h"

#include "wear_hal_if.h"
#include "ui_event_wear_status.h"
#include "gesture_hal_if.h"
#include "ui_event_a2dp.h"
#include "ui_event_bt.h"
#include "app_ibrt_if.h"
#include "app_anc.h"
#include "apps.h"
#include "app_status_ind.h"

extern "C" int app_get_earside(void);

void ui_wear_control(uint8_t flag)
{
	uint8_t wear_state[1];

	if(flag)
	{
		UI_WEAR_STATUS_TRACE("wear on !!!");
		if(ui_device_status.local_status.wear_report_status)
			app_voice_report(APP_STATUS_INDICATION_WEAR_ON,0);

		ui_device_status.local_status.wear_status = UI_WEAR_ON;
		ui_device_status.local_status.ear_status = UI_DEVICE_WARED;

		app_ui_event_process(UI_EVENT_STATUS, 0, UI_EVENT_STATUS_WEAR_UP, 0, 0);

		if (app_tws_ibrt_tws_link_connected())
		{
			wear_state[0]=0x01;//表示戴上
			zowee_ibrt_tws_exchange_info_without_rsp(TWS_WEAR_STATE,(uint8_t *)wear_state,sizeof(wear_state));			
		}

		UI_WEAR_STATUS_TRACE("wear on, local:%d peer:%d!!!",ui_device_status.local_status.wear_status,
			ui_device_status.peer_status.wear_status);

		if(app_tws_ibrt_mobile_link_connected())//主�?
		{
			if(!app_tws_ibrt_tws_link_connected())
			{
				app_ui_event_process(UI_EVENT_A2DP, 0, AVRCP_KEY_PLAY, 0, 0);
			}
			else if(ui_device_status.peer_status.wear_status == 1)
			{
				app_ui_event_process(UI_EVENT_A2DP, 0, AVRCP_KEY_PLAY, 0, 0);
			}
		}
		
#ifdef __WEAR_CONTROL_ANC__
		zowee_anc_status_stop_close_anc_timer();
		//if((app_get_earside() == 1) && app_tws_ibrt_tws_link_connected())		
		{
			zowee_anc_status_change_when_wear_status_change(1);
		}
#endif
	}
	else
	{
		UI_WEAR_STATUS_TRACE("wear off !!!");
		if(ui_device_status.local_status.wear_report_status)
			app_voice_report(APP_STATUS_INDICATION_WEAR_OFF,0);
		
		ui_device_status.local_status.wear_status = UI_WEAR_OFF;
		ui_device_status.local_status.ear_status = UI_DEVICE_OUT_BOX;

		app_ui_event_process(UI_EVENT_STATUS, 0, UI_EVENT_STATUS_WEAR_DOWN, 0, 0);

		if (app_tws_ibrt_tws_link_connected())
		{
			wear_state[0]=0x00;//表示取下
			zowee_ibrt_tws_exchange_info_without_rsp(TWS_WEAR_STATE,(uint8_t *)wear_state,sizeof(wear_state));			
		}

		UI_WEAR_STATUS_TRACE("wear off, local:%d peer:%d!!!",ui_device_status.local_status.wear_status,
			ui_device_status.peer_status.wear_status);

		app_ui_event_process(UI_EVENT_A2DP, 0, AVRCP_KEY_PAUSE, 0, 0);
#ifdef __WEAR_CONTROL_ANC__
		zowee_anc_status_change_when_wear_status_change(0);
#endif		
	}

}


void ui_manage_wear_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	UI_WEAR_STATUS_TRACE(": %d", msg_body->id);
	switch(msg_body->id)
	{
#if defined(__APP_WEAR_ENABLE__)
		case UI_EVENT_WEAR_INT:
		{
			wear_hal_if_int_handler();
		}
		break;

		case UI_EVENT_WEAR_ON:
		{
			ui_wear_control(1);
		}
		break;

		case UI_EVENT_WEAR_OFF:
		{
			ui_wear_control(0);
		}
		break;

		case UI_EVENT_WEAR_REFRESH_CALI_DATA:
		{
			wear_hal_if_refresh_cali_data();
		}
		break;
#endif

		
		default:

		break;
	}
}

#endif
