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

/*****************************header include********************************/
#include "norflash_drv.h"
#include "norflash_api.h"
#include "gsound_dbg.h"
#include "gsound_custom.h"
#include "nvrecord_ota.h"
#include "nvrecord_gsound.h"
#include "gsound_custom_ota.h"

#ifdef IBRT
#include "app_tws_ibrt_cmd_handler.h"
#include "app_ibrt_ota_cmd.h"
#include "app_tws_ctrl_thread.h"
#include "app_tws_if.h"
#endif

/************************private macro defination***************************/
#define FORCE_APPLY_IMMEDIATELY 1

/************************private type defination****************************/

/**********************private function declearation************************/
static void _init_handler(void);

/************************private variable defination************************/
#ifdef GSOUND_HOTWORD_ENABLED
extern uint32_t __hotword_model_start[];
extern uint32_t __hotword_model_end[];
#endif

static GSOTA_ENV_T gsOtaEnv;

/****************************function defination****************************/
/**
 * @brief BISTO specific handlings on OTA_BEGIN command received.
 * 
 * must in type of @see OTA_CMD_HANDLER_T
 * this function will be registored to ota_common layer to hanle BISTO specific
 * features
 * 
 * @param cmd           pointer of info related with OTA_BEGIN command
 * @param len           length of info related with OTA_BEGIN command
 * @return OTA_STATUS_E status of OTA_BEGIN command execution
 */
static OTA_STATUS_E _begin(const void *cmd, uint16_t len)
{
    OTA_STATUS_E status = OTA_STATUS_OK;

    /// enable the sanity check
    if (OTA_DEVICE_APP == gsOtaEnv.common->deviceId)
    {
        ota_common_enable_sanity_check(true);
    }
    else
    {
        ota_common_enable_sanity_check(false);
    }

    // OTA_BEGIN_PARAM_T* p = (OTA_BEGIN_PARAM_T*) cmd;
    // GSOTA_BEGIN_PARAM_T *pBegin = (GSOTA_BEGIN_PARAM_T *)p->customize;

#ifdef IBRT
    if (IBRT_MASTER == app_tws_ibrt_role_get_callback(NULL))
    {
        status = ota_common_relay_data_to_peer(OTA_COMMAND_BEGIN, (const uint8_t *)cmd, len);

        if (OTA_STATUS_OK == status)
        {
            status = ota_common_receive_peer_rsp();
        }
    }
#endif

    return status;
}

/**
 * @brief BISTO specific handlings on OTA_DATA command received.
 * 
 * must in type of @see OTA_CMD_HANDLER_T
 * this function will be registored to ota_common layer to hanle BISTO specific
 * features
 * 
 * @param cmd           pointer of info related with OTA_DATA command
 * @param len           length of info related with OTA_DATA command
 * @return OTA_STATUS_E status of OTA_DATA command execution
 */
static OTA_STATUS_E _data(const void *cmd, uint16_t len)
{
    OTA_STATUS_E status = OTA_STATUS_OK;
    OTA_DATA_PARAM_T *p = (OTA_DATA_PARAM_T *)cmd;
    GSOTA_DATA_PARAM_T *pData = (GSOTA_DATA_PARAM_T *)p->customize;
    const uint8_t *data = (const uint8_t *)p->data;
    uint16_t length = p->len;

#ifdef IBRT
    if (IBRT_MASTER == app_tws_ibrt_role_get_callback(NULL))
    {
        status = ota_common_relay_data_to_peer(OTA_COMMAND_DATA, data, length);
#endif

        if (OTA_STATUS_OK == status)
        {
            /// write data info flash
            status = ota_common_fw_data_write(data, length);
        }
        else
        {
            GLOG_W("relay data failed");
        }

#ifdef IBRT
        if (OTA_STATUS_OK == status)
        {
            status = ota_common_receive_peer_rsp();
        }
#endif

        /// update the ota status to libgsound
        OTA_STAGE_E stage = (OTA_STAGE_E)gsOtaEnv.common->currentStage;
        if (OTA_STAGE_DONE == stage)
        {
            pData->status->ota_done = true;
        }
        else
        {
            pData->status->ota_done = false;
        }

#ifdef IBRT
    }
    else //!< slave
    {
        /// write data info flash
        status = ota_common_fw_data_write(data, length);
    }
#endif

    return status;
}

/**
 * @brief BISTO specific handlings on OTA_APPLY command received.
 * 
 * must in type of @see OTA_CMD_HANDLER_T
 * this function will be registored to ota_common layer to hanle BISTO specific
 * features
 * 
 * @param cmd           pointer of info related with OTA_APPLY command
 * @param len           length of info related with OTA_APPLY command
 * @return OTA_STATUS_E status of OTA_APPLY command execution
 */
static OTA_STATUS_E _apply(const void *cmd, uint16_t len)
{
    OTA_STATUS_E status = OTA_STATUS_OK;

    POSSIBLY_UNUSED OTA_APPLY_PARAM_T *p = (OTA_APPLY_PARAM_T *)cmd;
    // GSOTA_APPLY_PARAM_T *pApply = (GSOTA_APPLY_PARAM_T *)p->customize;

#ifdef IBRT
    if (IBRT_MASTER == app_tws_ibrt_role_get_callback(NULL))
    {
        status = ota_common_relay_data_to_peer(OTA_COMMAND_APPLY, (const uint8_t *)cmd, len);

        if (OTA_STATUS_OK == status)
        {
            status = ota_common_receive_peer_rsp();
        }
    }
#endif

    if (OTA_STATUS_OK == status)
    {
        if (OTA_DEVICE_APP == gsOtaEnv.common->deviceId)
        {
#if FORCE_APPLY_IMMEDIATELY
            ota_common_apply_current_fw();
#else
            if (p->applyNow)
            {
                ota_common_apply_current_fw();
            }
            else
            {
                GLOG_I("will not apply because of the glib configuration");
            }
#endif
        }
#ifdef GSOUND_HOTWORD_ENABLED
        else
        {
            GLOG_I("apply hotword model");
            HOTWORD_MODEL_INFO_T info;
            memcpy(info.modelId, gsOtaEnv.common->version, ARRAY_SIZE(info.modelId));
            GLOG_I("model id:");
            DUMP8("0x%02x ", info.modelId, 4);

            info.startAddr = gsOtaEnv.common->newImageFlashOffset;
            GLOG_I("model flash offset:0x%x", info.startAddr);

            info.len = gsOtaEnv.common->totalImageSize;
            GLOG_I("model len:0x%x", info.len);

            nv_record_gsound_rec_add_new_model(&info);
        }
#endif
    }

    return status;
}

/**
 * @brief BISTO specific handlings on OTA_ABORT command received.
 * 
 * must in type of @see OTA_CMD_HANDLER_T
 * this function will be registored to ota_common layer to hanle BISTO specific
 * features
 * 
 * @param cmd           pointer of info related with OTA_ABORT command
 * @param len           length of info related with OTA_ABORT command
 * @return OTA_STATUS_E status of OTA_ABORT command execution
 */
static OTA_STATUS_E _abort(const void *cmd, uint16_t len)
{
    OTA_STATUS_E status = OTA_STATUS_OK;

    // OTA_ABORT_PARAM_T *p = (OTA_ABORT_PARAM_T *)cmd;
    // GSOTA_ABORT_PARAM_T *pAbort = (GSOTA_ABORT_PARAM_T *)p->customize;

#ifdef IBRT
    if (IBRT_MASTER == app_tws_ibrt_role_get_callback(NULL))
    {
        status = ota_common_relay_data_to_peer(OTA_COMMAND_ABORT, (const uint8_t *)cmd, len);

        if (OTA_STATUS_OK == status)
        {
            status = ota_common_receive_peer_rsp();
        }
    }
#endif

    return status;
}

#ifdef IBRT
/**
 * @brief BISTO specific relay needed check.
 * 
 * @return true         relay is needed
 * @return false        relay is not needed
 */
static bool _relay_needed(void)
{
    bool ret = true;

    if (gsOtaEnv.common)
    {
        /// hotword model is allowed solo update according to BISTO team
        if ((OTA_USER_COLORFUL == gsOtaEnv.common->currentUser) &&
            (GSOUND_TARGET_OTA_DEVICE_INDEX_FULL_FIRMWARE == gsOtaEnv.device))
        {
            if (!app_tws_ibrt_tws_link_connected())
            {
                GLOG_I("not relay, reason: tws_disconnected");
                ret = false;
            }
        }
    }
    else
    {
        GLOG_I("not relay, reason: null common pointer");
        ret = false;
    }

    return ret;
}

/**
 * @brief Peer command received handler for GSound OTA
 * 
 * @param cmdType       received command type
 * @param data          pointer of received data
 * @param length        length of received data
 * @return OTA_STATUS_E Excution result of handling received peer command
 */
static OTA_STATUS_E _peer_cmd_recevied_handler(OTA_COMMAND_E cmdType,
                                               const uint8_t *data,
                                               uint16_t length)
{
    OTA_DATA_PARAM_T dataCmd;
    void *cmdInfo = (void *) data;
    uint16_t cmdLen = length;
    OTA_STATUS_E status = OTA_STATUS_OK;

    switch (cmdType)
    {
    case OTA_COMMAND_BEGIN:
        _init_handler();
        break;

    /// convert the data command into low layer expected format
    case OTA_COMMAND_DATA:
        dataCmd.offset = gsOtaEnv.common->newImageProgramOffset;
        dataCmd.data = data;
        dataCmd.len = length;
        dataCmd.customizeLen = 0;
        dataCmd.customize = NULL;
        cmdInfo = (void *)&dataCmd;
        cmdLen = sizeof(dataCmd);
        break;

    default:
        break;
    }

    // pass the parameter to ota_common layer to handle
    status = ota_common_command_received_handler(cmdType,
                                                 cmdInfo,
                                                 cmdLen);

    return status;
}
#endif

/**
 * @brief BISTO OTA init handler.
 * 
 * This function is used to initialize the BISTO OTA context, should be called
 * everytime OTA_BEGIN command is received.
 * 
 */
static void _init_handler(void)
{
    memset(&gsOtaEnv, 0, sizeof(gsOtaEnv));
    gsOtaEnv.common = ota_common_get_env();

#ifdef GSOUND_HOTWORD_ENABLED
    GLOG_I("hotword model start:%p, hotword model end:%p", __hotword_model_start, __hotword_model_end);
    /// init hotword used flash module
    ota_common_init_flash((uint8_t)NORFLASH_API_MODULE_ID_HOTWORD_MODEL,
                          (uint32_t)__hotword_model_start,
                          (uint32_t)__hotword_model_end - (uint32_t)__hotword_model_start,
                          (uint32_t)NULL);
#endif

    /// registor the customized OTA command handler
    ota_common_registor_command_handler(OTA_COMMAND_BEGIN, (void *)_begin);
    ota_common_registor_command_handler(OTA_COMMAND_DATA, (void *)_data);
    ota_common_registor_command_handler(OTA_COMMAND_APPLY, (void *)_apply);
    ota_common_registor_command_handler(OTA_COMMAND_ABORT, (void *)_abort);

#ifdef IBRT
    /// registor the peer cmd received handler for data relay
    ota_common_registor_peer_cmd_received_handler((void*)_peer_cmd_recevied_handler);

    /// registor the customized relay needed check handler
    ota_common_registor_relay_needed_handler((void *)_relay_needed);
#endif
}

/**
 * @brief OTA command received handler
 * 
 * This function is used to handle OTA commands in format of ota_common level,
 * and pass these commands to ota_common layer to handle, then convert the
 * return status of ota_common format into OTA user-specific status.
 * NOTE: 1. should convert OTA user-specific infomations into ota_common format
 *          before calling this function
 *       2. designing this function instead of passing ota_common info to
 *          ota_common layer directly is to separate OTA spec layer from
 *          ota_common
 * 
 * @param cmdType       command type of ota_common layer
 * @param cmdInfo       pointer of command info
 * @param cmdLen        length of command info
 * @return GSoundStatus command handling result in OTA spec format
 */
static GSoundStatus _gsound_ota_command_handler(OTA_COMMAND_E cmdType,
                                                void *cmdInfo,
                                                uint16_t cmdLen)
{
    GSoundStatus ret = GSOUND_STATUS_OK;

    // pass the parameter to ota_common layer to handle
    OTA_STATUS_E status = ota_common_command_received_handler(cmdType,
                                                              cmdInfo,
                                                              cmdLen);

    // translate result from ota_common layer format into bisto layer format
    switch (status)
    {
    case OTA_STATUS_OK:
        ret = GSOUND_STATUS_OK;
        break;

    case OTA_STATUS_ERROR:
        ret = GSOUND_STATUS_ERROR;
        break;

    case OTA_STATUS_ERROR_RELAY_TIMEOUT:
        ret = GSOUND_STATUS_OTA_ERROR_OTHER;
        break;

    case OTA_STATUS_ERROR_CHECKSUM:
        ret = GSOUND_STATUS_OTA_ERROR_INCORRECT_CHECKSUM;
        break;

    default:
        ret = GSOUND_STATUS_ERROR;
        break;
    }

    return ret;
}

bool gsound_custom_ota_is_receiving_other(void)
{
    bool ret = false;

    if (ota_common_is_in_progress() &&
        (OTA_USER_COLORFUL != gsOtaEnv.common->currentUser))
    {
        ret = true;
    }

    return ret;
}

GSoundStatus gsound_custom_ota_begin(const GSoundOtaSessionSettings *settings)
{
    GSoundStatus ret = GSOUND_STATUS_OK;

    ///update the OTA device for
    gsOtaEnv.device = settings->device_index;

    OTA_BEGIN_PARAM_T otaBeginParam;

    // set path info
    uint8_t path = gsound_custom_get_connection_path();
    if (CONNECTION_BLE == path)
    {
        otaBeginParam.path = OTA_PATH_BLE;
    }
    else if (CONNECTION_BT == path)
    {
        otaBeginParam.path = OTA_PATH_BT;
    }
    else
    {
        ASSERT(0, "invalid ota path in %s", __func__);
    }

    // set OTA user to Google
    otaBeginParam.user = OTA_USER_COLORFUL;

    // set device ID info and the flash offset to write the new image
#ifdef GSOUND_HOTWORD_ENABLED
    if (GSOUND_TARGET_OTA_DEVICE_INDEX_HOTWORD_MODEL == settings->device_index)
    {
        otaBeginParam.device = OTA_DEVICE_HOTWORD;
        otaBeginParam.flashOffset = nv_record_gsound_rec_get_hotword_model_addr(settings->ota_version, true, settings->total_size);
        otaBeginParam.flashOffset = (0xFFFFFF & otaBeginParam.flashOffset);
    }
    else
#endif
        if (GSOUND_TARGET_OTA_DEVICE_INDEX_FULL_FIRMWARE == settings->device_index)
    {
        otaBeginParam.device = OTA_DEVICE_APP;
        otaBeginParam.flashOffset = NEW_IMAGE_FLASH_OFFSET;
    }
    else
    {
        ASSERT(0, "invalid device_index received in %s", __func__);
    }

    if (otaBeginParam.flashOffset)
    {
        /// initializing the OTA context
        _init_handler();

        /// registor the gsound OTA initializor for peer initializing the OTA
        /// context, will not be usful for non-relay system
        otaBeginParam.initializer = (CUSTOM_INITIALIZER_T)_init_handler;

        /// set image size info
        otaBeginParam.imageSize = settings->total_size;

        /// set start offset info
        otaBeginParam.startOffset = settings->start_offset;

        /// set version length info
        otaBeginParam.versionLen = strlen(settings->ota_version);

        /// set version string
        memcpy(otaBeginParam.version, settings->ota_version, otaBeginParam.versionLen);

        /// set bisto customized info pointer
        GSOTA_BEGIN_PARAM_T gBegin;
        gBegin.index = settings->device_index;
        otaBeginParam.customize = (void *)&gBegin;

        /// set bisto customized info length
        otaBeginParam.customizeLen = sizeof(GSOTA_BEGIN_PARAM_T);

        /// pass the parameter to ota_common layer to handle
        ret = _gsound_ota_command_handler(OTA_COMMAND_BEGIN,
                                        (void *)&otaBeginParam,
                                        sizeof(OTA_BEGIN_PARAM_T));
    }
    else
    {
        /// should use GSOUND_STATUS_OUT_OF_MEMORY?
        ret = GSOUND_STATUS_ERROR;
    }

    return ret;
}

GSoundStatus gsound_custom_ota_data(const uint8_t *data,
                                    uint32_t length,
                                    uint32_t offset,
                                    GSoundOtaDataResult *status,
                                    GSoundOtaDeviceIndex index)
{
    GSoundStatus ret = GSOUND_STATUS_OK;
    OTA_DATA_PARAM_T otaDataParam;

    // set data pointer
    otaDataParam.data = data;

    // set data length
    otaDataParam.len = length;

    // set offset in whole upgrade image of this packet of data
    otaDataParam.offset = offset;

    // set customized data pointer
    GSOTA_DATA_PARAM_T gData;
    gData.status = status;
    gData.index = index;
    otaDataParam.customize = (void *)&gData;

    // set customize data length
    otaDataParam.customizeLen = sizeof(GSOTA_DATA_PARAM_T);

    // pass the parameter to ota_common layer to handle
    ret = _gsound_ota_command_handler(OTA_COMMAND_DATA,
                                      (void *)&otaDataParam,
                                      sizeof(OTA_DATA_PARAM_T));

    return ret;
}

GSoundStatus gsound_custom_ota_apply(GSoundOtaDeviceIndex index,
                                     bool zdOta)
{
    GLOG_D("%s zeroDay OTA:%d", __func__, zdOta);
    GSoundStatus ret = GSOUND_STATUS_OK;

    OTA_APPLY_PARAM_T otaApplyParam;
    otaApplyParam.applyNow = zdOta;

    // set customized data pointer
    GSOTA_APPLY_PARAM_T gApply;
    gApply.index = index;
    otaApplyParam.customize = (void *)&gApply;

    // set customize data length
    otaApplyParam.customizeLen = sizeof(GSOTA_APPLY_PARAM_T);

    // pass the parameter to ota_common layer to handle
    ret = _gsound_ota_command_handler(OTA_COMMAND_APPLY,
                                      (void *)&otaApplyParam,
                                      sizeof(OTA_APPLY_PARAM_T));

    return ret;
}

GSoundStatus gsound_custom_ota_abort(GSoundOtaDeviceIndex index)
{
    GLOG_D("%s device index:%d", __func__, index);
    GSoundStatus ret = GSOUND_STATUS_OK;

    OTA_ABORT_PARAM_T otaAbortParam;

    // set customized data pointer
    GSOTA_ABORT_PARAM_T gAbort;
    gAbort.index = index;
    otaAbortParam.customize = (void *)&gAbort;

    // set customize data length
    otaAbortParam.customizeLen = sizeof(GSOTA_ABORT_PARAM_T);

    ret = _gsound_ota_command_handler(OTA_COMMAND_ABORT,
                                      (void *)&otaAbortParam,
                                      sizeof(OTA_ABORT_PARAM_T));

    return ret;
}

void gsound_custom_ota_exit(void)
{
    if (gsOtaEnv.common)
    {
        if (OTA_STAGE_IDLE != gsOtaEnv.common->currentStage)
        {
            gsound_custom_ota_abort(gsOtaEnv.device);
        }
    }
}

void gsound_custom_ota_get_resume_info(GSoundOtaResumeInfo *info)
{
    NV_OTA_INFO_T *gsoundRec = NULL;
    nv_record_ota_get_ptr((void **)&gsoundRec);
    OTA_DEVICE_E device = OTA_DEVICE_APP;

    if (GSOUND_TARGET_OTA_DEVICE_INDEX_HOTWORD_MODEL == info->device_index)
    {
        device = OTA_DEVICE_HOTWORD;
    }
    else if (GSOUND_TARGET_OTA_DEVICE_INDEX_FULL_FIRMWARE != info->device_index)
    {
        // ASSERT(0, "invalid device %d received.", info->device_index);
        GLOG_W("invalid OTA device:%d", info->device_index);
        info->requested_state = GSOUND_TARGET_OTA_RESUME_NONE;
        info->offset = 0;
        memset(info->version, 0, GSOUND_TARGET_OTA_VERSION_MAX_LEN);
        return;
    }

    NV_OTA_INFO_T *pInfo = &gsoundRec[device];

    GLOG_D("gsound ota status is %d, ota offset is %d",
           pInfo->otaStatus,
           pInfo->breakPoint);

    // ota progress complete, but not applied
    if (OTA_STAGE_DONE == pInfo->otaStatus)
    {
        info->requested_state = GSOUND_TARGET_OTA_RESUME_COMPLETE;

        info->offset = pInfo->breakPoint;

        memcpy(info->version,
               (uint8_t *)pInfo->version,
               sizeof(pInfo->version));
    }
    else if (OTA_STAGE_ONGOING == pInfo->otaStatus) // ota in progress
    {
        info->requested_state = GSOUND_TARGET_OTA_RESUME_IN_PROGRESS;
        info->offset = pInfo->breakPoint;

        memcpy(info->version,
               (uint8_t *)pInfo->version,
               ARRAY_SIZE(pInfo->version));
    }
    else // ota not in progress
    {
        info->requested_state = GSOUND_TARGET_OTA_RESUME_NONE;
    }
}
