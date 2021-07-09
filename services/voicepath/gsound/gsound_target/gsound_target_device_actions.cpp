/***************************************************************************
*
*Copyright 2015-2020 BES.
*All rights reserved. All unpublished rights reserved.
*
*No part of this work may be used or reproduced in any form or by any
*means, or stored in a database or retrieval system, without prior written
*permission of BES.
*
*Use of this work is governed by a license granted by BES.
*This work contains confidential and proprietary information of
*BES. which is protected by copyright, trade secret,
*trademark and other intellectual property rights.
*
****************************************************************************/


/*****************************header include********************************/
#include "gsound_dbg.h"
#include "gsound_target_device_actions.h"
#include "gsound_custom_device_actions.h"

/*********************external function declearation************************/

/************************private macro defination***************************/

/************************private type defination****************************/

/**********************private function declearation************************/

/************************private variable defination************************/

/****************************function defination****************************/
void GSoundTargetDeviceActionsStateUpdateConsumed(const uint8_t * const action_state_key)
{

}

GSoundStatus GSoundTargetDeviceActionsRequestState(void)
{
    return GSOUND_STATUS_OK;
}

GSoundStatus GSoundTargetDeviceActions(const uint8_t * const device_actions,
                                       uint32_t device_actions_len)
{
    return GSOUND_STATUS_OK;
}

void GSoundTargetDeviceActionsInit(const GSoundDeviceActionsInterface *handler)
{
    gsound_custom_device_actions_store_lib_interface(handler);
}
