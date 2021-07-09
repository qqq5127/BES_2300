#include "cmsis.h"
#include "cmsis_os.h"
#include "bluetooth.h"
#include "app_bt.h"
#include "btapp.h"
#include "spp_api.h"
#include "hfp_api.h"
#include "app_voicepath.h"
#include "gsound_dbg.h"
#include "gsound_custom.h"
#include "gsound_custom_bt.h"
#include "app_bt_func.h"
#include "app_btmap_sms.h"

#ifdef IBRT
#include "app_tws_ibrt.h"
#endif

extern struct BT_DEVICE_T app_bt_device;
extern "C" btif_remote_device_t *app_bt_get_connected_mobile_device_ptr(void);

uint8_t GSoundTargetBtNumDevicesConnected(void)
{
    return app_bt_get_num_of_connected_dev();
}

typedef struct
{
    uint16_t rfcomm_tx_buf_chunk_cnt;
    uint16_t rfcomm_tx_buf_allocated_status_mask;
    uint16_t rfcomm_tx_buf_chunk_size;
    uint16_t rfcomm_tx_buf_allocated_status;
    uint8_t *rfcomm_tx_buf_ptr;
} GSOUND_RFCOMM_TX_BUF_CONFIG_T;

#define GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_CNT 3
#define GSOUND_RFCOMM_CONTROL_TX_BUF_ALLOCTED_STATUS_MASK ((1 << GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_CNT) - 1)
#define GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_SIZE MAXIMUM_GSOUND_RFCOMM_TX_SIZE
#define GSOUND_RFCOMM_CONTROL_TX_BUF_SIZE (GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_CNT * GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_SIZE)

static uint8_t gsound_rfcomm_control_tx_buf[GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_CNT][GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_SIZE];

#define GSOUND_RFCOMM_TX_BUF_CHUNK_CNT 4
#define GSOUND_RFCOMM_TX_BUF_ALLOCTED_STATUS_MASK ((1 << GSOUND_RFCOMM_TX_BUF_CHUNK_CNT) - 1)
#define GSOUND_RFCOMM_TX_BUF_CHUNK_SIZE MAXIMUM_GSOUND_RFCOMM_TX_SIZE
#define GSOUND_RFCOMM_TX_BUF_SIZE (GSOUND_RFCOMM_TX_BUF_CHUNK_CNT * GSOUND_RFCOMM_TX_BUF_CHUNK_SIZE)

static uint8_t gsound_rfcomm_tx_buf[GSOUND_RFCOMM_TX_BUF_CHUNK_CNT][GSOUND_RFCOMM_TX_BUF_CHUNK_SIZE];

static GSOUND_RFCOMM_TX_BUF_CONFIG_T audio_tx_buf_config[GSOUND_NUM_CHANNEL_TYPES] = {
    {
        GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_CNT,
        GSOUND_RFCOMM_CONTROL_TX_BUF_ALLOCTED_STATUS_MASK,
        GSOUND_RFCOMM_CONTROL_TX_BUF_CHUNK_SIZE,
        0,
        (uint8_t *)&gsound_rfcomm_control_tx_buf,
    },

    {
        GSOUND_RFCOMM_TX_BUF_CHUNK_CNT,
        GSOUND_RFCOMM_TX_BUF_ALLOCTED_STATUS_MASK,
        GSOUND_RFCOMM_TX_BUF_CHUNK_SIZE,
        0,
        (uint8_t *)&gsound_rfcomm_tx_buf,
    },
};

static uint8_t *gsound_rfcomm_tx_buf_addr(GSoundChannelType channel, uint32_t chunk)
{
    GSOUND_RFCOMM_TX_BUF_CONFIG_T *pTxBufConfig =
        &audio_tx_buf_config[channel];

    return (uint8_t *)&(pTxBufConfig->rfcomm_tx_buf_ptr[chunk * pTxBufConfig->rfcomm_tx_buf_chunk_size]);
}

static int32_t gsound_rfcomm_alloc_tx_chunk(GSoundChannelType channel)
{
    GSOUND_RFCOMM_TX_BUF_CONFIG_T *pTxBufConfig =
        &audio_tx_buf_config[channel];

    uint32_t lock = int_lock_global();

    if (pTxBufConfig->rfcomm_tx_buf_allocated_status_mask ==
        pTxBufConfig->rfcomm_tx_buf_allocated_status)
    {
        int_unlock_global(lock);
        return -1;
    }

    for (int32_t index = 0; index < pTxBufConfig->rfcomm_tx_buf_chunk_cnt; index++)
    {
        if (!(pTxBufConfig->rfcomm_tx_buf_allocated_status & (1 << index)))
        {
            pTxBufConfig->rfcomm_tx_buf_allocated_status |= (1 << index);
            int_unlock_global(lock);
            return index;
        }
    }

    int_unlock_global(lock);
    return -1;
}

bool gsound_rfcomm_free_tx_chunk(GSoundChannelType channel, uint8_t *ptr)
{
    GSOUND_RFCOMM_TX_BUF_CONFIG_T *pTxBufConfig =
        &audio_tx_buf_config[channel];

    uint32_t lock = int_lock_global();

    if (0 == pTxBufConfig->rfcomm_tx_buf_allocated_status)
    {
        int_unlock_global(lock);
        return false;
    }

    for (uint32_t index = 0; index < pTxBufConfig->rfcomm_tx_buf_chunk_cnt; index++)
    {
        if (gsound_rfcomm_tx_buf_addr(channel, index) == ptr)
        {
            pTxBufConfig->rfcomm_tx_buf_allocated_status &= (~(1 << index));
            int_unlock_global(lock);
            return true;
        }
    }

    int_unlock_global(lock);
    return false;
}

void gsound_rfcomm_reset_tx_buf(GSoundChannelType channel)
{
    GSOUND_RFCOMM_TX_BUF_CONFIG_T *pTxBufConfig = &audio_tx_buf_config[channel];

    uint32_t lock = int_lock_global();

    pTxBufConfig->rfcomm_tx_buf_allocated_status = 0;
    int_unlock_global(lock);
}

extern "C" uint32_t gsound_rfcomm_get_available_tx_packet_cnt(void)
{
    uint32_t freeTxPacketsCnt = 0;

    GSOUND_RFCOMM_TX_BUF_CONFIG_T *pTxBufConfig =
        &audio_tx_buf_config[GSOUND_CHANNEL_AUDIO];

    for (int32_t index = 0; index < pTxBufConfig->rfcomm_tx_buf_chunk_cnt; index++)
    {
        if (!(pTxBufConfig->rfcomm_tx_buf_allocated_status & (1 << index)))
        {
            freeTxPacketsCnt++;
        }
    }

    return freeTxPacketsCnt;
}

// static uint32_t gsound_rfcomm_control_get_available_tx_packet_cnt(void)
// {
//     uint32_t freeTxPacketsCnt = 0;

//     GSOUND_RFCOMM_TX_BUF_CONFIG_T *pTxBufConfig =
//         &audio_tx_buf_config[GSOUND_CHANNEL_CONTROL];

//     for (int32_t index = 0; index < pTxBufConfig->rfcomm_tx_buf_chunk_cnt; index++)
//     {
//         if (!(pTxBufConfig->rfcomm_tx_buf_allocated_status & (1 << index)))
//         {
//             freeTxPacketsCnt++;
//         }
//     }

//     return freeTxPacketsCnt;
// }

static void gsound_rfcomm_transmit_handler(uint8_t *ptrData, uint32_t lenVsChannel)
{
    // decode channel & data length
    uint16_t dataLen = lenVsChannel & 0xFFFF;
    GSoundChannelType channel = (GSoundChannelType)(lenVsChannel >> 16);

    uint32_t bytes_consumed = 0;

    int8_t ret = gsound_bt_transmit(channel, ptrData, dataLen, &bytes_consumed);
    if (0 != ret)
    {
        GLOG_I("Rfcomm write failed! Free tx chunk buffer.");
        gsound_rfcomm_free_tx_chunk(channel, ptrData);
    }
}

GSoundStatus GSoundTargetBtTransmit(GSoundChannelType channel,
                                    const uint8_t *ptrData,
                                    uint32_t length,
                                    uint32_t *bytes_consumed)
{
    GLOG_I("GSound BT Transmit Callback: channel=%s, len=%d",
           gsound_chnl2str(channel),
           length);
    *bytes_consumed = 0;

#ifdef IBRT
    // slave is not allowed to send data out
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if (IBRT_SLAVE == p_ibrt_ctrl->current_role)
    {
        GLOG_I("slave is not allowed to send data.");
        *bytes_consumed = length;
        return GSOUND_STATUS_OK;
    }
#endif

    if (!gsound_bt_is_channel_connected(channel))
    {
        GLOG_I("%s not connected", gsound_chnl2str(channel));
        return GSOUND_STATUS_ERROR;
    }

    ASSERT(GSoundTargetOsMqActive(), "mq not active");

    int32_t index = -1;
    index = gsound_rfcomm_alloc_tx_chunk(channel);

    if (-1 != index)
    {
        uint8_t *tmpBuf = gsound_rfcomm_tx_buf_addr(channel, index);
        memcpy(tmpBuf, ptrData, length);

        uint32_t lenVsChannel = ((uint16_t)length) | ((uint32_t)(channel << 16));

        app_bt_start_custom_function_in_bt_thread((uint32_t)tmpBuf,
                                                  (uint32_t)lenVsChannel,
                                                  (uint32_t)gsound_rfcomm_transmit_handler);

        *bytes_consumed = length;
        return GSOUND_STATUS_OK;
    }
    else
    {
        return GSOUND_STATUS_OUT_OF_MEMORY;
    }
}

GSoundStatus GSoundTargetBtInit(const GSoundBtInterface *handler)
{
    gsound_bt_register_target_handle(handler);

    return GSOUND_STATUS_OK;
}

uint16_t GSoundTargetBtGetMtu(void)
{
    // TODO(jkessinger): Should this default to the real minimum value until we have
    // negotiated MTU, or does the RFCOMM guarantee negotiation???
    // Subtract the RFCOMM and L2CAP headers bytes.
    return gsound_bt_get_mtu();
}

GSoundStatus GSoundTargetBtHfpDial(const GSoundTargetBtHfpNumber *msg,
                                   const GSoundBTAddr *gsound_addr)
{
    hf_chan_handle_t hf_chan = app_bt_device.hf_channel[BT_DEVICE_ID_1];
    static uint8_t number[GSOUND_TARGET_BT_HFP_NUMBER_MAX];

    memcpy(number, msg->number, msg->size > sizeof(number) ? sizeof(number) : msg->size);
    btif_hf_dial_number(hf_chan, number, msg->size);

    return GSOUND_STATUS_OK;
}

GSoundStatus GSoundTargetBtMapSendSms(const GSoundTargetBtMapSmsNumber *number,
                                      const GSoundTargetBtMapSmsMessage *message,
                                      const GSoundBTAddr *gsound_addr)
{
#ifdef __BTMAP_ENABLE__
    if (true == app_btmap_check_is_connected(BT_DEVICE_ID_1))
    {
        GLOG_I("map client send sms");
        app_btmap_sms_send(BT_DEVICE_ID_1, (char *)number, (char *)message);
    }
    else
    {
        GLOG_I("map client send conn request");
        app_btmap_sms_open(BT_DEVICE_ID_1, btif_me_get_remote_device_bdaddr(app_bt_get_connected_mobile_device_ptr()));
        app_btmap_sms_save(BT_DEVICE_ID_1, (char *)number, (char *)message);
    }
#endif
    return GSOUND_STATUS_OK;
}

GSoundStatus GSoundTargetBtPauseSecondary(const GSoundBTAddr *gsound_addr)
{
    /*
   * If more than one device connected and
   * if we are streaming on secondary device (i.e. multipoint use-case)
   * We need to pause A2DP on that device and flag it to resume
   * when query completes.
   */
    if (GSoundTargetBtNumDevicesConnected() > 1)
    {
        uint8_t bisto_addr[BTIF_BD_ADDR_SIZE];

        gsound_convert_bdaddr_to_plateform(gsound_addr, bisto_addr);
        GLOG_I("GSound: Multi-point detected: %d", GSoundTargetBtNumDevicesConnected());

        // This call will pause inactive device if streaming
        app_gsound_a2dp_streaming_handler_pre_voice_query(bisto_addr);
    }

    return GSOUND_STATUS_OK;
}

GSoundStatus GSoundTargetBtResumeSecondary(const GSoundBTAddr *gsound_addr)
{
    app_gsound_a2dp_streaming_handler_post_voice_query();
    return GSOUND_STATUS_OK;
}

void GSoundTargetBtA2dpPause(void)
{
}

void GSoundTargetBtA2dpPlay(GSoundBTAddr *addr)
{
}

bool GSoundTargetBtA2dpIsStreaming(GSoundBTAddr *gsound_addr)
{
    return false;
}

bool GSoundTargetBtHfpIsActive(void)
{
    return app_voicepath_check_hfp_active();
}

bool GSoundTargetBtIsConnected(GSoundBTAddr const *gsound_addr)
{
    uint8_t connectedBtAddr[BTIF_BD_ADDR_SIZE];
    uint8_t convertedBtAddr[BTIF_BD_ADDR_SIZE];

    gsound_convert_bdaddr_to_plateform(gsound_addr, convertedBtAddr);

    GLOG_I("converted add:");
    DUMP8("%02x ", convertedBtAddr, 6);

    for (uint8_t devIndex = 0; devIndex < BT_DEVICE_NUM; devIndex++)
    {
        if (app_bt_get_device_bdaddr(devIndex, connectedBtAddr))
        {
            DUMP8("%02x ", connectedBtAddr, 6);
            if (!memcmp(connectedBtAddr, convertedBtAddr, BTIF_BD_ADDR_SIZE))
            {
                return true;
            }
        }
    }

    return false;
}

void GSoundTargetBtRxComplete(GSoundChannelType type,
                              const uint8_t *buffer,
                              uint32_t len)
{
    gsound_bt_rx_complete_handler(type, buffer, len);
}
