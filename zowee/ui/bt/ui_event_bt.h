#ifndef	__UI_EVENT_BT_H__
#define	__UI_EVENT_BT_H__


#include "hal_trace.h"



#define	UI_BT_TRACE_ENABLE		1

#if UI_BT_TRACE_ENABLE
#define UI_BT_TRACE(s,...)		TRACE(1, "[UI_BT]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_BT_TRACE(s,...)
#endif



#ifdef __cplusplus
	extern "C" {
#endif
typedef enum 
{
	TWS_WEAR_STATE,
	TWS_GESTURE_STATE,
	TWS_CALL_STATE,
	TWS_SYNC_AVRCP,
	TWS_SYNC_HFP,
	TWS_ANC_STATE,
	TWS_MASTER_ENTER_FREEMAN,
	TWS_SHUTDOWN,
	TWS_SYNC_INFO,
} ZOWEE_TWS_EXCHANGE_INFO_EVENT_ENUM;

typedef enum 
{
	TWS_STATE,

} ZOWEE_TWS_EXCHANGE_INFO_WITH_RSP_ENUM;

typedef enum 
{
	ZOWEE_BT_TWS_DISCONNECTED,
	ZOWEE_BT_TWS_CONNECTED,
	ZOWEE_BT_DISCONNECTED,
	ZOWEE_BT_CONNECTED,	
	ZOWEE_BT_RECONNECT,
	ZOWEE_BT_TWS_SWITCH,

} ZOWEE_TWS_BT_EVENT_ENUM;

bool zowee_tws_if_master_mode(void);
void zowee_ibrt_tws_exchange_info_without_rsp(ZOWEE_TWS_EXCHANGE_INFO_EVENT_ENUM cmd, uint8_t *buffer, uint8_t length);
void zowee_tws_exchange_info_handler(uint8_t *ptrParam, uint32_t paramLen);
void zowee_tws_disconnect_all_connections();
void zowee_disconnect_all_bt_connections(void);
void zowee_ibrt_remove_history_tws_paired_device(void);
void zowee_tws_send_local_info_to_peer(void);
void zowee_tws_send_local_info_to_peer_with_rsp(void);

#ifdef __cplusplus
}
#endif
void ui_manage_bt_event(UI_MANAGE_MESSAGE_BLOCK *msg_body);

#endif

