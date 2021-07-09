#ifndef __APP_GMA_CMD_CODE_H__
#define __APP_GMA_CMD_CODE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
extern uint32_t __custom_handler_table_start[];
extern uint32_t __custom_handler_table_end[];

#define LOG_MODULE HAL_TRACE_MODULE_APP

#define INVALID_CUSTOM_ENTRY_INDEX      0xFFFF

#define GMA_CAPACITY_WAKE_UP_WORD    0
#define GMA_CAPACITY_VAD             0
#define GMA_CAPACITY_ECHO_CANCEL     0
#define GMA_CAPACITY_NOISE_REDUCT    0
#define GMA_CAPACITY_SPEAKER         0
#define GMA_CAPACITY_FM              0
#define GMA_CAPACITY_HFP             1
#if VOB_ENCODING_ALGORITHM==ENCODING_ALGORITHM_OPUS
#define GMA_CAPACITY_AUDIO_TYPE     (0x02)
#else
#define GMA_CAPACITY_AUDIO_TYPE     (0x00)
#endif
#define GMA_CAPACITY_GMA_VERSION    {0x01, 0x00}

#define GMA_GATT_SERVICE_UUID       0xFEB3
#define GMA_GATT_ADV_LENGTH         (18)
#define GMA_GATT_ADV_DATA_LENGTH    17
#define GMA_GATT_ADV_DATA_TYPE      0xff
#define GMA_GATT_ADV_DATA_CID       {0xa8, 0x01}
#define GMA_GATT_ADV_DATA_VID       0xa5
#define GMA_GATT_ADV_DATA_FMSK      0x0D
//pid DEC 794
#define GMA_GATT_ADV_DATA_PID       {0x9b, 0x03, 0x00, 0x00}
#define GMA_CRYPTO_PID      "0000039b"
#define GMA_CRYPTO_SECRET   "7a2669d49f3235e2b2e483b70ae7d869"//"ee62a37415853371aaeb60f229c82c25"
#define GMA_IV_ORIGIN       "123aqwed#*$!(4ju"

#define GMA_SHA256_RST_LEN      16

#define GMA_CAPACITY_B1     ((uint8_t)GMA_CAPACITY_WAKE_UP_WORD | (GMA_CAPACITY_VAD<<1) | (GMA_CAPACITY_ECHO_CANCEL<<2) | \
                    (GMA_CAPACITY_NOISE_REDUCT<<3) | (GMA_CAPACITY_SPEAKER<<4) | (GMA_CAPACITY_FM<<5) | (GMA_CAPACITY_HFP<<6))

#if defined(GMA_HOLD_ACTIVE_ENABLE)
#define GMA_GATT_IBEACON_LENGTH         (27)
#define GMA_GATT_IBEACON_DATA_LENGTH    26
#define GMA_GATT_IBEACON_DATA_TYPE      0xff
#define GMA_GATT_IBEACON_DATA_PREAMBLE  {0x4C, 0x00, 0x02, 0x15} /*Apple Pre-Amble*/
#define GMA_GATT_IBEACON_DATA_CID       {0xa8, 0x01}
#define GMA_GATT_IBEACON_DATA_VID       0x01
#define GMA_GATT_IBEACON_DATA_MAC       {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define GMA_GATT_IBEACON_DATA_RFU       {0x00, 0x00, 0x00}
#define GMA_GATT_IBEACON_DATA_MAJOR     {0x00, 0x01} /*Major Vaule*/
#define GMA_GATT_IBEACON_DATA_MINOR     {0x00, 0x02} /*Minor Vaule*/
#define GMA_GATT_IBEACON_DATA_MEASURED_POWER    0xCD /*Measured Power*/
#define GMA_GATT_IBEACON_DATA_PID       {0x00, 0x00, 0x00, 0x00}
#endif
/**
 * @brief The command code
 *
 */
typedef enum
{
    GMA_OP_SEC_CERT_RANDOM = 0x10,      //  ->      app_gma_crypto_handler
    GMA_OP_SEC_CERT_CIPHER = 0x11,
    GMA_OP_SEC_CERT_RESULT = 0x12,      //  ->      app_gma_crypto_handler   
    GMA_OP_SEC_CERT_RESULT_RSP = 0x13,

    GMA_OP_OTA_VER_GET_CMD  =   0x20,   //  ->      app_gma_gatt_ota_handler
    GMA_OP_OTA_VER_GET_RSP  =   0x21,
    GMA_OP_OTA_UPDATE_CMD   =   0x22,
    GMA_OP_OTA_UPDATE_RSP   =   0x23,
    GMA_OP_OTA_PKG_RECV_RSP =   0x24,
    GMA_OP_OTA_COMP_NTF_CMD =   0x25,
    GMA_OP_OTA_COMP_CHECK_RSP = 0x26,
    GMA_OP_OTA_GET_RECVED_CMD = 0x27,
    GMA_OP_OTA_SEND_PKG_CMD =   0x2F,

    GMA_OP_DEV_SEND_STREAM = 0x30,
    GMA_OP_DEV_SEND_STATUS = 0x31,
    GMA_OP_DEV_INFO_CMD = 0x32,         //  ->      app_gma_dev_info_handler
    GMA_OP_DEV_INFO_RSP = 0x33,
    GMA_OP_DEV_ACTIV_CMD = 0x34,        //  ->      app_gma_dev_activate_handler
    GMA_OP_DEV_ACTIV_RSP = 0x35,
    GMA_OP_PHO_CHANGE_CMD = 0x36,       //  ->      app_gma_status_change_handler
    GMA_OP_PHO_CHANGE_RSP = 0x37,

    GMA_OP_MEDIA_CMD = 0x38,            //  ->      app_gma_media_control_handler
    GMA_OP_MEDIA_RSP = 0x39,
    GMA_OP_HFP_CMD = 0x3a,              //  ->      app_gma_hfp_control_handler
    GMA_OP_HFP_RSP = 0x3b,

    GMA_OP_DEV_CHANGE_CMD = 0x3e,       //  ->      
    GMA_OP_DEV_CHANGE_RSP = 0x3f,

    // device information(status)
    GMA_OP_EXTENDED_CMD = 0x40,         //  ->      app_gma_extended_cmd_handler
    GMA_OP_EXTENDED_RSP = 0x41,
#if defined(GMA_HOLD_ACTIVE_ENABLE)
    GMA_OP_HEARTBEAT_CMD = 0x50,         //  ->      app_gma_heartbeat_cmd_handler
#endif

    OP_CLOUD_MUSIC_PLAYING_CONTROL = 0x86,
    OP_CHOOSE_CLOUD_PLATFORM = 0x87,
    OP_SPP_DATA_ACK = 0x88,
    OP_INFORM_THROUGHPUT_TEST_CONFIG = 0x89,
    OP_THROUGHPUT_TEST_DATA = 0x8A,
    OP_THROUGHPUT_TEST_DATA_ACK = 0x8B,
    OP_THROUGHPUT_TEST_DONE = 0x8C,
    OP_INFORM_ALGORITHM_TYPE = 0x8D, 
    
    OP_COMMAND_COUNT,

    OP_DATA_XFER = 0xFF,    // to mark that it's a data packet  
        
} APP_GMA_CMD_CODE_E;

/**
 * @brief The Gma voice command handling return status
 *
 */
typedef enum
{
    NO_ERROR = 0,
    INVALID_CMD,
    INVALID_DATA_PACKET,
    PARAM_LEN_OUT_OF_RANGE,
    PARAMETER_LENGTH_TOO_SHORT,
    PARAM_LEN_TOO_SHORT,    
    CMD_HANDLING_FAILED,
    WAITING_RSP_TIMEOUT,
    DATA_XFER_ALREADY_STARTED,
    DATA_XFER_NOT_STARTED_YET,
    DATA_SEGMENT_CRC_CHECK_FAILED,
    WHOLE_DATA_CRC_CHECK_FAILED,
    DATA_XFER_LEN_NOT_MATCHED,  
    MUSIC_ALREADY_PLAYING,
    MUSIC_NOT_PLAYING_YET,  
    
    // TO ADD: new return status
} APP_GMA_CMD_RET_STATUS_E;

/**
 * @brief Format of the Gma voice command handler function, called when the command is received
 *
 * @param cmdCode   Custom command code
 * @param ptrParam  Pointer of the received parameter
 * @param paramLen  Length of the recevied parameter
 * 
 */
typedef void (*app_gma_cmd_handler_t)(APP_GMA_CMD_CODE_E cmdCode, uint8_t* ptrParam, uint32_t paramLen);

/**
 * @brief Format of the Gma voice command response handler function, 
 *  called when the response to formerly sent command is received
 *
 * @param retStatus Handling return status of the command
 * @param ptrParam  Pointer of the received parameter
 * @param paramLen  Length of the recevied parameter
 * 
 */
typedef void (*app_gma_cmd_response_handler_t)(APP_GMA_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);

/**
 * @brief Gma voice command definition data structure
 *
 */
typedef struct
{
    uint8_t             cmdCode;
    app_gma_cmd_handler_t   cmdHandler;             /**< command handler function */
    uint16_t                isNeedResponse;         /**< true if needs the response from the peer device */
    uint16_t                timeoutWaitingRspInMs;  /**< time-out of waiting for response in milli-seconds */
    app_gma_cmd_response_handler_t  cmdRspHandler;  /**< command response handler function */
} APP_GMA_CMD_INSTANCE_T;


#define CUSTOM_COMMAND_TO_ADD(cmdCode, cmdHandler, isNeedResponse, timeoutWaitingRspInMs, cmdRspHandler)    \
    static APP_GMA_CMD_INSTANCE_T cmdCode##_entry __attribute__((used, section(".custom_handler_table"))) =     \
        {(cmdCode), (cmdHandler), (isNeedResponse), (timeoutWaitingRspInMs), (cmdRspHandler)};

#define CUSTOM_COMMAND_PTR_FROM_ENTRY_INDEX(index)  \
    ((APP_GMA_CMD_INSTANCE_T *)((uint32_t)__custom_handler_table_start + (index)*sizeof(APP_GMA_CMD_INSTANCE_T)))



#ifdef __cplusplus
    }
#endif


#endif // #ifndef __APP_GMA_CMD_CODE_H__

