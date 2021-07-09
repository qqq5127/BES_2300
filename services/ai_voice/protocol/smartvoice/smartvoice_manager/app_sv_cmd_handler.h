#ifndef __APP_SV_CMD_HANDLER_H__
#define __APP_SV_CMD_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_timer.h"
#include "app_sv_cmd_code.h"

#define GET_CURRENT_TICKS()         hal_sys_timer_get()
#define GET_CURRENT_MS()            TICKS_TO_MS(GET_CURRENT_TICKS())


/**< maximum payload size of one smart voice command */
#define MAXIMUM_DATA_PACKET_LEN     672
#define APP_SV_CMD_MAXIMUM_PAYLOAD_SIZE MAXIMUM_DATA_PACKET_LEN

/**
 * @brief BLE custom command playload
 *
 */
typedef struct
{
    uint16_t    cmdCode;        /**< command code, from APP_SV_CMD_CODE_E */
    uint16_t    paramLen;       /**< length of the following parameter */
    uint8_t     param[APP_SV_CMD_MAXIMUM_PAYLOAD_SIZE - 2*sizeof(uint16_t)];
} APP_SV_CMD_PAYLOAD_T;

/**
 * @brief Command response parameter structure
 *
 */
typedef struct
{
    uint16_t    cmdCodeToRsp;   /**< tell which command code to response */
    uint16_t    cmdRetStatus;   /**< handling result of the command, from APP_SV_CMD_RET_STATUS_E */
    uint16_t    rspDataLen;     /**< length of the response data */
    uint8_t     rspData[APP_SV_CMD_MAXIMUM_PAYLOAD_SIZE - 5*sizeof(uint16_t)];
} APP_SV_CMD_RSP_T;


void                        app_sv_get_cmd_response_handler(APP_SV_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);
void                        app_sv_data_xfer_control_handler(APP_SV_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);
void                        app_sv_control_data_xfer(bool isStartXfer);
void                        app_sv_start_data_xfer_control_rsp_handler(APP_SV_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);
void                        app_sv_stop_data_xfer_control_rsp_handler(APP_SV_CMD_RET_STATUS_E retStatus, uint8_t* ptrParam, uint32_t paramLen);
APP_SV_CMD_RET_STATUS_E     app_sv_send_response_to_command(APP_SV_CMD_CODE_E responsedCmdCode, APP_SV_CMD_RET_STATUS_E returnStatus, 
                                uint8_t* rspData, uint32_t rspDataLen);
APP_SV_CMD_RET_STATUS_E     app_sv_send_command(APP_SV_CMD_CODE_E cmdCode, uint8_t* ptrParam, uint32_t paramLen);
void                        app_sv_cmd_handler_init(void);
APP_SV_CMD_INSTANCE_T*      app_sv_cmd_handler_get_entry_pointer_from_cmd_code(APP_SV_CMD_CODE_E cmdCode);
uint16_t                    app_sv_cmd_handler_get_entry_index_from_cmd_code(APP_SV_CMD_CODE_E cmdCode);
uint32_t app_sv_rcv_cmd(void *param1, uint32_t param2);
uint32_t app_sv_parse_cmd(void *param1, uint32_t param2);


#ifdef __cplusplus
    }
#endif

#endif // #ifndef __APP_SV_CMD_HANDLER_H__

