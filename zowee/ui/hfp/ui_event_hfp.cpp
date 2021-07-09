#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "app_thread.h"
#include "hwtimer_list.h"

#include "ui_event_manage.h"
#include "zowee_config.h"
#include "gesture_hal_if.h"
#include "ui_event_wear_status.h"

#include "app_bt_stream.h"
#include "ui_event_bt.h"
#include "ui_event_hfp.h"

#include "btapp.h"
#include "app_ibrt_if.h"

//
extern struct BT_DEVICE_T  app_bt_device;


//function define 
extern int app_hfp_siri_voice(bool en);
extern bool app_siri_is_online(void);
extern uint8_t btapp_hfp_get_call_active(void);
extern uint8_t app_poweroff_flag;

void check_sco_data(uint8_t *buf, uint32_t len)
{
    if (UI_DEVICE_BOX_OPEN == ui_device_status.local_status.ear_status || UI_DEVICE_BOX_CLOSED == ui_device_status.local_status.ear_status || app_poweroff_flag)
    {
        UI_HFP_TRACE("inbox mute!!!!!");
        memset(buf, 0, len);
    }
}

void check_mic_data(uint8_t *buf, uint32_t len)
{
    if (UI_DEVICE_BOX_OPEN == ui_device_status.local_status.ear_status || UI_DEVICE_BOX_CLOSED == ui_device_status.local_status.ear_status)
    {
        UI_HFP_TRACE("inbox mute!!!!!");
        memset(buf, 0, len);
    }
}

bool zowee_if_in_calling(void)
{
    //UI_HFP_TRACE();
    UI_HFP_TRACE(" hfchan_callSetup %d hfchan_call %d hf_audio_state %d",
            app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1],
            app_bt_device.hfchan_call[BT_DEVICE_ID_1],
            app_bt_device.hf_audio_state[BT_DEVICE_ID_1]);

    if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_NONE)&&
        (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_NONE)&&
        (app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == BTIF_HF_AUDIO_DISCON)) {
        return false;
    } else {
        return true;
    }
}

uint8_t zowee_if_calling_status_get(void)
{
    UI_HFP_TRACE();
    UI_HFP_TRACE(" hfchan_callSetup %d hfchan_call %d hf_audio_state %d",
            app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1],
            app_bt_device.hfchan_call[BT_DEVICE_ID_1],
            app_bt_device.hf_audio_state[BT_DEVICE_ID_1]);

    if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] != BTIF_HF_CALL_SETUP_NONE)&&
        (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_NONE))
    {
        return 1;
    } else if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_NONE)&&
        (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_ACTIVE))
	{
        return 2;
    }
	else
	{
		return 0;
	}
}


void zowee_ctrl_siri_process(void)
{
    UI_HFP_TRACE();
	if (true == app_siri_is_online())
	{
	    UI_HFP_TRACE("app_siri connected...now close");
	    app_hfp_siri_voice(false);
	}
	else
	{
		UI_HFP_TRACE("app_siri disconnected...now open");
		if ((false == zowee_if_in_calling()) || !btapp_hfp_get_call_active())
			app_hfp_siri_voice(true);
	}
}

void zowee_ctrl_call_process(HFP_KEY action)
{
	UI_HFP_TRACE();
	UI_HFP_TRACE(" hfchan_callSetup %d hfchan_call %d hf_audio_state %d",
             app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1],
             app_bt_device.hfchan_call[BT_DEVICE_ID_1],
             app_bt_device.hf_audio_state[BT_DEVICE_ID_1]);

    if ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_NONE) &&
        (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_NONE) &&
        (app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == BTIF_HF_AUDIO_DISCON))
    {
    	UI_HFP_TRACE("now call is none! return");
        return;
    }
	switch(action)
	{
		case UI_EVENT_HFP_ACCEPT_CALL:
		{
			if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_IN) &&
	             (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_NONE))
		    {
		        UI_HFP_TRACE("only call is incoming !!! accept call");
		        hfp_handle_key(HFP_KEY_ANSWER_CALL);
		    }
		}
		break;
		
		case UI_EVENT_HFP_END_CALL:
		{	
			if ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_OUT) ||
	             (app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_ALERT))
		    {
		        UI_HFP_TRACE("only call is outcoming !!!");
		        hfp_handle_key(HFP_KEY_HANGUP_CALL);
		    }
			else if (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_ACTIVE ||
             app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == BTIF_HF_AUDIO_CON)
		    {
		        UI_HFP_TRACE("only call -> hangup the call !!!");
		        hfp_handle_key(HFP_KEY_HANGUP_CALL);
		    }
		}
		break;
		
		case UI_EVENT_HFP_REJECT_CALL:
		{
			if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_IN) &&
	             (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_NONE))
		    {
		        UI_HFP_TRACE("only call is incoming !!! reject call");
		        hfp_handle_key(HFP_KEY_HANGUP_CALL);
		    }
		}
		break;

		case UI_EVENT_HFP_THREEWAY_CALL:
		{
			if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == BTIF_HF_CALL_SETUP_IN) &&
	             (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == BTIF_HF_CALL_ACTIVE))
		    {
		        UI_HFP_TRACE("Now is three call -> hold and answer!!!");
		        hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
		    }
		}
		break;

		default:

		break;
	}
	
}


void ui_manage_hfp_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	if(!app_tws_ibrt_mobile_link_connected())
	{// send to master
		uint8_t hfp_event[1];
		
		hfp_event[0]=msg_body->id;
		zowee_ibrt_tws_exchange_info_without_rsp(TWS_SYNC_HFP,(uint8_t *)hfp_event,sizeof(hfp_event));
		return;
	}

    UI_HFP_TRACE(" %d.",msg_body->id);
	switch(msg_body->id)
	{
		case UI_EVENT_HFP_ACCEPT_CALL:
		{
			zowee_ctrl_call_process(UI_EVENT_HFP_ACCEPT_CALL);
		}
		break;
		
		case UI_EVENT_HFP_END_CALL:
		{	
			zowee_ctrl_call_process(UI_EVENT_HFP_END_CALL);
		}
		break;
		
		case UI_EVENT_HFP_REJECT_CALL:
		{
			zowee_ctrl_call_process(UI_EVENT_HFP_REJECT_CALL);
		}
		break;
		
		case UI_EVENT_HFP_THREEWAY_CALL:
		{
			zowee_ctrl_call_process(UI_EVENT_HFP_THREEWAY_CALL);
		}
		break;
		
		case UI_EVENT_HFP_SIRI:
		{
			zowee_ctrl_siri_process();
		}
		break;
		
		default:

		break;
	}
}


