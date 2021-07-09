#ifndef __TWI_HW_MASTER_H
#define __TWI_HW_MASTER_H

#ifdef __BUS_TWI_HW_TWI__


#include "hal_gpio.h"
#include "zowee_config.h"
#include "hal_i2c.h"
#include "stdint.h"
#include "stdbool.h"
#include "hal_trace.h"

//#include "project_config.h"



#define	TWI_HW_TRACE_ENABLE		1

#if TWI_HW_TRACE_ENABLE
#define TWI_HW_TRACE(s,...)		TRACE(1, "[BUS_TWI_HW]%s " s, __func__, ##__VA_ARGS__)
#else
#define TWI_HW_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif


void twi_hw_master_init(void);
bool twi_hw_master_write(uint8_t address, uint8_t *data, uint16_t reg_len, uint32_t value_len);
bool twi_hw_master_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size);
void twi_hw_master_uninit(void);

#ifdef __cplusplus
}
#endif

#endif//__BUS_TWI_HW_TWI__

#endif

