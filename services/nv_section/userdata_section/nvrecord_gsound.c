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
#include "cmsis.h"
#include "nv_section_dbg.h"
#include "nvrecord_gsound.h"
#include "factory_section.h"

#ifdef BISTO_ENABLED
#include "gsound_target_ota.h"

#ifdef GSOUND_HOTWORD_ENABLED
#include "gsound_target_hotword_common.h"
#endif
#endif

/************************private macro defination***************************/
#define HOTWORD_MODEL_MAX_SIZE (80 * 1024) //!< should adjust this value if
//!< hotword model size changed and should adjust the flash section size at
//!< the sametime., @see HOTWORD_SECTION_SIZE in common.mk

#ifndef OTA_FLASH_LOGIC_ADDR
#define OTA_FLASH_LOGIC_ADDR        (FLASH_NC_BASE)
#endif

/************************private type defination****************************/

/************************extern function declearation***********************/
#ifdef GSOUND_HOTWORD_ENABLED
extern uint32_t __hotword_model_start[];
extern uint32_t __hotword_model_end[];
#endif

/**********************private function declearation************************/

/************************private variable defination************************/
#ifdef GSOUND_HOTWORD_ENABLED
#ifdef MODEL_FILE_EMBEDED
const uint8_t EMBEDED_MODEL_FILE [] = {
#include "res/gs_hw/en_all.txt" //!< must correspond to specific model ID, @see DEFAULT_HOTWORD_MODEL_ID
};

/// NOTE: Add more embeded model file info here

#define DEFAULT_MODEL_START_ADDR EMBEDED_MODEL_FILE //!< map to specific flash address within the bin file
#else /// #ifndef MODEL_FILE_EMBEDED
#define DEFAULT_MODEL_START_ADDR __hotword_model_start //!< map to specific flash address out of the bin file
#endif /// #ifdef MODEL_FILE_EMBEDED

HOTWORD_MODEL_INFO_T defaultModel[DEFAULT_MODEL_NUM] = {
    {DEFAULT_HOTWORD_MODEL_ID, (uint32_t)DEFAULT_MODEL_START_ADDR, HOTWORD_MODEL_MAX_SIZE},
    /// NOTE: Add other embeded model info here
};

char supportedModels[GSOUND_MAX_SUPPORTED_HOTWORD_MODELS_BYTES] = {};
#endif /// #ifdef GSOUND_HOTWORD_ENABLED

static NV_GSOUND_INFO_T *nvrecord_gsound_p = NULL;

/****************************function defination****************************/
void nv_record_gsound_rec_init(void)
{
    if (NULL == nvrecord_gsound_p)
    {
        nvrecord_gsound_p = &(nvrecord_extension_p->gsound_info);
    }

#ifdef GSOUND_HOTWORD_ENABLED
    /// init default supported model file(s)
    /// add to supported model file set(if it is not in this set)
    for (uint8_t i = 0; i < DEFAULT_MODEL_NUM; i++)
    {
        LOG_I("hotword_model_start:0x%x", defaultModel[i].startAddr);
        nv_record_gsound_rec_add_new_model((void *)&defaultModel[i]);
    }
#endif
}

void nv_record_gsound_rec_get_ptr(void **ptr)
{
    *ptr = nvrecord_gsound_p;
}

void nv_record_gsound_rec_updata_enable_state(bool enable)
{
    LOG_I("gsound enable state update:%d->%d", nvrecord_gsound_p->isGsoundEnabled, enable);

    if (nvrecord_gsound_p->isGsoundEnabled != enable)
    {
        /// disable the MPU protection for write operation
        uint32_t lock = nv_record_pre_write_operation();

        /// update the enable state
        nvrecord_gsound_p->isGsoundEnabled = enable;
        nv_record_extension_update();

        /// enable the MPU protection after the write operation
        nv_record_post_write_operation(lock);
    }
}

#ifdef GSOUND_HOTWORD_ENABLED
bool nv_record_gsound_rec_is_model_insert_allowed(const char *model)
{
    bool ret = true;
    uint8_t i;

    if (nvrecord_gsound_p->supportedModelCnt >= HOTWORD_MODLE_MAX_NUM)
    {
        for (i = 0; i < HOTWORD_MODLE_MAX_NUM; i++)
        {
            /// hotword model exist already
            if (!memcmp(&nvrecord_gsound_p->modelInfo[i].modelId, model, strlen(model)))
            {
                break;
            }
        }

        if (HOTWORD_MODLE_MAX_NUM == i)
        {
            ret = false;
        }
    }

    return ret;
}

bool nv_record_gsound_rec_add_new_model(void *pInfo)
{
    bool ret = true;
    bool found = false;

    HOTWORD_MODEL_INFO_T *info = (HOTWORD_MODEL_INFO_T *)pInfo;

    if (info)
    {
        uint32_t lock = nv_record_pre_write_operation();
        info->startAddr |= OTA_FLASH_LOGIC_ADDR;

        for (uint8_t i = 0; i < HOTWORD_MODLE_MAX_NUM; i++)
        {
            /// hotword model exist already
            if (!memcmp(&nvrecord_gsound_p->modelInfo[i].modelId, info->modelId, ARRAY_SIZE(info->modelId)))
            {
                LOG_I("info->startAddr:%x", info->startAddr);

                ///.update the start address
                if (info->startAddr)
                {
                    nvrecord_gsound_p->modelInfo[i].startAddr = info->startAddr;
                }

                LOG_I("model already exist in user section, model_id:%s, model_addr:%x",
                      nvrecord_gsound_p->modelInfo[i].modelId,
                      nvrecord_gsound_p->modelInfo[i].startAddr);
                found = true;
                break;
            }
        }

        LOG_I("current supportedModelCnt:%d", nvrecord_gsound_p->supportedModelCnt);

        /// brand new model insert
        if (!found)
        {
            memcpy(&nvrecord_gsound_p->modelInfo[nvrecord_gsound_p->supportedModelCnt],
                   info,
                   sizeof(HOTWORD_MODEL_INFO_T));

            LOG_I("new added modelId:%s, startAddr:%x, modelFileLen:%x", info->modelId, info->startAddr, info->len);
            nvrecord_gsound_p->supportedModelCnt++;
        }
        nv_record_post_write_operation(lock);

        /// new model file incoming, update flash info
        if (!found)
        {
            /// flush the hotword info to flash
            nv_record_update_runtime_userdata();
            nv_record_flash_flush();
        }

        LOG_I("current supportedModelCnt:%d", nvrecord_gsound_p->supportedModelCnt);
    }
    else
    {
        LOG_W("NULL pointer received in %s", __func__);
        ret = false;
    }

    return ret;
}

uint32_t nv_record_gsound_rec_get_hotword_model_addr(const char *model_id, bool add_new, int32_t newModelLen)
{
    uint32_t addr = 0;
    bool found = false;

    LOG_I("%s revceived model ID: %s", __func__, model_id);
    if (nvrecord_gsound_p)
    {
        for (uint8_t i = 0; i < ARRAY_SIZE(nvrecord_gsound_p->modelInfo); i++)
        {
            if (!memcmp(nvrecord_gsound_p->modelInfo[i].modelId, model_id, 4))
            {
                addr = nvrecord_gsound_p->modelInfo[i].startAddr;
                found = true;
                LOG_I("found saved model info, index:%d", i);
                break;
            }
        }
    }

    if (add_new && !found)
    {
        addr = (uint32_t)__hotword_model_start;
        uint32_t tempAddr;

        /// get the largest flash address
        for (uint8_t i = 0; i < nvrecord_gsound_p->supportedModelCnt; i++)
        {
            tempAddr = nvrecord_gsound_p->modelInfo[i].startAddr + nvrecord_gsound_p->modelInfo[i].len;
            if ((tempAddr & 0xFFFFFF) > (addr & 0xFFFFFF))
            {
                addr = tempAddr;
            }
        }

        /// force 4K align the address
        addr = F_4K_ALIGN(addr);
        LOG_I("Largest address:0x%x", addr);

        /// check is overwrite needed
        if ((newModelLen + addr) > (uint32_t)__hotword_model_end)
        {
            uint8_t index = 0;
            uint32_t lock = nv_record_pre_write_operation(); //!< disable the MPU for info update

#ifdef MODEL_FILE_EMBEDED
            index = DEFAULT_MODEL_NUM; //!< set offset to skip the embeded model file info
#endif

            /// update the addr value
            addr = nvrecord_gsound_p->modelInfo[index].startAddr;
            ASSERT(newModelLen <= nvrecord_gsound_p->modelInfo[index].len, "new model file size exceeded");
            ASSERT(IS_F_4K_ALIGNED(addr), "startAddr not 4K aligned:0x%x", addr);

            uint32_t intLock = int_lock(); //!< lock the global interrupt
            /// delete the first supported model file info
            for (; index < (nvrecord_gsound_p->supportedModelCnt - 1); index++)
            {
                nvrecord_gsound_p->modelInfo[index] = nvrecord_gsound_p->modelInfo[index+1];
            }

            /// decrease the number of supported model
            nvrecord_gsound_p->supportedModelCnt--;
            int_unlock(intLock); //!< unlock the global interrupt

            nv_record_post_write_operation(lock); //!< enable the MPU

            /// flush the hotword info to flash
            nv_record_update_runtime_userdata();
            nv_record_flash_flush();
        }
    }

    return addr;
}

void nv_record_gsound_rec_get_supported_model_id(const char **models_out,
                                                 uint8_t *length_out)
{
    for (uint8_t i = 0; i < nvrecord_gsound_p->supportedModelCnt; i++)
    {
        LOG_I("supported model_id:%s, model_addr:%x",
              nvrecord_gsound_p->modelInfo[i].modelId,
              nvrecord_gsound_p->modelInfo[i].startAddr);

        memcpy((supportedModels + i * GSOUND_HOTWORD_SUPPORTED_MODEL_ID_BYTES),
               nvrecord_gsound_p->modelInfo[i].modelId,
               GSOUND_HOTWORD_MODEL_ID_BYTES);
        memcpy(&supportedModels[(i * GSOUND_HOTWORD_SUPPORTED_MODEL_ID_BYTES) + GSOUND_HOTWORD_MODEL_ID_BYTES],
               SUPPORTED_HOTWORD_MODEL_DELIM,
               SUPPORTED_HOTWORD_MODEL_DELIM_BYTES);
    }

    *models_out = supportedModels;
    *length_out = nvrecord_gsound_p->supportedModelCnt * GSOUND_HOTWORD_SUPPORTED_MODEL_ID_BYTES -
                  SUPPORTED_HOTWORD_MODEL_DELIM_BYTES;

    LOG_I("total supported model_id(length is %d):", *length_out);
    DUMP8("0x%02x ", *models_out, *length_out);
}
#endif
