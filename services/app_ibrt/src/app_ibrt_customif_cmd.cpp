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
#include "string.h"
#include "app_tws_ibrt_trace.h"
#include "app_tws_ctrl_thread.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_ibrt_customif_cmd.h"
#include "app_tws_if.h"
#include "app_dip.h"
#ifdef BISTO_ENABLED
#include "gsound_custom.h"
#endif

#if defined(IBRT)
/*
* custom cmd handler add here, this is just a example
*/

#define app_ibrt_custom_cmd_rsp_timeout_handler_null   (0)
#define app_ibrt_custom_cmd_rsp_handler_null           (0)
#define app_ibrt_custom_cmd_rx_handler_null            (0)

#ifdef BISTO_ENABLED
static void app_ibrt_bisto_dip_sync(uint8_t *p_buff, uint16_t length);
static void app_ibrt_bisto_dip_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
#endif

static void app_ibrt_customif_test1_cmd_send(uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test1_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

static void app_ibrt_customif_test2_cmd_send(uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test2_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test2_cmd_send_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
static void app_ibrt_customif_test2_cmd_send_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

static const app_tws_cmd_instance_t g_ibrt_custom_cmd_handler_table[]=
{
#ifdef GFPS_ENABLED
    {
        APP_TWS_CMD_SHARE_FASTPAIR_INFO,                "SHARE_FASTPAIR_INFO",
        app_ibrt_share_fastpair_info,
        app_ibrt_shared_fastpair_info_received_handler,  0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,          app_ibrt_custom_cmd_rsp_handler_null
    },
#endif

#ifdef BISTO_ENABLED
    {
        APP_TWS_CMD_BISTO_DIP_SYNC,                     "BISTO_DIP_SYNC",
        app_ibrt_bisto_dip_sync,
        app_ibrt_bisto_dip_sync_handler,                0,
        app_ibrt_cmd_rsp_timeout_handler_null,          app_ibrt_cmd_rsp_handler_null
    },
#endif

    {
        APP_IBRT_CUSTOM_CMD_TEST1,                              "TWS_CMD_TEST1",
        app_ibrt_customif_test1_cmd_send,
        app_ibrt_customif_test1_cmd_send_handler,               0,
        app_ibrt_custom_cmd_rsp_timeout_handler_null,           app_ibrt_custom_cmd_rsp_handler_null
    },
    {
        APP_IBRT_CUSTOM_CMD_TEST2,                              "TWS_CMD_TEST2",
        app_ibrt_customif_test2_cmd_send,
        app_ibrt_customif_test2_cmd_send_handler,               RSP_TIMEOUT_DEFAULT,
        app_ibrt_customif_test2_cmd_send_rsp_timeout_handler,   app_ibrt_customif_test2_cmd_send_rsp_handler
    },
#ifdef __APP_ZOWEE_UI__    
    {
        ZOWEE_CMD_TWS_EXANGE_INFO,             					"TWS_CMD_EXANGE_SIMPLE_INFO",
        zowee_ibrt_tws_exchange_info_send_data_without_rsp,
        zowee_ibrt_tws_cmd_exchange_info_handler,          		0,
        NULL,                                                   NULL
    },

	{
        ZOWEE_CMD_TWS_EXANGE_INFO_WITH_RSP,                     "TWS_CMD_EXANGE_SIMPLE_INFO_WITH_RSP",
        zowee_ibrt_tws_exchange_info_send_with_rsp,
        zowee_ibrt_tws_exchange_info_with_rsp_receive_handler,  8000,
        zowee_ibrt_tws_exchange_info_rsp_timeout_handler,       zowee_ibrt_tws_exchange_info_rsp_receive_handler
    },
#endif
};

int app_ibrt_customif_cmd_table_get(void **cmd_tbl, uint16_t *cmd_size)
{
    *cmd_tbl = (void *)&g_ibrt_custom_cmd_handler_table;
    *cmd_size = ARRAY_SIZE(g_ibrt_custom_cmd_handler_table);
    return 0;
}


#ifdef BISTO_ENABLED
static void app_ibrt_bisto_dip_sync(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_TWS_CMD_BISTO_DIP_SYNC, p_buff, length);
}

static void app_ibrt_bisto_dip_sync_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    gsound_mobile_type_get_callback(*(MOBILE_CONN_TYPE_E *)p_buff);
}
#endif

void app_ibrt_customif_cmd_test(ibrt_custom_cmd_test_t *cmd_test)
{
    tws_ctrl_send_cmd(APP_IBRT_CUSTOM_CMD_TEST1, (uint8_t*)cmd_test, sizeof(ibrt_custom_cmd_test_t));
    tws_ctrl_send_cmd(APP_IBRT_CUSTOM_CMD_TEST2, (uint8_t*)cmd_test, sizeof(ibrt_custom_cmd_test_t));
}

static void app_ibrt_customif_test1_cmd_send(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(APP_IBRT_CUSTOM_CMD_TEST1, p_buff, length);
    TRACE(1,"%s", __func__);
}

static void app_ibrt_customif_test1_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
}

static void app_ibrt_customif_test2_cmd_send(uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
}

static void app_ibrt_customif_test2_cmd_send_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    tws_ctrl_send_rsp(APP_IBRT_CUSTOM_CMD_TEST2, rsp_seq, NULL, 0);
    TRACE(1,"%s", __func__);

}

static void app_ibrt_customif_test2_cmd_send_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);

}

static void app_ibrt_customif_test2_cmd_send_rsp_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"%s", __func__);
}

#ifdef __APP_ZOWEE_UI__
extern "C" void zowee_tws_exchange_info_handler(uint8_t *ptrParam, uint32_t paramLen);
extern void zowee_tws_exchange_info_with_rsp_handler(uint16_t rsp_seq, uint8_t *data, uint16_t len, uint8_t need_rsp);

/***send cmd without rsp***/
void zowee_ibrt_tws_exchange_info_send_data_without_rsp(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_without_rsp(ZOWEE_CMD_TWS_EXANGE_INFO, p_buff, length);
}

void zowee_ibrt_tws_cmd_exchange_info_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    if (NULL != p_buff && length != 0) 
	{
        zowee_tws_exchange_info_handler(p_buff,length);
    }
}

/***send cmd with rsp***/
void zowee_ibrt_tws_exchange_info_send_rsp(uint16_t rsp_seq, uint8_t *data, uint16_t len)
{
    tws_ctrl_send_rsp(ZOWEE_CMD_TWS_EXANGE_INFO_WITH_RSP, rsp_seq, (uint8_t*)data, len);
}

void zowee_ibrt_tws_exchange_info_send_with_rsp(uint8_t *p_buff, uint16_t length)
{
    app_ibrt_send_cmd_with_rsp(ZOWEE_CMD_TWS_EXANGE_INFO_WITH_RSP, p_buff, length);
}

void zowee_ibrt_tws_exchange_info_with_rsp_receive_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    if (NULL != p_buff && length != 0) {
        zowee_tws_exchange_info_with_rsp_handler(rsp_seq, p_buff, length, true);
    }
}

void zowee_ibrt_tws_exchange_info_rsp_receive_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    if (NULL != p_buff && length != 0) {
        zowee_tws_exchange_info_with_rsp_handler(rsp_seq, p_buff, length, false);
    }
}

void zowee_ibrt_tws_exchange_info_rsp_timeout_handler(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
    TRACE(1,"ibrt_ui_log:%s",__func__);
}


#endif//__APP_ZOWEE_UI__

#endif
