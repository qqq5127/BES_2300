#ifndef __APP_DMA_HANDLE_H__
#define __APP_DMA_HANDLE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "ai_control.h"
#include "dma.pb-c.h"

//#define DMA_PUSH_TO_TALK


#define  DMA_KEY  "Yg5Xb2NOK01bgB9csSUHAAgG4lUjMXXZ"
#define  DMA_PRODUCT_ID "asoYwYghv0fy6HFexl6bTIZUHjyZGnEH"

#define  DMA_SERIAL_NUM  "11111111"
#define  DMA_DEVICE_NAME  "bumblebee"
#define  DMA_DEVICE_TYPE "DOCK"
#define  DMA_MANUFACTURER  "BES"
#define  DMA_FIRMWARE_VERSION  "0.9.7"
#define  DMA_SOFTWARE_VERSION  "0.9.7"


enum dma_pair_state {
    DMA_PAIR_UNPAIR = 0,
    DMA_PAIR_PAIR,
};

enum {
    DMA_NO_ERROR = 0,
    DMA_DEVICE_IN_VOICECALL = -1,
    DMA_INVALID_PARAM = -2,
    DMA_UNKNOWN_ERR = -3,
    DMA_SEND_TMO = -4,
    DMA_SEND_ERR = -5,
};

extern const struct ai_handler_func_table dma_handler_function_table;

int app_dueros_dma_deinit(void);
const char* dma_get_key();
void dma_get_dev_info(DeviceInformation * device_info);
void dma_get_dev_config(DeviceConfiguration *device_config);
void dma_set_state(State *p_state);
void dma_get_state(uint32_t profile ,State *p_state);
void dma_get_setting(SpeechSettings *setting);
void dma_notify_speech_state(SpeechState speech_state);

void dma_ble_ccc_changed(unsigned char value);
void dma_ble_mtu_changed(unsigned int mtu_size);
void dma_a2dp_connected();
void dma_a2dp_disconnected();
void dma_media_control(MediaControl control, uint32_t volume);

#ifdef __cplusplus
}
#endif

#endif

