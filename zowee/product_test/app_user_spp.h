#ifndef __APP_USER_SPP_H__
#define __APP_USER_SPP_H__




#define	PRODUCT_TEST_TRACE_ENABLE	1

#if PRODUCT_TEST_TRACE_ENABLE
#define PRODUCT_TEST_TRACE(s,...)		TRACE(1, "[PRODUCT_TEST]%s " s, __func__, ##__VA_ARGS__)
#else
#define PRODUCT_TEST_TRACE(s,...)
#endif


#ifdef __cplusplus
extern "C" {
#endif



typedef enum {
    USER_PROCESS_INVALID = 0,
    USER_PROCESS_READ    = 1,
    USER_PROCESS_WRITE   = 2,
    USER_PROCESS_MAX,
} USER_PROCESS_E;

#define APP_USER_CONNECTED             (1 << 0)    
#define APP_USER_DISCONNECTED          (~(1 << 0))

#if defined(__3M_PACK__)
#define L2CAP_MTU                           980
#else
#define L2CAP_MTU                           672
#endif

#define USER_SPP_MAX_PACKET_SIZE     L2CAP_MTU
#define USER_SPP_MAX_PACKET_NUM      5
#define USER_SPP_TX_BUF_SIZE	    (USER_SPP_MAX_PACKET_SIZE*USER_SPP_MAX_PACKET_NUM)

#define USER_SPP_RECV_BUFFER_SIZE   1024


typedef uint32_t(*app_user_spp_tx_done_t)(void *param1, uint32_t param2);

//add new function
void app_user_spp_server_init(void);
void app_user_send_data_via_spp(uint8_t* ptrData, uint32_t length);
void user_open_spp(void);
void user_close_spp(void);
void app_user_spp_client_init(void);
void app_spp_user_send_data(uint8_t* ptrData, uint16_t length);




#ifdef __cplusplus
}
#endif

#endif

