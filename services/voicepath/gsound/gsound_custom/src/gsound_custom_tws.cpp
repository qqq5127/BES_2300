/***************************************************************************
*
*Copyright 2015-2019 BES.
*All rights reserved. All unpublished rights reserved.
*
*No part of this work may be used or reproduced in any form or by any
*means, or stored in a database or retrieval system, without prior written
*permission of BES.
*
*Use of this work is governed by a license granted by BES.
*This work contains confidential and proprietary information of
*BES. which is protected by copyright, trade secret,
*trademark and other intellectual property rights.
*
****************************************************************************/

/*****************************header include********************************/
#include "gsound_dbg.h"
#include "gsound_target.h"
#include "gsound_service.h"
#include "gsound_custom.h"
#include "gsound_custom_tws.h"
#include "gsound_custom_bt.h"
#include "gsound_custom_ble.h"
#include "app.h"
#include "app_voicepath.h"

#ifdef IBRT
#include "app_tws_ibrt.h"
#include "app_ibrt_ui.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_tws_ctrl_thread.h"
#endif

#ifdef BLE_ENABLE
#include "app_ble_mode_switch.h"
#endif

#include "app_ai_if.h"
#include "app_ai_tws.h"

/************************private macro defination***************************/
#define GSOUND_ROLE_SWITCH_PREPARE_DELAY_MS 50

#ifdef BISTO_USE_TWS_API
#define GSOUND_RECONNECT_AFTER_ROLE_SWITCH_DELA_MS 500
#define GSOUND_ROLE_SWITCH_MAX_TRY_TIMES 5
#define GSOUND_ROLE_SWITCH_RETRY_DELAY_MS 100
#define GSOUND_ROLE_UPDATE_DELAY_MS 100
#define GSOUND_ROLE_SWITCH_WAIT_DONE_DELAY_MS 200
#endif

/************************private type defination****************************/

/**********************private function declearation************************/

/************************private variable defination************************/
static const GSoundTwsInterface *pGsoundTwsInterface = NULL;

static void gsound_master_role_switch_prepare_timer_cb(void const *n);
osTimerDef(GSOUND_ROLE_SWITCH_PREPARE_TIMER, gsound_master_role_switch_prepare_timer_cb);
static osTimerId gsoundRoleSwitchPrepareTimer;

#ifdef BISTO_USE_TWS_API
static uint8_t gsound_role_switch_try_times = 0;

static void gsound_role_switch_retry_timer_cb(void const *n);
osTimerDef(GSOUND_ROLE_SWITCH_RETRY_TIMER, gsound_role_switch_retry_timer_cb);
static osTimerId gsoundRoleSwitchRetryTimer;

static void gsound_role_switch_wait_done_timer_cb(void const *n);
osTimerDef(GSOUND_ROLE_SWITCH_WAIT_DONE_TIMER, gsound_role_switch_wait_done_timer_cb);
static osTimerId gsoundRoleSwitchWaitDoneTimer;
#endif

#ifdef IBRT
uint8_t gRoleSwitchInitiator = IBRT_MASTER;
#endif

/****************************function defination****************************/
void gsound_tws_register_target_handle(const void *handle)
{
    pGsoundTwsInterface = (const GSoundTwsInterface *)handle;
}

const void *gsound_tws_get_target_handle(void)
{
    return (const void *)pGsoundTwsInterface;
}

void gsound_tws_init(void)
{
    gsoundRoleSwitchPrepareTimer = osTimerCreate(osTimer(GSOUND_ROLE_SWITCH_PREPARE_TIMER),
                                                 osTimerOnce,
                                                 NULL);

#ifdef BISTO_USE_TWS_API
    gsound_role_switch_try_times = 0;

    gsoundRoleSwitchRetryTimer = osTimerCreate(osTimer(GSOUND_ROLE_SWITCH_RETRY_TIMER),
                                               osTimerOnce,
                                               NULL);

    gsoundRoleSwitchWaitDoneTimer = osTimerCreate(osTimer(GSOUND_ROLE_SWITCH_WAIT_DONE_TIMER),
                                                  osTimerOnce,
                                                  NULL);
#endif
}

#ifdef BISTO_USE_TWS_API
void gsound_tws_role_update(uint8_t newRole)
{
    GLOG_I("[%s]+++", __func__);

    uint8_t role = gsound_get_role();

    if (pGsoundTwsInterface)
    {
        GLOG_I("IBRT role update:%s->%s",
               app_tws_ibrt_role2str(role),
               app_tws_ibrt_role2str(newRole));

        // default role has also be initialized as master, see @app_gsound_tws_init_role
        if ((IBRT_MASTER == role || IBRT_UNKNOW == role) &&
            IBRT_SLAVE == newRole)
        {
            gsound_set_role(newRole);
            pGsoundTwsInterface->gsound_tws_role_change_force(GSOUND_ROLE_UPDATE_DELAY_MS);
        }
        else if (IBRT_SLAVE == role &&
                 IBRT_MASTER == newRole)
        {
            gsound_set_role(newRole);
            pGsoundTwsInterface->gsound_tws_role_change_target_done(true);
        }
        else
        {
            GLOG_I("no need to update role.");
        }
    }
    else
    {
        GLOG_I("pGsoundTwsInterface is null.");
    }

    GLOG_I("[%s]---", __func__);
}

static void gsound_role_switch_retry_timer_cb(void const *n)
{
    // to avoid the corner case that GSoundTargetTwsRoleChangeResponse accept has been called,
    // while the timer callback has been pending for execute.
    // For this case, gsound_role_switch_try_times has been cleared in GSoundTargetTwsRoleChangeResponse
    if (0 == gsound_role_switch_try_times)
    {
        return;
    }

    // request the gsound roleswitch to gsound lib again
    gsound_tws_request_roleswitch();
}

static void gsound_role_switch_wait_done_timer_cb(void const *n)
{
    bool toMaster = (IBRT_SLAVE == app_tws_ibrt_role_get_callback(NULL));

    if (pGsoundTwsInterface)
    {
        pGsoundTwsInterface->gsound_tws_role_change_fatal(toMaster);
    }

    // master request role switch
    if (IBRT_MASTER == app_tws_ibrt_role_get_callback(NULL))
    {
        gsound_master_role_switch_prepare_timer_cb(NULL);
    }
}

#endif

// called right before target starts role switch, to assure that gsound owned
// connections are really down
static bool _master_prepare_bes_roleswitch(void)
{
    bool isWaitUntilGsoundDisconnected = false;

    // if gsound doesn't support controlled role switch, need to disconnect gsound connections here
#ifndef WITH_GSOUND_ROLE_SWITCH_INTEGRATED
    if (app_ble_is_any_connection_exist())
    {
        isWaitUntilGsoundDisconnected = true;
        // disconnect ble connection if existing
        app_ble_disconnect_all();
    }
#endif

#ifdef BISTO_USE_TWS_API
    if (gsound_bt_is_any_connected())
    {
        isWaitUntilGsoundDisconnected = true;
        // disconnect bt if existing
        gsound_custom_bt_disconnect_all_channel();
    }
#endif

#ifdef BISTO_USE_TWS_API
    if (gsound_bt_is_any_connected())
    {
        isWaitUntilGsoundDisconnected = true;
        // disconnect bt if existing
        gsound_custom_bt_disconnect_all_channel();
    }
#endif

    return isWaitUntilGsoundDisconnected;
}

static void _reuqest_master_do_roleswitch()
{
    GLOG_I("[%s]", __func__);

    uint8_t role = AI_SPEC_GSOUND;
    tws_ctrl_send_cmd(APP_TWS_CMD_LET_MASTER_PREPARE_RS, &role, 1);
}

static void let_slave_continue_roleswitch()
{
    uint8_t role = AI_SPEC_GSOUND;
    tws_ctrl_send_cmd(APP_TWS_CMD_LET_SLAVE_CONTINUE_RS, &role, 1);
}

static void gsound_target_tws_start_bes_role_switch(void)
{
#ifdef IBRT
    if (IBRT_SLAVE == gRoleSwitchInitiator)
    {
        let_slave_continue_roleswitch();
        gsound_tws_update_roleswitch_initiator(IBRT_MASTER);
    }
    else
    {
        GLOG_I("disconnect accomplished, start systme role switch.");
        app_ibrt_if_tws_switch_prepare_done_in_bt_thread(IBRT_ROLE_SWITCH_USER_AI, AI_SPEC_GSOUND);
    }
#endif
}

static void gsound_master_role_switch_prepare_timer_cb(void const *n)
{
    if (_master_prepare_bes_roleswitch())
    {
        GLOG_I("Wait until gsound disconnected.");
        osTimerStart(gsoundRoleSwitchPrepareTimer, GSOUND_ROLE_SWITCH_PREPARE_DELAY_MS);
    }
    else
    {
        gsound_target_tws_start_bes_role_switch();
    }
}

bool gsound_tws_request_roleswitch(void)
{
    GLOG_I("[%s]+++", __func__);
    bool ret = false;

    GLOG_I("path:%d, role:%d", gsound_custom_get_connection_path(), app_ai_tws_get_local_role());

    if (CONNECTION_BLE == gsound_custom_get_connection_path()
#ifdef BISTO_USE_TWS_API
        || true
#endif
    )
    {
        if (APP_AI_TWS_MASTER == app_ai_tws_get_local_role())
        {
#ifdef BISTO_USE_TWS_API
            if (pGsoundTwsInterface)
            {
                GLOG_I("request role switch to gsoundlib");
                gsound_set_role_switch_state(true);
                pGsoundTwsInterface->gsound_tws_role_change_request();

                // retry in three seconds
                gsound_role_switch_try_times++;
                if (gsound_role_switch_try_times < GSOUND_ROLE_SWITCH_MAX_TRY_TIMES)
                {
                    osTimerStart(gsoundRoleSwitchRetryTimer,
                                 GSOUND_ROLE_SWITCH_RETRY_DELAY_MS);
                }
                else
                {
                    gsound_role_switch_try_times = 0;

                    // won't wait but forcefully trigger the gsound role switch anyway
                    osTimerStart(gsoundRoleSwitchWaitDoneTimer, GSOUND_ROLE_SWITCH_WAIT_DONE_DELAY_MS);
                    pGsoundTwsInterface->gsound_tws_role_change_force(GSOUND_RECONNECT_AFTER_ROLE_SWITCH_DELA_MS);
                }

                ret = true;
            }
#else
            gsound_master_role_switch_prepare_timer_cb(NULL);
#endif
        }
        else if (APP_AI_TWS_SLAVE == app_ai_tws_get_local_role())
        {
            _reuqest_master_do_roleswitch();
        }
        else
        {
            ASSERT(0, "illegal role for BISTO role switch");
        }
    }
    else // CONNECTION_BT || CONNECTION_NULL
    {
        app_ibrt_if_tws_switch_prepare_done_in_bt_thread(IBRT_ROLE_SWITCH_USER_AI, AI_SPEC_GSOUND);
    }

    GLOG_I("[%s]---", __func__);
    return ret;
}

#ifdef BISTO_USE_TWS_API
static void _perform_roleswitch(void)
{
    GLOG_I("[%s]+++", __func__);

    if (pGsoundTwsInterface)
    {
        osTimerStart(gsoundRoleSwitchWaitDoneTimer, GSOUND_ROLE_SWITCH_WAIT_DONE_DELAY_MS);
        pGsoundTwsInterface->gsound_tws_role_change_perform(GSOUND_RECONNECT_AFTER_ROLE_SWITCH_DELA_MS);
    }

    GLOG_I("[%s]---", __func__);
}

static void _cancel_roleswitch(void)
{
    if (pGsoundTwsInterface)
    {
        pGsoundTwsInterface->gsound_tws_role_change_cancel();
    }
}

void gsound_tws_on_roleswitch_accepted_by_lib(void)
{
    osTimerStop(gsoundRoleSwitchRetryTimer);

    // reset the try counter
    gsound_role_switch_try_times = 0;

    GLOG_I("tws role switch accepted by gsound!.");

    // TODO: freddie: add conditions
    if (true)
    {
        _perform_roleswitch();
    }
    else
    {
        _cancel_roleswitch();
    }
}

void gsound_tws_roleswitch_lib_complete(void)
{
    uint8_t role = 0;
    bool isMaster = false;

    osTimerStop(gsoundRoleSwitchWaitDoneTimer);

#ifdef IBRT
    role = app_tws_ibrt_role_get_callback(NULL);
    isMaster = (IBRT_MASTER == role);
    GLOG_I("[%s] current role is %s", __func__, app_tws_ibrt_role2str(role));

    if (isMaster)
    {
        gsound_master_role_switch_prepare_timer_cb(NULL);
    }
    else
    {
        pGsoundTwsInterface->gsound_tws_role_change_target_done(false);
    }
#endif
}

void gsound_tws_inform_roleswitch_done(void)
{
    GLOG_I("[%s]+++", __func__);

    uint8_t role = app_tws_ibrt_role_get_callback(NULL);
    bool isMaster = (IBRT_SLAVE != role);

    if (pGsoundTwsInterface)
    {
        gsound_set_role(role);
        pGsoundTwsInterface->gsound_tws_role_change_target_done(isMaster);
    }

    gsound_set_role_switch_state(false);
    GLOG_I("[%s]---", __func__);
}
#endif

#ifdef IBRT
void gsound_tws_update_roleswitch_initiator(uint8_t role)
{
    gRoleSwitchInitiator = role;
}
#endif
