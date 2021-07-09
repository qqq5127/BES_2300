#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "app_thread.h"
#include "hwtimer_list.h"
#include "app_thread.h"
#include "app_bt.h"

#include "ui_event_manage.h"
#include "zowee_config.h"
#include "gesture_hal_if.h"
#include "ui_event_wear_status.h"
#include "ui_event_bt.h"
#include "ui_event_hfp.h"

//bes platform
#include "app_ibrt_if.h"
#include "app_ibrt_ui.h"
#include "app_ibrt_customif_cmd.h"
#include "apps.h"
#include "ui_event_a2dp.h"
#include "app_hfp.h"
#include "app_anc.h"
#include "app_ibrt_nvrecord.h"
#include "app_bt_func.h"
#include "btapp.h"
#include "nvrecord_bt.h"
#include "besbt.h"
#include "a2dp_decoder.h"
#include "zowee_section.h"
#include "nvrecord_env.h"
#include "app_anc.h"
#include "ui_event_battery.h"
#include "battery_hal_if.h"

void bt_access_timer_handler(void const *param);

osTimerDef(BT_ACCESS_MODE_TIMER, bt_access_timer_handler);
static osTimerId bt_access_timer = NULL;

//global variable define
extern "C" ANC_RESET_STATUS_T anc_reset_status;
extern struct BT_DEVICE_T	app_bt_device;
static bool peer_inbox_enter_freeman_flag = false; //for peer earphone inbox enter freeman when tws is connected,we need let the outside earphone enter unconnectable

//function define
extern "C" void app_anc_status_sync(uint8_t status);
extern "C" void app_anc_do_init(void);
extern "C" void app_anc_close_anc(void);
extern uint8_t btapp_hfp_get_call_setup(void);
extern uint8_t avrcp_get_media_status(void);

void bt_access_timer_handler(void const *param)
{
	UI_BT_TRACE("tws:%d", app_tws_ibrt_tws_link_connected());
	if(!app_tws_ibrt_tws_link_connected())
	{
		//enter not accessible
		app_tws_ibrt_set_access_mode(BTIF_BAM_NOT_ACCESSIBLE);
		//clear pair record
		zowee_ibrt_remove_history_tws_paired_device();
		nv_record_sector_clear();
		nv_record_env_init();
		
		osDelay(100);

//		zowee_section_set_battery_level_state(ui_device_status.local_status.battery_level);

		UI_BT_TRACE("reboot!");
		app_reboot();
	}
	else
	{
		osTimerStop(bt_access_timer);
		osTimerStart(bt_access_timer, 500);	
	}
}

void zowee_ibrt_remove_history_tws_paired_device(void)
{
    bt_status_t            retStatus;
    btif_device_record_t   record;
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    int                    paired_dev_count = nv_record_get_paired_dev_count();

    UI_BT_TRACE("Remove all history tws nv records.");
    UI_BT_TRACE("Master addr:");
    DUMP8("%02x ",p_ibrt_ctrl->local_addr.address, BTIF_BD_ADDR_SIZE);
    UI_BT_TRACE("Slave addr:");
    DUMP8("%02x ",p_ibrt_ctrl->peer_addr.address, BTIF_BD_ADDR_SIZE);

    for (int32_t index = paired_dev_count - 1; index >= 0; index--)
    {
        retStatus = nv_record_enum_dev_records(index, &record);
        if (BT_STS_SUCCESS == retStatus)
        {
            UI_BT_TRACE("The index %d of nv records:", index);
            DUMP8("%02x ", record.bdAddr.address, BTIF_BD_ADDR_SIZE);
            if (!memcmp(record.bdAddr.address, p_ibrt_ctrl->local_addr.address, BTIF_BD_ADDR_SIZE) ||
                !memcmp(record.bdAddr.address, p_ibrt_ctrl->peer_addr.address, BTIF_BD_ADDR_SIZE))
            {
                nv_record_ddbrec_delete(&record.bdAddr);
                UI_BT_TRACE("Delete the nv record entry %d", index);
            }
        }
    }

    memset(p_ibrt_ctrl->local_addr.address, 0, BTIF_BD_ADDR_SIZE);
    memset(p_ibrt_ctrl->peer_addr.address, 0, BTIF_BD_ADDR_SIZE);
}

void zowee_tws_disconnect_all_connections()
{
	UI_BT_TRACE("%d",app_tws_ibrt_tws_link_connected());
	app_ibrt_if_disconnect_mobile_tws_link();
}

void zowee_disconnect_all_bt_connections(void)
{
	UI_BT_TRACE("%d %d %d",app_tws_ibrt_tws_link_connected(),app_tws_ibrt_mobile_link_connected(),app_tws_ibrt_slave_ibrt_link_connected());
	app_disconnect_all_bt_connections();
}

//slave and master info exchange handler
void zowee_tws_exchange_info_handler(uint8_t *ptrParam, uint32_t paramLen)
{
    UI_BT_TRACE("%d,%d.", *ptrParam, paramLen);
	struct nvrecord_env_t *nvrecord_env;

    if (paramLen == 0)
    {
        return;
    }
	//ptrParam[0]:cmd
    switch (*ptrParam)
    {
	case TWS_WEAR_STATE:
	{
		if(ptrParam[1]==0x01)//根据副耳上报状态改变播放状态	  //wear on
		{
			ui_device_status.peer_status.wear_status = UI_WEAR_ON;
			if(app_tws_ibrt_mobile_link_connected())//主耳
			{
				if(ui_device_status.local_status.wear_status == 1)
				{
					app_ui_event_process(UI_EVENT_A2DP, 0, AVRCP_KEY_PLAY, 0, 0);

				}
			}
#if 0//def __WEAR_CONTROL_ANC__
			if(app_get_earside() == 1 && (ui_device_status.local_status.wear_status == 1))		
			{
				zowee_anc_wear_status_change_msg(1);
			}
#endif			
		}
		UI_BT_TRACE("TWS_WEAR_STATE local:%d peer:%d!!!",ui_device_status.local_status.wear_status,ui_device_status.peer_status.wear_status);

	}	
	break;
	
	case TWS_SYNC_AVRCP:
	{
		if(app_tws_ibrt_mobile_link_connected())//主耳
		{
			app_ui_event_process(UI_EVENT_A2DP, 0, (uint32_t )ptrParam[1], 0, 0);
		}
	}
	break;

	case TWS_SYNC_HFP:
	{
		if(app_tws_ibrt_mobile_link_connected())//主耳
		{
			app_ui_event_process(UI_EVENT_HFP, 0, (UI_EVENT_MANAGE_T )ptrParam[1], 0, 0);
		}
	}
	break;

	case TWS_ANC_STATE:
	{
		
		UI_BT_TRACE("ui_device_status.anc_status=%d", ui_device_status.local_status.anc_status);
		UI_BT_TRACE("peer send anc_coef_idx=%d anc_status:%d anc_switch_mode:%d", ptrParam[1],ptrParam[2],ptrParam[3]);
		if(ui_device_status.local_status.ear_status > UI_DEVICE_BOX_OPEN)
		{
			app_anc_set_coef(ptrParam[1]);
			zowee_anc_status_change_msg(ANC_SWITCH_DIRECT,(ZOWEE_ANC_STATUS_E)ptrParam[2]);
		}
		
		if((ANC_SWITCH_MODE_E)ptrParam[3] == ANC_SWITCH_LOOP)
		{
			ui_device_status.local_status.anc_status = (ZOWEE_ANC_STATUS_E)ptrParam[2];
			ui_device_status.peer_status.anc_status = (ZOWEE_ANC_STATUS_E)ptrParam[2];
			nv_record_env_get(&nvrecord_env);
			nvrecord_env->anc_state = ptrParam[2];	
			nv_record_env_set(nvrecord_env);
		}

	}	
	break;
	
	case TWS_MASTER_ENTER_FREEMAN:
	{
		UI_BT_TRACE("now i am new master,peer enter freeman,i need enter undiscoverable un connectable");		
		//zowee_ibrt_remove_history_tws_paired_device();

#if 0		
		ibrt_config_t config;
		app_ibrt_if_config_load(&config);
		config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LRMERGE;
		app_ibrt_if_reconfig(&config);
#endif	

		app_ibrt_if_disconnect_mobile_tws_link();
		//zowee_ibrt_remove_history_tws_paired_device();
		peer_inbox_enter_freeman_flag = true;
		if(bt_access_timer == NULL)
		{
			bt_access_timer = osTimerCreate(osTimer(BT_ACCESS_MODE_TIMER), osTimerOnce, NULL); 
		}
		osTimerStop(bt_access_timer);
		osTimerStart(bt_access_timer, 500);	
	}
	break;
	
	case TWS_SHUTDOWN:
	{
		UI_BT_TRACE();
		app_shutdown();
	}
	break;

	case TWS_SYNC_INFO:
	{	
	    UI_DEVICE_SYNC_STATUS_T *p_peer_sync_status;
		p_peer_sync_status = (UI_DEVICE_SYNC_STATUS_T *)(ptrParam+1);
		memcpy((void *)&ui_device_status.peer_status, (void *)p_peer_sync_status, sizeof(UI_DEVICE_SYNC_STATUS_T));
		UI_BT_TRACE("peer ear wear:%x,ear box:%x battery level:%d",ui_device_status.peer_status.wear_status,
																	ui_device_status.peer_status.ear_status,
																	ui_device_status.peer_status.battery_level);
		if(ui_device_status.peer_status.battery_level < ui_device_status.local_status.battery_level)
		{
			app_ui_event_process(UI_EVENT_CHARGING,0,UI_EVENT_BATTERY_LEVEL_REPROT,0,0);
		}
#ifdef __WEAR_CONTROL_ANC__				
		ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();
		UI_BT_TRACE("curr_role:%d, local_anc:%d, peer_anc:%d",p_ibrt_ctrl->current_role,
															  ui_device_status.local_status.anc_status,
															  ui_device_status.peer_status.anc_status);
		if(p_ibrt_ctrl->current_role != IBRT_UNKNOW)
		{
			ui_device_status.local_status.anc_status = ui_device_status.peer_status.anc_status;
		}
#endif
	}
	break;
	
    default:
    break;
	}
}

const char *cmd_str[] =
{
    "[TWS_WEAR_STATE]",
    "[TWS_GESTURE_STATE]",
    "[TWS_CALL_STATE]",
    "[TWS_SYNC_AVRCP]",
    "[TWS_SYNC_HFP]",
    "[TWS_ANC_STATE]",
    "[TWS_MASTER_ENTER_FREEMAN]",
    "[TWS_SHUTDOWN]",
    "[TWS_SYNC_INFO]",
};

const char *cmd2str(ZOWEE_TWS_EXCHANGE_INFO_EVENT_ENUM cmd)
{
    const char *str = NULL;

    if (cmd >= 0 && cmd <= TWS_SYNC_INFO)
    {
        str = cmd_str[cmd];
    }
    else
    {
        str = "[UNKNOWN] CMD";
    }

    return str;
}

/***send cmd without rsp***/
void zowee_ibrt_tws_exchange_info_without_rsp(ZOWEE_TWS_EXCHANGE_INFO_EVENT_ENUM cmd, uint8_t *buffer, uint8_t length)
{
    uint8_t data_buffer[30];

    UI_BT_TRACE("%d,%s,len:%d.", cmd, cmd2str(cmd),length);

    if (length > sizeof(data_buffer) - 1)
    {
        return;
    }

    data_buffer[0] = (uint8_t)cmd;

    if (length > 0)
    {
        memcpy(data_buffer + 1, buffer, length);
    }

    zowee_ibrt_tws_exchange_info_send_data_without_rsp(data_buffer, length + 1);
}

void zowee_tws_send_local_info_to_peer(void)
{
	UI_BT_TRACE();
	UI_DEVICE_SYNC_STATUS_T peer_sync_status;
	memcpy((void *)&peer_sync_status, (void *)&ui_device_status.local_status, sizeof(UI_DEVICE_SYNC_STATUS_T));	
	UI_BT_TRACE("local ear wear:%d,ear status:%d battery level:%d",peer_sync_status.wear_status,peer_sync_status.ear_status
			,peer_sync_status.battery_level);
	if(app_tws_ibrt_tws_link_connected())
	{
		if(battery_hal_if_charging_status() == CHARGING_FULL && (peer_sync_status.battery_level>= 9))
		{//charging full
			peer_sync_status.battery_level = 10;
		}
		zowee_ibrt_tws_exchange_info_without_rsp(TWS_SYNC_INFO,(uint8_t*)&peer_sync_status,sizeof(UI_DEVICE_SYNC_STATUS_T));
	}
}

void zowee_tws_exchange_info_with_rsp_handler(uint16_t rsp_seq, uint8_t *data, uint16_t len, uint8_t need_rsp)
{
	UI_BT_TRACE("rsp_seq:%d,cmd:%d,len:%d need_rsp:%d", rsp_seq,*data, len,need_rsp);
	DUMP8("%02x", data, len);
	
    if (NULL == data || len == 0)
    {
        return;
    }
	//data[0]:cmd
	if(need_rsp)
	{
		switch (*data)
		{
			case TWS_STATE:
			{
				UI_DEVICE_SYNC_STATUS_T *p_peer_sync_status;
				p_peer_sync_status = (UI_DEVICE_SYNC_STATUS_T *)(data+1);
				memcpy((void *)&ui_device_status.peer_status, (void *)p_peer_sync_status, sizeof(UI_DEVICE_SYNC_STATUS_T));
				UI_BT_TRACE("peer ear wear:%x,ear:%x battery level:%x",ui_device_status.peer_status.wear_status,ui_device_status.peer_status.ear_status,
					ui_device_status.peer_status.battery_level); 
				zowee_ibrt_tws_exchange_info_send_rsp(rsp_seq, NULL, 0);
			}
			break;
		
			default:
			break;
		}
	}
	else
	{
		UI_BT_TRACE("receive rsp handler!!!");
	}

}

/***send cmd with rsp***/
void zowee_ibrt_tws_exchange_info_with_rsp(ZOWEE_TWS_EXCHANGE_INFO_WITH_RSP_ENUM cmd, uint8_t *buffer, uint8_t length)
{
    uint8_t data_buffer[30];

    UI_BT_TRACE("%d,%d.", cmd, length);

    if (length > sizeof(data_buffer) - 1)
    {
        return;
    }

    data_buffer[0] = (uint8_t)cmd;

    if (length > 0)
    {
        memcpy(data_buffer + 1, buffer, length);
    }

    zowee_ibrt_tws_exchange_info_send_with_rsp(data_buffer, length + 1);

}

void zowee_tws_send_local_info_to_peer_with_rsp(void)
{
	UI_BT_TRACE();
	UI_DEVICE_SYNC_STATUS_T peer_sync_status;
	memcpy((void *)&peer_sync_status, (void *)&ui_device_status.local_status, sizeof(UI_DEVICE_SYNC_STATUS_T));	
	UI_BT_TRACE("local ear wear:%d,ear status:%d battery level:%d",peer_sync_status.wear_status,peer_sync_status.ear_status,
			peer_sync_status.battery_level);
	if(app_tws_ibrt_tws_link_connected())
	{
		zowee_ibrt_tws_exchange_info_with_rsp(TWS_STATE,(uint8_t*)&peer_sync_status,sizeof(UI_DEVICE_SYNC_STATUS_T));
	}
}

bool zowee_tws_if_master_mode(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();

    if (IBRT_MASTER == p_ibrt_ctrl->current_role && !app_ibrt_ui_get_freeman_enable()) {
        UI_BT_TRACE(" Current mode is Master mode");
        return true;
    }

    return false;
}


//bes ibrt ui tws(master and slave) switch
static void app_zowee_ibrt_ui_tws_switch(void)
{
	UI_BT_TRACE("mobile link:%d, tws link:%d, master: %d",app_tws_ibrt_mobile_link_connected(), app_tws_ibrt_tws_link_connected(), zowee_tws_if_master_mode());
	if(app_tws_ibrt_mobile_link_connected() && app_tws_ibrt_tws_link_connected())
	{
		bool ret = app_ibrt_ui_tws_switch();
		UI_BT_TRACE("now role switch: %d, master mode: %d !!!", ret, zowee_tws_if_master_mode());
	}
}

static void ui_clean_peer_status(void)
{
	ui_device_status.peer_status.battery_level = 0xff;
	ui_device_status.peer_status.wear_status = UI_WEAR_OFF;
}

void ui_manage_bt_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{	
	ibrt_config_t config;
	app_ibrt_if_config_load(&config);
	ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
	
	UI_BT_TRACE(":id:%d nv_role:%d current_role:%d",msg_body->id,config.nv_role,p_ibrt_ctrl->current_role);
	
	switch(msg_body->id)
	{
		case ZOWEE_BT_TWS_DISCONNECTED:
		{
			UI_BT_TRACE("zowee BT TWS disconnected!!!");
			ui_start_disconncet_shutdown_timer();

#if 0//ndef	__APP_WEAR_ENABLE__
			zowee_anc_status_change_when_tws_status_change(0);
#endif
		}
		ui_clean_peer_status();
		app_ui_event_process(UI_EVENT_CHARGING,0,UI_EVENT_BATTERY_LEVEL_REPROT,0,0);
		break;

		case ZOWEE_BT_TWS_CONNECTED:
		{
			UI_BT_TRACE("zowee BT TWS connected!!!");
			
			
			ui_start_disconncet_shutdown_timer();

#if 0//ndef	__APP_WEAR_ENABLE__
			zowee_anc_status_change_when_tws_status_change(1);
#endif			
			zowee_tws_send_local_info_to_peer();
		}
		break;
		
		case ZOWEE_BT_DISCONNECTED:
		{
			UI_BT_TRACE("zowee BT disconnected!!! peer_inbox_enter_freeman_flag:%d",peer_inbox_enter_freeman_flag);
			if(peer_inbox_enter_freeman_flag == true)
			{
				app_tws_ibrt_set_access_mode(BTIF_BAM_NOT_ACCESSIBLE);
			}

			if(p_ibrt_ctrl->current_role)
				app_voice_report(APP_STATUS_INDICATION_DISCONNECTED, 0);

			ui_start_disconncet_shutdown_timer();
		}
		break;

		case ZOWEE_BT_CONNECTED:
		{
			UI_BT_TRACE("zowee BT connected!!!");

			app_voice_report(APP_STATUS_INDICATION_CONNECTED, 0);
			//if(config.nv_role == IBRT_MASTER)
			peer_inbox_enter_freeman_flag = false;

			ui_start_disconncet_shutdown_timer();

			zowee_tws_send_local_info_to_peer();
			app_ui_event_process(UI_EVENT_CHARGING,0,UI_EVENT_BATTERY_LEVEL_REPROT,0,0);
		}
		break;
		
		case ZOWEE_BT_RECONNECT:
		{

		}
		break;

		case ZOWEE_BT_TWS_SWITCH:
		{
			app_zowee_ibrt_ui_tws_switch();
		}
		break;
		
		default:
		UI_BT_TRACE("bt event do not define");
		break;
	}
}
