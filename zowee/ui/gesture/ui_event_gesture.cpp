#ifdef __APP_GESTURE_ENABLE__
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
#include "ui_event_hfp.h"
#include "zowee_config.h"
#include "gesture_hal_if.h"
#include "ui_event_gesture.h"
#include "ui_event_a2dp.h"
#include "app_anc.h"
#include "app_hfp.h"
#include "ui_event_bt.h"
#include "app_ibrt_if.h"
#include "apps.h"
#include "zowee_section.h"
#include "nvrecord_env.h"
#include "app_anc.h"


extern "C" void app_anc_status_sync(uint8_t status);
extern uint8_t btapp_hfp_get_call_setup(void);

static void gesture_event_feature_implement(uint32_t msg)
{
	uint16_t gesture_feature,gesture_cnt,i;
	//struct nvrecord_env_t *nvrecord_env;

	gesture_cnt = msg - UI_EVENT_GESTURE_CLICK;

	if(gesture_cnt >= sizeof(UI_GESTURE_FEATURE_DEFINE)/sizeof(uint16_t))
	{
		return;
	}
	gesture_feature = UI_GESTURE_FEATURE_DEFINE[gesture_cnt];

	UI_GESTURE_TRACE(" gesture_cnt:%d gesture_feature: 0x%x", gesture_cnt, gesture_feature);

	for(i=0;i<16;i++)
	{
		//UI_GESTURE_TRACE("gesture_feature & (0x01 << %d): 0x%x ", i, gesture_feature & (0x01 << i));
		switch(gesture_feature & (0x01 << i))
		{
			case UI_EVENT_GESTURE_CALL_ACCEPTE:
			{		
				UI_GESTURE_TRACE("000 btapp_hfp_is_call_active:%d, btapp_hfp_get_call_setup:%d", btapp_hfp_is_call_active(), btapp_hfp_get_call_setup());

				if(BTIF_HF_CALL_SETUP_IN == btapp_hfp_get_call_setup())
				{
					if(btapp_hfp_is_call_active())
					{
						UI_GESTURE_TRACE("UI_EVENT_GESTURE_CALL_ACCEPTE THREEWAY HOLD AND ANSWER");					
						app_ui_event_process(UI_EVENT_HFP,0,UI_EVENT_HFP_THREEWAY_CALL,0,0);
						return;
					}
					else
					{
						UI_GESTURE_TRACE("UI_EVENT_GESTURE_CALL_ACCEPTE ACCEPT");					
						app_ui_event_process(UI_EVENT_HFP,0,UI_EVENT_HFP_ACCEPT_CALL,0,0);
						return;
					}
				}
			}
			break;

			case UI_EVENT_GESTURE_HANGUP_CALL:
			{
				UI_GESTURE_TRACE("111 btapp_hfp_is_call_active:%d btapp_hfp_get_call_setup:%d",btapp_hfp_is_call_active(), btapp_hfp_get_call_setup());
				
				if(btapp_hfp_is_call_active() || btapp_hfp_get_call_setup())
				{
					UI_GESTURE_TRACE("UI_EVENT_GESTURE_HANGUP_CALL");
					app_ui_event_process(UI_EVENT_HFP,0,UI_EVENT_HFP_END_CALL,0,0);
					return;
				}

			}
			break;

			case UI_EVENT_GESTURE_CALL_REJECT:
			{
				UI_GESTURE_TRACE("222 btapp_hfp_is_call_active:%d btapp_hfp_get_call_setup:%d", btapp_hfp_is_call_active(), btapp_hfp_get_call_setup());

				if(!btapp_hfp_is_call_active() && (BTIF_HF_CALL_SETUP_IN == btapp_hfp_get_call_setup()))
				{
					UI_GESTURE_TRACE("UI_EVENT_GESTURE_CALL_REJECT");
					app_ui_event_process(UI_EVENT_HFP,0,UI_EVENT_HFP_REJECT_CALL,0,0);
					//app_ui_event_process(UI_EVENT_HFP_END_CALL,0,0,0,0);
					return;
				}
			}
			break;

			case UI_EVENT_GESTURE_PLAY_PAUSE:
			{
				UI_GESTURE_TRACE("UI_EVENT_GESTURE_PLAY_PAUSE");
				app_ui_event_process(UI_EVENT_A2DP,0,AVRCP_KEY_PLAY_OR_PAUSE,0,0);
				return;
			}
			break;

			case UI_EVENT_GESTURE_ANC_LOOP:
			{
				UI_GESTURE_TRACE("UI_EVENT_GESTURE_ANC_LOOP");
				#ifdef __WEAR_CONTROL_ANC__
				//if(ui_device_status.local_status.wear_status && ui_device_status.peer_status.wear_status)
				{
				#endif
				zowee_anc_status_change_msg(ANC_SWITCH_LOOP,(ZOWEE_ANC_STATUS_E)0);
				#ifdef __WEAR_CONTROL_ANC__
				}	
				#endif
			}
			break;

			case UI_EVENT_GESTURE_A2DP_BACKWARD:
			{
				UI_GESTURE_TRACE("UI_EVENT_GESTURE_A2DP_BACKWARD");
				app_ui_event_process(UI_EVENT_A2DP,0,AVRCP_KEY_BACKWARD,0,0);
				return;
			}
			break;

			case UI_EVENT_GESTURE_A2DP_FORWARD:
			{
				UI_GESTURE_TRACE("UI_EVENT_GESTURE_A2DP_FORWARD");
				app_ui_event_process(UI_EVENT_A2DP,0,AVRCP_KEY_FORWARD,0,0);
				return;
			}
			break;

			case UI_EVENT_GESTURE_VOICE_WAKE:
			{
				UI_GESTURE_TRACE("UI_EVENT_GESTURE_VOICE_WAKE");
				app_ui_event_process(UI_EVENT_HFP,0,UI_EVENT_HFP_SIRI,0,0);
				return;
			}
			break;
			
			default:

			break;
		}
	}
}

void ui_manage_gesture_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	UI_GESTURE_TRACE("%d %d",msg_body->id,ui_device_status.local_status.ear_status);

	switch(msg_body->id)
	{
		case UI_EVENT_GESTURE_INT:
		{
#ifdef __PROJ_NAME_ZT203__
			if(ui_device_status.local_status.ear_status != UI_DEVICE_WARED)
			{
				return;
			}
#endif
			gesture_hal_if_int_handler();
		}
		break;

		case UI_EVENT_GESTURE_CLICK:
		case UI_EVENT_GESTURE_LONG_PRESS:
		case UI_EVENT_GESTURE_LONG_LONG_PRESS:
		case UI_EVENT_GESTURE_DB_CLICK_PRESS:
		case UI_EVENT_GESTURE_TB_CLICK_PRESS:
		{
			if(ui_device_status.local_status.ear_status != UI_DEVICE_WARED)
			{
				return;
			}
			gesture_event_feature_implement(msg_body->id);
		}
		break;

		default:

		break;
	}
}

#endif//__APP_GESTURE_ENABLE__
