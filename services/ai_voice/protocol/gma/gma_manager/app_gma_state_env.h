#ifndef __APP_GMA_STATE_ENV_H
#define __APP_GMA_STATE_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_gma_cmd_code.h"

#define GMA_CMD_CODE_SIZE       (4*sizeof(uint8_t))
#define GMA_DATA_HEADER_SIZE        (5*sizeof(uint8_t))
#define GMA_TLV_HEADER_SIZE     (2*sizeof(uint8_t))

typedef struct
{
    uint8_t     msg_id;
    uint8_t     data_encry_flag;
    uint8_t     version_num;
    uint8_t     sequence;
    uint8_t     device_state;
} APP_GMA_STATE_ENV_T;

typedef enum
{
    GMA_PHO_STA_CON_READY = 0x00,
    GMA_PHO_STA_LISTENING = 0x01,
    GMA_PHO_STA_THINKING = 0x02,
    GMA_PHO_STA_SPEAKING = 0x03,
    GMA_PHO_STA_IDLE = 0x04,
    GMA_PHO_STA_START_RECORD = 0x05,
    GMA_PHO_STA_STOP_RECORD = 0x06,
} APP_GMA_PHONE_STATUS_E;

typedef enum
{
    GMA_DEV_STA_REC_START = 0x00,
    GMA_DEV_STA_REC_STOP = 0x01,
} APP_GMA_DEV_STATUS_E;

typedef enum
{
    GMA_DEV_STA_CHANGE_SUCS = 0x00,
    GMA_DEV_STA_CHANGE_FAIL = 0x01,
} APP_GMA_DEV_STATUS_CHANGE_RSP_E;

typedef enum
{
    GMA_HFP_OPERATION_SUCS = 0x00,
    GMA_HFP_OPERATION_FAIL = 0x01,
} APP_GMA_HFP_OPERATION_RSP_E;

#define GMA_GET_HEAD_MSG_ID(x)  ((uint8_t)x & 0x0f)
#define GMA_GET_HEAD_ENCRY_FLAG(x)      ((uint8_t)(x>>4) & 0x01)
#define GMA_GET_HEAD_VERSION_NUM(x)      ((uint8_t)(x>>5) & 0x07)

void app_gma_state_env_reset(void);
uint8_t app_gma_state_env_msg_id_receive(uint8_t msg_id);
uint8_t app_gma_state_env_msg_id_get(void);
uint8_t app_gma_state_env_msg_id_incre(void);
uint8_t app_gma_state_env_encry_flag_receive(uint8_t flag);
uint8_t app_gma_state_env_encry_flag_get(void);
uint8_t app_gma_state_env_version_num_receive(uint8_t flag);
uint8_t app_gma_state_Version_num_get(void);

uint8_t app_gma_state_env_sequence_get(void);
uint8_t app_gma_state_env_sequence_receive(uint8_t seq);

#ifdef __cplusplus
    }
#endif
#endif