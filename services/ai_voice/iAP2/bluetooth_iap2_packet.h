#ifndef __BLUETOOTH_IAP2_PACKET_H__
#define __BLUETOOTH_IAP2_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bluetooth_iap2_def.h"

#define IAP2_CONTROL_SESSION_HEADER_SIZE            6
#define IAP2_CONTROL_SESSION_PARAMETER_HEADER_SIZE  4




#define IAP2_MATCH_ACTION_NO_PROMPT                                 0
#define IAP2_MATCH_ACTION_DEVICE_MAY_PROMPT_AND_FIND_APP_BUTTON     1
#define IAP2_MATCH_ACTION_DEVICE_NO_PROMPT_AND_FIND_APP_BUTTON      2



#define IAP2_HI_BYTE(X) (((X) >> 8) & 0xFF)
#define IAP2_LO_BYTE(X) ((X) & 0xFF)

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

#define IdentificationInformation_id_AppMatchTeamID                                 11
#define IdentificationInformation_id_CurrentLanguage                                12
#define IdentificationInformation_id_SupportedLanguage                              13
#define IdentificationInformation_id_BluetoothTransportComponent                    17

#define ExternalAccessoryProtocolGroup_id_ExternalAccessoryProtocolIdentifier       0
#define ExternalAccessoryProtocolGroup_id_ExternalAccessoryProtocolName             1
#define ExternalAccessoryProtocolGroup_id_ExternalAccessoryMatchAction              2
#define ExternalAccessoryProtocolGroup_id_NativeTransportComponentIdentifier        3

#define BluetoothTransportComponent_id_TransportComponentId             0
#define BluetoothTransportComponent_id_TransportComponentName           1
#define BluetoothTransportComponent_id_TransportSupportsiAP2Connection  2
#define BluetoothTransportComponent_id_BluetoothTransportMACAddress     3


//
// Control session messages (EA protocol)
//
#define StartExternalAccessoryProtocolSession                               0xEA00 /* from device */
#define StartExternalAccessoryProtocolSession_id_ExternalAccessoryProtocolIdentifier        0
#define StartExternalAccessoryProtocolSession_id_ExternalAccessoryProtocolSessionIdentifier 1


#define StopExternalAccessoryProtocolSession                                0xEA01 /* from device */
#define StopExternalAccessoryProtocolSession_id_ExternalAccessoryProtocolSessionIdentifier  0

typedef enum
{
    kiAP2PacketParseStateSOP1 = 0,
    kiAP2PacketParseStateSOP2,
    kiAP2PacketParseStateLEN1,
    kiAP2PacketParseStateLEN2,
    kiAP2PacketParseStateCTRL,
    kiAP2PacketParseStateSEQ,
    kiAP2PacketParseStateACK,
    kiAP2PacketParseStateSESSID,
    kiAP2PacketParseStateCHK,
    kiAP2PacketParseStatePAYLOAD,
    kiAP2PacketParseStatePAYLOADCHK,
    kiAP2PacketParseStateFINISH,
    kiAP2PacketParseStateDETECT,
    kiAP2PacketParseStateDETECTBAD,

    kiAP2PacketParseStateLAST = kiAP2PacketParseStateDETECTBAD

} kiAP2PacketParseState_t;

enum IAP2PacketEnums
{
    /* Start of packet bytes */
    kIAP2PacketSYNC        = 0xFF,
    kIAP2PacketSOP         = 0x5A,
    kIAP2PacketSOPOrig     = 0x55,

    kIAP2PacketSOPLen      = 2,

    kiAP2PacketVersion     = 1,

    /* Packet field index */
    kIAP2PacketIndexSYNC   = 0,   /* Start of packet byte 1 */
    kIAP2PacketIndexSOP    = 1,   /* Start of packet byte 2 */
    kIAP2PacketIndexLEN1   = 2,   /* Packet length MSB */
    kIAP2PacketIndexLEN2   = 3,   /* Packet length LSB */
    kIAP2PacketIndexCTRL   = 4,   /* Control flags */
    kIAP2PacketIndexSEQ    = 5,   /* SEQ number */
    kIAP2PacketIndexACK    = 6,   /* ACK number */
    kIAP2PacketIndexSESSID = 7,   /* Session ID */
    kIAP2PacketIndexCHK    = 8,   /* Header checksum */

    /* iAP 1.0/2.0 packet detect: FF 55 02 00 EE 10 */
    kIAP2PacketDetectLEN    = 0x0200,
    kIAP2PacketDetectCTRL   = 0xEE,
    kIAP2PacketDetectSEQ    = 0x10,
    kIAP1PacketDetectNackCTRL = 0x00,
    kIAP1PacketDetectNACKSEQ  = 0xFE,

    /* iAP 1.0/2.0 packet detect BAD ACK: FF 55 04 00 02 04 EE 08 */
    kIAP2PacketDetectNACKLEN    = 0x0400,
    kIAP2PacketDetectNACKCTRL   = 0x02,
    kIAP2PacketDetectNACKSEQ    = 0x04,
    kIAP2PacketDetectNACKACK    = 0xEE,
    kIAP2PacketDetectNACKSESSID = 0x08,

    /*
    ** Packet Header Len
    ** SOP(2) + LEN(2) + CONTROL(1) + SEQ(1) + ACK(1) + SESSID(1) + CHK(1)
    */
    kIAP2PacketHeaderLen = 9,
    kIAP2PacketChksumLen = 1,   /* checksum byte length */

    kiAP2PacketLenMax = 0xFFFF,
    kiAP2PacketMaxPayloadSize = (kiAP2PacketLenMax - kIAP2PacketHeaderLen - kIAP2PacketChksumLen),

    /* BitMask defines for bits in control byte */
    kIAP2PacketControlMaskSYN = 0x80,   /* synchronization */
    kIAP2PacketControlMaskACK = 0x40,   /* acknowledgement */
    kIAP2PacketControlMaskEAK = 0x20,   /* extended acknowledgement */
    kIAP2PacketControlMaskRST = 0x10,   /* reset */
    kIAP2PacketControlMaskSUS = 0x08,   /* suspend (sleep) */

    kIAP2PacketSynDataIdxVersion           = 0,
    kIAP2PacketSynDataIdxMaxOutstanding    = 1,
    kIAP2PacketSynDataIdxMaxPacketSize     = 2,
    kIAP2PacketSynDataIdxRetransmitTimeout = 4,
    kIAP2PacketSynDataIdxCumAckTimeout     = 6,
    kIAP2PacketSynDataIdxMaxRetransmit     = 8,
    kIAP2PacketSynDataIdxMaxCumACK         = 9,

    kIAP2PacketSynDataBaseLen              = 10,

    kIAP2PacketSynDataIdxSessionInfo       = 10,

    kIAP2PacketSynSessionIdxID             = 0,
    kIAP2PacketSynSessionIdxType           = 1,
    kIAP2PacketSynSessionIdxVersion        = 2,
    kIAP2PacketSynSessionSize              = 3,

    /* Session ID 0x0 shall not be used for any session types */
    kIAP2PacketReservedSessionID           = 0,

    kIAP2PacketMaxSessions                 = 10,

    kIAP2PacketSynOptionMaskLP             = 0x80,
    kIAP2PacketSynOptionMaskHP             = 0x40
};

typedef struct {
    uint8_t sop1;   /* 0xFF */
    uint8_t sop2;   /* 0x5A */
    uint8_t len1;
    uint8_t len2;
    uint8_t ctl;
    uint8_t seq;
    uint8_t ack;
    uint8_t sess;
    uint8_t chk;

    uint8_t data [1024];

} iAP2PacketData_t;


/* To create and delete packets, must use functions provided here. */
typedef struct iAP2Packet_st
{
    void*       link;       /* link associated with this packet */

    void*       cbContext;
    void*       callbackOnSend; /* iAP2LinkDataSentCB_t */

    uint8_t*    payloadoffset;
    uint32_t    payloadLen;

    uint32_t    dataCurLen;
    uint32_t    bufferLen;

    uint32_t    timeStamp;  /* timestamp (ms) when packet was sent or received(and parse). */

    uint16_t    packetLen;

    uint8_t     dataChecksum;
    uint8_t     seqPlus;    /* Used to determine ACK location in sent queue */

    uint8_t     retransmitCount;
    uint8_t     timer;

    kiAP2PacketParseState_t state:8;

    BOOL        recvEAK:1;
    BOOL        sendPacket:1;

    /* reserve 4 bits for debug use */
    uint8_t     debugData:4;

    iAP2PacketData_t* pckData;

} iAP2Packet_t;

extern void iAP2PacketResetPacket (iAP2Packet_t* pck);
extern uint32_t iAP2PacketParseBuffer (const uint8_t*  buffer,
                                uint32_t        bufferLen,
                                iAP2Packet_t*   packet,
                                uint32_t        maxPacketSize,
                                BOOL*           bDetect,
                                BOOL*           bComplete,
                                BOOL*           bBad,
                                uint32_t*       failedChecksums,
                                uint32_t*       sopDetect);
uint8_t iAP2PacketCalcChecksum (const uint8_t*  buffer,
                                uint32_t        bufferLen);
uint8_t *iap2_add_parameter(uint8_t *p, uint16_t id, uint8_t *p_param, uint16_t param_len);
uint8_t *iap2_add_external_accessory_protocol_group(uint8_t *p, uint16_t id);
uint8_t *iap2_add_external_accessory_protocol_group1(uint8_t *p, uint16_t id);

uint8_t *iap2_add_bluetooth_transport_component_group(uint8_t *p, uint16_t id);

#ifdef __cplusplus
}
#endif
#endif /* __BLUETOOTH_IAP2_PACKET_H__ */
