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
#ifndef __BT_DRV_2300_INTERNAL_H__
#define __BT_DRV_2300_INTERNAL_H__



/***************************************************************************
 *RF conig macro
 ****************************************************************************/
//#define __HW_AGC__

#ifdef __HW_AGC__
#define __HW_AGC_I2V_DISABLE_DC_CAL__
#endif

#define __FANG_HW_AGC_CFG__
#define __FANG_LNA_CFG__

#define __PWR_FLATNESS__


/***************************************************************************
 *BT clock gate disable
 ****************************************************************************/
#define __CLK_GATE_DISABLE__


/***************************************************************************
 *PCM config macro
 ****************************************************************************/
//#define APB_PCM
//#define SW_INSERT_MSBC_TX_OFFSET

#define __BLE_PRIO_FOR_SCO__

#ifdef __cplusplus
extern "C" {
#endif

void btdrv_rf_blerx_gain_adjust_default(void);
void btdrv_rf_blerx_gain_adjust_lowgain(void);
void btdrv_rf_rx_gain_adjust_req(uint32_t user, bool lowgain);


#ifdef __cplusplus
}
#endif

#endif
