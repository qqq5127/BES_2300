#ifndef	__UI_EVENT_PRODUCT_TEST_H__
#define	__UI_EVENT_PRODUCT_TEST_H__

#ifdef __SCPI_TEST__

#include "hal_trace.h"

typedef enum {
    APP_UI_SYS_INVALID,
    APP_UI_SYS_PRODUCT_TEST,
    APP_UI_SYS_NORMAL,
} APP_UI_SYS_STATUS_E;

typedef enum{
	TEST_PARING_NONE,
	TEST_PARING_DE,
	TEST_PARING_SE,
}UI_PRODUCT_TEST_PARING_STATE;

typedef	struct{
	APP_UI_SYS_STATUS_E sys_status;
	bool touch_status;
	bool spp_vbus_flag;	//0 means spp, 1 means vbus
	bool head_flag;
	bool dut_flag;
}UI_PRODUCT_TEST_STATUS_T;

#define	UI_PRODUCT_TEST_TRACE_ENABLE		1

#if UI_PRODUCT_TEST_TRACE_ENABLE
#define UI_PRODUCT_TEST_TRACE(s,...)		TRACE(1, "[UI_PRODUCT_TEST]%s " s, __func__, ##__VA_ARGS__)
#else
#define UI_PRODUCT_TEST_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif
/* SYSTem */
void app_scpi_reboot();
void app_scpi_shutdown(void);
void app_scpi_restore_factory_settings_reboot();
int app_scpi_get_sw_version(uint8_t *sw_version);
unsigned char *app_scpi_get_addr(void);
const char *app_scpi_get_bt_name(void);
int app_scpi_set_earside(bool control);
int app_scpi_get_earside(void);
void app_scpi_tws_switch_mode(void);
int app_scpi_get_touch_status(void);
int app_scpi_get_earbuds_status(void);
int app_scpi_set_earbuds_status(uint8_t device_state);
void ui_set_sys_status(APP_UI_SYS_STATUS_E sys_status_id);
APP_UI_SYS_STATUS_E ui_get_sys_status(void);
int app_scpi_entry_scpi(void);
int app_scpi_exit_scpi(void);
int app_scpi_get_scpi_mode(void);

/* BATTERY */
uint8_t app_scpi_battery_is_charging();
uint16_t app_scpi_battery_voltage();
uint8_t app_scpi_battery_percent();

/* PAIRing */
uint8_t app_scpi_get_tws_status(void);
int app_scpi_peer_pair(uint8_t *peer_bt_addr,bool role);
int app_scpi_freeman_pair(void);
void app_scpi_enter_phone_pair(void);
uint8_t* app_scpi_get_tws_addr(void);
uint8_t app_scpi_get_tws_connect_status(void);
uint8_t app_scpi_get_phone_conncet_status(void);
uint8_t* app_scpi_get_phone_addr(void);

/* HFP */
uint8_t app_scpi_get_call_status(void);
int app_scpi_get_call_volume(void);

/* MUSIc */
uint8_t app_scpi_get_music_status(void);
int app_scpi_get_music_volume(void);

/*AVRCp*/
uint8_t app_scpi_touch_control(uint8_t touch_status);

/* ANC */
int app_scpi_switch_anc_mode();
int app_scpi_get_anc_mode(void);

/* DUT */
void app_scpi_loopback_switch_mic(uint8_t smic);
void app_scpi_sco_switch_mic(uint8_t smic);
//uint8_t app_scpi_get_sco_mic(void);
int app_scpi_entry_dut(void);
int app_scpi_set_ir_calib_value(int IRdistanceGrad, uint32_t buffer);
int app_scpi_get_ir_calib_value(int IRdistanceGrad, uint32_t *buffer);
uint16_t app_scpi_set_ir_cabli_value(uint8_t type);
uint16_t app_scpi_set_xtal_fcap(unsigned int set_value);

extern UI_PRODUCT_TEST_STATUS_T product_test_stauts;


#ifdef __cplusplus
}
#endif



#endif//__SCPI_TEST__

#endif

