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
#ifndef AUDIO_FLINGER_H
#define AUDIO_FLINGER_H

#include "plat_types.h"
#include "stdbool.h"
#include "hal_aud.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*AF_STREAM_HANDLER_T)(uint8_t *buf, uint32_t len);
typedef void (*AF_IRQ_NOTIFICATION_T)(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
typedef void (*AF_CODEC_PLAYBACK_POST_HANDLER_T)(uint8_t *buf, uint32_t len, void *config);

//pingpong machine
enum AF_PP_T{
    PP_PING = 0,
    PP_PANG = 1
};

typedef enum  {
    AF_USER_AI,
    AF_USER_THIRDPART,
    AF_USER_AUDIO,
    AF_USER_SBC,
    AF_USER_SCO,
    AF_USER_TEST,
    AF_USER_NUM
} AF_USER_E;

struct AF_STREAM_CONFIG_T {
    enum AUD_BITS_T bits;
    enum AUD_SAMPRATE_T sample_rate;
    enum AUD_CHANNEL_NUM_T channel_num;
    enum AUD_CHANNEL_MAP_T channel_map;
    enum AUD_STREAM_USE_DEVICE_T device;
    enum AUD_IO_PATH_T io_path;
    bool chan_sep_buf;
    bool sync_start;
    uint8_t slot_cycles;
    uint16_t fs_cycles;
    AF_STREAM_HANDLER_T handler;

    uint8_t *data_ptr;
    uint32_t data_size;

    //should define type
    uint8_t vol;
};

//Should define return status
uint32_t af_open(void);
void *af_thread_tid_get(void);
uint32_t af_stream_open(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, const struct AF_STREAM_CONFIG_T *cfg);
uint32_t af_stream_setup(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, const struct AF_STREAM_CONFIG_T *cfg);
uint32_t af_stream_mute(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, bool mute);
uint32_t af_stream_set_chan_vol(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol);
uint32_t af_stream_restore_chan_vol(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_stream_get_cfg(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream, struct AF_STREAM_CONFIG_T **cfg, bool needlock);
uint32_t af_stream_start(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_stream_stop(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_stream_pause(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_stream_restart(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_stream_close(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_close(void);
uint32_t af_stream_get_cur_dma_addr(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
int af_stream_get_cur_dma_pos(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
int af_stream_buffer_error(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_stream_dma_tc_irq_enable(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
uint32_t af_stream_dma_tc_irq_disable(enum AUD_STREAM_ID_T id, enum AUD_STREAM_T stream);
void af_set_irq_notification(AF_IRQ_NOTIFICATION_T notif);

void af_lock_thread(void);
void af_unlock_thread(void);
void af_set_priority(AF_USER_E user, int priority);
int af_get_priority(void);
int af_get_default_priority(void);
void af_reset_priority(void);

void af_codec_tune_resample_rate(enum AUD_STREAM_T stream, float ratio);
void af_codec_direct_tune_resample_rate(enum AUD_STREAM_T stream, float ratio);
void af_codec_tune_both_resample_rate(float ratio);
void af_codec_tune_pll(float ratio);
void af_codec_tune_xtal(float ratio);
void af_codec_tune(enum AUD_STREAM_T stream, float ratio);
void af_codec_direct_tune(enum AUD_STREAM_T stream, float ratio);

void af_codec_set_perf_test_power(int type);
void af_codec_set_noise_reduction(bool enable);
void af_codec_swap_output(bool swap);
void af_codec_set_playback_post_handler(AF_CODEC_PLAYBACK_POST_HANDLER_T hdlr);

enum AF_CODEC_SYNC_TYPE_T {
    AF_CODEC_SYNC_TYPE_NONE,
    AF_CODEC_SYNC_TYPE_GPIO,
    AF_CODEC_SYNC_TYPE_BT,
    AF_CODEC_SYNC_TYPE_WIFI,
};

enum AF_I2S_SYNC_TYPE_T {
    AF_I2S_SYNC_TYPE_NONE,
    AF_I2S_SYNC_TYPE_BT,
    AF_I2S_SYNC_TYPE_GPIO,
};


void af_codec_sync_config(enum AUD_STREAM_T stream, enum AF_CODEC_SYNC_TYPE_T type, bool enable);
void af_codec_sync_resample_rate_config(enum AUD_STREAM_T stream, enum AF_CODEC_SYNC_TYPE_T type, bool enable);
void af_codec_sync_gain_config(enum AUD_STREAM_T stream, enum AF_CODEC_SYNC_TYPE_T type, bool enable);
void af_i2s_sync_config(enum AUD_STREAM_T stream, enum AF_I2S_SYNC_TYPE_T type, bool enable);

typedef void (*AF_ANC_HANDLER)(enum AUD_STREAM_T stream, enum AUD_SAMPRATE_T rate, enum AUD_SAMPRATE_T *new_play, enum AUD_SAMPRATE_T *new_cap);

int af_anc_open(enum ANC_TYPE_T type, enum AUD_SAMPRATE_T play_rate, enum AUD_SAMPRATE_T capture_rate, AF_ANC_HANDLER handler);
int af_anc_close(enum ANC_TYPE_T type);

int af_vad_open(const struct AUD_VAD_CONFIG_T *cfg);
int af_vad_close(void);
int af_vad_start(void);
int af_vad_stop(void);
uint32_t af_vad_get_data(uint8_t *buf, uint32_t len);

void af_dsd_enable(void);
void af_dsd_disable(void);

typedef void (*AF_CODEC_BT_TRIGGER_CALLBACK)(void);
void af_codec_bt_trigger_config(bool en, AF_CODEC_BT_TRIGGER_CALLBACK callback);

#ifdef __RAND_FROM_MIC__
uint8_t random_mic_is_on(uint8_t *deviceId);
#endif

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_FLINGER_H */
