#ifndef	__UI_EVENT_A2DP_H__
#define	__UI_EVENT_A2DP_H__


#include "hal_trace.h"



#define	UI_A2DP_TRACE_ENABLE		1

#if UI_A2DP_TRACE_ENABLE
#define UI_A2DP_TRACE(s,...)		TRACE(1, "[UI_A2DP]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_A2DP_TRACE(s,...)
#endif



#ifdef __cplusplus
	extern "C" {
#endif
enum AVRCP_KEY
{
	AVRCP_KEY_NULL = 0,
	AVRCP_KEY_STOP,                  
	AVRCP_KEY_PLAY,                  
	AVRCP_KEY_PAUSE,                 
	AVRCP_KEY_FORWARD,               
	AVRCP_KEY_BACKWARD,              
	AVRCP_KEY_VOLUME_UP,             
	AVRCP_KEY_VOLUME_DOWN, 
	AVRCP_KEY_PLAY_OR_PAUSE,
};
#ifdef __cplusplus
}
#endif
void ui_manage_a2dp_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);


#endif//__UI_EVENT_A2DP_H__

