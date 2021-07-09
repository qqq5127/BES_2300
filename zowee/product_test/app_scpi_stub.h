#ifdef __SCPI_TEST__

#ifndef __SCPI_STUB_H_
#define __SCPI_STUB_H_

#include "hal_trace.h"
#include "scpi.h"

#ifdef __cplusplus
extern "C" {
#endif



#define APP_SCPI_TRACE_ENABLE		1

#if APP_SCPI_TRACE_ENABLE
#define APP_SCPI_TRACE(s,...)		TRACE(1, "[SCPI]%s " s, __func__, ##__VA_ARGS__)
#else
#define APP_SCPI_TRACE(s,...)
#endif

/* scpi test stub type */
scpi_result_t SCPI_Read_IDN(scpi_t * context);

/* SYSTem */
scpi_result_t SCPI_Shut_Down(scpi_t * context);
scpi_result_t SCPI_Reoot(scpi_t * context);
scpi_result_t SCPI_Get_Temperature(scpi_t * context);
scpi_result_t SCPI_Factory_Reset(scpi_t * context);
scpi_result_t SCPI_Get_SW_Version(scpi_t * context);
scpi_result_t SCPI_Get_HW_Version(scpi_t * context);
scpi_result_t SCPI_Get_BT_Address(scpi_t * context);
scpi_result_t SCPI_Get_BT_Name(scpi_t * context);
scpi_result_t SCPI_Get_Touch_State(scpi_t * context);
scpi_result_t SCPI_Set_Box_State(scpi_t * context);
scpi_result_t SCPI_Set_Wear_State(scpi_t * context);
scpi_result_t SCPI_Get_Sys_State(scpi_t * context);
scpi_result_t SCPI_Set_Role(scpi_t * context);
scpi_result_t SCPI_Get_Role(scpi_t * context);
scpi_result_t SCPI_Switch_Role(scpi_t * context);
scpi_result_t SCPI_SCPI_Mode(scpi_t * context);
scpi_result_t SCPI_Get_SCPI_Mode(scpi_t * context);
scpi_result_t SCPI_Return_Head(scpi_t * context);
scpi_result_t SCPI_Project_Name(scpi_t * context);

/* BATTERY */
scpi_result_t SCPI_Get_Charge_State(scpi_t * context);
scpi_result_t SCPI_Get_Battery_Level(scpi_t * context);
scpi_result_t SCPI_Get_Battery_Voltage(scpi_t * context);

/*PAIRing */
scpi_result_t SCPI_Get_Pair_Mode(scpi_t * context);
scpi_result_t SCPI_Get_Pair_State(scpi_t * context);
scpi_result_t SCPI_Set_Freeman_Pair(scpi_t * context);
scpi_result_t SCPI_Set_Pair_Peer(scpi_t * context);
scpi_result_t SCPI_Get_Pair_Peer(scpi_t * context);
scpi_result_t SCPI_Set_Pair_Phone(scpi_t * context);
scpi_result_t SCPI_Get_Pair_Phone(scpi_t * context);

/*HFP*/
//scpi_result_t SCPI_Get_Hfp_Version(scpi_t * context);
scpi_result_t SCPI_Get_HFP_Status(scpi_t * context);
scpi_result_t SCPI_Get_HFP_Volume(scpi_t * context);

/*MUSIc*/
scpi_result_t SCPI_Get_Music_Status(scpi_t * context);
scpi_result_t SCPI_Get_Music_Volume(scpi_t * context);

/*AVRCp*/
scpi_result_t SCPI_Control_Key(scpi_t * context);

/* ANC */
scpi_result_t SCPI_Switch_ANC_Mode(scpi_t * context);
scpi_result_t SCPI_Get_ANC_Mode(scpi_t * context);

/* DUT */
scpi_result_t SCPI_Switch_Mic(scpi_t * context);
scpi_result_t SCPI_Switch_Sco_Mic(scpi_t * context);
scpi_result_t SCPI_Dut_Mode(scpi_t * context);
scpi_result_t SCPI_Write_IR_Value(scpi_t * context);
scpi_result_t SCPI_Read_IR_Value(scpi_t * context);
scpi_result_t SCPI_IR_Cal(scpi_t * context);
scpi_result_t SCPI_OTA(scpi_t * context);
scpi_result_t SCPI_XTAL_Cal(scpi_t * context);
scpi_result_t SCPI_DUT_SCPI_Mode(scpi_t * context);
scpi_result_t SCPI_DUT_Set_Freeman_Pair(scpi_t * context);
scpi_result_t SCPI_DUT_Set_Pair_Peer(scpi_t * context);
scpi_result_t SCPI_DUT_Shut_Down(scpi_t * context);
scpi_result_t SCPI_DUT_Reoot(scpi_t * context);
scpi_result_t SCPI_DUT_Factory_Reset(scpi_t * context);
scpi_result_t SCPI_DUT_Set_Role(scpi_t * context);
scpi_result_t SCPI_DUT_Wear_Report(scpi_t * context);


#ifdef __cplusplus
}
#endif

#endif /* __SCPI_STUB_H_ */

#endif


