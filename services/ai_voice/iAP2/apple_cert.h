#ifndef __APPLE_CERT__
#define __APPLE_CERT__


#include <unistd.h>
#include "plat_types.h"


#ifdef __cplusplus
extern "C" {
#endif
#define DEVICE_VERSION_REG    0x00   // reset value 0x05
#define FIRMWARE_VERSION_REG  0x01   // reset value 0x01
#define AP_MAJOR_VERSION_REG  0x02   // reset value 0x02
#define AP_MINOR_VERSION_REG  0x03   // reset value 0x00
#define DEVICE_ID_REG         0x0x04 // 4bytes reset value  0x00000200
#define ERROR_CODE_REG        0x05   // reset value 0x0
#define CONTROL_STATUS_REG    0x10   // reset value 0x00

#define CHALLENGE_RESPONSE_LEN_REG  0x11  // 2 bytes reset value 128
#define CHALLENGE_RESPONSE_DATA_REG 0x12  //128 bytes

#define CHALLENGE_DATA_LEN_REG  0x20  // 2 bytes reset value 20
#define CHALLENGE_DATA_REG      0x21  // 128 byte

#define ACCESSORY_CERTIFICATE_DATA_LEN_REG 0X30 // 2 bytes   <=1280
#define ACCESSORY_CERTIFICATE_DATA_REG     0x31   // 0x31 --> 0x3A 


#define APPLE_DEVICE_CERTIFICATE_DATA_LEN_REG  0X50  // 2bytes 
#define APPLE_DEVICE_CERTIFICATE_DATA_REG      0X51  // 0X51 --> 0X58

#define CERTIFICATE_SERIAL_NUMBER       0x4E

#define SELF_TEST_REG   0x40

#if 1
#define COPROCESSOR_ADDR (0x20>>1)
#else
#define COPROCESSOR_ADDR (0x22>>1)
#endif

int read_apple_version(void);
int read_accessory_certificate_serial_number(u8 *buf,u16 buf_len);
int read_challenge_responce_data(u8 *challenge_buf,u16 challenge_len,u8 *challenge_response_buf,u16 response_buf_len);
int read_apple_cert_data(u8 *buf,u16 buf_len);
int iap2_buf_parse(u8 *buf, u32 length);
void test_apple_cert(void);

#ifdef __cplusplus
    }
#endif

#endif
