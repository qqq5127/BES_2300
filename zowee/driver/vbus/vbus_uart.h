#ifndef __VBUS_UART__
#define	__VBUS_UART__

#if defined(__APP_VBUS_ENABLE__) //&& defined(__VBUS_USE_UART__)

#include "hal_trace.h"
#include "vbus_hal_if.h"



#define	APP_VUART_TRACE_ENABLE		1

#if APP_VUART_TRACE_ENABLE
#define APP_VUART_TRACE(s,...)		TRACE(1, "[DRIVER_VBUS]%s " s, __func__, ##__VA_ARGS__)
#else
#define APP_VUART_TRACE(s,...)
#endif
#define APP_VUART_DUMP8(str, buf, cnt)     DUMP8(str, buf, cnt)


#define RX_BUFF_SIZE 32
#define __UART_RECV_BY_INT__		//中断或者dma接收切换
#define USART_REV_BAUD  120000		//接收波特率
#define USART_SEND_BAUD 120000		//发送波特率
#define VBUS_REV_BUF_SIZE               (520)

#define	UNCHARGER_DEBOUNCE_TIMER_DELAY_MS	200	//200 ms

typedef struct
{
    uint8_t VbusSendingFlag;				//now vbus sending or not
    uint8_t VbusRecvStep;					//recv step
    uint8_t revBuf[VBUS_REV_BUF_SIZE];
    uint8_t revCnt;
    uint16_t recvLength;
} VBusCommunicateBufStaus_s;

typedef enum
{
    VBUS_RECV_START = 0,
    VBUS_RECV_EARSIDE,
    VBUS_RECV_DATA,
    VBUS_RECV_CRC
} vbus_recv_step_enum;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t frame_receive_head_get();
int32_t vbus_init_raw(void);
void vbus_uninit_raw(void);
void vbus_irq_handler(void);
void VBUS_SendData(uint8_t *buf,uint8_t length);
void vbus_rx_start_handle(void);
void vbus_as_gpioint_disable(void);


const hal_vbus_if_t vbus_ctrl =
{
    vbus_init_raw,
	vbus_uninit_raw,
    vbus_irq_handler,
    NULL,
    VBUS_SendData,
};

#ifdef __cplusplus
}
#endif

#endif //__APP_VBUS_ENABLE__ && __VBUS_USE_UART__

#endif //__VBUS_UART__

