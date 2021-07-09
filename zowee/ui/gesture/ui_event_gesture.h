#ifndef	__UI_EVENT_GESTURE_H__
#define	__UI_EVENT_GESTURE_H__

#ifdef __APP_GESTURE_ENABLE__

#include "hal_trace.h"



#define	UI_GESTURE_TRACE_ENABLE		1

#if UI_GESTURE_TRACE_ENABLE
#define UI_GESTURE_TRACE(s,...)		TRACE(1, "[UI_GESTURE]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_GESTURE_TRACE(s,...)
#endif


enum GESTURE_KEY
{
	UI_EVENT_GESTURE_INT = 0,	
	UI_EVENT_GESTURE_CLICK,
	UI_EVENT_GESTURE_LONG_PRESS,
	UI_EVENT_GESTURE_LONG_LONG_PRESS,
	UI_EVENT_GESTURE_DB_CLICK_PRESS,
	UI_EVENT_GESTURE_TB_CLICK_PRESS,								
	UI_EVENT_GESTURE_TOUCH_UP,
	UI_EVENT_GESTURE_TOUCH_DOWN,
};

enum{
	UI_EVENT_GESTURE_CALL_ACCEPTE = 0x01,
	UI_EVENT_GESTURE_CALL_REJECT = 0x01<<1,
	UI_EVENT_GESTURE_HANGUP_CALL = 0x01<<2,
	UI_EVENT_GESTURE_ANC_LOOP = 0x01<<3,
	UI_EVENT_GESTURE_PLAY_PAUSE = 0x01<<4,
	UI_EVENT_GESTURE_A2DP_BACKWARD = 0x01<<5,
	UI_EVENT_GESTURE_A2DP_FORWARD = 0x01<<6,
	UI_EVENT_GESTURE_VOICE_WAKE = 0x01<<7,
}; 

#ifdef __PROJ_NAME_ZT203__
const uint16_t UI_GESTURE_FEATURE_DEFINE[] = 
{
	UI_EVENT_GESTURE_CALL_ACCEPTE | UI_EVENT_GESTURE_PLAY_PAUSE,//click
	UI_EVENT_GESTURE_ANC_LOOP,//long press
	UI_EVENT_GESTURE_VOICE_WAKE,   //long long press
	UI_EVENT_GESTURE_CALL_REJECT | UI_EVENT_GESTURE_HANGUP_CALL | UI_EVENT_GESTURE_A2DP_FORWARD, // double click
	UI_EVENT_GESTURE_A2DP_BACKWARD,// trible click
};
#elif __PROJ_NAME_ZT102__
const uint16_t UI_GESTURE_FEATURE_DEFINE[] = 
{
	0,
	UI_EVENT_GESTURE_CALL_REJECT | UI_EVENT_GESTURE_ANC_LOOP,//long press	//0xC-12-1100
	0,
	UI_EVENT_GESTURE_CALL_ACCEPTE | UI_EVENT_GESTURE_HANGUP_CALL | UI_EVENT_GESTURE_PLAY_PAUSE,// double click	//0x13-19-10011
	UI_EVENT_GESTURE_VOICE_WAKE,
};

#endif

void ui_manage_gesture_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);

#endif//__APP_GESTURE_ENABLE__

#endif
