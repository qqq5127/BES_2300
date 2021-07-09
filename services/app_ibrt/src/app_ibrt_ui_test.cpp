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
#include <string.h>
#include "app_tws_ibrt_trace.h"
#include "factory_section.h"
#include "apps.h"
#include "app_battery.h"
#include "app_anc.h"
#include "app_key.h"
#include "app_ibrt_if.h"
#include "app_ibrt_ui_test.h"
#include "app_ibrt_auto_test.h"
#include "app_ibrt_peripheral_manager.h"
#include "a2dp_decoder.h"
#include "app_ibrt_keyboard.h"
#include "nvrecord_env.h"
#include "nvrecord_ble.h"
#include "app_tws_if.h"
#include "besbt.h"
#include "app_bt.h"
#include "app_ai_if.h"
#include "app_ai_manager_api.h"
#include "nvrecord_extension.h"
#include "app_ibrt_auto_test.h"
#include "app_key.h"
#include "app_ble_mode_switch.h"
#include "app.h"
#include "norflash_api.h"

#if defined(BISTO_ENABLED)
#include "gsound_custom_actions.h"
#include "gsound_custom_ble.h"
#include "gsound_custom_bt.h"
#endif

#ifdef __AI_VOICE__
#include "ai_spp.h"
#include "ai_thread.h"
#include "app_ai_ble.h"
#endif

#ifdef IBRT_OTA
#include "ota_control.h"
#endif

void app_voice_assistant_key(APP_KEY_STATUS *status, void *param);
extern void app_bt_volumedown();
extern void app_bt_volumeup();
#ifdef IBRT_OTA
extern uint8_t ota_role_switch_flag;
extern uint8_t avoid_even_packets_protect_flag;
#endif
#if defined(IBRT)
#include "btapp.h"
extern struct BT_DEVICE_T  app_bt_device;

bt_bdaddr_t master_ble_addr = {0x76, 0x33, 0x33, 0x22, 0x11, 0x11};
bt_bdaddr_t slave_ble_addr  = {0x77, 0x33, 0x33, 0x22, 0x11, 0x11};
bt_bdaddr_t box_ble_addr    = {0x78, 0x33, 0x33, 0x22, 0x11, 0x11};

#ifdef IBRT_SEARCH_UI
void app_ibrt_battery_callback(APP_BATTERY_MV_T currvolt, uint8_t currlevel,enum APP_BATTERY_STATUS_T curstatus,uint32_t status, union APP_BATTERY_MSG_PRAMS prams);
void app_ibrt_simulate_charger_plug_in_test(void)
{
    union APP_BATTERY_MSG_PRAMS msg_prams;
    msg_prams.charger = APP_BATTERY_CHARGER_PLUGIN;
    app_ibrt_battery_callback(0, 0, APP_BATTERY_STATUS_CHARGING, 1, msg_prams);
}
void app_ibrt_simulate_charger_plug_out_test(void)
{
    union APP_BATTERY_MSG_PRAMS msg_prams;
    msg_prams.charger = APP_BATTERY_CHARGER_PLUGOUT;
    app_ibrt_battery_callback(0, 0, APP_BATTERY_STATUS_CHARGING, 1, msg_prams);
}
void app_ibrt_simulate_charger_plug_box_test(void)
{
    static int count = 0;
    if (count++ % 2 == 0)
    {
        app_ibrt_simulate_charger_plug_in_test();
    }
    else
    {
        app_ibrt_simulate_charger_plug_out_test();
    }
}
extern void app_ibrt_sync_volume_info();

void app_ibrt_search_ui_gpio_key_handle(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s,event:%d,code:%d",__func__,status->event,status->code);
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (IBRT_SLAVE == p_ibrt_ctrl->current_role && status->event == APP_KEY_EVENT_DOUBLECLICK)
    {
        app_ibrt_if_keyboard_notify(status,param);
    }
    else
    {
        switch (status->event)
        {
            case APP_KEY_EVENT_CLICK:
                if (status->code == APP_KEY_CODE_FN1)
                {
                    app_ibrt_simulate_charger_plug_in_test();
                }
                else if(status->code == APP_KEY_CODE_FN2)
                {
                    app_ibrt_simulate_charger_plug_out_test();
                }
                else
                {

                }
                break;
            case APP_KEY_EVENT_DOUBLECLICK:
                if (status->code == APP_KEY_CODE_FN1)
                {
                    app_bt_volumeup();
                    if(IBRT_MASTER==p_ibrt_ctrl->current_role)
                    {
                        TRACE(0,"ibrt master sync volume up to slave !");
                        app_ibrt_sync_volume_info();
                    }

                }
                else if(status->code == APP_KEY_CODE_FN2)
                {
                    app_bt_volumedown();
                    if(IBRT_MASTER==p_ibrt_ctrl->current_role)
                    {
                        TRACE(0,"ibrt master sync volume up to slave !");
                        app_ibrt_sync_volume_info();
                    }
                }
                else
                {

                }
                break;
            case APP_KEY_EVENT_LONGPRESS:
                if (status->code == APP_KEY_CODE_FN1)
                {
#ifdef TPORTS_KEY_COEXIST
                    app_ibrt_simulate_charger_plug_out_test();
#else
                    app_ibrt_ui_tws_switch();
#endif
                }
                else if(status->code == APP_KEY_CODE_FN2)
                {

                }
                else
                {

                }
                break;
            default:
                break;
        }

    }




}




#endif
void app_ibrt_ui_audio_play_test(void)
{
    uint8_t action[] = {IBRT_ACTION_PLAY};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_audio_pause_test(void)
{
    uint8_t action[] = {IBRT_ACTION_PAUSE};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_audio_forward_test(void)
{
    uint8_t action[] = {IBRT_ACTION_FORWARD};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_audio_backward_test(void)
{
    uint8_t action[] = {IBRT_ACTION_BACKWARD};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_avrcp_volume_up_test(void)
{
    uint8_t action[] = {IBRT_ACTION_AVRCP_VOLUP};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_avrcp_volume_down_test(void)
{
    uint8_t action[] = {IBRT_ACTION_AVRCP_VOLDN};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_hfsco_create_test(void)
{
    uint8_t action[] = {IBRT_ACTION_HFSCO_CREATE};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_hfsco_disc_test(void)
{
    uint8_t action[] = {IBRT_ACTION_HFSCO_DISC};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_call_redial_test(void)
{
    uint8_t action[] = {IBRT_ACTION_REDIAL};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_call_answer_test(void)
{
    uint8_t action[] = {IBRT_ACTION_ANSWER};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_call_hangup_test(void)
{
    uint8_t action[] = {IBRT_ACTION_HANGUP};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_local_volume_up_test(void)
{
    uint8_t action[] = {IBRT_ACTION_LOCAL_VOLUP};
    app_ibrt_if_start_user_action(action, sizeof(action));
}
void app_ibrt_ui_local_volume_down_test(void)
{
    uint8_t action[] = {IBRT_ACTION_LOCAL_VOLDN};
    app_ibrt_if_start_user_action(action, sizeof(action));
}

void app_ibrt_ui_get_tws_conn_state_test(void)
{
    if(app_tws_ibrt_tws_link_connected())
    {
        TRACE(0, "ibrt_ui_log:TWS CONNECTED");
    }
    else
    {
        TRACE(0, "ibrt_ui_log:TWS DISCONNECTED");
    }
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_soft_reset_test
 Description  : soft reset device
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/7/7
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
static void app_ibrt_ui_soft_reset_test(void)
{
    app_reset();
}



extern void bt_change_to_iic(APP_KEY_STATUS *status, void *param);
void app_ibrt_ui_iic_uart_switch_test(void)
{
    bt_change_to_iic(NULL,NULL);
}

void app_ibrt_ui_get_a2dp_state_test(void)
{
   const char* a2dp_state_strings[] = {"IDLE", "CODEC_CONFIGURED", "OPEN","STREAMING","CLOSED","ABORTING"};
   AppIbrtA2dpState a2dp_state;

   AppIbrtStatus  status = app_ibrt_if_get_a2dp_state(&a2dp_state);
   if(APP_IBRT_IF_STATUS_SUCCESS == status)
    {
        TRACE(1, "ibrt_ui_log:a2dp_state=%s",a2dp_state_strings[a2dp_state]);
    }
    else
    {
        TRACE(0, "ibrt_ui_log:get a2dp state error");
    }
}

void app_ibrt_ui_get_avrcp_state_test(void)
{
    const char* avrcp_state_strings[] = {"DISCONNECTED",  "CONNECTED", "PLAYING","PAUSED", "VOLUME_UPDATED"};
    AppIbrtAvrcpState avrcp_state;

    AppIbrtStatus status = app_ibrt_if_get_avrcp_state(&avrcp_state);
    if(APP_IBRT_IF_STATUS_SUCCESS == status)
    {
        TRACE(1,"ibrt_ui_log:avrcp_state=%s",avrcp_state_strings[avrcp_state]);
    }
    else
    {
        TRACE(0,"ibrt_ui_log:get avrcp state error");
    }
}


void app_ibrt_ui_get_hfp_state_test(void)
{
    const char* hfp_state_strings[] = {"SLC_DISCONNECTED",  "CLOSED", "SCO_CLOSED","PENDING","SLC_OPEN", \
                            "NEGOTIATE","CODEC_CONFIGURED","SCO_OPEN","INCOMING_CALL","OUTGOING_CALL","RING_INDICATION"};
    AppIbrtHfpState hfp_state;
    AppIbrtStatus status = app_ibrt_if_get_hfp_state(&hfp_state);

    if(APP_IBRT_IF_STATUS_SUCCESS == status)
    {
        TRACE(1, "ibrt_ui_log:hfp_state=%s",hfp_state_strings[hfp_state]);
    }
    else
    {
        TRACE(0, "ibrt_ui_log:get hfp state error");
    }
}


void app_ibrt_ui_get_call_status_test(void)
{
    const char* call_status_strings[] = {"NO_CALL","CALL_ACTIVE","HOLD","INCOMMING","OUTGOING","ALERT"};
    AppIbrtCallStatus call_status;

    AppIbrtStatus status = app_ibrt_if_get_hfp_call_status(&call_status);
    if(APP_IBRT_IF_STATUS_SUCCESS == status)
    {
        TRACE(1, "ibrt_ui_log:call_status=%s",call_status_strings[call_status]);
    }
    else
    {
        TRACE(0, "ibrt_ui_log:get call status error");
    }
}


void app_ibrt_ui_get_ibrt_role_test(void)
{
    ibrt_role_e role = app_ibrt_if_get_ibrt_role();

    if(IBRT_MASTER == role)
    {
        TRACE(0, "ibrt_ui_log:ibrt role is MASTER");
    }
    else if(IBRT_SLAVE == role)
    {
        TRACE(0, "ibrt_ui_log:ibrt role is SLAVE");
    }
    else
    {
        TRACE(0, "ibrt_ui_log:ibrt role is UNKNOW");
    }
}


#if defined(ENHANCED_STACK)
#define IBRT_ENHANCED_STACK_PTS
#include "bt_if.h"
static bt_bdaddr_t pts_bt_addr = {{
#if 0
        0x14, 0x71, 0xda, 0x7d, 0x1a, 0x00
#else
        0x13, 0x71, 0xda, 0x7d, 0x1a, 0x00
#endif
    }
};
void btif_pts_hf_create_link_with_pts(void)
{
    btif_pts_hf_create_service_link(&pts_bt_addr);
}
void btif_pts_av_create_channel_with_pts(void)
{
    btif_pts_av_create_channel(&pts_bt_addr);
}
void btif_pts_ar_connect_with_pts(void)
{
    btif_pts_ar_connect(&pts_bt_addr);
}
#endif
/*****************************************************************************
 Prototype    : app_ibrt_ui_open_box_event_test
 Description  : app ibrt ui open box test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_open_box_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_OPEN_BOX_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_fetch_out_box_event_test
 Description  : fetch out box test function
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_fetch_out_box_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_FETCH_OUT_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_put_in_box_event_test
 Description  : app ibrt put in box test function
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_put_in_box_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_PUT_IN_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_close_box_event_test
 Description  : app ibrt close box test function
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_close_box_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_CLOSE_BOX_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_reconnect_event_test
 Description  : app ibrt reconnect function test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_reconnect_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_RECONNECT_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_ware_up_event_test
 Description  : app ibrt wear up function test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_ware_up_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_WEAR_UP_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_ware_down_event_test
 Description  : app ibrt wear down function test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_ware_down_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_WEAR_DOWN_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_phone_connect_event_test
 Description  : app ibrt ui phone connect event
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/4/3
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_phone_connect_event_test(void)
{
    app_ibrt_if_event_entry(IBRT_PHONE_CONNECT_TEST_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_shut_down_test
 Description  : shut down test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/4/10
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_shut_down_test(void)
{
    app_shutdown();
}

/*****************************************************************************
 Prototype    : app_ibrt_ui_reboot_test
 Description  : system reboot test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/5/8
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_reboot_test(void)
{
    app_reset();
}

/*****************************************************************************
 Prototype    : app_ibrt_ui_factory_reset_test
 Description  : Factory reset test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/5/8
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_factory_reset_test(void)
{
    nv_record_rebuild();
    app_reset();
}


/*****************************************************************************
 Prototype    : app_ibrt_ui_tws_swtich_test
 Description  : tws switch test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/4/10
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
extern bt_status_t btif_me_ibrt_role_switch(uint8_t switch_op);
void app_ibrt_ui_tws_swtich_test(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (p_ibrt_ctrl->current_role == IBRT_MASTER)
    {
        btif_me_ibrt_role_switch(IBRT_SWITCH);
    }
    else
    {
        TRACE(0,"ibrt_ui_log:local role is ibrt_slave, pls send tws switch in another dev");
    }
}

/*****************************************************************************
 Prototype    : app_ibrt_ui_suspend_ibrt_test
 Description  : suspend ibrt fastack
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/4/24
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_suspend_ibrt_test(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (p_ibrt_ctrl->current_role== IBRT_MASTER)
    {
        btif_me_suspend_ibrt();
    }
    else
    {
        TRACE(0,"ibrt_ui_log:local role is ibrt_slave, suspend ibrt failed");
    }
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_resume_ibrt_test
 Description  : resume ibrt test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/4/27
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_resume_ibrt_test(void)
{
    btif_me_resume_ibrt(1);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_tws_pairing_test
 Description  : tws pairing mode test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/11/20
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_tws_pairing_test(void)
{
    app_ibrt_ui_event_entry(IBRT_TWS_PAIRING_EVENT);
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_freeman_pairing_test
 Description  : ibrt freeman pairing mode test
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/11/21
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_freeman_pairing_test(void)
{
    app_ibrt_ui_event_entry(IBRT_FREEMAN_PAIRING_EVENT);
}

/*****************************************************************************
 Prototype    : app_tws_ibrt_disconnect_tws
 Description  : disconnect tws
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/4/24
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_disconnect_tws(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    btif_remote_device_t *remdev = NULL;

    if(app_tws_ibrt_tws_link_connected())
    {
        remdev = btif_me_get_remote_device_by_handle(p_ibrt_ctrl->tws_conhandle);

        if (remdev != NULL)
        {
            app_tws_ibrt_disconnect_connection(remdev);

        }
    }

}

void app_ibrt_inquiry_start_test(void)
{
    app_bt_start_search();
}

const app_uart_handle_t app_ibrt_uart_test_handle[]=
{
    {"inquiry_start_test",app_ibrt_inquiry_start_test},
    {"open_box_event_test",app_ibrt_ui_open_box_event_test},
    {"fetch_out_box_event_test",app_ibrt_ui_fetch_out_box_event_test},
    {"put_in_box_event_test",app_ibrt_ui_put_in_box_event_test},
    {"close_box_event_test",app_ibrt_ui_close_box_event_test},
    {"reconnect_event_test",app_ibrt_ui_reconnect_event_test},
    {"wear_up_event_test",app_ibrt_ui_ware_up_event_test},
    {"wear_down_event_test",app_ibrt_ui_ware_down_event_test},
    {"shut_down_test",app_ibrt_ui_shut_down_test},
    {"phone_connect_event_test",app_ibrt_ui_phone_connect_event_test},
    {"switch_ibrt_test",app_ibrt_ui_tws_swtich_test},
    {"suspend_ibrt_test",app_ibrt_ui_suspend_ibrt_test},
    {"resume_ibrt_test",app_ibrt_ui_resume_ibrt_test},
    {"conn_second_mobile_test",app_ibrt_ui_choice_connect_second_mobile},
    {"mobile_tws_disc_test",app_ibrt_if_disconnect_mobile_tws_link},
    {"pairing_mode_test",app_ibrt_ui_tws_pairing_test},
    {"freeman_mode_test",app_ibrt_ui_freeman_pairing_test},
    {"audio_play",app_ibrt_ui_audio_play_test},
    {"audio_pause",app_ibrt_ui_audio_pause_test},
    {"audio_forward",app_ibrt_ui_audio_forward_test},
    {"audio_backward",app_ibrt_ui_audio_backward_test},
    {"avrcp_volup",app_ibrt_ui_avrcp_volume_up_test},
    {"avrcp_voldn",app_ibrt_ui_avrcp_volume_down_test},
    {"hfsco_create",app_ibrt_ui_hfsco_create_test},
    {"hfsco_disc",app_ibrt_ui_hfsco_disc_test},
    {"call_redial",app_ibrt_ui_call_redial_test},
    {"call_answer",app_ibrt_ui_call_answer_test},
    {"call_hangup",app_ibrt_ui_call_hangup_test},
    {"volume_up",app_ibrt_ui_local_volume_up_test},
    {"volume_down",app_ibrt_ui_local_volume_down_test},
    {"get_a2dp_state",app_ibrt_ui_get_a2dp_state_test},
    {"get_avrcp_state",app_ibrt_ui_get_avrcp_state_test},
    {"get_hfp_state",app_ibrt_ui_get_hfp_state_test},
    {"get_call_status",app_ibrt_ui_get_call_status_test},
    {"get_ibrt_role",app_ibrt_ui_get_ibrt_role_test},
    {"get_tws_state",app_ibrt_ui_get_tws_conn_state_test},
    {"iic_switch", app_ibrt_ui_iic_uart_switch_test},
    {"soft_reset", app_ibrt_ui_soft_reset_test},
#ifdef IBRT_SEARCH_UI
    {"plug_in_test",app_ibrt_simulate_charger_plug_in_test},
    {"plug_out_test",app_ibrt_simulate_charger_plug_out_test},
    {"plug_box_test",app_ibrt_simulate_charger_plug_box_test},
#endif
#ifdef IBRT_ENHANCED_STACK_PTS
    {"hf_create_service_link",btif_pts_hf_create_link_with_pts},
    {"hf_disc_service_link",btif_pts_hf_disc_service_link},
    {"hf_create_audio_link",btif_pts_hf_create_audio_link},
    {"hf_disc_audio_link",btif_pts_hf_disc_audio_link},
    {"hf_answer_call",btif_pts_hf_answer_call},
    {"hf_hangup_call",btif_pts_hf_hangup_call},
    {"rfc_register",btif_pts_rfc_register_channel},
    {"rfc_close",btif_pts_rfc_close},
    {"av_create_channel",btif_pts_av_create_channel_with_pts},
    {"av_disc_channel",btif_pts_av_disc_channel},
    {"ar_connect",btif_pts_ar_connect_with_pts},
    {"ar_disconnect",btif_pts_ar_disconnect},
    {"ar_panel_play",btif_pts_ar_panel_play},
    {"ar_panel_pause",btif_pts_ar_panel_pause},
    {"ar_panel_stop",btif_pts_ar_panel_stop},
    {"ar_panel_forward",btif_pts_ar_panel_forward},
    {"ar_panel_backward",btif_pts_ar_panel_backward},
    {"ar_volume_up",btif_pts_ar_volume_up},
    {"ar_volume_down",btif_pts_ar_volume_down},
    {"ar_volume_notify",btif_pts_ar_volume_notify},
    {"ar_volume_change",btif_pts_ar_volume_change},
    {"ar_set_absolute_volume",btif_pts_ar_set_absolute_volume},
#endif
};


/*****************************************************************************
 Prototype    : app_ibrt_ui_auto_test_voice_promt_handle
 Description  : handle voice promt test cmd handle
 Input        : uint8_t operation_code
              : uint8_t *param
              : uint8_t param_len
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2020/3/3
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_voice_promt_handle(uint8_t operation_code,
        uint8_t *param,
        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    switch (operation_code)
    {
        default:
            break;
    }
}

/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_a2dp_handle
Description  : handle a2dp test cmd handle
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_a2dp_handle(uint8_t operation_code,
                                       uint8_t *param,
                                       uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);

    switch (operation_code)
    {
        case A2DP_AUTO_TEST_AUDIO_PLAY:
            app_ibrt_ui_audio_play_test();
            break;
        case A2DP_AUTO_TEST_AUDIO_PAUSE:
            app_ibrt_ui_audio_pause_test();
            break;
        case A2DP_AUTO_TEST_AUDIO_FORWARD:
            app_ibrt_ui_audio_forward_test();
            break;
        case A2DP_AUTO_TEST_AUDIO_BACKWARD:
            app_ibrt_ui_audio_backward_test();
            break;
        case A2DP_AUTO_TEST_AVRCP_VOL_UP:
            app_bt_volumeup();
            break;
        case A2DP_AUTO_TEST_AVRCP_VOL_DOWN:
            app_bt_volumedown();
            break;
        default:
            break;
    }
}

/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_hfp_handle
Description  : handle hfp test cmd handle
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_hfp_handle(uint8_t operation_code,
                                      uint8_t *param,
                                      uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);

    switch (operation_code)
    {
        case HFP_AUTO_TEST_SCO_CREATE:
            app_ibrt_ui_hfsco_create_test();
            break;
        case HFP_AUTO_TEST_SCO_DISC:
            app_ibrt_ui_hfsco_disc_test();
            break;
        case HFP_AUTO_TEST_CALL_REDIAL:
            app_ibrt_ui_call_redial_test();
            break;
        case HFP_AUTO_TEST_CALL_ANSWER:
            app_ibrt_ui_call_answer_test();
            break;
        case HFP_AUTO_TEST_CALL_HANGUP:
            app_ibrt_ui_call_hangup_test();
            break;
        case HFP_AUTO_TEST_VOLUME_UP:
            app_ibrt_ui_local_volume_up_test();
            break;
        case HFP_AUTO_TEST_VOLUME_DOWN:
            app_ibrt_ui_local_volume_down_test();
            break;
        default:
            break;
    }
}
/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_ui_handle
Description  : handle UI test cmd handle
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_ui_handle(uint8_t operation_code,
                                     uint8_t *param,
                                     uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);

    switch (operation_code)
    {
        case UI_AUTO_TEST_OPEN_BOX:
            app_ibrt_ui_open_box_event_test();
            break;
        case UI_AUTO_TEST_CLOSE_BOX:
            app_ibrt_ui_close_box_event_test();
            break;
        case UI_AUTO_TEST_FETCH_OUT_BOX:
            app_ibrt_ui_fetch_out_box_event_test();
            break;
        case UI_AUTO_TEST_PUT_IN_BOX:
            app_ibrt_ui_put_in_box_event_test();
            break;
        case UI_AUTO_TEST_WEAR_UP:
            app_ibrt_ui_ware_up_event_test();
            break;
        case UI_AUTO_TEST_WEAR_DOWN:
            app_ibrt_ui_ware_down_event_test();
            break;
        case UI_AUTO_TEST_ROLE_SWITCH:
            app_tws_if_trigger_role_switch();
            break;
        case UI_AUTO_TEST_PHONE_CONN_EVENT:
            app_ibrt_ui_phone_connect_event_test();
            break;
        case UI_AUTO_TEST_RECONN_EVENT:
            app_ibrt_ui_reconnect_event_test();
            break;
        case UI_AUTO_TEST_CONN_SECOND_MOBILE:
            app_ibrt_ui_choice_connect_second_mobile();
            break;
        case UI_AUTO_TEST_MOBILE_TWS_DISC:
            app_ibrt_if_disconnect_mobile_tws_link();
            break;
        case UI_AUTO_TEST_TWS_PAIRING:
            app_ibrt_ui_tws_pairing_test();
            break;
        case UI_AUTO_TEST_FREEMAN_PAIRING:
            app_ibrt_ui_freeman_pairing_test();
            break;
        case UI_AUTO_TEST_SUSPEND_IBRT:
            app_ibrt_ui_suspend_ibrt_test();
            break;
        case UI_AUTO_TEST_RESUME_IBRT:
            app_ibrt_ui_resume_ibrt_test();
            break;
        case UI_AUTO_TEST_SHUT_DOWN:
            app_ibrt_ui_shut_down_test();
            break;
        case UI_AUTO_TEST_REBOOT:
            app_ibrt_ui_reboot_test();
            break;
        case UI_AUTO_TEST_FACTORY_RESET:
            app_ibrt_ui_factory_reset_test();
            break;
        case UI_AUTO_TEST_ASSERT:
            ASSERT(false, "Force Panic!!!");
            break;
        case UI_AUTO_TEST_CONNECT_MOBILE:
            app_ibrt_if_choice_mobile_connect(0);
            break;
        case UI_AUTO_TEST_DISCONNECT_MOBILE:
            app_tws_ibrt_disconnect_mobile();
            break;
        case UI_AUTO_TEST_CONNECT_TWS:
            app_tws_ibrt_create_tws_connection(app_ibrt_auto_test_get_tws_page_timeout_value());
            break;
        case UI_AUTO_TEST_DISCONNECT_TWS:
            app_ibrt_ui_disconnect_tws();
            break;
        default:
            break;
    }
}

/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_ai_simulate_button_action
Description  : simulate ai button action
Input        : enum APP_KEY_EVENT_T eventCode
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/5/8
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_ai_simulate_button_action(enum APP_KEY_EVENT_T eventCode)
{
    APP_KEY_STATUS keyEvent;
    keyEvent.code = APP_KEY_CODE_GOOGLE;
    keyEvent.event = eventCode;
    app_voice_assistant_key(&keyEvent, NULL);
}

/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_ai_disconnect
Description  : disconnect AI connection
Input        : None
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/5/8
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_ai_disconnect(void)
{
#ifdef __AI_VOICE__
    if (app_ai_ble_is_connected() && AI_TRANSPORT_BLE != app_ai_get_transport_type())
    {
        app_ai_disconnect_ble();
    }
    if (app_ai_spp_is_connected() && AI_TRANSPORT_SPP != app_ai_get_transport_type())
    {
        app_ai_spp_disconnlink();
    }
#endif

#ifdef BISTO_ENABLED
    gsound_custom_ble_disconnect_ble_link();
    gsound_custom_bt_disconnect_all_channel();
#endif
}

/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_ai_handle
Description  : handle AI test cmd handle
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_ai_handle(uint8_t operation_code,
                                     uint8_t *param,
                                     uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    switch (operation_code)
    {
        case AI_AUTO_TEST_PTT_BUTTON_ACTION:
            app_ibrt_ui_auto_test_ai_simulate_button_action((enum APP_KEY_EVENT_T)*param);
            break;
        case AI_AUTO_TEST_DISCONNECT_AI:
            app_ibrt_ui_auto_test_ai_disconnect();
            break;
        default:
            break;
    }
}

typedef struct
{
    uint8_t  scanFilterPolicy;
    uint16_t scanWindowInMs;
    uint16_t scanIntervalInMs;
} APP_IBRT_UI_AUTO_TEST_BLE_SCAN_PARAM_T;

typedef struct
{
    uint8_t  conidx;
    uint32_t min_interval_in_ms;
    uint32_t max_interval_in_ms;
    uint32_t supervision_timeout_in_ms;
    uint8_t  slaveLantency;
} APP_IBRT_UI_AUTO_TEST_BLE_UPDATED_CONN_PARAM_T;

/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_ble_handle
Description  : handle ble test cmd handle
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_ble_handle(uint8_t operation_code,
                                      uint8_t *param,
                                      uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
#ifdef __IAG_BLE_INCLUDE__
    switch (operation_code)
    {
        case BLE_AUTO_TEST_START_ADV:
        {
            uint16_t advIntervalMs = *(uint16_t *)param;
            app_ble_start_connectable_adv(advIntervalMs);
            break;
        }
        case BLE_AUTO_TEST_STOP_ADV:
        {
            app_ble_stop_activities();
            break;
        }
        case BLE_AUTO_TEST_DISCONNECT:
        {
            app_ble_disconnect_all();
            break;
        }
        case BLE_AUTO_TEST_START_CONNECT:
        {
            // six bytes ble address as the parameter
            app_ble_start_connect(param);
            break;
        }
        case BLE_AUTO_TEST_STOP_CONNECT:
        {
            app_ble_stop_activities();
            break;
        }
        case BLE_AUTO_TEST_START_SCAN:
        {
            APP_IBRT_UI_AUTO_TEST_BLE_SCAN_PARAM_T* pScanParam =
                (APP_IBRT_UI_AUTO_TEST_BLE_SCAN_PARAM_T *)param;
            app_ble_start_scan((enum BLE_SCAN_FILTER_POLICY)(pScanParam->scanFilterPolicy),
                pScanParam->scanWindowInMs, pScanParam->scanIntervalInMs);
            break;
        }
        case BLE_AUTO_TEST_STOP_SCAN:
        {
            app_ble_stop_activities();
            break;
        }
        case BLE_AUTO_TEST_UPDATE_CONN_PARAM:
        {
        #ifdef __IAG_BLE_INCLUDE__
            APP_IBRT_UI_AUTO_TEST_BLE_UPDATED_CONN_PARAM_T* pConnParam
             = (APP_IBRT_UI_AUTO_TEST_BLE_UPDATED_CONN_PARAM_T *)param;
            if (app_ble_is_connection_on(pConnParam->conidx))
            {
                l2cap_update_param(pConnParam->conidx,
                    pConnParam->min_interval_in_ms,
                    pConnParam->max_interval_in_ms,
                    pConnParam->supervision_timeout_in_ms,
                    pConnParam->slaveLantency);
            }
        #endif
            break;
        }
        default:
            return;
    }
#endif
}
/*****************************************************************************
Prototype    : app_ibrt_ui_auto_test_flash_handle
Description  : handle flash test cmd handle
Input        : uint8_t operation_code
             : uint8_t *param
             : uint8_t param_len
Output       : None
Return Value :
Calls        :
Called By    :

History        :
Date         : 2020/3/3
Author       : bestechnic
Modification : Created function

*****************************************************************************/
void app_ibrt_ui_auto_test_flash_handle(uint8_t operation_code,
                                        uint8_t *param,
                                        uint8_t param_len)
{
    TRACE(2, "%s op_code 0x%x", __func__, operation_code);
    switch (operation_code)
    {
        case FLASH_AUTO_TEST_PROGRAM:
            break;
        case FLASH_AUTO_TEST_ERASE:
            break;
        case FLASH_AUTO_TEST_FLUSH_NV:
        {
            nv_record_flash_flush();
            norflash_flush_all_pending_op();
            break;
        }
        default:
            return;
    }
}

/*****************************************************************************
 Prototype    : app_ibrt_ui_find_uart_handle
 Description  : find the test cmd handle
 Input        : uint8_t* buf
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
app_uart_test_function_handle app_ibrt_ui_find_uart_handle(unsigned char* buf)
{
    app_uart_test_function_handle p = NULL;
    for(uint8_t i = 0; i<sizeof(app_ibrt_uart_test_handle)/sizeof(app_uart_handle_t); i++)
    {
        if(strncmp((char*)buf, app_ibrt_uart_test_handle[i].string, strlen(app_ibrt_uart_test_handle[i].string))==0 ||
           strstr(app_ibrt_uart_test_handle[i].string, (char*)buf))
        {
            p = app_ibrt_uart_test_handle[i].function;
            break;
        }
    }
    return p;
}
/*****************************************************************************
 Prototype    : app_ibrt_ui_automate_test_cmd_handler
 Description  : ibrt ui automate test cmd handler
 Input        : uint8_t group_code
              : uint8_t operation_code
              : uint8_t *param
              : uint8_t param_len
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2020/3/3
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
extern "C" void app_ibrt_ui_automate_test_cmd_handler(uint8_t group_code,
        uint8_t operation_code,
        uint8_t *param,
        uint8_t param_len)
{
    switch (group_code)
    {
        case AUTO_TEST_VOICE_PROMPT:
            app_ibrt_ui_auto_test_voice_promt_handle(operation_code, param, param_len);
            break;
        case AUTO_TEST_A2DP:
            app_ibrt_ui_auto_test_a2dp_handle(operation_code, param, param_len);
            break;
        case AUTO_TEST_HFP:
            app_ibrt_ui_auto_test_hfp_handle(operation_code, param, param_len);
            break;
        case AUTO_TEST_UI:
            app_ibrt_ui_auto_test_ui_handle(operation_code, param, param_len);
            break;
        case AUTO_TEST_AI:
            app_ibrt_ui_auto_test_ai_handle(operation_code, param, param_len);
            break;
        case AUTO_TEST_BLE:
            app_ibrt_ui_auto_test_ble_handle(operation_code, param, param_len);
            break;
        case AUTO_TEST_FLASH:
            app_ibrt_ui_auto_test_flash_handle(operation_code, param, param_len);
            break;
        case AUTO_TEST_UNUSE:
            break;
        default:
            break;
    }
}

/*****************************************************************************
 Prototype    : app_ibrt_ui_test_cmd_handler
 Description  : ibrt ui test cmd handler
 Input        : uint8_t *buf
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/3/30
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
extern "C" int app_ibrt_ui_test_cmd_handler(unsigned char *buf, unsigned int length)
{
    int ret = 0;

    if (buf[length-2] == 0x0d ||
        buf[length-2] == 0x0a)
    {
        buf[length-2] = 0;
    }

    app_uart_test_function_handle handl_function = app_ibrt_ui_find_uart_handle(buf);
    if(handl_function)
    {
        handl_function();
    }
    else
    {
        ret = -1;
        TRACE(0,"can not find handle function");
    }
    return ret;
}
#ifdef BES_AUDIO_DEV_Main_Board_9v0
void app_ibrt_key1(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    TWS_PD_MSG_BLOCK msg;
    msg.msg_body.message_id = 0;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_ibrt_peripheral_mailbox_put(&msg);
}

void app_ibrt_key2(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    TWS_PD_MSG_BLOCK msg;
    msg.msg_body.message_id = 1;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_ibrt_peripheral_mailbox_put(&msg);
}

void app_ibrt_key3(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    TWS_PD_MSG_BLOCK msg;
    msg.msg_body.message_id = 2;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_ibrt_peripheral_mailbox_put(&msg);
}

void app_ibrt_key4(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    TWS_PD_MSG_BLOCK msg;
    msg.msg_body.message_id = 3;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_ibrt_peripheral_mailbox_put(&msg);
}

void app_ibrt_key5(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    TWS_PD_MSG_BLOCK msg;
    msg.msg_body.message_id = 4;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_ibrt_peripheral_mailbox_put(&msg);
}

void app_ibrt_key6(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    TWS_PD_MSG_BLOCK msg;
    msg.msg_body.message_id = 5;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    app_ibrt_peripheral_mailbox_put(&msg);
}
#endif



#if defined(__BT_DEBUG_TPORTS__) && !defined(TPORTS_KEY_COEXIST)

void app_ibrt_ui_test_key(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    {
#ifdef IBRT_SEARCH_UI
        app_ibrt_search_ui_handle_key(status,param);
#else
        app_ibrt_normal_ui_handle_key(status,param);
#endif
    }
}

#else

void app_ibrt_ui_test_key(APP_KEY_STATUS *status, void *param)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
#ifdef TILE_DATAPATH
    uint8_t shutdown_key = HAL_KEY_EVENT_TRIPLECLICK;
#else
    uint8_t shutdown_key = HAL_KEY_EVENT_ULTRACLICK;
#endif
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);

#ifdef IBRT_OTA
	if ((status->code == HAL_KEY_CODE_PWR)&&\
        (status->event == APP_KEY_EVENT_CLICK)&&\
        (app_check_user_can_role_switch_in_ota()))
	{
	    TRACE(0, "[OTA] role switch in progress!");
        return ;
	}
#endif

    if (IBRT_SLAVE == p_ibrt_ctrl->current_role && status->event != shutdown_key)
    {
        app_ibrt_if_keyboard_notify(status,param);
    }
    else
    {
#ifdef IBRT_SEARCH_UI
        app_ibrt_search_ui_handle_key(status,param);
#else
        app_ibrt_normal_ui_handle_key(status,param);
#endif
    }
}

#endif



void app_ibrt_ui_test_key_io_event(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    switch(status->event)
    {
        case APP_KEY_EVENT_CLICK:
            if (status->code== APP_KEY_CODE_FN1)
            {
                app_ibrt_if_event_entry(IBRT_OPEN_BOX_EVENT);
            }
            else if (status->code== APP_KEY_CODE_FN2)
            {
                app_ibrt_if_event_entry(IBRT_FETCH_OUT_EVENT);
            }
            else
            {
                app_ibrt_if_event_entry(IBRT_WEAR_UP_EVENT);
            }
            break;

        case APP_KEY_EVENT_DOUBLECLICK:
            if (status->code== APP_KEY_CODE_FN1)
            {
                app_ibrt_if_event_entry(IBRT_CLOSE_BOX_EVENT);
            }
            else if (status->code== APP_KEY_CODE_FN2)
            {
                app_ibrt_if_event_entry(IBRT_PUT_IN_EVENT);
            }
            else
            {
                app_ibrt_if_event_entry(IBRT_WEAR_DOWN_EVENT);
            }
            break;

        case APP_KEY_EVENT_LONGPRESS:
            break;

        case APP_KEY_EVENT_TRIPLECLICK:
            break;

        case HAL_KEY_EVENT_LONGLONGPRESS:
            break;

        case APP_KEY_EVENT_ULTRACLICK:
            break;

        case APP_KEY_EVENT_RAMPAGECLICK:
            break;
    }
}

void app_ibrt_ui_test_key_custom_event(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    switch(status->event)
    {
        case APP_KEY_EVENT_CLICK:
            break;

        case APP_KEY_EVENT_DOUBLECLICK:
            break;

        case APP_KEY_EVENT_LONGPRESS:
            break;

        case APP_KEY_EVENT_TRIPLECLICK:
            break;

        case HAL_KEY_EVENT_LONGLONGPRESS:
            break;

        case APP_KEY_EVENT_ULTRACLICK:
            break;

        case APP_KEY_EVENT_RAMPAGECLICK:
            break;
    }
}

void app_ibrt_ui_test_voice_assistant_key(APP_KEY_STATUS *status, void *param)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    TRACE(3,"%s code 0x%x event %d", __func__, status->code, status->event);

    if (APP_KEY_CODE_GOOGLE != status->code)
    {
        return;
    }
    else
    {
        ibrt_ctrl_t *pIbrtCtrl = app_tws_ibrt_get_bt_ctrl_ctx();

        if (IBRT_ACTIVE_MODE != pIbrtCtrl->tws_mode)
        {
            app_tws_ibrt_exit_sniff_with_mobile();
        }
    }

    if (p_ibrt_ctrl->current_role != IBRT_MASTER)
    {
        app_ibrt_if_keyboard_notify(status, param);
        TRACE(2,"%s isn't master %d", __func__, p_ibrt_ctrl->current_role);
        return;
    }

#ifdef IS_MULTI_AI_ENABLED
    if (app_ai_manager_spec_get_status_is_in_invalid())
    {
        TRACE(0,"AI feature has been diabled");
        return;
    }

    if (app_ai_manager_is_need_reboot())
    {
        TRACE(1, "%s ai need to reboot", __func__);
        return;
    }

#ifdef MAI_TYPE_REBOOT_WITHOUT_OEM_APP
    if (app_ai_manager_get_spec_update_flag())
    {
        TRACE(0,"device reboot is ongoing...");
        return;
    }
#endif

    if(app_ai_manager_voicekey_is_enable())
    {
        if (AI_SPEC_GSOUND == app_ai_manager_get_current_spec())
        {
#ifdef BISTO_ENABLED
            gsound_custom_actions_handle_key(status, param);
#endif
        }
        else if(AI_SPEC_INIT != app_ai_manager_get_current_spec())
        {
            app_ai_key_event_handle(status, 0);
        }
    }
#else
#ifdef __AI_VOICE__
    app_ai_key_event_handle(status, 0);
#endif
#ifdef BISTO_ENABLED
    gsound_custom_actions_handle_key(status, param);
#endif
#endif
}

const APP_KEY_HANDLE  app_ibrt_ui_test_key_cfg[] =
{
#if defined(__AI_VOICE__) || defined(BISTO_ENABLED)
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_FIRST_DOWN}, "google assistant key", app_ibrt_ui_test_voice_assistant_key, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_UP}, "google assistant key", app_ibrt_ui_test_voice_assistant_key, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_LONGPRESS}, "google assistant key", app_ibrt_ui_test_voice_assistant_key, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_CLICK}, "google assistant key", app_ibrt_ui_test_voice_assistant_key, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_DOUBLECLICK}, "google assistant key", app_ibrt_ui_test_voice_assistant_key, NULL},
#endif
#ifdef TILE_DATAPATH
    {{APP_KEY_CODE_TILE,APP_KEY_EVENT_DOWN},"tile function key",app_ibrt_ui_test_key, NULL},
    {{APP_KEY_CODE_TILE,APP_KEY_EVENT_UP},"tile function key",app_ibrt_ui_test_key, NULL},
#endif
#if defined( __BT_ANC_KEY__)&&defined(ANC_APP)
/*modify by kyle 20210114*/
    //{{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt anc key",app_anc_key, NULL},
/*modify by kyle 20210114*/
#else
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key, NULL},
#endif
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"app_ibrt_ui_test_key", app_ibrt_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"app_ibrt_ui_test_key", app_ibrt_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_TRIPLECLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_ULTRACLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key, NULL},

#if defined(CHIP_BEST1400)
#ifdef IBRT_SEARCH_UI
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key",app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"app_ibrt_ui_test_key",app_ibrt_search_ui_gpio_key_handle, NULL},
#endif
#elif defined(CHIP_BEST1402)
#ifdef IBRT_SEARCH_UI
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key",app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"app_ibrt_ui_test_key",app_ibrt_search_ui_gpio_key_handle, NULL},
#endif
#else
#ifdef IBRT_SEARCH_UI
#ifdef TPORTS_KEY_COEXIST
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN1,HAL_KEY_EVENT_LONGPRESS},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
#else
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key",app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_search_ui_gpio_key_handle, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"app_ibrt_ui_test_key",app_ibrt_search_ui_gpio_key_handle, NULL},

#endif
#else
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key_io_event, NULL},
#endif
#endif
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key_io_event, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_DOUBLECLICK},"app_ibrt_ui_test_key", app_ibrt_ui_test_key_io_event, NULL},
    /*
    #ifdef BES_AUDIO_DEV_Main_Board_9v0
        {{APP_KEY_CODE_FN1,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_key1, NULL},
        {{APP_KEY_CODE_FN2,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_key2, NULL},
        {{APP_KEY_CODE_FN3,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_key3, NULL},
        {{APP_KEY_CODE_FN4,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_key4, NULL},
        {{APP_KEY_CODE_FN5,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_key5, NULL},
        {{APP_KEY_CODE_FN6,APP_KEY_EVENT_CLICK},"app_ibrt_ui_test_key", app_ibrt_key6, NULL},
    #endif
    */
};

/*
* customer addr config here
*/
ibrt_pairing_info_t g_ibrt_pairing_info[] =
{
    {{0x51, 0x33, 0x33, 0x22, 0x11, 0x11},{0x50, 0x33, 0x33, 0x22, 0x11, 0x11}},
    {{0x53, 0x33, 0x33, 0x22, 0x11, 0x11},{0x52, 0x33, 0x33, 0x22, 0x11, 0x11}}, /*LJH*/
    {{0x61, 0x33, 0x33, 0x22, 0x11, 0x11},{0x60, 0x33, 0x33, 0x22, 0x11, 0x11}},
    {{0x67, 0x66, 0x66, 0x22, 0x11, 0x11},{0x66, 0x66, 0x66, 0x22, 0x11, 0x11}}, /*bisto*/
    {{0x71, 0x33, 0x33, 0x22, 0x11, 0x11},{0x70, 0x33, 0x33, 0x22, 0x11, 0x11}},
    {{0x81, 0x33, 0x33, 0x22, 0x11, 0x11},{0x80, 0x33, 0x33, 0x22, 0x11, 0x11}},
    {{0x91, 0x33, 0x33, 0x22, 0x11, 0x11},{0x90, 0x33, 0x33, 0x22, 0x11, 0x11}}, /*Customer use*/
    {{0x05, 0x33, 0x33, 0x22, 0x11, 0x11},{0x04, 0x33, 0x33, 0x22, 0x11, 0x11}}, /*Rui*/
    {{0x07, 0x33, 0x33, 0x22, 0x11, 0x11},{0x06, 0x33, 0x33, 0x22, 0x11, 0x11}}, /*zsl*/
    {{0x88, 0xaa, 0x33, 0x22, 0x11, 0x11},{0x87, 0xaa, 0x33, 0x22, 0x11, 0x11}}, /*Lufang*/
    {{0x77, 0x22, 0x66, 0x22, 0x11, 0x11},{0x77, 0x33, 0x66, 0x22, 0x11, 0x11}}, /*xiao*/
    {{0xAA, 0x22, 0x66, 0x22, 0x11, 0x11},{0xBB, 0x33, 0x66, 0x22, 0x11, 0x11}}, /*LUOBIN*/
    {{0x08, 0x33, 0x66, 0x22, 0x11, 0x11},{0x07, 0x33, 0x66, 0x22, 0x11, 0x11}}, /*Yangbin1*/
    {{0x0B, 0x33, 0x66, 0x22, 0x11, 0x11},{0x0A, 0x33, 0x66, 0x22, 0x11, 0x11}}, /*Yangbin2*/
    {{0x35, 0x33, 0x66, 0x22, 0x11, 0x11},{0x34, 0x33, 0x66, 0x22, 0x11, 0x11}}, /*Lulu*/
    {{0xF8, 0x33, 0x66, 0x22, 0x11, 0x11},{0xF7, 0x33, 0x66, 0x22, 0x11, 0x11}}, /*jtx*/
    {{0xd3, 0x53, 0x86, 0x42, 0x71, 0x31},{0xd2, 0x53, 0x86, 0x42, 0x71, 0x31}}, /*shhx*/
    {{0xcc, 0xaa, 0x99, 0x88, 0x77, 0x66},{0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66}}, /*mql*/
    {{0x95, 0x33, 0x69, 0x22, 0x11, 0x11},{0x94, 0x33, 0x69, 0x22, 0x11, 0x11}}, /*wyl*/
    {{0x82, 0x35, 0x68, 0x24, 0x19, 0x17},{0x81, 0x35, 0x68, 0x24, 0x19, 0x17}}, /*hy*/
    {{0x66, 0x66, 0x88, 0x66, 0x66, 0x88},{0x65, 0x66, 0x88, 0x66, 0x66, 0x88}}, /*xdl*/
    {{0x61, 0x66, 0x66, 0x66, 0x66, 0x81},{0x16, 0x66, 0x66, 0x66, 0x66, 0x18}}, /*test1*/
    {{0x62, 0x66, 0x66, 0x66, 0x66, 0x82},{0x26, 0x66, 0x66, 0x66, 0x66, 0x28}}, /*test2*/
    {{0x63, 0x66, 0x66, 0x66, 0x66, 0x83},{0x36, 0x66, 0x66, 0x66, 0x66, 0x38}}, /*test3*/
    {{0x64, 0x66, 0x66, 0x66, 0x66, 0x84},{0x46, 0x66, 0x66, 0x66, 0x66, 0x48}}, /*test4*/
    {{0x65, 0x66, 0x66, 0x66, 0x66, 0x85},{0x56, 0x66, 0x66, 0x66, 0x66, 0x58}}, /*test5*/
    {{0xaa, 0x66, 0x66, 0x66, 0x66, 0x86},{0xaa, 0x66, 0x66, 0x66, 0x66, 0x68}}, /*test6*/
    {{0x67, 0x66, 0x66, 0x66, 0x66, 0x87},{0x76, 0x66, 0x66, 0x66, 0x66, 0x78}}, /*test7*/
    {{0x68, 0x66, 0x66, 0x66, 0x66, 0xa8},{0x86, 0x66, 0x66, 0x66, 0x66, 0x8a}}, /*test8*/
    {{0x69, 0x66, 0x66, 0x66, 0x66, 0x89},{0x86, 0x66, 0x66, 0x66, 0x66, 0x18}}, /*test9*/
    {{0x93, 0x33, 0x33, 0x33, 0x33, 0x33},{0x92, 0x33, 0x33, 0x33, 0x33, 0x33}}, /*gxl*/
    {{0xae, 0x28, 0x00, 0xe9, 0xc6, 0x5c},{0xd8, 0x29, 0x00, 0xe9, 0xc6, 0x5c}}, /*lsk*/
    {{0x07, 0x13, 0x66, 0x22, 0x11, 0x11},{0x06, 0x13, 0x66, 0x22, 0x11, 0x11}}, /*yangguo*/
    {{0x02, 0x15, 0x66, 0x22, 0x11, 0x11},{0x01, 0x15, 0x66, 0x22, 0x11, 0x11}}, /*mql fpga*/
    
};

int app_ibrt_ui_test_config_load(void *config)
{
    ibrt_pairing_info_t *ibrt_pairing_info_lst = g_ibrt_pairing_info;
    uint32_t lst_size = sizeof(g_ibrt_pairing_info)/sizeof(ibrt_pairing_info_t);
    ibrt_config_t *ibrt_config = (ibrt_config_t *)config;
    struct nvrecord_env_t *nvrecord_env;
    uint8_t ble_address[6] = {0, 0, 0, 0, 0, 0};

    nv_record_env_get(&nvrecord_env);
    if(nvrecord_env->ibrt_mode.tws_connect_success == 0)
    {
        app_ibrt_ui_clear_tws_connect_success_last();
    }
    else
    {
        app_ibrt_ui_set_tws_connect_success_last();
    }

    if (memcmp(nv_record_tws_get_self_ble_info(), bt_get_ble_local_address(), BD_ADDR_LEN) &&
        memcmp(nv_record_tws_get_self_ble_info(), ble_address, BD_ADDR_LEN))
    {
        nv_record_tws_exchange_ble_info();
    }

    factory_section_original_btaddr_get(ibrt_config->local_addr.address);
    for(uint32_t i =0; i<lst_size; i++)
    {
        if (!memcmp(ibrt_pairing_info_lst[i].master_bdaddr.address, ibrt_config->local_addr.address, BD_ADDR_LEN))
        {
            ibrt_config->nv_role = IBRT_MASTER;
            ibrt_config->audio_chnl_sel = AUDIO_CHANNEL_SELECT_RCHNL;
            memcpy(ibrt_config->peer_addr.address, ibrt_pairing_info_lst[i].slave_bdaddr.address, BD_ADDR_LEN);
            return 0;
        }
        else if (!memcmp(ibrt_pairing_info_lst[i].slave_bdaddr.address, ibrt_config->local_addr.address, BD_ADDR_LEN))
        {
            ibrt_config->nv_role = IBRT_SLAVE;
            ibrt_config->audio_chnl_sel = AUDIO_CHANNEL_SELECT_LCHNL;
            memcpy(ibrt_config->peer_addr.address, ibrt_pairing_info_lst[i].master_bdaddr.address, BD_ADDR_LEN);
            return 0;
        }
    }
    return -1;
}

void app_ibrt_ui_test_key_init(void)
{
    app_key_handle_clear();
    for (uint8_t i=0; i<ARRAY_SIZE(app_ibrt_ui_test_key_cfg); i++)
    {
        app_key_handle_registration(&app_ibrt_ui_test_key_cfg[i]);
    }
}

void app_ibrt_ui_test_init(void)
{
    TRACE(1,"%s", __func__);

    app_ibrt_ui_box_init(&box_ble_addr);
    app_ibrt_auto_test_init();
}

void app_ibrt_ui_sync_status(uint8_t *param_ptr,uint16_t length)
{
#ifdef ANC_APP
/*modify by kyle 20210114 define for zowee ui anc switch*/
    //app_anc_status_post(((anc_sync_status_t*)param_ptr)->anc_status);
/*modify by kyle 20210114*/
#endif
}

#endif
