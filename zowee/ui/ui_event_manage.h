#ifndef __UI_EVENT_MANAGE_H__
#define	__UI_EVENT_MANAGE_H__

#include "hal_trace.h"

#ifdef __cplusplus
extern "C" {
#endif


#define	UI_TRACE_ENABLE			1

#if UI_TRACE_ENABLE
#define APP_UI_TRACE(s,...)		TRACE(1, "[APP_UI]%s " s, __func__, ##__VA_ARGS__)
#else
#define APP_UI_TRACE(s,...)
#endif
/*modify by kyle 20210114*/
typedef enum
{
	UI_ANC_OFF,
#ifdef __APP_ANC_HEAR_THROUGH__
	UI_ANC_HEARTHROUGH,
#endif
	UI_ANC_ON,
	
	UI_ANC_NONE,
}ZOWEE_ANC_STATUS_E;
/*modify by kyle 20210114*/

typedef	struct{
	uint16_t ibrt_dis_timer_count;
	uint8_t  ibrt_dis_timer_flag;
	bool     ibrt_shutdown_flag;
}UI_DEVICE_DISCONNCET_STATUS_T;

typedef enum{
	UI_DEVICE_NONE = 0,
	UI_DEVICE_BOX_CLOSED,
	UI_DEVICE_BOX_OPEN,
	UI_DEVICE_OUT_BOX,
	UI_DEVICE_WARED,
}UI_DEVICE_STATUS_E;

typedef enum{
	UI_WEAR_OFF,
	UI_WEAR_ON,
}UI_WEAR_STATUS_E;

typedef struct{
	uint8_t battery_level;
	uint8_t fetch_status;
	uint8_t wear_report_status;
	UI_WEAR_STATUS_E wear_status;
	UI_DEVICE_STATUS_E ear_status;
	ZOWEE_ANC_STATUS_E anc_status;
	
}UI_DEVICE_SYNC_STATUS_T;

typedef	struct{
	bool ear_power_off_inbox_fg;
	UI_DEVICE_SYNC_STATUS_T local_status;
	UI_DEVICE_SYNC_STATUS_T peer_status;
}UI_DEVICE_TOTAL_STATUS_T;

typedef enum{
	UI_EVENT_STATUS_CLOSE_BOX = 0,	
	UI_EVENT_STATUS_OPEN_BOX,
	UI_EVENT_STATUS_FETCH_OUT,
	UI_EVENT_STATUS_PUT_IN,
	UI_EVENT_STATUS_WEAR_UP,
	UI_EVENT_STATUS_WEAR_DOWN,
	UI_EVENT_STATUS_SHUTDOWN,
}UI_EVENT_STATUS_E;

typedef enum{
	UI_EVENT_STATUS = 0,		//ui event 
	UI_EVENT_GESTURE,			// gesture event 
	UI_EVENT_WEAR,				// wear event 										
	UI_EVENT_VBUS,				// vbus event 							
	UI_EVENT_BT,				// bt event 
	UI_EVENT_A2DP,				// a2dp event 
	UI_EVENT_HFP,				// hfp event 
	UI_EVENT_CHARGING,			//charge event
	UI_EVENT_SCPI,				// scpi test event 	
}UI_EVENT_MANAGE_T;


typedef struct {
    UI_EVENT_MANAGE_T msg_id;
    uint32_t ptr;
    uint32_t id;
    uint32_t param0;
    uint32_t param1;
} UI_MANAGE_MESSAGE_BLOCK;


void ui_set_disconnect_init(void);
void ui_start_disconncet_shutdown_timer(void);
void ui_stop_disconncet_shutdown_timer(void);
void ui_manage_init(void);
void ui_anc_timer_init(void);
void ui_anc_timer_start(void);
void ui_anc_timer_stop(void);
void app_ui_event_process(UI_EVENT_MANAGE_T msg_id, uint32_t ptr, uint32_t id, uint32_t param0, uint32_t param1);
void ui_send_vbus_data(uint8_t *buff,uint8_t length);
void app_reboot();
void zowee_thread_init(void);
uint8_t app_ibrt_current_era_status(void);
void app_ibrt_init_ear_status(uint8_t status);


extern UI_DEVICE_TOTAL_STATUS_T ui_device_status;
extern UI_DEVICE_DISCONNCET_STATUS_T ui_disconncet_status;


#ifdef __cplusplus
}
#endif

#endif
