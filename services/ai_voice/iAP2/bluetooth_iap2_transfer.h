#ifndef __BLUETOOTH_IAP2_TRANSFER_H__
#define __BLUETOOTH_IAP2_TRANSFER_H__
#ifdef __cplusplus
extern "C" {
#endif
//#include "resources.h"

// MAX packet size that can be sent over iAP2
#define IAP2_TRANS_MAX_DATA     (512)   //???
#define IAP2_TRANS_DATA_MAX     (512)
#define IAP2_TRANS_BUFFER_MAX   (512)

typedef struct {
    uint16_t    pckLength;
    uint8_t pckValid;
    uint8_t   padding;
    uint8_t pckData[IAP2_TRANS_BUFFER_MAX];
} tIAP2_TRANS_PCK;

int bt_iap2_transfer_init();
void bt_iap2_transfer_free();
int iap2_push_data_to_trans_pool(uint8_t *buf, uint16_t len);
void iap2_set_all_transfer_packet_invalid();

#ifdef __cplusplus
}
#endif
#endif /* __BLUETOOTH_IAP2_TRANSFER_H__ */