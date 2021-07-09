/*
 * vbus_protocol.h
 *
 * Copyright (C) 2020 ZOWEE Tochnology Co.,Ltd.
 *
 * 充电盒通讯协议定义
 *
 * Written by Stone Lee<litb@zowee.com.cn>
 */
#ifndef ZW_VBUS_PROTOCOL_H
#define ZW_VBUS_PROTOCOL_H

#ifdef __APP_VBUS_ENABLE__

#include "hal_trace.h"
/* TODO */

#define APP_VBUS_FRAME_DATA_SIZE_MAX    (64)

typedef enum
{
    VBUS_CMD_NONE = 0,
    VBUS_CMD_TWS_PAIRING,
    VBUS_CMD_PAIRING_STATUS,
    VBUS_CMD_EARPHONE_OTA,
    VBUS_CMD_EARPHONE_RESET,
    VBUS_CMD_EARPHONE_SHUTDOWN,
    VBUS_CMD_EARPHONE_BATTERY,
    VBUS_CMD_EARPHONE_HARDWARE_RESET,
    VBUS_CMD_PAIRING_STOP = 0x08,
    VBUS_CMD_FREEMAN_PAIRING,
    VBUS_CMD_START_VBUS_UPDATA = 0x10,
    VBUS_CMD_CLOSE_BOX = 0x0a,
    VBUS_CMD_OPEN_BOX = 0x0b,
    
    VBUS_CMD_DUT = 0x62,
    VBUS_CMD_EARSIDE = 0x77,
    VBUS_CMD_TWS_PAIR_TEST = 0x78,
    VBUS_CMD_PARI_STATUS_TEST = 0x79,
    VBUS_CMD_CNT
} app_vbus_cmd_enum;


typedef enum
{
    EARPHONE_LEFT  = 0xA3,		// left earphone
    EARPHONE_RIGHT = 0xA2,		// right earphone
    EARPHONE_UNKNOWN = 0xFF,	// earphone status unknown
} vbus_earphone_type_enum;


typedef enum
{
    TGT_EP = 1,		//earphone
    TGT_CB,			//charger box
    TGT_ATE,		//ATE
    TGT_L_EP,		//left earphone
    TGT_R_EP,		//right earphone
} vbus_tgt_enum;

typedef struct
{
    app_vbus_cmd_enum frame_cmd;
    void (*FuncProPtr)(void);
} app_vbus_cmd_frame_struct_t;

typedef struct
{
    uint8_t head;
    uint8_t cmd;
    uint8_t length;
    uint8_t data[APP_VBUS_FRAME_DATA_SIZE_MAX];
} app_vbus_cmd_struct_t;

#endif /* __APP_VBUS_ENABLE__ */

#endif /* ZW_VBUS_PROTOCOL_H */


