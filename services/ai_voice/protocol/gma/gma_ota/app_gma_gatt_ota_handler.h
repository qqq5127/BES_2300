#if 0
#ifndef __APP_GMA_GATT_OTA_HANDLER_H
#define __APP_GMA_GATT_OTA_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

typedef enum
{
    NORMAL_MODE = 0,
    FAST_MODE = 1,
} GMA_DEV_OTA_TRANSFER_MODE_E;

#define OTA_TRANSFER_MODE FAST_MODE

typedef struct
{
    uint8_t type;           // FW type
    uint8_t revise_nb;
    uint8_t second_ver_nb;
    uint8_t primary_ver_nb;
    uint8_t reservation;
} __attribute__ ((__packed__))GMA_DEV_FW_Ver_T;

typedef struct
{
    GMA_DEV_FW_Ver_T FW_Ver;
    uint32_t FW_size;
    uint16_t CRC16;
    uint8_t flag;
} __attribute__ ((__packed__))GMA_OTA_UPDATE_REQ_T;

typedef struct
{
    uint8_t allow;
    uint32_t last_total_nb;
    uint8_t loop_pkt_nb;
} __attribute__ ((__packed__))GMA_OTA_UPDATE_RSP_T;

typedef struct
{
    uint8_t current_frame_nb;
    uint32_t total_received_size;
} __attribute__ ((__packed__))GMA_OTA_RECEIVED_RSP_T;

typedef struct
{
    GMA_OTA_UPDATE_REQ_T g_update_req;
    uint8_t g_current_frame_number;
    int8_t g_last_frame_number;
    uint32_t g_total_received_size;
    bool need_response;
} __attribute__ ((__packed__))GMA_OTA_ENV_T;

bool app_gma_gatt_ota_FW_valid(GMA_DEV_FW_Ver_T *FW_Ver);
void app_gma_gatt_ota_FW_Ver_set(GMA_DEV_FW_Ver_T *FW_Ver);
void app_gma_gatt_ota_FW_Ver_get(GMA_DEV_FW_Ver_T *FW_Ver);
int8_t app_gma_gatt_ota_FW_Ver_compare(GMA_DEV_FW_Ver_T *FW1, GMA_DEV_FW_Ver_T *FW2);
void app_gma_gatt_ota_FW_Ver_print(GMA_DEV_FW_Ver_T *FW_Ver);

#ifdef __cplusplus
	}
#endif

#endif
#endif
