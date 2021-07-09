#include "stdint.h"
#include "stdbool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "twi_hal_if.h"
#include "zowee_config.h"
#include "twi_hw_master.h"
#include "twi_sw_master.h"


static hal_twi_if_t	hal_twi_if;

void twi_hal_if_init(void)
{
	memset(&hal_twi_if,0,sizeof(hal_twi_if));
	
#ifdef __BUS_TWI_HW_TWI__
	hal_twi_if.init = twi_hw_master_init;
	hal_twi_if.read = twi_hw_master_read;
	hal_twi_if.write = twi_hw_master_write;
	hal_twi_if.uninit = twi_hw_master_uninit;
#endif
	
#ifdef __BUS_TWI_SW_TWI__
	hal_twi_if.init = twi_sw_master_init;
	hal_twi_if.read = twi_sw_master_read;
	hal_twi_if.write = twi_sw_master_write;
	hal_twi_if.uninit = twi_sw_master_uninit;
#endif

	if(hal_twi_if.init != NULL)
	{
		hal_twi_if.init();
	}
}


bool twi_hal_if_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size)
{
	if(hal_twi_if.read != NULL)
	{
		return hal_twi_if.read(address,reg_addr,data,rx_size);
	}
	return false;
}

bool twi_hal_if_write(uint8_t address, uint8_t *data, uint32_t reg_len, uint32_t value_len)
{
	if(hal_twi_if.write != NULL)
	{
		return hal_twi_if.write(address,data,reg_len,value_len);
	}
	return false;
}

void twi_hal_if_uninit(void)
{
	if(hal_twi_if.uninit != NULL)
	{
		hal_twi_if.uninit();
	}
}

