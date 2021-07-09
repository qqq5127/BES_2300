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
#ifndef __BT_DRV_1400_INTERNAL_H__
#define __BT_DRV_1400_INTERNAL_H__

/***************************************************************************
 *RF conig macro
 ****************************************************************************/
#define __HYBIRD_AGC__
#define __HW_AGC__
//#define __SWAGC_2_LEVEL__

//#define __1402_3_AGC__
//#define __1402_3_AGC_for_test_mode

#define BT_RF_OLD_CORR_MODE

/***************************************************************************
 *BT clock gate disable
 ****************************************************************************/
#define __CLK_GATE_DISABLE__

/***************************************************************************
 *PCM config macro
 ****************************************************************************/
//#define APB_PCM
//#define SW_INSERT_MSBC_TX_OFFSET

/***************************************************************************
 *BT read controller rssi
 ****************************************************************************/
#define  BT_RSSI_MONITOR  1

/***************************************************************************
 *BT afh follow function
 ****************************************************************************/
#define  BT_AFH_FOLLOW  0

#ifdef __cplusplus
extern "C" {
#endif

bool btdrv_rf_rx_gain_adjust_getstatus(void);
void btdrv_rf_rx_gain_adjust_req(uint32_t user, bool lowgain);
void btdrv_rf_txcal(void);

void bt_drv_reg_op_for_test_mode_disable(void);
void btdrv_override_regs(void);
void bt_drv_reg_op_disable_swagc_nosync_count(void);

#ifdef __cplusplus
}
#endif

#endif
