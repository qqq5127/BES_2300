#ifndef	__UI_EVENT_WEAR_STSTUS_H__
#define	__UI_EVENT_WEAR_STSTUS_H__

#ifdef __APP_WEAR_ENABLE__


#include "hal_trace.h"



#define	UI_WEAR_STATUS_TRACE_ENABLE		1

#if UI_WEAR_STATUS_TRACE_ENABLE
#define UI_WEAR_STATUS_TRACE(s,...)		TRACE(1, "[UI_WEAR]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_WEAR_STATUS_TRACE(s,...)
#endif


enum WEAR_ACTION
{
	UI_EVENT_WEAR_INT = 0,	
	UI_EVENT_WEAR_ON,
	UI_EVENT_WEAR_OFF,
	UI_EVENT_WEAR_REFRESH_CALI_DATA,
};


#ifdef __cplusplus
	extern "C" {
#endif

void ui_manage_wear_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);
void ui_wear_control(uint8_t flag);



#ifdef __cplusplus
}
#endif

#endif //__APP_WEAR_ENABLE__

#endif //__UI_EVENT_WEAR_STSTUS_H__

