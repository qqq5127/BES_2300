#ifndef __SIMULATE_I2C__
#define __SIMULATE_I2C__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_gpio.h"
#include "reg_gpio.h"
#include "plat_types.h"

#define BOARD_HW_GPIO_I2C_MODULE

#ifdef BOARD_HW_GPIO_I2C_MODULE
#define I2C_SLEEP_TIME 4

#define BOARD_IOMUX_GPIO_I2C_SCL            HAL_IOMUX_PIN_P1_0
#define BOARD_IOMUX_GPIO_I2C_SDA            HAL_IOMUX_PIN_P1_1
#define BOARD_GPIO_GPIO_I2C_SCL             HAL_GPIO_PIN_P1_0
#define BOARD_GPIO_GPIO_I2C_SDA             HAL_GPIO_PIN_P1_1

#define I2C_IOMUX_SDA   BOARD_IOMUX_GPIO_I2C_SDA
#define I2C_IOMUX_SCL   BOARD_IOMUX_GPIO_I2C_SCL
#define I2C_SDA_PIN     BOARD_GPIO_GPIO_I2C_SDA
#define I2C_SCK_PIN     BOARD_GPIO_GPIO_I2C_SCL


void i2c_init(void);
int i2c_write_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len);
int i2c_read_reg_data(u8 device_addr,u8 reg_addr,u8 *buf,int len);
int GetI2CData(u8 *pData, u8 u8Reg, int u16Len);
#endif

#ifdef __cplusplus
}
#endif
#endif

