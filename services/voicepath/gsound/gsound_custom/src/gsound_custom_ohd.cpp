/**
 * @file gsound_custom_ohd.cpp
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
#include "gsound_custom_ohd.h"

/*********************external function declearation************************/

/************************private macro defination***************************/

/************************private type defination****************************/

/**********************private function declearation************************/

/************************private variable defination************************/
static const GSoundOhdInterface *ohdInterface = NULL;

/****************************function defination****************************/
void gsound_custom_ohd_init(const GSoundOhdInterface *handlers)
{
    ohdInterface = handlers;
}

void gsound_custom_ohd_get_state(GSoundOhdState *status)
{
    // TODO: should implement the ohd state management later
    *status = OHD_UNSUPPORTED;
}

void gsound_custom_ohd_update_state(GSoundOhdState state)
{
    if (ohdInterface != NULL)
    {
        ohdInterface->gsound_ohd_status(state);
    }
}
