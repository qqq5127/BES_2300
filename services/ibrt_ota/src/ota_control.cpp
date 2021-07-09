#include "hal_iomux.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "hal_bootmode.h"
#include "hal_norflash.h"
#include "hal_cmu.h"
#include "pmu.h"
#include "crc32.h"
#include "cmsis_nvic.h"
#include "string.h"
#include "ota_control.h"
#include "hal_cache.h"
#include "factory_section.h"
#include "bt_drv_interface.h"
#include "ota_spp.h"
#include "ota_bes.h"
#include "rwapp_config.h"
#include "hal_wdt.h"
#include "norflash_api.h"
#include "nvrecord.h"
#include "factory_section.h"
#if (BLE_APP_OTA)
#include "app_ble_rx_handler.h"
#include "app_ota.h"
#include "bt_drv_internal.h"
#endif
#include "apps.h"
#include "app_utils.h"
#include "app_bt.h"
#include "hal_aud.h"
#include "btapp.h"
#include "app_tws_ctrl_thread.h"
#include "app_ibrt_ota_cmd.h"
#ifdef __GATT_OVER_BR_EDR__
#include "btgatt_api.h"
#endif

#ifdef IBRT
#include "app_tws_ibrt.h"
#include "app_ibrt_ui.h"
#endif
#include "app.h"
#include "app_ibrt_if.h"

#define DelayMs(a)      hal_sys_timer_delay(MS_TO_TICKS(a))

#define OTA_CONTROL_NORFLASH_BUFFER_LEN    (FLASH_SECTOR_SIZE_IN_BYTES*2)
#define OTA_DATA_PKT_SYNC_MS       1

#if PUYA_FLASH_ERASE_LIMIT
static uint8_t upsize_segment_cnt = 0;
static uint8_t upsize_erase_cnt = 0;
#define UPSIZE_ELEMENT_LIMIT    4           //No more than UPSIZE_ELEMENT_LIMIT+1 Writes after a Erase
#define UPSIZE_WRITE_FREQ       10          //Write once every UPSIZE_WRITE_FREQ usual upgrade
#endif
extern struct app_env_tag app_env;
extern uint32_t __ota_upgrade_log_start[];
extern OtaContext ota;
uint32_t new_image_copy_flash_offset;
static uint8_t ota_role_switch_req_timer_num = 1;
static bool ota_get_pkt_in_role_switch = false;
#define LEN_OF_IMAGE_TAIL_TO_FIND_SANITY_CRC    512
static const char* image_info_sanity_crc_key_word = "CRC32_OF_IMAGE=0x";

FLASH_OTA_UPGRADE_LOG_FLASH_T *otaUpgradeLogInFlash = (FLASH_OTA_UPGRADE_LOG_FLASH_T *)__ota_upgrade_log_start;
#define otaUpgradeLog   (*otaUpgradeLogInFlash)
FLASH_OTA_BOOT_INFO_T otaBootInfoInFlash = { NORMAL_BOOT, 0, 0} ;

#define HW_VERSION_LENTH        4    //sizeof(FIRMWARE_REV_INFO_T)
uint8_t peerFwRevInfo[HW_VERSION_LENTH] = {0};
static uint8_t ibrt_tws_mode = 0;
#if OTA_DATA_PKT_SYNC_MS
static uint8_t ota_pkt_sync_cache = 0;
#endif
static uint8_t isInBesOtaState = false;
static const bool norflash_api_mode_async = true;
extern "C" void system_get_info(uint8_t *fw_rev_0, uint8_t *fw_rev_1,  uint8_t *fw_rev_2, uint8_t *fw_rev_3);
void ota_status_change(bool status);
OTA_CONTROL_ENV_T ota_control_env __attribute__((aligned(4096)));
static bool ota_control_check_image_crc(void);
static uint32_t user_data_nv_flash_offset;
static void app_update_ota_boot_info(FLASH_OTA_BOOT_INFO_T* otaBootInfo);
static void app_update_magic_number_of_app_image(uint32_t newMagicNumber);

static void BesOtaErase(uint32_t flashOffset);
static void BesOtaProgram(uint32_t flashOffset, uint8_t* ptr, uint32_t len);
static void BesFlushPendingFlashOp(enum NORFLASH_API_OPRATION_TYPE type);

static uint8_t ota_role_switch_flag = 0;
static bool ota_role_switch_initiator = false;
osTimerId ota_role_switch_req_timer_id = NULL;
static int app_forbidden_roleswitch_handling_timer_handler(void const *param);
osTimerDef (APP_POST_ROLESWITCH_FORRIDEN_TIMER, (void (*)(void const *))app_forbidden_roleswitch_handling_timer_handler);

static void ota_env_var_init(void)
{
    ota_role_switch_flag = 0;
    ota_get_pkt_in_role_switch = false;
    ota_pkt_sync_cache = 0x00;
    app_set_ota_role_switch_initiator(false);
}

static int app_forbidden_roleswitch_handling_timer_handler(void const *param)
{
    osTimerStop(ota_role_switch_req_timer_id);
    if(ota_role_switch_req_timer_num < 2)
    {
        ota_control_send_start_role_switch();
        osTimerStart(ota_role_switch_req_timer_id,200);
        ota_role_switch_req_timer_num++;
    }
    else if (ota_role_switch_req_timer_num == 2)
    {
        app_set_ota_role_switch_initiator(false);
    }
    return 0;
}

void bes_ota_send_role_switch_req(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if(ota_role_switch_req_timer_id == NULL)
    {
        ota_role_switch_req_timer_id = osTimerCreate(osTimer(APP_POST_ROLESWITCH_FORRIDEN_TIMER), osTimerOnce, NULL);
    }

    if((p_ibrt_ctrl->master_tws_switch_pending == false)&&\
        (p_ibrt_ctrl->slave_tws_switch_pending == false))
    {
        osTimerStart(ota_role_switch_req_timer_id, 200);
        ota_control_send_start_role_switch();
        ota_role_switch_req_timer_num = 1;
    }
}

static const char *typeCode2Str(uint8_t typeCode) {
    #define CASE_M(s) \
        case s: return "["#s"]";

    switch(typeCode) {
    CASE_M(OTA_COMMAND_START)
    CASE_M(OTA_RSP_START)
    CASE_M(OTA_COMMAND_SEGMENT_VERIFY)
    CASE_M(OTA_RSP_SEGMENT_VERIFY)
    CASE_M(OTA_RSP_RESULT)
    CASE_M(OTA_DATA_PACKET)
    CASE_M(OTA_COMMAND_CONFIG_OTA)
    CASE_M(OTA_RSP_CONFIG)
    CASE_M(OTA_COMMAND_GET_OTA_RESULT)
    CASE_M(OTA_READ_FLASH_CONTENT)
    CASE_M(OTA_FLASH_CONTENT_DATA)
    CASE_M(OTA_DATA_ACK)
    CASE_M(OTA_COMMAND_RESUME_VERIFY)
    CASE_M(OTA_RSP_RESUME_VERIFY)
    CASE_M(OTA_COMMAND_GET_VERSION)
    CASE_M(OTA_RSP_VERSION)
    CASE_M(OTA_COMMAND_SIDE_SELECTION)
    CASE_M(OTA_RSP_SIDE_SELECTION)
    CASE_M(OTA_COMMAND_IMAGE_APPLY)
    CASE_M(OTA_RSP_IMAGE_APPLY)
    CASE_M(OTA_RSP_START_ROLE_SWITCH)
    }

    return "[]";
}

#ifndef BLE_ENABLE
#define VOICEPATH_COMMON_OTA_BUFF_SIZE 4096

uint8_t* app_voicepath_get_common_ota_databuf(void)
{
    static uint8_t voicepath_common_ota_buf[VOICEPATH_COMMON_OTA_BUFF_SIZE];
    return voicepath_common_ota_buf;
}
#endif

static void Bes_enter_ota_state(void)
{
    if (isInBesOtaState)
    {
        return;
    }

#ifdef IBRT
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    btdrv_reg_op_set_tws_link_duration(IBRT_TWS_LINK_LARGE_DURATION);
    btdrv_reg_op_set_private_tws_poll_interval(p_ibrt_ctrl->config.default_private_poll_interval,
                                               p_ibrt_ctrl->config.default_private_poll_interval_in_sco);
#endif

    // 1. switch to the highest freq
    app_sysfreq_req(APP_SYSFREQ_USER_OTA, APP_SYSFREQ_208M);

    // 2. exit bt sniff mode
#ifdef IBRT
    app_ibrt_ui_judge_link_policy(OTA_START_TRIGGER, BTIF_BLP_DISABLE_ALL);

    if (app_tws_ibrt_tws_link_connected() && \
        (p_ibrt_ctrl->nv_role == IBRT_MASTER) && \
        p_ibrt_ctrl->p_tws_remote_dev)
    {
        btif_me_stop_sniff(p_ibrt_ctrl->p_tws_remote_dev);
    }
#else
    app_bt_active_mode_set(ACTIVE_MODE_KEEPER_OTA, UPDATE_ACTIVE_MODE_FOR_ALL_LINKS);
#endif

#ifdef BLE_ENABLE
    // 3. update BLE conn parameter
    if (DATA_PATH_BLE == ota_control_get_datapath_type())
    {
        app_ble_update_conn_param_mode(BLE_CONN_PARAM_MODE_OTA, true);
    }
#endif
    isInBesOtaState = true;
    ota_env_var_init();
}

void Bes_exit_ota_state(void)
{
    if (!isInBesOtaState)
    {
        return;
    }

#ifdef IBRT
    app_ibrt_ui_t *p_ibrt_ui = app_ibrt_ui_get_ctx();
    btdrv_reg_op_set_tws_link_duration(IBRT_TWS_LINK_DEFAULT_DURATION);
    btdrv_reg_op_set_private_tws_poll_interval(p_ibrt_ui->config.long_private_poll_interval,
                                               p_ibrt_ui->config.default_private_poll_interval_in_sco);
#endif

    app_sysfreq_req(APP_SYSFREQ_USER_OTA, APP_SYSFREQ_32K);

#ifdef IBRT
    app_ibrt_ui_judge_link_policy(OTA_STOP_TRIGGER, BTIF_BLP_SNIFF_MODE);
#else
    app_bt_active_mode_clear(ACTIVE_MODE_KEEPER_OTA, UPDATE_ACTIVE_MODE_FOR_ALL_LINKS);
#endif

#ifdef BLE_ENABLE
    app_ble_update_conn_param_mode(BLE_CONN_PARAM_MODE_OTA, false);
    app_ble_update_conn_param_mode(BLE_CONN_PARAM_MODE_OTA_SLOWER, false);
#endif

    isInBesOtaState = false;
    ota_env_var_init();
}

extern "C" void appm_disconnect_all(void);

void ota_control_send_start_role_switch(void)
{
    uint8_t ibrt_role_switch_parm = OTA_RS_INFO_MASTER_SEND_RS_REQ_CMD;

    OTA_START_ROLE_SWITCH_T tRsp = {OTA_RSP_START_ROLE_SWITCH};
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if(app_get_bes_ota_state())
    {
        ota_role_switch_flag = 1;
        if ((p_ibrt_ctrl->current_role == IBRT_MASTER) && (ota_control_env.transmitHander != NULL))
        {
            TRACE(0,"[OTA] send 0x95 cmd complete");
            ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
        }
        else if (p_ibrt_ctrl->current_role == IBRT_SLAVE)
        {
            osTimerStop(ota_role_switch_req_timer_id);
            TRACE(0,"[OTA] tws cmd : info send 0x95 cmd complete");
            tws_ctrl_send_cmd(IBRT_OTA_TWS_ROLE_SWITCH_CMD,&ibrt_role_switch_parm,1);
        }
    }
}

void ota_control_send_role_switch_complete(void)
{
    uint8_t ibrt_role_switch_parm = OTA_RS_INFO_MASTER_DISCONECT_CMD;

    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if((ota_control_get_datapath_type() == DATA_PATH_SPP)&&(app_get_bes_ota_state()))
    {
        if((p_ibrt_ctrl->current_role == IBRT_MASTER)&&(ota_control_env.transmitHander != NULL))
        {
            OTA_ROLE_SWITCH_COMPLETE_T tRsp = {OTA_RSP_ROLE_SWITCH_COMPLETE};
            TRACE(0,"[OTA] send 0x96 cmd complete");
            ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
        }
        else if ((p_ibrt_ctrl->current_role == IBRT_SLAVE) && ota_get_pkt_in_role_switch)
        {
             tws_ctrl_send_cmd(IBRT_OTA_TWS_ROLE_SWITCH_CMD,&ibrt_role_switch_parm,1);
             ota_get_pkt_in_role_switch = false;
        }
    }

    ota_role_switch_flag = 0;
}

static void ota_update_userdata_pool(void)
{

    if (ota_control_env.configuration.isToClearUserData)
    {
        nv_record_rebuild();
    }

    if (ota_control_env.configuration.isToRenameBT || ota_control_env.configuration.isToRenameBLE ||
        ota_control_env.configuration.isToUpdateBTAddr || ota_control_env.configuration.isToUpdateBLEAddr)
    {
        factory_section_t* pOrgFactoryData, *pUpdatedFactoryData;
        pOrgFactoryData = (factory_section_t *)(OTA_FLASH_LOGIC_ADDR + ota_control_env.flasehOffsetOfFactoryDataPool);
        memcpy(ota_control_env.dataBufferForBurning, (uint8_t *)pOrgFactoryData,
            FLASH_SECTOR_SIZE_IN_BYTES);
        pUpdatedFactoryData = (factory_section_t *)(ota_control_env.dataBufferForBurning);
        uint32_t nv_record_dev_rev = factory_section_get_version();

        if (1 == nv_record_dev_rev)
        {
            if (ota_control_env.configuration.isToRenameBT)
            {
//                memset(pUpdatedFactoryData->data.device_name, 0, sizeof(pUpdatedFactoryData->data.device_name));
                memcpy(pUpdatedFactoryData->data.device_name, (uint8_t *)(ota_control_env.configuration.newBTName),
                    BES_OTA_NAME_LENGTH);
            }

            if (ota_control_env.configuration.isToUpdateBTAddr)
            {
                memcpy(pUpdatedFactoryData->data.bt_address, (uint8_t *)(ota_control_env.configuration.newBTAddr),
                    BD_ADDR_LEN);
            }

            if (ota_control_env.configuration.isToUpdateBLEAddr)
            {
                memcpy(pUpdatedFactoryData->data.ble_address, (uint8_t *)(ota_control_env.configuration.newBLEAddr),
                    BD_ADDR_LEN);
            }

            pUpdatedFactoryData->head.crc =
                crc32(0,(unsigned char *)(&(pUpdatedFactoryData->head.reserved0)),
                    sizeof(factory_section_t)-2-2-4-(5+63+2+2+2+1+8)*sizeof(int));
        }
        else
        {
            if (ota_control_env.configuration.isToRenameBT) {
//              memset(pUpdatedFactoryData->data.rev2_bt_name, 0,
//                     sizeof(pUpdatedFactoryData->data.rev2_bt_name));
              memcpy(pUpdatedFactoryData->data.rev2_bt_name,
                     (uint8_t *) (ota_control_env.configuration.newBTName),
                     BES_OTA_NAME_LENGTH);
            }

            if (ota_control_env.configuration.isToRenameBLE) {
                memset(pUpdatedFactoryData->data.rev2_ble_name,
                    0, sizeof(pUpdatedFactoryData->data.rev2_ble_name));
                memcpy(pUpdatedFactoryData->data.rev2_ble_name,
                    (uint8_t *)(ota_control_env.configuration.newBLEName),
                    BES_OTA_NAME_LENGTH);
            }

            if (ota_control_env.configuration.isToUpdateBTAddr) {
              memcpy(pUpdatedFactoryData->data.rev2_bt_addr,
                     (uint8_t *) (ota_control_env.configuration.newBTAddr),
                     BD_ADDR_LEN);
            }

            if (ota_control_env.configuration.isToUpdateBLEAddr) {
              memcpy(pUpdatedFactoryData->data.rev2_ble_addr,
                     (uint8_t *) (ota_control_env.configuration.newBLEAddr),
                     BD_ADDR_LEN);
            }

             pUpdatedFactoryData->head.crc =
                crc32(0,(unsigned char *)(&(pUpdatedFactoryData->head.reserved0)),
                    sizeof(factory_section_t)-2-2-4-(5+63+2+2+2+1+8)*sizeof(int));
            pUpdatedFactoryData->data.rev2_crc =
                crc32(0,(unsigned char *)(&(pUpdatedFactoryData->data.rev2_reserved0)),
                pUpdatedFactoryData->data.rev2_data_len);
        }

        BesOtaErase(ota_control_env.flasehOffsetOfFactoryDataPool);
        BesOtaProgram(ota_control_env.flasehOffsetOfFactoryDataPool,
            (uint8_t *)pUpdatedFactoryData, FLASH_SECTOR_SIZE_IN_BYTES);
    }

    BesFlushPendingFlashOp(NORFLASH_API_ALL);
}

/**
 * @brief update MTU, called when the MTU exchange indication is received. The MTU exchange request is sent
 *       right after connection is created.
 *
 * @param mtu    MTU size to update
 *
 */
void ota_control_update_MTU(uint16_t mtu)
{
    // remove the 3 bytes of overhead
    ota_control_env.dataPacketSize = mtu - 3;
    LOG_DBG(1,"updated data packet size is %d", ota_control_env.dataPacketSize);
}

/**
 * @brief Register the data transmission handler. Called from SPP or BLE layer
 *
 * @param transmit_handle    Handle of the transmission handler
 *
 */
void ota_control_register_transmitter(ota_transmit_data_t transmit_handle)
{
    ota_control_env.transmitHander = transmit_handle;
}

void ota_control_set_datapath_type(uint8_t datapathType)
{
    ota_control_env.dataPathType = datapathType;
}

uint8_t ota_control_get_datapath_type(void)
{
    return ota_control_env.dataPathType;
}
/**
 * @brief Reset the environment of the OTA handling
 *
 */
void ota_control_reset_env(void)
{

    ota_control_env.configuration.startLocationToWriteImage = new_image_copy_flash_offset;
    ota_control_env.offsetInFlashToProgram = new_image_copy_flash_offset;

    ota_control_env.offsetInFlashOfCurrentSegment = ota_control_env.offsetInFlashToProgram;

    ota_control_env.configuration.isToClearUserData = true;
    ota_control_env.configuration.isToRenameBLE = false;
    ota_control_env.configuration.isToRenameBT = false;
    ota_control_env.configuration.isToUpdateBLEAddr = false;
    ota_control_env.configuration.isToUpdateBTAddr = false;
    ota_control_env.configuration.lengthOfFollowingData = 0;
    ota_control_env.AlreadyReceivedConfigurationLength = 0;
    ota_control_env.flasehOffsetOfUserDataPool = user_data_nv_flash_offset;
    ota_control_env.flasehOffsetOfFactoryDataPool = user_data_nv_flash_offset + FLASH_SECTOR_SIZE_IN_BYTES;
    ota_control_env.crc32OfSegment = 0;
    ota_control_env.crc32OfImage = 0;
    ota_control_env.offsetInDataBufferForBurning = 0;
    ota_control_env.alreadyReceivedDataSizeOfImage = 0;
    ota_control_env.offsetOfImageOfCurrentSegment = 0;
    ota_status_change(false);
    ota_control_env.isPendingForReboot = false;

    ota_control_env.leftSizeOfFlashContentToRead = 0;

    if(ota_control_env.resume_at_breakpoint == false)
    {
        ota_control_env.breakPoint = 0;
        ota_control_env.i_log = -1;
    }

    ota_control_env.dataBufferForBurning = app_voicepath_get_common_ota_databuf();
}

/**
 * @brief Send the response to start OTA control packet
 *
 *
 */
static void ota_control_send_start_response(bool isViaBle)
{
/*    if (isViaBle)
    {
        ota_control_env.dataPacketSize = BES_OTA_BLE_DATA_PACKET_MAX_SIZE;
    }
    else
    {
        ota_control_env.dataPacketSize = BES_OTA_BT_DATA_PACKET_MAX_SIZE;
    }*/
    OTA_START_RSP_T tRsp =
        {OTA_RSP_START, BES_OTA_START_MAGIC_CODE, OTA_SW_VERSION, OTA_HW_VERSION,
        ota_control_env.dataPacketSize};

    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

void ibrt_ota_send_start_response(bool isViaBle)
{
    ota_control_send_start_response(isViaBle);
}

static void ota_control_send_configuration_response(bool isDone)
{
    OTA_RSP_CONFIG_T tRsp = {OTA_RSP_CONFIG, isDone};

    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

void ibrt_ota_send_configuration_response(bool isDone)
{
    ota_control_send_configuration_response(isDone);
}

#if DATA_ACK_FOR_SPP_DATAPATH_ENABLED
static void ota_control_send_data_ack_response(void)
{
    uint8_t packeType = OTA_DATA_ACK;
    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander(&packeType, sizeof(packeType));
    }
}
#endif

static void ota_control_send_version_rsp(void)
{
    uint8_t deviceType;
    OTA_RSP_OTA_VERSION_T tRsp =
    {OTA_RSP_VERSION, BES_OTA_START_MAGIC_CODE, };

    uint8_t fw_rev[4];
    system_get_info(&fw_rev[0], &fw_rev[1], &fw_rev[2], &fw_rev[3]);
    deviceType = 0x00;
    for(int i=0; i<HW_VERSION_LENTH; i++)
    {
        tRsp.leftVersion[i] = fw_rev[i];
        tRsp.rightVersion[i] = 0;
    }
    tRsp.deviceType = deviceType;
    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

void ibrt_ota_send_version_rsp(void)
{
    ota_control_send_version_rsp();
}

void ota_control_side_selection_rsp(uint8_t success)
{
    OTA_RSP_SELECTION_T tRsp =
        {OTA_RSP_SIDE_SELECTION, success};

    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

void ota_control_image_apply_rsp(uint8_t success)
{
    OTA_RSP_APPLY_T tRsp =
        {OTA_RSP_IMAGE_APPLY, success};
    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

/**
 * @brief Send the response to segement verification request
 *
 * @param isPass    false if failed and central will retransmit the former segment.
 *
 */
static void ota_control_send_segment_verification_response(bool isPass)
{
    LOG_DBG(1,"Segment of image's verification pass status is %d (1:pass 0:failed)", isPass);
    OTA_RSP_SEGMENT_VERIFY_T tRsp =
        {OTA_RSP_SEGMENT_VERIFY, isPass};
    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

void ibrt_ota_send_segment_verification_response(bool isPass)
{
    ota_control_send_segment_verification_response(isPass);
}

/**
 * @brief Send the response to inform central that the OTA has been successful or not
 *
 */
static void ota_control_send_result_response(uint8_t isSuccessful)
{
    OTA_RSP_OTA_RESULT_T tRsp =
        {OTA_RSP_RESULT, isSuccessful};

    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if(p_ibrt_ctrl->current_role != IBRT_MASTER)
    {
        return;
    }
    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

static void ota_control_send_get_irk_response(void)
{
    uint8_t data[18] = {0x00};
    uint8_t len = 0;

    data[0] = OTA_GET_LOCAL_IRK_RSP;
#ifdef BLE_USE_RPA
    len = 18;
    data[1] = 16;
    memcpy(&data[2],app_env.loc_irk,16);
#else
    len = 2;
    data[1] = 0x00;
#endif

    if ((ota_control_get_datapath_type() == DATA_PATH_BLE) && \
        (ota_control_env.transmitHander != NULL))
    {
        ota_control_env.transmitHander(data, len);
    }
}

void ibrt_ota_send_result_response(uint8_t isSuccessful)
{
    ota_control_send_result_response(isSuccessful);
}

static void BesFlushPendingFlashOp(enum NORFLASH_API_OPRATION_TYPE type)
{
    hal_trace_pause();
    do
    {
        norflash_api_flush();
        if (NORFLASH_API_ALL != type)
        {
            if (0 == norflash_api_get_used_buffer_count(NORFLASH_API_MODULE_ID_OTA, type))
            {
                break;
            }
        }
        else
        {
            if (norflash_api_buffer_is_free(NORFLASH_API_MODULE_ID_OTA))
            {
                break;
            }
        }

        osDelay(10);
    } while(1);

    hal_trace_continue();
}

static void BesOtaErase(uint32_t flashOffset)
{
    LOG_DBG(2,"[%s] flashOffset=0x%x.",__func__, flashOffset);
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;
    bool isFlashVacant = false;
    uint8_t flashContent = 0xff;

    flashOffset &= 0xFFFFFF;

    for(uint32_t checknum = 0 ; checknum < FLASH_SECTOR_SIZE_IN_BYTES ; checknum ++)
    {
        memcpy(&flashContent, (uint8_t *)(OTA_FLASH_LOGIC_ADDR + flashOffset + checknum),
                sizeof(uint8_t));
        if(flashContent != 0xFF)
        {
            isFlashVacant = false;
            break;
        }
        else
        {
            isFlashVacant = true;
        }

    }

    if(isFlashVacant == true)
    {
        LOG_DBG(1,"%s: flash sector is empty, no need erase.",__func__);
        return;
    }

    do
    {
        lock = int_lock_global();
        hal_trace_pause();

        ret = norflash_api_erase(NORFLASH_API_MODULE_ID_OTA, (OTA_FLASH_LOGIC_ADDR + flashOffset),
            FLASH_SECTOR_SIZE_IN_BYTES,
            norflash_api_mode_async);

        hal_trace_continue();
        int_unlock_global(lock);

        if (NORFLASH_API_OK == ret)
        {
            LOG_DBG(1,"%s: norflash_api_erase ok!",__func__);
            break;
        }
        else if(NORFLASH_API_BUFFER_FULL == ret)
        {
            LOG_DBG(0,"Flash async cache overflow! To flush it.");
            BesFlushPendingFlashOp(NORFLASH_API_ERASING);
        }
        else
        {
            ASSERT(0,"OtaErase: norflash_api_erase failed. ret = %d",ret);
        }
    } while(1);
}

static void BesOtaProgram(uint32_t flashOffset, uint8_t* ptr, uint32_t len)
{
    LOG_DBG(3,"[%s] flashOffset=0x%x, len=0x%x.",__func__, flashOffset, len);
    uint32_t lock;
    enum NORFLASH_API_RET_T ret;

    flashOffset &= 0xFFFFFF;

    do
    {
        lock = int_lock_global();
        hal_trace_pause();

        ret = norflash_api_write(NORFLASH_API_MODULE_ID_OTA,
            (OTA_FLASH_LOGIC_ADDR + flashOffset), ptr, len,norflash_api_mode_async);

        hal_trace_continue();

        int_unlock_global(lock);

        if (NORFLASH_API_OK == ret)
        {
            TRACE(1,"%s: norflash_api_write ok!",__func__);
            break;
        }
        else if (NORFLASH_API_BUFFER_FULL == ret)
        {
            TRACE(0,"Flash async cache overflow! To flush it.");
            BesFlushPendingFlashOp(NORFLASH_API_WRITTING);
        }
        else
        {
            ASSERT(0,"_write_flash: norflash_api_write failed. ret = %d",ret);
        }
    } while(1);
}

/**
 * @brief Program the data in the data buffer to flash.
 *
 * @param ptrSource    Pointer of the source data buffer to program.
 * @param lengthToBurn    Length of the data to program.
 * @param offsetInFlashToProgram    Offset in bytes in flash to program
 *
 */
static void ota_control_flush_data_to_flash(uint8_t* ptrSource, uint32_t lengthToBurn, uint32_t offsetInFlashToProgram)
{
    LOG_DBG(2,"flush %d bytes to flash offset 0x%x", lengthToBurn, offsetInFlashToProgram);

    uint32_t preBytes = (FLASH_SECTOR_SIZE_IN_BYTES - (offsetInFlashToProgram%FLASH_SECTOR_SIZE_IN_BYTES))%FLASH_SECTOR_SIZE_IN_BYTES;
    if (lengthToBurn < preBytes)
    {
        preBytes = lengthToBurn;
    }

    uint32_t middleBytes = 0;
    if (lengthToBurn > preBytes)
    {
       middleBytes = ((lengthToBurn - preBytes)/FLASH_SECTOR_SIZE_IN_BYTES*FLASH_SECTOR_SIZE_IN_BYTES);
    }
    uint32_t postBytes = 0;
    if (lengthToBurn > (preBytes + middleBytes))
    {
        postBytes = (offsetInFlashToProgram + lengthToBurn)%FLASH_SECTOR_SIZE_IN_BYTES;
    }

    LOG_DBG(3,"Prebytes is %d middlebytes is %d postbytes is %d", preBytes, middleBytes, postBytes);

    if (preBytes > 0)
    {
        BesOtaProgram(offsetInFlashToProgram, ptrSource, preBytes);

        ptrSource += preBytes;
        offsetInFlashToProgram += preBytes;
    }

    uint32_t sectorIndexInFlash = offsetInFlashToProgram/FLASH_SECTOR_SIZE_IN_BYTES;

    if (middleBytes > 0)
    {
        uint32_t sectorCntToProgram = middleBytes/FLASH_SECTOR_SIZE_IN_BYTES;
        for (uint32_t sector = 0;sector < sectorCntToProgram;sector++)
        {
            BesOtaErase(sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES);
            BesOtaProgram(sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES,
                ptrSource + sector*FLASH_SECTOR_SIZE_IN_BYTES, FLASH_SECTOR_SIZE_IN_BYTES);

            sectorIndexInFlash++;
        }

        ptrSource += middleBytes;
    }

    if (postBytes > 0)
    {
        BesOtaErase(sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES);
        BesOtaProgram(sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES,
                ptrSource, postBytes);
    }

    BesFlushPendingFlashOp(NORFLASH_API_ALL);
}

/**
 * ota_control_flush_data_to_flash() doesn't erase sector firstly on non sector boundary addresses,
 * so the erase_segment() function is supplemented.
 */
static void erase_segment(uint32_t addr, uint32_t length)
{
    uint32_t bytes = addr & (FLASH_SECTOR_SIZE_IN_BYTES - 1);  // The number of bytes current address minus the previous sector boundary address.
    uint32_t sector_num = (bytes + length)/FLASH_SECTOR_SIZE_IN_BYTES + 1;  //The number of sectors to be erased
    if(bytes)
    {
        //memcpy(ota_control_env.dataBufferForBurning,
            //(uint8_t *)(OTA_FLASH_LOGIC_ADDR|addr), bytes);
        for(uint8_t i = 0; i < sector_num; i++)
        {
            BesOtaErase((addr - bytes + i * FLASH_SECTOR_SIZE_IN_BYTES));
        }
       // BesOtaProgram(addr, ota_control_env.dataBufferForBurning, bytes);
    }
    else
    {
        for(uint8_t i = 0; i < sector_num; i++)
        {
            BesOtaErase((addr + i * FLASH_SECTOR_SIZE_IN_BYTES));
        }
    }

}

/**
 * @brief CRC check on the whole image
 *
 * @return true if the CRC check passes, otherwise, false.
 */

static bool ota_control_check_image_crc(void)
{
    uint32_t verifiedDataSize = 0;
    uint32_t crc32Value = 0;
    uint32_t verifiedBytes = 0;
    uint32_t startFlashAddr = (ota_control_env.dstFlashOffsetForNewImage+OTA_FLASH_LOGIC_ADDR);

    LOG_DBG(2,"%s:start word: 0x%x",__func__, *(uint32_t *)startFlashAddr);

    if (*((uint32_t *)startFlashAddr) != NORMAL_BOOT)
    {
        LOG_DBG(3,"%s:%d,Wrong Boot = 0x%p.",
                __func__,__LINE__,ota_control_env.dataBufferForBurning);

        return false;
    }
    else
    {
        uint32_t firstWord = 0xFFFFFFFF;
        crc32Value = crc32(0, (uint8_t *)&firstWord, 4);
        crc32Value = crc32(crc32Value,
            (uint8_t *)(startFlashAddr + 4), ota_control_env.totalImageSize - 4);

        LOG_DBG(2,"Original CRC32 is 0x%x Confirmed CRC32 is 0x%x.", ota_control_env.crc32OfImage, crc32Value);
        if (crc32Value == ota_control_env.crc32OfImage)
        {
            return true;
        }
        else
        {
            LOG_DBG(4,"%s:%d,crc32Value = 0x%x,crc32OfImage = 0x%x.",
                __func__,__LINE__,crc32Value,ota_control_env.crc32OfImage);
            return false;
        }
    }

    while (verifiedDataSize < ota_control_env.totalImageSize)
    {
        if (ota_control_env.totalImageSize - verifiedDataSize > OTA_DATA_BUFFER_SIZE_FOR_BURNING)
        {
            verifiedBytes = OTA_DATA_BUFFER_SIZE_FOR_BURNING;
        }
        else
        {
            verifiedBytes = ota_control_env.totalImageSize - verifiedDataSize;
        }

        memcpy(ota_control_env.dataBufferForBurning,
            (uint8_t *)(OTA_FLASH_LOGIC_ADDR+ota_control_env.dstFlashOffsetForNewImage + verifiedDataSize),
            OTA_DATA_BUFFER_SIZE_FOR_BURNING);

        if (0 == verifiedDataSize)
        {
            if (*(uint32_t *)ota_control_env.dataBufferForBurning != NORMAL_BOOT)
            {
                 LOG_DBG(3,"%s:%d,Wrong Boot = 0x%p.",
                        __func__,__LINE__,ota_control_env.dataBufferForBurning);
                return false;
            }
            else
            {
                *(uint32_t *)ota_control_env.dataBufferForBurning = 0xFFFFFFFF;
            }
        }

        verifiedDataSize += verifiedBytes;

        crc32Value = crc32(crc32Value, (uint8_t *)ota_control_env.dataBufferForBurning,
            verifiedBytes);

   }

    LOG_DBG(2,"Original CRC32 is 0x%x Confirmed CRC32 is 0x%x.", ota_control_env.crc32OfImage, crc32Value);
    if (crc32Value == ota_control_env.crc32OfImage)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ota_update_info(void)
{
    FLASH_OTA_BOOT_INFO_T otaBootInfo = {COPY_NEW_IMAGE, ota_control_env.totalImageSize, ota_control_env.crc32OfImage};
    app_update_ota_boot_info(&otaBootInfo);
    ota_update_userdata_pool();
}

void ota_check_and_reboot_to_use_new_image(void)
{
    if (ota.permissionToApply && ota_control_env.isPendingForReboot == true)
    {
        app_start_postponed_reset();
    }
}

bool ota_is_in_progress(void)
{
    return ota_control_env.isOTAInProgress;
}

void ota_status_change(bool status)
{
    ota_control_env.isOTAInProgress = status;
}

static void ota_control_sending_flash_content(OTA_READ_FLASH_CONTENT_REQ_T* pReq)
{
    OTA_READ_FLASH_CONTENT_RSP_T rsp = {OTA_READ_FLASH_CONTENT, true};

    if (pReq->isToStart)
    {
        LOG_DBG(2,"Getreading flash content request start addr 0x%x size %d",
                    pReq->startAddr, pReq->lengthToRead);

        // check the sanity of the request
        if ((pReq->startAddr >= hal_norflash_get_flash_total_size(HAL_NORFLASH_ID_0)) ||
            ((pReq->startAddr + pReq->lengthToRead) >
            hal_norflash_get_flash_total_size(HAL_NORFLASH_ID_0)))
        {
            LOG_DBG(3,"%s %d total:%d",__func__,__LINE__,hal_norflash_get_flash_total_size(HAL_NORFLASH_ID_0));
            rsp.isReadingReqHandledSuccessfully = false;
        }
        else
        {
            ota_control_env.offsetInFlashToRead = pReq->startAddr;
            ota_control_env.leftSizeOfFlashContentToRead = pReq->lengthToRead;
            LOG_DBG(2,"Start sending flash content start addr 0x%x size %d",
                pReq->startAddr, pReq->lengthToRead);
        }
    }
    else
    {
        LOG_DBG(0,"Get stop reading flash content request.");
        ota_control_env.leftSizeOfFlashContentToRead = 0;
    }
    LOG_DBG(2,"%s %d",__func__,__LINE__);
    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&rsp, sizeof(rsp));
    }
}

static int32_t find_key_word(uint8_t* targetArray, uint32_t targetArrayLen,
    uint8_t* keyWordArray,
    uint32_t keyWordArrayLen)
{
    if ((keyWordArrayLen > 0) && (targetArrayLen >= keyWordArrayLen))
    {
        uint32_t index = 0, targetIndex = 0;
        for (targetIndex = 0;targetIndex < targetArrayLen;targetIndex++)
        {
            for (index = 0;index < keyWordArrayLen;index++)
            {
                if (targetArray[targetIndex + index] != keyWordArray[index])
                {
                    break;
                }
            }

            if (index == keyWordArrayLen)
            {
                return targetIndex;
            }
        }

        return -1;
    }
    else
    {
        return -1;
    }
}

static uint8_t asciiToHex(uint8_t asciiCode)
{
    if ((asciiCode >= '0') && (asciiCode <= '9'))
    {
        return asciiCode - '0';
    }
    else if ((asciiCode >= 'a') && (asciiCode <= 'f'))
    {
        return asciiCode - 'a' + 10;
    }
    else if ((asciiCode >= 'A') && (asciiCode <= 'F'))
    {
        return asciiCode - 'A' + 10;
    }
    else
    {
        return 0xff;
    }
}

static bool ota_check_image_data_sanity_crc(void) {
  // find the location of the CRC key word string
  uint8_t* ptrOfTheLast4KImage = (uint8_t *)(OTA_FLASH_LOGIC_ADDR+NEW_IMAGE_FLASH_OFFSET+
    ota_control_env.totalImageSize-LEN_OF_IMAGE_TAIL_TO_FIND_SANITY_CRC);
  uint32_t sanityCrc32 = 0;

  int32_t sanity_crc_location = find_key_word(ptrOfTheLast4KImage,
    LEN_OF_IMAGE_TAIL_TO_FIND_SANITY_CRC,
    (uint8_t *)image_info_sanity_crc_key_word,
    strlen(image_info_sanity_crc_key_word));
    TRACE(3, "%s,ptrOfTheLast4KImage %p, sanity_crc_location %d", __func__, ptrOfTheLast4KImage, sanity_crc_location);
  if (-1 == sanity_crc_location)
  {
    // if no sanity crc, the image has the old format, just ignore such a check
    return true;
  }

  TRACE(1,"sanity_crc_location is %d", sanity_crc_location);

  uint32_t crc32ImageOffset = sanity_crc_location+ota_control_env.totalImageSize-
    LEN_OF_IMAGE_TAIL_TO_FIND_SANITY_CRC+strlen(image_info_sanity_crc_key_word);
  TRACE(1,"Bytes to generate crc32 is %d", crc32ImageOffset);

  uint8_t* crcString = (uint8_t *)(OTA_FLASH_LOGIC_ADDR+NEW_IMAGE_FLASH_OFFSET+crc32ImageOffset);

  for (uint8_t index = 0;index < 8;index++)
  {
      sanityCrc32 |= (asciiToHex(crcString[index]) << (28-4*index));
  }


//  uint32_t sanityCrc32 = *(uint32_t *)(OTA_FLASH_LOGIC_ADDR+NEW_IMAGE_FLASH_OFFSET+crc32ImageOffset);

  TRACE(1,"sanityCrc32 is 0x%x", sanityCrc32);

  // generate the CRC from image data
  uint32_t calculatedCrc32 = 0;
  calculatedCrc32 = crc32(calculatedCrc32, (uint8_t *)(OTA_FLASH_LOGIC_ADDR+NEW_IMAGE_FLASH_OFFSET),
    crc32ImageOffset);

  TRACE(1,"calculatedCrc32 is 0x%x", calculatedCrc32);

  return (sanityCrc32 == calculatedCrc32);
}


static unsigned int seed = 1;

static void set_rand_seed(unsigned int init)
{
    seed = init;
}

static int get_rand(void)
{
    //Based on Knuth "The Art of Computer Programming"
    seed = seed * 1103515245 + 12345;
    return ( (unsigned int) (seed / 65536) % (32767+1) );
}

void ota_randomCode_log(uint8_t randomCode[])
{
    BesOtaErase((uint32_t)&otaUpgradeLog);
    BesOtaProgram((uint32_t)&otaUpgradeLog, randomCode, sizeof(otaUpgradeLog.randomCode));
}

void ota_update_start_message(void)
{
    BesOtaProgram((uint32_t)&otaUpgradeLog.totalImageSize,
        (uint8_t *)&ota_control_env.totalImageSize, sizeof(ota_control_env.totalImageSize));
    BesOtaProgram((uint32_t)&otaUpgradeLog.crc32OfImage,
        (uint8_t *)&ota_control_env.crc32OfImage, sizeof(ota_control_env.crc32OfImage));
}

void ota_get_start_message(void)
{
    ota_control_env.totalImageSize = otaUpgradeLog.totalImageSize;
    ota_control_env.crc32OfImage = otaUpgradeLog.crc32OfImage;
    LOG_DBG(3,"[%s] totalImageSize = 0x%x, crc32OfImage = 0x%x", __func__, ota_control_env.totalImageSize, ota_control_env.crc32OfImage);
}

void ota_upgradeSize_log(void)
{
#if PUYA_FLASH_ERASE_LIMIT
    if(upsize_segment_cnt < UPSIZE_WRITE_FREQ)
    {
        upsize_segment_cnt++;
        LOG_DBG(0,"PUYA upgradesize return");
        return;
    }
    else if((upsize_segment_cnt == UPSIZE_WRITE_FREQ) && (upsize_erase_cnt < UPSIZE_ELEMENT_LIMIT))
    {
        upsize_segment_cnt = 0;
        upsize_erase_cnt++;
        BesOtaProgram((uint32_t)&otaUpgradeLog.upgradeSize[++ota_control_env.i_log],
                       (uint8_t*)&ota_control_env.alreadyReceivedDataSizeOfImage, 4);
        LOG_DBG(3,"PUYA WRITE {i_log: %d, RecSize: 0x%x, FlashWrSize: 0x%x}", ota_control_env.i_log, \
                ota_control_env.alreadyReceivedDataSizeOfImage, otaUpgradeLog.upgradeSize[ota_control_env.i_log]);
    }
    else if((upsize_segment_cnt == UPSIZE_WRITE_FREQ) && (upsize_erase_cnt == UPSIZE_ELEMENT_LIMIT))
    {
        ota_control_env.i_log = 0;
        upsize_erase_cnt = 0;
        upsize_segment_cnt = 0;
        memcpy(ota_control_env.dataBufferForBurning,
            (uint8_t *)(OTA_FLASH_LOGIC_ADDR|(uint32_t)&otaUpgradeLog),
            OTAUPLOG_HEADSIZE);

        BesOtaErase((uint32_t)&otaUpgradeLog);
        BesOtaProgram((uint32_t)&otaUpgradeLog, ota_control_env.dataBufferForBurning,
            OTAUPLOG_HEADSIZE);
        BesOtaProgram((uint32_t)&otaUpgradeLog.upgradeSize[ota_control_env.i_log],
                       (uint8_t*)&ota_control_env.alreadyReceivedDataSizeOfImage, 4);
        LOG_DBG(3,"PUYA ERASE WRITE {i_log: %d, RecSize: 0x%x, FlashWrSize: 0x%x}", ota_control_env.i_log, \
                ota_control_env.alreadyReceivedDataSizeOfImage, otaUpgradeLog.upgradeSize[ota_control_env.i_log]);

    }
#else
    if(++ota_control_env.i_log >= sizeof(otaUpgradeLog.upgradeSize)/(sizeof(uint32_t)/sizeof(uint8_t)))
    {
        ota_control_env.i_log = 0;

        memcpy(ota_control_env.dataBufferForBurning,
            (uint8_t *)(OTA_FLASH_LOGIC_ADDR|(uint32_t)&otaUpgradeLog),
            OTAUPLOG_HEADSIZE);

        BesOtaErase((uint32_t)&otaUpgradeLog);
        BesOtaProgram((uint32_t)&otaUpgradeLog, ota_control_env.dataBufferForBurning,
            OTAUPLOG_HEADSIZE);

    }

    BesOtaProgram((uint32_t)&otaUpgradeLog.upgradeSize[ota_control_env.i_log],
                       (uint8_t*)&ota_control_env.alreadyReceivedDataSizeOfImage, 4);

    LOG_DBG(3,"{i_log: %d, RecSize: 0x%x, FlashWrSize: 0x%x}", ota_control_env.i_log, ota_control_env.alreadyReceivedDataSizeOfImage, otaUpgradeLog.upgradeSize[ota_control_env.i_log]);
#endif
}

void ota_upgradeLog_destroy(void)
{
    ota_control_env.resume_at_breakpoint = false;
    BesOtaErase((uint32_t)&otaUpgradeLog);
    LOG_DBG(0,"Destroyed upgrade log in flash.");

    BesFlushPendingFlashOp(NORFLASH_API_ERASING);
}

uint32_t get_upgradeSize_log(void)
{
    int32_t *p = (int32_t*)otaUpgradeLog.upgradeSize,
            left = 0, right = sizeof(otaUpgradeLog.upgradeSize)/4 - 1, mid;

    if(p[0] != -1)
    {
        while(left < right)
        {
            mid = (left + right) / 2;
            if(p[mid] == -1)
                right = mid - 1;
            else
                left = mid + 1;
        }
    }
    if(p[left]==-1)
        left--;

    ota_control_env.i_log = left;
    ota_control_env.breakPoint = left!=-1 ? p[left] : 0;
    ota_control_env.resume_at_breakpoint = ota_control_env.breakPoint?true:false;

    LOG_DBG(1,"ota_control_env.i_log: %d", ota_control_env.i_log);
    return ota_control_env.breakPoint;
}

void ota_control_send_resume_response(uint32_t breakPoint, uint8_t randomCode[])
{
    OTA_RSP_RESUME_VERIFY_T tRsp =
        {OTA_RSP_RESUME_VERIFY, breakPoint,};
    for(uint32_t i = 0; i < sizeof(otaUpgradeLog.randomCode); i++)
        tRsp.randomCode[i] = randomCode[i];
    tRsp.crc32 = crc32(0, (uint8_t*)&tRsp.breakPoint, sizeof(tRsp.breakPoint) + sizeof(tRsp.randomCode));

    if(ota_control_env.transmitHander != NULL){
        ota_control_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
    }
}

void ibrt_tws_ota_breakPoint_resume(uint32_t breakPoint, uint8_t randomCode[])
{
    OTA_RSP_RESUME_VERIFY_T tRsp =
        {OTA_RSP_RESUME_VERIFY, breakPoint,};
    for(uint32_t i = 0; i < sizeof(otaUpgradeLog.randomCode); i++)
        tRsp.randomCode[i] = randomCode[i];

    tRsp.crc32 = crc32(0, (uint8_t*)&tRsp.breakPoint, sizeof(tRsp.breakPoint) + sizeof(tRsp.randomCode));
    tws_ctrl_send_cmd(IBRT_OTA_TWS_BP_CHECK_CMD, (uint8_t*)&tRsp, sizeof(OTA_RSP_RESUME_VERIFY_T));

}

static void ota_tws_get_master()
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    ibrt_tws_mode = p_ibrt_ctrl->current_role;
}

void tws_breakPoint_confirm(uint32_t breakPoint, uint8_t randomCode[])
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if(p_ibrt_ctrl->current_role == IBRT_MASTER)
    {
        ibrt_ota_cmd_type = OTA_RSP_RESUME_VERIFY;
        twsBreakPoint = breakPoint;
    }
    else if(p_ibrt_ctrl->current_role == IBRT_SLAVE)
    {
        ibrt_tws_ota_breakPoint_resume(breakPoint, randomCode);
        //tws_ctrl_send_cmd(IBRT_OTA_TWS_BP_CHECK_CMD, ibrt_slave_randomCode, sizeof(OTA_RSP_RESUME_VERIFY_T));
    }
}

static void ota_tws_send_handle(uint8_t typeCode, uint8_t *buff, uint16_t length, uint32_t cmd_dode)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if(p_ibrt_ctrl->current_role == IBRT_MASTER)
    {
        ibrt_ota_cmd_type = typeCode;
    }
    else if(p_ibrt_ctrl->current_role == IBRT_SLAVE)
    {
        tws_ctrl_send_cmd(cmd_dode, (uint8_t *)buff, length);
    }
}
void ota_bes_handle_received_data(uint8_t *otaBuf, bool isViaBle,uint16_t dataLenth)
{
    uint8_t typeCode = otaBuf[0];
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    LOG_DBG(3,"[%s],case = 0x%x%s",__func__, typeCode, typeCode2Str(typeCode));
    switch (typeCode)
    {
        case OTA_DATA_PACKET:
        {
            if ((!ota_is_in_progress()) ||
                ((IBRT_MASTER == p_ibrt_ctrl->current_role) && !app_tws_ibrt_tws_link_connected()))
            {
                ota_control_send_result_response(false);
                return;
            }

            uint8_t* rawDataPtr = &otaBuf[1];

            uint32_t rawDataSize = dataLenth - 1;
            LOG_DBG(1,"Received image data size %d", rawDataSize);
            uint32_t leftDataSize = rawDataSize;
            uint32_t offsetInReceivedRawData = 0;
            do
            {
                uint32_t bytesToCopy;
                // copy to data buffer
                if ((ota_control_env.offsetInDataBufferForBurning + leftDataSize) >
                    OTA_DATA_BUFFER_SIZE_FOR_BURNING)
                {
                    bytesToCopy = OTA_DATA_BUFFER_SIZE_FOR_BURNING - ota_control_env.offsetInDataBufferForBurning;
                }
                else
                {
                    bytesToCopy = leftDataSize;
                }

                leftDataSize -= bytesToCopy;

                memcpy(&ota_control_env.dataBufferForBurning[ota_control_env.offsetInDataBufferForBurning],
                        &rawDataPtr[offsetInReceivedRawData], bytesToCopy);
                offsetInReceivedRawData += bytesToCopy;
                ota_control_env.offsetInDataBufferForBurning += bytesToCopy;
                LOG_DBG(1,"offsetInDataBufferForBurning is %d", ota_control_env.offsetInDataBufferForBurning);
                if (OTA_DATA_BUFFER_SIZE_FOR_BURNING <= ota_control_env.offsetInDataBufferForBurning)
                {
                    LOG_DBG(0,"Program the image to flash.");

                    #if (IMAGE_RECV_FLASH_CHECK == 1)
                    if((ota_control_env.offsetInFlashToProgram - ota_control_env.dstFlashOffsetForNewImage > ota_control_env.totalImageSize) ||
                        (ota_control_env.totalImageSize > MAX_IMAGE_SIZE) ||
                        (ota_control_env.offsetInFlashToProgram & (MIN_SEG_ALIGN - 1)))
                    {
                        LOG_DBG(0,"ERROR: IMAGE_RECV_FLASH_CHECK");
                        LOG_DBG(0," ota_control_env(.offsetInFlashToProgram - .dstFlashOffsetForNewImage >= .totalImageSize)");
                        LOG_DBG(1," or (ota_control_env.totalImageSize > %d)", MAX_IMAGE_SIZE);
                        LOG_DBG(0," or .offsetInFlashToProgram isn't segment aligned");
                        LOG_DBG(3,".offsetInFlashToProgram:0x%x  .dstFlashOffsetForNewImage:0x%x  .totalImageSize:%d", ota_control_env.offsetInFlashToProgram, ota_control_env.dstFlashOffsetForNewImage, ota_control_env.totalImageSize);
                        // ota_upgradeLog_destroy();  // In order to reduce unnecessary erasures and retransmissions we don't imeediately destory the log but reset ota, because a boundary check is performed before flashing and if there is really wrong we'll catch when an image CRC32 check finally.
                        ota_control_send_result_response(OTA_RESULT_ERR_FLASH_OFFSET);
                        return;
                    }
                    #endif

                    ota_control_flush_data_to_flash(ota_control_env.dataBufferForBurning, OTA_DATA_BUFFER_SIZE_FOR_BURNING,
                        ota_control_env.offsetInFlashToProgram);
                    ota_control_env.offsetInFlashToProgram += OTA_DATA_BUFFER_SIZE_FOR_BURNING;
                    ota_control_env.offsetInDataBufferForBurning = 0;
                }
            } while (offsetInReceivedRawData < rawDataSize);

            ota_control_env.alreadyReceivedDataSizeOfImage += rawDataSize;
            LOG_DBG(1,"Image already received %d", ota_control_env.alreadyReceivedDataSizeOfImage);

            #if (IMAGE_RECV_FLASH_CHECK == 1)
            if((ota_control_env.alreadyReceivedDataSizeOfImage > ota_control_env.totalImageSize) ||
                (ota_control_env.totalImageSize > MAX_IMAGE_SIZE))
            {
                LOG_DBG(0,"ERROR: IMAGE_RECV_FLASH_CHECK");
                LOG_DBG(0," ota_control_env(.alreadyReceivedDataSizeOfImage > .totalImageSize)");
                LOG_DBG(1," or (ota_control_env.totalImageSize > %d)", MAX_IMAGE_SIZE);
                LOG_DBG(2,".alreadyReceivedDataSizeOfImage:%d  .totalImageSize:%d", ota_control_env.alreadyReceivedDataSizeOfImage, ota_control_env.totalImageSize);
                //ota_upgradeLog_destroy();  // In order to reduce unnecessary erasures and retransmissions we don't imeediately destory the log but reset ota, because a boundary check is performed before flashing and if there is really wrong we'll catch when an image CRC32 check finally.
                ota_control_send_result_response(OTA_RESULT_ERR_RECV_SIZE);
                return;
            }
            #endif

#if DATA_ACK_FOR_SPP_DATAPATH_ENABLED
            if (DATA_PATH_SPP == ota_control_env.dataPathType)
            {
#if OTA_DATA_PKT_SYNC_MS
                app_send_ota_pkt_sync_ack_response(false);
#else
                ota_control_send_data_ack_response();
#endif
            }
#endif
            break;
        }
        case OTA_COMMAND_SEGMENT_VERIFY:
        {
            #define MAX_SEG_VERIFY_RETEY    3
            static uint32_t seg_verify_retry = MAX_SEG_VERIFY_RETEY;

            OTA_CONTROL_SEGMENT_VERIFY_T* ptVerifyCmd = (OTA_CONTROL_SEGMENT_VERIFY_T *)(otaBuf);

            #if (IMAGE_RECV_FLASH_CHECK == 1)
            if((ota_control_env.offsetInFlashToProgram - ota_control_env.dstFlashOffsetForNewImage > ota_control_env.totalImageSize) ||
                (ota_control_env.totalImageSize > MAX_IMAGE_SIZE) ||
                (ota_control_env.offsetInFlashToProgram & (MIN_SEG_ALIGN - 1)))
            {
                LOG_DBG(0,"ERROR: IMAGE_RECV_FLASH_CHECK");
                LOG_DBG(0," ota_control_env(.offsetInFlashToProgram - .dstFlashOffsetForNewImage >= .totalImageSize)");
                LOG_DBG(1," or (ota_control_env.totalImageSize > %d)", MAX_IMAGE_SIZE);
                LOG_DBG(0," or .offsetInFlashToProgram isn't segment aligned");
                LOG_DBG(3,".offsetInFlashToProgram:0x%x  .dstFlashOffsetForNewImage:0x%x  .totalImageSize:%d", ota_control_env.offsetInFlashToProgram, ota_control_env.dstFlashOffsetForNewImage, ota_control_env.totalImageSize);
                //ota_upgradeLog_destroy();  // In order to reduce unnecessary erasures and retransmissions we don't imeediately destory the log but reset ota, because a boundary check is performed before flashing and if there is really wrong we'll catch when an image CRC32 check finally.
                errOtaCode = OTA_RESULT_ERR_FLASH_OFFSET;
                ota_tws_send_handle(OTA_RSP_SEGMENT_VERIFY, (uint8_t *)&errOtaCode, 1, IBRT_OTA_TWS_SEGMENT_CRC_CMD);
                return;
            }
            #endif

            ota_control_flush_data_to_flash(ota_control_env.dataBufferForBurning, ota_control_env.offsetInDataBufferForBurning,
                    ota_control_env.offsetInFlashToProgram);
            ota_control_env.offsetInFlashToProgram += ota_control_env.offsetInDataBufferForBurning;
            ota_control_env.offsetInDataBufferForBurning = 0;

            LOG_DBG(1,"Calculate the crc32 of the segment,start addr = 0x%x.",OTA_FLASH_LOGIC_ADDR +
            ota_control_env.dstFlashOffsetForNewImage + ota_control_env.offsetOfImageOfCurrentSegment);

            uint32_t startFlashAddr = OTA_FLASH_LOGIC_ADDR +
            ota_control_env.dstFlashOffsetForNewImage + ota_control_env.offsetOfImageOfCurrentSegment;
            uint32_t lengthToDoCrcCheck = ota_control_env.alreadyReceivedDataSizeOfImage-ota_control_env.offsetOfImageOfCurrentSegment;

#if OTA_NON_CACHE_READ_ISSUE_WORKAROUND
            hal_cache_invalidate(HAL_CACHE_ID_D_CACHE,
                (uint32_t)startFlashAddr,
                lengthToDoCrcCheck);
#endif

            ota_control_env.crc32OfSegment = crc32(0, (uint8_t *)(startFlashAddr), lengthToDoCrcCheck);
            LOG_DBG(2,"CRC32 of the segement is 0x%x,receiced num = 0x%x", ota_control_env.crc32OfSegment, ptVerifyCmd->crc32OfSegment);

            if ((BES_OTA_START_MAGIC_CODE == ptVerifyCmd->magicCode) &&
                (ptVerifyCmd->crc32OfSegment == ota_control_env.crc32OfSegment))
            {
                uint8_t ota_segment_crc = 1;
                errOtaCode = 1;
                ota_tws_send_handle(OTA_RSP_SEGMENT_VERIFY, (uint8_t *)&ota_segment_crc, 1, IBRT_OTA_TWS_SEGMENT_CRC_CMD);
                ota_status_change(true);

                ota_upgradeSize_log();
                seg_verify_retry = MAX_SEG_VERIFY_RETEY;

                // backup of the information in case the verification of current segment failed
                ota_control_env.offsetInFlashOfCurrentSegment = ota_control_env.offsetInFlashToProgram;
                ota_control_env.offsetOfImageOfCurrentSegment = ota_control_env.alreadyReceivedDataSizeOfImage;
            }
            else
            {
                if(--seg_verify_retry == 0)
                {
                    seg_verify_retry = MAX_SEG_VERIFY_RETEY;

                    LOG_DBG(0,"ERROR: segment verification retry too much!");
                    ota_upgradeLog_destroy();  // Yes, destory it and retransmit the entire image.
                    errOtaCode = OTA_RESULT_ERR_SEG_VERIFY;
                    ota_tws_send_handle(OTA_RSP_SEGMENT_VERIFY, (uint8_t *)&errOtaCode, 1, IBRT_OTA_TWS_SEGMENT_CRC_CMD);
                    return;
                }

                erase_segment(startFlashAddr, lengthToDoCrcCheck);

                // restore the offset
                ota_control_env.offsetInFlashToProgram = ota_control_env.offsetInFlashOfCurrentSegment;
                ota_control_env.alreadyReceivedDataSizeOfImage = ota_control_env.offsetOfImageOfCurrentSegment;
                uint8_t ota_segment_crc = 0;
                ota_tws_send_handle(OTA_RSP_SEGMENT_VERIFY, (uint8_t *)&ota_segment_crc, 1, IBRT_OTA_TWS_SEGMENT_CRC_CMD);
            }

            // reset the CRC32 value of the segment
            ota_control_env.crc32OfSegment = 0;

            // reset the data buffer
            LOG_DBG(2,"total size is %d already received %d", ota_control_env.totalImageSize,
                ota_control_env.alreadyReceivedDataSizeOfImage);

            break;
        }
        case OTA_COMMAND_START:
        {
            ibrt_ota_cmd_type = 0;
            OTA_CONTROL_START_T* ptStart = (OTA_CONTROL_START_T *)(otaBuf);
            if (BES_OTA_START_MAGIC_CODE == ptStart->magicCode)
            {
                LOG_DBG(0,"Receive command start request:");
                ota_control_reset_env();
                ota_control_env.totalImageSize = ptStart->imageSize;
                ota_control_env.crc32OfImage = ptStart->crc32OfImage;
                ota_status_change(true);

                ota_control_env.AlreadyReceivedConfigurationLength = 0;

                TRACE(2,"Image size is 0x%x, crc32 of image is 0x%x",
                    ota_control_env.totalImageSize, ota_control_env.crc32OfImage);

                ota_update_start_message();

                ota_tws_send_handle(OTA_RSP_START, (uint8_t *)&isViaBle, 1, IBRT_OTA_TWS_START_OTA_CMD);
            }
            break;
        }
        case OTA_COMMAND_RESUME_VERIFY:
        {
            OTA_CONTROL_RESUME_VERIFY_T* ptStart = (OTA_CONTROL_RESUME_VERIFY_T *)(otaBuf);
            ota_tws_get_master();
            if (BES_OTA_START_MAGIC_CODE == ptStart->magicCode)
            {
                uint32_t breakPoint;
                uint8_t *randomCode = NULL;

                if (ota_control_env.dataBufferForBurning == NULL) {
                    LOG_DBG(1,"%s:randomCode buffer is NULL,reset buffer here!", __func__);
                    ota_control_env.dataBufferForBurning = app_voicepath_get_common_ota_databuf();
                }

                randomCode = ota_control_env.dataBufferForBurning;
                memset(randomCode, 0, sizeof(otaUpgradeLog.randomCode));

                LOG_DBG(0,"Receive command resuming verification:");
                if(ptStart->crc32 != crc32(0, ptStart->randomCode, sizeof(ptStart->randomCode) + sizeof(ptStart->segmentSize)))
                {
                    breakPoint = -1;
                    LOG_DBG(0,"Resuming verification crc32 check fail.");
                    goto resume_response;
                }
                LOG_DBG(0,"Resuming verification crc32 check pass.");

                LOG_DBG(1,"Receive segment size 0x%x.", ptStart->segmentSize);

                LOG_DBG(0,"Receive random code:");
                LOG_DUMP("%02x ", ptStart->randomCode, sizeof(otaUpgradeLog.randomCode));

                LOG_DBG(0,"Device's random code:");
                LOG_DUMP("%02x ", otaUpgradeLog.randomCode, sizeof(otaUpgradeLog.randomCode));

                breakPoint = get_upgradeSize_log();
                if(breakPoint & (MIN_SEG_ALIGN - 1))  // Minimum segment alignment.
                {
                    LOG_DBG(1,"Breakpoint:0x%x isn't minimum segment alignment!", breakPoint);
                    ota_upgradeLog_destroy();  // Error in log, we'd better try to retransmit the entire image.
                    #if 1
                    breakPoint = get_upgradeSize_log();
                    #else
                    ota_control_send_result_response(OTA_RESULT_ERR_BREAKPOINT);
                    return;
                    #endif
                }
                if(breakPoint)
                {
                    if(!memcmp(otaUpgradeLog.randomCode, ptStart->randomCode, sizeof(otaUpgradeLog.randomCode)))
                    {
                        ota_control_reset_env();
                        ota_status_change(true);
                        ota_get_start_message();
                        LOG_DBG(1,"OTA can resume. Resuming from the breakpoint at: 0x%x.", breakPoint);
                    }
                    else
                    {
                        LOG_DBG(1,"OTA can't resume because the randomCode is inconsistent. [breakPoint: 0x%x]", breakPoint);

                        breakPoint = ota_control_env.breakPoint = ota_control_env.resume_at_breakpoint = 0;
                    }
                }
                if(breakPoint == 0)
                {
                    if(p_ibrt_ctrl->current_role == IBRT_SLAVE)
                    {
                        LOG_DBG(1,"OTA resume none. Generate new random code for the new transmisson now. [breakPoint: 0x%x]", breakPoint);
                        for(uint32_t i = 0; i < sizeof(otaUpgradeLog.randomCode); i++)
                        {
                            set_rand_seed(hal_sys_timer_get());
                            randomCode[i] = get_rand();
                            DelayMs(1);
                        }

                        ota_randomCode_log(randomCode);

                        LOG_DBG(0,"New random code:");
                        LOG_DUMP("%02x ", randomCode, sizeof(otaUpgradeLog.randomCode));
                    }
                }

            resume_response:
                if(breakPoint == 0)
                {
                    tws_breakPoint_confirm(breakPoint, randomCode);
                }
                else
                {
                    tws_breakPoint_confirm(breakPoint, ptStart->randomCode);
                }
            }
            break;
        }
        case OTA_COMMAND_CONFIG_OTA:
        {
            OTA_FLOW_CONFIGURATION_T* ptConfig = (OTA_FLOW_CONFIGURATION_T *)&(ota_control_env.configuration);
            Bes_enter_ota_state();
            memcpy((uint8_t *)ptConfig + ota_control_env.AlreadyReceivedConfigurationLength,
                &(otaBuf[1]), dataLenth - 1);

            ota_control_env.AlreadyReceivedConfigurationLength += (dataLenth - 1);
            if ((ptConfig->lengthOfFollowingData + 4) <= ota_control_env.AlreadyReceivedConfigurationLength)
            {
                LOG_DBG(1,"lengthOfFollowingData 0x%x", ptConfig->lengthOfFollowingData);
                LOG_DBG(1,"startLocationToWriteImage 0x%x", ptConfig->startLocationToWriteImage);
                LOG_DBG(1,"isToClearUserData %d", ptConfig->isToClearUserData);
                LOG_DBG(1,"isToRenameBT %d", ptConfig->isToRenameBT);
                LOG_DBG(1,"isToRenameBLE %d", ptConfig->isToRenameBLE);
                LOG_DBG(1,"isToUpdateBTAddr %d", ptConfig->isToUpdateBTAddr);
                LOG_DBG(1,"isToUpdateBLEAddr %d", ptConfig->isToUpdateBLEAddr);
                LOG_DBG(0,"New BT name:");
                LOG_DUMP("0x%02x ", ptConfig->newBTName, BES_OTA_NAME_LENGTH);
                LOG_DBG(0,"New BLE name:");
                LOG_DUMP("0x%02x ", ptConfig->newBLEName, BES_OTA_NAME_LENGTH);
                LOG_DBG(0,"New BT addr:");
                LOG_DUMP("0x%02x ", ptConfig->newBTAddr, BD_ADDR_LEN);
                LOG_DBG(0,"New BLE addr:");
                LOG_DUMP("0x%02x ", ptConfig->newBLEAddr, BD_ADDR_LEN);
                LOG_DBG(1,"crcOfConfiguration 0x%x", ptConfig->crcOfConfiguration);

                if((ota_control_env.configuration.startLocationToWriteImage < __APP_IMAGE_FLASH_OFFSET__) &&
                    (ota_control_env.offsetInFlashToProgram < __APP_IMAGE_FLASH_OFFSET__))
                {
                    LOG_DBG(0,"Wrong Start Location. Check OTA Flash Offset of Update Image!");
                    ota_control_send_result_response(OTA_RESULT_ERR_IMAGE_SIZE);
                    ota_control_reset_env();
                    return;
                }

                // check CRC
                if (ptConfig->crcOfConfiguration == crc32(0, (uint8_t *)ptConfig, sizeof(OTA_FLOW_CONFIGURATION_T) - sizeof(uint32_t)))
                {
                    if(ota_control_env.totalImageSize > MAX_IMAGE_SIZE)
                    {
                        LOG_DBG(2,"ImageSize 0x%x greater than 0x%x! Terminate the upgrade.", ota_control_env.totalImageSize, MAX_IMAGE_SIZE);
                        //ota_control_send_result_response(OTA_RESULT_ERR_IMAGE_SIZE);
                        //uint8_t err_image_size = OTA_RESULT_ERR_IMAGE_SIZE;
                        errOtaCode = OTA_RESULT_ERR_IMAGE_SIZE;
                        ota_tws_send_handle(OTA_RSP_CONFIG, (uint8_t *)&errOtaCode, 1, IBRT_OTA_TWS_OTA_CONFIG_CMD);
                        return;
                    }

                    if(ota_control_env.breakPoint > ota_control_env.totalImageSize)
                    {
                        ota_upgradeLog_destroy();  // Error in log, we'd better try to retransmit the entire image.
                        //ota_control_send_result_response(OTA_RESULT_ERR_BREAKPOINT);
                        errOtaCode = OTA_RESULT_ERR_BREAKPOINT;
                        ota_tws_send_handle(OTA_RSP_CONFIG, (uint8_t *)&errOtaCode, 1, IBRT_OTA_TWS_OTA_CONFIG_CMD);
                        return;
                    }
                    ota_control_env.offsetInFlashToProgram = new_image_copy_flash_offset;
                    ota_control_env.offsetInFlashOfCurrentSegment = ota_control_env.offsetInFlashToProgram;
                    ota_control_env.dstFlashOffsetForNewImage = ota_control_env.offsetInFlashOfCurrentSegment;

                    if(ota_control_env.resume_at_breakpoint == true)
                    {
                        ota_control_env.alreadyReceivedDataSizeOfImage = ota_control_env.breakPoint;
                        ota_control_env.offsetOfImageOfCurrentSegment = ota_control_env.alreadyReceivedDataSizeOfImage;
                        ota_control_env.offsetInFlashOfCurrentSegment = ota_control_env.dstFlashOffsetForNewImage + ota_control_env.offsetOfImageOfCurrentSegment;
                        ota_control_env.offsetInFlashToProgram = ota_control_env.offsetInFlashOfCurrentSegment;
                    }
                    LOG_DBG(0,"OTA config pass.");

                    LOG_DBG(1,"Start writing the received image to flash offset 0x%x", ota_control_env.offsetInFlashToProgram);

                    FLASH_OTA_BOOT_INFO_T otaBootInfo = {NORMAL_BOOT,0,0};
                    app_update_ota_boot_info(&otaBootInfo);
                    LOG_DBG(0,"update NORMAL_BOOT done.");

                    bool ota_config = 1;
                    errOtaCode = 1;
                    ota_tws_send_handle(OTA_RSP_CONFIG, (uint8_t *)&ota_config, 1, IBRT_OTA_TWS_OTA_CONFIG_CMD);
                }
                else
                {
                    LOG_DBG(0,"OTA config failed.");
                    bool ota_config = 0;
                    ota_tws_send_handle(OTA_RSP_CONFIG, (uint8_t *)&ota_config, 1, IBRT_OTA_TWS_OTA_CONFIG_CMD);
                }
            }
            break;
        }
        case OTA_COMMAND_GET_OTA_RESULT:
        {
            // check whether all image data have been received
            if (ota_control_env.alreadyReceivedDataSizeOfImage == ota_control_env.totalImageSize)
            {
                LOG_DBG(0,"The final image programming and crc32 check.");

                // flush the remaining data to flash
                if(0 != ota_control_env.offsetInDataBufferForBurning)
                {
                    ota_control_flush_data_to_flash(ota_control_env.dataBufferForBurning,
                                                    ota_control_env.offsetInDataBufferForBurning,
                                                    ota_control_env.offsetInFlashToProgram);
                }

                bool Ret  =  ota_check_image_data_sanity_crc();
                if(Ret)
                {
                    // update the magic code of the application image
                    app_update_magic_number_of_app_image(NORMAL_BOOT);
                }
                else
                {
                #if defined(__VBUS_SUP_OLD__)
					app_update_magic_number_of_app_image(NORMAL_BOOT);
                #else
                    TRACE(0,"data sanity crc failed.");
                    errOtaCode = Ret;
                    ota_tws_send_handle(OTA_RSP_RESULT, (uint8_t *)&Ret, 1, IBRT_OTA_TWS_IMAGE_CRC_CMD);
                    ota_control_reset_env();
                    return;
				#endif
                }

                // check the crc32 of the image
                bool ret = ota_control_check_image_crc();
                if (ret)
                {
                    LOG_DBG(0,"Whole image verification pass.");
                    ota_control_env.isPendingForReboot = true;
                    errOtaCode = ret;
                    ota_tws_send_handle(OTA_RSP_RESULT, (uint8_t *)&ret, 1, IBRT_OTA_TWS_IMAGE_CRC_CMD);
                }
                else
                {
                    Bes_exit_ota_state();
                    errOtaCode = ret;
                    ota_tws_send_handle(OTA_RSP_RESULT, (uint8_t *)&ret, 1, IBRT_OTA_TWS_IMAGE_CRC_CMD);
                    LOG_DBG(0,"Whole image verification failed.");
                    ota_control_reset_env();

                }

                ota_upgradeLog_destroy();
            }
            else
            {
                bool whole_image_crc = 0;
                ota_tws_send_handle(OTA_RSP_RESULT, (uint8_t *)&whole_image_crc, 1, IBRT_OTA_TWS_IMAGE_CRC_CMD);
            }
            break;
        }
        case OTA_READ_FLASH_CONTENT:
        {
            ota_control_sending_flash_content((OTA_READ_FLASH_CONTENT_REQ_T*)otaBuf);
            break;
        }
        case OTA_COMMAND_GET_VERSION:
        {
            OTA_GET_VERSION_REQ_T* ptStart = (OTA_GET_VERSION_REQ_T *)(otaBuf);
            hal_norflash_disable_protection(HAL_NORFLASH_ID_0);
            ota_tws_get_master();
            if (BES_OTA_START_MAGIC_CODE == ptStart->magicCode)
            {
                 uint8_t fw_rev[4];
                 system_get_info(&fw_rev[0], &fw_rev[1], &fw_rev[2], &fw_rev[3]);
                 ota_tws_send_handle(OTA_RSP_VERSION, fw_rev, sizeof(fw_rev)/sizeof(fw_rev[0]), IBRT_OTA_TWS_GET_VERSION_CMD);
            }
            break;
        }
        case OTA_COMMAND_SIDE_SELECTION:
        {
            uint8_t selectSide[2] = {0x91 , 0x01};
            ota_tws_send_handle(OTA_RSP_SIDE_SELECTION, selectSide, sizeof(selectSide)/sizeof(selectSide[0]), IBRT_OTA_TWS_SELECT_SIDE_CMD);
            break;
        }
        case OTA_COMMAND_IMAGE_APPLY:
        {
            Bes_exit_ota_state();
            OTA_IMAGE_APPLY_REQ_T* ptStart = (OTA_IMAGE_APPLY_REQ_T *)(otaBuf);
            if (BES_OTA_START_MAGIC_CODE == ptStart->magicCode && ota_control_env.isPendingForReboot)
            {
                ota.permissionToApply = 1;
                bool overWrite = 1;
                errOtaCode = 1;
                ota_tws_send_handle(OTA_RSP_IMAGE_APPLY, (uint8_t *)&overWrite, 1, IBRT_OTA_TWS_IMAGE_OVERWRITE_CMD);
            }
            else
            {
                bool overWrite = 0;
                ota_tws_send_handle(OTA_RSP_IMAGE_APPLY, (uint8_t *)&overWrite, 1, IBRT_OTA_TWS_IMAGE_OVERWRITE_CMD);
                ota.permissionToApply = 0;
            }
            break;
        }
        case OTA_RSP_START_ROLE_SWITCH:
        {
            uint8_t ota_connected = ota_control_get_datapath_type();
            TRACE(2,"%s ota_connected %d", __func__, ota_connected);

            if ((ota_connected == DATA_PATH_BLE) && (p_ibrt_ctrl->current_role == IBRT_MASTER))
            {
                // disconnect ble connection if existing
                #ifdef BLE_ENABLE
                uint16_t ble_Conhandle = 0;
                ble_Conhandle = app_ota_get_conidx();
                bt_drv_reg_op_ble_sup_timeout_set(ble_Conhandle, 15); //fix ble disconnection takes long time:connSupervisionTime=150ms
                app_ble_disconnect_all();
                #endif
            }
            else if((ota_connected == DATA_PATH_SPP) && (p_ibrt_ctrl->current_role == IBRT_MASTER))
            {
                LOG_DBG(0,"OTA_RSP_START_ROLE_SWITCH result %d",otaBuf[1]);
                if ((otaBuf[1] == 1)&&(app_check_is_ota_role_switch_initiator()))
                {
                    osTimerStop(ota_role_switch_req_timer_id);
                    DelayMs(200); // wait for sending rfcomm credit
                    app_ibrt_if_tws_switch_prepare_done_in_bt_thread(IBRT_ROLE_SWITCH_USER_OTA,0);
                }
            }
            break;
        }
        case OTA_GET_LOCAL_IRK_REQ:
        {
            ota_control_send_get_irk_response();
            break;
        }
        default:
            break;
    }
}

void ota_ibrt_handle_received_data(uint8_t *otaBuf, bool isViaBle, uint16_t len)
{
    TRACE(2,"[%s],case = 0x%x", __func__, otaBuf[0]);
    static uint8_t stop_receive = 0;
    if(otaBuf[0] == OTA_COMMAND_IMAGE_APPLY)
    {
        stop_receive = 1;
    }
    else if(stop_receive == 1)
    {
        stop_receive = 0;
        return;
    }

    ota_bes_handle_received_data(otaBuf, isViaBle, len);
    tws_ctrl_send_cmd(IBRT_OTA_TWS_IMAGE_BUFF, otaBuf, len);
}

static void app_update_magic_number_of_app_image(uint32_t newMagicNumber)
{

    uint32_t startFlashAddr = OTA_FLASH_LOGIC_ADDR +
            ota_control_env.dstFlashOffsetForNewImage;

    memcpy(ota_control_env.dataBufferForBurning, (uint8_t *)startFlashAddr,
        FLASH_SECTOR_SIZE_IN_BYTES);


    *(uint32_t *)ota_control_env.dataBufferForBurning = newMagicNumber;

    BesOtaErase(ota_control_env.dstFlashOffsetForNewImage);
    BesOtaProgram(ota_control_env.dstFlashOffsetForNewImage,
        ota_control_env.dataBufferForBurning, FLASH_SECTOR_SIZE_IN_BYTES);

    BesFlushPendingFlashOp(NORFLASH_API_ALL);
}

uint32_t app_get_magic_number(void)
{
    // Workaround for reboot: controller in standard SPI mode while FLASH in QUAD mode
    // First read will fail when FLASH in QUAD mode, but it will make FLASH roll back to standard SPI mode
    // Second read will succeed

    volatile uint32_t *magic;

    magic = (volatile uint32_t *)&otaBootInfoInFlash;

    // First read (and also flush the controller prefetch buffer)
    *(magic + 0x400);

    return *magic;
}

static void app_update_ota_boot_info(FLASH_OTA_BOOT_INFO_T* otaBootInfo)
{
    BesOtaErase( OTA_INFO_IN_OTA_BOOT_SEC);
    BesOtaProgram(OTA_INFO_IN_OTA_BOOT_SEC, (uint8_t*)otaBootInfo,
        sizeof(FLASH_OTA_BOOT_INFO_T));

    BesFlushPendingFlashOp(NORFLASH_API_ALL);
}

static void ota_control_opera_callback(void* param)
{
    NORFLASH_API_OPERA_RESULT *opera_result;

    opera_result = (NORFLASH_API_OPERA_RESULT*)param;

    LOG_DBG(5,"%s:type = %d, addr = 0x%x,len = 0x%x,result = %d.",
                __func__,
                opera_result->type,
                opera_result->addr,
                opera_result->len,
                opera_result->result);
}

void ota_flash_init(void)
{
    LOG_DBG(1,"[%s].",__func__);
#ifdef __APP_USER_DATA_NV_FLASH_OFFSET__
    user_data_nv_flash_offset = __APP_USER_DATA_NV_FLASH_OFFSET__;
#else
    user_data_nv_flash_offset = hal_norflash_get_flash_total_size(HAL_NORFLASH_ID_0) - 2*4096;
#endif

    enum NORFLASH_API_RET_T ret;
    uint32_t block_size = 0;
    uint32_t sector_size = 0;
    uint32_t page_size = 0;

    hal_norflash_get_size(HAL_NORFLASH_ID_0,NULL,&block_size,&sector_size,&page_size);
    ret = norflash_api_register(NORFLASH_API_MODULE_ID_OTA,
                    HAL_NORFLASH_ID_0,
                    0x0,
                    (OTA_FLASH_LOGIC_ADDR + user_data_nv_flash_offset), // include ota_upgradeLog  and OTA new_imag_offset
                    block_size,
                    sector_size,
                    page_size,
                    OTA_CONTROL_NORFLASH_BUFFER_LEN,
                    ota_control_opera_callback);
    ASSERT(ret == NORFLASH_API_OK, "ota_control_init: norflash_api register failed,ret = %d.",ret);
    hal_norflash_suspend_check_irq(HAL_NORFLASH_ID_0,AUDMA_IRQn);
    hal_norflash_suspend_check_irq(HAL_NORFLASH_ID_0,ISDATA_IRQn);
    hal_norflash_suspend_check_irq(HAL_NORFLASH_ID_0,ISDATA1_IRQn);
    new_image_copy_flash_offset = NEW_IMAGE_FLASH_OFFSET;
}


uint8_t app_get_bes_ota_state(void)
{
    return isInBesOtaState;
}

bool app_check_is_ota_role_switch_initiator(void)
{
    return ota_role_switch_initiator;
}

void app_set_ota_role_switch_initiator(bool is_initiate)
{
    ota_role_switch_initiator = is_initiate;
}

void app_statistics_get_ota_pkt_in_role_switch(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if ((p_ibrt_ctrl->master_tws_switch_pending == true)&& \
        (p_ibrt_ctrl->current_role == IBRT_MASTER) && \
        (ota_control_get_datapath_type() == DATA_PATH_SPP))
    {
        TRACE(0,"GET PKT IN OTA");
        ota_get_pkt_in_role_switch = true;
    }
}

bool app_check_user_can_role_switch_in_ota(void)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    return((app_get_bes_ota_state())&&\
           ((p_ibrt_ctrl->master_tws_switch_pending != false) || \
            (p_ibrt_ctrl->slave_tws_switch_pending != false))&& \
           (ota_role_switch_flag));
}

void app_send_ota_pkt_sync_ack_response(bool ret)
{
#if OTA_DATA_PKT_SYNC_MS
    ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();

    if(ret == false)
    {
        if((p_ibrt_ctrl->current_role == IBRT_MASTER) && \
           (ota_pkt_sync_cache == OTA_DATA_ACK))
        {
            TRACE(0,"[OTA]send ota pkt sync response");
            ota_control_send_data_ack_response();
            ota_pkt_sync_cache = 0x00;
            return;
        }

        TRACE(0,"[OTA]send ota pkt sync response");
        ota_tws_send_handle(OTA_DATA_ACK, NULL, 0, IBRT_OTA_TWS_ACK_CMD);
    }
    else
    {
        TRACE(1,"[OTA]ibrt_ota_cmd_type %d",ibrt_ota_cmd_type);
        if(ibrt_ota_cmd_type == OTA_DATA_ACK)
        {
            ota_control_send_data_ack_response();
            ota_pkt_sync_cache = 0x00;
            ibrt_ota_cmd_type = 0;
        }
        else
        {
            /*master do not finish write data into flash*/
            ota_pkt_sync_cache = OTA_DATA_ACK;
        }
   }
#endif
}
