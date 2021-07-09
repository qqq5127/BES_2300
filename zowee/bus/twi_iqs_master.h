#ifndef __TWI_MASTER_H
#define __TWI_MASTER_H

#if 1//defined(__TWI_MASTER_CTRL__)
#include "hal_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "hal_timer.h"
#include "best2300p_zt102_config.h"


#define	TWI_IQS_MASTER_TRACE_ENABLE		1

#if TWI_IQS_MASTER_TRACE_ENABLE
#define TWI_IQS_MASTER_TRACE(s,...)		TRACE(1, "[TWI_IQS_MASTER]%s " s, __func__, ##__VA_ARGS__)
#else
#define TWI_IQS_MASTER_TRACE(s,...)
#endif



#ifdef __cplusplus
extern "C" {
#endif

#define TWI_READ_BIT                (0x01)        //!< If this bit is set in the address field, transfer direction is from slave to master.

#define TWI_ISSUE_STOP              ((bool)true)  //!< Parameter for @ref twi_master_transfer
#define TWI_DONT_ISSUE_STOP         ((bool)false) //!< Parameter for @ref twi_master_transfer

#define TWI_SW_SDA_HIGH()           do { hal_gpio_pin_set((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SDA_PIN); } while(0)    /*!< Pulls SDA line high */
#define TWI_SW_SDA_LOW()            do { hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SDA_PIN); } while(0)    /*!< Pulls SDA line low  */
#define TWI_SW_SDA_INPUT()          do { hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SDA_PIN, HAL_GPIO_DIR_IN, 1);  } while(0)   /*!< Configures SDA pin as input  */
#define TWI_SW_SDA_OUTPUT()         do { hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SDA_PIN, HAL_GPIO_DIR_OUT,1);  } while(0)   /*!< Configures SDA pin as output */
#define TWI_SW_SCL_OUTPUT()         do { hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SCL_PIN, HAL_GPIO_DIR_OUT,1);  } while(0)   /*!< Configures SCL pin as output */
#define TWI_SW_SCL_OUTPUTL()        do { hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SCL_PIN, HAL_GPIO_DIR_OUT,0);  } while(0)   /*!< Configures SCL pin as output */
#define TWI_SW_SCL_INPUT()          do { hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SCL_PIN, HAL_GPIO_DIR_IN,1);  } while(0)   /*!< Configures SCL pin as output */
#define TWI_SW_SCL_HIGH()           TWI_SW_SCL_INPUT() 
#define TWI_SW_SCL_LOW()            TWI_SW_SCL_OUTPUTL()


#define TWI_SW_SDA_READ()           (hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SDA_PIN))               /*!< Reads current state of SDA */
#define TWI_SW_SCL_READ()           (hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)TWI_SW_MASTER_SCL_PIN))               /*!< Reads current state of SCL */

#define TWI_RAM_52M_DELAYN()        hal_sys_timer_delay_ns(100)
#define TWI_RAM_SHORT_DELAYN()      hal_sys_timer_delay_ns(200)
#define TWI_RAM_DELAYN()         	hal_sys_timer_delay_ns(200)

#define TWI_SW_DELAYN_SHORT()       TWI_RAM_SHORT_DELAYN()
#define TWI_SW_DELAYN()             TWI_RAM_DELAYN()
#define TWI_RH_ACK_DELAYN()			hal_sys_timer_delay_ns(20000)//80000


void twi_master_init(void);
bool twi_iqs_master_write(uint8_t address, uint8_t reg_addr, uint8_t *data_buffer, uint8_t len);
bool twi_iqs_master_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size);
bool twi_iqs_sw_master_transfer(uint8_t address, uint8_t *data, uint16_t data_length, bool issue_stop_condition);
void twi_iqs_master_init(void);
bool twi_iqs_sw_master_init(void);

struct iqs_iic_state
{
	uint8_t i2c_stop_bit;
};

#ifdef __cplusplus
}
#endif

#endif

#endif

