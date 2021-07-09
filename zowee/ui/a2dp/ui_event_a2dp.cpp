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

#include "ui_event_a2dp.h"
#include "app_bt_stream.h"
#include "ui_event_bt.h"
#include "ui_event_manage.h"

#include "btapp.h"
#include "app_ibrt_if.h"


extern bool zowee_if_in_calling(void);
//function define 
extern uint8_t avrcp_get_media_status(void);
//extern void avrcp_set_media_status(uint8_t status);
extern uint8_t app_poweroff_flag;

void check_sbc_data(uint8_t *buf, uint32_t len)
{
    if ((UI_DEVICE_BOX_OPEN == ui_device_status.local_status.ear_status) || (UI_DEVICE_BOX_CLOSED == ui_device_status.local_status.ear_status) || app_poweroff_flag)
    {
        UI_A2DP_TRACE("inbox mute!!!!!");
        memset(buf, 0, len);
    }
}

static void app_zowee_ctrl_music_forward(void)
{
    UI_A2DP_TRACE();
    a2dp_handleKey(AVRCP_KEY_FORWARD);
}

static void app_zowee_ctrl_music_backward(void)
{
    UI_A2DP_TRACE();

    a2dp_handleKey(AVRCP_KEY_BACKWARD);
}

static void app_zowee_ctrl_music_paly(void)
{
    uint8_t media_status = avrcp_get_media_status();

    UI_A2DP_TRACE("media_status=%d", media_status);
	if (media_status == BTIF_AVRCP_MEDIA_PAUSED)
	{
		//avrcp_set_media_status(BTIF_AVRCP_MEDIA_PLAYING);
		a2dp_handleKey(AVRCP_KEY_PLAY);
	}
}

static void app_zowee_ctrl_music_pause(void)
{
    uint8_t media_status = avrcp_get_media_status();

    UI_A2DP_TRACE("media_status=%d", media_status);
	if (media_status == BTIF_AVRCP_MEDIA_PLAYING)
	{
		//avrcp_set_media_status(BTIF_AVRCP_MEDIA_PAUSED);
        a2dp_handleKey(AVRCP_KEY_PAUSE);
	}
}

static void app_zowee_ctrl_music(void)
{
    uint8_t media_status = avrcp_get_media_status();

    UI_A2DP_TRACE("media_status=%d", media_status);

    if (media_status == BTIF_AVRCP_MEDIA_PAUSED)
    {
        //avrcp_set_media_status(BTIF_AVRCP_MEDIA_PLAYING);
        a2dp_handleKey(AVRCP_KEY_PLAY);
    }
    else
    {
        //avrcp_set_media_status(BTIF_AVRCP_MEDIA_PAUSED);
        a2dp_handleKey(AVRCP_KEY_PAUSE);
    }
}

void ui_manage_a2dp_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	if(!app_tws_ibrt_mobile_link_connected())
	{// send to master
		uint8_t a2dp_event[1];
		
		a2dp_event[0]=msg_body->id;// 
		zowee_ibrt_tws_exchange_info_without_rsp(TWS_SYNC_AVRCP,(uint8_t *)a2dp_event,sizeof(a2dp_event));
		return;
	}

	if(zowee_if_in_calling())
	{
		// if hfp active 
		return;
	}

    UI_A2DP_TRACE("msg_body->id:%d,%d.", msg_body->id, msg_body->param1);
	switch(msg_body->id)
	{
		case AVRCP_KEY_PLAY:
		{
			app_zowee_ctrl_music_paly();
		}
		break;
		
		case AVRCP_KEY_PAUSE:
		{
			app_zowee_ctrl_music_pause();
		}
		break;
		
		case AVRCP_KEY_PLAY_OR_PAUSE:
		{
			app_zowee_ctrl_music();
		}
		break;
		
		case AVRCP_KEY_FORWARD:
		{
			app_zowee_ctrl_music_forward();
		}
		break;
		
		case AVRCP_KEY_BACKWARD:
		{
			app_zowee_ctrl_music_backward();
		}
		break;
			
		default:
		break;
	}
}

