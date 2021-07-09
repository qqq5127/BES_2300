/*
 * vbus_uart.cpp
 *
 * Copyright (C) 2020 ZOWEE Tochnology Co.,Ltd.
 *
 * implementation of commmunication between earbuds and charging case
 * 与充电盒通讯的uart接口实现
 *
 *
 * Written by Stone Lee<litb@zowee.com.cn>
 */
#if defined(__APP_VBUS_ENABLE__) && defined(__VBUS_USE_UART__)
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


static void app_vbus_manage_communicate_timeout_handler(void const *param);
osTimerDef (CHARGER_COMMUNICATE_TIMER, app_vbus_manage_communicate_timeout_handler);
static osTimerId charger_communicate_timer = NULL;


//parameter define
static uint8_t frame_receive_head = 0;
static uint8_t frame_reply_head = 0;

//function define
extern "C" void hal_iomux_uart1_Tx(void);
static void vbus_as_gpioint_init(void);



const struct HAL_IOMUX_PIN_FUNCTION_MAP vbus_uart_tx_pin_cfg = {
	VBUS_UART_TX_PIN,
	HAL_IOMUX_FUNC_AS_GPIO,
	HAL_IOMUX_PIN_VOLTAGE_MEM,
	HAL_IOMUX_PIN_NOPULL
};

static struct HAL_UART_CFG_T uart_cfg = 
{
	.parity = HAL_UART_PARITY_NONE,
	.stop = HAL_UART_STOP_BITS_1,
	.data = HAL_UART_DATA_BITS_8,
	.flow = HAL_UART_FLOW_CONTROL_NONE,
	.rx_level = HAL_UART_FIFO_LEVEL_1_4,
	.tx_level = HAL_UART_FIFO_LEVEL_1_2,
	.baud = USART_REV_BAUD,
	.dma_rx = false,
	.dma_tx = false,
	.dma_rx_stop_on_err = false
};

uint8_t frame_receive_head_get()
{
	return frame_receive_head;
}

//rx receive handler
void hal_box_rx_irq_handler(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status)
{	
	//APP_VUART_TRACE("%s receive data RX=%d, RT=%d.", status.RX,status.RT);
	
	if (status.RX || status.RT)
	{
		while (hal_uart_readable(HAL_UART_ID_1))
		{
			if(vbus_rx_buff.buff_length<VBUS_BUFF_LENGTH)
			{
				vbus_rx_buff.buff[vbus_rx_buff.buff_length] = hal_uart_getc(HAL_UART_ID_1);
#if defined(__VBUS_SUP_OLD__)
				hal_sys_timer_delay_us(100);
#endif
				APP_VUART_TRACE("###Vbus Recv data %02x %08x", vbus_rx_buff.buff[vbus_rx_buff.buff_length], hal_sys_timer_get());
				vbus_rx_buff.buff_length++;				
			}
			else
			{
				memset(&vbus_rx_buff.buff,0,sizeof(vbus_rx_buff.buff));
				vbus_rx_buff.buff_length = 0;
				APP_VUART_TRACE("###Vbus Recv data out of lenght !!!!!!!!!!!");
			}
			//prepare to recevie

		}
		app_ui_event_process(UI_EVENT_VBUS,0,UI_EVENT_VBUS_RX_PROCESS,0,0);
	}
	if (status.TX)
    {
        APP_VUART_TRACE("send\r\n");
    }

    if ( status.FE || status.OE || status.PE || status.BE)
    {
        APP_VUART_TRACE("UASRT ERR!\r\n");
    }
}

int hal_vbus_rx_open(void)
{
    union HAL_UART_IRQ_T mask;

	mask.reg = 0;
#ifdef __UART_RECV_BY_INT__
  //  mask.BE = 1;
//    mask.FE = 1;
//    mask.OE = 1;
//    mask.PE = 1;
    mask.RT = 1;
    mask.RX = 1;
#else
    mask.RX = 1;
#endif
    hal_uart_irq_set_mask(HAL_UART_ID_1, mask);
    hal_uart_irq_set_handler(HAL_UART_ID_1, hal_box_rx_irq_handler);

    return 0;
}

//disable gpio
void vbus_as_gpioint_disable(void)
{
	// disable gpioint
	struct HAL_GPIO_IRQ_CFG_T gpiocfg;
	gpiocfg.irq_enable = false;
	hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)vbus_uart_tx_pin_cfg.pin, &gpiocfg);
}

//enable rx
static void uart_rx_int_start(void)
{
    union HAL_UART_IRQ_T mask;

    uint32_t lock = int_lock();
    hal_uart_flush(VBUS_UART_PORT, 0);
    mask.reg = 0;
    mask.BE = 1;
    mask.RT = 1;
    mask.RX = 1;
    hal_uart_irq_set_mask(VBUS_UART_PORT, mask);
    int_unlock(lock);
}

//disable rx
static void uart_int_rx_stop(void)
{
    union HAL_UART_IRQ_T mask;

    uint32_t lock = int_lock();
    mask.reg = 0;
    hal_uart_irq_set_mask(VBUS_UART_PORT, mask);
    int_unlock(lock);
}

//set uart baud rate
static void set_uart_baud(uint32_t baud)
{
    uart_cfg.baud = baud;
    hal_uart_close(VBUS_UART_PORT);
    hal_uart_open(VBUS_UART_PORT, &uart_cfg);
}

//uart 1 rx set input mode
void hal_iomux_uart1_Rx(void)
{
	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&vbus_uart_tx_pin_cfg, 1);
	hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)vbus_uart_tx_pin_cfg.pin, HAL_GPIO_DIR_IN, 0);
}

//vbus tx/rx switch
//true:enable tx  
//false:set rx input mode
void vbus_TxRx_switch(bool flag)
{
   APP_VUART_TRACE(": %d.", flag);

    if(flag == true)
    {
        hal_iomux_uart1_Tx();
    }
    else
    {
    	hal_iomux_uart1_Rx();
    }
}

/*communication mode and charging mode switch*/
int32_t vbus_uart_communication_switch(bool onOff)
{
    APP_VUART_TRACE(": %d.", onOff);

    if (onOff == true)
    {
        APP_VUART_TRACE("communicateMode");
		//disable irq
		vbus_as_gpioint_disable();
#ifdef __UART_RECV_BY_INT__
		 set_uart_baud(USART_REV_BAUD);
		 uart_rx_int_start();
#else
		//uart_rx_dma_start();
#endif
	    app_sysfreq_req(APP_SYSFREQ_USER_VBUS, APP_SYSFREQ_26M);
    }
	else
    {
        APP_VUART_TRACE("chargingMode");
		//uart_rx_dma_stop();
#ifdef __UART_RECV_BY_INT__
		uart_int_rx_stop();
#else
		//uart_rx_dma_stop();
#endif
		vbus_as_gpioint_init();
        app_sysfreq_req(APP_SYSFREQ_USER_VBUS, APP_SYSFREQ_32K);
        
        /*
        	do not delete!!!!!!!!
			fix the gpiote bug.
        */
        APP_VUART_TRACE("chargingMode %d",hal_gpio_pin_get_val((HAL_GPIO_PIN_T)vbus_uart_tx_pin_cfg.pin));
        hal_gpio_pin_get_val((HAL_GPIO_PIN_T)vbus_uart_tx_pin_cfg.pin);
    }
	return 0;
}


static void app_vbus_manage_communicate_timeout_handler(void const *param)
{
	APP_VUART_TRACE("22222222");

	if(ui_get_sys_status() == APP_UI_SYS_NORMAL)
	{
		vbus_uart_communication_switch(false);
	}
	app_battery_enable_pmu_irq(battery_hal_if_charging_status());
}


//uart int mode callback
static void vbus_as_gpioint_handler(enum HAL_GPIO_PIN_T pin)
{
	//APP_VUART_TRACE("vbus_gpio_irq !!! send to ui");
	app_ui_event_process(UI_EVENT_VBUS,0,UI_EVENT_VBUS_INT,0,0);
}

void vbus_irq_handler()
{
	APP_VUART_TRACE("!!!");
	//set rx enable
    vbus_TxRx_switch(false);
	//switch to communicate mode
    vbus_uart_communication_switch(true);	
}

//uart irq init
static void vbus_as_gpioint_init(void)
{
	//enable gpioint
	APP_VUART_TRACE("vbus_as_gpioint_init!!!");
	struct HAL_GPIO_IRQ_CFG_T gpiocfg;

	//vbus_uart_communication_switch(false);
	gpiocfg.irq_enable = true;
	gpiocfg.irq_debounce = true;
	gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
	gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
	gpiocfg.irq_handler = vbus_as_gpioint_handler;
	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&vbus_uart_tx_pin_cfg, 1);
	hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)vbus_uart_tx_pin_cfg.pin, HAL_GPIO_DIR_IN, 0);
	hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)vbus_uart_tx_pin_cfg.pin, &gpiocfg);
	// disable uart
	
}

void VBUS_SendData(uint8_t *buf,uint8_t length)
{
    int i;

#ifdef __UART_RECV_BY_INT__
	//if(uart_rx_switch_flag)
    uart_int_rx_stop();
#else
	  uart_rx_dma_stop();
#endif
    APP_VUART_TRACE("VBUS_SendData");
 //   uart_rx_dma_stop();
    set_uart_baud(USART_SEND_BAUD);
    vbus_TxRx_switch(true);
    hal_sys_timer_delay_ns(1000*10);	//delay 10us

    for (i = 0; i < length; i++)
    {
        hal_uart_blocked_putc(VBUS_UART_PORT, buf[i]);
    }

    hal_sys_timer_delay_ns(1000*1000*5);	//delay 5ms

    set_uart_baud(USART_REV_BAUD);
#ifdef __UART_RECV_BY_INT__
   		uart_rx_int_start();
#else
    	uart_rx_dma_start();
#endif	/* End of #ifdef __UART_RECV_BY_INT__ */
    //uart_rx_dma_start();
    vbus_TxRx_switch(false);
    hal_uart_flush(VBUS_UART_PORT, 0);
    vbus_TxRx_switch(false);
	set_uart_baud(USART_REV_BAUD);  //
	uart_rx_int_start();
}

void vbus_rx_start_handle(void)
{
	APP_VUART_TRACE("!!!");
	app_battery_disable_pmu_irq();
	{
		osTimerStop(charger_communicate_timer);
		osTimerStart(charger_communicate_timer, UNCHARGER_DEBOUNCE_TIMER_DELAY_MS);
	}
}

int32_t vbus_init_raw(void)
{
	APP_VUART_TRACE("[---- VBUS ----] vbus_init");

	memset(&vbus_rx_buff,0,sizeof(vbus_rx_buff));
	
	//vbus cmd head init
	if(app_get_earside()== 0xFF)	//for first used define
	{
		frame_receive_head = 0x25;
		frame_reply_head = 0x52;
	}
	else
	{
		frame_receive_head = (0x20 | (app_get_earside() + 4));
		frame_reply_head = ((app_get_earside() + 4) << 4);
	}
	
	if (VBUS_UART_PORT == HAL_UART_ID_1)
		hal_iomux_set_uart1();
	else
		hal_iomux_set_uart0();
		
    if (charger_communicate_timer == NULL) {
        charger_communicate_timer = osTimerCreate (osTimer(CHARGER_COMMUNICATE_TIMER), osTimerOnce, NULL);
    }

#ifdef __UART_RECV_BY_INT__		
	//irq init
	vbus_as_gpioint_init();	
	//set Baud rate
	set_uart_baud(USART_REV_BAUD);	
	//open uart and set uart irq handler
	hal_vbus_rx_open();
#else //ifndef __UART_RECV_BY_INT__  use dma
	tx_enable(false);	
	if (hal_uart_open(VBUS_UART_PORT, &uart_cfg)) {
		APP_VUART_TRACE("[---- VBUS ----] init error");
		return -1;
	}
	hal_uart_irq_set_dma_handler(VBUS_UART_PORT, rx_dma_handler, tx_dma_handler);
	rx_dma_enable(true);
#endif //__UART_RECV_BY_INT__

#ifndef	DEBUG
	app_log_uart_init();
#endif	

	return 0;
}

//vbus uninit
void vbus_uninit_raw(void)
{
	uart_int_rx_stop();
	osTimerStop(charger_communicate_timer);
}

	
#endif //defined(__APP_VBUS_ENABLE__) && defined(__VBUS_USE_UART__)
