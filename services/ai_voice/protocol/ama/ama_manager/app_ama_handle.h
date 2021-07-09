
#ifndef __APP_AMA_HANDLE_H__
#define __APP_AMA_HANDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#define APP_AMA_SPEECH_STATE_TIMEOUT_INTERVEL           (90000)

#if VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_OPUS
    #ifdef __KWS_ALEXA__
    #define SPP_TRANS_SIZE  80
    #define SPP_TRANS_TIMES 1
    #define BLE_TRANS_SIZE  80
    #define BLE_TRANS_TINES 1
    #else
    #define SPP_TRANS_SIZE  320
    #define SPP_TRANS_TIMES 1
    #define BLE_TRANS_SIZE  160
    #define BLE_TRANS_TINES 1
    #endif
#elif VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_SBC
    #define SPP_TRANS_SIZE  342
    #define SPP_TRANS_TIMES 3
    #define BLE_TRANS_SIZE  180
    #define BLE_TRANS_TINES 6
#else
    #define SPP_TRANS_SIZE  80
    #define SPP_TRANS_TIMES 1
    #define BLE_TRANS_SIZE  80
    #define BLE_TRANS_TINES 1
#endif
/// Table of ama handlers function
extern const struct ai_handler_func_table ama_handler_function_table;

int32_t app_ama_event_notify(uint32_t msgID,uint32_t msgPtr, uint32_t param0, uint32_t param1);


#ifdef __cplusplus
}
#endif
#endif
