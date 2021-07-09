#ifndef	__BATTERY_ZT203_H__
#define	__BATTERY_ZT203_H__

#if __BATTERY_ZT203__

#include "hal_gpio.h"
#include "zowee_config.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "battery_hal_if.h"



#define	BATTERY_ZT203_TRACE_ENABLE		1

#if BATTERY_ZT203_TRACE_ENABLE
#define BATTERY_ZT203_TRACE(s,...)		TRACE(1, "[BATTERY_ZT203]%s " s, __func__, ##__VA_ARGS__)
#else
#define BATTERY_ZT203_TRACE(s,...)
#endif


#define	BATTERY_TEST_TEMPERATURE	0

#define BATTERY_ZT203_FULL_TIMEROUT_COUNT	60	// 60 min



typedef struct
{
	uint16_t vot;
	const char *temperature;
}NTC_TALBE;

typedef enum
{
	NTC_TEMPERATURE_COLD = 1,
	NTC_TEMPERATURE_LOW,
	NTC_TEMPERATURE_NORMAL,
	NTC_TEMPERATURE_HOT,
}BATTERY_NTC_STATUS;

#ifdef __cplusplus
extern "C" {
#endif

const struct HAL_IOMUX_PIN_FUNCTION_MAP battery_full_int_cfg = {
	BATTERY_CHARGE_FULL_INT_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP battery_enable_cfg = {
	BATTERY_CHARGE_ENABLE_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_NOPULL
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP battery_control_cfg = {
	BATTERY_CHARGE_CONTROL_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_NOPULL
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP battery_ntc_enable_cfg = {
	BATTERY_NTC_ENABLE_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_NOPULL
};

void battery_zt203_init(void);
void battery_zt203_uninit(void);
void battery_zt203_plugin(void);
bool battery_zt203_plugout(void);
void battery_zt203_entry_low_power(void);
void battery_zt203_exit_low_power(void);
uint8_t battery_zt203_status(void);
	


#ifdef __cplusplus
}
#endif

#define	BATTERY_POWER_DOWN_VOLTAGE				3300					

const uint16_t battery_discharging_level_table[10] = 
{
	3580, //
	3699, //
	3720, //
	3731, //
	3765, //
	3879, //
	3860, //
	3920, //
	4020, //
	4200, //
};


const uint16_t battery_charging_level_table[10] = 
{
	3745, //
	3807, //
	3835, //
	3849, //
	3874, //
	3920, //
	3967, //
	4027, //
	4132, //
	4160, //
};


const hal_battery_if_t battery_ctrl =
{
	battery_zt203_init,
	battery_zt203_uninit,
	battery_zt203_plugin,
	battery_zt203_plugout,
	battery_zt203_status,
};

#endif

#endif
