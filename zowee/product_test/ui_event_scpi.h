#ifndef	__UI_EVENT_SCPI_H__
#define	__UI_EVENT_SCPI_H__


#include "hal_trace.h"
#include "hal_timer.h"


#define	UI_SCPI_TRACE_ENABLE		1

#if UI_SCPI_TRACE_ENABLE
#define UI_SCPI_TRACE(s,...)		TRACE(1, "[UI_SCPI]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_SCPI_TRACE(s,...)
#endif

#define SCPI_DelayMs(a)      hal_sys_timer_delay(MS_TO_TICKS(a))

typedef enum{
	UI_EVENT_SCPI_SPP_PARSE = 0,
	UI_EVENT_SCPI_VBUS_PARSE = 1,		
	UI_EVENT_SCPI_DUT_MODE,
	UI_EVENT_SCPI_SPP_FACTORY_MODE,
}UI_EVENT_SCPI_E;


#ifdef __cplusplus
	extern "C" {
#endif

void ui_manage_scpi_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);



#ifdef __cplusplus
}
#endif

#endif

