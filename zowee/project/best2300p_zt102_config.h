#ifndef	__ZOWEE_CONFIG_H__
#define	__ZOWEE_CONFIG_H__

#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"
#include "hal_aud.h"
#include "hal_uart.h"

#define	AUTO_POWER_OFF_TIME						180	// 180*10 = 1800s
#define LOW_BAT_CHECK_TIME						10	// 10 min low battery prompt report interval

//GPIO
#define TWI_SW_MASTER_SCL_PIN   	            (HAL_IOMUX_PIN_P0_2)
#define TWI_SW_MASTER_SDA_PIN   	            (HAL_IOMUX_PIN_P0_3)
//#define	IR_W2001_INT_PIN						(HAL_IOMUX_PIN_P0_6)
//#define	TOUCH_GPIOTE_INT_PIN					(HAL_IOMUX_PIN_P1_1)

//LDO
//#define	LDO_ENABLE_PIN							(HAL_IOMUX_PIN_P1_5)

#define TOUCH_INT_PIN                           (HAL_IOMUX_PIN_P1_4)
#define TOUCH_LDO_PIN			            	(HAL_IOMUX_PIN_P1_5)

#define VBUS_UART_TX_PIN            (HAL_IOMUX_PIN_P2_1)
#define VBUS_UART_PORT 				HAL_UART_ID_1
#define TWS_SIDE_ROLE_PIN			HAL_IOMUX_PIN_P2_2

//battery
#define	BATTERY_CHARGE_FULL_INT_PIN					(HAL_IOMUX_PIN_P1_0)

#endif//__ZOWEE_CONFIG_H__
