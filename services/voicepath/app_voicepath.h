/***************************************************************************
*
*Copyright 2015-2019 BES.
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


#ifndef __APP_VOICEPATH_H__
#define __APP_VOICEPATH_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/
#include "cmsis_os.h"
#include "cqueue.h"
#include "hal_aud.h"
#include "audioflinger.h"

/******************************macro defination*****************************/

#define AUDIO_OUTPUT_STREAMING          0
#define VOICEPATH_STREAMING             1

#define CPATURE_FUNC_INDEX_SMARTVOICE   0
#define CAPTURE_FUNC_INDEX_MIX_MIC      1

#define CAPTURE_FEATURE_BIT_OFFSET_SMARTVOICE    CPATURE_FUNC_INDEX_SMARTVOICE
#define CAPTURE_FEATURE_BIT_OFFSET_MIC_MIX       CAPTURE_FUNC_INDEX_MIX_MIC

#define CAPTURE_FEATURE_SMARTVOICE    (1 << CAPTURE_FEATURE_BIT_OFFSET_SMARTVOICE)
#define CAPTURE_FEATURE_MIC_MIX       (1 << CAPTURE_FEATURE_BIT_OFFSET_MIC_MIX)

#define CAPTURE_STREAM_MIC_DATA_HANDLER_CNT  2

#define HW_SIDE_TONE_ATTENUATION_COEF_IN_DISABLE_MODE   (0x1F*2)
#define HW_SIDE_TONE_MAX_ATTENUATION_COEF       60  // in unit of db
#define HW_SIDE_TONE_MIN_ATTENUATION_COEF       0   // in unit of db
#define HW_SIDE_TONE_FADE_IN_GAIN_STEP          10  // in unit of db
#define HW_SIDE_TONE_FADE_IN_TIME_STEP_IN_MS    30  // in unit of ms
#define HW_SIDE_TONE_USED_ATTENUATION_COEF      0   // in unit of db

/******************************type defination******************************/
typedef void (*capture_stream_init_func)(void);
typedef void (*capture_stream_deinit_func)(void);
typedef uint32_t (*capture_stream_mic_data_handler_func)(uint8_t* ptrData, uint32_t dataLen, uint8_t *outputPtr);

enum
{
    INIT_STATE_IDLE = 0,
    INIT_STATE_DONE = 1,
};

typedef struct
{
    capture_stream_init_func             initializer;
    capture_stream_deinit_func           deinitializer;
    capture_stream_mic_data_handler_func handler;
} CAPTURE_STREAM_HANDLER_T;

typedef struct
{
    CQueue pcmDataCacheBufQueue;
    uint8_t *pcmDataCacheBuf;
    osMutexId pcmDataCacheBufMutexId;
    uint8_t *pcmDataHandleBuf;
    uint8_t *encodeDataBuf;

    struct AF_STREAM_CONFIG_T streamCfg;
    CAPTURE_STREAM_HANDLER_T captureStreamHandler;
} VOICEPATH_CTX_T;

enum {
    APP_VOICEPATH_MESSAGE_ID_CUSTOMER_FUNC,
    APP_VOICEPATH_MESSAGE_ID_MAX
};

typedef void (*APP_VOICEPATH_MOD_HANDLE_T)(void);

/****************************function declearation**************************/
#ifdef VOICE_DETECTOR_EN
void app_voicepath_set_vad_audio_capture_start_flag(int flag);

int app_voicepath_get_vad_audio_capture_start_flag(void);
#endif

void app_voicepath_reset_env(void);

void* app_voicepth_get_env_ptr(void);

void app_voicepath_custom_init(void);

void app_voicepath_start_ble_adv_uuid(void);

void app_voicepath_init(void);

int app_voicepath_enqueue_pcm_data(uint8_t* ptrBuf, uint32_t length);

int app_voicepath_start_audio_stream(void);

int app_voicepath_stop_audio_stream(void);

void app_voicepath_enable_mic_mix_during_music(void);

void app_voicepath_disable_mic_mix_during_music(void);

void app_voicepath_connected(uint8_t connType);

void app_voicepath_disconnected(uint8_t disconnType);

bool app_voicepath_check_hfp_active(void);

bool app_voicepath_check_active(void);

uint32_t app_voicepath_more_data(uint8_t *ptrBuf, uint32_t length);

void app_voicepath_registor_enqueue_handler(uint32_t handler);

void app_voicepath_set_pending_started_stream(uint8_t pendingStream, bool isEnabled);

bool app_voicepath_get_stream_pending_state(uint8_t pendingStream);

bool app_voicepath_get_stream_state(uint8_t stream);

void app_voicepath_set_stream_state(uint8_t stream, bool isEnabled);

void app_voicepath_resmple(uint8_t *ptrInput, uint32_t dataLen);

uint32_t app_voicepath_mic_data_process(uint8_t *ptrBuf, uint32_t length);

void app_pop_data_from_cqueue(CQueue * ptrQueue,
                              uint8_t *buff,
                              uint32_t len);

#ifdef __cplusplus
}
#endif
#endif
