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
#ifndef __APP_MEDIA_PLAYER_H__
#define __APP_MEDIA_PLAYER_H__

#include "resources.h"
#include "app_bt_stream.h"

#define MEDIA_DEFAULT_LANGUAGE (1)

typedef enum
{
    T_AUDIO_ID = 0x0,
        
    T_AUDIO_NUMBER,     
    
}MEDIA_AUDIO_TYPE;

#define MAX_PHB_NUMBER 20

#define PROMOT_ID_BIT_MASK                      (0xff << 8)
#define PROMOT_ID_BIT_MASK_MERGING              (1 << 15)
#define PROMOT_ID_BIT_MASK_CHNLSEl_LCHNL        (1 << 14)
#define PROMOT_ID_BIT_MASK_CHNLSEl_RCHNL        (1 << 13)
#define PROMOT_ID_BIT_MASK_CHNLSEl_ALL          (PROMOT_ID_BIT_MASK_CHNLSEl_LCHNL|PROMOT_ID_BIT_MASK_CHNLSEl_RCHNL)

#define IS_PROMPT_NEED_MERGING(promptId)           ((promptId)&PROMOT_ID_BIT_MASK_MERGING ? true : false)
#define PROMPT_ID_FROM_ID_VALUE(promptIdVal)      ((promptIdVal)&(~PROMOT_ID_BIT_MASK))
#define PROMPT_CHNLSEl_FROM_ID_VALUE(promptIdVal) ((promptIdVal)&PROMOT_ID_BIT_MASK_CHNLSEl_ALL)
#define PROMPT_PRAM_FROM_ID_VALUE(promptIdVal)    ((promptIdVal)&PROMOT_ID_BIT_MASK)
#define IS_PROMPT_CHNLSEl_ALL(promptId)           (((promptId)&PROMOT_ID_BIT_MASK_CHNLSEl_ALL) == PROMOT_ID_BIT_MASK_CHNLSEl_ALL  ? true : false)
#define IS_PROMPT_CHNLSEl_LCHNL(promptId)         (((promptId)&PROMOT_ID_BIT_MASK_CHNLSEl_ALL) == PROMOT_ID_BIT_MASK_CHNLSEl_RCHNL  ? true : false)
#define IS_PROMPT_CHNLSEl_RCHNL(promptId)         (((promptId)&PROMOT_ID_BIT_MASK_CHNLSEl_ALL) == PROMOT_ID_BIT_MASK_CHNLSEl_LCHNL  ? true : false)

typedef struct 
{
    uint16_t id;
    uint16_t status;
    // APP_AUDIO_STATUS
    uint16_t aud_type;
    uint16_t aud_id;
    char phb_number[MAX_PHB_NUMBER];	
}msg_num_ptr;

typedef struct
{
    uint16_t    promptId;
    int16_t     volume_level_override;
    float       coeff_for_mix_prompt_for_music;
    float       coeff_for_mix_music_for_music;
    float       coeff_for_mix_prompt_for_call;
    float       coeff_for_mix_call_for_call;

} __attribute__((packed)) PROMPT_MIX_PROPERTY_T;

#define PROMPT_MIX_PROPERTY_TO_ADD(promptId, \
                                    volume_level_override,  \
                                    coeff_for_mix_prompt_for_music, \
                                    coeff_for_mix_music_for_music,  \
                                    coeff_for_mix_prompt_for_call,  \
                                    coeff_for_mix_call_for_call)  \
static PROMPT_MIX_PROPERTY_T promptId##_entry __attribute__((used, section(".mixprompt_property_table"))) =     \
    {(promptId),                        \
    (volume_level_override),            \
    (coeff_for_mix_prompt_for_music),   \
    (coeff_for_mix_music_for_music),    \
    (coeff_for_mix_prompt_for_call),    \
    (coeff_for_mix_call_for_call),      \
    };

extern uint32_t __mixprompt_property_table_start[];
extern uint32_t __mixprompt_property_table_end[];

#ifdef __cplusplus
extern "C" {
#endif

//uint32_t media_playAudioSideSelect(AUD_ID_ENUM id,uint8_t device_id, uint8_t side_select);
// Prompt will be played on the both sides if they're connected.
// If there are music/phone call on-going, the prompt will be mixed.
uint32_t media_PlayAudio(AUD_ID_ENUM id,uint8_t device_id);

uint32_t media_playAudioSideSelect(AUD_ID_ENUM id,uint8_t device_id, uint16_t side_select);

// Prompt will be played locally no matter whether earbuds are connected or not
// If there are music/phone call on-going, the prompt will be mixed.
void media_PlayAudio_locally(AUD_ID_ENUM id, uint8_t device_id);

// Prompt will be played on peer device only if they're connected. 
// If there are music/phone call on-going, the music or phone call output will be stopped
// until the prompt playing is completed.
void media_PlayAudio_remotely(AUD_ID_ENUM id, uint8_t device_id);

// Prompt will be played on the both sides if they're connected.
// If there are music/phone call on-going, the music or phone call output will be stopped
// until the prompt playing is completed.
void media_PlayAudio_standalone(AUD_ID_ENUM id, uint8_t device_id);

// Prompt will be played locally no matter whether earbuds are connected or not
// If there are music/phone call on-going, the music or phone call output will be stopped
// until the prompt playing is completed.
void media_PlayAudio_standalone_locally(AUD_ID_ENUM id, uint8_t device_id);

// Prompt will be played on the peer device only if they're connected.
// If there are music/phone call on-going, the music or phone call output will be stopped
// until the prompt playing is completed.
void media_PlayAudio_standalone_remotely(AUD_ID_ENUM id, uint8_t device_id);

void media_Set_IncomingNumber(const char* pNumber);

void media_FreeMemory(void* data);

int app_play_audio_onoff(bool on, APP_AUDIO_STATUS* status);

void stand_alone_prompt_supervision_timer_init(void);

void app_play_audio_set_lang(int L);

int app_play_audio_get_lang();

void app_audio_playback_done(void);

void media_runtime_audio_prompt_update(uint16_t id, uint8_t** ptr, uint32_t* len);

PROMPT_MIX_PROPERTY_T* get_prompt_mix_property(uint16_t promptId);

AUD_ID_ENUM media_GetCurrentPrompt(uint8_t device_id);

AUD_ID_ENUM app_get_current_standalone_promptId(void);

void update_standalone_prompt_volume(int8_t vol);

void app_tws_cmd_stop_prompt_handler(uint8_t* ptrParam, uint16_t paramLen);

void app_stop_both_prompt_playing(void);

void app_tws_stop_peer_prompt(void);

int app_play_audio_stop(void);

int app_play_audio_get_aud_id(void);

void trigger_media_play(AUD_ID_ENUM id, uint8_t device_id, uint16_t aud_pram);
void trigger_media_stop(AUD_ID_ENUM id, uint8_t device_id);


#ifdef AUDIO_LINEIN
uint32_t app_play_audio_lineinmode_more_data(uint8_t *buf, uint32_t len);

int app_play_audio_lineinmode_init(uint8_t mode, uint32_t buff_len);

int app_play_audio_lineinmode_start(APP_AUDIO_STATUS* status);

int app_play_audio_lineinmode_stop(APP_AUDIO_STATUS* status);

#endif

void app_tws_sync_prompt_check(void);

void app_tws_sync_prompt_manager_reset(void);

#ifdef __cplusplus
}
#endif

#endif

