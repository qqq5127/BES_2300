#include "twi_hw_master.h"

#ifdef __BUS_TWI_HW_TWI__
#include "tgt_hardware.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_iomux.h"

void twi_hw_master_init(void)
{
    struct HAL_I2C_CONFIG_T hal_i2c_cfg;
    
	hal_iomux_set_i2c1();

    hal_i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
    hal_i2c_cfg.use_dma  = 0;
    hal_i2c_cfg.use_sync = 1;
    hal_i2c_cfg.speed = 400000;
    hal_i2c_cfg.as_master = 1;
    hal_i2c_open(HAL_I2C_ID_0, &hal_i2c_cfg);
}

bool twi_hw_master_write(uint8_t address, uint8_t *data, uint16_t reg_len, uint32_t value_len)
{
    bool ret = true;

    ret = !hal_i2c_send(HAL_I2C_ID_0, address, data, reg_len, value_len, 0, NULL);

    return ret;
}

bool twi_hw_master_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size)
{
    bool ret = true;

    ret = !hal_i2c_send(HAL_I2C_ID_0, address, (uint8_t *)&reg_addr, 1, 0, 0, NULL);
    if (ret == false)
    {
        return false;
    }

    return !hal_i2c_recv(HAL_I2C_ID_0, address, (uint8_t *)data, 0, rx_size, HAL_I2C_STOP_AFTER_WRITE, 0, NULL);
}

void twi_hw_master_uninit(void)
{
	hal_i2c_close(HAL_I2C_ID_0);
}

#endif

