#ifndef __VBUS_HAL_IF__
#define	__VBUS_HAL_IF__

#ifdef __APP_VBUS_ENABLE__

#include "hal_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "hal_trace.h"



#define	VBUS_HAL_TRACE_ENABLE		1

#if VBUS_HAL_TRACE_ENABLE
#define VBUS_HAL_TRACE(s,...)		TRACE(1, "[DRIVER_VBUS]%s " s, __func__, ##__VA_ARGS__)
#else
#define VBUS_HAL_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*hal_vbus_rx_cmd_cb_func)(uint8_t *data, uint16_t len);

typedef struct
{
    /*
    * Select the vbus driver and initialize the IC.
    * void:
    * return: 0: initialize success, -1: initialize fail.
    */       
    int32_t (* init)(void);
	
	/********* added by rh 20210107  start	*/
	void (* uninit)(void);
	/********* added by rh 20210107  end	*/
	/* 
	*vbus irq handler
	*/
	void (* int_handle)(void);
	/*
    * Set the callback function when the VBUS IC cmd rx is Done.
    * void:
    * return: 0: set success, -1: set fail.
    */ 
    int32_t (*set_cmd_rx_cb)(hal_vbus_rx_cmd_cb_func cb);
    /*
	*
    */
	void (*send_data)(uint8_t *buff,uint8_t length);

} hal_vbus_if_t;

void vbus_hal_if_init(void);
void vbus_hal_if_uninit(void);

void vbus_hal_if_int_handler(void);
void vbus_hal_if_send_data(uint8_t *buff,uint8_t length);
void vbus_hal_if_rx_start_handler(void);

#ifdef __cplusplus
}
#endif


#endif//__APP_VBUS_ENABLE__

#endif//__TOUCH_HAL_IF__
