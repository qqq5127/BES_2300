#ifndef APP_AI_IF_CONFIG_H_
#define APP_AI_IF_CONFIG_H_

#ifdef __KNOWLES
#define DIG_MIC_WORKAROUND
#define KNOWLES_UART_DATA
#endif

#ifdef __AMA_VOICE__
//#define PUSH_AND_HOLD_ENABLED
//#define AI_32KBPS_VOICE
//#define NO_LOCAL_START_TONE
#endif

#ifdef __DMA_VOICE__
//#define PUSH_AND_HOLD_ENABLED
//#define AI_32KBPS_VOICE

#define FLOW_CONTROL_ON_UPLEVEL
#define ASSAM_PKT_ON_UPLEVEL
#define CLOSE_BLE_ADV_WHEN_VOICE_CALL
#define CLOSE_BLE_ADV_WHEN_SPP_CONNECTED
#define BAIDU_RFCOMM_DIRECT_CONN
#define BYPASS_SLOW_ADV_MODE
#endif

#ifdef __SMART_VOICE__
#define PUSH_AND_HOLD_ENABLED
//#define AI_32KBPS_VOICE
#endif

#ifdef __TENCENT_VOICE__
#define PUSH_AND_HOLD_ENABLED
//#define AI_32KBPS_VOICE
#endif

#ifdef __GMA_VOICE__
//#define PUSH_AND_HOLD_ENABLED
//#define AI_32KBPS_VOICE
#endif

#endif //APP_AI_IF_CONFIG_H_

