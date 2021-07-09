#ifndef APP_TENCENT_BLE_H_
#define APP_TENCENT_BLE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Smart Voice Application entry point.
 *
 * @{
 ****************************************************************************************
 */

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

/**
 ****************************************************************************************
 * @brief Add a DataPath Server instance in the DB
 ****************************************************************************************
 */
bool app_tencent_send_cmd_via_notification(uint8_t* ptrData, uint32_t length);

void app_tencent_send_cmd_via_indication(uint8_t* ptrData, uint32_t length);

void app_tencent_send_data_via_notification(uint8_t* ptrData, uint32_t length);

void app_tencent_send_data_via_indication(uint8_t* ptrData, uint32_t length);

#ifdef __cplusplus
}
#endif

/// @} APP

#endif // APP_TENCENT_BLE_H_
