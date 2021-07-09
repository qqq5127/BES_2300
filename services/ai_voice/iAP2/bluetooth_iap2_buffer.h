#ifndef __BLUETOOTH_IAP2_BUFFER_H__
#define __BLUETOOTH_IAP2_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "bluetooth_iap2_def.h"
//#include "resources.h"
// MAX packet size that can be sent over iAP2
#define IAP2_REC_MAX_DATA   (512)
#define IAP2_REC_BUFFER_MAX (512)

typedef struct {
    u16 pckLength;
    u8  pckValid;
    u8   padding;
    u8  pckData[IAP2_REC_BUFFER_MAX];
} tIAP2_REC_PCK;

typedef struct {
    u8  seqNumber;
    u8  ackNumber;
} tIAP2_SEND_PCK;

extern int iap2_receive_buffer_init();
extern int iap2_push_data_to_rec_pool(uint8_t *buf, uint32_t len); 
extern tIAP2_REC_PCK * iap2_pull_data_from_rec_pool();
//tern void iap2_receive_buffer_free();
extern void iap2_set_all_receive_packet_invalid();

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_IAP2_BUFFER_H__ */