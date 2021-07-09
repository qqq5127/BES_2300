#ifndef __APP_AI_VOICE_H
#define __APP_AI_VOICE_H


#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_OUTPUT_STREAMING          0
#define VOICEPATH_STREAMING             1

int app_ai_voice_stream_init(void);
int app_ai_voice_stream_deinit(void);
int app_ai_voice_start_mic_stream(void);
int app_ai_voice_stop_mic_stream(void);
void app_ai_voice_start_stream(void);
void app_ai_voice_stop_stream(void);
int app_ai_voice_on_off_control(bool on);

uint32_t app_ai_voice_thirdparty_data_come_callback(void* param1, uint32_t param2);
uint32_t app_ai_voice_thirdparty_wake_up_callback(void* param1, uint32_t param2);
uint32_t app_ai_voice_thirdparty_start_speech_callback(void* param1, uint32_t param2);
uint32_t app_ai_voice_thirdparty_stop_speech_callback(void* param1, uint32_t param2);

void app_ai_voice_set_pending_started_stream(uint8_t pendingStream, bool isEnabled);
bool app_ai_voice_get_stream_pending_state(uint8_t pendingStream);
bool app_ai_voice_get_stream_state(uint8_t stream);
void app_ai_voice_set_stream_state(uint8_t stream, bool isEnabled);

int app_ai_voice_uart_audio_init();

#ifdef __cplusplus
}
#endif

#endif

