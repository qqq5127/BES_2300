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
#ifndef __APP_IBRT_PERIPHERAL_MANAGER_H__
#define __APP_IBRT_PERIPHERAL_MANAGER_H__

/*************************
*   automate_cmd format
*   item        len(byte)
*   group_code  1
*   opera_code  1
*   param_len   1
*   *param      param_len(0-255)
*   test_times  1
*************************/
#define GROUP_CODE_OFFSET                   (0)
#define OPERATION_CODE_OFFSET               (1)
#define PARAM_LEN_OFFSET                    (2)
#define PARAM_CODE_OFFSET                   (3)
#define AUTOMATE_TEST_CMD_TEST_TIMES_LEN    (1)
#define AUTOMATE_TEST_CMD_CRC_RECORD_LEN    (2)
#define AUTOMATE_TEST_CMD_PARAM_MAX_LEN     (255)
#define AUTOMATE_TEST_CMD_MIN_LEN           (PARAM_CODE_OFFSET+AUTOMATE_TEST_CMD_TEST_TIMES_LEN+AUTOMATE_TEST_CMD_CRC_RECORD_LEN)
#define AUTOMATE_TEST_CMD_MAX_LEN           (AUTOMATE_TEST_CMD_MIN_LEN+AUTOMATE_TEST_CMD_PARAM_MAX_LEN)

typedef struct {
    uint32_t message_id;
    uint32_t message_ptr;
    uint32_t message_Param0;
    uint32_t message_Param1;
    uint32_t message_Param2;
} TWS_PD_MSG_BODY;

typedef struct {
    TWS_PD_MSG_BODY msg_body;
} TWS_PD_MSG_BLOCK;

int app_ibrt_peripheral_mailbox_put(TWS_PD_MSG_BLOCK* msg_src);
int app_ibrt_peripheral_mailbox_free(TWS_PD_MSG_BLOCK* msg_p);
int app_ibrt_peripheral_mailbox_get(TWS_PD_MSG_BLOCK** msg_p);
void app_ibrt_peripheral_thread_init(void);
void app_ibrt_peripheral_run0(uint32_t ptr);
void app_ibrt_peripheral_run1(uint32_t ptr, uint32_t param0);
void app_ibrt_peripheral_run2(uint32_t ptr, uint32_t param0, uint32_t param1);

#endif
