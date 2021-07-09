#ifndef __AI_MANAGER_H__
#define __AI_MANAGER_H__

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "nvrecord_env.h"
#include "app_ai_if.h"
#include "app_ai_manager_api.h"


#define DEFAULT_AI_SPEC AI_SPEC_INIT

typedef struct
{
    uint8_t     connectedStatus;        // 1:has connected      0: not connected
    uint8_t     assistantStatus;        // 1:assistant enable   0: assistant disable
} AI_MANAGER_STAT_T;

typedef struct {
    AI_MANAGER_INFO_T info;
    AI_MANAGER_STAT_T status[AI_SPEC_COUNT];
    bool need_reboot;
    bool reboot_ongoing;
    bool disableFlag;   //all ai disable flag
    bool specUpdateStart;
    uint8_t rebootAmaEnable;
} AI_MANAGER_T;

#ifdef __cplusplus
extern "C" {
#endif

bool ai_voicekey_is_enable(void);
void ai_voicekey_save_status(bool state);
void ai_manager_init(void);
void ai_manager_switch_spec(AI_SPEC_TYPE_E ai_spec);
void ai_manager_set_current_spec(AI_SPEC_TYPE_E ai_spec);
bool ai_manager_is_need_reboot(void);
uint8_t ai_manager_get_current_spec(void);
void ai_manager_enable(bool isEnabled, AI_SPEC_TYPE_E ai_spec);
void ai_manager_set_spec_connected_status(AI_SPEC_TYPE_E ai_spec, uint8_t connected);
int8_t ai_manager_get_spec_connected_status(uint8_t ai_spec);
void ai_manager_set_spec_assistant_status(uint8_t ai_spec, uint8_t value);
int8_t ai_manager_get_spec_assistant_status(uint8_t ai_spec);
bool ai_manager_spec_get_status_is_in_invalid(void);
void ai_manager_set_ama_assistant_enable_status(uint8_t ama_assistant_value);
uint8_t ai_manager_get_ama_assistant_enable_status();
void ai_manager_set_spec_update_flag(bool onOff);
bool ai_manager_get_spec_update_flag(void);
void ai_manager_spec_update_start_reboot();
uint32_t ai_manager_save_ctx(uint8_t *buf, uint32_t length);
uint32_t ai_manager_restore_ctx(uint8_t *buf, uint32_t buf_len);
uint32_t ai_manager_save_ctx_rsp_handle(uint8_t *buf, uint32_t buf_len);

#ifdef __cplusplus
}
#endif

#endif

