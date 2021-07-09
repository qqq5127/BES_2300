#ifndef __AI_THREAD_H__
#define __AI_THREAD_H__

#include "ai_custom_ble.h"
#include "btapp.h"
#include "app_dip.h"
#include "app_ai_if_config.h"

#ifdef RTOS
#include "cmsis_os.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t __ai_handler_function_table_start[];
extern uint32_t __ai_handler_function_table_end[];

typedef void (*AI_UI_GLOBAL_HANDLER_IND)(uint32_t code_id, void *param1, uint32_t param2);

#define INVALID_AI_HANDLER_FUNCTION_ENTRY_INDEX 0xFFFF
#define APP_AI_VOICE_MIC_TIMEOUT_INTERVEL           (15000)
#define APP_AI_SART_SPEECH_MUTE_A2DP_STREAMING_LASTING_TIME_IN_MS  (15000)
#define APP_AI_VOICE_MUTE_A2DP_TIMEOUT_IN_MS (APP_AI_SART_SPEECH_MUTE_A2DP_STREAMING_LASTING_TIME_IN_MS+1000)
#define AI_MAILBOX_MAX (40)
#define MAX_TX_CREDIT 3

typedef enum  {
    SIGN_AI_NONE,
    SIGN_AI_CONNECT,
    SIGN_AI_WAKEUP,
    SIGN_AI_CMD_COME,
    SIGN_AI_TRANSPORT,
} AI_SIGNAL_E;

typedef struct {
    AI_SIGNAL_E signal;
    uint32_t    len;
} AI_MESSAGE_BLOCK;

//the ai transport type
typedef enum {
    AI_TRANSPORT_IDLE,
    AI_TRANSPORT_BLE,
    AI_TRANSPORT_SPP,
} AI_TRANS_TYPE_E;

//the ai wake up type
typedef enum {
    TYPE__NONE,
    TYPE__PRESS_AND_HOLD,
    TYPE__TAP,
    TYPE__KEYWORD_WAKEUP
} AI_WAKE_UP_TYPE_E;

//the ai speech type
typedef enum {
    TYPE__NORMAL_SPEECH,
    TYPE__PROVIDE_SPEECH,
} AI_SPEECH_TYPE_E;

typedef enum _AiSpeechState {
    AI_SPEECH_STATE__IDLE = 0,
    AI_SPEECH_STATE__LISTENING = 1,
    AI_SPEECH_STATE__PROCESSING = 2,
    AI_SPEECH_STATE__SPEAKING = 3
} APP_AI_SPEECH_STATE_E;

typedef struct
{
    uint32_t    sentDataSizeWithInFrame;
    uint8_t     seqNumWithInFrame;
    volatile uint8_t    ai_stream_opened    :1;
    volatile uint8_t    ai_stream_init      :1;
    volatile uint8_t    ai_stream_mic_open  :1;
    volatile uint8_t    ai_stream_running   :1;
    volatile uint8_t    audio_algorithm_engine_reset   :1;
    volatile uint8_t    ai_setup_complete   :1;
    volatile uint8_t    reserve             :2;

} APP_AI_STREAM_ENV_T;

typedef struct
{
    bool                    used;
    MOBILE_CONN_TYPE_E      mobile_connect_type;
    bt_bdaddr_t             addr;

} APP_AI_MOBILE_INFO_T;

typedef struct
{
    bool                    is_role_switching;
    bool                    can_role_switch;
    bool                    initiative_switch;  //current earphone initiative to role switch
    bool                    role_switch_direct;
    bool                    role_switch_need_ai_param;
    bool                    can_wake_up;
    bool                    transfer_voice_start;
    bool                    ai_voice_timeout_timer_need;
    bool                    peer_ai_set_complete;

    bool                    use_ai_32kbps_voice;
    bool                    local_start_tone;
    bool                    is_in_tws_mode;
    bool                    is_use_thirdparty;
    bool                    opus_in_overlay;

    uint8_t                 send_num;
    uint8_t                 tx_credit;
    uint8_t                 encode_type;
    uint32_t                currentAiSpec;              //current ai_type
    uint32_t                data_trans_size_per_trans;  //ai data transport size every transmission
    uint32_t                mtu;    //AI maximum transmission unit
    uint32_t                wake_word_start_index;
    uint32_t                wake_word_end_index;
    uint32_t                ignoredPcmDataRounds; // use to record the pcm data that ai voice ignore
    AI_TRANS_TYPE_E         transport_type;
    AI_WAKE_UP_TYPE_E       wake_up_type;
    APP_AI_STREAM_ENV_T     ai_stream;
    APP_AI_SPEECH_STATE_E   speechState;
    AI_SPEECH_TYPE_E        ai_speech_type;
    APP_AI_MOBILE_INFO_T    ai_mobile_info[BT_DEVICE_NUM];
} AI_struct ;

/**
 * @brief AI voice ble_table_handler definition data structure
 *
 */
typedef struct
{
    uint32_t                            ai_code;
    const struct ai_handler_func_table* ai_handler_function_table;      /**< ai handler function table */
} AI_HANDLER_FUNCTION_TABLE_INSTANCE_T;


#define AI_HANDLER_FUNCTION_TABLE_TO_ADD(ai_code, ai_handler_function_table)    \
    static const AI_HANDLER_FUNCTION_TABLE_INSTANCE_T ai_code##_handler_function_table __attribute__((used, section(".ai_handler_function_table"))) =     \
        {(ai_code), (ai_handler_function_table)};

#define AI_HANDLER_FUNCTION_TABLE_PTR_FROM_ENTRY_INDEX(index)   \
    ((AI_HANDLER_FUNCTION_TABLE_INSTANCE_T *)((uint32_t)__ai_handler_function_table_start + (index)*sizeof(AI_HANDLER_FUNCTION_TABLE_INSTANCE_T)))

extern AI_struct ai_struct;

extern osTimerId app_ai_voice_timeout_timer_id;
const char *mobile_connect_type2str(MOBILE_CONN_TYPE_E connect_type);
const char *ai_trans_type2str(AI_TRANS_TYPE_E trans_type);
const char *ai_wake_up_type2str(AI_WAKE_UP_TYPE_E wake_up_type);
const char *ai_speech_state2str(APP_AI_SPEECH_STATE_E speech_state);
const char *ai_speech_type2str(AI_SPEECH_TYPE_E speech_state);

void app_ai_register_ui_global_handler_ind(AI_UI_GLOBAL_HANDLER_IND handler);
void app_ai_ui_global_handler_ind(uint32_t code_id, void *param1, uint32_t param2);

bool app_ai_is_to_mute_a2dp_during_ai_starting_speech(void);
void app_ai_control_mute_a2dp_state(bool isEnabled);

void app_ai_set_ai_spec(uint8_t AiSpec);
uint8_t app_ai_get_ai_spec(void);
void app_ai_set_transport_type(AI_TRANS_TYPE_E type);
AI_TRANS_TYPE_E app_ai_get_transport_type(void);
void app_ai_set_data_trans_size(uint32_t trans_size);
uint32_t app_ai_get_data_trans_size(void);
void app_ai_set_mtu(uint32_t mtu);
uint32_t app_ai_get_mtu(void);
bool app_is_ai_voice_connected(void);
void app_ai_set_wake_up_type(AI_WAKE_UP_TYPE_E type);
AI_WAKE_UP_TYPE_E app_ai_get_wake_up_type(void);
void app_ai_set_speech_state(APP_AI_SPEECH_STATE_E speech_state);
APP_AI_SPEECH_STATE_E app_ai_get_speech_state(void);
void app_ai_disconnect_all_mobile_link(void);
bool app_ai_dont_have_mobile_connect(void);
bool app_ai_connect_mobile_has_ios(void);
void app_ai_set_speech_type(AI_SPEECH_TYPE_E type);
AI_SPEECH_TYPE_E app_ai_get_speech_type(void);
void app_ai_set_encode_type(uint8_t encode_type);
uint8_t app_ai_get_encode_type(void);

void app_ai_set_role_switching(bool switching);
bool app_ai_is_role_switching(void);
void app_ai_set_can_role_switch(bool can_switch);
bool app_ai_can_role_switch(void);
void app_ai_set_initiative_switch(bool initiative_switch);
bool app_ai_is_initiative_switch(void);
void app_ai_set_role_switch_direct(bool role_switch_direct);
bool app_ai_is_role_switch_direct(void);
void app_ai_set_need_ai_param(bool need);
bool app_ai_is_need_ai_param(void);
bool is_ai_can_wake_up(void);
void ai_set_can_wake_up(bool wake_up);
void ai_audio_stream_allowed_to_send_set(bool isEnabled);
bool is_ai_audio_stream_allowed_to_send();
void app_ai_set_timer_need(bool need);
bool app_ai_is_timer_need(void);
void app_ai_set_stream_opened(bool opened);
bool app_ai_is_stream_opened(void);
void app_ai_set_stream_init(bool init);
bool app_ai_is_stream_init(void);
void app_ai_set_stream_mic_open(bool open);
bool app_ai_is_stream_mic_open(void);
void app_ai_set_stream_running(bool running);
bool app_ai_is_stream_running(void);
void app_ai_set_algorithm_engine_reset(bool reset);
bool app_ai_is_algorithm_engine_reset();
void app_ai_set_setup_complete(bool complete);
bool app_ai_is_setup_complete(void);
void app_ai_set_peer_setup_complete(bool complete);
bool app_ai_is_peer_setup_complete(void);
void app_ai_set_use_ai_32kbps_voice(bool use_ai_32kbps_voice);
bool app_ai_is_use_ai_32kbps_voice(void);
void app_ai_set_local_start_tone(bool local_start_tone);
bool app_ai_is_local_start_tone(void);
void app_ai_set_in_tws_mode(bool is_in_tws_mode);
bool app_ai_is_in_tws_mode(void);
bool app_ai_is_in_multi_ai_mode(void);
void app_ai_set_use_thirdparty(bool is_use_thirdparty);
bool app_ai_is_use_thirdparty(void);
void app_ai_set_opus_in_overlay(bool opus_in_overlay);
bool app_ai_is_opus_in_overlay(void);
bool app_ai_is_sco_mode(void);

void app_ai_mobile_connect_handle(MOBILE_CONN_TYPE_E type, bt_bdaddr_t *_addr);
void app_ai_mobile_disconnect_handle(bt_bdaddr_t *_addr);
void app_ai_connect_handle(AI_TRANS_TYPE_E ai_trans_type);
void app_ai_disconnect_handle(AI_TRANS_TYPE_E ai_trans_type);
void app_ai_setup_complete_handle(void);

void ai_parse_data_init(void (*cb)(void));
osStatus ai_mailbox_put(AI_SIGNAL_E signal, uint32_t len);
void ai_open(uint32_t ai_spec);
uint32_t ai_save_ctx(uint8_t *buf, uint32_t buf_len);
uint32_t ai_restore_ctx(uint8_t *buf, uint32_t buf_len);

#ifdef __cplusplus
}
#endif

#endif

