#ifndef __APP_GMA_DATA_HANDLER_H__
#define __APP_GMA_DATA_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_gma_cmd_code.h"
#include "app_gma_state_env.h"

typedef struct {
    uint8_t     cmdType;
    uint8_t     cmdLen;
} APP_GMA_STAT_CHANGE_CMD_T;

typedef struct {
    uint8_t     cmdType;
    uint8_t     cmdLen;
    uint8_t     value;
} APP_GMA_STAT_CHANGE_RSP_T;

typedef struct {
    uint8_t     system;
    uint8_t     gmaVer[2];
} APP_GMA_PHONE_INFO_T;

typedef struct {
    uint8_t     capacity[2];
    uint8_t     audioType;
    uint8_t     gmaVer[2];
    uint8_t     btAddr[6];
    uint8_t     btAddr_slave[6];
} APP_GMA_DEVICE_INFO_T;

typedef struct {
    uint8_t     random[16];
    uint8_t     digest[16];
} APP_GMA_DEV_ACTIV_RSP_T;


void app_gma_data_xfer_started(APP_GMA_CMD_RET_STATUS_E retStatus);
void app_gma_data_xfer_stopped(APP_GMA_CMD_RET_STATUS_E retStatus);
void app_gma_data_segment_verified(APP_GMA_CMD_RET_STATUS_E retStatus);
void app_gma_data_received_callback(uint8_t* ptrData, uint32_t dataLength);
uint32_t app_gma_send_voice_stream(void *param1, uint32_t param2);
uint32_t app_gma_audio_stream_handle(void *param1, uint32_t param2);
uint32_t app_gma_rcv_data(void *param1, uint32_t param2);
bool app_gma_data_is_data_transmission_started(void);
void app_gma_kickoff_dataxfer(void);
void app_gma_stop_dataxfer(void);
void app_gma_data_reset_env(void);
void app_gma_dev_data_xfer(APP_GMA_DEV_STATUS_E isStart);
void app_gma_device_infor_reset(void);
void gma_bt_addr_order_revers(uint8_t* bt_addr, uint16_t len);

#ifdef __cplusplus
    }
#endif

#endif // #ifndef __APP_GMA_DATA_HANDLER_H__

