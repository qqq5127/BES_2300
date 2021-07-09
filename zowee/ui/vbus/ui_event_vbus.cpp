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
#include "besbt.h"

#include "ui_event_manage.h"
#include "zowee_config.h"

#include "vbus_hal_if.h"
#include "ui_event_vbus.h"
#include "vbus_protocol.h"
#include "vbus_uart.h"
#include "apps.h"
#include "zowee_section.h"
#include "app_ibrt_if.h"
#include "a2dp_decoder.h"
#include "app_ibrt_customif_ui.h"
#include "app_battery.h"
#include "app_bt.h"
//#include "pmu.h"
#include "os_api.h"
#include "nvrecord_env.h"
#include "hal_bootmode.h"
#include "ui_event_bt.h"
#include "app_ibrt_nvrecord.h"
#include "app_bt_func.h"
#include "btapp.h"
#include "apps.h"
#include "app_factory_bt.h"
#include "app_bt_stream.h"
#include "battery_hal_if.h"
#include "ui_event_scpi.h"
#if defined(__SCPI_TEST__)
#include "app_scpi_test.h"
#endif
#include "ui_event_product_test.h"
#include "norflash_api.h"
#include "nvrecord_bt.h"



VBUS_RX_BUFF_T vbus_rx_buff = {0};


extern void VBUS_SendData(uint8_t *buf,uint8_t length);
extern int app_scpi_entry_dut(void);

static void vbus_close_box_timer_handler(void const *param);
static void vbus_enter_freeman_timer_handler(void const *param);

osTimerDef(VBUS_CLOSE_BOX_TIMER, vbus_close_box_timer_handler);
static osTimerId vbus_close_box_timer = NULL;
static bool vbus_close_box_start_fg = false;

osTimerDef(VBUS_FREEMAN_TIMER, vbus_enter_freeman_timer_handler);
static osTimerId vbus_freeman_timer = NULL;



//function define
#if defined(__VBUS_SUP_OLD__)
uint8_t app_old_vbus_parse_head(void);
uint8_t app_old_vbus_command_vaild(void);
static uint8_t app_vbus_send_earside_response(void);
#else
uint8_t app_vbus_parse_head(void);
uint8_t app_vbus_parse_command_vaild(void);
static void app_vbus_execute_command(void);
static uint8_t app_vbus_get_earside(void);
#endif


void app_vbus_pop_buff(uint8_t pop_length);
static int app_vbus_response_check(void);
static void app_vbus_send_data(uint8_t *buff,uint8_t length);
static uint16_t CRC16withBaseDate(const uint8_t *buffer, uint32_t size, uint16_t crc);

static void app_tws_pair(void);
static void app_vbus_manage_get_pairing_status(void);
static void app_vbus_reset_factory_setting(void);
static void app_vbus_get_battery(void);
static void app_vbus_enter_freeman(void);
static void app_vbus_close_box(void);
static void app_vbus_open_box(void);
static void app_vbus_enter_dut(void);
static void app_vbus_set_earside(void);
static void app_vbus_tws_pair_test(void);
static void app_vbus_tws_pair_status_test(void);



static const app_vbus_cmd_frame_struct_t vbus_cmd_frame_struct[] =  {
	{VBUS_CMD_TWS_PAIRING,			app_tws_pair},
    {VBUS_CMD_PAIRING_STATUS,		app_vbus_manage_get_pairing_status},
    {VBUS_CMD_EARPHONE_RESET,		app_vbus_reset_factory_setting},
    {VBUS_CMD_EARPHONE_BATTERY,		app_vbus_get_battery},
    {VBUS_CMD_FREEMAN_PAIRING,		app_vbus_enter_freeman},
	{VBUS_CMD_CLOSE_BOX,			app_vbus_close_box},
    {VBUS_CMD_OPEN_BOX,				app_vbus_open_box},
    {VBUS_CMD_DUT,					app_vbus_enter_dut},
	{VBUS_CMD_EARSIDE,				app_vbus_set_earside},
    {VBUS_CMD_TWS_PAIR_TEST,		app_vbus_tws_pair_test},
    {VBUS_CMD_PARI_STATUS_TEST,		app_vbus_tws_pair_status_test},
};

static void app_tws_pair(void)
{
#ifndef __APP_HOLD_TWS_WHILE_PAIRING_MODE__

    ibrt_config_t local_ibrt_config;
    bool use_same_addr = true;
	unsigned char *local_bt_addr;
	uint8_t vbus_send_data[12];

    UI_VBUS_TRACE("\n----------------- VBUS_CMD_TWS_PAIRING -----------------\n");
	UI_VBUS_TRACE("battery current level: %d", ui_device_status.local_status.battery_level);

    //factory_section_original_btaddr_get(bt_mac_addr);
    //app_vbus_manage_send_data(VBUS_CMD_TWS_PAIRING, 6, bt_mac_addr);
    local_bt_addr = bt_get_local_address();
    use_same_addr = app_ibrt_ui_get_tws_use_same_addr_enable();
	//set reponse data
	
#else

	unsigned char *local_bt_addr;
	uint8_t vbus_send_data[12];

    UI_VBUS_TRACE("\n----------------- VBUS_CMD_TWS_PAIRING -----------------\n");
    local_bt_addr = bt_get_local_address();

#endif

#if defined(__VBUS_SUP_OLD__)
	unsigned short CRC16val;

	vbus_send_data[0] = app_vbus_send_earside_response();
	vbus_send_data[1] = VBUS_CMD_TWS_PAIRING;
	vbus_send_data[2] = 0x06;
	memcpy(&vbus_send_data[3], local_bt_addr, 6);
	CRC16val = CRC16withBaseDate(vbus_send_data,9,0xffff);
	vbus_send_data[9] = CRC16val & 0x00ff;
	vbus_send_data[10] = ((CRC16val & 0xff00) >> 8);
	app_vbus_send_data(vbus_send_data,11);	

#else

	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_TWS_PAIRING;
	vbus_send_data[2] = 0x07;
	memcpy(&vbus_send_data[3], local_bt_addr, 6);
	vbus_send_data[9] = 0x00;
	vbus_send_data[10] = CRC16withBaseDate(vbus_send_data,10,0xffff);
	app_vbus_send_data(vbus_send_data,11);

#endif

#ifndef __APP_HOLD_TWS_WHILE_PAIRING_MODE__
	//disconnect tws and mobile
	//zowee_tws_disconnect_all_connections();
	//disconnect tws and mobile
	zowee_disconnect_all_bt_connections();

#else
	//disconnect mobile
	zowee_disconnect_all_bt_connections();
#endif
	
	app_ibrt_ui_clear_freeman_enable();

#ifndef __APP_HOLD_TWS_WHILE_PAIRING_MODE__
	UI_VBUS_TRACE("USER_SAME ADDR = %d, app_get_earside = %d, vbus_rx_buff.buff[0] = 0x%x", use_same_addr, app_get_earside(), vbus_rx_buff.buff[0]);
	
    if (use_same_addr) {
        if (app_get_earside() == 0) {
        	//left ear copy right ear's addr
        	if(vbus_rx_buff.buff[0] == EARPHONE_RIGHT)
        	{
				UI_VBUS_TRACE(" 000 set tws left ear addr");
				DUMP8("0x%02x ", &vbus_rx_buff.buff[3], 6);
				local_ibrt_config.nv_role = IBRT_SLAVE;
				memcpy((void *)local_ibrt_config.local_addr.address, &vbus_rx_buff.buff[3], 6);
				memcpy((void *)local_ibrt_config.peer_addr.address, &vbus_rx_buff.buff[3], 6);
				local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;

				app_ibrt_if_reconfig(&local_ibrt_config); 
				
				app_ibrt_customif_ui_reconfig_bd_addr(local_ibrt_config.local_addr,
														local_ibrt_config.peer_addr,
														local_ibrt_config.nv_role);

        	}
#if defined(__VBUS_SUP_OLD__)
			else if(vbus_rx_buff.buff[0] == 0x24)
			{
				UI_VBUS_TRACE(" 111 set tws left ear addr");
				DUMP8("0x%02x ", &vbus_rx_buff.buff[3], 6);
				local_ibrt_config.nv_role = IBRT_SLAVE;
				memcpy((void *)local_ibrt_config.local_addr.address, &vbus_rx_buff.buff[3], 6);
				memcpy((void *)local_ibrt_config.peer_addr.address, &vbus_rx_buff.buff[3], 6);
				local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;

				app_ibrt_if_reconfig(&local_ibrt_config); 
				
				app_ibrt_customif_ui_reconfig_bd_addr(local_ibrt_config.local_addr,
														local_ibrt_config.peer_addr,
														local_ibrt_config.nv_role);
			}
#endif
        } else{
			UI_VBUS_TRACE(" 222 set tws right ear addr");
			DUMP8("0x%02x ", local_bt_addr, 6);
			memcpy((void *)local_ibrt_config.local_addr.address, local_bt_addr, 6);
			memcpy((void *)local_ibrt_config.peer_addr.address, local_bt_addr, 6);
            local_ibrt_config.nv_role = IBRT_MASTER;
            local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
			app_ibrt_if_reconfig(&local_ibrt_config); 
			
			app_ibrt_customif_ui_reconfig_bd_addr(local_ibrt_config.local_addr,
													local_ibrt_config.peer_addr,
													local_ibrt_config.nv_role);
        }
        
    }    
#endif

	app_ibrt_if_enter_pairing_after_tws_connected(); 
}


static void app_vbus_manage_get_pairing_status(void)
{
	uint8_t vbus_send_data[10];
	
    UI_VBUS_TRACE("\n---------------- VBUS_CMD_PAIRING_STATUS ---------------\n");

#if defined(__VBUS_SUP_OLD__)
	unsigned short CRC16val;

	vbus_send_data[0] = app_vbus_send_earside_response();
	vbus_send_data[1] = VBUS_CMD_PAIRING_STATUS;
	vbus_send_data[2] = 0x01;
	
	UI_VBUS_TRACE("tws link: %d, mobile link: %d, slave link:%d, connected dev:%d !!!", 
		app_tws_ibrt_tws_link_connected(), app_tws_ibrt_mobile_link_connected(), app_tws_ibrt_slave_ibrt_link_connected(), app_bt_get_num_of_connected_dev());
	
	/**tws pair status*/
	if(app_tws_ibrt_tws_link_connected())
	{
		if(app_tws_ibrt_mobile_link_connected() && app_bt_get_num_of_connected_dev())
		{
			UI_VBUS_TRACE("tws: master mobile link !!!");
			vbus_send_data[3] = 0x02;
		} 
		else if(app_tws_ibrt_slave_ibrt_link_connected())
		{
			UI_VBUS_TRACE("tws: slave mobile link !!!");
			vbus_send_data[3] = 0x02;
		}
		else
		{
			UI_VBUS_TRACE("tws: tws link !!!");
			vbus_send_data[3] = 0x01;
		}
	}
	/**freeman pair status*/
	else 
	{
		if(app_tws_ibrt_mobile_link_connected())
		{
			UI_VBUS_TRACE("freeman: mobile link !!!");
			vbus_send_data[3] = 0x02;
		}
		else
		{
			vbus_send_data[3] = 0x00;
		}
	}
	
	CRC16val = CRC16withBaseDate(vbus_send_data,4,0xffff);
	vbus_send_data[4] = CRC16val & 0x00ff;
	vbus_send_data[5] = ((CRC16val & 0xff00) >> 8);
	app_vbus_send_data(vbus_send_data,6);	
	
#else
	
	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_PAIRING_STATUS;
	vbus_send_data[2] = 0x01;
	vbus_send_data[3] = 0x00;

    /**tws pair status*/
    //if (app_tws_ibrt_tws_link_connected()) {
     //       vbus_send_data[3] = 1;
    //}
    
    /**freeman pair status*/
    if(app_ibrt_ui_get_freeman_enable()) {
        if (app_tws_ibrt_mobile_link_connected()) {
            vbus_send_data[3] = 1;
        }
    }

	if (app_tws_ibrt_mobile_link_connected() || app_tws_ibrt_slave_ibrt_link_connected()) {
			vbus_send_data[3] = 1;
	}
		
	vbus_send_data[4] = CRC16withBaseDate(vbus_send_data,4,0xffff);
	app_vbus_send_data(vbus_send_data,5);

#endif

	if(app_ibrt_current_era_status() == IBRT_CLOSE_BOX_EVENT)
	{
		app_ui_event_process(UI_EVENT_STATUS, 0, UI_EVENT_STATUS_OPEN_BOX, 0, 0);
		osTimerStop(vbus_close_box_timer);
		vbus_close_box_start_fg = false;
	}

}

static void app_vbus_reset_factory_setting(void)
{
	uint8_t vbus_send_data[10];
	struct nvrecord_env_t *nvrecord_env;

	UI_VBUS_TRACE("\n---------------- VBUS_CMD_EARPHONE_RESET ---------------\n");
	UI_VBUS_TRACE("battery current level: %d", ui_device_status.local_status.battery_level);

#if defined(__VBUS_SUP_OLD__)
	unsigned short CRC16val;

	vbus_send_data[0] = app_vbus_send_earside_response();
	vbus_send_data[1] = VBUS_CMD_EARPHONE_RESET;
	vbus_send_data[2] = 0x00;
	CRC16val = CRC16withBaseDate(vbus_send_data, 3, 0xffff);
	vbus_send_data[3] = CRC16val & 0x00ff;
	vbus_send_data[4] = ((CRC16val & 0xff00) >> 8);
	app_vbus_send_data(vbus_send_data, 5);

#else

	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_EARPHONE_RESET;
	vbus_send_data[2] = 0x01;
	vbus_send_data[3] = 0x00;
	vbus_send_data[4] = CRC16withBaseDate(vbus_send_data,4,0xffff);;
	app_vbus_send_data(vbus_send_data,5);
	osDelay(100);
#endif

	UI_VBUS_TRACE("tws link:%d,if master:%d",app_tws_ibrt_tws_link_connected(),app_tws_ibrt_mobile_link_connected());

#ifndef __APP_HOLD_TWS_WHILE_PAIRING_MODE__
	if(app_tws_ibrt_tws_link_connected()||app_tws_ibrt_mobile_link_connected())
		zowee_tws_disconnect_all_connections();
	nv_record_sector_clear();
	nv_record_env_init();
#else
	if(app_tws_ibrt_tws_link_connected()||app_tws_ibrt_mobile_link_connected())
	{
		zowee_disconnect_all_bt_connections();
	}
	app_ibrt_nvrecord_delete_all_mobile_record();
	
	nv_record_env_get(&nvrecord_env);
	nvrecord_env->anc_state = UI_ANC_OFF;	
	nv_record_env_set(nvrecord_env);	

	
    nv_record_flash_flush();
#endif
	
	osDelay(100);

//	zowee_section_set_battery_level_state(ui_device_status.local_status.battery_level);

	UI_VBUS_TRACE("reboot!");
	hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
	hal_cmu_sys_reboot();
}


static void app_vbus_get_battery(void)
{
	uint8_t vbus_send_data[10];
	
    UI_VBUS_TRACE("\n--------------- VBUS_CMD_EARPHONE_BATTERY --------------\n");
	UI_VBUS_TRACE("battery current level: %d", ui_device_status.local_status.battery_level);

#if defined(__VBUS_SUP_OLD__)
	unsigned short CRC16val;

	vbus_send_data[0] = app_vbus_send_earside_response();

	if(vbus_send_data[0] == 0x42)
	{
		vbus_send_data[4] = 0x00;
	}
	else
	{
		vbus_send_data[4] = 0x01;
	}
	vbus_send_data[1] = VBUS_CMD_EARPHONE_BATTERY;
	vbus_send_data[2] = 0x03;

	UI_VBUS_TRACE("battery_hal_if_charging_status %d %d", battery_hal_if_charging_status(), CHARGING_FULL);
	if(battery_hal_if_charging_status() == CHARGING_FULL)
	{
		// close the 5v of charge case
		vbus_send_data[3] = 100;
	}
	else
	{
		vbus_send_data[3] = ui_device_status.local_status.battery_level*10;
	}

	if (app_tws_ibrt_mobile_link_connected())
		vbus_send_data[5] = 0x01;
	else
		vbus_send_data[5] = 0x00;
	
	CRC16val = CRC16withBaseDate(vbus_send_data,6,0xffff);
	vbus_send_data[6] = CRC16val & 0x00ff;
	vbus_send_data[7] = ((CRC16val & 0xff00) >> 8);
	app_vbus_send_data(vbus_send_data,8);	
	
#else	
	uint8_t right_ear_battery,left_ear_battery;
	
	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_EARPHONE_BATTERY;
	vbus_send_data[2] = 0x03;

	UI_VBUS_TRACE("battery_hal_if_charging_status %d %d",battery_hal_if_charging_status(),CHARGING_FULL);

	if(app_vbus_get_earside() == EARPHONE_RIGHT)
	{
		right_ear_battery = ui_device_status.local_status.battery_level;
		left_ear_battery = ui_device_status.peer_status.battery_level;
	}
	else
	{
		left_ear_battery = ui_device_status.local_status.battery_level;
		right_ear_battery = ui_device_status.peer_status.battery_level;
	}
	if(left_ear_battery == 0xff)
	{
		left_ear_battery = 25;//invaild
	}
	if(right_ear_battery == 0xff)
	{
		right_ear_battery = 25;//invalid
	}
 	if(battery_hal_if_charging_status() == CHARGING_FULL)
	{
		// close the 5v of charge case
		if(app_vbus_get_earside() == EARPHONE_RIGHT)
		{
			vbus_send_data[3] = 100;
			vbus_send_data[4] = left_ear_battery * 10;
		}
		else
		{
			vbus_send_data[3] = right_ear_battery * 10;
			vbus_send_data[4] = 100;
		}
	}
	else
	{
			vbus_send_data[3] = right_ear_battery * 10;
			vbus_send_data[4] = left_ear_battery * 10;
	}
	UI_VBUS_TRACE("nv_record_get_paired_dev_count %d",nv_record_get_paired_dev_count());
	if(nv_record_get_paired_dev_count() == 0)
	{
		vbus_send_data[5] = 0;
	}
	else if(nv_record_get_paired_dev_count() == 1)
	{
		btif_device_record_t record;

        if (BT_STS_SUCCESS != nv_record_enum_dev_records(0, &record))
        {
			vbus_send_data[5] = 0;
        }
        else
        {
			if (MOBILE_LINK == app_tws_ibrt_get_link_type_by_addr(&record.bdAddr))
			{
				vbus_send_data[5] = 1;
			}
			else
			{
				vbus_send_data[5] = 0;
			}
        }
	}
	else
	{
		vbus_send_data[5] = 1;
	}
	
	vbus_send_data[6] = CRC16withBaseDate(vbus_send_data,6,0xffff);
	app_vbus_send_data(vbus_send_data,7);
#endif
}


static void vbus_enter_freeman_timer_handler(void const *param)
{
	UI_VBUS_TRACE("tws:%d", app_tws_ibrt_tws_link_connected());
	if(!app_tws_ibrt_tws_link_connected())
	{
		unsigned char *local_bt_addr;
		ibrt_config_t config;

		local_bt_addr = bt_get_local_address();
		UI_VBUS_TRACE();
		DUMP8("0x%02x ", local_bt_addr, 6);
		app_ibrt_nvrecord_delete_all_mobile_record();
		zowee_ibrt_remove_history_tws_paired_device();
		app_ibrt_if_config_load(&config);
		config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LRMERGE;
		memcpy((void *)config.local_addr.address, local_bt_addr, 6);
		memcpy((void *)config.peer_addr.address, local_bt_addr, 6);
		app_ibrt_if_reconfig(&config);
		osTimerStop(vbus_freeman_timer);
		//app_ibrt_ui_judge_scan_type(IBRT_FREEMAN_PAIR_TRIGGER, NO_LINK_TYPE, IBRT_UI_NO_ERROR);		
		app_ibrt_ui_set_freeman_enable();	
		app_ibrt_if_enter_freeman_pairing();
	}
	else
	{
		zowee_tws_disconnect_all_connections();
		osTimerStop(vbus_freeman_timer);
		osTimerStart(vbus_freeman_timer, 500);	
	}
}

static void app_vbus_enter_freeman(void)
{
	uint8_t vbus_send_data[10];
	
    UI_VBUS_TRACE("\n--------------- VBUS_CMD_FREEMAN_PAIRING ---------------\n %x",hal_sys_timer_get());
	UI_VBUS_TRACE("battery current level: %d", ui_device_status.local_status.battery_level);

#if defined(__VBUS_SUP_OLD__)

	unsigned short CRC16val;

	vbus_send_data[0] = app_vbus_send_earside_response();
	vbus_send_data[1] = VBUS_CMD_FREEMAN_PAIRING;
	vbus_send_data[2] = 0x01;

	UI_VBUS_TRACE("battery_hal_if_charging_status %d %d", battery_hal_if_charging_status(), CHARGING_FULL);
	if(battery_hal_if_charging_status() == CHARGING_FULL)
	{
		// close the 5v of charge case
		vbus_send_data[3] = 100;
	}
	else
	{
		vbus_send_data[3] = ui_device_status.local_status.battery_level*10;
	}
	

	CRC16val = CRC16withBaseDate(vbus_send_data,4,0xffff);
	vbus_send_data[4] = CRC16val & 0x00ff;
	vbus_send_data[5] = ((CRC16val & 0xff00) >> 8);
	app_vbus_send_data(vbus_send_data, 6);	
		
#else

	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_FREEMAN_PAIRING;
	vbus_send_data[2] = 0x01;
	vbus_send_data[3] = 0x00;
	vbus_send_data[4] = CRC16withBaseDate(vbus_send_data,4,0xffff);
	app_vbus_send_data(vbus_send_data,5);

#endif

	UI_VBUS_TRACE("tws link:%d,if master:%d",app_tws_ibrt_tws_link_connected(),app_tws_ibrt_mobile_link_connected());
	if(app_tws_ibrt_tws_link_connected())
	{
		UI_VBUS_TRACE("now i am in box freeman send to master!!!");
		zowee_ibrt_tws_exchange_info_without_rsp(TWS_MASTER_ENTER_FREEMAN,0,0);
	}

	if(vbus_freeman_timer == NULL)
		vbus_freeman_timer = osTimerCreate(osTimer(VBUS_FREEMAN_TIMER), osTimerOnce, NULL);	
	osTimerStop(vbus_freeman_timer);
	osTimerStart(vbus_freeman_timer, 500);	
}


static void vbus_close_box_timer_handler(void const *param)
{
	UI_VBUS_TRACE("\n-----\n");
	vbus_close_box_start_fg = false;
	if(app_tws_ibrt_mobile_link_connected() && app_tws_ibrt_tws_link_connected())
	{
		osTimerStart(vbus_close_box_timer, 1000);	//1s
		return;
	}
	app_ui_event_process(UI_EVENT_STATUS, 0, UI_EVENT_STATUS_CLOSE_BOX, 0, 0);
}


static void app_vbus_close_box(void)
{
	uint8_t vbus_send_data[10];
	
	UI_VBUS_TRACE("\n------------------- VBUS_CMD_CLOSE_BOX -----------------\n");

#if defined(__VBUS_SUP_OLD__)
	unsigned short CRC16val;
	vbus_send_data[0] = app_vbus_send_earside_response();
	vbus_send_data[1] = 0x0b;
	vbus_send_data[2] = 0x00;
	CRC16val = CRC16withBaseDate(vbus_send_data,3,0xffff);
	vbus_send_data[3] = CRC16val & 0x00ff;
	vbus_send_data[4] = ((CRC16val & 0xff00) >> 8);
	app_vbus_send_data(vbus_send_data,5);

#else	
	
	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_CLOSE_BOX;
	vbus_send_data[2] = 0x00;
	vbus_send_data[3] = CRC16withBaseDate(vbus_send_data,3,0xffff);
	app_vbus_send_data(vbus_send_data,4);

#endif
	if(vbus_close_box_start_fg == false)
	{
		osTimerStop(vbus_close_box_timer);
		osTimerStart(vbus_close_box_timer, 2000);	//2s		
		vbus_close_box_start_fg = true;
	}
}


static void app_vbus_open_box(void)
{
	uint8_t vbus_send_data[10];
	
    UI_VBUS_TRACE("\n------------------- VBUS_CMD_OPEN_BOX ------------------\n");

#if defined(__VBUS_SUP_OLD__)
	unsigned short CRC16val;

	vbus_send_data[0] = app_vbus_send_earside_response();
	vbus_send_data[1] = 0x0a;
	vbus_send_data[2] = 0x00;
	CRC16val =CRC16withBaseDate(vbus_send_data,3,0xffff);
	vbus_send_data[3] = CRC16val & 0x00ff;
	vbus_send_data[4] = ((CRC16val & 0xff00) >> 8);
	app_vbus_send_data(vbus_send_data,5);

#else

	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_OPEN_BOX;
	vbus_send_data[2] = 0x00;
	vbus_send_data[3] = CRC16withBaseDate(vbus_send_data,3,0xffff);
	app_vbus_send_data(vbus_send_data,4);
	
#endif

	app_ui_event_process(UI_EVENT_STATUS, 0, UI_EVENT_STATUS_OPEN_BOX, 0, 0);
	osTimerStop(vbus_close_box_timer);
	vbus_close_box_start_fg	= false;
	
	if(ui_device_status.ear_power_off_inbox_fg == true)
	{
		zowee_section_set_box_state(UI_DEVICE_BOX_OPEN);
        nv_record_flash_flush();
        norflash_api_flush_all();
#if defined(_AUTO_TEST_)
		UI_VBUS_TRACE("open box after power off , reboot!!!!");
		osDelay(50);
#endif
		hal_cmu_sys_reboot();
	}
	
	return;
}


static void app_vbus_enter_dut(void)
{
	UI_VBUS_TRACE("\n------------------- VBUS_CMD_DUT -----------------\n");

#if defined(__VBUS_SUP_OLD__)

//	unsigned short CRC16val;

//	vbus_send_data[0] = app_vbus_send_earside_response();
//	vbus_send_data[1] = VBUS_CMD_DUT;
//	vbus_send_data[2] = 0x00;

//	CRC16val = CRC16withBaseDate(vbus_send_data,3,0xffff);
//	vbus_send_data[3] = CRC16val & 0x00ff;
//	vbus_send_data[4] = ((CRC16val & 0xff00) >> 8);
//	app_vbus_send_data(vbus_send_data, 5);	
		
#else

	uint8_t vbus_send_data[10];

	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_DUT;
	vbus_send_data[2] = 0x0;
	vbus_send_data[3] = CRC16withBaseDate(vbus_send_data,3,0xffff);
	app_vbus_send_data(vbus_send_data,4);
	app_scpi_entry_dut();

#endif

}


static void app_vbus_set_earside(void)
{
#ifndef __VBUS_SUP_OLD__

	uint8_t vbus_send_data[10];
	
	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_EARSIDE;
	vbus_send_data[2] = 0x00;
	vbus_send_data[3] = CRC16withBaseDate(vbus_send_data,3,0xffff);
	app_vbus_send_data(vbus_send_data,4);
	
#endif

    UI_VBUS_TRACE("\n------------------- VBUS_CMD_EARSIDE -------------------\n");

	if(vbus_rx_buff.buff[3] == 0)
	{
		zowee_section_set_earside(0);
	}
	else if(vbus_rx_buff.buff[3] == 1)
	{
		zowee_section_set_earside(1);
	}
	app_get_earside_from_zowee_section();

	return;
}


static void app_vbus_tws_pair_test(void)
{
#ifndef __VBUS_SUP_OLD__

    ibrt_config_t local_ibrt_config;
    bool use_same_addr = true;
	unsigned char *local_bt_addr;
	uint8_t vbus_send_data[12];

    UI_VBUS_TRACE("\n----------------- VBUS_CMD_TWS_PAIR_TEST -----------------\n");
	UI_VBUS_TRACE("battery current level: %d", ui_device_status.local_status.battery_level);

    //factory_section_original_btaddr_get(bt_mac_addr);
    //app_vbus_manage_send_data(VBUS_CMD_TWS_PAIRING, 6, bt_mac_addr);
    local_bt_addr = bt_get_local_address();
    use_same_addr = app_ibrt_ui_get_tws_use_same_addr_enable();
	//set reponse data
	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_TWS_PAIR_TEST;
	vbus_send_data[2] = 0x07;
	memcpy(&vbus_send_data[3], local_bt_addr, 6);
	vbus_send_data[9] = 0x00;
	vbus_send_data[10] = CRC16withBaseDate(vbus_send_data,10,0xffff);
	app_vbus_send_data(vbus_send_data,11);

	//disconnect tws and mobile
	zowee_tws_disconnect_all_connections();
	
	app_ibrt_ui_clear_freeman_enable();

	UI_VBUS_TRACE("USER_SAME ADDR = %d, app_get_earside = %d, vbus_rx_buff.buff[0] = 0x%x", use_same_addr, app_get_earside(), vbus_rx_buff.buff[0]);
	
    if (use_same_addr) {
        if (app_get_earside() == 0) {
        	//left ear copy right ear's addr
        	if(vbus_rx_buff.buff[0] == EARPHONE_RIGHT)
        	{
				UI_VBUS_TRACE(" 000 set tws left ear addr");
				DUMP8("0x%02x ", &vbus_rx_buff.buff[3], 6);
				local_ibrt_config.nv_role = IBRT_SLAVE;
				memcpy((void *)local_ibrt_config.local_addr.address, &vbus_rx_buff.buff[3], 6);
				memcpy((void *)local_ibrt_config.peer_addr.address, &vbus_rx_buff.buff[3], 6);
				local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_LCHNL;

				app_ibrt_if_reconfig(&local_ibrt_config); 
				
				app_ibrt_customif_ui_reconfig_bd_addr(local_ibrt_config.local_addr,
														local_ibrt_config.peer_addr,
														local_ibrt_config.nv_role);

        	}
        } else{
			UI_VBUS_TRACE(" 222 set tws right ear addr");
			DUMP8("0x%02x ", local_bt_addr, 6);
			memcpy((void *)local_ibrt_config.local_addr.address, local_bt_addr, 6);
			memcpy((void *)local_ibrt_config.peer_addr.address, local_bt_addr, 6);
            local_ibrt_config.nv_role = IBRT_MASTER;
            local_ibrt_config.audio_chnl_sel = A2DP_AUDIO_CHANNEL_SELECT_RCHNL;
			app_ibrt_if_reconfig(&local_ibrt_config); 
			
			app_ibrt_customif_ui_reconfig_bd_addr(local_ibrt_config.local_addr,
													local_ibrt_config.peer_addr,
													local_ibrt_config.nv_role);
        }
        
    }    

	app_ibrt_if_enter_pairing_after_tws_connected(); 
#endif	
}


static void app_vbus_tws_pair_status_test(void)
{
#ifndef __VBUS_SUP_OLD__

	uint8_t vbus_send_data[12];
	
	vbus_send_data[0] = app_vbus_get_earside();
	vbus_send_data[1] = VBUS_CMD_PARI_STATUS_TEST;
	vbus_send_data[2] = 0x01;
	vbus_send_data[3] = 0x00;

    if(app_tws_ibrt_tws_link_connected()) {
	    vbus_send_data[3] = 1;
    }
		
	vbus_send_data[4] = CRC16withBaseDate(vbus_send_data,4,0xffff);
	app_vbus_send_data(vbus_send_data,5);

#endif	
}




#if defined(__VBUS_SUP_OLD__)

static uint8_t app_vbus_send_earside_response(void)
{
	if(app_get_earside() == 0)
	{// left
		return 0x42;
	}
	else if(app_get_earside() == 1)
	{//right
		return 0x52;
	}

	//unknow
	return 0xff;
}

#else

static uint8_t app_vbus_get_earside(void)
{
	if(app_get_earside() == 0)
	{// left
		return EARPHONE_LEFT;
	}
	else if(app_get_earside() == 1)
	{//right
		return EARPHONE_RIGHT;
	}

	//unknow
	return EARPHONE_UNKNOWN;
}

#endif


static int app_vbus_response_check(void)
{
#if defined(__VBUS_SUP_OLD__)
	return 1;
#endif

	if(app_get_earside() == 1)
	{
		if(vbus_rx_buff.buff[0] == 0xa1)
		{
			return 1;
		}
	}
	else if(app_get_earside() == 0)
	{
		hal_sys_timer_delay_ns(1000*1000*10);	//delay 10ms
		return 1;
	}

	return 0;
}

static void app_vbus_send_data(uint8_t *buff,uint8_t length)
{
	if(app_vbus_response_check())
	{
		UI_VBUS_TRACE();
		DUMP8("0x%02x ", buff, length);
		vbus_hal_if_send_data(buff,length);
	}
	else
	{	
		ASSERT(length<=sizeof(vbus_rx_buff.send_buff),"vbus_rx_buff.send_length out of memery");
		vbus_rx_buff.send_length = length;	
		memcpy(vbus_rx_buff.send_buff,buff,length);
	}
}


static uint16_t CRC16withBaseDate(const uint8_t *buffer, uint32_t size, uint16_t crc)
{
    if (NULL != buffer && size > 0)
    {
        while (size--)
        {
            crc = (crc >> 8) | (crc << 8);
            crc ^= *buffer++;
            crc ^= ((unsigned char) crc) >> 4;
            crc ^= crc << 12;
            crc ^= (crc & 0xFF) << 5;
        }
    }

    return crc;
}


void app_vbus_pop_buff(uint8_t pop_length)
{
	//UI_VBUS_TRACE("buff_length, pop_length = %d, %d !!!", vbus_rx_buff.buff_length, pop_length);
	if(vbus_rx_buff.buff_length <= pop_length)
	{
		memset(vbus_rx_buff.buff,0,VBUS_BUFF_LENGTH);
		vbus_rx_buff.buff_length = 0;
		return;
	}

	vbus_rx_buff.buff_length = vbus_rx_buff.buff_length - pop_length;
	memcpy(vbus_rx_buff.buff,vbus_rx_buff.buff+pop_length,vbus_rx_buff.buff_length);
}



//针对旧充电盒修改
#if defined(__VBUS_SUP_OLD__)

uint8_t app_old_vbus_parse_head(void)
{
	uint8_t i;

	for(i=0;i<vbus_rx_buff.buff_length;i++)
	{
		if(((vbus_rx_buff.buff[i] & 0xff) == 0x24) && (vbus_rx_buff.buff[i+1] < 0x0c))
		{
			break;
		}
		else if(((vbus_rx_buff.buff[i] & 0xff) == 0x25) && (vbus_rx_buff.buff[i+1]  < 0x0c))
		{
			break;
		}
		else if(((vbus_rx_buff.buff[i] & 0xff) == 0xFE) && (vbus_rx_buff.buff[i+1]== 0xEF))
		{
			break;
		}
	}

	//UI_VBUS_TRACE("i : %d", i);

	return i;
}

uint8_t app_old_vbus_parse_command_vaild(void)
{
	uint8_t rx_length;
	static uint16_t rx_crc = 0, mycrc = 0;

	while(1)
	{
		if(vbus_rx_buff.buff_length < 4)
		{
			return 0;
		}
		rx_length = vbus_rx_buff.buff[2] + 4;
		UI_VBUS_TRACE("rx_length: %d, %d", rx_length, vbus_rx_buff.buff_length);
		if(rx_length >= 20)
		{
			//error length
			UI_VBUS_TRACE("too long, parse next !!!");
			app_vbus_pop_buff(1);	// parse next
			app_vbus_pop_buff(app_old_vbus_parse_head());
		}
		else if(rx_length > vbus_rx_buff.buff_length)
		{
			UI_VBUS_TRACE("error length, return !!!");
			return 0;
		}
		
		rx_crc = CRC16withBaseDate(vbus_rx_buff.buff,rx_length-1,0xffff);
		mycrc = ((vbus_rx_buff.buff[rx_length-1] & 0x00ff)<< 8) | (vbus_rx_buff.buff[rx_length] & 0x00ff);
		
		UI_VBUS_TRACE("rx_crc: %x, buff: %x %x, mycrc: %x", rx_crc, vbus_rx_buff.buff[rx_length-1], vbus_rx_buff.buff[rx_length], mycrc);
		
		if(rx_crc != mycrc)
		{
			app_vbus_pop_buff(1);		// parse next
			app_vbus_pop_buff(app_old_vbus_parse_head());
		}
		else
		{
			return 1;
		}
	}
	
	return 1;
}

uint8_t app_old_vbus_command_vaild(void)
{
	uint8_t respond_cmd_data[5] = {0xFF, 0xAB, 0x05, 0x01, 0x00};

	if(((app_get_earside()==0)&&(vbus_rx_buff.buff[0]==0x24))||
		((app_get_earside()==1)&&(vbus_rx_buff.buff[0]==0x25)))
	{
		//UI_VBUS_TRACE("111 vbus_rx_buff.buff[1]: 0x%x", vbus_rx_buff.buff[1]);
		switch(vbus_rx_buff.buff[1])
		{
			case 0x01://配对
				app_tws_pair();
			break;

			case 0x02://获取配对状态
				app_vbus_manage_get_pairing_status();
			break;
			
			case 0x04://恢复出厂设置
				app_vbus_reset_factory_setting();
			break;

			case 0x06://查电量
				app_vbus_get_battery();
			break;

			case 0x09://单耳模式
				app_vbus_enter_freeman();
			break;

			case 0x0a://开盖
				app_vbus_open_box();
			break;

			case 0x0b://关盖
				app_vbus_close_box();
			break;

			default:

			break;
		}
	}
	else if((vbus_rx_buff.buff[0]==0xFE)&&(vbus_rx_buff.buff[1]==0xEF))
	{
		//UI_VBUS_TRACE("111 vbus_rx_buff.buff[2]: 0x%x", vbus_rx_buff.buff[2]);
		switch(vbus_rx_buff.buff[2])
		{
			case 0x01://shutdown
				app_shutdown();
			break;

			case 0x02:
			{
				app_ui_event_process(UI_EVENT_SCPI,0,UI_EVENT_SCPI_DUT_MODE,0,0);

			}
			break;

			case 0x03:
			{
				respond_cmd_data[2]=0x03;
				if(ui_device_status.local_status.wear_status==1)
				{
					respond_cmd_data[3]=0x01;
					UI_VBUS_TRACE("yes wear!!!");
				}
				else
				{
					respond_cmd_data[3]=0x00;
					UI_VBUS_TRACE("not wear!!!");
				}
				app_vbus_send_data((uint8_t *)respond_cmd_data,4);
			}
			break;

			case 0x04:
			{
				respond_cmd_data[2]=0x04;
				if(app_scpi_get_touch_status()==1)
				{
					respond_cmd_data[3]=0x01;
					UI_VBUS_TRACE("yes touch!!!");
				}
				else
				{
					respond_cmd_data[3]=0x00;
					UI_VBUS_TRACE("not touch!!!");
				}
			
				app_vbus_send_data((uint8_t *)respond_cmd_data,4);
			}
			break;

			case 0x05://freeman
			{
				respond_cmd_data[2]=0x05;
				respond_cmd_data[3]=0x01;
				app_vbus_send_data((uint8_t *)respond_cmd_data,4);
				zowee_tws_disconnect_all_connections();

				app_ibrt_ui_disable_scan_timer_cb((void const *)NO_LINK_TYPE);
				app_ibrt_ui_set_freeman_enable();
				app_ibrt_if_enter_freeman_pairing();
			}
			break;

			case 0x06://change mic
			{
				if(vbus_rx_buff.buff[3]==0x01)//FF MIC
				{
					app_product_test_looptester_local_ff(false, 1);
					app_product_test_looptester_local_ff(true, 1);
					respond_cmd_data[2]=0x06;
					respond_cmd_data[3]=0x01;
					respond_cmd_data[4]=0x01;
					app_vbus_send_data((uint8_t *)respond_cmd_data,5);
				}
				else if(vbus_rx_buff.buff[3]==0x02)//TALK MIC
				{
					app_product_test_looptester_local_talk(false, 1);
					app_product_test_looptester_local_talk(true, 1);
					respond_cmd_data[2]=0x06;
					respond_cmd_data[3]=0x02;
					respond_cmd_data[4]=0x01;
					app_vbus_send_data((uint8_t *)respond_cmd_data,5);
				}
				else if(vbus_rx_buff.buff[3]==0x03)//FB MIC
				{
					app_product_test_looptester_local_fb(false, 1);
					app_product_test_looptester_local_fb(true, 1);
					respond_cmd_data[2]=0x06;
					respond_cmd_data[3]=0x03;
					respond_cmd_data[4]=0x01;
					app_vbus_send_data((uint8_t *)respond_cmd_data,5);

				}
			}
			break;

			default:

			break;
		}

	}
	
	memset(vbus_rx_buff.buff,0,vbus_rx_buff.buff_length);
	vbus_rx_buff.buff_length=0;
	
	return 0;
}


#else


uint8_t app_vbus_parse_head(void)
{
	uint8_t i;

	for(i=0;i<vbus_rx_buff.buff_length;i++)
	{
		if(((vbus_rx_buff.buff[i] & 0xf0) == 0xa0) && (vbus_rx_buff.buff[i] & 0x0f) < 4)
		{
			break;
		}
	}

	return i;
}

uint8_t app_vbus_parse_command_vaild(void)
{
	uint8_t rx_length;
	uint8_t rx_crc;

	while(1)
	{
		if(vbus_rx_buff.buff_length < 4)
		{
			return 0;
		}
		rx_length = vbus_rx_buff.buff[2] + 4;
		UI_VBUS_TRACE(" rx_length %x %x",rx_length,vbus_rx_buff.buff_length);
		if(rx_length >= 50)
		{
			//error length
			app_vbus_pop_buff(1);	// parse next
			app_vbus_pop_buff(app_vbus_parse_head());
		}
		else if(vbus_rx_buff.buff_length < rx_length)
		{
			return 0;
		}
		
		rx_crc = CRC16withBaseDate(vbus_rx_buff.buff,rx_length-1,0xffff);
		
		UI_VBUS_TRACE(" rx_crc %x %x", rx_crc , vbus_rx_buff.buff[rx_length-1]);
		
		if(rx_crc != vbus_rx_buff.buff[rx_length-1])
		{
			app_vbus_pop_buff(1);		// parse next
			app_vbus_pop_buff(app_vbus_parse_head());
		}
		else
		{
			return 1;
		}
	}
	
	return 1;
}

static bool app_vbus_filter_ear_commond(void)
{
	if(vbus_rx_buff.buff[0] != 0xa1)
	{
		if((vbus_rx_buff.buff[1] != VBUS_CMD_TWS_PAIRING) && (vbus_rx_buff.buff[1] != VBUS_CMD_TWS_PAIR_TEST))
		{
			return true;
		}
	}

	return false;
}

static void app_vbus_execute_command(void)
{
	for(uint8_t i=0;i<(sizeof(vbus_cmd_frame_struct)/sizeof(app_vbus_cmd_frame_struct_t));i++)
	{
		if(app_vbus_filter_ear_commond())
		{
			break;
		}
		if(vbus_cmd_frame_struct[i].frame_cmd == vbus_rx_buff.buff[1])
		{
			vbus_cmd_frame_struct[i].FuncProPtr();
		}
	}
	app_vbus_pop_buff(vbus_rx_buff.buff[2] + 4);
}

#endif



static void app_vbus_manage_rx_process(void)
{
#if defined(__VBUS_SUP_OLD__)
	UI_VBUS_TRACE("Receive data: ");
	for(int i=0;i<vbus_rx_buff.buff_length;i++)
		{
			UI_VBUS_TRACE("%x ", vbus_rx_buff.buff[i]);
		}
	app_vbus_pop_buff(app_old_vbus_parse_head());

	if(app_old_vbus_parse_command_vaild())
	{
		app_old_vbus_command_vaild();
	}

//	if(vbus_rx_buff.buff_length>5000)//只是防止报错
//		app_vbus_execute_command();

#else
	app_vbus_pop_buff(app_vbus_parse_head());
	if(app_vbus_parse_command_vaild())
	{
		app_vbus_execute_command();
	}
#endif
}


void ui_manage_vbus_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	switch(msg_body->id)
	{
		case UI_EVENT_VBUS_INT:
		{
			vbus_hal_if_int_handler();
		}
		break;
		
		case UI_EVENT_VBUS_RX_PROCESS:
		{
			APP_UI_SYS_STATUS_E test_status = ui_get_sys_status();

#if defined(__SCPI_TEST__)
			app_scpi_test_vbus_parse(vbus_rx_buff.buff, vbus_rx_buff.buff_length);
#endif
			if(test_status == APP_UI_SYS_NORMAL)
			{
				app_vbus_manage_rx_process();			
				vbus_rx_start_handle();
			}
		}
		break;

		default:

		break;
	}
}

void ui_manage_vbus_init_timer(void)
{
	vbus_close_box_timer = osTimerCreate(osTimer(VBUS_CLOSE_BOX_TIMER), osTimerOnce, NULL);	
	vbus_close_box_start_fg = false;
}
