#if defined(__SCPI_TEST__)
#include "app_scpi_stub.h"
#include "ui_event_manage.h"
#include "ui_event_product_test.h"
#include "types.h"
#include "utils_private.h"
#include "app_key.h"

extern "C" int osDelay(uint32_t ms);

scpi_result_t SCPI_Read_IDN(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: READ IDN");
	char idn_info[100];
	char version[25];
	unsigned char *addr;
	const char *bt_name;
	app_scpi_get_sw_version((uint8_t*)version);
	addr = app_scpi_get_addr();
	bt_name = app_scpi_get_bt_name();
	snprintf(idn_info,sizeof(idn_info),"%s,%x:%x:%x:%x:%x:%x,%s",
		bt_name,addr[5],addr[4],addr[3],addr[2],addr[1],addr[0],version);
	APP_SCPI_TRACE("TR SCPI: IDN IS %s", idn_info);
	SCPI_ResultMnemonic(context, idn_info);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Shut_Down(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: SHUT DOWN");
	app_scpi_shutdown();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Reoot(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: REBOOT START");
	app_scpi_reboot();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Temperature(scpi_t * context)
{
	int temp=0;
	APP_SCPI_TRACE("TR SCPI: EARPHONE TEMPERATURE IS %d", temp);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Factory_Reset(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: EARPHONE RESET FACTORY");
	app_scpi_restore_factory_settings_reboot();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_SW_Version(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: SW VERSION");
	char version[25];
	app_scpi_get_sw_version((uint8_t*)version);
	APP_SCPI_TRACE("TR SCPI: SW VERSION %s", version);
	SCPI_ResultMnemonic(context, (const char*)version);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_HW_Version(scpi_t * context)
{
	char version[25] = "0.0";
	APP_SCPI_TRACE("TR SCPI: HW VERSION %s", version);
	SCPI_ResultMnemonic(context, (const char*)version);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_BT_Address(scpi_t * context)
{
	unsigned char *addr;
	char b_addr[30];
	addr = app_scpi_get_addr();
	snprintf(b_addr,sizeof(b_addr),"%x:%x:%x:%x:%x:%x",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
	APP_SCPI_TRACE("TR SCPI: BT ADDRESS IS %s", b_addr);
	SCPI_ResultMnemonic(context, (const char*)b_addr);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_BT_Name(scpi_t * context)
{
	const char *bt_name;
	bt_name = app_scpi_get_bt_name();
	APP_SCPI_TRACE("TR SCPI: BT ADDRESS IS %s", bt_name);
	SCPI_ResultMnemonic(context, bt_name);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Touch_State(scpi_t * context)
{
	int t_state;
	t_state = app_scpi_get_touch_status();
	APP_SCPI_TRACE("TR SCPI: TOUCH STATE IS %d", t_state);
	SCPI_ResultBool(context, t_state);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Set_Box_State(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"BOXClose", 0},
		{"BOXOpen", 1},
		{"OUTBox", 2},
		{"INBOx", 3},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SET SYS STATE PARA ERROR");
		return SCPI_RES_ERR;
	}
	APP_SCPI_TRACE("TR SCPI: SET SYS BOX STATE IS %s", scpi_choice_def[intval].name);
	app_scpi_set_earbuds_status(intval);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Set_Wear_State(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"UP", 4},
		{"DOWN", 5},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SET SYS STATE PARA ERROR");
		return SCPI_RES_ERR;
	}
	APP_SCPI_TRACE("TR SCPI: SET SYS WEAR STATE IS %s", scpi_choice_def[intval].name);
	app_scpi_set_earbuds_status(intval);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Sys_State(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"NONE", UI_DEVICE_NONE},
		{"BOXClose", UI_DEVICE_BOX_CLOSED},
		{"BOXOpen", UI_DEVICE_BOX_OPEN},
		{"OUTBox", UI_DEVICE_OUT_BOX},
		{"WEAR", UI_DEVICE_WARED},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	intval = app_scpi_get_earbuds_status();
	APP_SCPI_TRACE("TR SCPI: SYS STATE IS %s", scpi_choice_def[intval].name);
	SCPI_ResultMnemonic(context, scpi_choice_def[intval].name);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Set_Role(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"LEFT", 0},
		{"RIGHt", 1},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	APP_SCPI_TRACE("TR SCPI: SET ROLE");
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SET ROLE PARA ERROR");
		return SCPI_RES_ERR;
	}
	else
	{
		APP_SCPI_TRACE("TR SCPI: SET ROLE %s", scpi_choice_def[intval].name);
		app_scpi_set_earside(intval);
	}
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Role(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"LEFT", 0},
		{"RIGHt", 1},
		{"UNKNown", 2},
		SCPI_CHOICE_LIST_END 
	};
	int32_t role_status = 2;
	role_status = app_scpi_get_earside();
	APP_SCPI_TRACE("TR SCPI: ROLE IS %s", scpi_choice_def[role_status].name);
	SCPI_ResultMnemonic(context, scpi_choice_def[role_status].name);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Switch_Role(scpi_t * context)
{
	app_scpi_tws_switch_mode();
	APP_SCPI_TRACE("TR SCPI: SWITCH ROLE");
	return SCPI_RES_OK;
}

scpi_result_t SCPI_SCPI_Mode(scpi_t * context)
{
	bool value_return = false;
	if(!SCPI_ParamBool(context,&value_return,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SCPI MODE PARA ERROR");
		return SCPI_RES_ERR;
	}
	if(value_return)
	{
		app_scpi_entry_scpi();
		APP_SCPI_TRACE("TR SCPI: SCPI MODE ON");
	}
	else
	{
		app_scpi_exit_scpi();
		app_scpi_sco_switch_mic(0x00);
		APP_SCPI_TRACE("TR SCPI: SCPI MODE OFF");
	}
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_SCPI_Mode(scpi_t * context)
{
	int sys_mode;
	int scpi_mode=0;
	sys_mode = ui_get_sys_status();
	if(sys_mode == 1)
	{
		scpi_mode = 1;
	}
	APP_SCPI_TRACE("TR SCPI: SCPI MODE IS %d", scpi_mode);
	SCPI_ResultBool(context, scpi_mode);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Return_Head(scpi_t * context)
{
	bool value_return = false;
	if(!SCPI_ParamBool(context,&value_return,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SCPI MODE PARA ERROR");
		return SCPI_RES_ERR;
	}
	if(value_return)
	{
		product_test_stauts.head_flag = 1;
	}
	else
	{
		product_test_stauts.head_flag = 0;
	}
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Project_Name(scpi_t * context)
{
#ifdef __PROJ_NAME_ZT203__
	APP_SCPI_TRACE("TR SCPI: PROJECT NAME IS ZT203");
	SCPI_ResultMnemonic(context, "ZT203");
#elif __PROJ_NAME_ZT102__
	APP_SCPI_TRACE("TR SCPI: PROJECT NAME IS ZT102");
	SCPI_ResultMnemonic(context, "ZT102");
#endif
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Charge_State(scpi_t * context)
{
	uint8_t charge_state = app_scpi_battery_is_charging();
	APP_SCPI_TRACE("TR SCPI: BATTERY CHARGING STATE: %d", charge_state);
	SCPI_ResultBool(context, charge_state);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Battery_Level(scpi_t * context)
{
	uint8_t level;
	level = app_scpi_battery_percent();
	APP_SCPI_TRACE("TR SCPI: CURRENT BATTERY LEVEL:%d", level);
	SCPI_ResultInt32(context, level);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Battery_Voltage(scpi_t * context)
{
	uint16_t vol;
	vol = app_scpi_battery_voltage();
	APP_SCPI_TRACE("TR SCPI: CURRENT BATTERY VOLTAGE:%d mV", vol);
	SCPI_ResultInt32(context, vol);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Pair_State(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: GET PAIR STATE IS INQUiry");
	APP_SCPI_TRACE("TR SCPI: GET PAIR STATE IS DISCover");
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Pair_Mode(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"NONE", TEST_PARING_NONE},
		{"DE", TEST_PARING_DE},
		{"SE", TEST_PARING_SE},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	intval = app_scpi_get_tws_status();
	APP_SCPI_TRACE("TR SCPI: SYS STATE IS %s", scpi_choice_def[intval].name);
	SCPI_ResultMnemonic(context, scpi_choice_def[intval].name);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Set_Pair_Peer(scpi_t * context)
{
	const char * addr;
	size_t value_len;
	if(!SCPI_ParamCharacters(context,&addr,&value_len, FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: PAIR INSTRUCT ERROR");
		return SCPI_RES_ERR;
	}

	char baddr[30];
	uint8_t btaddr[6]={0,0,0,0,0,0};
	uint32_t hex_value;
	for(int i=0;i<6;i++)
	{
		strncpy(baddr, addr+3*i, 2);
		strBaseToUInt32(baddr,&hex_value,16);
		btaddr[5-i] = hex_value;
	}
	bool role;
	role = app_scpi_get_earside();
	app_scpi_peer_pair(btaddr,role);
	APP_SCPI_TRACE("TR SCPI: PAIR PEER BT ADDR:%x:%x:%x:%x:%x:%x",btaddr[5],btaddr[4],btaddr[3],btaddr[2],btaddr[1],btaddr[0]);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Pair_Peer(scpi_t * context)
{
	//if(app_scpi_get_tws_connect_status())
	{
		uint8_t* baddr;
		char  baddr_string[20];
		
		baddr = app_scpi_get_tws_addr();
		APP_SCPI_TRACE("TR SCPI: PEER PAIR ADDRESS IS %s", baddr);
		sprintf(baddr_string,"%02x:%02x:%02x:%02x:%02x:%02x",baddr[0],baddr[1],baddr[2],baddr[3],baddr[4],baddr[5]);
		SCPI_ResultMnemonic(context, (const char*)baddr_string);
	}
	//APP_SCPI_TRACE("TR SCPI: DID NOT PAIR PEER");
	//SCPI_ResultMnemonic(context, "0:0:0:0:0:0");
	return SCPI_RES_OK;
}
	
scpi_result_t SCPI_Set_Freeman_Pair(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: FREEMAN PAIR");
	app_scpi_freeman_pair();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Set_Pair_Phone(scpi_t * context)
{
	app_scpi_enter_phone_pair();
	APP_SCPI_TRACE("TR SCPI: ENTER PAIRING PHONE");
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Pair_Phone(scpi_t * context)
{
	if(app_scpi_get_phone_conncet_status())
	{
		uint8_t* baddr;
		baddr = app_scpi_get_phone_addr();
		APP_SCPI_TRACE("TR SCPI: PAIR PHONE ADDRESS IS %s", baddr);
		SCPI_ResultMnemonic(context, (const char*)baddr);
	}
	else
	{
		APP_SCPI_TRACE("TR SCPI: DID NOT PAIR PHONE");
		SCPI_ResultMnemonic(context, "0:0:0:0:0:0");
	}
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_HFP_Status(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"IDLE", 0},
		{"RINGing", 1},
		{"INCAlling", 2},
		SCPI_CHOICE_LIST_END 
	};
	uint8_t call_state = 3;
	call_state = app_scpi_get_call_status();
	APP_SCPI_TRACE("TR SCPI: HFP STATE IS %s", scpi_choice_def[call_state].name);
	SCPI_ResultMnemonic(context, scpi_choice_def[call_state].name);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_HFP_Volume(scpi_t * context)
{
	int vol;
	vol = app_scpi_get_call_volume();
	APP_SCPI_TRACE("TR SCPI: HFP VOLUME IS %d", vol);
	SCPI_ResultInt32(context, vol);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Music_Status(scpi_t * context)
{
	uint8_t music_state = 0;
	if(app_scpi_get_music_status() == 1)
	{
		music_state = 1;
	}
	APP_SCPI_TRACE("TR SCPI: MUSIC STATE IS %d", music_state);
	SCPI_ResultBool(context, music_state);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_Music_Volume(scpi_t * context)
{
	int vol;
	vol = app_scpi_get_music_volume();
	APP_SCPI_TRACE("TR SCPI: MUSIC VOLUME IS %d", vol);
	SCPI_ResultInt32(context, vol);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Control_Key(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"ONCE", 0x01},
		{"DOUBle", 0x02},
		{"TRIPle", 0x03},
		{"LONG", 0x0f},
		{"LLONg", 0x0ff},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	APP_SCPI_TRACE("TR SCPI: CONTROL KEY");
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: CONTROL KEY PARA ERROR");
		return SCPI_RES_ERR;
	}
	APP_SCPI_TRACE("TR SCPI: CONTROL KEY %s PRESS", scpi_choice_def[intval].name);
	app_scpi_touch_control(intval);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Switch_ANC_Mode(scpi_t * context)
{
	app_scpi_switch_anc_mode();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Get_ANC_Mode(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"OFF", 0},
		{"HEARthrough", 1},
		{"ANC", 2},
		SCPI_CHOICE_LIST_END 
	};
	int anc_mode = 3;
	anc_mode = app_scpi_get_anc_mode();
	APP_SCPI_TRACE("TR SCPI: ANC MODE IS %s", scpi_choice_def[anc_mode].name);
	SCPI_ResultMnemonic(context, scpi_choice_def[anc_mode].name);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Switch_Mic(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"FF", 0},
		{"FB", 1},
		{"TALK", 2},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	APP_SCPI_TRACE("TR SCPI: SWITCH MIC");
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SWITCH MIC PARA ERROR");
		return SCPI_RES_ERR;
	}
	else
	{
		APP_SCPI_TRACE("TR SCPI: SWITCH TO %s MIC", scpi_choice_def[intval].name);
		app_scpi_loopback_switch_mic(intval);
	}
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Switch_Sco_Mic(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"NORMal", 0},
		{"FF", 1},
		{"TALK", 2},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	APP_SCPI_TRACE("TR SCPI: SWITCH SCO MIC");
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SWITCH SCO MIC PARA ERROR");
		return SCPI_RES_ERR;
	}
	else
	{
		APP_SCPI_TRACE("TR SCPI: SWITCH TO %s SCO MIC", scpi_choice_def[intval].name);
		app_scpi_sco_switch_mic(intval);
	}
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Dut_Mode(scpi_t * context)
{
	bool value_return = false;
	if(!SCPI_ParamBool(context,&value_return,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: DUT MODE PARA ERROR");
		return SCPI_RES_ERR;
	}
	else
	{
		if(value_return)
		{
			APP_SCPI_TRACE("TR SCPI: DUT MODE ENTER");
			app_scpi_entry_dut();
		}
		else
		{
			APP_SCPI_TRACE("TR SCPI: DUT MODE REBOOT");
			app_scpi_reboot();
		}
	}
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}

#if defined(__IR_W2001__)
scpi_result_t SCPI_Write_IR_Value(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"CT", 0},
		{"MM5", 1},
		{"NONE", 2},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	int32_t value;
	APP_SCPI_TRACE("TR SCPI: SET IR VALUE");
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SET IR VALUE PARA1 ERROR");
		return SCPI_RES_ERR;
	}
	if(!SCPI_ParamInt32(context,&value,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SET IR VALUE PARA2 ERROR");
		return SCPI_RES_ERR;
	}
	APP_SCPI_TRACE("TR SCPI: SET %s IR VALUE: %d",scpi_choice_def[intval].name, value);
	app_scpi_set_ir_calib_value(intval, value);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Read_IR_Value(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"CT", 0},
		{"MM5", 1},
		{"NONE", 2},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	uint32_t value;
	APP_SCPI_TRACE("TR SCPI: READ IR VALUE");
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: READ IR VALUE PARA1 ERROR");
		return SCPI_RES_ERR;
	}
	app_scpi_get_ir_calib_value(intval, &value);
	APP_SCPI_TRACE("TR SCPI: %s IR VALUE is %d",scpi_choice_def[intval].name, value);
	SCPI_ResultInt32(context, value);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_IR_Cal(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"CT", 0},
		{"MM5", 1},
		{"NONE", 2},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	uint32_t value;
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: READ IR CALIB VALUE PARA1 ERROR %d",intval);
		return SCPI_RES_ERR;
	}
	value = app_scpi_set_ir_cabli_value((uint8_t )intval);
	APP_SCPI_TRACE("TR SCPI: %s IR CALIB VALUE is %d",scpi_choice_def[intval].name, value);
	SCPI_ResultInt32(context, value);
	return SCPI_RES_OK;
}
#endif //__IR_W2001__

scpi_result_t SCPI_OTA(scpi_t * context)
{
	extern void app_otaMode_enter(APP_KEY_STATUS *status, void *param);

	app_otaMode_enter(NULL, NULL);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_XTAL_Cal(scpi_t * context)
{
	uint32_t value_return;
	unsigned int set_value;
	
	if(!SCPI_ParamUInt32(context,&value_return,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SCPI XTAL CALIb PARA ERROR");
		return SCPI_RES_ERR;
	}
	set_value = (unsigned int )value_return;
	app_scpi_set_xtal_fcap(set_value);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_DUT_SCPI_Mode(scpi_t * context)
{
	bool value_return = false;
	if(!SCPI_ParamBool(context,&value_return,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SCPI MODE PARA ERROR");
		return SCPI_RES_ERR;
	}
	if(value_return)
	{
		app_scpi_entry_scpi();
		APP_SCPI_TRACE("TR SCPI: SCPI MODE ON");
	}
	else
	{
		app_scpi_exit_scpi();
		app_scpi_sco_switch_mic(0x00);
		APP_SCPI_TRACE("TR SCPI: SCPI MODE OFF");
	}
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}
	
scpi_result_t SCPI_DUT_Set_Freeman_Pair(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: FREEMAN PAIR");
	app_scpi_freeman_pair();
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_DUT_Set_Pair_Peer(scpi_t * context)
{
	const char * addr;
	size_t value_len;
	if(!SCPI_ParamCharacters(context,&addr,&value_len, FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: PAIR INSTRUCT ERROR");
		return SCPI_RES_ERR;
	}

	char baddr[30];
	uint8_t btaddr[6]={0,0,0,0,0,0};
	uint32_t hex_value;
	for(int i=0;i<6;i++)
	{
		strncpy(baddr, addr+3*i, 2);
		strBaseToUInt32(baddr,&hex_value,16);
		btaddr[5-i] = hex_value;
	}
	bool role;
	role = app_scpi_get_earside();
	app_scpi_peer_pair(btaddr,role);
	APP_SCPI_TRACE("TR SCPI: PAIR PEER BT ADDR:%x:%x:%x:%x:%x:%x",btaddr[5],btaddr[4],btaddr[3],btaddr[2],btaddr[1],btaddr[0]);
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_DUT_Shut_Down(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: SHUT DOWN");
	SCPI_ResultBool(context, 1);
	app_scpi_shutdown();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_DUT_Reoot(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: REBOOT START");
	SCPI_ResultBool(context, 1);
	app_scpi_reboot();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_DUT_Factory_Reset(scpi_t * context)
{
	APP_SCPI_TRACE("TR SCPI: EARPHONE RESET FACTORY");
	SCPI_ResultBool(context, 1);
	app_scpi_restore_factory_settings_reboot();
	return SCPI_RES_OK;
}

scpi_result_t SCPI_DUT_Set_Role(scpi_t * context)
{
	const scpi_choice_def_t scpi_choice_def[] = {
		{"LEFT", 0},
		{"RIGHt", 1},
		SCPI_CHOICE_LIST_END 
	};
	int32_t intval;
	APP_SCPI_TRACE("TR SCPI: SET ROLE");
	if(!SCPI_ParamChoice(context,scpi_choice_def,&intval,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SET ROLE PARA ERROR");
		return SCPI_RES_ERR;
	}
	else
	{
		APP_SCPI_TRACE("TR SCPI: SET ROLE %s", scpi_choice_def[intval].name);
		app_scpi_set_earside(intval);
	}
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}

scpi_result_t SCPI_DUT_Wear_Report(scpi_t * context)
{
	bool value_return = false;
	if(!SCPI_ParamBool(context,&value_return,FALSE))
	{
		APP_SCPI_TRACE("TR SCPI: SCPI_DUT_Wear_Report PARA ERROR");
		return SCPI_RES_ERR;
	}
	if(value_return)
	{
		ui_device_status.local_status.wear_report_status =1;
		APP_SCPI_TRACE("TR SCPI: SCPI_DUT_Wear_Report ON");
	}
	else
	{
		ui_device_status.local_status.wear_report_status =0;
		APP_SCPI_TRACE("TR SCPI: SCPI_DUT_Wear_Report OFF");
	}
	SCPI_ResultBool(context, 1);
	return SCPI_RES_OK;
}

#endif
