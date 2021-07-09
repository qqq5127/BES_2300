#ifndef	__UI_EVENT_HFP_H__
#define	__UI_EVENT_HFP_H__

#include "hal_trace.h"



#define	UI_HFP_TRACE_ENABLE		1

#if UI_HFP_TRACE_ENABLE
#define UI_HFP_TRACE(s,...)		TRACE(1, "[UI_HFP]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_HFP_TRACE(s,...)
#endif


enum HFP_KEY
{
	UI_EVENT_HFP_ACCEPT_CALL = 0,
	UI_EVENT_HFP_END_CALL,
	UI_EVENT_HFP_REJECT_CALL,
	UI_EVENT_HFP_THREEWAY_CALL,
	UI_EVENT_HFP_SIRI,
};


#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __cplusplus
}
#endif
void ui_manage_hfp_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);


#endif//__UI_EVENT_HFP_H__

