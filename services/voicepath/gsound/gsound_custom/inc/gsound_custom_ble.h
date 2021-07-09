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

#ifndef __GSOUND_CUSTOM_BLE_H__
#define __GSOUND_CUSTOM_BLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*****************************header include********************************/
#include "ke_msg.h"
#include "ke_task.h"
#include "prf.h"
#include "gsound_target.h"

/******************************macro defination*****************************/

/******************************type defination******************************/

/****************************function declearation**************************/
/*---------------------------------------------------------------------------
 *            app_gsound_ble_init
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    initialize gsound related ble configuration
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void app_gsound_ble_init(void);

/*---------------------------------------------------------------------------
 *            app_gsound_ble_data_fill_handler
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    handler to fill the gsound related BLE advertisement and scan response data
 *
 * Parameters:
 *    param - pointer of next BLE advertisement parameter
 *
 * Return:
 *    void
 */
void app_gsound_ble_data_fill_handler(void *param);

/*---------------------------------------------------------------------------
 *            app_gsound_ble_is_connected
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get the gsound BLE connection status
 *
 * Parameters:
 *    void
 *
 * Return:
 *    true - gsound BLE is connected
 *    flase - gsound BLE is not connected
 */
bool app_gsound_ble_is_connected(void);

uint8_t app_gsound_ble_get_mtu(void);

const struct ke_state_handler *gsound_ble_get_msg_handler_table(void);

void gsound_ble_conn_parameter_updated(uint8_t conidx,
                                       uint16_t minConnIntervalInMs,
                                       uint16_t maxConnIntervalInMs);

void app_gsound_rx_control_data_handler(uint8_t *ptrData, uint32_t dataLen);

void gsound_ble_register_target_handle(const GSoundBleInterface *handler);

void app_gsound_ble_add_svc(void);

uint8_t app_gsound_ble_get_connection_index(void);

bool app_gsound_ble_send_notification(GSoundChannelType channel,
                                      uint16_t length,
                                      const uint8_t *data);

void app_gsound_ble_mtu_exchanged_handler(uint8_t conidx, uint16_t mtu);

void app_gsound_ble_voicepath_disconnected_evt_handler(uint8_t conidx);

void gsound_custom_ble_disconnect_ble_link(void);

void app_gsound_ble_disconnected_evt_handler(uint8_t conidx);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __GSOUND_CUSTOM_BLE_H__ */