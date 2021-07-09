#ifndef	__UI_EVENT_BATTERY_H__
#define	__UI_EVENT_BATTERY_H__

#ifdef __APP_BATTERY_USER__


#include "hal_trace.h"
#include "ui_event_manage.h"

#ifdef __BATTERY_LOWPOWER_PROMPT_FIXED__
#define	BATTERY_LOWPOWER_PROMPT_MAX		3
#endif

#define	UI_BATTERY_BUFF_LENGTH			10


#define	UI_BATTERY_TRACE_ENABLE		1

#if UI_BATTERY_TRACE_ENABLE
#define UI_BATTERY_TRACE(s,...)		TRACE(1, "[UI_BATTERY]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_BATTERY_TRACE(s,...)
#endif


typedef enum{
	UI_EVENT_CHARGING_PLUG_IN = 0,										
	UI_EVENT_CHARGING_PLUG_OUT,	
	UI_EVENT_BATTERY_LEVEL_CHECK,
	UI_EVENT_BATTERY_LEVEL_REPROT,
	UI_EVENT_SHUT_DOWN_POWER,
}UI_EVENT_BATTERY_E;

void ui_manage_battery_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);
void battery_monitor_timer_stop(void);
void ui_manage_battery_init();

#endif //__APP_BATTERY_USER__

#endif //__UI_EVENT_BATTERY_H__

