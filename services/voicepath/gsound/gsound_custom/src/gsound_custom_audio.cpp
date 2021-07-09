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
#include "hal_timer.h"
#include "hal_aud.h"
#include "gsound_dbg.h"
#include "voice_sbc.h"
#include "app_audio.h"
#include "app_voicepath.h"
#include "app_bt_media_manager.h"
#include "gsound_custom_audio.h"
#include "audio_dump.h"

#ifdef IBRT
#include "app_tws_ibrt.h"
#endif

#if defined(GSOUND_HOTWORD_EXTERNAL)
#include "hotword_dsp_multi_bank_api.h"
#include "gsound_custom_hotword_external.h"
#endif

/************************private macro defination***************************/
#define GSOUND_ENCODER_OUTPUT_FRAME_SIZE 312
#define GSOUND_ENCODER_OUTPUT_FRAME_NUM 4
#define GSOUND_ENCODER_OUTPUT_BUF_SIZE (GSOUND_ENCODER_OUTPUT_FRAME_SIZE * GSOUND_ENCODER_OUTPUT_FRAME_NUM)
#define GSOUND_VOICE_BIT_NUMBER (AUD_BITS_16)

#define PCM_DATA_CACHE_FRAME_NUM (2) // cache 2 frame pcm data

#ifdef VOICE_DETECTOR_EN
/// ENABLE_VOICE_DETECTOR
#ifdef VAD_USE_8K_SAMPLE_RATE
/// SAMPLE_RATE_8K
#define GSOUND_PCM_DATA_BUF_SIZE ((32 * 1024) + (VOICE_SBC_PCM_DATA_SIZE_PER_FRAME * PCM_DATA_CACHE_FRAME_NUM))
/// SAMPLE_RATE_8K
#else
/// SAMPLE_RATE_16K
#define GSOUND_PCM_DATA_BUF_SIZE ((16 * 1024) + (VOICE_SBC_PCM_DATA_SIZE_PER_FRAME * PCM_DATA_CACHE_FRAME_NUM))
/// SAMPLE_RATE_16K
#endif
///ENABLE_VOICE_DETECTOR_EN
#else
/// DISABLE_VOICE_DETECTOR_EN
#define GSOUND_PCM_DATA_BUF_SIZE (VOICE_SBC_PCM_DATA_SIZE_PER_FRAME * PCM_DATA_CACHE_FRAME_NUM)
/// DISABLE_VOICE_DETECTOR_EN
#endif

#if AI_CAPTURE_CHANNEL_NUM == 2
#define MIC_CHANNEL_COUNT (2)
#else
#define MIC_CHANNEL_COUNT (1)
#endif

#ifdef AI_CAPTURE_DATA_AEC
#define GSOUND_MIC_CHANNEL_COUNT (MIC_CHANNEL_COUNT + 2)
#else
#define GSOUND_MIC_CHANNEL_COUNT (MIC_CHANNEL_COUNT)
#endif

#define GSOUND_VOICE_VOLUME (16)
#define GSOUND_VOICE_SAMPLE_RATE (AUD_SAMPRATE_16000)
#define GSOUND_VOICE_PCM_CHUNK_SIZE (GSOUND_MIC_CHANNEL_COUNT * VOICE_SBC_PCM_DATA_SIZE_PER_FRAME)
#define GSOUND_VOICE_PCM_DATA_SIZE (GSOUND_VOICE_PCM_CHUNK_SIZE * 2)

// #define BISTO_DUMP_AUDIO_DATA (1)
// #define HW_TRACE_PROCESS_TIME (1)

/*********************external function declearation************************/

/************************private type defination****************************/
enum
{
    STATE_IDLE = 0,
    STATE_BUSY = 1,

    STATE_NUM,
};

typedef struct
{
    uint8_t micDtatStreaming : 1;
    uint8_t resetEncoder : 1;
    uint8_t blockReason : 2;
    uint8_t glibRequestData : 1;
    uint8_t reserved : 3;

    uint8_t resetEncoderState;
    uint8_t *inputDataBuf;
    uint32_t inputDataLen;
    uint8_t *encodeDataBuf;
    uint32_t encodeDataLen;

    // used to store the audio settings from gsoundLib
    GSoundTargetAudioInSettings libSettings;

    // used to store the audio interface handler
    const GSoundAudioInterface *handler;
} GSOUND_AUDIO_CONTEXT_T;

/**********************private function declearation************************/
/**
 * @brief set resetEncoder flag
 * NOTE: this function should be called when:
 *       1.initialize the SBC encoder(isReset=true)
 *       2.any update of encoder param(isReset=true)
 *       3.updated encoder param took effect(isReset=false)
 *
 * @param isReset flag value to set
 */
static void _reset_codec(uint8_t isReset);

/**
 * @brief configure the param of voice encoder
 * 
 */
static void _config_voice_encoder(void);

/**
 * @brief initialize the voicepath environment
 * NOTE: all of the param in @see VOICEPATH_CTX_T should be initialized 
 * 
 */
static void _init_voicepath_env(void);

/**
 * @brief start capture mic data
 * 
 */
static void _start_capture_stream(void);

void _init_voicepath(void);
void _deinit_voicepath(void);
uint32_t _mic_data_handle(uint8_t *ptrBuf, uint32_t length, uint8_t *outputPtr);

/************************private variable defination************************/
static GSOUND_AUDIO_CONTEXT_T gAudioEnv;

static VOICE_SBC_CONFIG_T gSbcEncodeCfg = {
    VOICE_SBC_CHANNEL_COUNT,
    VOICE_SBC_CHANNEL_MODE,
    VOICE_SBC_BIT_POOL,
    VOICE_SBC_SIZE_PER_SAMPLE,
    VOICE_SBC_SAMPLE_RATE,
    VOICE_SBC_NUM_BLOCKS,
    VOICE_SBC_NUM_SUB_BANDS,
    VOICE_SBC_MSBC_FLAG,
    VOICE_SBC_ALLOC_METHOD,
};

struct AF_STREAM_CONFIG_T gStreamCfg = {
    .bits = GSOUND_VOICE_BIT_NUMBER,
    .sample_rate = GSOUND_VOICE_SAMPLE_RATE,
    .channel_num = (enum AUD_CHANNEL_NUM_T)GSOUND_MIC_CHANNEL_COUNT,
    .channel_map = (enum AUD_CHANNEL_MAP_T)0,
    .device = AUD_STREAM_USE_INT_CODEC,
#ifdef VOICE_DETECTOR_EN
    .io_path = AUD_INPUT_PATH_VADMIC,
#else
    .io_path = AUD_INPUT_PATH_ASRMIC,
#endif
    .chan_sep_buf = false,
    .sync_start = false,
    .slot_cycles = 0,
    .fs_cycles = 0,
    .handler = NULL,
    .data_ptr = NULL,
    .data_size = GSOUND_VOICE_PCM_DATA_SIZE,
    .vol = GSOUND_VOICE_VOLUME,
};

CAPTURE_STREAM_HANDLER_T GVPstreamHandler = {
    _init_voicepath,
    _deinit_voicepath,
    _mic_data_handle,
};

/// used to mark the output SBC frame length
uint32_t outputSbcFrameLen = 0;

/// used to mark if someone is handling @see outputSbcFrameLen
bool outputSbcFrameLenHandling = false;

double frameCntPerMs = 0.0;

#ifdef HW_TRACE_PROCESS_TIME
static uint32_t last_process_time;
#endif

/****************************function defination****************************/
static void _reset_codec(uint8_t isReset)
{
    gAudioEnv.resetEncoder = isReset;
    GLOG_D("reset_codec flag is set to:%d", isReset);
}

static void _config_voice_encoder(void)
{
    voice_sbc_init(&gSbcEncodeCfg);
}

static void _init_voicepath_env(void)
{
    VOICEPATH_CTX_T *pCtx = (VOICEPATH_CTX_T *)app_voicepth_get_env_ptr();

    // the capture stream could be exsiting at the same time of playback stream,
    // so the mempool has to be an isolated one
    app_capture_audio_mempool_init();

    // init pcm data cache buffer
    app_capture_audio_mempool_get_buff(&(pCtx->pcmDataCacheBuf),
                                       GSOUND_PCM_DATA_BUF_SIZE);

    // init pcm data cache queue
    InitCQueue(&(pCtx->pcmDataCacheBufQueue),
               GSOUND_PCM_DATA_BUF_SIZE,
               (CQItemType *)(pCtx->pcmDataCacheBuf));

    // init pcm data handle buffer(used to handle data in a integrated chunck)
    app_capture_audio_mempool_get_buff(&(pCtx->pcmDataHandleBuf),
                                       VOICE_SBC_PCM_DATA_SIZE_PER_FRAME);

    // init encoded data buffer
    app_capture_audio_mempool_get_buff(&(pCtx->encodeDataBuf),
                                       GSOUND_ENCODER_OUTPUT_BUF_SIZE);

    // capture stream configuration
    pCtx->streamCfg = gStreamCfg;

    // capture handler configuration
    pCtx->captureStreamHandler = GVPstreamHandler;

    // update the SBC encoder confiuration
    _reset_codec(true);
}

static void _start_capture_stream(void)
{
    gAudioEnv.micDtatStreaming = true;
    _init_voicepath_env();

#ifdef GSOUND_HOTWORD_EXTERNAL
    gsound_custom_hotword_external_init_buffer();
#endif

#ifdef BISTO_DUMP_AUDIO_DATA
    audio_dump_init(VOICE_SBC_PCM_DATA_SIZE_PER_FRAME / VOICE_SBC_SIZE_PER_SAMPLE, VOICE_SBC_SIZE_PER_SAMPLE, 1);
#endif

    // the audio functionality is managed by audio manager, so
    // we start the capture stream here, and the callback function
    // app_voicepath_start_audio_stream will be called to execute the opening
    // mic operation
    app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,
                                  BT_STREAM_CAPTURE,
                                  0,
                                  0);

#ifdef HW_TRACE_PROCESS_TIME
    last_process_time = hal_fast_sys_timer_get();
#endif
}

void gsound_custom_audio_init(void)
{
    app_voicepath_reset_env();
    memset((uint8_t *)&gAudioEnv, 0, sizeof(gAudioEnv));
}

uint32_t gsound_custom_audio_get_output_sbc_frame_len(void)
{
    /// wait until outputSbcFrameLen handling done
    while (outputSbcFrameLenHandling)
    {
        hal_sys_timer_delay_us(10);
    };

    return outputSbcFrameLen;
}

double gsound_custom_audio_get_sbc_frame_cnt_per_ms(void)
{
    return frameCntPerMs;
}

void gsound_custom_audio_set_libgsound_request_data_flag(bool flag)
{
    gAudioEnv.glibRequestData = flag;
    GLOG_I("glib request data flag is set to:%d", gAudioEnv.glibRequestData);
}

void gsound_custom_audio_store_audio_in_settings(const GSoundTargetAudioInSettings *settings)
{
    memcpy((void *)&gAudioEnv.libSettings, (void *)settings, sizeof(GSoundTargetAudioInSettings));
    GLOG_D("received audio in settings");
    GLOG_D("bitpool:%d", gAudioEnv.libSettings.sbc_bitpool);
    GLOG_D("enable_sidetone:%d", gAudioEnv.libSettings.enable_sidetone);
    GLOG_D("raw_samples_required:%d", gAudioEnv.libSettings.raw_samples_required);
    GLOG_D("include_sbc_headers:%d", gAudioEnv.libSettings.include_sbc_headers);
}

void gsound_custom_audio_store_lib_interface(const GSoundAudioInterface *handler)
{
    gAudioEnv.handler = handler;
}

uint32_t gsound_custom_audio_get_bitpool(void)
{
    return gAudioEnv.libSettings.sbc_bitpool;
}

void gsound_custom_audio_open_mic(int sbc_bitpool)
{
    GLOG_I("%s bitpool:%d, micDataStreaming:%d",
           __func__,
           sbc_bitpool,
           gAudioEnv.micDtatStreaming);

    // check if need to updata bitpool
    if (gSbcEncodeCfg.bitPool != sbc_bitpool)
    {
        GLOG_I("Run-time update gsound bitpool from %d to %d",
               gSbcEncodeCfg.bitPool,
               sbc_bitpool);

        // update the codec info
        gSbcEncodeCfg.bitPool = sbc_bitpool;

        while (STATE_BUSY == gAudioEnv.resetEncoderState)
        {
            hal_sys_timer_delay_us(10);
        }
        gAudioEnv.resetEncoderState = STATE_BUSY;

        // set the reset encoder flag for the next round sbc encoding to reset the encoder env
        _reset_codec(true);
        gAudioEnv.resetEncoderState = STATE_IDLE;
    }

    if (gAudioEnv.micDtatStreaming)
    {
        GLOG_I("mic data already streaming");
    }
    else
    {
#ifdef IBRT
        if (app_tws_ibrt_role_get_callback(NULL) == IBRT_SLAVE)
        {
            GLOG_I("slave cannot open mic.");
            gAudioEnv.blockReason = AUDIO_BLOCK_REASON_ROLE;
        }
        else
#endif
        {
            _start_capture_stream();
        }
    }
}

static uint32_t _audio_encode_mic_data(void)
{
    uint32_t pcmEncodedLen = GSOUND_ENCODER_OUTPUT_BUF_SIZE;
    uint32_t sbcFrameLen = 0;

    while (STATE_BUSY == gAudioEnv.resetEncoderState)
    {
        hal_sys_timer_delay_us(10);
    }

    gAudioEnv.resetEncoderState = STATE_BUSY;
    bool resetEncoder = gAudioEnv.resetEncoder;

    // check if codec need to reset
    if (resetEncoder)
    {
        _config_voice_encoder();

        // clear the reset codec flag
        _reset_codec(false);
    }

    uint32_t encodedDataLen = voice_sbc_encode(gAudioEnv.inputDataBuf,
                                               gAudioEnv.inputDataLen,
                                               &pcmEncodedLen,
                                               gAudioEnv.encodeDataBuf,
                                               resetEncoder);

    gAudioEnv.resetEncoderState = STATE_IDLE;
    GLOG_V("raw data len:%d, encodedDataLen:%d", gAudioEnv.inputDataLen, encodedDataLen);

    sbcFrameLen = voice_sbc_get_frame_len();

    /// mark that handling outputSbcFrameLen now
    outputSbcFrameLenHandling = true;
    outputSbcFrameLen = sbcFrameLen;

    // calculate the frame count
    uint32_t frameCount = encodedDataLen / sbcFrameLen;
    frameCntPerMs = ((double)frameCount) / ((double)VOICE_SBC_FRAME_PERIOD_IN_MS);
    GLOG_V("sbc frameCntPerMs is %d/1000", (uint32_t)(frameCntPerMs * 1000));

    //// remove the SBC frame header if needed
    if (!gAudioEnv.libSettings.include_sbc_headers)
    {
        // trimed the frame header as libgsound required
        // Note: it is assumed that the sbc encoder will
        // return an integer number of frames(no partial frames)
        outputSbcFrameLen -= VOICE_SBC_FRAME_HEADER_LEN;

        uint32_t offsetToFill = 0;
        uint32_t offsetToFetch = VOICE_SBC_FRAME_HEADER_LEN;

        for (uint8_t frame = 0; frame < frameCount; frame++)
        {
            memmove((gAudioEnv.encodeDataBuf + offsetToFill),
                    (gAudioEnv.encodeDataBuf + offsetToFetch),
                    outputSbcFrameLen);

            offsetToFill += outputSbcFrameLen;
            offsetToFetch += sbcFrameLen;
        }

        encodedDataLen = offsetToFill;
    }
    else
    {
        GLOG_W("include_sbc_headers:%d", gAudioEnv.libSettings.include_sbc_headers);
    }

    /// mark that handling outputSbcFrameLen done
    outputSbcFrameLenHandling = false;

    return encodedDataLen;
}

void gsound_custom_audio_transmit_data_to_libgsound()
{
    if (gAudioEnv.glibRequestData)
    {
#if defined(GSOUND_HOTWORD_ENABLED)
        uint32_t rawDataLen = 0;
        const uint16_t *rawDataBuf = NULL;

        if (gAudioEnv.libSettings.raw_samples_required)
        {
            rawDataBuf = (const uint16_t *)gAudioEnv.inputDataBuf;
            rawDataLen = gAudioEnv.inputDataLen / VOICE_SBC_SIZE_PER_SAMPLE;
        }

        if (GSOUND_STATUS_OK != gAudioEnv.handler->gsound_target_on_audio_in_ex(rawDataBuf,
                                                                                rawDataLen,
                                                                                gAudioEnv.encodeDataBuf,
                                                                                gAudioEnv.encodeDataLen,
                                                                                outputSbcFrameLen))
        {
            GLOG_W("no room in audio buffer of libgsound");
        }
#else  // #if !defined(GSOUND_HOTWORD_ENABLE)
        gAudioEnv.handler->gsound_target_on_audio_in(gAudioEnv.encodeDataBuf,
                                                     gAudioEnv.encodeDataLen,
                                                     outputSbcFrameLen);
#endif // #if defined(GSOUND_HOTWORD_ENABLE)
    }
}

uint32_t gsound_custom_audio_handle_mic_data(uint8_t *inputBuf,
                                             uint32_t inputLen,
                                             uint8_t *outputBuf)
{
    if ((NULL != inputBuf) && (0 != inputLen) && (NULL != outputBuf))
    {
        gAudioEnv.inputDataBuf = inputBuf;
        gAudioEnv.inputDataLen = inputLen;
        gAudioEnv.encodeDataBuf = outputBuf;

        // encode mic data into SBC frames
        gAudioEnv.encodeDataLen = _audio_encode_mic_data();

#if defined(GSOUND_HOTWORD_EXTERNAL)
        // cache data into hotword buffer
        gsound_custom_hotword_external_process_data(inputBuf,
                                                    inputLen,
                                                    outputBuf,
                                                    gAudioEnv.encodeDataLen,
                                                    gAudioEnv.glibRequestData);
#else
        gsound_custom_audio_transmit_data_to_libgsound();
#endif
    }
    else
    {
        gAudioEnv.encodeDataLen = 0;
        GLOG_W("INVALID input param, inputBuf:%p, inputLen:%d, outputBuf:%p",
               inputBuf,
               inputLen,
               outputBuf);
    }

    return gAudioEnv.encodeDataLen;
}

void gsound_custom_audio_close_mic(void)
{
    GLOG_D("%s", __func__);

    if (!gAudioEnv.micDtatStreaming)
    {
        GLOG_D("%s: not streaming", __func__);

        if (AUDIO_BLOCK_REASON_ROLE == gAudioEnv.blockReason)
        {
            gAudioEnv.blockReason = AUDIO_BLOCK_REASON_NULL;
        }
    }
    else
    {
        // stop the capture steam via the BT stream manager
        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,
                                      BT_STREAM_CAPTURE,
                                      0,
                                      0);

#ifdef GSOUND_HOTWORD_EXTERNAL
        gsound_custom_hotword_external_deinit_buffer();
#endif

        gAudioEnv.micDtatStreaming = false;

        if (gAudioEnv.blockReason == AUDIO_BLOCK_REASON_NULL)
        {
            gAudioEnv.libSettings.raw_samples_required = false;
        }
    }
}

void gsound_custom_audio_in_stream_interrupted_handler(uint8_t reason)
{
    if (gAudioEnv.micDtatStreaming && gAudioEnv.handler)
    {
        gAudioEnv.handler->audio_in_stream_interrupted((GSoundTargetAudioInStreamIntReason)reason);
    }
}

#ifdef IBRT
void gsound_custom_audio_role_switch_complete_handler(uint8_t role)
{
    if ((IBRT_MASTER == role) && (AUDIO_BLOCK_REASON_ROLE == gAudioEnv.blockReason))
    {
        gAudioEnv.blockReason = AUDIO_BLOCK_REASON_NULL;
        gsound_custom_audio_open_mic(gSbcEncodeCfg.bitPool);
    }
    else if (IBRT_SLAVE == role && gAudioEnv.micDtatStreaming)
    {
        gAudioEnv.blockReason = AUDIO_BLOCK_REASON_ROLE;
        gsound_custom_audio_close_mic();
    }
}
#endif

// called by every starting voice capture operation
void _init_voicepath(void)
{
    _config_voice_encoder();

#ifdef GSOUND_HOTWORD_EXTERNAL
    // init cp to start processing data
    gsound_custom_hotword_external_start_cp_process();
#endif
}

void _deinit_voicepath(void)
{
#ifdef GSOUND_HOTWORD_EXTERNAL
    gsound_custom_hotword_external_stop_cp_process();
#endif
}

uint32_t _mic_data_handle(uint8_t *ptrBuf, uint32_t length, uint8_t *outputPtr)
{
    if (!gAudioEnv.micDtatStreaming)
    {
        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,
                                      BT_STREAM_CAPTURE,
                                      0,
                                      0);
    }

#ifdef HW_TRACE_PROCESS_TIME
    uint32_t stime;
    uint32_t etime;

    stime = hal_fast_sys_timer_get();
#endif

#ifdef BISTO_DUMP_AUDIO_DATA
    audio_dump_add_channel_data(0, (void *)gAudioEnv.inputDataBuf, gAudioEnv.inputDataLen / VOICE_SBC_SIZE_PER_SAMPLE);
#endif

#ifdef GSOUND_HOTWORD_EXTERNAL
    // inform co-processor to handle data
    gsound_custom_hotword_external_handle_mic_data(ptrBuf, length, outputPtr);
#else
    gsound_custom_audio_handle_mic_data(ptrBuf, length, outputPtr);
#endif

#ifdef HW_TRACE_PROCESS_TIME
    etime = hal_fast_sys_timer_get();
    GLOG_D("MCU handle time: %5u us in %5u us",
           FAST_TICKS_TO_US(etime - stime),
           FAST_TICKS_TO_US(etime - last_process_time));
    last_process_time = etime;
#endif

#ifdef BISTO_DUMP_AUDIO_DATA
    audio_dump_run();
#endif

    return 0;
}
