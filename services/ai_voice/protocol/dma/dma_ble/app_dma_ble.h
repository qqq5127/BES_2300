#ifndef APP_DMA_BLE_H_
#define APP_DMA_BLE_H_

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

void app_dma_send_cmd_via_notification(uint8_t* ptrData, uint32_t length);

void app_dma_send_cmd_via_indication(uint8_t* ptrData, uint32_t length);

bool app_dma_send_data_via_notification(uint8_t* ptrData, uint32_t length);

void app_dma_send_data_via_indication(uint8_t* ptrData, uint32_t length);

#ifdef __cplusplus
}
#endif


/// @} APP

#endif // APP_DMA_VOICE_H_
