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
#ifndef __APP_ANC_H__
#define __APP_ANC_H__

#include "hal_aud.h"
#include "app_key.h"
#include "ui_event_manage.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t  anc_status;
    uint8_t  anc_type;
    uint16_t anc_param;
} __attribute__((packed)) anc_sync_status_t;
void app_anc_set_coef(uint8_t index);
uint8_t app_anc_get_coef(void);
void app_anc_set_playback_samplerate(enum AUD_SAMPRATE_T sample_rate);
void app_anc_init(enum AUD_IO_PATH_T input_path, enum AUD_SAMPRATE_T playback_rate, enum AUD_SAMPRATE_T capture_rate);
void app_anc_close(void);
void app_anc_enable(void);
void app_anc_disable(void);
bool anc_enabled(void);
void test_anc(void);
void app_anc_resample(uint32_t res_ratio, uint32_t *in, uint32_t *out, uint32_t samples);
void app_anc_key(APP_KEY_STATUS *status, void *param);
int app_anc_open_module(void);
int app_anc_close_module(void);
enum AUD_SAMPRATE_T app_anc_get_play_rate(void);
bool app_anc_work_status(void);
void app_anc_send_howl_evt(uint32_t howl);
uint32_t app_anc_get_anc_status(void);
bool app_pwr_key_monitor_get_val(void);
bool app_anc_switch_get_val(void);
void app_anc_ios_init(void);
void app_anc_set_init_done(void);
bool app_anc_set_reboot(void);
void app_anc_status_post(uint8_t status);
#ifdef __APP_ZOWEE_UI__
typedef	enum{
	ANC_SWITCH_LOOP,
	ANC_SWITCH_DIRECT,
}ANC_SWITCH_MODE_E;

void zowee_anc_status_init();
void zowee_anc_status_change_msg(ANC_SWITCH_MODE_E anc_switch_mode,ZOWEE_ANC_STATUS_E anc_status);
int32_t zowee_anc_status_change_handle(void);
void zowee_anc_status_change(ZOWEE_ANC_STATUS_E anc_status,ANC_SWITCH_MODE_E mode);
void zowee_anc_wear_status_change_msg(bool onoff);

#ifndef	__APP_WEAR_ENABLE__
void zowee_anc_status_change_when_tws_status_change(uint8_t tws_status);
#endif
void zowee_anc_status_change_when_wear_status_change(uint8_t wear_status);
void zowee_anc_status_stop_close_anc_timer(void);
void zowee_anc_close_when_put_in_box(void);
#endif

typedef	struct{
	uint8_t  anc_reset_flag;
}ANC_RESET_STATUS_T;


#if defined(IBRT)
void app_anc_cmd_receive_process(uint8_t *buf, uint16_t len);
#endif
#ifdef __cplusplus
}
#endif

#endif
