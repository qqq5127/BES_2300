#ifndef	__BATTERY_ZT102_H__
#define	__BATTERY_ZT102_H__

#if __BATTERY_ZT102__

#include "hal_gpio.h"
#include "zowee_config.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "battery_hal_if.h"



#define BATTERY_ZT102_TRACE_ENABLE		1

#if (BATTERY_ZT102_TRACE_ENABLE)
#define BATTERY_ZT102_TRACE(s, ...)		TRACE(1, "[BATTERY_ZT102]%s " s, __func__, ##__VA_ARGS__)
#else
#define BATTERY_ZT102_TRACE(s, ...)
#endif




#define BATTERY_ZT102_FULL_TIMEROUT_COUNT	60	//  60 min



#ifdef __cplusplus
extern "C" {
#endif


const struct HAL_IOMUX_PIN_FUNCTION_MAP battery_full_int_cfg = {
	BATTERY_CHARGE_FULL_INT_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE
};


void battery_zt102_init(void);
void battery_zt102_uninit(void);
void battery_zt102_plugin(void);
bool battery_zt102_plugout(void);
uint8_t battery_zt102_status(void);
	


#ifdef __cplusplus
}
#endif

#define	BATTERY_POWER_DOWN_VOLTAGE				3300					

const uint16_t battery_discharging_level_table[10] = 
{
	3500, //10-0.4v
	3600, //20-0.1v
	3700, //30-0.1v
	3780, //40-0.08v
	3860, //50-0.08v
	3930, //60-0.07v
	4000, //70-0.07v
	4060, //80-0.06v
	4120, //90-0.06v
	4200,//100-0.08v
};


const uint16_t battery_charging_level_table[10] = 
{
	3700, //10-0.6v	   10min    3min(average/realtime)
	3800, //20-0.1v		6min	5min
	3870, //30-0.07v	7min	7min		
	3910, //40-0.04v	7min    8min
	3950, //50-0.04v	5min	7min
	4000, //60-0.05v	5min	6min
	4050, //70-0.05v	5min	4min
	4100, //80-0.05v	5min	4min
	4150, //90-0.05v	5min	4min
	4200,//100-0.05v	6min	4min
};


const hal_battery_if_t battery_ctrl =
{
	battery_zt102_init,
	battery_zt102_uninit,
	battery_zt102_plugin,
	battery_zt102_plugout,
	battery_zt102_status,
};

#endif

#endif
