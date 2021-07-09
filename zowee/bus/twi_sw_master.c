#ifdef __BUS_TWI_SW_TWI__
#include "twi_sw_master.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "hal_i2c.h"

#define	READ_BUFF_MAX 30

void twi_sw_master_uninit(void)
{	
	hal_gpio_i2c_close();
}

void twi_sw_master_init(void)
{
    struct HAL_GPIO_I2C_CONFIG_T hal_i2c_cfg;

	hal_i2c_cfg.scl = TWI_SW_MASTER_SCL_PIN;
	hal_i2c_cfg.sda = TWI_SW_MASTER_SDA_PIN;

    hal_gpio_i2c_open(&hal_i2c_cfg);
}

bool twi_sw_master_write(uint8_t address, uint8_t *data, uint16_t reg_len, uint32_t value_len)
{
    bool ret = true;

    ret = !hal_gpio_i2c_send(address, data, reg_len, value_len);

    return ret;
}

bool twi_sw_master_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size)
{
    bool ret = true;
    
	uint8_t twi_read_buff[READ_BUFF_MAX];

    ASSERT((rx_size + 1) < READ_BUFF_MAX, "%s: out_len", __func__);

	twi_read_buff[0] = reg_addr;

	memcpy((twi_read_buff+1),data,rx_size);
	
    ret = hal_gpio_i2c_recv(address, (uint8_t *)twi_read_buff, 1, rx_size, HAL_I2C_STOP_AFTER_WRITE);

	memcpy(data,twi_read_buff+1,rx_size);

	return ret;
}

#endif

