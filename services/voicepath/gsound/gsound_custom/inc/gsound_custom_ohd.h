/**
 * @file gsound_custom_ohd.h
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


#ifndef __GSOUND_CUSTOM_OHD_H__
#define __GSOUND_CUSTOM_OHD_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/
#include "gsound_target_ohd.h"

/******************************macro defination*****************************/

/******************************type defination******************************/

/****************************function declearation**************************/
/**
 * @brief Initializor of OHD handler.
 * 
 * This function is used to initialize the OHD handlers by libgsound.
 * 
 * @param handlers      pointer of OHD handlers
 */
void gsound_custom_ohd_init(const GSoundOhdInterface *handlers);

/**
 * @brief Get OHD state handler.
 * 
 * This function is used to return the current OHD state to libgsound.
 * 
 * @param status        pointer of status variable from ligsound.
 */
void gsound_custom_ohd_get_state(GSoundOhdState *status);

/**
 * @brief Update OHD state to libgsound.
 * 
 * This function is used to update the OHD state to libgsound, should be called
 * everytime the OHD state has changed.
 * 
 * NOTE: This function is not supposed to be called within @see
 * GSoundTargetGetOhdState according to the libgsound requirement
 * 
 * @param state         OHD state to update.
 */
void gsound_custom_ohd_update_state(GSoundOhdState state);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __GSOUND_CUSTOM_OHD_H__ */