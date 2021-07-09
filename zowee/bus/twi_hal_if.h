#ifndef __TWI_HAL_IF_H
#define __TWI_HAL_IF_H

#include "hal_trace.h"



#define	TWI_HAL_TRACE_ENABLE		1

#if TWI_HAL_TRACE_ENABLE
#define TWI_HAL_TRACE(s,...)		TRACE(1, "[BUS_TWI]%s " s, __func__, ##__VA_ARGS__)
#else
#define TWI_HAL_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif

void twi_hal_if_init(void);
bool twi_hal_if_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size);
bool twi_hal_if_write(uint8_t address, uint8_t *data, uint32_t reg_len, uint32_t value_len);
void twi_hal_if_uninit(void);


typedef struct
{
    void (* init)(void);
    bool (* read)(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size);
    bool (* write)(uint8_t address, uint8_t *data, uint16_t reg_len, uint32_t value_len);
    void (* uninit)(void);
} hal_twi_if_t;


#ifdef __cplusplus
}
#endif



#endif
