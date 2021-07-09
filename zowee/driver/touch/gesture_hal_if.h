#ifndef __GESTURE_HAL_IF__
#define	__GESTURE_HAL_IF__

#ifdef __APP_GESTURE_ENABLE__

#include "hal_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "hal_trace.h"



#define	GESTURE_HAL_TRACE_ENABLE		1

#if GESTURE_HAL_TRACE_ENABLE
#define GESTURE_HAL_TRACE(s,...)		TRACE(1, "[GESTURE_HAL]%s " s, __func__, ##__VA_ARGS__)
#else
#define GESTURE_HAL_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HAL_TOUCH_EVENT_NONE = 0,
    HAL_TOUCH_EVENT_CLICK = 1,
    HAL_TOUCH_EVENT_DOUBLE_CLICK = 2,
    HAL_TOUCH_EVENT_THREE_CLICK = 3,
    HAL_TOUCH_EVENT_FOUR_CLICK = 4,
    HAL_TOUCH_EVENT_FIVE_CLICK = 5,
    HAL_TOUCH_EVENT_LONGPRESS = 6,
    HAL_TOUCH_EVENT_LONGPRESS_1S = 7,
    HAL_TOUCH_EVENT_LONGPRESS_3S = 8,
    HAL_TOUCH_EVENT_LONGPRESS_5S = 9,
    HAL_TOUCH_EVENT_UP_SLIDE = 10,
    HAL_TOUCH_EVENT_DOWN_SLIDE = 11,
    HAL_TOUCH_EVENT_LEFT_SLIDE = 12,
    HAL_TOUCH_EVENT_RIGHT_SLIDE = 13,  
    HAL_TOUCH_EVENT_SEND_DATA = 14,  
} hal_touch_event_type_enum;

typedef enum
{
    HAL_TOUCH_REMOVED = 0,
    HAL_TOUCH_NEAR
} hal_touch_wear_status_enum;

typedef void (*hal_touch_gesture_cb_func)(hal_touch_event_type_enum event, uint8_t *data, uint16_t len);
typedef void (*hal_touch_wear_cb_func)(hal_touch_wear_status_enum status);

typedef struct
{
    /*
    * Select the touch IC driver and initialize the IC.
    * void:
    * return: 0: initialize success, -1: initialize fail.
    */       
    int32_t (* init)(void);
#if 0
    /*
    * Get the chip ID of the touc IC.
    * chip_id: memory the chip ID
    * return: 0: get success, -1: get fail.
    */      
    int32_t (* get_chip_id)(uint32_t *chip_id);

    /*
    * Control the touch IC soft or hard reset.
    * void:
    * return: 0: reset success, -1: reset fail.
    */      
    int32_t (* reset)(void);

    /*
    * When IC is struck by static electricity, check whether the touch IC works normally.
    * void:
    * return: 0: ic works normally, -1: ic is dead.
    */        
    int32_t (* check_esd)(void);

    /*
    * Set the callback function when the touch IC gestures event is generated.
    * void:
    * return: 0: set success, -1: set fail.
    */         
    int32_t (* set_gesture_cb)(hal_touch_gesture_cb_func cb);

    /*
    * Set the callback function when the touch IC wear status changes.
    * void:
    * return: 0: set success, -1: set fail.
    */         
    int32_t (* set_wear_cb)(hal_touch_wear_cb_func cb);

    /*
    * After calling this function, the touch IC enter normal mode.
    * void:
    * return: 0: set success, -1: set fail.
    */        
    int32_t (* enter_normal_mode)(void);   

    /*
    * After calling this function, the touch IC enter standby mode.
    * void:
    * return: 0: set success, -1: set fail.
    */        
    int32_t (* enter_standby_mode)(void);

    /*
    * Set the touch sensitivity of touch IC.
    * void:
    * return: 0: set success, -1: set fail.
    */        
    int32_t (* set_sensitivity_level)(uint8_t level);

    /*
    * After calling this function, the touch IC begins to measure.
    * void:
    * return: 0: start success, -1: start fail.
    */     
    int32_t (* wear_start_measure)(void);

    /*
    * After calling this function, the touch IC stops to measure.
    * void:
    * return: 0: stop success, -1: stop fail.
    */      
    int32_t (* wear_stop_measure)(void);

	int32_t (* get_gesture_status)(uint8_t *sts);

	void (*iqs_touch_handler)(uint8_t str);

	void (*iqs_wear_handler)(uint8_t str);
#endif
	void (* int_handle)(void);
	void (* disable_gesture)(void);
	void (* enable_gesture)(void);
} hal_gesture_if_t;

void gesture_hal_if_init(void);
void gesture_hal_if_int_handler(void);
void gesture_hal_if_disable(void);
void gesture_hal_if_enable(void);






#ifdef __cplusplus
}
#endif


#endif//__APP_GESTURE_ENABLE__

#endif
