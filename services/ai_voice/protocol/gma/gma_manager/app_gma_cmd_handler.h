#ifndef __APP_GMA_CMD_HANDLER_H__
#define __APP_GMA_CMD_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_timer.h"
#include "app_gma_cmd_code.h"

#define GET_CURRENT_TICKS()         hal_sys_timer_get()
#define GET_CURRENT_MS()            TICKS_TO_MS(GET_CURRENT_TICKS())


/**< maximum payload size of one smart voice command */
#define MAXIMUM_DATA_PACKET_LEN     256
#define APP_GMA_CMD_MAXIMUM_PAYLOAD_SIZE    MAXIMUM_DATA_PACKET_LEN

/**
 * @brief BLE custom command playload
 *
 */
typedef struct
{
    uint8_t     header0;    // b7~b5:Version,   b4:Encrypt, b3~b0:Msg ID
    uint8_t     cmdCode;    // Cmd Tpye
    uint8_t     sequence;   // b7~b4:Total Frame,  b3~b0:Frame Seq
    uint8_t     length;     // Frame Length
    uint8_t     param[APP_GMA_CMD_MAXIMUM_PAYLOAD_SIZE - 4*sizeof(uint8_t)];    // PayLoad
} __attribute__((packed))APP_GMA_CMD_PAYLOAD_T;

typedef struct
{
    uint8_t         cmdType;
    uint8_t         cmdLen;             /**< command handler function */
    uint8_t         payLoad[APP_GMA_CMD_MAXIMUM_PAYLOAD_SIZE - 2*sizeof(uint8_t)];
} __attribute__((packed))APP_GMA_CMD_TLV_T;

typedef struct
{
    uint8_t     header0;    // b7~b5:Version,   b4:Encrypt, b3~b0:Msg ID
    uint8_t     cmdCode;    // Cmd Tpye
    uint8_t     sequence;   // b7~b4:Total Frame,  b3~b0:Frame Seq
    uint8_t     length;     // Frame Length

    uint8_t     RefandMic;
    uint16_t    micLength;
    uint16_t    refLength;
    
     uint8_t     payLoad[APP_GMA_CMD_MAXIMUM_PAYLOAD_SIZE - 9*sizeof(uint8_t)];
}__attribute__((packed))APP_GMA_STREAM_CMD_T;


void                        app_gma_get_cmd_response_handler(APP_GMA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);
void                        app_gma_control_data_xfer(bool isStartXfer);
void                        app_gma_start_data_xfer_control_rsp_handler(APP_GMA_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);
void                        app_gma_stop_data_xfer_control_rsp_handler(APP_GMA_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);
APP_GMA_CMD_RET_STATUS_E    app_gma_send_response_to_command(APP_GMA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);
APP_GMA_CMD_RET_STATUS_E    app_gma_send_command(APP_GMA_CMD_CODE_E cmdCode, uint8_t* ptrParam, uint32_t paramLen, bool isCmd);
void                        app_gma_cmd_handler_init(void);
APP_GMA_CMD_INSTANCE_T*         app_gma_cmd_handler_get_entry_pointer_from_cmd_code(APP_GMA_CMD_CODE_E cmdCode);
uint16_t                    app_gma_cmd_handler_get_entry_index_from_cmd_code(APP_GMA_CMD_CODE_E cmdCode);
uint32_t app_gma_rcv_cmd(void *param1, uint32_t param2);
uint32_t app_gma_parse_cmd(void *param1, uint32_t param2);


#ifdef __cplusplus
    }
#endif

#endif // #ifndef __APP_GMA_CMD_HANDLER_H__

