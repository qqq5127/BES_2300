#ifdef __SCPI_TEST__
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "tgt_hardware.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "app_thread.h"
#include "hwtimer_list.h"
#include "app_thread.h"
#include "ui_event_manage.h"
#include "ui_event_hfp.h"
#include "zowee_config.h"
#include "gesture_hal_if.h"
#include "app_ibrt_if.h"
#include "apps.h"
#include "app_bt_stream.h"
#include "nvrecord_env.h"
#include "hal_bootmode.h"
#include "app_battery.h"
#include "zowee_section.h"
#include "ui_event_bt.h"
#include "a2dp_decoder.h"
#include "besbt.h"
#include "app_ibrt_customif_ui.h"
#include "apps.h"
#include "app_factory_bt.h"
#include "app_factory.h"
#include "app_anc.h"
#include "ui_event_product_test.h"
#include "wear_hal_if.h"
#include "battery_hal_if.h"
#include "factory_section.h"
#include "ir_w2001.h"
#include "ui_event_battery.h"
#include "app_tws_ibrt.h"
#include "ui_event_gesture.h"
#include "btapp.h"

static void scpi_freeman_timer_handler(void const *param);
static void scpi_dut_timer_handler(void const *param);

#define	SCPI_IR_CALIB_REPEAT_COUNT		3

osTimerDef(SCPI_FREEMAN_TIMER, scpi_freeman_timer_handler);
static osTimerId scpi_freeman_timer = NULL;

osTimerDef(SCPI_DUT_MODE_TIMER, scpi_dut_timer_handler);
static osTimerId scpi_dut_mode_timer = NULL;
static uint8_t scpi_dut_step = 0;

extern "C" void app_anc_status_sync(uint8_t status);
extern int app_nvrecord_rebuild(void);

extern int hfp_volume_get(enum BT_DEVICE_ID_T id);
extern int a2dp_volume_get(enum BT_DEVICE_ID_T id);
extern bool zowee_if_in_calling(void);
extern uint8_t zowee_if_calling_status_get(void);
extern uint8_t avrcp_get_media_status(void);

//global variable define
extern uint8_t bt_dev_sw_version[3];
extern const char bt_init_name[];
static uint16_t test_mic_channel = 0;

UI_PRODUCT_TEST_STATUS_T product_test_stauts;


//function define
extern int app_shutdown(void);

/*****************restore_factory_settings_reboot*******************/
void app_scpi_restore_factory_settings_reboot()
{
	UI_PRODUCT_TEST_TRACE();
	zowee_tws_disconnect_all_connections();
	nv_record_sector_clear();
	nv_record_env_init();
	
	osDelay(100);

	UI_PRODUCT_TEST_TRACE(" reboot!");
	hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
	hal_cmu_sys_reboot();
}

void app_scpi_reboot()
{
	UI_PRODUCT_TEST_TRACE();
	hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
	hal_cmu_sys_reboot();
}

void app_scpi_shutdown(void)
{
	UI_PRODUCT_TEST_TRACE();
	app_shutdown();
}

/*****************get software version*******************/
int app_scpi_get_sw_version(uint8_t *sw_version)
{
	UI_PRODUCT_TEST_TRACE("sw version :%d.%d.%d",bt_dev_sw_version[0],bt_dev_sw_version[1],bt_dev_sw_version[2]);
	char str[25];
	snprintf(str, sizeof(str), "%d.%d.%d", bt_dev_sw_version[0], bt_dev_sw_version[1], bt_dev_sw_version[2]);
	memcpy(sw_version, str, sizeof(str));
	return 1;
}

unsigned char *app_scpi_get_addr(void)
{
	unsigned char *p_addr;
	
	p_addr = bt_get_local_address();

	return p_addr;
}

const char *app_scpi_get_bt_name(void)
{
	return bt_init_name;
}

int app_scpi_set_earside(bool control)
{
	zowee_section_set_earside(control);
	
	return 1;
}

int app_scpi_get_earside(void)
{
	return zowee_section_get_earside();
}

//scpi tws(master and slave) switch
void app_scpi_tws_switch_mode(void)
{
	UI_PRODUCT_TEST_TRACE("switch to slave !!!");
	app_ui_event_process(UI_EVENT_BT,0,ZOWEE_BT_TWS_SWITCH,0,0);
}

int app_scpi_get_touch_status(void)
{
#ifdef __PROJ_NAME_ZT203__
	return !hal_gpio_pin_get_val((HAL_GPIO_PIN_T )(TOUCH_GPIOTE_INT_PIN));
#endif
	return product_test_stauts.touch_status;
}

int app_scpi_get_earbuds_status(void)
{
	return ui_device_status.local_status.ear_status;
}

//list based on emun UI_EVENT_STATUS_E
int app_scpi_set_earbuds_status(uint8_t device_state)
{
	if(device_state==0)
	{
		UI_PRODUCT_TEST_TRACE("set UI_EVENT_STATUS_CLOSE_BOX sucess !");
		app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_CLOSE_BOX,0,0);
		return 0;
	}
	else if(device_state==1)
	{
		UI_PRODUCT_TEST_TRACE("set UI_EVENT_STATUS_OPEN_BOX sucess !");
		app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_OPEN_BOX,0,0);
		return 0;
	}
	else if(device_state==2)
	{
		UI_PRODUCT_TEST_TRACE("set UI_EVENT_STATUS_FETCH_OUT sucess !");
		app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_FETCH_OUT,0,0);
		return 0;
	}
	else if(device_state==3)
	{
		UI_PRODUCT_TEST_TRACE("set UI_EVENT_STATUS_PUT_IN sucess !");
		app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_PUT_IN,0,0);
		return 0;
	}
	else if(device_state==4)
	{
		UI_PRODUCT_TEST_TRACE("set UI_EVENT_STATUS_WEAR_UP sucess !");
		app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_WEAR_UP,0,0);
		return 0;
	}
	else if(device_state==5)
	{
		UI_PRODUCT_TEST_TRACE("set UI_EVENT_STATUS_WEAR_DOWN sucess !");
		app_ui_event_process(UI_EVENT_STATUS,0,UI_EVENT_STATUS_WEAR_DOWN,0,0);
		return 0;
	}
	else
	{
		UI_PRODUCT_TEST_TRACE("set earbuds status fail !");
		return -1;
	}
	return 0;
}

void ui_set_sys_status(APP_UI_SYS_STATUS_E sys_status_id)
{
	product_test_stauts.sys_status = sys_status_id;
}

APP_UI_SYS_STATUS_E ui_get_sys_status(void)
{
	return product_test_stauts.sys_status;
}

#ifndef	DEBUG
void app_factory_test_reset_function(bool high)
{
	struct HAL_IOMUX_PIN_FUNCTION_MAP factory_test_reset_cfg = {
		HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
	};
	struct HAL_IOMUX_PIN_FUNCTION_MAP factory_test_reset_cfg2 = {
		HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL
	};

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&factory_test_reset_cfg, 1);
	hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)factory_test_reset_cfg.pin, HAL_GPIO_DIR_OUT, high);

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&factory_test_reset_cfg2, 1);
	hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)factory_test_reset_cfg2.pin, HAL_GPIO_DIR_OUT, high);

}

#endif


int app_scpi_entry_scpi(void)
{
 	ui_stop_disconncet_shutdown_timer();
	
	app_ui_event_process(UI_EVENT_CHARGING,0,UI_EVENT_CHARGING_PLUG_OUT,0,0);
	product_test_stauts.head_flag = 0;
	ui_set_sys_status(APP_UI_SYS_PRODUCT_TEST);

#ifndef	DEBUG
	app_factory_test_reset_function(1);
#endif	
	return 1;
}

int app_scpi_exit_scpi(void)
{
	ui_set_sys_status(APP_UI_SYS_NORMAL);
	return 1;
}

int app_scpi_get_scpi_mode(void)
{
	return ui_get_sys_status();
}

uint8_t app_scpi_battery_is_charging()
{
	return app_battery_is_charging();
}

uint8_t app_scpi_battery_percent()
{
	UI_PRODUCT_TEST_TRACE(":%d",app_battery_current_level()*10);	
	return app_battery_current_level()*10;
}

uint16_t app_scpi_battery_voltage()
{
	UI_PRODUCT_TEST_TRACE(":%d",app_battery_get_current_voltage());
	return app_battery_get_current_voltage();
}

//scpi tws status
uint8_t app_scpi_get_tws_status(void)
{
	UI_PRODUCT_TEST_PARING_STATE tws_status=TEST_PARING_NONE;
	if(app_tws_ibrt_tws_link_connected())
	{
		tws_status = TEST_PARING_DE;
	}
	/**freeman pair status*/
	else 
	{
		if(app_tws_ibrt_mobile_link_connected())
		{
			UI_PRODUCT_TEST_TRACE("freeman: mobile link !!!");
			tws_status = TEST_PARING_SE;
		}
		else
		{
			UI_PRODUCT_TEST_TRACE("no any link !!!");
			tws_status = TEST_PARING_NONE;
		}
	}
	return tws_status;
}

int app_scpi_peer_pair(uint8_t *peer_bt_addr,bool role)
{	
	ibrt_config_t local_ibrt_config;

	zowee_tws_disconnect_all_connections();
	app_ibrt_ui_clear_freeman_enable();
	
	local_ibrt_config.nv_role = role;
	memcpy((void *)local_ibrt_config.local_addr.address, peer_bt_addr, 6);
	memcpy((void *)local_ibrt_config.peer_addr.address, peer_bt_addr, 6);
	if(role == 0)
	{//master
		local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
	}
	else
	{
		local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;
	}

	app_ibrt_if_reconfig(&local_ibrt_config); 
	
	app_ibrt_customif_ui_reconfig_bd_addr(local_ibrt_config.local_addr,
											local_ibrt_config.peer_addr,
											local_ibrt_config.nv_role);
											
	app_ibrt_if_enter_pairing_after_tws_connected(); 
	
	return 1;
}

static void scpi_freeman_timer_handler(void const *param)
{
	UI_PRODUCT_TEST_TRACE("tws:%d", app_tws_ibrt_mobile_link_connected());
	if(app_tws_ibrt_tws_link_connected() || app_tws_ibrt_mobile_link_connected() || app_ibrt_ui_is_profile_exchanged() || app_tws_ibrt_slave_ibrt_link_connected())
	{
		zowee_tws_disconnect_all_connections();
		osTimerStop(scpi_freeman_timer);
		osTimerStart(scpi_freeman_timer, 500);	
	}
	else
	{
		unsigned char *local_bt_addr;
		ibrt_config_t config;

		local_bt_addr = bt_get_local_address();
		app_ibrt_if_config_load(&config);
		config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LRMERGE;
		memcpy((void *)config.local_addr.address, local_bt_addr, 6);
		memcpy((void *)config.peer_addr.address, local_bt_addr, 6);
		app_ibrt_if_reconfig(&config);
		osTimerStop(scpi_freeman_timer);
		app_ibrt_ui_set_freeman_enable();	
		app_ibrt_if_enter_freeman_pairing();
	}

}

int app_scpi_freeman_pair(void)
{

	if(scpi_freeman_timer == NULL)
	{
		scpi_freeman_timer = osTimerCreate(osTimer(SCPI_FREEMAN_TIMER), osTimerOnce, NULL);	
	}

	if(app_tws_ibrt_tws_link_connected() || app_tws_ibrt_mobile_link_connected() || app_ibrt_ui_is_profile_exchanged() || app_tws_ibrt_slave_ibrt_link_connected())
	{
		osTimerStop(scpi_freeman_timer);
		osTimerStart(scpi_freeman_timer, 500);	
	}
	else
	{
		unsigned char *p_addr;
		ibrt_config_t local_ibrt_config = {0};
		
		
		
		p_addr = bt_get_local_address();
		memcpy((void *)local_ibrt_config.local_addr.address, p_addr, 6);
		memcpy((void *)local_ibrt_config.peer_addr.address, p_addr, 6);
		local_ibrt_config.nv_role = IBRT_MASTER;
		local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LRMERGE;
		app_ibrt_if_reconfig(&local_ibrt_config);
		
		app_ibrt_ui_set_freeman_enable();	
		app_ibrt_if_enter_freeman_pairing();
	}

	return 1;
}

void app_scpi_enter_phone_pair(void)
{
#ifdef __PROJ_NAME_ZT203__
	//disconnect mobile
	zowee_disconnect_all_bt_connections();
	osDelay(100);
	app_ibrt_if_enter_pairing_after_tws_connected(); 
#endif
}

//scpi get tws connect status 
uint8_t app_scpi_get_tws_connect_status(void)
{
	uint8_t tws_conn_status;
	if(app_tws_ibrt_tws_link_connected())
	{
		tws_conn_status=1;
	}
	else
	{
		tws_conn_status=0;
	}
	return tws_conn_status;
}

//scpi get tws connect addr
uint8_t* app_scpi_get_tws_addr(void)
{
	ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
	UI_PRODUCT_TEST_TRACE("tws addr is :");
	UI_PRODUCT_TEST_TRACE("%x:%x:%x:%x:%x:%x",p_ibrt_ctrl->peer_addr.address[0],
		p_ibrt_ctrl->peer_addr.address[1],p_ibrt_ctrl->peer_addr.address[2],
		p_ibrt_ctrl->peer_addr.address[3],p_ibrt_ctrl->peer_addr.address[4],
		p_ibrt_ctrl->peer_addr.address[5]);
	return p_ibrt_ctrl->peer_addr.address;
}

//scpi connect phone or not
uint8_t app_scpi_get_phone_conncet_status(void)
{
	uint8_t conn_status;
	if(app_tws_ibrt_mobile_link_connected())
	{
		conn_status=1;
	}
	else
	{
		conn_status=0;
	}
	return conn_status;
}

//scpi get phone addr
uint8_t* app_scpi_get_phone_addr(void)
{
	ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
	UI_PRODUCT_TEST_TRACE("mobile addr is :");
	UI_PRODUCT_TEST_TRACE("0x%x %x %x %x %x %x ",p_ibrt_ctrl->mobile_addr.address[0],
		p_ibrt_ctrl->mobile_addr.address[1],p_ibrt_ctrl->mobile_addr.address[2],
		p_ibrt_ctrl->mobile_addr.address[3],p_ibrt_ctrl->mobile_addr.address[4],
		p_ibrt_ctrl->mobile_addr.address[5]);
	return p_ibrt_ctrl->mobile_addr.address;
}

//scpi get call status
uint8_t app_scpi_get_call_status(void)
{
	uint8_t call_status;
	if(!zowee_if_in_calling())
	{
		call_status=0;
	}
	else
	{
		call_status = zowee_if_calling_status_get();
	}
	return call_status;
}

//scpi get call volume
int app_scpi_get_call_volume(void)
{
	int volume=0;
	enum BT_DEVICE_ID_T device_id = BT_DEVICE_ID_1;
	volume = hfp_volume_get(device_id);
	UI_PRODUCT_TEST_TRACE("volume is %d !!!",volume);
	return volume;
}

//scpi get music volume
int app_scpi_get_music_volume(void)
{
	int volume=0;
	enum BT_DEVICE_ID_T device_id = BT_DEVICE_ID_1;
	volume = a2dp_volume_get(device_id);
	UI_PRODUCT_TEST_TRACE("volume is %d !!!",volume);
	return volume;
}

//scpi get music status
uint8_t app_scpi_get_music_status(void)
{
	uint8_t music_status;
	music_status = avrcp_get_media_status();
	return music_status;
}

//scpi set touch control
uint8_t app_scpi_touch_control(uint8_t touch_status)
{
#ifdef __PROJ_NAME_ZT203__
	switch(touch_status)
	{
		case 0x01:
			UI_PRODUCT_TEST_TRACE("one click");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_CLICK,0,0);
		break;

		case 0x02:
			UI_PRODUCT_TEST_TRACE("double click");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_DB_CLICK_PRESS,0,0);
		break;

		case 0x03:
			UI_PRODUCT_TEST_TRACE("trible click");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_TB_CLICK_PRESS,0,0);
		break;

		case 0x0f:
			UI_PRODUCT_TEST_TRACE("long press");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_LONG_PRESS,0,0);
		break;

		case 0x0ff:
			UI_PRODUCT_TEST_TRACE("long long press");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_LONG_LONG_PRESS,0,0);
		break;

		default:

		break;

	}
#elif defined(__PROJ_NAME_ZT102__)
	switch(touch_status)
	{
		case 0x01:
			UI_PRODUCT_TEST_TRACE("one click");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_CLICK,0,0);
		break;

		case 0x02:
			UI_PRODUCT_TEST_TRACE("double click");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_DB_CLICK_PRESS,0,0);
		break;

		case 0x03:
			UI_PRODUCT_TEST_TRACE("trible click");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_TB_CLICK_PRESS,0,0);
		break;

		case 0x0f:
			UI_PRODUCT_TEST_TRACE("long press");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_LONG_PRESS,0,0);
		break;

		case 0x0ff:
			UI_PRODUCT_TEST_TRACE("long long press");
			app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_LONG_LONG_PRESS,0,0);
		break;

		default:

		break;

	}

#endif

	return 0;
}

int app_scpi_switch_anc_mode()
{
	if(ui_device_status.local_status.anc_status==UI_ANC_OFF)
		ui_device_status.local_status.anc_status=UI_ANC_HEARTHROUGH;
	else if(ui_device_status.local_status.anc_status==UI_ANC_HEARTHROUGH)
		ui_device_status.local_status.anc_status=UI_ANC_ON;
	else if(ui_device_status.local_status.anc_status==UI_ANC_ON)
		ui_device_status.local_status.anc_status=UI_ANC_OFF;
	else
		return 0;

#if defined(IBRT)
	app_anc_status_sync(ui_device_status.local_status.anc_status);
#endif
	app_anc_status_post(ui_device_status.local_status.anc_status);

	return 1;
}

int app_scpi_get_anc_mode(void)
{
	return ui_device_status.local_status.anc_status;
}

static void scpi_dut_timer_handler(void const *param)
{
	
	UI_PRODUCT_TEST_TRACE("tws:%d", app_tws_ibrt_mobile_link_connected());

	if(app_tws_ibrt_tws_link_connected() || app_tws_ibrt_mobile_link_connected() || app_ibrt_ui_is_profile_exchanged() || app_tws_ibrt_slave_ibrt_link_connected())
	{
		zowee_tws_disconnect_all_connections();
		osTimerStop(scpi_dut_mode_timer);
		osTimerStart(scpi_dut_mode_timer, 500);	
		scpi_dut_step = 0;
	}
	else if(scpi_dut_step == 0)
	{
		scpi_dut_step = 1;
		app_nvrecord_rebuild();
		app_factorymode_key_init();
		app_factorymode_bt_signalingtest(NULL,NULL);
	}
}

int app_scpi_entry_dut(void)
{
	wear_hal_if_disable();
	gesture_hal_if_disable();
	battery_hal_if_uninit();
	ui_stop_disconncet_shutdown_timer();
	product_test_stauts.dut_flag = true;

	if(scpi_dut_mode_timer == NULL)
	{
		scpi_dut_mode_timer = osTimerCreate(osTimer(SCPI_DUT_MODE_TIMER), osTimerOnce, NULL);	
	}
	scpi_dut_step = 0;
	osTimerStop(scpi_dut_mode_timer);
	osTimerStart(scpi_dut_mode_timer, 500);	
	return true;
}

/*****************switch to FF MIC/TALK MIC*******************/
void app_scpi_loopback_switch_mic(uint8_t smic)
{
	UI_PRODUCT_TEST_TRACE("%d",smic);
	app_product_test_looptester_local_ff(false, 1);
	app_product_test_looptester_local_fb(false, 1);
	app_product_test_looptester_local_talk(false, 1);
	if(smic==0x00)//FF MIC
	{
		app_product_test_looptester_local_ff(true, 1);

	}
	else if(smic==0x01)//FB MIC
	{
		app_product_test_looptester_local_fb(true, 1);

	}
	else if(smic==0x02)//TALK MIC
	{
		app_product_test_looptester_local_talk(true, 1);

	}
}

void app_scpi_sco_switch_mic(uint8_t smic)
{
	UI_PRODUCT_TEST_TRACE(":%d",smic);
	if(smic == 0x01)
	{
		test_mic_channel = 1;	//ff mic
	}
	else if(smic == 0x02)
	{
		test_mic_channel = 2;	//talk mic
	}
	else if(smic == 0x00)
	{
		test_mic_channel = 0;	//exit test mode
	}
}

uint8_t app_scpi_get_sco_mic(void)
{
	//UI_PRODUCT_TEST_TRACE(": %d", test_mic_channel);
	return 	test_mic_channel;
}

#if defined(__IR_W2001__)
uint16_t STK_Read_PS(void);

int app_scpi_set_ir_calib_value(int IRdistanceGrad, uint32_t buffer)
{
	UI_PRODUCT_TEST_TRACE();
	int ret = 0;
	ret = zowee_section_set_ir_calib_value(nvrec_pt_dev_ir_calib_type(IRdistanceGrad), buffer);
	
	return ret;
}

int app_scpi_get_ir_calib_value(int IRdistanceGrad, uint32_t *buffer)
{
	UI_PRODUCT_TEST_TRACE();	
	int ret = 0;
	ret = zowee_section_get_ir_calib_value(nvrec_pt_dev_ir_calib_type(IRdistanceGrad), buffer);

	return ret;
}

uint16_t app_scpi_set_ir_cabli_value(uint8_t type)
{
	bool scpi_ir_calib_flag = 0;
	uint32_t scpi_ir_calib_total_value;
	uint8_t scpi_ir_calib_count;
	uint32_t value;
	uint16_t read_ps_value;

	UI_PRODUCT_TEST_TRACE(":%d",type);

	
	scpi_ir_calib_total_value = 0;
	scpi_ir_calib_flag = type;
	scpi_ir_calib_count = 0;
	scpi_ir_calib_count++;
	for(scpi_ir_calib_count= 0; scpi_ir_calib_count < SCPI_IR_CALIB_REPEAT_COUNT;scpi_ir_calib_count++)
	{
		read_ps_value = STK_Read_PS();
		if(read_ps_value > IR_W2001_MAX_PS_DATA)
		{
			read_ps_value = IR_W2001_MAX_PS_DATA;
		}
		scpi_ir_calib_total_value += read_ps_value;
		osDelay(500);
	}
	value = scpi_ir_calib_total_value/SCPI_IR_CALIB_REPEAT_COUNT;
	app_scpi_set_ir_calib_value(scpi_ir_calib_flag,value);

	return value;
}
#endif //__IR_W2001__

uint16_t app_scpi_set_xtal_fcap(unsigned int set_value)
{
	unsigned int fcap_value;
	
	factory_section_xtal_fcap_get(&fcap_value);
	UI_PRODUCT_TEST_TRACE("read %d set %d",fcap_value,set_value);
	factory_section_xtal_fcap_set(set_value);

	return (uint16_t )fcap_value;
}



#endif//__SCPI_TEST__
