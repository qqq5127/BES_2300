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
#include "cqueue.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "voice_sbc.h"
#include "app_audio.h"
#include "app_utils.h"
#include "apps.h"
#include "app_voicepath.h"
#include "app_ble_rx_handler.h"
#include "app_ble_mode_switch.h"
#include "gapm_task.h"
#include "nvrecord_extension.h"
#include "app_ai_manager_api.h"
#include "app_bt_media_manager.h"

#ifdef BISTO_ENABLED
#include "gsound_custom.h"
#include "gsound_custom_bt.h"
#include "gsound_custom_actions.h"
#endif


#ifdef VOICE_DETECTOR_EN
#include "app_voice_detector.h"

#ifdef VAD_USE_8K_SAMPLE_RATE
#include "app_bt_stream.h" //!< use the resample APIs
#endif
#endif

/*********************external function declearation************************/
#ifdef VOICE_DETECTOR_EN
extern short vad_buf[VAD_BUFFER_LEN];
extern unsigned int vad_buf_len;
#endif

/************************private macro defination***************************/
#ifdef GSOUND_HOTWORD_EXTERNAL
#define AI_REQUIRED_SYSFREQ APP_SYSFREQ_26M
#else
#define AI_REQUIRED_SYSFREQ APP_SYSFREQ_78M
#endif

/************************private type defination****************************/

/**********************private function declearation************************/
/**
 * @brief process enqueued PCM data
 * 
 */
static void _mic_data_process(void);

/************************private variable defination************************/
static uint8_t app_voicepath_pending_streaming = 0;
static uint8_t app_voicepath_stream_state = 0;
osMutexDef(pcm_queue_mutex);
osMutexId pcm_queue_mutex_id;

static VOICEPATH_CTX_T VPctx;

#ifdef VOICE_DETECTOR_EN
static volatile int audioCaptureStartFlag = 0;

#ifdef VAD_USE_8K_SAMPLE_RATE
struct APP_RESAMPLE_T *resampler = NULL;
#endif
#endif

/****************************function defination****************************/
#ifdef VOICE_DETECTOR_EN
static void _voice_detector_run_done_cb(int state, void *argv)
{
    if (state == VOICE_DET_STATE_AUD_CAP_START)
    {
        audioCaptureStartFlag = 1;
    }
}

void app_voicepath_set_vad_audio_capture_start_flag(int flag)
{
    audioCaptureStartFlag = flag;
}

int app_voicepath_get_vad_audio_capture_start_flag(void)
{
    return audioCaptureStartFlag;
}

#ifdef VAD_USE_8K_SAMPLE_RATE
static void _resampled_data_come(uint8_t *ptrBuf, uint32_t length)
{
    TRACE(1, "Resampled data len:%d", length);

    /// enqueue data into pcm data queue
    app_voicepath_enqueue_pcm_data(ptrBuf, length);
}

void app_voicepath_init_resampler(uint8_t channelCnt,
                                  uint32_t outputBufSize,
                                  uint32_t srcSampleRate,
                                  uint32_t dstSampleRate,
                                  void *cb)
{
    APP_RESAMPLE_BUF_ALLOC_CALLBACK oldCb;

    if (NULL == resampler)
    {
        TRACE(3,
              "Create the resampler outputSize:%d, srcSampleRate:%d, dstSampleRate:%d",
              outputBufSize,
              srcSampleRate,
              dstSampleRate);

        oldCb = app_resample_set_buf_alloc_callback(app_capture_audio_mempool_get_buff);

        resampler = app_capture_resample_any_open((enum AUD_CHANNEL_NUM_T)channelCnt,
                                                  (APP_RESAMPLE_ITER_CALLBACK)cb,
                                                  outputBufSize,
                                                  ((float)srcSampleRate) / ((float)(dstSampleRate)));

        app_resample_set_buf_alloc_callback(oldCb);
    }
}

void app_voicepath_deinit_resample(void)
{
    if (resampler)
    {
        app_capture_resample_close(resampler);
        resampler = NULL;
    }
}

void app_voicepath_resample_run(uint8_t *ptrInput, uint32_t dataLen)
{
    TRACE(1, "Start resmple %d bytes", dataLen);
    uint32_t formerTicks = hal_sys_timer_get();
    app_capture_resample_run(resampler, ptrInput, dataLen);
    TRACE(1, "Resmple cost %d ms", TICKS_TO_MS(hal_sys_timer_get() - formerTicks));
}
#endif
#endif //!< VOICE_DETECTOR_EN

void app_voicepath_reset_env(void)
{
    memset((void *)&VPctx, 0, sizeof(VPctx));
    VPctx.pcmDataCacheBufMutexId = osMutexCreate((osMutex(pcm_queue_mutex)));
}

void *app_voicepth_get_env_ptr(void)
{
    return (void *)&VPctx;
}

int app_voicepath_enqueue_pcm_data(uint8_t *ptrBuf, uint32_t length)
{
    int ret = 0;
    uint32_t qAvaliable;

    osMutexWait(VPctx.pcmDataCacheBufMutexId, osWaitForever);
    qAvaliable = AvailableOfCQueue(&(VPctx.pcmDataCacheBufQueue));

    if (0 == qAvaliable)
    {
        TRACE(0, "voice PCM data buffer is full!");
        ret = -1;
    }
    else if (length > qAvaliable)
    {
        TRACE(0, "voice PCM data buffer exceed!");
        ret = -1;
    }
    else
    {
        // push the pcm data pending for encoding and xfer.
        // As the encoding costs time and may cause confliction with the a2dp
        // streaming callback function, when the a2dp streaming is active, we will
        // execute the encoding at the end of the a2dp streaming callback
        EnCQueue(&(VPctx.pcmDataCacheBufQueue), ptrBuf, length);
    }

    osMutexRelease(VPctx.pcmDataCacheBufMutexId);

    return ret;
}

static void _mic_data_process(void)
{
    osMutexWait(VPctx.pcmDataCacheBufMutexId, osWaitForever);
    uint32_t length = LengthOfCQueue(&(VPctx.pcmDataCacheBufQueue));
    osMutexRelease(VPctx.pcmDataCacheBufMutexId);

    if (0 == length)
    {
        TRACE(0, "voice PCM data buffer is empty!");
        return;
    }

    /*
   * TODO(mfk): This queue is never empty once there has been at least one voice
   *            query.
   */
    while (length >= VOICE_SBC_PCM_DATA_SIZE_PER_FRAME)
    {
        osMutexWait(VPctx.pcmDataCacheBufMutexId, osWaitForever);
        app_pop_data_from_cqueue(&(VPctx.pcmDataCacheBufQueue),
                                 VPctx.pcmDataHandleBuf,
                                 VOICE_SBC_PCM_DATA_SIZE_PER_FRAME);
        length = LengthOfCQueue(&(VPctx.pcmDataCacheBufQueue));
        osMutexRelease(VPctx.pcmDataCacheBufMutexId);

        if (VPctx.captureStreamHandler.handler)
        {
            VPctx.captureStreamHandler.handler(
                VPctx.pcmDataHandleBuf,
                VOICE_SBC_PCM_DATA_SIZE_PER_FRAME,
                VPctx.encodeDataBuf);
        }
    }
}

static void _init_capture(void)
{
    if (VPctx.captureStreamHandler.initializer)
    {
        VPctx.captureStreamHandler.initializer();
    }
}

static void _dinit_capture(void)
{
    if (VPctx.captureStreamHandler.deinitializer)
    {
        VPctx.captureStreamHandler.deinitializer();
    }
}

uint32_t app_voicepath_more_data(uint8_t *ptrBuf, uint32_t length)
{
    // if the voice stream is already on, we won't start the a2dp stream right-now,
    // otherwise there will be confliction between the mic&speaker audio callback
    // triggering. So we put set the pending flag when opening a2dp stream and
    // trigger the a2dp stream at the time of catpure stream callback function here.
    if (app_voicepath_get_stream_pending_state(AUDIO_OUTPUT_STREAMING))
    {
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        app_voicepath_set_stream_state(AUDIO_OUTPUT_STREAMING, true);
        app_voicepath_set_pending_started_stream(AUDIO_OUTPUT_STREAMING, false);
    }

    // this part is used for mic data pre-process, NOTE:
    // 1. Finally enqueue the data
    // 2. Enqueue the data directly if there is no pro-process
#ifdef AI_ALGORITHM_ENABLE
    app_ai_algorithm_mic_data_handle(ptrBuf, length);
#else
#ifdef VOICE_DETECTOR_EN
    if (vad_buf_len > 0)
    {
        TRACE(2, "%s, vad_buf_len:%d", __func__, vad_buf_len);
#ifdef VAD_USE_8K_SAMPLE_RATE
        /// upsampling to 16K and enqueue resampled data
        app_voicepath_resample_run((uint8_t *)vad_buf,
                                   vad_buf_len);
#else
        app_voicepath_enqueue_pcm_data((uint8_t *)vad_buf,
                                       vad_buf_len);
#endif
        vad_buf_len = 0;
    }
#endif
    app_voicepath_enqueue_pcm_data(ptrBuf, length);
#endif

    // process enqueued data
    _mic_data_process();

    return length;
}

int app_voicepath_start_audio_stream(void)
{
    TRACE(1, "%s", __func__);

    app_sysfreq_req(APP_SYSFREQ_USER_AI_VOICE, AI_REQUIRED_SYSFREQ);

    uint8_t *bt_audio_buff = NULL;
    app_capture_audio_mempool_get_buff(&bt_audio_buff, VPctx.streamCfg.data_size);
    VPctx.streamCfg.data_ptr = BT_AUDIO_CACHE_2_UNCACHE(bt_audio_buff);
    VPctx.streamCfg.handler = app_voicepath_more_data;

    _init_capture();

#ifdef VOICE_DETECTOR_EN
#ifdef VAD_USE_8K_SAMPLE_RATE
    app_voicepath_init_resampler(AUD_CHANNEL_NUM_1,
                                 (16 * 1024),
                                 AUD_SAMPRATE_8000,
                                 AUD_SAMPRATE_16000,
                                 (void *)_resampled_data_come);
#endif

    app_voice_detector_init();
    app_voice_detector_open(VOICE_DETECTOR_ID_0);

    app_voice_detector_setup_stream(VOICE_DETECTOR_ID_0,
                                    AUD_STREAM_CAPTURE,
                                    &(VPctx.streamCfg));

    app_voice_detector_setup_callback(VOICE_DETECTOR_ID_0,
                                      VOICE_DET_CB_RUN_DONE,
                                      _voice_detector_run_done_cb,
                                      NULL);

    app_voice_detector_send_event(VOICE_DETECTOR_ID_0, VOICE_DET_EVT_VAD_START);
#else
    af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &(VPctx.streamCfg));

    // start the capture stream immediately
    af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
#endif

    // mark that the voice-path streaming is on
    app_voicepath_set_stream_state(VOICEPATH_STREAMING, true);

    return 0;
}

int app_voicepath_stop_audio_stream(void)
{
#ifdef VOICE_DETECTOR_EN
    app_voice_detector_close(VOICE_DETECTOR_ID_0);
    TRACE(0, "voice detector off");
#ifdef VAD_USE_8K_SAMPLE_RATE
    app_voicepath_deinit_resample();
#endif
#else
    af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
#endif

    _dinit_capture();

    // clear the voice path stream pending flag and streaming state flag
    app_voicepath_set_pending_started_stream(VOICEPATH_STREAMING, false);
    app_voicepath_set_stream_state(VOICEPATH_STREAMING, false);

    if (app_voicepath_get_stream_pending_state(AUDIO_OUTPUT_STREAMING))
    {
        af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
        app_voicepath_set_stream_state(AUDIO_OUTPUT_STREAMING, true);
        app_voicepath_set_pending_started_stream(AUDIO_OUTPUT_STREAMING, false);
    }

    // release the acquired system clock
    app_sysfreq_req(APP_SYSFREQ_USER_AI_VOICE, APP_SYSFREQ_32K);

    return 0;
}

void app_voicepath_set_pending_started_stream(uint8_t pendingStream, bool isEnabled)
{
    if (isEnabled)
    {
        app_voicepath_pending_streaming |= (1 << pendingStream);
    }
    else
    {
        app_voicepath_pending_streaming &= (~(1 << pendingStream));
    }
}

bool app_voicepath_get_stream_pending_state(uint8_t pendingStream)
{
    return app_voicepath_pending_streaming & (1 << pendingStream);
}

bool app_voicepath_get_stream_state(uint8_t stream)
{
    return (app_voicepath_stream_state & (1 << stream));
}

void app_voicepath_set_stream_state(uint8_t stream, bool isEnabled)
{
    if (isEnabled)
    {
        app_voicepath_stream_state |= (1 << stream);
    }
    else
    {
        app_voicepath_stream_state &= (~(1 << stream));
    }
}

void app_pop_data_from_cqueue(CQueue *ptrQueue, uint8_t *buff, uint32_t len)
{
    uint8_t *e1 = NULL, *e2 = NULL;
    unsigned int len1 = 0, len2 = 0;

    PeekCQueue(ptrQueue, len, &e1, &len1, &e2, &len2);
    if (len == (len1 + len2))
    {
        memcpy(buff, e1, len1);
        memcpy(buff + len1, e2, len2);
        DeCQueue(ptrQueue, 0, len);
    }
    else
    {
        memset(buff, 0x00, len);
    }
}

bool app_voicepath_check_hfp_active(void)
{
    return (bool)bt_media_is_media_active_by_type(BT_STREAM_VOICE);
}

extern uint8_t hfp_get_call_setup_running_on_state(void);

bool app_voicepath_check_hfp_callsetup_running(void)
{
    return hfp_get_call_setup_running_on_state();
}

bool app_voicepath_check_active()
{
    return (bool)bt_media_is_media_active_by_type(BT_STREAM_CAPTURE);
}

void app_voicepath_start_ble_adv_uuid(void)
{
    app_ble_start_connectable_adv(BLE_ADVERTISING_INTERVAL);
}

bool voicepathInitializated = false;

void app_voicepath_init(void)
{
    if (!voicepathInitializated)
    {
        bool isEnable = true;
        voicepathInitializated = true;

        app_ble_rx_handler_init();

#ifdef BISTO_ENABLED
        if (app_ai_manager_is_in_multi_ai_mode())
        {
            if ((app_ai_manager_get_current_spec() != AI_SPEC_GSOUND) ||
                (app_ai_manager_spec_get_status_is_in_invalid() == true))
            {
                TRACE(1,"%s set gsound false", __func__);
                isEnable = false;
            }

            if (isEnable)
            {
                NV_EXTENSION_RECORD_T *pNvExtRec = nv_record_get_extension_entry_ptr();
                if (!(pNvExtRec->gsound_info.isGsoundEnabled))
                {
                    isEnable = false;
                }
            }
        }

        gsound_custom_init(isEnable);
#else // #ifdef BISTO_ENABLED
        app_voicepath_custom_init();
#endif
    }
    else
    {
        TRACE(0, "already initialized");
    }
}
