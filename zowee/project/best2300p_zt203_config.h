#ifndef	__ZOWEE_CONFIG_H__
#define	__ZOWEE_CONFIG_H__

#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"
#include "hal_aud.h"
#include "hal_uart.h"

#define	AUTO_POWER_OFF_TIME						60	// 60*10 = 600s
#define LOW_BAT_CHECK_TIME						5	// 5 min low battery prompt report interval

//GPIO
#define TWI_SW_MASTER_SCL_PIN   	            (HAL_IOMUX_PIN_P2_2)
#define TWI_SW_MASTER_SDA_PIN   	            (HAL_IOMUX_PIN_P2_3)
#define	IR_W2001_INT_PIN						(HAL_IOMUX_PIN_P0_6)
#define	TOUCH_GPIOTE_INT_PIN					(HAL_IOMUX_PIN_P1_1)

//LDO
#define	LDO_ENABLE_PIN							(HAL_IOMUX_PIN_P1_5)

//VBUS
#define VBUS_UART_TX_PIN            (HAL_IOMUX_PIN_P2_1)
#define VBUS_UART_PORT 				HAL_UART_ID_1

//battery
#define	BATTERY_CHARGE_FULL_INT_PIN					(HAL_IOMUX_PIN_P1_4)
#define	BATTERY_CHARGE_ENABLE_PIN					(HAL_IOMUX_PIN_P1_3)
#define	BATTERY_CHARGE_CONTROL_PIN					(HAL_IOMUX_PIN_P2_7)
#define	BATTERY_NTC_ENABLE_PIN						(HAL_IOMUX_PIN_P1_0)

#endif//__ZOWEE_CONFIG_H__
