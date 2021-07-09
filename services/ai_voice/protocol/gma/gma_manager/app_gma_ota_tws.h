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
#ifndef __GMA_OTA_TWS_H__
#define __GMA_OTA_TWS_H__

#include "cqueue.h"
#include "cmsis_os.h"
#include "app_gma_ota_tws.h"
#include "app_gma_cmd_code.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RESEND_TIME 2

#ifndef APP_TWS_MAXIMUM_CMD_DATA_SIZE
#define APP_TWS_MAXIMUM_CMD_DATA_SIZE 512
#endif
#define GMA_OTA_TWS_BUFSIZE APP_TWS_MAXIMUM_CMD_DATA_SIZE
// prefix of the gsuond_tws_data see 
#define GMA_OTA_TWS_HEAD_SIZE (sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t))
// payload max length is tws_buffer_size minimize the prefix, see @GMA_OTA_TWS_T
#define GMA_OTA_TWS_PAYLOAD_SIZE (GMA_OTA_TWS_BUFSIZE - GMA_OTA_TWS_HEAD_SIZE)
#define GMA_MAX_MTU 1024
#define GMA_OTA_TWS_PACKET_LENGTH (GMA_MAX_MTU - 19)  //
#define GMA_SIGNAL_NUM 10
#define GMA_THREAD_SIGNAL 11
#define GMApacketCompleteMagicCode 0x28269395
#define GMApacketIncompleteMagicCode 0x17188284
#define GMA_OTA_TWS_WAITTIME 2000

typedef enum {
    GMA_OTA_BEGIN = 0xA0,  // from central to device, to let OTA start
    GMA_OTA_DATA,
    GMA_OTA_CRC,
    GMA_OTA_APPLY,
    GMA_OTA_ABORT,
    GMA_OTA_RSP_RESULT,
    GMA_OTA_RSP_FINAL_RESULT,
} GMA_OTA_CONTROL_PACKET_TYPE_E;

typedef struct
{
    APP_GMA_CMD_RET_STATUS_E ota_result;
} __attribute__((__packed__)) GMA_OTA_TWS_RSP_T;

typedef struct
{
  //  GSoundOtaSessionSettings ota_begin;
} __attribute__((__packed__)) GMA_OTA_CMD_BEGIN_T;

typedef struct
{
    uint32_t device_index;
} __attribute__((__packed__)) GMA_OTA_CMD_APPLY_T;

typedef struct
{
    uint32_t device_index;
} __attribute__((__packed__)) GMA_OTA_CMD_ABORT_T;

typedef struct
{
    uint8_t  packetType;
    uint32_t magicCode;
    uint16_t length;
    uint8_t  data[GMA_OTA_TWS_PAYLOAD_SIZE];
} __attribute__((__packed__)) GMA_OTA_TWS_DATA_T;

typedef struct {
    osThreadId   txThreadId;
    osThreadId   rxThreadId;
    CQueue       txQueue;
    CQueue       rxQueue;
    APP_GMA_CMD_RET_STATUS_E peerResult;
    uint32_t     currentReceivedMagicCode;
    uint8_t      currentReceivedPacketType;
    uint8_t      txBuf[GMA_OTA_TWS_BUFSIZE];
    uint8_t      rxBuf[GMA_MAX_MTU];
} GMA_OTA_TWS_ENV_T;

void app_gma_ota_tws_init(void);

void app_gma_ota_tws_transfer(uint8_t *pDataBuf, uint16_t dataLength);

bool app_gma_ota_tws_rev_cmd_handler(uint8_t *p_buff, uint32_t rsp_seq);

void app_gma_ota_tws_rev_cmd_rsp_handler(uint8_t *p_buff, uint16_t length);

void app_gma_ota_tws_rev_cmd_rsp_timeout_handler(uint8_t *p_buff, uint16_t length);

void app_gma_ota_tws_rev_data_form_peer_handler(uint8_t *p_buff, uint16_t length);

void app_gma_ota_update_peer_result(APP_GMA_CMD_RET_STATUS_E ota_result);

void app_gma_ota_tws_rsp(APP_GMA_CMD_RET_STATUS_E ota_result);

APP_GMA_CMD_RET_STATUS_E app_gma_ota_get_peer_result(void);


void app_gma_ota_tws_push_data_into_cqueue(CQueue *ptrQueue, osMutexId queueMutex, uint8_t *buff, uint32_t len);

void app_gma_ota_tws_peer_data_handler(uint8_t *ptrParam, uint32_t paramLen);

osMutexId app_gma_ota_tws_get_tx_queue_mutex(void);

void lock_gma_tws_tx_queue(void);

void unlock_gma_tws_tx_queue(void);

osMutexId app_gma_ota_tws_get_rx_queue_mutex(void);

void lock_gma_tws_rx_queue(void);

void umlock_gma_tws_rx_queue(void);

void app_gma_ota_tws_deinit(void);

void app_gma_ota_tws_reset(void);

void app_gma_ota_check_cached_slave_req(void);
void app_gma_ota_reset_cached_slave_req(void);
void app_gma_ota_tws_rev_cmd_cached_handler(uint16_t rsp_seq, uint8_t *p_buff);


extern uint8_t gma_start_ota;
extern bool gma_crc;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

