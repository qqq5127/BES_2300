#ifdef __APP_VBUS_ENABLE__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt_hardware.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "zowee_config.h"
#include "vbus_hal_if.h"
#include "vbus_uart.h"

extern const hal_vbus_if_t vbus_ctrl;

void vbus_hal_if_send_data(uint8_t *buff,uint8_t length)
{
	vbus_ctrl.send_data(buff,length);
}

void vbus_hal_if_int_handler(void)
{
	vbus_ctrl.int_handle();
}

void vbus_hal_if_init(void)
{
	vbus_ctrl.init();
}

void vbus_hal_if_uninit(void)
{
	vbus_ctrl.uninit();
}


#endif//__APP_VBUS_ENABLE__

