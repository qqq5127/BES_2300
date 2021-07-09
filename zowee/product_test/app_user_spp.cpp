/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/	
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "hal_trace.h"
#include "cqueue.h"
#include "cmsis_os.h"
#include "cqueue.h"
#include "spp_api.h"
#include "app_spp.h"
#if defined(IBRT)
#include "app_tws_ibrt.h"
#endif
#include "app_user_spp.h"
#include "app_spp_anc.h"
#include "app_scpi_test.h"
	
static struct spp_device *user_spp_dev = NULL;
	
#if BTIF_SPP_SERVER == BTIF_ENABLED
osMutexDef(user_server_spp_mutex);
#endif
	
#if BTIF_SPP_CLIENT == BTIF_ENABLED
osMutexDef(user_client_spp_mutex);
#endif
	
static app_user_spp_tx_done_t app_user_spp_tx_done_func = NULL;


uint8_t UserSppTxBuf[USER_SPP_TX_BUF_SIZE];
uint8_t UserSppRxBuf[USER_SPP_RECV_BUFFER_SIZE];

#if 1
/*128 bit UUID in Big Endian f8d1fbe4-7966-4334-8024-ff96c9330e15*/
static const U8 UserSppClassId[] = {
   SDP_ATTRIB_HEADER_8BIT(17),
   DETD_UUID + DESD_16BYTES,
	0x12, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
};
#else
/*128 bit UUID in Big Endian f8d1fbe4-7966-4334-8024-ff96c9330e15*/
static const U8 UserSppClassId[] = {
   SDP_ATTRIB_HEADER_8BIT(17),
   DETD_UUID + DESD_16BYTES,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
};
#endif
static const U8 UserSppProtoDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(12),  /* Data element sequence, 12 bytes */ 

    /* Each element of the list is a Protocol descriptor which is a 
     * data element sequence. The first element is L2CAP which only 
     * has a UUID element.  
     */ 
    SDP_ATTRIB_HEADER_8BIT(3),   /* Data element sequence for L2CAP, 3 
                                  * bytes 
                                  */ 

    SDP_UUID_16BIT(PROT_L2CAP),  /* Uuid16 L2CAP */ 

    /* Next protocol descriptor in the list is RFCOMM. It contains two 
     * elements which are the UUID and the channel. Ultimately this 
     * channel will need to filled in with value returned by RFCOMM.  
     */ 

    /* Data element sequence for RFCOMM, 5 bytes */
    SDP_ATTRIB_HEADER_8BIT(5),

    SDP_UUID_16BIT(PROT_RFCOMM), /* Uuid16 RFCOMM */

    /* Uint8 RFCOMM channel number - value can vary */
    SDP_UINT_8BIT(RFCOMM_CHANNEL_FP)

};

/*
 * BluetoothProfileDescriptorList
 */
static const U8 UserSppProfileDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(8),        /* Data element sequence, 8 bytes */

    /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_ATTRIB_HEADER_8BIT(6),

    SDP_UUID_16BIT(SC_SERIAL_PORT),   /* Uuid16 SPP */
    SDP_UINT_16BIT(0x0102)            /* As per errata 2239 */ 
};

/*
 * * OPTIONAL *  ServiceName
 */
static const U8 UserSppServiceName[] = {
    SDP_TEXT_8BIT(5),          /* Null terminated text string */ 
    'u', 's', 'e', 'r','\0'
};

/* SPP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SPP SDP record.
 */
static sdp_attribute_t UserSppSdpAttributes[] = {

    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, UserSppClassId), 

    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, UserSppProtoDescList),

    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, UserSppProfileDescList),

    /* SPP service name*/
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), UserSppServiceName),
};
	
uint16_t app_spp_user_tx_buf_size(void)
{
	return USER_SPP_TX_BUF_SIZE;
}
		
void app_spp_user_send_data(uint8_t* ptrData, uint16_t length)
{
#if defined(IBRT)
	if (app_tws_ibrt_tws_link_connected() && (app_tws_ibrt_role_get_callback(NULL) != IBRT_MASTER))
	{
		PRODUCT_TEST_TRACE("role %d isn't MASTER", app_tws_ibrt_role_get_callback(NULL));
		return ;
	}
#endif
	btif_spp_write(user_spp_dev, (char *)ptrData, &length);
}
	
uint32_t app_user_send_done(void *param1, uint32_t param2)
{
    PRODUCT_TEST_TRACE("Data sent out");
	
	return 0;
}
void app_user_register_spp_tx_done(app_user_spp_tx_done_t callback)
{
    app_user_spp_tx_done_func = callback;
}
void app_user_disconnected(uint8_t disconnType)
{
    PRODUCT_TEST_TRACE("disconnected.");
}

void app_user_connected(uint8_t conidx, uint8_t connType)
{
    app_user_register_spp_tx_done(app_user_send_done);
}

int app_user_spp_handle_data_event_func(uint8_t *pData, uint16_t dataLen)
{
    //PRODUCT_TEST_TRACE("app user SPP receive pData %p length=%d", pData, dataLen);
    //DUMP8("0x%02x ", pData, dataLen);
	
	app_scpi_test_spp_parse(pData,dataLen);
	return 0;
}

int user_spp_data_event_callback(void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen)
{

    switch(process)
    {
        case USER_PROCESS_READ:
			app_user_spp_handle_data_event_func(pData,(uint32_t )dataLen);
	        PRODUCT_TEST_TRACE("user spp read process");
        break;
        
        case USER_PROCESS_WRITE:
	        PRODUCT_TEST_TRACE("user spp write event");
        break;

        default:
	        PRODUCT_TEST_TRACE("user spp process not defined.");
        break;
    }
    return 0;
}



static void user_spp_callback(struct spp_device *locDev, spp_callback_parms *Info)
{
    if (BTIF_SPP_EVENT_REMDEV_CONNECTED == Info->event)
    {
        PRODUCT_TEST_TRACE("::AI_SPP_CONNECTED %d\n", Info->event);
        locDev->spp_connected_flag = true;
        app_user_connected(0, APP_USER_CONNECTED);
		PRODUCT_TEST_TRACE("SPP_CONNECTED");
    }
    else if (BTIF_SPP_EVENT_REMDEV_DISCONNECTED == Info->event)
    {
        PRODUCT_TEST_TRACE("::AI_SPP_DISCONNECTED %d\n", Info->event);
        locDev->spp_connected_flag = false;
        app_user_spp_tx_done_func = NULL;
		app_user_disconnected(APP_USER_DISCONNECTED);
    }
    else if (BTIF_SPP_EVENT_DATA_SENT == Info->event)
    {

    }
    else
    {
        PRODUCT_TEST_TRACE("::unknown event %d\n", Info->event);
    }
}

void app_user_spp_init(spp_port_t portType, osMutexId mid)
{

    PRODUCT_TEST_TRACE();
    btif_sdp_record_param_t param;
    uint8_t *rx_buf;
	
    if (NULL == user_spp_dev)
    {
        user_spp_dev = app_create_spp_device();
    }

	rx_buf = user_spp_dev->rx_buffer = &UserSppRxBuf[0];
    btif_spp_init_rx_buf(user_spp_dev, rx_buf, USER_SPP_RECV_BUFFER_SIZE);

    user_spp_dev->portType = portType;
    user_spp_dev->app_id = BTIF_APP_SPP_SERVER_FP_RFCOMM_ID;

    user_spp_dev->spp_handle_data_event_func = user_spp_data_event_callback;


	param.attrs = UserSppSdpAttributes;
    param.attr_count = ARRAY_SIZE(UserSppSdpAttributes);
    param.COD = BTIF_COD_MAJOR_PERIPHERAL;

    app_spp_open(user_spp_dev, NULL, &param, mid, RFCOMM_CHANNEL_FP, user_spp_callback);
	
}

void app_user_spp_client_init(void)
{
    PRODUCT_TEST_TRACE("%d ", __LINE__);
    osMutexId mid;

    mid = osMutexCreate(osMutex(user_client_spp_mutex));
    if (!mid)
    {
        ASSERT(0, "cannot create mutex");
    }

#if BTIF_SPP_CLIENT == BTIF_ENABLED
    app_user_spp_init(BTIF_SPP_CLIENT_PORT, mid);
#else
    PRODUCT_TEST_TRACE("!!!SPP_CLIENT is no support ");
#endif
}


void app_user_spp_server_init(void)
{
    osMutexId mid;

    mid = osMutexCreate(osMutex(user_server_spp_mutex));
    if (!mid)
    {
        ASSERT(0, "cannot create mutex");
    }

    PRODUCT_TEST_TRACE("%d ", __LINE__);
#if BTIF_SPP_SERVER == BTIF_ENABLED
    app_user_spp_init(BTIF_SPP_SERVER_PORT, mid);
#else
    PRODUCT_TEST_TRACE("!!!SPP_SERVER is no support ");
#endif
}

void user_open_spp(void)
{

	btif_spp_open(user_spp_dev, NULL, user_spp_callback);


}

void user_close_spp(void)
{

	btif_spp_close(user_spp_dev);
	PRODUCT_TEST_TRACE("1111111111111111111111111");


}
