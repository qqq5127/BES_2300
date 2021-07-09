#ifndef __APP_GMA_EXTENDED_HANDLER_H__
#define __APP_GMA_EXTENDED_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "app_gma_cmd_code.h"
#include <stdlib.h>
#include "app_gma_state_env.h"

#define GMA_A2DP_VOLUME_MAX     0x64
#define GMA_A2DP_VOLUME_MIN     0x00
#define GMA_BT_HFP_NUMBER_MAX   21

typedef enum
{
    GMA_MEDIA_GET_STATE          = 0x00,
    GMA_MEDIA_CONTROL_VOLUME     = 0x01,
    GMA_MEDIA_CONTROL_START      = 0x02,
    GMA_MEDIA_CONTROL_PAUSE      = 0x03,
    GMA_MEDIA_CONTROL_PRE        = 0x04,
    GMA_MEDIA_CONTROL_NEXT       = 0x05,
    GMA_MEDIA_CONTROL_STOP       = 0x06,
} GMA_MEDIA_CMD_TYPE_E;


typedef enum
{
    GMA_HFP_GET_STATE            = 0x00,
    GMA_HFP_DIAL                 = 0x01,
    GMA_HFP_PICK_UP              = 0x02,
    GMA_HFP_HANG_UP              = 0x03,
    GMA_HFP_REDIAL               = 0x04,
    GMA_HFP_HANGUP_AND_ANSWER    = 0x05,
    GMA_HFP_HOLD_REL_INCOMING    = 0x06,
    GMA_HFP_HOLD_AND_ANSWER      = 0x07,
    GMA_HFP_HOLD_ADD_HELD_CALL   = 0x08,
} GMA_HFP_CMD_TYPE_E;

typedef enum
{
    GMA_EXT_POWER_LEL            = 0x00,
    GMA_EXT_BATTERY_STAT         = 0x01,
    GMA_EXT_FM_FREQ_SET          = 0x02,
    GMA_EXT_FM_FREQ_GET          = 0x03,
    GMA_EXT_VERSION_GET          = 0x04,
    GMA_EXT_BT_NAME              = 0x05,
    GMA_EXT_MIC_STAT             = 0x06,
} GMA_EXTEND_CMD_TYPE_E;

typedef enum
{
    GMA_EXT_POW_RSP_LOW           = 0x00,
    GMA_EXT_POW_RSP_NORM          = 0x01,
} GMA_EXTEND_POW_RSP_STATE_E;

typedef struct
{
    uint8_t     power_percent;
    GMA_EXTEND_POW_RSP_STATE_E     power_state;
} GMA_EXT_POW_RSP_T;

typedef enum
{
    GMA_EXT_BATT_RSP_CHARGE           = 0x00,
    GMA_EXT_BATT_RSP_USE_BAT          = 0x01,
    GMA_EXT_BATT_RSP_USE_AC           = 0x02,
} GMA_EXTEND_BATTERY_RSP_STATE_E;

typedef struct
{
    uint8_t     result;
} GMA_EXT_RESULT_RSP_T;

#ifdef __cplusplus
    }
#endif


#endif // #ifndef __APP_GMA_EXTENDED_HANDLER_H__

