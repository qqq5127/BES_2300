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
#include "plat_types.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"


bool btdrv_get_accept_new_mobile_enable(void)
{
    bool ret = false;
    return ret;
}

bool btdrv_get_page_pscan_coex_enable(void)
{
    bool ret = false;
    return ret;
}
void btdrv_switch_agc_mode(enum BT_WORK_MODE_T mode)
{
}

void btdrv_rf_bit_offset_track_enable(bool enable)
{
    return;
}

void bt_drv_adaptive_fa_rx_gain(int8_t rssi)
{
}

void bt_drv_digital_config_for_ble_adv(bool en)
{
    (void)en;
}

