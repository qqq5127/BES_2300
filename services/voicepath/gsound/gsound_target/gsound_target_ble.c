#include "gsound_target.h"
#include "gsound_dbg.h"
#include "ke_msg.h"
#include "gapc_task.h"
#include "app.h"
#include "app_ble_mode_switch.h"
#include "gsound_gatt_server.h"
#include "gsound_custom_ble.h"
#include "app_bt_func.h"


void GSoundTargetBleRxComplete(GSoundChannelType type,
                               const uint8_t *buffer,
                               uint32_t len)
{
    struct gsound_gatt_rx_data_ind_t *last_rx_packet =
        ( struct gsound_gatt_rx_data_ind_t * )(( uint8_t * )buffer -
                                               offsetof(
                                                   struct gsound_gatt_rx_data_ind_t,
                                                   data));

    app_bt_start_custom_function_in_bt_thread(( uint32_t )ke_param2msg((last_rx_packet)),
                                           0,
                                           ( uint32_t )ke_msg_free);

    app_bt_start_custom_function_in_bt_thread(app_gsound_ble_get_connection_index(),
                                           0,
                                           ( uint32_t )gsound_send_control_rx_cfm);

}

GSoundStatus GSoundTargetBleTransmit(GSoundChannelType channel,
                                     const uint8_t *data,
                                     uint32_t length,
                                     uint32_t *bytes_consumed)
{
    // GLOG_D("[%s]channel=%s, len=%d",
    //       __func__,
    //       gsound_chnl2str(channel),
    //       length);

    ASSERT(GSoundTargetOsMqActive(), "Mq not active");
    *bytes_consumed = 0;

    // If no valid connection, do not transmit - we don't want to
    // alter the shared Tx buffer in case in use by BT Classic
    if (!app_gsound_ble_is_connected())
    {
        GLOG_I("%s: TX - INVALID CONNECTION !!!", __func__);
        return GSOUND_STATUS_ERROR;
    }

    bool isConsumedImmediately = app_gsound_ble_send_notification(channel, length, data);

    if (isConsumedImmediately)
    {
        *bytes_consumed = length;
        return GSOUND_STATUS_OK;
    }
    else
    {
        return GSOUND_STATUS_OUT_OF_MEMORY;
    }
}

uint16_t GSoundTargetBleGetMtu()
{
    return app_gsound_ble_get_mtu();
}

GSoundStatus GSoundTargetBleUpdateConnParams(bool valid_interval,
                                             uint32_t min_interval,
                                             uint32_t max_interval,
                                             bool valid_slave_latency,
                                             uint32_t slave_latency)
{
    uint8_t conidx = app_gsound_ble_get_connection_index();

    if (valid_interval)
    {
        l2cap_update_param(conidx, min_interval*8/5, 
            max_interval*8/5, 6000, slave_latency);
    }

    return GSOUND_STATUS_OK;
}

GSoundStatus GSoundTargetBleInit(const GSoundBleInterface *handlers)
{
    GLOG_D("%s ", __func__);

    gsound_ble_register_target_handle(handlers);

    // add gsound related data into adv
    app_ble_register_data_fill_handle(USER_GSOUND, ( BLE_DATA_FILL_FUNC_T )app_gsound_ble_data_fill_handler, false);

    return GSOUND_STATUS_OK;
}
