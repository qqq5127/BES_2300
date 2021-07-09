#ifndef	__UI_EVENT_VBUS_H__
#define	__UI_EVENT_VBUS_H__


#include "hal_trace.h"



#define	VBUS_BUFF_LENGTH	200


#define	UI_VBUS_TRACE_ENABLE		1

#if UI_VBUS_TRACE_ENABLE
#define UI_VBUS_TRACE(s,...)		TRACE(1, "[UI_VBUS]%s " s, __func__,##__VA_ARGS__)
#else
#define UI_VBUS_TRACE(s,...)
#endif


typedef struct
{
	uint8_t buff[VBUS_BUFF_LENGTH];
	uint8_t buff_length;
	uint8_t send_buff[200];
	uint8_t send_length;
}VBUS_RX_BUFF_T;



extern VBUS_RX_BUFF_T vbus_rx_buff;

typedef enum{
	UI_EVENT_VBUS_INT = 0,	
	UI_EVENT_VBUS_RX_PROCESS,	
}UI_EVENT_VBUS_E;


#ifdef __cplusplus
	extern "C" {
#endif


void ui_manage_vbus_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);
void ui_manage_vbus_init_timer(void);




#ifdef __cplusplus
}
#endif

#endif

