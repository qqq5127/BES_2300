#ifndef __WEAR_HAL_IF__
#define	__WEAR_HAL_IF__

#ifdef __APP_WEAR_ENABLE__

#include "hal_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "hal_trace.h"



#define	WEAR_HAL_TRACE_ENABLE		1

#if WEAR_HAL_TRACE_ENABLE
#define WEAR_HAL_TRACE(s,...)		TRACE(1, "[DRIVER_WEAR]%s " s, __func__, ##__VA_ARGS__)
#else
#define WEAR_HAL_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    int (* init)(void);
    void (* int_handle)(void);
    void (* disable_wear)(void);
	void (* enable_wear)(void);
	void (* refresh_flash_data)(void);
} hal_wear_if_t;

void wear_hal_if_init(void);
void wear_hal_if_int_handler(void);
void wear_hal_if_enable(void);
void wear_hal_if_disable(void);
void wear_hal_if_refresh_cali_data(void);

#ifdef __cplusplus
}
#endif

#endif//__APP_WEAR_ENABLE__

#endif
