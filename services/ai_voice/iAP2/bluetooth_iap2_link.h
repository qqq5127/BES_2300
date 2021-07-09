#ifndef __BLUETOOTH_IAP2_LINK_H__
#define __BLUETOOTH_IAP2_LINK_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "bluetooth_iap2_def.h"

typedef enum {
    BT_IAP2_NONE = 0,
    BT_IAP2_DETECT,
    BT_IAP2_SYNC,
    BT_IAP2_AUTH,
    BT_IAP2_AUTH_SUCCESS,
    BT_IAP2_TRANSFER
} tIAP2_AUTH_STATE;

#define UINT8_TO_BE_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
#define UINT16_TO_BE_STREAM(p, u16) {*(p)++ = (uint8_t)((u16) >> 8); *(p)++ = (uint8_t)(u16);}

#define IAP2_AMA_IDENTIFIER 0x01

//
// Control session messages (authentication)
//
#define RequestAuthenticationCertificate                                    0xAA00 /* from device */

#define AuthenticationCertificate                                           0xAA01 /* from accessory */
#define AuthenticationCertificate_id_AuthenticationCertificate              0

#define RequestAuthenticationChallengeResponse                              0xAA02 /* from device */
#define RequestAuthenticationChallengeResponse_id_AuthenticationChallenge   0

#define AuthenticationResponse                                              0xAA03 /* from accessory */
#define AuthenticationFailed                                                0xAA04 /* from device */
#define AuthenticationSucceeded                                             0xAA05 /* from device */

//
// Control session messages (identification)
//
#define StartIdentification                                                 0x1D00 /* from device */

#define IdentificationInformation                                           0x1D01 /* from accessory */
#define IdentificationInformation_id_Name                                           0
#define IdentificationInformation_id_ModelIdentifier                                1
#define IdentificationInformation_id_Manufacturer                                   2
#define IdentificationInformation_id_SerialNumber                                   3
#define IdentificationInformation_id_FirmwareVersion                                4
#define IdentificationInformation_id_HardwareVersion                                5
#define IdentificationInformation_id_MessagesSentByAccessory                        6
#define IdentificationInformation_id_MessagesReceivedFromDevice                     7
#define IdentificationInformation_id_PowerProvidingCapability                       8
#define IdentificationInformation_id_MaximumCurrentDrawnFromDevice                  9
#define IdentificationInformation_id_SupportedExternalAccessoryProtocol             10

#define ExternalAccessoryProtocolGroup_id_ExternalAccessoryProtocolIdentifier       0
#define ExternalAccessoryProtocolGroup_id_ExternalAccessoryProtocolName             1
#define ExternalAccessoryProtocolGroup_id_ExternalAccessoryMatchAction              2
#define ExternalAccessoryProtocolGroup_id_NativeTransportComponentIdentifier        3

#define IdentificationInformation_id_AppMatchTeamID                                 11
#define IdentificationInformation_id_CurrentLanguage                                12
#define IdentificationInformation_id_SupportedLanguage                              13
#define IdentificationInformation_id_BluetoothTransportComponent                    17

#define BluetoothTransportComponent_id_TransportComponentIdentifier                 0
#define BluetoothTransportComponent_id_TransportComponentName                       1
#define BluetoothTransportComponent_id_TransportSupportsiAP2Connection              2
#define BluetoothTransportComponent_id_BluetoothTransportMediaAccessControlAddress  3

#define IdentificationInformation_id_LocationInformationComponent                   22
#define IdentificationInformation_id_BluetoothHIDComponent                          22

#define IdentificationAccepted                                              0x1D02 /* from device */

// a subset of IdentificationInformation parameters are used as is */
#define IdentificationRejected                                              0x1D03 /* from device */

#define CancelIdentification                                                0x1D05 /* from accessory */

// a subset of IdentificationInformation parameters are used as is */
#define IdentificationInformationUpdate                                     0x1D06 /* from accessory */
extern void bt_iap2_packet_deinit(void);

extern void bt_iap2_packet_init(void);
extern bool iap2_send_throuth_bt( void* pBuf, uint16_t len);
extern void iap2_send_detect(void);
extern void set_iap2_state_and_wake(uint8_t state);
extern void set_iap2_link_state(uint8_t state);
extern uint8_t get_iap2_link_state();
extern void bt_iap2_packet_free();
extern void wake_up_recv_process();
extern int* iap2_process_recv_packet_ota(uint8_t* buffer, uint32_t len);
extern u16 get_ea_protocol_session_id(int proto_id);
extern uint8_t get_seq_number();
extern uint8_t get_ack_number();
extern bool only_mfi_reset_connect;
bool iap2_ea_send_data(uint8_t *buf, uint32_t len,int proto_id);
extern bool iap2_handle_request_certificate_serial_number(uint8_t seq_num);
int iap2_data_stream_judge(uint8_t *spp_buf, u16 bufLen, uint16_t  *amaAddr, uint16_t* amaLen);
#ifdef __cplusplus
}
#endif
#endif /* __BLUETOOTH_IAP2_LINK_H__ */
