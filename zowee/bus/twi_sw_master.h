#ifndef __TWI_SW_MASTER_H
#define __TWI_SW_MASTER_H

#ifdef __BUS_TWI_SW_TWI__

#include "hal_gpio.h"
#include "zowee_config.h"
#include "stdint.h"
#include "stdbool.h"
#include "tgt_hardware.h"
#include "hal_timer.h"
#include "twi_hal_if.h"
#include "hal_trace.h"



#define	TWI_SW_TRACE_ENABLE		1

#if TWI_SW_TRACE_ENABLE
#define TWI_SW_TRACE(s,...)		TRACE(1, "[BUS_TWI_SW]%s " s, __func__, ##__VA_ARGS__)
#else
#define TWI_SW_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif

void twi_sw_master_init(void);
bool twi_sw_master_write(uint8_t address, uint8_t *data, uint16_t reg_len, uint32_t value_len);
bool twi_sw_master_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size);
void twi_sw_master_uninit(void);

#ifdef __cplusplus
}
#endif

#endif

/*lint --flb "Leave library region" */
#endif

