/**
 ****************************************************************************************
 *
 * @file app_gma_ble.h
 *
 * @brief Gma Voice Application entry point
 *
 * Copyright (C) BES
 *
 *
 ****************************************************************************************
 */

#ifndef APP_GMA_BLE_H_
#define APP_GMA_BLE_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>          // Standard Integer Definition

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t     gma_gatt_adv_lenth;
    uint8_t     gma_gatt_adv_type;
    uint8_t     gma_gatt_adv_cid[2];
    uint8_t     gma_gatt_adv_vid;
    uint8_t     gma_gatt_adv_fmsk;
    uint8_t     gma_gatt_adv_pid[4];
    uint8_t     gma_gatt_adv_mac[6];
    uint8_t     gam_gatt_adv_extend[2];
}__attribute__ ((__packed__)) GMA_GATT_ADV_DATA_PAC;

#if defined(GMA_HOLD_ACTIVE_ENABLE)
typedef struct
{
    uint8_t     gma_gatt_ibeacon_lenth;
    uint8_t     gma_gatt_ibeacon_type;
    
    /*Apple Pre-Amble*/
    uint8_t     gma_gatt_ibeacon_preAmble[4];

    /*Device UUID */
    uint8_t     gma_gatt_ibeacon_cid[2];
    uint8_t     gma_gatt_ibeacon_vid;
    uint8_t     gma_gatt_ibeacon_pid[4];
    uint8_t     gma_gatt_ibeacon_mac[6];
    uint8_t     gam_gatt_ibeacon_rfu[3];
    /*Major Vaule*/
    uint8_t     gma_gatt_ibeacon_major[2];
    /*Minor Vaule*/
    uint8_t     gma_gatt_ibeacon_Minor[2];
    /*Measured Power*/
    uint8_t     gma_gatt_ibeacon_measured_Power;
}__attribute__ ((__packed__)) GMA_GATT_IBEACON_DATA_PAC;

extern GMA_GATT_IBEACON_DATA_PAC gma_gatt_ibeacon_data;
#endif

extern GMA_GATT_ADV_DATA_PAC gma_gatt_adv_data;
/**
 ****************************************************************************************
 * @brief Add a DataPath Server instance in the DB
 ****************************************************************************************
 */
void app_gma_send_cmd_via_notification(uint8_t* ptrData, uint32_t length);

void app_gma_send_cmd_via_indication(uint8_t* ptrData, uint32_t length);

void app_gma_send_data_via_notification(uint8_t* ptrData, uint32_t length);

void app_gma_send_data_via_indication(uint8_t* ptrData, uint32_t length);


#ifdef __cplusplus
    }
#endif


/// @} APP

#endif // APP_GMA_BLE_H_
