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
#include "stdio.h"
#include "hal_timer.h"
#include "gsound_dbg.h"
#include "gsound_custom_bt.h"
#include "gsound_custom_hotword_external.h"
#include "gsound_custom_audio.h"
#include "voice_sbc.h"
#include "app_voicepath.h"
#include "hotword_dsp_multi_bank_api.h"
#include "nvrecord_gsound.h"
#include "audio_dump.h"
#include "app_ai_if_thirdparty.h"
#include "norflash_api.h"

#ifdef VOICE_DETECTOR_EN
#include "app_voice_detector.h"
#endif

/*********************external function declearation************************/

/************************private macro defination***************************/
// #define HW_TRACE_CP_PROCESS_TIME (1)

/************************private type defination****************************/
typedef struct
{
    char modelId[GSOUND_HOTWORD_MODEL_ID_BYTES];
    int bankCnt;
    uint8_t dataIdx;
    uint32_t dataSize;
    uint8_t textIdx;
    uint32_t textSize;
    GSoundHotwordMmapType bankType;
    void *bankAddr[MEMORY_BANK_CNT];
} MODEL_INFO_T;

/**********************private function declearation************************/
CP_TEXT_SRAM_LOC static unsigned int _cp_hw_main(uint8_t event);
static unsigned int _mcu_hw_evt_handler(uint8_t event);

/************************private variable defination************************/
static uint8_t emptyFrame[GSOUND_TARGET_MAX_SBC_FRAME_LEN] = {
    0x00, 0x00, 0x00, 0x00,
    0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb,
    0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb,
    0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb,
    0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb,
};

#ifdef HW_TRACE_CP_PROCESS_TIME
static CP_BSS_LOC uint32_t cp_last_process_time;
#endif

osMutexDef(hotwordDataCacheMutex);

HW_EXTERNAL_CONTEXT_T EHWctx;

static struct cp_task_desc TASK_DESC_HW = {CP_ACCEL_STATE_CLOSED, _cp_hw_main, NULL, _mcu_hw_evt_handler, NULL};

/****************************function defination****************************/
static uint32_t _read_uint32_big_endian_and_increment(const uint8_t **read_ptr)
{
    uint8_t i = 0;
    uint32_t ret = 0;
    for (i = 0; i < 4; i++)
    {
        ret |= **read_ptr << (8 * (3 - i));
        (*read_ptr)++;
    }
    return ret;
}

int gsound_custom_hotword_external_start_cp_process(void)
{
    norflash_api_flush_disable(NORFLASH_API_USER_CP,(uint32_t)cp_accel_init_done);
    cp_accel_open(CP_TASK_HW, &TASK_DESC_HW);

    while (cp_accel_init_done() == false)
    {
        GLOG_D("[%s] Delay...", __func__);
        osDelay(1);
    }
    norflash_api_flush_enable(NORFLASH_API_USER_CP);
#ifdef HW_TRACE_CP_PROCESS_TIME
    cp_last_process_time = hal_fast_sys_timer_get();
#endif

    GLOG_D("[%s]", __func__);

    return 0;
}

void gsound_custom_hotword_external_init(bool enable)
{
    GLOG_D("[%s]", __func__);

    memset(&EHWctx, 0, sizeof(EHWctx));

    if (EHWctx.hotwordDataCacheMutexId == NULL)
    {
        EHWctx.hotwordDataCacheMutexId = osMutexCreate((osMutex(hotwordDataCacheMutex)));
    }

    EHWctx.hotwordDetectStatus = HW_DETECT_STATUS_IDLE;
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_IDLE;
    EHWctx.isEmptyLastTime = true;
    memcpy(EHWctx.activeModelId, INVALID_MODEL_ID, GSOUND_HOTWORD_MODEL_ID_BYTES);

    uint8_t *buffer;
    app_ai_if_thirdparty_mempool_init();
    app_ai_if_thirdparty_mempool_get_buff(&buffer, (GSOUND_MAX_HOTWORD_MODEL_DATA_BUFFER_SIZE + GSOUND_MAX_HOTWORD_MODEL_ALIGNMENT_EDIAN));
    EHWctx.hotwordModelData = (uint8_t*)ALIGN((uint32_t)buffer, 4);
    app_ai_if_thirdparty_mempool_get_buff(&buffer, (GSOUND_MAX_HOTWORD_MODEL_TEXT_BUFFER_SIZE + GSOUND_MAX_HOTWORD_MODEL_ALIGNMENT_EDIAN));
    EHWctx.hotwordModelText = (uint8_t*)ALIGN((uint32_t)buffer, 4);
}

void gsound_custom_hotword_external_registor_buffer_alloc_func(GHW_BUF_ALLOC_FUNC_T func)
{
    GLOG_I("hotword buffer alloc func update:%p->%p", EHWctx.alloc, func);
    EHWctx.alloc = func;
}

void gsound_custom_hotword_external_init_buffer(void)
{
    GLOG_D("%s", __func__);

    uint8_t *buffer;
    if (EHWctx.alloc)
    {
        /// alloc cp PCM data cache buffer
        EHWctx.alloc(&buffer, CP_PCM_DATA_CACHE_BUFFER_SIZE);
        for (uint8_t i = 0; i < CP_PCM_DATA_CACHE_FRAME_NUM; i++)
        {
            EHWctx.micDataInputBuf[i] = buffer + (i * VOICE_SBC_PCM_DATA_SIZE_PER_FRAME);
        }

        /// alloc hotword SBC encoded data cache buffer
        EHWctx.alloc(&buffer, HOTWORD_SBC_DATA_CACHE_BUF_MAX_LEN);

        osMutexWait(EHWctx.hotwordDataCacheMutexId, osWaitForever);
        EHWctx.hotwordDataCacheBuf = buffer;

        // init mic data cache queue
        InitCQueue(&(EHWctx.hotwordDataCacheQueue),
                HOTWORD_SBC_DATA_CACHE_BUF_MAX_LEN,
                EHWctx.hotwordDataCacheBuf);
        osMutexRelease(EHWctx.hotwordDataCacheMutexId);
    }
    else
    {
        ASSERT(0, "pls registor hotword buffer alloc func first.");
    }
}

void gsound_custom_hotword_external_deinit_buffer(void)
{
    GLOG_D("%s", __func__);

    for (uint8_t i = 0; i < CP_PCM_DATA_CACHE_FRAME_NUM; i++)
    {
        EHWctx.micDataInputBuf[i] = NULL;
    }

    osMutexWait(EHWctx.hotwordDataCacheMutexId, osWaitForever);
    EHWctx.hotwordDataCacheBuf = NULL;

    // reset mic data cache queue
    ResetCQueue(&(EHWctx.hotwordDataCacheQueue));
    osMutexRelease(EHWctx.hotwordDataCacheMutexId);
}

void *gsound_custom_hotword_external_get_env(void)
{
    return (void *)&EHWctx;
}

static void _read_model_file(void *modelAddr, MODEL_INFO_T *info)
{
    ASSERT(modelAddr, "invalid model address");
    ASSERT(info, "invalid info pointer");

    uint32_t readValue;
    GSoundHotwordMmapType mType;
    const uint8_t *pRead = (const uint8_t *)modelAddr;

    // model file header version check
    readValue = _read_uint32_big_endian_and_increment(&pRead);
    GLOG_D("model header version:%d", readValue);
    ASSERT(MODEL_HEADER_VERSION == readValue,
           "model file header version parse error:%d",
           readValue);

    // model file library version check
    readValue = _read_uint32_big_endian_and_increment(&pRead);
    GLOG_D("hotword version:%d", readValue);
    ASSERT(((uint32_t)GoogleHotwordVersion()) == readValue,
           "model file library version parse error:%d",
           readValue);

    // length of architecture string length
    readValue = _read_uint32_big_endian_and_increment(&pRead);
    GLOG_D("arch len%d", readValue);
    GLOG_D("arch string:");
    DUMP8("0x%02x ", pRead, readValue);
    ASSERT((readValue == strlen(PLATEFORM_ARCHITECTURE) &&
            memcmp(pRead, PLATEFORM_ARCHITECTURE, readValue) == 0),
           "arch info parse error");
    pRead += ARCHITECTURE_LENGTH;

    // memory bank num check
    info->bankCnt = (uint8_t)_read_uint32_big_endian_and_increment(&pRead);
    ASSERT(MEMORY_BANK_CNT >= info->bankCnt,
           "model file bank cnt parse error");
    GLOG_D("bankCnt:%x", info->bankCnt);

    for (uint8_t i = 0; i < info->bankCnt; i++)
    {
        // memory bank type
        mType = (GSoundHotwordMmapType)_read_uint32_big_endian_and_increment(&pRead);
        GLOG_D("mtype %d", mType);

        // memory bank offset
        readValue = _read_uint32_big_endian_and_increment(&pRead);
        info->bankAddr[i] = (void *)(((uint8_t *)modelAddr) + readValue);
        GLOG_D("memBank address: %p", info->bankAddr[i]);

        // memory bank length
        readValue = _read_uint32_big_endian_and_increment(&pRead);

        switch (mType)
        {
        case GSOUND_HOTWORD_MMAP_TEXT:
            // For TEXT section don't copy to RAM
            info->textIdx = i;
            info->textSize = readValue;
            ASSERT(GSOUND_MAX_HOTWORD_MODEL_TEXT_BUFFER_SIZE >= info->textSize,
                   "model file text bank length error");
            break;
        case GSOUND_HOTWORD_MMAP_DATA:
            info->dataIdx = i;
            info->dataSize = readValue;
            ASSERT(GSOUND_MAX_HOTWORD_MODEL_DATA_BUFFER_SIZE >= info->dataSize,
                   "model file data bank length error");
            break;
        case GSOUND_HOTWORD_MMAP_BSS:
            // Not supported yet. Need to allocate scratch space
            ASSERT(0, "BSS not implemented");
            break;
        default:
            ASSERT(0, "Bad hw mmap");
        }
    }
}

static void _load_data_and_init_lib(MODEL_INFO_T *info)
{
    static void *memBank[MEMORY_BANK_CNT];

    // current model is not the incoming model
    if (memcmp(info->modelId, EHWctx.activeModelId, GSOUND_HOTWORD_MODEL_ID_BYTES))
    {
        // update the local saved active model info
        memcpy(EHWctx.activeModelId, info->modelId, GSOUND_HOTWORD_MODEL_ID_BYTES);

        // copy model data into ram
        memcpy((uint8_t *)EHWctx.hotwordModelData, (uint8_t *)info->bankAddr[info->dataIdx], info->dataSize);
        memcpy((uint8_t *)EHWctx.hotwordModelText, (uint8_t *)info->bankAddr[info->textIdx], info->textSize);

        memBank[info->dataIdx] = (void *)EHWctx.hotwordModelData;
        memBank[info->textIdx] = (void *)EHWctx.hotwordModelText;

        for (uint8_t i = 0; i < info->bankCnt; i++)
        {
            GLOG_D("bank Addr:%p", memBank[i]);
        }

        // init hotword detection engine
        EHWctx.mHandle = GoogleHotwordDspMultiBankInit((void **)memBank, info->bankCnt);
        ASSERT(EHWctx.mHandle, "libgsound init error");
    }
    else
    {
        GLOG_W("model already inited before");
    }
}

void gsound_custom_hotword_external_init_process(const char *model_id,
                                                 uint8_t model_length)
{
    void *modelAddr = NULL;
    MODEL_INFO_T modelInfo;
    memset((uint8_t *)&modelInfo, 0, sizeof(modelInfo));

    if (0 == model_length || NULL == model_id)
    {
        GLOG_W("all model will be disabled");

        /// clear out its active model disabling hotword until a valid model is provided
        memcpy(EHWctx.activeModelId, INVALID_MODEL_ID, GSOUND_HOTWORD_MODEL_ID_BYTES);
    }
    else
    {
        modelAddr = (void *)nv_record_gsound_rec_get_hotword_model_addr(model_id, false, 0);
        GLOG_D("modelHeader addr:%p", modelAddr);
        ASSERT(modelAddr, "INVALID model address");

        memcpy(modelInfo.modelId, model_id, GSOUND_HOTWORD_MODEL_ID_BYTES);
        _read_model_file(modelAddr, &modelInfo);
        _load_data_and_init_lib(&modelInfo);
    }
}

void gsound_custom_hotword_external_handle_mic_data(uint8_t *ptrBuf, uint32_t length, uint8_t *outputPtr)
{
    ASSERT(VOICE_SBC_PCM_DATA_SIZE_PER_FRAME >= length, "invalid length:%d", length);

    // configure the global param which will be used in co-processor
    uint8_t cacheIdx = EHWctx.micDataCacheIdx;
    if (EHWctx.micDataInputBuf[cacheIdx])
    {
        memcpy(EHWctx.micDataInputBuf[cacheIdx], ptrBuf, length);
        EHWctx.micDataInputLen[cacheIdx] = length;
        EHWctx.micDataOutputBuf = outputPtr;
        EHWctx.micDataStatus[cacheIdx] = HW_MIC_DATA_CACHED;
        EHWctx.micDataCacheIdx++;

        if (EHWctx.micDataCacheIdx >= CP_PCM_DATA_CACHE_FRAME_NUM)
        {
            EHWctx.micDataCacheIdx = 0;
        }

        GLOG_V("data cached, cacheIdx:%d, processIdx:%d", cacheIdx, EHWctx.micDataProcessIdx);

        cp_accel_send_event_mcu2cp(CP_BUILD_ID(CP_TASK_HW, CP_EVENT_HW_PROCESSING));
    }
    else
    {
        EHWctx.micDataInputLen[cacheIdx] = 0;
        GLOG_W("mic data cache buffer is NULL, will drop the data");
    }
}

CP_TEXT_SRAM_LOC
static unsigned int _cp_hw_main(uint8_t event)
{
    uint8_t processIdx;

    GLOG_V("cache idx:%d", EHWctx.micDataCacheIdx);
    GLOG_V("process idx:%d", EHWctx.micDataProcessIdx);

#ifdef HW_TRACE_CP_PROCESS_TIME
    uint32_t stime;
    uint32_t etime;

    stime = hal_fast_sys_timer_get();
#endif

    switch (event)
    {
    case CP_EVENT_HW_PROCESSING:
        for (uint8_t idx = 0; idx < CP_PCM_DATA_CACHE_FRAME_NUM; idx++)
        {
            processIdx = EHWctx.micDataProcessIdx;
            if (HW_MIC_DATA_CACHED == EHWctx.micDataStatus[processIdx])
            {
                gsound_custom_audio_handle_mic_data(EHWctx.micDataInputBuf[processIdx],
                                                    EHWctx.micDataInputLen[processIdx],
                                                    EHWctx.micDataOutputBuf);

                EHWctx.micDataStatus[processIdx] = HW_MIC_DATA_IDLE;
                EHWctx.micDataProcessIdx++;

                if (EHWctx.micDataProcessIdx >= CP_PCM_DATA_CACHE_FRAME_NUM)
                {
                    EHWctx.micDataProcessIdx = 0;
                }
            }
        }
        break;
    default:
        break;
    }

#ifdef HW_TRACE_CP_PROCESS_TIME
    etime = hal_fast_sys_timer_get();
    GLOG_D("CP handle time: %5u us in %5u us",
           FAST_TICKS_TO_US(etime - stime),
           FAST_TICKS_TO_US(etime - cp_last_process_time));
    cp_last_process_time = etime;
#endif

    return 0;
}

static unsigned int _mcu_hw_evt_handler(uint8_t msg)
{
    uint8_t event = CP_EVENT_GET(msg);
    GLOG_V("received cp event:%d", event);

    switch (event)
    {
    case MCU_EVENT_HW_DETECTED:
        // inform libgsound hotword detected, and wait for libgsound call @see GSoundTargetHotwordRequestForData
        EHWctx.hotwordInterface->gsound_on_hotword_detected(gsound_custom_audio_get_bitpool());
        break;

    case MCU_EVENT_HW_NEW_DATA_AVAILABLE:
        // inform libgsound that new data is coming in, and wait for libgsound call @see GSoundTargetHotwordRequestForData
        EHWctx.hotwordInterface->gsound_on_hotword_data_available(EHWctx.newAvailableDataLen);
        break;

    case MCU_EVENT_HW_UPSTREAM_DATA:
        gsound_custom_audio_transmit_data_to_libgsound();
        break;

#ifdef VOICE_DETECTOR_EN
    case MCU_EVENT_HW_START_VAD:
        if (app_voicepath_get_vad_audio_capture_start_flag())
        {
            app_voicepath_set_vad_audio_capture_start_flag(0);
            app_voice_detector_send_event(VOICE_DETECTOR_ID_0, VOICE_DET_EVT_VAD_START);
        }
        break;
#endif

    default:
        GLOG_E("invalid event received:%d", event);
        break;
    }

    return 0;
}

int gsound_custom_hotword_external_stop_cp_process(void)
{
    GLOG_I("external hotword stop the mic data processing in cp");
    cp_accel_close(CP_TASK_HW);

    return 0;
}

void gsound_custom_hotword_external_start_detection(const GSoundTargetAudioInSettings *audio_config,
                                                    const GSoundTargetHotwordConfig *hw_config)
{
    // parameter validity check
    ASSERT(hw_config->audio_backlog_ms <= GSOUND_TARGET_MAX_HOTWORD_BUF_LEN_MS, "audio_backlog_ms exceed.");

    // allow bt enter sniff mode(since hotword detection is locally, no need to transmit audio data to APP)
    gsound_custom_bt_allow_sniff();

    memcpy((void *)&EHWctx.hwConfig,
           (void *)hw_config,
           sizeof(GSoundTargetHotwordConfig));
    GLOG_I("audio_backlog_ms is set to:%d", EHWctx.hwConfig.audio_backlog_ms);

    while (HW_BUF_STATUS_IDLE != EHWctx.hotwordBufStatus)
    {
        hal_sys_timer_delay_us(10);
    }
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_PROCESSING;
    osMutexWait(EHWctx.hotwordDataCacheMutexId, osWaitForever);
    // Queue size is calculated according to EHWctx.hwConfig.audio_backlog_ms
    ResetCQueue(&(EHWctx.hotwordDataCacheQueue));
    osMutexRelease(EHWctx.hotwordDataCacheMutexId);
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_IDLE;

    EHWctx.micDataCacheIdx = 0;
    EHWctx.micDataProcessIdx = 0;
    EHWctx.micDataOutputBuf = NULL;
    for (uint8_t i = 0; i < CP_PCM_DATA_CACHE_FRAME_NUM; i++)
    {
        EHWctx.micDataStatus[i] = HW_MIC_DATA_IDLE;
    }

    // set hotword status to DETECT_ONGOING
    // start hotword detection progress
    EHWctx.hotwordDetectStatus = HW_DETECT_STATUS_ONGOING;

    GoogleHotwordDspMultiBankReset(EHWctx.mHandle);

    // open mic and start capture stream
    gsound_custom_audio_open_mic(audio_config->sbc_bitpool);

    // store audio_config
    gsound_custom_audio_store_audio_in_settings(audio_config);
}

void gsound_custom_hotword_external_request_data(uint32_t max_num_bytes,
                                                 uint32_t bytes_per_frame,
                                                 GSoundTargetHotwordDataMsg *sync_hw_data_out,
                                                 bool *ready_now_out)
{
    GLOG_V("[%s]", __func__);
    uint32_t frameLen = gsound_custom_audio_get_output_sbc_frame_len();

    GLOG_V("max required data len:%d", max_num_bytes);
    ASSERT(bytes_per_frame == frameLen,
           "SBC frame size is not expected:%d/%d",
           frameLen,
           bytes_per_frame);

    int maxOutput = max_num_bytes / bytes_per_frame;
    maxOutput *= bytes_per_frame;
    unsigned int len1, len2;

    while (HW_BUF_STATUS_IDLE != EHWctx.hotwordBufStatus)
    {
        hal_sys_timer_delay_us(10);
    }
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_PROCESSING;
    osMutexWait(EHWctx.hotwordDataCacheMutexId, osWaitForever);

    // libgsound ask for first packet data
    if (HW_DETECT_STATUS_START_UPSTREAM == EHWctx.hotwordDetectStatus)
    {
        // calculate the libgsound required data length
        double frameCntPerMs = gsound_custom_audio_get_sbc_frame_cnt_per_ms();
        uint32_t dataCnt = ((uint32_t)(frameCntPerMs * EHWctx.preambleMs)) * frameLen;
        GLOG_V("%d bytes data is required from libgsound, cached data length:%d",
               dataCnt,
               LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)));

        // make sure cached data is enough for libgsound required
        if (dataCnt > (uint32_t)LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)))
        {
            dataCnt -= LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue));
            uint16_t emptyFrameNum = dataCnt / ARRAY_SIZE(emptyFrame);
            if (dataCnt % ARRAY_SIZE(emptyFrame))
            {
                emptyFrameNum++;
            }

            GLOG_D("%d frames empty data enqueued", emptyFrameNum);
            for (uint16_t i = 0; i < emptyFrameNum; i++)
            {
                // enqueue empty data to head of the queue
                ASSERT(CQ_OK == EnCQueueFront(&(EHWctx.hotwordDataCacheQueue), (CQItemType *)&emptyFrame, ARRAY_SIZE(emptyFrame)),
                       "%s EnCQueueFront error",
                       __func__);
            }
        }
        else
        {
            // calculate the redundant data length and drop them
            dataCnt = LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)) - dataCnt;
            DeCQueue(&(EHWctx.hotwordDataCacheQueue), NULL, dataCnt);
            GLOG_D("dropped %d cached data, left data length:%d",
                   dataCnt,
                   LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)));
        }
    }

    sync_hw_data_out->bytes_per_frame = frameLen;

    // data cache buffer is empty
    if (CQ_OK == IsEmptyCQueue(&(EHWctx.hotwordDataCacheQueue)))
    {
        GLOG_W("hotword data cache buffer is empty");
        sync_hw_data_out->data1 = NULL;
        sync_hw_data_out->data2 = NULL;
        sync_hw_data_out->num_bytes1 = 0;
        sync_hw_data_out->num_bytes2 = 0;
        *ready_now_out = false;

        // mark buffer status, so that we can inform libgsound when new data come in
        EHWctx.isEmptyLastTime = true;
    }
    // data cache buffer is not empty
    else
    {
        maxOutput = (maxOutput < LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)))
                        ? maxOutput
                        : LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue));

        // send data to libgsound
        ASSERT(CQ_ERR != PeekCQueue(&(EHWctx.hotwordDataCacheQueue),
                                    maxOutput,
                                    (CQItemType **)&(sync_hw_data_out->data1),
                                    &len1,
                                    (CQItemType **)&(sync_hw_data_out->data2),
                                    &len2),
               "peek queue error");

        sync_hw_data_out->num_bytes1 = len1;
        sync_hw_data_out->num_bytes2 = len2;
        *ready_now_out = true;

        GLOG_V("num_bytes1:%d, num_bytes2:%d, bytes_per_frame:%d",
               sync_hw_data_out->num_bytes1,
               sync_hw_data_out->num_bytes2,
               sync_hw_data_out->bytes_per_frame);
    }

    osMutexRelease(EHWctx.hotwordDataCacheMutexId);
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_IDLE;

    EHWctx.hotwordDetectStatus = HW_DETECT_STATUS_UPSTREAMING;
}

void gsound_custom_hotword_data_consumed(GSoundTargetHotwordDataMsg *hw_data)
{
    GLOG_V("data1 consumed:%d, data2 consumed:%d, bytes_per_frame:%d",
           hw_data->num_bytes1, hw_data->num_bytes2, hw_data->bytes_per_frame);

    // ASSERT(0 == ((hw_data->num_bytes1 + hw_data->num_bytes2) % hw_data->bytes_per_frame),
    //        "consumed bytes should be integer multiples of bytes_per_frame");

    while (HW_BUF_STATUS_IDLE != EHWctx.hotwordBufStatus)
    {
        hal_sys_timer_delay_us(10);
    }
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_PROCESSING;
    osMutexWait(EHWctx.hotwordDataCacheMutexId, osWaitForever);
    DeCQueue(&(EHWctx.hotwordDataCacheQueue), NULL, hw_data->num_bytes1 + hw_data->num_bytes2);
    osMutexRelease(EHWctx.hotwordDataCacheMutexId);
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_IDLE;
}

void gsound_custom_hotword_get_lib_info(GSoundHotwordLibInfo *info_out)
{
    int version = GoogleHotwordVersion();
    GLOG_D("version is %d", version);

    sprintf(EHWctx.libVersionStr, "%d", version);

    info_out->hotword_library_version = (const char *)EHWctx.libVersionStr;
    info_out->version_length = strlen(EHWctx.libVersionStr);
    GLOG_D("version is %s, strLen is %d", info_out->hotword_library_version, info_out->version_length);

    info_out->hotword_library_architecture = PLATEFORM_ARCHITECTURE;
    info_out->arch_length = strlen(PLATEFORM_ARCHITECTURE);

    nv_record_gsound_rec_get_supported_model_id(&(info_out->supported_models),
                                                &(info_out->supported_model_length));

    if (memcmp(EHWctx.activeModelId, INVALID_MODEL_ID, GSOUND_HOTWORD_MODEL_ID_BYTES))
    {
        info_out->active_model = (const char *)EHWctx.activeModelId;
        info_out->active_model_length = GSOUND_HOTWORD_MODEL_ID_BYTES;
        GLOG_D("activated model:");
        DUMP8("0x%02x ", info_out->active_model, info_out->active_model_length);
    }
    else
    {
        info_out->active_model = NULL;
        info_out->active_model_length = 0;
        GLOG_D("no activated model");
    }
}

int gsound_custom_hotword_external_stop_stream(void)
{
    int ret = 0;

    GLOG_I("current hotword detection status:%d", EHWctx.hotwordDetectStatus);

    if (HW_DETECT_STATUS_UPSTREAMING == EHWctx.hotwordDetectStatus)
    {
        // enable the local mic data processing again
        EHWctx.hotwordDetectStatus = HW_DETECT_STATUS_ONGOING;

        while (HW_BUF_STATUS_IDLE != EHWctx.hotwordBufStatus)
        {
            osDelay(5);
        }
        EHWctx.hotwordBufStatus = HW_BUF_STATUS_PROCESSING;
        osMutexWait(EHWctx.hotwordDataCacheMutexId, osWaitForever);
        // clear the hotword data cache buffer
        DeCQueue(&(EHWctx.hotwordDataCacheQueue), NULL, LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)));
        GLOG_I("hotword data cache queue cleared, current len:%d", LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)));
        osMutexRelease(EHWctx.hotwordDataCacheMutexId);
        EHWctx.hotwordBufStatus = HW_BUF_STATUS_IDLE;

        EHWctx.micDataCacheIdx = 0;
        EHWctx.micDataProcessIdx = 0;
        EHWctx.micDataOutputBuf = NULL;
        for (uint8_t i = 0; i < CP_PCM_DATA_CACHE_FRAME_NUM; i++)
        {
            EHWctx.micDataStatus[i] = HW_MIC_DATA_IDLE;
        }
    }
    else
    {
        ret = -1;
        ASSERT(0, "hotword detect status error.");
    }

    return ret;
}

void gsound_custom_hotword_external_stop_detection(bool voiceReady)
{
    // just encode the data and wait libgsound call
    // @see GSoundTargetAudioInOpen pass data to libgsound
    if (voiceReady)
    {
        GLOG_I("stop detection but not stop the stream");
    }
    // close the mic data capture
    else
    {
        gsound_custom_audio_close_mic();
    }

    while (HW_BUF_STATUS_IDLE != EHWctx.hotwordBufStatus)
    {
        hal_sys_timer_delay_us(10);
    }
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_PROCESSING;
    osMutexWait(EHWctx.hotwordDataCacheMutexId, osWaitForever);
    // clear the hotword data cache buffer
    DeCQueue(&(EHWctx.hotwordDataCacheQueue), NULL, LengthOfCQueue(&(EHWctx.hotwordDataCacheQueue)));
    osMutexRelease(EHWctx.hotwordDataCacheMutexId);
    EHWctx.hotwordBufStatus = HW_BUF_STATUS_IDLE;

    EHWctx.hotwordDetectStatus = HW_DETECT_STATUS_IDLE;

    EHWctx.micDataCacheIdx = 0;
    EHWctx.micDataProcessIdx = 0;
    EHWctx.micDataOutputBuf = NULL;
    for (uint8_t i = 0; i < CP_PCM_DATA_CACHE_FRAME_NUM; i++)
    {
        EHWctx.micDataStatus[i] = HW_MIC_DATA_IDLE;
    }
}

void gsound_custom_hotword_external_store_active_model(const char *model_id,
                                                       uint8_t model_length)
{
    GLOG_I("%s", __func__);

    if (0 == model_length || NULL == model_id)
    {
        GLOG_I("disable all models");

        // disable all hotword models
        memcpy(EHWctx.activeModelId, INVALID_MODEL_ID, GSOUND_HOTWORD_MODEL_ID_BYTES);
        EHWctx.hotwordDetectStatus = HW_DETECT_STATUS_IDLE;
    }
    else
    {
        GLOG_I("activate model id(len %d): %s", model_length, model_id);

        // TODO: parse the incoming param, save the glib configuration and enable corresponding models
    }
}

void gsound_custom_hotword_external_store_interface(const GSoundHotwordInterface *handlers)
{
    EHWctx.hotwordInterface = handlers;
}

CP_TEXT_SRAM_LOC
void gsound_custom_hotword_external_process_data(uint8_t *rawDataBuf,
                                                 uint32_t rawDataLen,
                                                 uint8_t *encodedDataBuf,
                                                 uint32_t encodedDataLen,
                                                 bool upstreamFlag)
{
    GLOG_V("[%s]+++", __func__);
    static uint32_t hotwordIdleCycle = 0;
    uint32_t availableCacheQueue = 0;
    uint32_t removeLen = 0;
    uint32_t frameLen = gsound_custom_audio_get_output_sbc_frame_len();

    // just transmit data to libgsound
    if (upstreamFlag)
    {
        GLOG_I("upstream data to libgsound");
        cp_accel_send_event_cp2mcu(CP_BUILD_ID(CP_TASK_HW, MCU_EVENT_HW_UPSTREAM_DATA));
    }
    else
    {
        if (HW_DETECT_STATUS_IDLE != EHWctx.hotwordDetectStatus)
        {
            while (HW_BUF_STATUS_IDLE != EHWctx.hotwordBufStatus)
            {
                hal_sys_timer_delay_us(10);
            }

            EHWctx.hotwordBufStatus = HW_BUF_STATUS_CACHING;
            availableCacheQueue = AvailableOfCQueue(&(EHWctx.hotwordDataCacheQueue));

            // throw the earliest cached data, if the queue is not enough
            if (availableCacheQueue < encodedDataLen)
            {
                if (HW_DETECT_STATUS_UPSTREAMING == EHWctx.hotwordDetectStatus)
                {
                    GLOG_W("hotword data cache buff exceed!");
                }

                removeLen = (encodedDataLen - availableCacheQueue) / frameLen * frameLen;
                if (removeLen != (encodedDataLen - availableCacheQueue))
                {
                    removeLen += frameLen;
                }

                GLOG_V("will remove %d bytes", encodedDataLen - availableCacheQueue);
                DeCpQueue(&(EHWctx.hotwordDataCacheQueue), NULL, encodedDataLen - availableCacheQueue);
            }

            EnCpQueue(&(EHWctx.hotwordDataCacheQueue), encodedDataBuf, encodedDataLen);

            //// detecet hotword
            // local detection
            if (HW_DETECT_STATUS_ONGOING == EHWctx.hotwordDetectStatus)
            {
                if (EHWctx.mHandle)
                {
                    // hotword local detected
                    // NOTE: should use raw PCM data according to BISTO team
                    if (GoogleHotwordDspMultiBankProcess((const void *)rawDataBuf,
                                                         rawDataLen / VOICE_SBC_SIZE_PER_SAMPLE,
                                                         &EHWctx.preambleMs,
                                                         EHWctx.mHandle))
                    {
                        GLOG_D("external hotword detected");

                        // update the hotword detect status
                        EHWctx.hotwordDetectStatus = HW_DETECT_STATUS_START_UPSTREAM;
                        hotwordIdleCycle = 0;
                        cp_accel_send_event_cp2mcu(CP_BUILD_ID(CP_TASK_HW, MCU_EVENT_HW_DETECTED));
                    }
                    else
                    {
                        hotwordIdleCycle++;

#ifdef VOICE_DETECTOR_EN
                        if (hotwordIdleCycle % 300 == 0)
                        {
                            ResetCpQueue(&(EHWctx.hotwordDataCacheQueue));
                            cp_accel_send_event_cp2mcu(CP_BUILD_ID(CP_TASK_HW, MCU_EVENT_HW_START_VAD));
                        }
#endif
                    }
                }
                else
                {
                    GLOG_W("glib memory handle is null");
                }
            }
            // mic data upstreaming
            else if (HW_DETECT_STATUS_UPSTREAMING == EHWctx.hotwordDetectStatus)
            {
                if (EHWctx.isEmptyLastTime)
                {
                    // update the cache buffer status
                    EHWctx.isEmptyLastTime = false;

                    EHWctx.newAvailableDataLen = encodedDataLen;
                    cp_accel_send_event_cp2mcu(CP_BUILD_ID(CP_TASK_HW, MCU_EVENT_HW_NEW_DATA_AVAILABLE));
                }
            }

            EHWctx.hotwordBufStatus = HW_BUF_STATUS_IDLE;
            GLOG_V("hotword detect status:%d", EHWctx.hotwordDetectStatus);
        }
        else
        {
            GLOG_I("will not cache hotword data because detect status is idle");
        }
    }

    GLOG_V("[%s]---", __func__);
}