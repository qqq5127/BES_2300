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
#include "gsound_custom_device_actions.h"

/*********************external function declearation************************/

/************************private macro defination***************************/

/************************private type defination****************************/

/**********************private function declearation************************/

/************************private variable defination************************/
const GSoundDeviceActionsInterface *deviceActionsInterface = NULL;

/****************************function defination****************************/

/**
 * @brief store the device actions related handlers pointer comes from gsound library
 * 
 * @param hanlders pointer of device actions interface
 */
void gsound_custom_device_actions_store_lib_interface(const GSoundDeviceActionsInterface *handlers)
{
    deviceActionsInterface = handlers;
}