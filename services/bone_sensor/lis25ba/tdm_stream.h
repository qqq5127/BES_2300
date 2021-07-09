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
#ifndef __TDM_STREAM_H__
#define __TDM_STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*tdm_stream_callback_t)(uint8_t *buf_ptr, uint32_t frame_len);

int tdm_stream_register(tdm_stream_callback_t callback);

int tdm_stream_open(void);
int tdm_stream_close(void);

int tdm_stream_start(void);
int tdm_stream_stop(void);

#ifdef __cplusplus
}
#endif

#endif
