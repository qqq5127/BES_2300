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
#include "cmsis_os.h"
#include "gsound_custom_ble.h"
#include "gsound_custom.h"
#include "gsound_dbg.h"
#include "app.h"
#include "app_task.h" // application task definitions
#include "app_voicepath.h"
#include "app_ble_mode_switch.h"
#include "app_ble_rx_handler.h"
#include "gsound_gatt_server.h"
#include "gsound_custom_ota.h"
#include "gsound_custom_bt.h"
#include "app_bt_func.h"

#ifdef GFPS_ENABLED
#include "app_gfps.h"
#endif

extern "C"
{
#if (BLE_APP_ANCC)
#include "app_ancc.h"
#endif

#if (BLE_APP_AMS)
#include "app_amsc.h"
#endif
}

#ifdef IS_MULTI_AI_ENABLED
#include "ai_manager.h"
#endif

#ifdef IBRT
#include "app_tws_ibrt.h"
#include "app_tws_if.h"
#endif

/************************private macro defination***************************/
#define GSOUND_SERVICE_DATA_LEN 6
#define GSOUND_SERVICE_DATA_UUID 0xFE26
#define GSOUND_DEVICE_MODEL_ID 0x00003E

#define GSOUND_BLE_TX_BUFFER_SIZE 2048
#define GSOUND_BLE_CONNECTION_INTERVAL_MIN_IN_MS 20
#define GSOUND_BLE_CONNECTION_INTERVAL_MAX_IN_MS 40
#define GSOUND_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS 6000
#define GSOUND_BLE_CONNECTION_SLAVELATENCY 0

#define MAXIMUM_INFLIGHT_TX_PACKET_CNT 3

/************************private type defination****************************/
typedef struct
{
    uint16_t len;
    const uint8_t *data;
    GSoundChannelType chnl;
} GSOUND_TX_INSTANCE_T;

typedef struct
{
    uint8_t inputIndex;
    uint8_t outputIndex;
    uint8_t count;

    GSOUND_TX_INSTANCE_T inflight_tx[MAXIMUM_INFLIGHT_TX_PACKET_CNT];
} APP_GSOUND_INFLIGHT_TX_ENV_T;

struct app_gsound_env_tag
{
    uint8_t connectionIndex;
    volatile int mtu;
    uint16_t savedMTU[BLE_CONNECTION_MAX];

    APP_GSOUND_INFLIGHT_TX_ENV_T inflight_env;
};

/**********************private function declearation************************/
static int app_gsound_msg_handler(ke_msg_id_t const msgid,
                                  void const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);

static int app_gsound_channel_connected_handler(ke_msg_id_t const msgid,
                                                struct gsound_gatt_connection_event_t *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id);

static int app_gsound_channel_disconnected_handler(ke_msg_id_t const msgid,
                                                   struct gsound_gatt_connection_event_t *param,
                                                   ke_task_id_t const dest_id,
                                                   ke_task_id_t const src_id);

static int app_gsound_control_received_handler(ke_msg_id_t const msgid,
                                               struct gsound_gatt_rx_data_ind_t *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id);

static int app_gsound_audio_received_handler(ke_msg_id_t const msgid,
                                             struct gsound_gatt_rx_data_ind_t *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id);

static int app_gsound_notify_complete_handler(ke_msg_id_t const msgid,
                                              struct gsound_gatt_tx_complete_ind_t *param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id);

/************************private variable defination************************/
static struct app_gsound_env_tag app_gsound_env;
static const GSoundBleInterface *ble_interface;

static const struct ke_msg_handler gsound_msg_handler_list[] = {
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER, (ke_msg_func_t)app_gsound_msg_handler},
    {GSOUND_CHANNEL_CONNECTED, (ke_msg_func_t)app_gsound_channel_connected_handler},
    {GSOUND_CHANNEL_DISCONNECTED, (ke_msg_func_t)app_gsound_channel_disconnected_handler},
    {GSOUND_CONTROL_RECEIVED, (ke_msg_func_t)app_gsound_control_received_handler},
    {GSOUND_AUDIO_RECEIVED, (ke_msg_func_t)app_gsound_audio_received_handler},
    {GSOUND_SEND_COMPLETE_IND, (ke_msg_func_t)app_gsound_notify_complete_handler},
};

const struct ke_state_handler gsound_msg_handler_table = {
    gsound_msg_handler_list,
    ARRAY_SIZE(gsound_msg_handler_list),
};

static osMutexId gsound_ble_mutex_id;
osMutexDef(gsound_ble_mutex);

/****************************function defination****************************/
static void gsound_ble_set_connection_idx(uint8_t idx)
{
    app_gsound_env.connectionIndex = idx;
    GLOG_I("gsound ble connection idx is set:%d", app_gsound_env.connectionIndex);
}

uint8_t app_gsound_ble_get_connection_index(void)
{
    return app_gsound_env.connectionIndex;
}

void app_gsound_ble_init(void)
{
    gsound_ble_set_connection_idx(BLE_INVALID_CONNECTION_INDEX);
    app_gsound_env.mtu = 23; // Default 27 bytes - 4 ATT bytes.
    memset((uint8_t *)&(app_gsound_env.savedMTU), 0, sizeof(app_gsound_env.mtu));
    memset((uint8_t *)&(app_gsound_env.inflight_env), 0, sizeof(app_gsound_env.inflight_env));
    gsound_ble_mutex_id = osMutexCreate((osMutex(gsound_ble_mutex)));
}

void gsound_ble_register_target_handle(const GSoundBleInterface *handlers)
{
    ble_interface = handlers;
}

const struct ke_state_handler *gsound_ble_get_msg_handler_table(void)
{
    return &gsound_msg_handler_table;
}

uint8_t app_gsound_ble_get_mtu(void)
{
    return app_gsound_env.mtu;
}

bool app_gsound_ble_is_connected(void)
{
    return (BLE_INVALID_CONNECTION_INDEX != app_gsound_ble_get_connection_index());
}

void app_gsound_ble_data_fill_handler(void *param)
{
    GLOG_I("[%s]+++", __func__);

    BLE_ADV_PARAM_T *advInfo = (BLE_ADV_PARAM_T *)param;
    bool adv_enable = false;
    GLOG_I("adv data offset:%d, scan response data offset:%d",
           advInfo->advDataLen,
           advInfo->scanRspDataLen);

#ifdef IBRT
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    GLOG_I("current role:%s", app_tws_ibrt_role2str(p_ibrt_ctrl->current_role));

    if ((IBRT_MASTER == p_ibrt_ctrl->current_role) &&
        p_ibrt_ctrl->init_done &&
        (BTIF_APP_INVALID_ID != p_ibrt_ctrl->mobile_constate) &&
        (MOBILE_CONNECT_IOS == gsound_get_connected_mobile_type()))
#endif
    {
#ifdef GFPS_ENABLED
        if (!app_is_in_fastpairing_mode())
#endif
        {
            if (!app_gsound_ble_is_connected() && !gsound_bt_is_any_connected())
            {
                GLOG_I("gsound data add in scan response");
                adv_enable = true;
                advInfo->scanRspData[advInfo->scanRspDataLen++] = GSOUND_SERVICE_DATA_LEN;
                advInfo->scanRspData[advInfo->scanRspDataLen++] = BLE_ADV_SVC_FLAG;
                advInfo->scanRspData[advInfo->scanRspDataLen++] = (GSOUND_SERVICE_DATA_UUID >> 0) & 0xFF;
                advInfo->scanRspData[advInfo->scanRspDataLen++] = (GSOUND_SERVICE_DATA_UUID >> 8) & 0xFF;
                advInfo->scanRspData[advInfo->scanRspDataLen++] = (GSOUND_DEVICE_MODEL_ID >> 0) & 0xFF;
                advInfo->scanRspData[advInfo->scanRspDataLen++] = (GSOUND_DEVICE_MODEL_ID >> 8) & 0xFF;
                advInfo->scanRspData[advInfo->scanRspDataLen++] = (GSOUND_DEVICE_MODEL_ID >> 16) & 0xFF;
            }
        }
#ifdef GFPS_ENABLED
        else
        {
            GLOG_I("gsound data skip, in_fp_mode:%d, bt_connected:%d",
                   app_is_in_fastpairing_mode(),
                   gsound_bt_is_any_connected());
        }
#endif
    }

    app_ble_data_fill_enable(USER_GSOUND, adv_enable);
    GLOG_I("[%s]---", __func__);
}

void app_gsound_ble_add_svc(void)
{
    GLOG_I("Registering GSound GATT Service");
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             0);

    req->operation = GAPM_PROFILE_TASK_ADD;
#if BLE_CONNECTION_MAX > 1
    req->sec_lvl = PERM(SVC_AUTH, SEC_CON) | PERM(SVC_MI, ENABLE);
#else
    req->sec_lvl = PERM(SVC_AUTH, SEC_CON);
#endif
    req->prf_task_id = TASK_ID_VOICEPATH;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    ke_msg_send(req);
}

static int app_gsound_msg_handler(ke_msg_id_t const msgid,
                                  void const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    GLOG_I("GSound App Default Message Handler: id=%d", msgid);
    return KE_MSG_CONSUMED;
}

void app_gsound_ble_disconnected_evt_handler(uint8_t conidx)
{
    if (conidx == app_gsound_ble_get_connection_index())
    {
        GLOG_I("%s", __func__);
        osMutexWait(gsound_ble_mutex_id, osWaitForever);
        gsound_ble_set_connection_idx(BLE_INVALID_CONNECTION_INDEX);
        app_gsound_env.mtu = 23; // Default 27 bytes - 4 ATT bytes.

        memset((uint8_t *)&(app_gsound_env.savedMTU), 0, sizeof(app_gsound_env.mtu));
        memset((uint8_t *)&(app_gsound_env.inflight_env), 0, sizeof(app_gsound_env.inflight_env));

        osMutexRelease(gsound_ble_mutex_id);

        if (gsound_get_ble_channel_connect_state(GSOUND_CHANNEL_AUDIO))
        {
            ble_interface->gsound_ble_on_disconnect(GSOUND_CHANNEL_AUDIO);
            gsound_set_ble_connect_state(GSOUND_CHANNEL_AUDIO, false);
        }
        else
        {
            GLOG_I("%s already disconnected", gsound_chnl2str(GSOUND_CHANNEL_AUDIO));
        }

        if (gsound_get_ble_channel_connect_state(GSOUND_CHANNEL_CONTROL))
        {
            ble_interface->gsound_ble_on_disconnect(GSOUND_CHANNEL_CONTROL);
            gsound_set_ble_connect_state(GSOUND_CHANNEL_CONTROL, false);
        }
        else
        {
            GLOG_I("%s already disconnected", gsound_chnl2str(GSOUND_CHANNEL_CONTROL));
        }

#ifdef IBRT
        app_tws_if_ble_disconnected_handler();
#endif
    }

    app_gsound_env.savedMTU[conidx] = 0;
}

static int app_gsound_channel_connected_handler(ke_msg_id_t const msgid,
                                                struct gsound_gatt_connection_event_t *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id)
{
    GSoundBTAddr addr = {0};
    uint8_t conidx = KE_IDX_GET(src_id);

    if (MOBILE_CONNECT_IOS != gsound_get_connected_mobile_type())
    {
        GLOG_I("disconnect BLE because mobile type:%d", gsound_get_connected_mobile_type());
        app_ble_start_disconnect(conidx);
    }

    appm_check_and_resolve_ble_address(conidx);

    GLOG_I("conidx %d isGotSolvedBdAddr %d",
           conidx,
           app_env.context[conidx].isGotSolvedBdAddr);

    if (!app_env.context[conidx].isGotSolvedBdAddr)
    {
        GLOG_I("Gsound BLE addr RESOLVING");
        return KE_MSG_SAVED;
    }
    else
    {
        uint8_t status;
        gsound_ble_set_connection_idx(conidx);

        if (app_gsound_env.savedMTU[conidx] > 0)
        {
            app_gsound_env.mtu = app_gsound_env.savedMTU[conidx];
        }

        GLOG_I("Gsound connected with BLE public addr:");
        DUMP8("%02x ", app_env.context[conidx].solvedBdAddr, BD_ADDR_LEN);

        gsound_convert_bdaddr_to_gsound(&(app_env.context[conidx].solvedBdAddr[0]), &addr);

        // Attempt to connect. This is a synchronous call

        APP_BLE_NEGOTIATED_CONN_PARAM_T connParam;
        bool isGetNegotiatedConnParamSuccessful =
            app_ble_get_connection_interval(conidx, &connParam);
        if (!isGetNegotiatedConnParamSuccessful ||
            (GSOUND_STATUS_OK !=
             ble_interface->gsound_ble_on_connect(param->channel, &addr,
                                                  connParam.con_interval, connParam.con_latency)))
        {
            // If not success, need to inform remote client
            GLOG_I("BLE connect failure");
            status = ATT_ERR_APP_ERROR;
        }
        else
        {
            gsound_set_ble_connect_state(param->channel, true);
#ifdef IBRT
            // app_tws_if_sync_info(TWS_SYNC_USER_AI_CONNECTION);
#endif

#ifdef IS_MULTI_AI_ENABLED
            if (app_ai_manager_is_in_multi_ai_mode())
            {
                if (param->channel == GSOUND_CHANNEL_AUDIO)
                {
                    app_ai_manager_set_spec_connected_status(AI_SPEC_GSOUND, 1);
                }
            }
#endif

            // On success, start other GATT servers
#if (BLE_APP_ANCC)
            app_ancc_enable(app_gsound_ble_get_connection_index());
#endif
#if (BLE_APP_AMS)
            app_amsc_enable(app_gsound_ble_get_connection_index());
#endif

            GLOG_I("BLE connect success");
            status = GAP_ERR_NO_ERROR;
        }

        struct gsound_gatt_connection_event_t *cfm =
            KE_MSG_ALLOC(GSOUND_SEND_CONNECTION_CFM,
                         KE_BUILD_ID(prf_get_task_from_id(TASK_ID_VOICEPATH),
                                     conidx),
                         dest_id,
                         gsound_gatt_connection_event_t);
        *cfm = *param;
        cfm->status = status;
        ke_msg_send(cfm);
        return KE_MSG_CONSUMED;
    }
}

static int app_gsound_channel_disconnected_handler(ke_msg_id_t const msgid,
                                                   struct gsound_gatt_connection_event_t *param,
                                                   ke_task_id_t const dest_id,
                                                   ke_task_id_t const src_id)
{
    ble_interface->gsound_ble_on_disconnect(param->channel);
    gsound_set_ble_connect_state(param->channel, false);

    if (0 == gsound_get_ble_connect_state())
    {
        gsound_ble_set_connection_idx(BLE_INVALID_CONNECTION_INDEX);
    }

#ifdef IS_MULTI_AI_ENABLED
    if (app_ai_manager_is_in_multi_ai_mode())
    {
        if (param->channel == GSOUND_CHANNEL_AUDIO)
        {
            app_ai_manager_set_spec_connected_status(AI_SPEC_GSOUND, 0);
        }
    }
#endif

    uint8_t conidx = KE_IDX_GET(src_id);

    struct gsound_gatt_connection_event_t *cfm =
        KE_MSG_ALLOC(GSOUND_SEND_DISCONNECTION_CFM,
                     KE_BUILD_ID(prf_get_task_from_id(TASK_ID_VOICEPATH),
                                 conidx),
                     dest_id,
                     gsound_gatt_connection_event_t);

    *cfm = *param;
    cfm->status = GAP_ERR_NO_ERROR;
    ke_msg_send(cfm);

    gsound_custom_ota_exit();
    return KE_MSG_CONSUMED;
}

static int app_gsound_rx_ready(GSoundChannelType channel,
                               struct gsound_gatt_rx_data_ind_t *param)
{
    ble_interface->gsound_ble_on_rx_ready(channel, param->data, param->length);
    return KE_MSG_NO_FREE;
}

void app_gsound_rx_control_data_handler(uint8_t *ptrData, uint32_t dataLen)
{
    ble_interface->gsound_ble_on_rx_ready(GSOUND_CHANNEL_CONTROL, ptrData, dataLen);
}

static int app_gsound_control_received_handler(ke_msg_id_t const msgid,
                                               struct gsound_gatt_rx_data_ind_t *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
    app_ble_push_rx_data(BLE_RX_DATA_GSOUND_CONTROL,
                         app_gsound_ble_get_connection_index(),
                         param->data,
                         param->length);
    return KE_MSG_NO_FREE;
}

static int app_gsound_audio_received_handler(ke_msg_id_t const msgid,
                                             struct gsound_gatt_rx_data_ind_t *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    return app_gsound_rx_ready(GSOUND_CHANNEL_AUDIO, param);
}

void gsound_ble_conn_parameter_updated(uint8_t conidx,
                                       uint16_t connInterval,
                                       uint16_t connSlavelatency)
{
    GLOG_I("[%s]+++", __func__);

    GLOG_I("interval:%d, connSlavelatency:%d", connInterval, connSlavelatency);

    GLOG_I("[%s]---", __func__);

    ble_interface->gsound_ble_on_param_change(connInterval, connSlavelatency);
}

void app_gsound_ble_mtu_exchanged_handler(uint8_t conidx, uint16_t mtu)
{
    if (conidx == app_gsound_ble_get_connection_index())
    {
        GLOG_I("%s: mtu=%d", __FUNCTION__, mtu);
        app_gsound_env.mtu = mtu - 4;
    }
    else
    {
        app_gsound_env.savedMTU[conidx] = mtu - 4;
    }
}

// Assumes gsound_ble_mutex_id is locked.
bool app_gsound_ble_send_notification(GSoundChannelType channel,
                                      uint16_t length,
                                      const uint8_t *data)
{
    GLOG_I("%s %s", __func__, gsound_chnl2str(channel));

    uint8_t conidx = app_gsound_ble_get_connection_index();
    
    if (gattc_check_if_notification_processing_is_busy(conidx))
    {
        return false;
    }

    osMutexWait(gsound_ble_mutex_id, osWaitForever);

    if (app_gsound_env.inflight_env.count >= MAXIMUM_INFLIGHT_TX_PACKET_CNT)
    {
        GLOG_I("%s: Maximum inflight tx reaches!", __func__);
        osMutexRelease(gsound_ble_mutex_id);
        return false;
    }

    uint8_t indexToFill = app_gsound_env.inflight_env.inputIndex;

    GSOUND_TX_INSTANCE_T *inflight_tx =
        &(app_gsound_env.inflight_env.inflight_tx[indexToFill]);

    inflight_tx->len = length;
    inflight_tx->chnl = channel;
    inflight_tx->data = data;

    indexToFill++;
    if (MAXIMUM_INFLIGHT_TX_PACKET_CNT == indexToFill)
    {
        indexToFill = 0;
    }

    app_gsound_env.inflight_env.inputIndex = indexToFill;
    app_gsound_env.inflight_env.count++;
    osMutexRelease(gsound_ble_mutex_id);

    struct gsound_gatt_tx_data_req_t *req =
        KE_MSG_ALLOC_DYN(GSOUND_SEND_DATA_REQUEST,
                         KE_BUILD_ID(prf_get_task_from_id(TASK_ID_VOICEPATH),
                                     app_gsound_ble_get_connection_index()),
                         TASK_APP,
                         gsound_gatt_tx_data_req_t,
                         length);

    req->connectionIndex = conidx;
    req->channel         = channel;
    req->length          = length;

    memcpy((uint8_t *)req->data, data, length);

    ke_msg_send(req);
    return true;
}

static int tx_complete_handler_locked(GSoundChannelType *pType, bool isSuccess)
{
    *pType = GSOUND_NUM_CHANNEL_TYPES;

    osMutexWait(gsound_ble_mutex_id, osWaitForever);
    if (0 == app_gsound_env.inflight_env.count && isSuccess)
    {
        osMutexRelease(gsound_ble_mutex_id);
        ASSERT(false, "%s: No notification in flight!", __func__);
        return KE_MSG_CONSUMED;
    }

    // If no valid connection, do not transmit - we don't want to
    // alter the shared Tx buffer in case in use by BT Classic
    if (!app_gsound_ble_is_connected())
    {
        // Note inflight != 0 at this point. However during disconnect inflight is set
        // to 0. Thus this case can only occur if locally preempted.
        GLOG_I("%s: TX - INVALID CONNECTION !!!", __func__);
        osMutexRelease(gsound_ble_mutex_id);
        return KE_MSG_CONSUMED;
    }

    GLOG_I("tx done.");

    uint8_t indexToGet = app_gsound_env.inflight_env.outputIndex;

    GSOUND_TX_INSTANCE_T *inflight_tx =
        &(app_gsound_env.inflight_env.inflight_tx[indexToGet]);

    indexToGet++;
    if (MAXIMUM_INFLIGHT_TX_PACKET_CNT == indexToGet)
    {
        indexToGet = 0;
    }

    app_gsound_env.inflight_env.outputIndex = indexToGet;
    app_gsound_env.inflight_env.count--;

    *pType = inflight_tx->chnl;
    osMutexRelease(gsound_ble_mutex_id);

    return KE_MSG_CONSUMED;
}

static int app_gsound_notify_complete_handler(ke_msg_id_t const msgid,
                                              struct gsound_gatt_tx_complete_ind_t *param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    GSoundChannelType channelType;
    int value = tx_complete_handler_locked(&channelType, param->success);

    if (GSOUND_NUM_CHANNEL_TYPES != channelType)
    {
        ble_interface->gsound_ble_on_tx_available(GSOUND_CHANNEL_CONTROL);
        ble_interface->gsound_ble_on_tx_available(GSOUND_CHANNEL_AUDIO);
    }

    return value;
}

void gsound_custom_ble_disconnect_ble_link(void)
{
    if (app_gsound_ble_is_connected())
    {
        app_ble_start_disconnect(app_gsound_ble_get_connection_index());
    }
}
