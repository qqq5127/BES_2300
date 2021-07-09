#ifndef	DEBUG
#include "vbus_uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* bes platform stuff  */
#include "hal_uart.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "app_utils.h"
#include "cmsis.h"
#include "vbus_protocol.h"
#include "apps.h"
#include "hal_timer.h"
#include "hal_iomux_best2300p.h"
#include "zowee_config.h"
#include "ui_event_manage.h"
#include "ui_event_vbus.h"
#include "apps.h"
#include "pmu.h"
#include "app_battery.h"
#include "battery_hal_if.h"
#include "ui_event_product_test.h"
#include "log_uart.h"
#include "ui_event_scpi.h"

const uint8_t entry_dfu_string[] = "DUT:MODE? 1"; 
static uint8_t log_uart_buff[20];
static uint8_t log_uart_length = 0;

static void app_log_uart_timeout_handler(void const *param);
osTimerDef (LOG_UART_TIMER, app_log_uart_timeout_handler);
static osTimerId log_uart_timer = NULL;


static struct HAL_UART_CFG_T uart_cfg = 
{
	.parity = HAL_UART_PARITY_NONE,
	.stop = HAL_UART_STOP_BITS_1,
	.data = HAL_UART_DATA_BITS_8,
	.flow = HAL_UART_FLOW_CONTROL_NONE,
	.rx_level = HAL_UART_FIFO_LEVEL_1_4,
	.tx_level = HAL_UART_FIFO_LEVEL_1_2,
	.baud = 120000,
	.dma_rx = false,
	.dma_tx = false,
	.dma_rx_stop_on_err = false
};


//log uart uninit
static void app_log_uart_uninit(void)
{
    union HAL_UART_IRQ_T mask;

    uint32_t lock = int_lock();
    mask.reg = 0;
    hal_uart_irq_set_mask(HAL_UART_ID_0, mask);
    int_unlock(lock);
}

static void app_log_uart_timeout_handler(void const *param)
{
	app_log_uart_uninit();
}


//rx receive handler
static void hal_log_tx_rx_irq_handler(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status)
{	
	if (status.RX || status.RT)
	{
		while (hal_uart_readable(HAL_UART_ID_0))
		{
			uint8_t data;
			data = hal_uart_getc(HAL_UART_ID_0);
			//hal_uart_blocked_putc(HAL_UART_ID_0,data);
			if(log_uart_length == 0)
			{
				if(data == 'D')
				{
					log_uart_buff[log_uart_length] = data;
					log_uart_length++;
					if(log_uart_length >= sizeof(log_uart_buff))
					{
						memset(log_uart_buff,0,sizeof(log_uart_buff));
						log_uart_length = 0;
					}
				}
			}
			else
			{
				log_uart_buff[log_uart_length] = data;
				log_uart_length++;
				if(log_uart_length >= sizeof(log_uart_buff))
				{
					memset(log_uart_buff,0,sizeof(log_uart_buff));
					log_uart_length = 0;
				}
			}
			//prepare to recevie

		}
	}
	if (status.TX)
    {
    }

    if ( status.FE || status.OE || status.PE || status.BE)
    {
    }
		
    if(log_uart_length > 11)
    {
		log_uart_buff[11] = 0;
		if(memcmp(log_uart_buff,entry_dfu_string,sizeof(entry_dfu_string))	== 0)
		{
			app_ui_event_process(UI_EVENT_SCPI,0,UI_EVENT_SCPI_DUT_MODE,0,0);
			hal_uart_blocked_putc(HAL_UART_ID_0,0x31);
			hal_uart_blocked_putc(HAL_UART_ID_0,0x0a);
		}
		memset(log_uart_buff,0,sizeof(log_uart_buff));
		log_uart_length = 0;
    }

}

static void app_log_uart_rx_tx_int_start(void)
{
    union HAL_UART_IRQ_T mask;

    uint32_t lock = int_lock();
    hal_uart_flush(VBUS_UART_PORT, 0);
    mask.reg = 0;
    mask.BE = 1;
    mask.RT = 1;
    mask.RX = 1;
    mask.TX = 1;
    hal_uart_irq_set_mask(HAL_UART_ID_0, mask);
    hal_uart_irq_set_handler(HAL_UART_ID_0, hal_log_tx_rx_irq_handler);
    int_unlock(lock);
}


void app_log_uart_init(void)
{
	hal_iomux_set_uart0();
    hal_uart_close(HAL_UART_ID_0);
    hal_uart_open(HAL_UART_ID_0, &uart_cfg);
	app_log_uart_rx_tx_int_start();

    if (log_uart_timer == NULL) {
        log_uart_timer = osTimerCreate (osTimer(LOG_UART_TIMER), osTimerOnce, NULL);
    }
	osTimerStart(log_uart_timer, 30000); //30 seconds
}



#endif

