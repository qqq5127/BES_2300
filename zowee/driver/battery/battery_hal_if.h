#ifndef __BATTERY_HAL_IF__
#define	__BATTERY_HAL_IF__

#ifdef __APP_BATTERY_USER__

#include "hal_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "hal_trace.h"


#define	BATTERY_HAL_TRACE_ENABLE		1

#if BATTERY_HAL_TRACE_ENABLE
#define BATTERY_HAL_TRACE(s,...)		TRACE(1, "[DRIVER_BATTERY]%s " s, __func__, ##__VA_ARGS__)
#else
#define BATTERY_HAL_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	CHARGING_PLUG_IN  = 0,
	CHARGING_PLUG_OUT,
	CHARGING_FULL,

	CHARGING_NONE,
}CHARGING_BATTERY_STATUS;

typedef struct
{
    void (* init)(void);
	void (* uninit)(void);
    void (* plug_in)(void);
    bool (* plug_out)(void);
    uint8_t (* charging_status)(void);
} hal_battery_if_t;

void battery_hal_if_init(void);
void battery_hal_if_uninit(void);
bool battery_hal_if_plug_out(void);
void battery_hal_if_plug_in(void);
uint8_t battery_hal_if_charging_status(void);


#ifdef __cplusplus
}
#endif

#endif//__APP_WEAR_ENABLE__

#endif
