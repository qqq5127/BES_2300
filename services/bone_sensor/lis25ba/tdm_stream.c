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
#include "plat_types.h"
#include "hal_trace.h"
#include "tdm_stream.h"
#include "audio_dump.h"
#include "audioflinger.h"
#include "string.h"

// #define TDM_STREAM_AUDIO_DUMP

// 0: X
// 1: Y
// 2: Z
#define SELECT_CHANNEL      (1)

#define I2S_STREAM_ID       AUD_STREAM_ID_2

#define FRAME_LEN           (240)
#define CHANNEL_NUM         (2)
#define TEMP_BUF_SIZE       (FRAME_LEN * CHANNEL_NUM * 2 * 2)

// static uint32_t codec_capture_buf_len = TEMP_BUF_SIZE;
// static uint32_t codec_playback_buf_len = TEMP_BUF_SIZE;
static uint32_t i2s_capture_buf_len = TEMP_BUF_SIZE;
static uint32_t i2s_playback_buf_len = TEMP_BUF_SIZE;

// static uint8_t codec_capture_buf[TEMP_BUF_SIZE];
// static uint8_t codec_playback_buf[TEMP_BUF_SIZE];
static uint8_t i2s_capture_buf[TEMP_BUF_SIZE];
static uint8_t i2s_playback_buf[TEMP_BUF_SIZE];

// static short i2s_capture_tmp_buf[FRAME_LEN * CHANNEL_NUM];
static short i2s_capture_tmp_buf[3][FRAME_LEN];     // Just X, Y, Z

#if defined(AF_ADC_I2S_SYNC)
// static uint32_t POSSIBLY_UNUSED codec_capture_cnt = 0;
// static uint32_t POSSIBLY_UNUSED codec_playback_cnt = 0;
static uint32_t POSSIBLY_UNUSED i2s_capture_cnt = 0;
static uint32_t POSSIBLY_UNUSED i2s_playback_cnt = 0;
#endif

static tdm_stream_callback_t tdm_stream_callback = NULL;

static uint32_t i2s_capture_callback(uint8_t *buf, uint32_t len)
{
    short POSSIBLY_UNUSED *pcm_buf = (short*)buf;
    int pcm_len = len / 2;

    // TRACE(2,"[%s] i2s frame length = %d", __func__, pcm_len / 4);

#if defined(AF_ADC_I2S_SYNC)
    //TRACE(2,"[%s] cnt = %d", __func__, i2s_capture_cnt++);
#endif

    // memcpy(i2s_capture_tmp_buf, buf, len);

#if 0
    int POSSIBLY_UNUSED sample_idx = 0;
    int POSSIBLY_UNUSED playback_idx = 0;
    #define X_OFFSET    (6)
    #define Y_OFFSET    (4)
    #define Z_OFFSET    (2)

    for (int i = 0; i < len; i += 8) {
        i2s_capture_tmp_buf[0][sample_idx] = buf[i+X_OFFSET] + (buf[i+X_OFFSET+1] << 8);
        i2s_capture_tmp_buf[1][sample_idx] = buf[i+Y_OFFSET] + (buf[i+Y_OFFSET+1] << 8);
        i2s_capture_tmp_buf[2][sample_idx] = buf[i+Z_OFFSET] + (buf[i+Z_OFFSET+1] << 8);
        sample_idx++;
        // i2s_capture_tmp_buf[sample_idx++] = codec_playback_tmp_buf[playback_idx++];
    }
#else
#define X_OFFSET (2)
#define Y_OFFSET (0)

    int POSSIBLY_UNUSED sample_idx = 0;

    for (int i = 0; i < len; i += 4)
    {
        i2s_capture_tmp_buf[0][sample_idx] = buf[i + X_OFFSET] + (buf[i + X_OFFSET + 1] << 8);
        i2s_capture_tmp_buf[1][sample_idx] = buf[i + Y_OFFSET] + (buf[i + Y_OFFSET + 1] << 8);
        sample_idx++;
    }
#endif

#if defined(TDM_STREAM_AUDIO_DUMP)
    audio_dump_clear_up();
    audio_dump_add_channel_data(0, i2s_capture_tmp_buf[0], FRAME_LEN);
    audio_dump_add_channel_data(1, i2s_capture_tmp_buf[1], FRAME_LEN);
    audio_dump_add_channel_data(2, i2s_capture_tmp_buf[2], FRAME_LEN);
    audio_dump_run();
#endif

    if (tdm_stream_callback)
    {
        tdm_stream_callback((uint8_t *)i2s_capture_tmp_buf[SELECT_CHANNEL], FRAME_LEN);
    }

    return pcm_len * 2;
}

static uint32_t i2s_playback_callback(uint8_t *buf, uint32_t len)
{
    short POSSIBLY_UNUSED *pcm_buf = (short*)buf;
    int POSSIBLY_UNUSED pcm_len = len / 2;

#if defined(AF_ADC_I2S_SYNC)
    //TRACE(2,"[%s] cnt = %d", __func__, i2s_playback_cnt++);
#endif

    // memcpy(buf, codec_queue_buf, len);


    return pcm_len * 2;
}

int tdm_stream_register(tdm_stream_callback_t callback)
{
    tdm_stream_callback = callback;

    return 0;
}

int tdm_stream_open(void)
{
    TRACE(1,"[%s] Run...", __func__);
    int ret = 0;
    struct AF_STREAM_CONFIG_T stream_cfg;

#if defined(AF_ADC_I2S_SYNC)
    i2s_capture_cnt = 0;
    i2s_playback_cnt = 0;
#endif

#if defined(TDM_STREAM_AUDIO_DUMP)
    audio_dump_init(FRAME_LEN, sizeof(short), 2);
#endif

    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.sync_start = true;
    stream_cfg.bits = AUD_BITS_16;
    stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
    stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    stream_cfg.sample_rate = AUD_SAMPRATE_16000;
    stream_cfg.chan_sep_buf = false;
    stream_cfg.vol = 12;
    stream_cfg.device = AUD_STREAM_USE_I2S0_MASTER;
    stream_cfg.io_path = AUD_IO_PATH_NULL;
    stream_cfg.data_size = i2s_playback_buf_len;
    stream_cfg.data_ptr = i2s_playback_buf;
    stream_cfg.handler = i2s_playback_callback;

    TRACE(2,"i2s playback sample_rate:%d, data_size:%d",stream_cfg.sample_rate,stream_cfg.data_size);
    ret = af_stream_open(I2S_STREAM_ID, AUD_STREAM_PLAYBACK, &stream_cfg);
    ASSERT(ret == 0, "i2s playback failed: %d", ret);


    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.sync_start = false;
    stream_cfg.bits = AUD_BITS_16;
    stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
    stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    stream_cfg.sample_rate = AUD_SAMPRATE_16000;
    stream_cfg.chan_sep_buf = false;
    stream_cfg.vol = 12;
    stream_cfg.device = AUD_STREAM_USE_I2S0_MASTER;
    stream_cfg.io_path = AUD_IO_PATH_NULL;
    stream_cfg.data_size = i2s_capture_buf_len;
    stream_cfg.data_ptr = i2s_capture_buf;
    stream_cfg.handler = i2s_capture_callback;
    TRACE(2,"i2s capture sample_rate:%d, data_size:%d",stream_cfg.sample_rate,stream_cfg.data_size);
    ret = af_stream_open(I2S_STREAM_ID, AUD_STREAM_CAPTURE, &stream_cfg);
    ASSERT(ret == 0, "i2s capture failed: %d", ret);

    return 0;
}

int tdm_stream_close(void)
{
    TRACE(1,"[%s] Run...", __func__);
    af_stream_close(I2S_STREAM_ID, AUD_STREAM_CAPTURE);
    af_stream_close(I2S_STREAM_ID, AUD_STREAM_PLAYBACK);

    return 0;
}

int tdm_stream_start(void)
{
    TRACE(1,"[%s] Run...", __func__);
    af_stream_start(I2S_STREAM_ID, AUD_STREAM_PLAYBACK);
    af_stream_start(I2S_STREAM_ID, AUD_STREAM_CAPTURE);

    return 0;
}

int tdm_stream_stop(void)
{
    TRACE(1,"[%s] Run...", __func__);
    af_stream_stop(I2S_STREAM_ID, AUD_STREAM_CAPTURE);
    af_stream_stop(I2S_STREAM_ID, AUD_STREAM_PLAYBACK);

    return 0;
}