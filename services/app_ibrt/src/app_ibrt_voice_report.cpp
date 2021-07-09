/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "cmsis_os.h"
#include <string.h>
#include "me_api.h"
#include "app_tws_ibrt_trace.h"
#include "audioflinger.h"
#include "app_bt_stream.h"
#include "app_media_player.h"
#include "app_bt_media_manager.h"
#include "bt_drv_interface.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_tws_ctrl_thread.h"
#include "app_ibrt_voice_report.h"
#include "app_bt.h"
#ifdef TWS_PROMPT_SYNC
#include "audio_prompt_sbc.h"
#endif
#include "app_audio.h"

#ifdef __APP_ZOWEE_UI__
#include "ui_event_manage.h"
#include "app_battery.h"
#include "battery_zt102.h"
#include "battery_zt203.h"
#endif



#if defined(IBRT)
extern uint8_t g_findme_fadein_vol;

typedef uint8_t voice_report_role_t;
#define VOICE_REPORT_MASTER   0x00
#define VOICE_REPORT_SLAVE    0x01
#define VOICE_REPORT_LOCAL    0x02


#define APP_PLAY_AUDIO_SYNC_DELAY_US         (250*1000)
#define APP_PLAY_AUDIO_SYNC_TRIGGER_TIMEROUT (APP_PLAY_AUDIO_SYNC_DELAY_US/1000*2)

static uint32_t app_ibrt_voice_tg_tick = 0;
static uint32_t app_ibrt_voice_trigger_enable = 0;

static void app_ibrt_voice_report_trigger_timeout_cb(void const *n);
osTimerDef(APP_IBRT_VOICE_REPORT_TRIGGER_TIMEOUT, app_ibrt_voice_report_trigger_timeout_cb); 
osTimerId app_ibrt_voice_report_trigger_timeout_id = NULL;

static void app_ibrt_voice_report_trigger_timeout_cb(void const *n)
{    
    TRACE(1,"[%s]-->APP_BT_SETTING_CLOSE", __func__);
    app_play_audio_stop();
}

static int app_ibrt_voice_report_trigger_checker_init(void)
{
    if (app_ibrt_voice_report_trigger_timeout_id == NULL){
        app_ibrt_voice_report_trigger_timeout_id = osTimerCreate(osTimer(APP_IBRT_VOICE_REPORT_TRIGGER_TIMEOUT), osTimerOnce, NULL);
    }
    return 0;
}

static int app_ibrt_voice_report_trigger_checker_start(void)
{
    app_ibrt_voice_trigger_enable = true;
    osTimerStart(app_ibrt_voice_report_trigger_timeout_id, APP_PLAY_AUDIO_SYNC_TRIGGER_TIMEROUT);
    return 0;
}

static int app_ibrt_voice_report_trigger_checker_stop(void)
{
    app_ibrt_voice_trigger_enable = false;
    osTimerStop(app_ibrt_voice_report_trigger_timeout_id);
    return 0;
}

int app_ibrt_voice_report_trigger_checker(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (!p_ibrt_ctrl->init_done){
        return 0;
    }

    if (app_ibrt_voice_trigger_enable){
        TRACE(1,"[%s] trigger ok", __func__);
        app_ibrt_voice_trigger_enable = false;
        osTimerStop(app_ibrt_voice_report_trigger_timeout_id);
    }
    return 0;
}

voice_report_role_t app_ibrt_voice_report_get_role(void)
{
    voice_report_role_t report_role = VOICE_REPORT_LOCAL;
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if (app_ibrt_ui_is_profile_exchanged()){
        if (app_tws_ibrt_mobile_link_connected()){
            report_role = VOICE_REPORT_MASTER;
        }else if (app_tws_ibrt_slave_ibrt_link_connected()){
            report_role = VOICE_REPORT_SLAVE;
        }else{
            TRACE(1, "[%s] skip it (profile_exchanged)", __func__);
        }
    }else if (app_tws_ibrt_tws_link_connected()){
        if (app_tws_ibrt_mobile_link_connected()){
            report_role = VOICE_REPORT_MASTER;
        }else if(p_ibrt_ctrl->current_role == IBRT_MASTER){
            report_role = VOICE_REPORT_MASTER;
        }else{
            report_role = VOICE_REPORT_SLAVE;
        }

    }else{
        TRACE(1, "[%s] loc only", __func__);
    }
    
    TRACE(2, "[%s] report_role: %d", __func__, report_role);
    return report_role;
}

static void app_ibrt_voice_report_set_trigger_time(uint32_t tg_tick)
{
    if (tg_tick){
        ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();
        btif_connection_role_t connection_role =  app_tws_ibrt_get_local_tws_role();
        btdrv_syn_trigger_codec_en(0);
        btdrv_syn_clr_trigger();
        btdrv_enable_playback_triggler(ACL_TRIGGLE_MODE);
        if (connection_role == BTIF_BCR_MASTER){
            bt_syn_set_tg_ticks(tg_tick, p_ibrt_ctrl->tws_conhandle, BT_TRIG_MASTER_ROLE);
        }else if (connection_role == BTIF_BCR_SLAVE){ 
            bt_syn_set_tg_ticks(tg_tick, p_ibrt_ctrl->tws_conhandle, BT_TRIG_SLAVE_ROLE);
        }
        btdrv_syn_trigger_codec_en(1);
        app_ibrt_voice_report_trigger_checker_start();
        TRACE(2,"[%s] set trigger tg_tick:%08x", __func__, tg_tick);
    }else{
        btdrv_syn_trigger_codec_en(0);
        btdrv_syn_clr_trigger();
        app_ibrt_voice_report_trigger_checker_stop();
        TRACE(1,"[%s] trigger clear", __func__);
    }
}

int app_ibrt_voice_report_trigger_init(uint32_t aud_id, uint32_t aud_pram)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (!p_ibrt_ctrl->init_done){
        TRACE(0,"ibrt not init done.");
        return 0;
    }

    app_ibrt_voice_report_trigger_checker_init();
    if (app_tws_ibrt_tws_link_connected()){
        ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();
        voice_report_role_t report_role =  app_ibrt_voice_report_get_role();
        uint32_t curr_ticks = 0;
        uint32_t tg_tick = 0;
        app_ibrt_voice_report_trigger_deinit();

        curr_ticks = bt_syn_get_curr_ticks(p_ibrt_ctrl->tws_conhandle);

        if (report_role == VOICE_REPORT_MASTER){
            app_ibrt_voice_report_info_t voice_report_info;

            tg_tick = curr_ticks + US_TO_BTCLKS(APP_PLAY_AUDIO_SYNC_DELAY_US);
#ifdef PLAYBACK_USE_I2S
            af_i2s_sync_config(AUD_STREAM_PLAYBACK, AF_I2S_SYNC_TYPE_BT, false);
            af_i2s_sync_config(AUD_STREAM_PLAYBACK, AF_I2S_SYNC_TYPE_BT, true);
#else
            af_codec_sync_config(AUD_STREAM_PLAYBACK, AF_CODEC_SYNC_TYPE_BT, false);
            af_codec_sync_config(AUD_STREAM_PLAYBACK, AF_CODEC_SYNC_TYPE_BT, true);
#endif
            app_ibrt_voice_report_set_trigger_time(tg_tick);
            voice_report_info.aud_id = aud_id;
            voice_report_info.aud_pram = aud_pram;
            voice_report_info.tg_tick = tg_tick;
#ifdef __INTERACTION__          
            voice_report_info.vol = g_findme_fadein_vol;
#endif
            if (p_ibrt_ctrl->tws_mode == IBRT_SNIFF_MODE)
            {
                TRACE(0,"ibrt_ui_log:force exit_sniff_with_tws\n");
                app_tws_ibrt_exit_sniff_with_tws();
            }
			//modify by lifeizhu 2021.01.16 begin
#ifdef __APP_ZOWEE_UI__ 
			if(!app_ibrt_if_voice_report_local_only(voice_report_info.aud_id))
            	tws_ctrl_send_cmd(APP_TWS_CMD_VOICE_REPORT_START, (uint8_t *)&voice_report_info, sizeof(app_ibrt_voice_report_info_t));
#else
			tws_ctrl_send_cmd(APP_TWS_CMD_VOICE_REPORT_START, (uint8_t *)&voice_report_info, sizeof(app_ibrt_voice_report_info_t));
#endif
			//modify by lifeizhu 2021.01.16 begin
            TRACE(4,"[%s] MASTER aud_id:%d trigger [%08x]-->[%08x]", __func__, aud_id, bt_syn_get_curr_ticks(p_ibrt_ctrl->tws_conhandle), tg_tick);
        }else if (report_role == VOICE_REPORT_SLAVE){
            tg_tick = app_ibrt_voice_tg_tick;
            if (curr_ticks < tg_tick && tg_tick != 0)
            {
#ifdef PLAYBACK_USE_I2S
                af_i2s_sync_config(AUD_STREAM_PLAYBACK, AF_I2S_SYNC_TYPE_BT, false);
                af_i2s_sync_config(AUD_STREAM_PLAYBACK, AF_I2S_SYNC_TYPE_BT, true);
#else
                af_codec_sync_config(AUD_STREAM_PLAYBACK, AF_CODEC_SYNC_TYPE_BT, false);
                af_codec_sync_config(AUD_STREAM_PLAYBACK, AF_CODEC_SYNC_TYPE_BT, true);
#endif
                app_ibrt_voice_report_set_trigger_time(tg_tick);
            }
            app_ibrt_voice_tg_tick = 0;
            if (tg_tick <= curr_ticks)
            {
			//modify by lifeizhu 2021.01.16 begin
#ifdef __APP_ZOWEE_UI__ 
				if(!app_ibrt_if_voice_report_local_only(aud_id))
                	app_audio_sendrequest(APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);
#else
				app_audio_sendrequest(APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);
#endif
                TRACE(1,"[%s] Prompt TRIGGER ERROR !!!", __func__);
			//modify by lifeizhu 2021.01.16 end
            }

            TRACE(4,"[%s] SLAVE aud_id:%d trigger [%08x]-->[%08x]", __func__, aud_id, bt_syn_get_curr_ticks(p_ibrt_ctrl->tws_conhandle), tg_tick);
        }
    }
    else
    {
        TRACE(0,"tws not connected.");
    }

    return 0;
}

int app_ibrt_voice_report_trigger_deinit(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (!p_ibrt_ctrl->init_done){
        return 0;
    }

#ifdef PLAYBACK_USE_I2S
    af_i2s_sync_config(AUD_STREAM_PLAYBACK, AF_I2S_SYNC_TYPE_BT, false);
#else
    af_codec_sync_config(AUD_STREAM_PLAYBACK, AF_CODEC_SYNC_TYPE_BT, false);
#endif
    app_ibrt_voice_report_set_trigger_time(0);

    return 0;
}

int app_ibrt_if_voice_report_filter(uint32_t aud_id)
{
    int nRet = 0;
    ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();

    switch (aud_id){
        case AUD_ID_BT_PAIRING_SUC:
            nRet = 1;
            break;
        default:
            break;
    }

    if (app_tws_ibrt_slave_ibrt_link_connected()){
        switch (aud_id){
            case AUD_ID_BT_CALL_INCOMING_CALL:
            case AUD_ID_BT_CALL_INCOMING_NUMBER:
            case AUD_ID_BT_CALL_OVER:
            case AUD_ID_BT_CALL_ANSWER:
            case AUD_ID_BT_CONNECTED:
            case AUD_ID_BT_CALL_HUNG_UP:
            case AUD_ID_BT_CALL_REFUSE:
                nRet = 1;
                break;
            default:
                break;
        }
    }

    if (!app_tws_ibrt_slave_ibrt_link_connected() &&
        !app_tws_ibrt_mobile_link_connected()){
        if (p_ibrt_ctrl->nv_role == IBRT_SLAVE){
            switch (aud_id){
                case AUD_ID_BT_CONNECTED:
                    nRet = 1;
                    break;
                default:
                    break;
            }
        }
    }

//modify by lifeizhu 2021.01.30 for power off begin
#ifdef __APP_ZOWEE_UI__ 
	switch (aud_id)
	{
		case AUD_ID_POWER_OFF:
		{
			TRACE(4,"[%s] nv_role: %d, current_role: %d, ibrt_shutdown_flag: %d", __func__, p_ibrt_ctrl->nv_role, p_ibrt_ctrl->current_role, ui_disconncet_status.ibrt_shutdown_flag);
			TRACE(4,"[%s] tws link: %d, slave link: %d, mobile link: %d ", __func__, app_tws_ibrt_tws_link_connected(), app_tws_ibrt_slave_ibrt_link_connected(), app_tws_ibrt_mobile_link_connected());

			if (app_tws_ibrt_tws_link_connected() && (p_ibrt_ctrl->current_role == IBRT_SLAVE))
			{
				uint16_t current_voltage;
	
				current_voltage = app_battery_get_current_voltage();
				TRACE(3,"[%s] current_voltage : %d, BATTERY_POWER_DOWN_VOLTAGE : %d", __func__, current_voltage, BATTERY_POWER_DOWN_VOLTAGE);
	
				if((!ui_disconncet_status.ibrt_shutdown_flag) && current_voltage > BATTERY_POWER_DOWN_VOLTAGE)
				{
					nRet = 1;
					TRACE(1,"[%s] AUTO_POWER_OFF_TIME nv slave skip voice report", __func__);
				}
			}
		}
			break;
		default:
			break;
	}
#endif
//modify by lifeizhu 2021.01.30 for power off end

    return nRet;
}

int app_ibrt_if_voice_report_local_only(uint32_t aud_id)
{
    int nRet = 0;

    //TRACE(3,"[%s] aud_id:%d,ibrt_shutdown_flag:%d", __func__,aud_id,ui_disconncet_status.ibrt_shutdown_flag);    

    switch (aud_id){
/*add by kyle 20210114*/
#ifdef __APP_ZOWEE_UI__
		case AUD_ID_POWER_OFF:
    	{
			//modify by lifeizhu 2021.01.16 begin
			TRACE(3,"[%s] ibrt_shutdown_flag: %d, tws link: %d", __func__, ui_disconncet_status.ibrt_shutdown_flag, app_tws_ibrt_tws_link_connected());
			//TRACE(4,"[%s] slave link: %d, mobile link: %d ", __func__, app_tws_ibrt_slave_ibrt_link_connected(), app_tws_ibrt_mobile_link_connected());
			if((!ui_disconncet_status.ibrt_shutdown_flag) && (app_tws_ibrt_tws_link_connected()))
			{
				nRet = 1;
			}
			//modify by lifeizhu 2021.01.16 end
		}
		break;
		case AUD_ID_BT_CHARGE_PLEASE:
		{
			nRet = 1;
		}
		break;
		case AUD_ID_WEAR_ON:
			nRet = 1;
			break;
		case AUD_ID_WEAR_OFF:
			nRet = 1;
			break;
#endif
/*add by kyle 20210114*/
        default:
            break;
    }

    return nRet;
}

int app_ibrt_if_voice_report_force_audio_retrigger_hander(uint32_t msg_id, uint32_t aud_id)
{
    if (msg_id == APP_BT_STREAM_MANAGER_STOP_MEDIA){
        if (aud_id == AUDIO_ID_BT_MUTE){
            TRACE(1,"[%s]", __func__);
#ifdef MEDIA_PLAYER_SUPPORT
            trigger_media_play((AUD_ID_ENUM)aud_id, 0, PROMOT_ID_BIT_MASK_CHNLSEl_ALL);
#endif
        }
    }
    return 0;
}

static int app_ibrt_if_voice_report_init(uint32_t aud_id, uint16_t aud_pram, uint8_t init_play)
{
#ifdef MEDIA_PLAYER_SUPPORT
    

    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (!p_ibrt_ctrl->init_done)
    {
    	TRACE(0,"ibrt init not done!");
        trigger_media_play((AUD_ID_ENUM)aud_id, 0, aud_pram);
    }
    else if (app_ibrt_if_voice_report_filter(aud_id) && init_play)
    {
        TRACE(3,"[%s] skip aud_id:%d%s", __func__, aud_id, aud_id2str(aud_id));
    }
    else if (app_ibrt_if_voice_report_local_only(aud_id))
    {
    	TRACE(3,"[%s] local only aud_id:%d%s", __func__, aud_id, aud_id2str(aud_id));
        trigger_media_play((AUD_ID_ENUM)aud_id, 0, aud_pram);
    }
    else if (!app_tws_ibrt_tws_link_connected())
    {
		TRACE(1,"[%s] tws link disconnected!", __func__);
        trigger_media_play((AUD_ID_ENUM)aud_id, 0, aud_pram);
    }
    else
    {
        voice_report_role_t report_role =  app_ibrt_voice_report_get_role();

        app_ibrt_if_tws_sniff_block(5);
        if (report_role == VOICE_REPORT_MASTER)
        {
            TRACE(4,"[%s] loc aud_id:%d%s aud_pram:%04x", __func__, aud_id, aud_id2str(aud_id), aud_pram);

            if (aud_id == AUDIO_ID_BT_MUTE)
            {
#ifdef TWS_PROMPT_SYNC
                audio_prompt_stop_playing();
#endif
                app_audio_manager_sendrequest_need_callback(APP_BT_STREAM_MANAGER_STOP_MEDIA,
                                                            BT_STREAM_MEDIA,
                                                            BT_DEVICE_ID_1,
                                                            0,
                                                            ( uint32_t )app_ibrt_if_voice_report_force_audio_retrigger_hander,
                                                            AUDIO_ID_BT_MUTE);
            }
            else
            {
                trigger_media_play((AUD_ID_ENUM)aud_id, 0, aud_pram);
            }
        }
        else if (report_role == VOICE_REPORT_SLAVE)
        {
            app_ibrt_voice_report_request_t voice_report_request;
            TRACE(4,"[%s] rmt aud_id:%d%s aud_pram:%04x", __func__, aud_id, aud_id2str(aud_id), aud_pram);
            app_ibrt_voice_tg_tick = 0;
            if (aud_id == AUDIO_ID_BT_MUTE)
            {
#ifdef TWS_PROMPT_SYNC
                audio_prompt_stop_playing();
#endif
                app_audio_manager_sendrequest_need_callback(APP_BT_STREAM_MANAGER_STOP_MEDIA,
                                                            BT_STREAM_MEDIA,
                                                            BT_DEVICE_ID_1,
                                                            0,
                                                            0,
                                                            0);
            }

            voice_report_request.aud_id = aud_id;
            voice_report_request.aud_pram = aud_pram;
            voice_report_request.vol = 0;
            if((aud_id & AUDIO_ID_FIND_MY_BUDS) == AUDIO_ID_FIND_MY_BUDS)
            {
                trigger_media_play(( AUD_ID_ENUM )aud_id, 0, aud_pram);
            }
            else
                tws_ctrl_send_cmd(APP_TWS_CMD_VOICE_REPORT_REQUEST, (uint8_t *)&voice_report_request, sizeof(voice_report_request));
                 
        }
        else
        {
            TRACE(3, "[%s] loc only aud_id:%d aud_pram:%04x", __func__, aud_id, aud_pram);
#ifdef MEDIA_PLAYER_SUPPORT
            trigger_media_play((AUD_ID_ENUM)aud_id, 0, aud_pram);
#endif
        }
    }
#endif
    return 0;
}

int app_ibrt_if_voice_report_trig_retrigger(void)
{
    return app_ibrt_if_voice_report_handler(AUDIO_ID_BT_MUTE, PROMOT_ID_BIT_MASK_CHNLSEl_ALL);
}

int app_ibrt_if_voice_report_handler(uint32_t aud_id, uint16_t aud_pram)
{
    return app_ibrt_if_voice_report_init(aud_id, aud_pram, true);
}

void app_ibrt_send_voice_report_request_req(uint8_t *p_buff, uint16_t length)
{
    voice_report_role_t report_role =  app_ibrt_voice_report_get_role();

    if (report_role == VOICE_REPORT_SLAVE)
    {
        app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_VOICE_REPORT_REQUEST, p_buff, length);
    }
}

void app_ibrt_voice_report_request_req_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    voice_report_role_t report_role =  app_ibrt_voice_report_get_role();

    if (report_role == VOICE_REPORT_MASTER)
    {
        app_ibrt_voice_report_request_t *voice_report_request = (app_ibrt_voice_report_request_t *)p_buff;
        app_ibrt_if_voice_report_init(voice_report_request->aud_id, voice_report_request->aud_pram, false);
    }
    else
    {
        TRACE(2, "%s role is :%d", __func__, report_role);
    }
}

void app_ibrt_send_voice_report_start_req(uint8_t *p_buff, uint16_t length)
{
    voice_report_role_t report_role =  app_ibrt_voice_report_get_role();

    if (report_role == VOICE_REPORT_MASTER)

    {
        app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_VOICE_REPORT_START, p_buff, length);
    }
    else
    {
        TRACE(2, "%s role is :%d", __func__, report_role);
    }
}

void app_ibrt_voice_report_request_start_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
#ifdef MEDIA_PLAYER_SUPPORT
    voice_report_role_t report_role =  app_ibrt_voice_report_get_role();

	//TRACE(3,"[%s] slave link:%d, mobile link:%d ", __func__, app_tws_ibrt_slave_ibrt_link_connected(), app_tws_ibrt_mobile_link_connected());

    if (report_role == VOICE_REPORT_SLAVE)
    {
        app_ibrt_voice_report_info_t *voice_report_info = (app_ibrt_voice_report_info_t *)p_buff;
        app_ibrt_voice_tg_tick = voice_report_info->tg_tick;
#ifdef __INTERACTION__          
        g_findme_fadein_vol = voice_report_info->vol;
#endif

        if (voice_report_info->aud_id == AUDIO_ID_BT_MUTE)
        {
            app_audio_manager_sendrequest_need_callback(APP_BT_STREAM_MANAGER_STOP_MEDIA,
                                                        BT_STREAM_MEDIA,
                                                        BT_DEVICE_ID_1,
                                                        0,
                                                        ( uint32_t )app_ibrt_if_voice_report_force_audio_retrigger_hander,
                                                        AUDIO_ID_BT_MUTE);
        }
        else
        {
            TRACE(2,"%s aud id:%d", __func__,voice_report_info->aud_id);
            trigger_media_play((AUD_ID_ENUM)voice_report_info->aud_id, 0, voice_report_info->aud_pram);
        }
    }
    else
    {
        TRACE(2, "%s role is :%d", __func__, report_role);
    }
#endif
}

bool app_ibrt_voice_report_is_me(uint32_t voice_chnlsel)
{
    bool is_me = true;
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    switch ((AUDIO_CHANNEL_SELECT_E)p_ibrt_ctrl->audio_chnl_sel)
    {
        case AUDIO_CHANNEL_SELECT_LCHNL:
            if (IS_PROMPT_CHNLSEl_RCHNL(voice_chnlsel)){
                is_me = false;
            }
            break;
        case AUDIO_CHANNEL_SELECT_RCHNL:
            if (IS_PROMPT_CHNLSEl_LCHNL(voice_chnlsel)){
                is_me = false;
            }
            break;
        default:
            break;
    }
    return is_me;
}
#endif

