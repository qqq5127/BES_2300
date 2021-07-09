/**
 * @file gsound_target_ohd.cpp
 * @author BES AI team
 * @version 0.1
 * @date 2020-05-06
 * 
 * @copyright Copyright (c) 2015-2020 BES Technic.
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
 */

/*****************************header include********************************/
#include "gsound_target_ohd.h"
#include "gsound_custom_ohd.h"

/*********************external function declearation************************/

/************************private macro defination***************************/

/************************private type defination****************************/

/**********************private function declearation************************/

/************************private variable defination************************/

/****************************function defination****************************/
GSoundStatus GSoundTargetGetOhdState(GSoundOhdState *ohd_status_out)
{
    gsound_custom_ohd_get_state(ohd_status_out);

    return GSOUND_STATUS_OK;
}

GSoundStatus GSoundTargetOhdInit(const GSoundOhdInterface *handlers)
{
    gsound_custom_ohd_init(handlers);

    return GSOUND_STATUS_OK;
}
