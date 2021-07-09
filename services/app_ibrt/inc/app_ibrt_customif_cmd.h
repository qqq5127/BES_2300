/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __APP_IBRT_IF_CUSTOM_CMD__
#define __APP_IBRT_IF_CUSTOM_CMD__

#include "app_ibrt_custom_cmd.h"

typedef enum
{
    APP_IBRT_CUSTOM_CMD_TEST1 = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x01,
    APP_IBRT_CUSTOM_CMD_TEST2 = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x02,

    APP_TWS_CMD_SHARE_FASTPAIR_INFO = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x03,

#ifdef BISTO_ENABLED
    APP_TWS_CMD_BISTO_DIP_SYNC      = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x04,
#endif
    //new customer cmd add here
#ifdef __APP_ZOWEE_UI__
    ZOWEE_CMD_TWS_EXANGE_INFO = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x05,
    ZOWEE_CMD_TWS_EXANGE_INFO_WITH_RSP = APP_IBRT_CMD_BASE|APP_IBRT_CUSTOM_CMD_PREFIX|0x06,
#endif
} app_ibrt_custom_cmd_code_e;

typedef struct
{
    uint8_t rsv;
    uint8_t buff[6];
} __attribute__((packed))ibrt_custom_cmd_test_t;

void app_ibrt_customif_cmd_test(ibrt_custom_cmd_test_t *cmd_test);

#ifdef __APP_ZOWEE_UI__
void zowee_ibrt_tws_exchange_info_send_data_without_rsp(uint8_t *p_buff, uint16_t length);
void zowee_ibrt_tws_cmd_exchange_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

void zowee_ibrt_tws_exchange_info_send_rsp(uint16_t rsp_seq, uint8_t *data, uint16_t len);
void zowee_ibrt_tws_exchange_info_send_with_rsp(uint8_t *p_buff, uint16_t length);
void zowee_ibrt_tws_exchange_info_with_rsp_receive_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void zowee_ibrt_tws_exchange_info_rsp_receive_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
void zowee_ibrt_tws_exchange_info_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

#endif
