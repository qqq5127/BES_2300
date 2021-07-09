#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "app_gma_cmd_code.h"

#define APP_WAIT_TIMEOUT                    60//second
#define DEVICE_RESTART_TIMEOUT              60
#define OTA_UNIT_SIZE_SPP                   512
#define INTERVAL                            10//ms
#define ACK_ENABLE


#define FLASH_SECTOR_SIZE_IN_BYTES          4096
#define OTA_DATA_BUFFER_SIZE_FOR_BURNING    FLASH_SECTOR_SIZE_IN_BYTES
#define INTERACTION_OTA_NORFLASH_BUFFER_LEN    (FLASH_SECTOR_SIZE_IN_BYTES*2)

#define OTA_START_MAGIC_CODE                (0x54534542)  // BEST
#define NORMAL_BOOT                         0xBE57EC1C


#ifndef NEW_IMAGE_FLASH_OFFSET
#define NEW_IMAGE_FLASH_OFFSET              (0x200000)
#endif

#ifndef __APP_IMAGE_FLASH_OFFSET__
#define __APP_IMAGE_FLASH_OFFSET__          (0x18000)
#endif

#define OTA_FLASH_LOGIC_ADDR                (FLASH_NC_BASE)
#define OTA_FLASH_OFFSET_OF_IMAGE           __APP_IMAGE_FLASH_OFFSET__
#define OTA_FLASH_ENTRY_OFFSET              (OTA_FLASH_LOGIC_ADDR + OTA_FLASH_OFFSET_OF_IMAGE)

#define BD_ADDR_LENGTH                      6
#define NAME_LENGTH                         32
#define OTA_BLE_DATA_PACKET_MAX_SIZE        257
#define OTA_BT_DATA_PACKET_MAX_SIZE         513

#define OTA_BOOT_INFO_FLASH_OFFSET          0x1000
#define OTA_MAX_NUMBER_OF_FRAMES (0x0f)
/**
 * @brief The format of the otaBootInfo
 *
 */


typedef struct
{
    uint32_t magicNumber;
    uint32_t imageSize;
    uint32_t imageCrc;
}__attribute__ ((__packed__)) FLASH_OTA_BOOT_INFO_T;

typedef struct
{
    uint8_t     version_md5[4];
    uint32_t    upgradeSize[(FLASH_SECTOR_SIZE_IN_BYTES - 4) / 4];
}__attribute__ ((__packed__)) FLASH_OTA_UPGRADE_LOG_FLASH_T;


typedef struct
{
    uint8_t    isToRenameBT;
    uint8_t    newBTName[32];
    uint8_t    isToRenameBLE;
    uint8_t    newBLEName[32];
    uint8_t    isToUpdateBTAddr;
    uint8_t    newBTAddr[6];
    uint8_t    isToUpdateBLEAddr;
    uint8_t    newBLEAddr[6];
    uint32_t    startLocationToWriteImage;    // the offset of the flash to start writing the image
}__attribute__ ((__packed__))GMA_OTA_CONFIGURATION_T;

typedef struct
{
  uint8_t   dataBufferForBurning[OTA_DATA_BUFFER_SIZE_FOR_BURNING];
  uint32_t  dstFlashOffsetForNewImage;
  uint32_t  offsetInDataBufferForBurning;
  uint32_t  offsetInFlashToProgram;
  uint32_t  totalImageSize;
  uint32_t  alreadyReceivedDataSizeOfImage;
 // uint32_t  offsetInFlashOfCurrentSegment;
//  uint32_t  offsetOfImageOfCurrentSegment;
  uint16_t  crc16OfImage;
  uint8_t   g_current_frame_number;
  uint8_t   g_expected_frame_number;
  uint32_t  flashOffsetOfUserDataPool;

  uint32_t  flasehOffsetOfFactoryDataPool;
  //uint8_t   version_md5[4];
  GMA_OTA_CONFIGURATION_T configuration;

  uint8_t    isOTAInProgress;
  uint8_t    isReadyToApply;

 // bool        resume_at_breakpoint;
 // uint32_t    breakPoint;
//  uint32_t    i_log;
  
} __attribute__((__packed__)) GMA_OTA_ENV_T;


typedef struct
{
    uint8_t type;           // FW type
    uint8_t revise_nb;
    uint8_t second_ver_nb;
    uint8_t primary_ver_nb;
    uint8_t reservation;
} __attribute__ ((__packed__))GMA_DEV_FW_VER_T;


#define OTA_START_FROM_START   0
#define OTA_START_FROM_BREAK   1
typedef struct
{
    uint8_t type;           // FW type
    uint32_t version;
    uint32_t imageSize;
    uint16_t crc16;
    uint8_t flag;
} __attribute__ ((__packed__))GMA_OTA_UPGRADE_CMD_T;

#define  DO_PERMIT_UPGRADE          1
#define  DO_NOT_PERMIT_UPGRADE      0
typedef struct
{
   uint8_t upgradeornot;
   uint32_t  size;
   uint8_t maxNumOfFrame;
} __attribute__ ((__packed__))GMA_OTA_UPGRADE_RSP_T;


typedef struct
{
  
   uint8_t   current_frame_num;
   uint32_t  total_received_size;
} __attribute__ ((__packed__))GMA_OTA_REPORT_CMD_T;

typedef struct
{
  
   uint8_t   result;
} __attribute__ ((__packed__))GMA_OTA_REPORT_CRC_CMD_T;




#define  PACKETCRC_RESULT_TYPE  0x01
#define  PACKETCRC_RESULT_LENGHT  0x01

#define IMAGE_RECV_FLASH_CHECK       1  // It's best to turn it on durning development and not a big deal off in the release.
#define MAX_IMAGE_SIZE              ((uint32_t)(NEW_IMAGE_FLASH_OFFSET - __APP_IMAGE_FLASH_OFFSET__))
#define MIN_SEG_ALIGN               256


void app_gma_ota_FW_Ver_get(GMA_DEV_FW_VER_T *FW_Ver);
void app_gma_ota_finished_handler(void);

void app_gma_ota_whole_crc_validate_handler(APP_GMA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);
void app_gma_ota_upgrade_data_handler(APP_GMA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);
void app_gma_ota_start_handler(APP_GMA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen);
uint32_t app_gma_ota_get_already_received_size();
void app_gma_ota_reboot(uint8_t status);
bool app_gma_check_if_in_ota(void);
void app_gma_ota_exception_reboot(void);
#ifdef __cplusplus
}
#endif

