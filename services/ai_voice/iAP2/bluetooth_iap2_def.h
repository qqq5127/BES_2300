#ifndef __BLUETOOTH_IAP2_DEF_H__
#define __BLUETOOTH_IAP2_DEF_H__

#include "cmsis_os.h"
#include "apple_cert.h"
#include "nvrecord.h"
//#include "middle_transport.h"
#include "hal_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

//typedef signed char   BOOL;

#if 0
typedef unsigned int  UINT32;
typedef unsigned char UINT8; 
typedef unsigned short UINT16;
#endif


//#define FALSE 0
//#define TRUE  1

#define IAP2_VERSION                        1
#define IAP2_MAX_OUTSTANDING                5
#define IAP2_ZERO_ACK_RE_TX_TIMEOUT         0
#define IAP2_ZERO_ACK_CUM_ACK_TIMEOUT       0
#define IAP2_ZERO_ACK_MAX_RETRANSMISSIONS   0
#define IAP2_ZERO_ACK_MAX_CUM_ACKS          0
#define IAP2_RE_TX_TIMEOUT                  1500
#define IAP2_CUM_ACK_TIMEOUT                200 // 73
#define IAP2_MAX_RETRANSMISSIONS            30
#define IAP2_MAX_CUM_ACKS                   3

// Session IDs
#define IAP2_CONTROL_SESSION_ID                 0xA
#define IAP2_EA_SESSION_ID                      0xB

#define kIAP2PacketServiceTypeControl       0       /* Control & Authentication */
#define kIAP2PacketServiceTypeBuffer        1       /* Buffer (ie. Artwork, Workout) */
#define kIAP2PacketServiceTypeEA            2       /* EA stream */

#define iAP2_LINK_VERSION 1

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_IAP2_DEF_H__ */