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
#include "cmsis.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_internal.h"
#include "bt_drv_2300a_internal.h"
#include "bt_drv_interface.h"
#include "bt_drv.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_iomux.h"
#include <string.h>
#include "CrashCatcher.h"
#include "bt_drv_internal.h"

#ifdef __CLK_GATE_DISABLE__
#define BT_DRV_REG_OP_CLK_ENB()
#define BT_DRV_REG_OP_CLK_DIB()
#else
static uint32_t reg_op_clk_enb = 0;
#define BT_DRV_REG_OP_CLK_ENB()    do{\
                                        if (!reg_op_clk_enb){ \
                                            hal_cmu_bt_sys_clock_force_on(); \
                                            BTDIGITAL_REG(0xD0330024) |= (1<<18); \
                                            BTDIGITAL_REG(0xD0330024)&=(~(1<<5)); \
                                        } \
                                        reg_op_clk_enb++;

#define BT_DRV_REG_OP_CLK_DIB()     reg_op_clk_enb--; \
                                    if (!reg_op_clk_enb){ \
                                        BTDIGITAL_REG(0xD0330024) &= ~(1<<18); \
                                        BTDIGITAL_REG(0xD0330024)|=(1<<5); \
                                        hal_cmu_bt_sys_clock_auto(); \
                                    } \
                                    }while(0);
#endif

#define GAIN_TBL_SIZE           0x0F

//define for DEBUG
#define EM_BT_CLKOFF0_MASK   ((uint16_t)0x0000FFFF)
#define ASSERT_ERR(cond)                             { if (!(cond)) { BT_DRV_TRACE(2,"line is %d file is %s", __LINE__, __FILE__); } }
#define abs(x)  (x>0?x:(-x))

#define BT_DRIVER_GET_U8_REG_VAL(regAddr)       (*(uint8_t *)(regAddr))
#define BT_DRIVER_GET_U16_REG_VAL(regAddr)      (*(uint16_t *)(regAddr))
#define BT_DRIVER_GET_U32_REG_VAL(regAddr)      (*(uint32_t *)(regAddr))

#define BT_DRIVER_PUT_U8_REG_VAL(regAddr, val)      *(uint8_t *)(regAddr) = (val)
#define BT_DRIVER_PUT_U16_REG_VAL(regAddr, val)     *(uint16_t *)(regAddr) = (val)
#define BT_DRIVER_PUT_U32_REG_VAL(regAddr, val)     *(uint32_t *)(regAddr) = (val)

static uint32_t bt_ram_start_addr=0;
static uint32_t hci_fc_env_addr=0;
static uint32_t ld_acl_env_addr=0;
static uint32_t bt_util_buf_env_addr=0;
static uint32_t em_buf_env_addr=0;
static uint32_t ld_ibrt_env_addr=0;
static uint32_t dbg_state_addr=0;
static uint32_t lc_state_addr=0;
static uint32_t task_message_buffer_addr=0;
static uint32_t lmp_message_buffer_addr=0;
static uint32_t ld_sco_env_addr=0;
static uint32_t rx_monitor_addr=0;
static uint32_t lc_env_addr=0;
static uint32_t dbg_bt_setting_addr=0;
static uint32_t lm_nb_sync_active_addr=0;
static uint32_t lm_env_addr=0;
static uint32_t pending_seq_error_link_addr=0;
static uint32_t hci_env_addr=0;
static uint32_t lc_sco_env_addr=0;
static uint32_t llm_le_env_addr=0;
static uint32_t ld_env_addr=0;
static uint32_t rwip_env_addr=0;
static uint32_t MIC_BUFF_addr=0;
static uint32_t g_mem_dump_ctrl_addr=0;
static uint32_t ble_rx_monitor_addr=0;
static uint32_t reconnecting_flag_addr=0;
static uint32_t link_connect_bak_addr=0;
static uint32_t llc_env_addr=0;
static uint32_t dbg_bt_setting_ext1_addr=0;
static uint32_t rwip_rf_addr=0;
static uint32_t ld_acl_metrics_addr = 0;
static uint32_t rf_rx_hwgain_tbl_addr = 0;
static uint32_t rf_hwagc_rssi_correct_tbl_addr = 0;
//static uint32_t curr_cw_addr = 0;
static uint32_t rf_rx_gain_fixed_tbl_addr = 0;
static uint32_t ebq_test_setting_addr = 0;
static uint32_t host_ref_clk_addr = 0;
//static uint32_t cmd_filter_addr = 0;
//static uint32_t evt_filter_addr = 0;
static uint32_t hci_dbg_bt_setting_ext2_addr = 0;
static uint32_t m_dbg_log_tl_env_addr = 0;
static uint32_t dbg_log_mem_env_addr = 0;
void bt_drv_reg_op_global_symbols_init(void)
{
    bt_ram_start_addr = 0xc0000000;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0)
    {
        hci_fc_env_addr = 0xc0008bf0;
        ld_acl_env_addr = 0xc0007074;
        bt_util_buf_env_addr = 0xc00063b8;
        em_buf_env_addr = 0xc0008948;
        dbg_state_addr = 0xc00059c0;
        lc_state_addr = 0xc0006848;
        ld_sco_env_addr = 0xc0007088;
        rx_monitor_addr = 0xc00060cc;
        lc_env_addr = 0xc000683c;
        dbg_bt_setting_addr = 0xc00059cc;
        lm_nb_sync_active_addr = 0xc0006836;
        lm_env_addr = 0xc00065e4;
        hci_env_addr = 0xc0008b40;
        lc_sco_env_addr = 0xc0006818;
        llm_le_env_addr = 0xc00087c8;
        ld_env_addr = 0xc0006900;
        rwip_env_addr = 0xc00058e0;
        MIC_BUFF_addr = 0xc0006184;
        g_mem_dump_ctrl_addr = 0xc0005f84;
        ble_rx_monitor_addr = 0xc000606c;
        llc_env_addr = 0xc00088d0;
        dbg_bt_setting_ext1_addr = 0xc0005a9c;
        rwip_rf_addr = 0xc00006f4;
        ld_acl_metrics_addr = 0xc0006e60;
        rf_rx_hwgain_tbl_addr =  0xc0000484;
        rf_hwagc_rssi_correct_tbl_addr = 0xc0000404;
        ld_ibrt_env_addr = 0xc0008528;
        rf_rx_gain_fixed_tbl_addr = 0xc0006060;
        ebq_test_setting_addr = 0xc0005ab8;
        host_ref_clk_addr = 0xc0000608;
        hci_dbg_bt_setting_ext2_addr = 0xc0005a40;
        m_dbg_log_tl_env_addr = 0xc0005f28;
        dbg_log_mem_env_addr = 0xc0005b08;
    }
}
void bt_drv_reg_op_set_accept_new_mobile_enable(void)
{
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_0)
    {
        extern uint8_t bt_setting_2300a[];
        if (bt_setting_2300a[97] == 0)
        {
            uint8_t *p_new_accept_enable_addr = (uint8_t*)(dbg_bt_setting_addr + 0x67); 
            BT_DRV_TRACE(0,"BT_DRV_CONFIG:set_new_mobile enable");
            bt_setting_2300a[97] = 1;
            *p_new_accept_enable_addr = 1;
        }
    }
}
void bt_drv_reg_op_clear_accept_new_mobile_enable(void)
{
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_0)
    {
        extern uint8_t bt_setting_2300a[];
        if (bt_setting_2300a[97] == 1)
        {
            uint8_t *p_new_accept_enable_addr = (uint8_t*)(dbg_bt_setting_addr + 0x67); 
            BT_DRV_TRACE(0,"BT_DRV_CONFIG:clear_new_mobile enable");
            bt_setting_2300a[97] = 0;
            *p_new_accept_enable_addr = 0;
        }
    }
}
uint32_t bt_drv_reg_op_get_host_ref_clk(void)
{
    uint32_t ret = 0;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    ret = *(uint32_t *)host_ref_clk_addr;
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return ret;
}
void bt_drv_reg_op_set_controller_log_buffer(void)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t *p_write_addr1 = (uint32_t*)(dbg_log_mem_env_addr+0x200);//5d08
    uint32_t *p_write_addr2 = (uint32_t*)(dbg_log_mem_env_addr+0x410);//5f18

    uint32_t *p_start_addr1 = (uint32_t*)(dbg_log_mem_env_addr+0x204);//5d0c
    uint32_t *p_start_addr2 = (uint32_t*)(dbg_log_mem_env_addr+0x414);//5f1c

    uint16_t *p_max_len1 = (uint16_t*)(dbg_log_mem_env_addr+0x208);//5d10
    uint16_t *p_max_len2 = (uint16_t*)(dbg_log_mem_env_addr+0x418);//5f20

    *p_write_addr1 = 0xC0009C90;
    *p_start_addr1 = 0xC0009C90;

    *p_write_addr2 = (0xC0009C90+0x400);
    *p_start_addr2 = (0xC0009C90+0x400);

    *p_max_len1 = 0x3e8;
    *p_max_len2 = 0x3e8;
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}
extern "C" void hal_iomux_set_p16_controller_log(void);
extern "C" void hal_iomux_set_p20_p21_controller_log(void);
void bt_drv_reg_op_config_controller_log(uint32_t log_mask, uint16_t* p_cmd_filter, uint8_t cmd_nb, uint8_t* p_evt_filter, uint8_t evt_nb)
{
#ifdef BT_UART_LOG_P16
    hal_iomux_set_p16_controller_log();
#else
    hal_iomux_set_p20_p21_controller_log();
#endif
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint16_t cmd_filter_buf[CMD_FILTER_LEN];
    uint8_t evt_filter_buf[EVT_FILTER_LEN];
    memset((void*)cmd_filter_buf, 0xff, CMD_FILTER_LEN*2);
    memset((void*)evt_filter_buf, 0xff, EVT_FILTER_LEN);
    for(uint32_t i = 0; i < cmd_nb; i++)
    {
        cmd_filter_buf[i] = *p_cmd_filter++;
    }
    for(uint32_t j = 0; j < evt_nb; j++)
    {
        evt_filter_buf[j] = *p_evt_filter++;
    }
    bt_drv_reg_op_set_controller_log_buffer();
    bt_drv_reg_op_config_cotroller_cmd_filter(cmd_filter_buf);
    bt_drv_reg_op_config_cotroller_evt_filter(evt_filter_buf);
    bt_drv_reg_op_set_controller_trace_curr_sw(log_mask);

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_set_controller_trace_curr_sw(uint32_t log_mask)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t* p_curr_sw = (uint32_t*)(m_dbg_log_tl_env_addr + 4);
    if(p_curr_sw != NULL)
    {
        *p_curr_sw = log_mask;
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_config_cotroller_cmd_filter(uint16_t* p_cmd_filter)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t* p_cmd_filter_addr = (uint32_t*)(m_dbg_log_tl_env_addr + 0x3a);
    if(p_cmd_filter != NULL)
    {
        memcpy((void*)p_cmd_filter_addr, (void*)p_cmd_filter, CMD_FILTER_LEN*sizeof(uint16_t));
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_config_cotroller_evt_filter(uint8_t* p_evt_filter)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t* p_evt_filter_addr = (uint32_t*)(m_dbg_log_tl_env_addr + 0x4e);
    if(p_evt_filter != NULL)
    {
        memcpy((void*)p_evt_filter_addr, (void*)p_evt_filter, EVT_FILTER_LEN);
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}



#if (LL_MONITOR == 1)

void bt_drv_reg_op_ll_monitor(uint16_t connhdl, uint8_t metric_type, uint32_t* ret_val)
{
    BT_DRV_REG_OP_CLK_ENB();
    uint8_t link_id = btdrv_conhdl_to_linkid(connhdl);
    uint32_t ld_acl_metrics_ptr = 0;
    uint32_t metric_off = 12 + metric_type*4;
    if(ld_acl_metrics_addr != 0)
    {
        ld_acl_metrics_ptr = ld_acl_metrics_addr + link_id * 156;
    }
    *ret_val = *(uint32_t*)(ld_acl_metrics_ptr + metric_off);
    BT_DRV_REG_OP_CLK_DIB();
}

#endif


void bt_drv_reg_op_set_rf_rx_hwgain_tbl(int8_t (*p_tbl)[3])
{
    BT_DRV_REG_OP_CLK_ENB();
    int i, j ;
    int8_t (*p_tbl_addr)[3] = (int8_t (*)[3])rf_rx_hwgain_tbl_addr;

    if(p_tbl != NULL)
    {
        for(i = 0; i < GAIN_TBL_SIZE; i++)
        {
            for(j = 0; j < 3; j++)
            {
                p_tbl_addr[i][j] = p_tbl[i][j];
            }
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_set_rf_hwagc_rssi_correct_tbl(int8_t* p_tbl)
{
    BT_DRV_REG_OP_CLK_ENB();
    int i;
    int8_t* p_tbl_addr = (int8_t*)rf_hwagc_rssi_correct_tbl_addr;
    if(p_tbl != NULL)
    {
        for(i = 0; i < 16; i++)
        {
            p_tbl_addr[i] = p_tbl[i];
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}


#if (defined(ACL_DATA_CRC_TEST))
uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t len)
{
    crc = ~crc;
    for (size_t i = 0; i < len; i++)
    {
        crc = crc ^ data[i];

        for (uint8_t j = 0; j < 8; j++)
        {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return (~crc);
}

uint32_t crc32_ieee(const uint8_t *data, size_t len)
{
    uint32_t crc = 0;
    BT_DRV_REG_OP_CLK_ENB();
    crc = crc32_ieee_update(0x0, data, len);
    BT_DRV_REG_OP_CLK_DIB();
    return crc;
}
#endif

void bt_drv_reg_op_ecc_softbit_process(uint16_t conn_handle, uint16_t length, uint8_t *data)
{
#if 0
    struct hci_acl_data_pkt* p_dat = (struct hci_acl_data_pkt*)data;
    if(p_dat != NULL)
    {
        if(p_dat->ecc_flg == 1)
        {
            p_dat->ecc_flg = 0;
            BT_DRV_TRACE(0, "acl ecc flg");

#if (defined(ACL_DATA_CRC_TEST) && (ACL_DATA_CRC_TEST == 1))

            uint32_t calc_crc32 = 0;
            uint32_t raw_crc32 = 0;
            calc_crc32 = crc32_ieee(p_dat->sdu_data + 8, p_dat->data_load_len - 8);
            memcpy(&calc_crc32, p_dat->sdu_data + 4, 4);
            if(calc_crc32 != raw_crc32)
            {
                BT_DRV_TRACE(0, "ecc correct fail\n");
                DUMP8("%02x ", p_dat, p_dat->data_load_len+2);
            }
            else
            {
                BT_DRV_TRACE(0, "ecc correct success");
            }
#endif
        }
        if(p_dat->softbit_flg == 1)
        {
            p_dat->softbit_flg = 0;
            BT_DRV_TRACE(0, "acl softbit flg");
        }
    }
#endif
}
void bt_drv_reg_op_hci_vender_ibrt_ll_monitor(uint8_t* ptr, uint16_t* p_sum_err,uint16_t* p_rx_total)
{
    const char *monitor_str[35] =
    {
        "TX DM1",
        "TX DH1",
        "TX DM3",
        "TX DH3",
        "TX DM5",
        "TX DH5",
        "TX 2DH1",
        "TX 3DH1",
        "TX 2DH3",
        "TX 3DH3",
        "TX 2DH5",
        "TX 3DH5",

        "RX DM1",
        "RX DH1",
        "RX DM3",
        "RX DH3",
        "RX DM5",
        "RX DH5",
        "RX 2DH1",
        "RX 3DH1",
        "RX 2DH3",
        "RX 3DH3",
        "RX 2DH5",
        "RX 3DH5",
        "hec error",
        "crc error",
        "fec error",
        "gard error",
        "ecc count",

        "radio_count",
        "sleep_duration_count",
        "radio_tx_succss_count",
        "radio_tx_count",
        "softbit_success_count",
        "softbit_count",
    };

    uint8_t *p = ( uint8_t * )ptr;
    uint32_t sum_err = 0;
    uint32_t rx_data_sum = 0;
    uint32_t val;

    BT_DRV_TRACE(0,"ibrt_ui_log:ll_monitor");

    for (uint8_t i = 0; i < 35; i++)
    {
        val = co_read32p(p);
        if (val)
        {
            if(i>= 12 && i<=23){
                rx_data_sum += val;
            }

            if((i > 23) && (i < 29))
            {
                sum_err += val;
            }
            BT_DRV_TRACE(2,"%s %d", monitor_str[i], val);
        }
        p+=4;
    }
    *p_sum_err = sum_err;
    *p_rx_total = rx_data_sum;
}


void bt_drv_reg_op_rssi_set(uint16_t rssi)
{
}

void bt_drv_reg_op_scan_intv_set(uint32_t scan_intv)
{
}

void bt_drv_reg_op_encryptchange_errcode_reset(uint16_t hci_handle)
{

}

void bt_drv_reg_op_sco_sniffer_checker(void)
{
}

void bt_drv_reg_op_trigger_time_checker(void)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    BT_DRV_TRACE(1,"BT_REG_OP:0xd02201f0 = %x",*(volatile uint32_t *)0xd02201f0);
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}
void bt_drv_reg_op_tws_output_power_fix_separate(uint16_t hci_handle, uint16_t pwr)
{
}

///BD Address structure
struct bd_addr
{
    ///6-byte array address value
    uint8_t  addr[6];
};
///device info structure
struct ld_device_info
{
    struct bd_addr bd_addr;
    uint8_t link_id;
    uint8_t state;
};


bool bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_get(void)
{
    return false;
}

void bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_set(bool state)
{
}

void bt_drv_reg_op_ibrt_env_reset(void)
{
}

int bt_drv_reg_op_currentfreeaclbuf_get(void)
{
    int nRet = 0;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    //ACL packert number of host set - ACL number of controller has been send to host
    //hci_fc_env.host_set.acl_pkt_nb - hci_fc_env.cntr.acl_pkt_sent
    if(hci_fc_env_addr != 0)
    {
        nRet = (*(volatile uint16_t *)(hci_fc_env_addr+0x4) - *(volatile uint16_t *)(hci_fc_env_addr+0xc));//acl_pkt_nb - acl_pkt_sent
    }
    else
    {
        nRet = 0;
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return nRet;
}

//static uint16_t mobile_sync_info;
static uint16_t mobile_sync_info;
void bt_drv_reg_op_save_mobile_airpath_info(uint16_t hciHandle)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);

    if (btdrv_is_link_index_valid(link_id))
    {
        mobile_sync_info = BT_DRIVER_GET_U16_REG_VAL(EM_BT_BTADDR1_ADDR + BT_EM_SIZE*link_id);
        BT_DRV_TRACE(1,"BT_REG_OP: save xfer mobile sync info 0x%x ", mobile_sync_info);
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_block_xfer_with_mobile(uint16_t hciHandle)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);

    if (btdrv_is_link_index_valid(link_id))
    {
        BT_DRIVER_PUT_U16_REG_VAL((EM_BT_BTADDR1_ADDR + BT_EM_SIZE*link_id), mobile_sync_info^0x8000);
        BT_DRV_TRACE(2,"BT_REG_OP: block xfer with mobile, save hci handle 0x%x sync info as 0x%x", hciHandle, mobile_sync_info^0x8000);
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_resume_xfer_with_mobile(uint16_t hciHandle)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);

    if (btdrv_is_link_index_valid(link_id))
    {
        BT_DRIVER_PUT_U16_REG_VAL((EM_BT_BTADDR1_ADDR + BT_EM_SIZE*link_id), mobile_sync_info);
        BT_DRV_TRACE(2,"BT_REG_OP:resume xfer with mobile, hci handle 0x%x as 0x%x", hciHandle, mobile_sync_info);
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_block_fast_ack_with_mobile(void)
{
    /*
     * ibrt master set its fast ack sync bit wrong,let itself NACK mobile link always
     */

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    bt_drv_set_fa_invert_enable(BT_FA_INVERT_DISABLE);

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_resume_fast_ack_with_mobile(void)
{

    /*
     *  old master recovery its fast ack sync bit right
     */

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    bt_drv_set_fa_invert_enable(BT_FA_INVERT_EN);

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

int bt_drv_reg_op_packet_type_checker(uint16_t hciHandle)
{
    return 0;
}

void bt_drv_reg_op_max_slot_setting_checker(uint16_t hciHandle)
{
}

void bt_drv_reg_op_force_task_dbg_idle(void)
{
}

void bt_drv_reg_op_afh_follow_mobile_mobileidx_set(uint16_t hciHandle)
{
}

void bt_drv_reg_op_afh_follow_mobile_twsidx_set(uint16_t hciHandle)
{
}

void bt_drv_reg_op_afh_bak_reset(void)
{
}

void bt_drv_reg_op_afh_bak_save(uint8_t role, uint16_t mobileHciHandle)
{
}

void bt_drv_reg_op_connection_checker(void)
{
}

void bt_drv_reg_op_sco_status_store(void)
{
}

void bt_drv_reg_op_sco_status_restore(void)
{
}

void bt_drv_reg_op_afh_set_default(void)
{
}

void bt_drv_reg_op_ld_sniffer_master_addr_set(uint8_t * addr)
{
}

void bt_drv_reg_op_lsto_hack(uint16_t hciHandle, uint16_t lsto)
{
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t acl_par_ptr = 0;
    //ld_acl_env

    if(ld_acl_env_addr)
    {
        acl_par_ptr = *(uint32_t *)(ld_acl_env_addr+(hciHandle-0x80)*4);
    }

    if(acl_par_ptr)
    {
        //lsto off:0x9a
        BT_DRV_TRACE(3,"BT_REG_OP:Set the lsto for hciHandle=0x%x, from:0x%x to 0x%x",
                     hciHandle,*(uint16_t *)(acl_par_ptr+0x9a),lsto);

        *(uint16_t *)(acl_par_ptr+0x9a) = lsto;
    }
    else
    {
        BT_DRV_TRACE(0,"BT_REG_OP:ERROR,acl par address error");
    }

    BT_DRV_REG_OP_CLK_DIB();
}

uint16_t bt_drv_reg_op_get_lsto(uint16_t hciHandle)
{
    uint32_t acl_par_ptr = 0;
    uint16_t lsto = 0;
    //ld_acl_env
    BT_DRV_REG_OP_CLK_ENB();
    if(ld_acl_env_addr)
    {
        acl_par_ptr = *(uint32_t *)(ld_acl_env_addr+(hciHandle-0x80)*4);
    }


    if(acl_par_ptr)
    {
        //lsto off:0x9a
        lsto = *(uint16_t *)(acl_par_ptr+0x9a) ;
        BT_DRV_TRACE(2,"BT_REG_OP:lsto=0x%x for hciHandle=0x%x",lsto,hciHandle);
    }
    else
    {
        lsto= 0xffff;
        BT_DRV_TRACE(0,"BT_REG_OP:ERROR,acl par null ptr");
    }

    BT_DRV_REG_OP_CLK_DIB();

    return lsto;
}

uint8_t bt_drv_reg_opmsbc_find_tx_sync(uint8_t *buff)
{
    uint8_t i;
    for(i=0; i<60; i++)
    {
        if(buff[i]==0x1 && buff[(i+2)%60] == 0xad)
        {
            //    BT_DRV_TRACE(1,"MSBC tx sync find =%d",i);
            return i;
        }
    }
    BT_DRV_TRACE(0,"BT_REG_OP:TX No pACKET");

    return 0;
}

bool bt_drv_reg_op_sco_tx_buf_restore(uint8_t *trigger_test)
{
    uint8_t offset;
    bool nRet = false;

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    offset = bt_drv_reg_opmsbc_find_tx_sync((uint8_t *)EM_BT_AUDIOBUF_OFF);
    if(offset !=0)
    {
#ifndef APB_PCM
        *trigger_test = (((BTDIGITAL_REG(0xd022045c) & 0x3f)) +(60-offset))%64;
#endif
        BT_DRV_TRACE(2,"BT_REG_OP:TX BUF ERROR trigger_test=%p,offset=%x", trigger_test,offset);
        DUMP8("%02x ",(uint8_t *)EM_BT_AUDIOBUF_OFF,10);
//   BT_DRV_TRACE(2,"pcm reg=%x %x",*(uint32_t *)0xd0220468,*(uint32_t *)0x400000f0);
        nRet = true;
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();

    return nRet;
}

int bt_drv_reg_op_acl_chnmap(uint16_t hciHandle, uint8_t *chnmap, uint8_t chnmap_len)
{
    int nRet  = 0;
    uint32_t acl_evt_ptr = 0;
    uint8_t *chnmap_ptr = 0;
    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);

    BT_DRV_REG_OP_CLK_ENB();

    if (!btdrv_is_link_index_valid(link_id))
    {
        memset(chnmap, 0, chnmap_len);
        nRet = -1;
        goto exit;
    }
    if (chnmap_len < 10)
    {
        memset(chnmap, 0, chnmap_len);
        nRet = -1;
        goto exit;
    }


    if(ld_acl_env_addr)
    {
        acl_evt_ptr = *(volatile uint32_t *)(ld_acl_env_addr+link_id*4);
    }


    if(acl_evt_ptr != 0)
    {
        //afh_map off:0x34
        chnmap_ptr = (uint8_t *)(acl_evt_ptr+0x34);
    }

    if (!chnmap_ptr)
    {
        memset(chnmap, 0, chnmap_len);
        nRet = -1;
        goto exit;
    }
    else
    {
        memcpy(chnmap, chnmap_ptr, chnmap_len);
    }

exit:
    BT_DRV_REG_OP_CLK_DIB();

    return nRet;
}

extern "C" uint32_t hci_current_left_tx_packets_left(void);
extern "C" uint32_t hci_current_left_rx_packets_left(void);
extern "C" uint32_t hci_current_rx_packet_complete(void);
extern "C" uint8_t hci_current_rx_aclfreelist_cnt(void);
void bt_drv_reg_op_bt_info_checker(void)
{
    uint32_t *rx_buf_ptr=NULL;
    uint32_t *tx_buf_ptr=NULL;
    uint8_t rx_free_buf_count=0, i=0;
    uint8_t tx_free_buf_count=0;
    uint16_t em_buf ;

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(bt_util_buf_env_addr)
    {
        //acl_rx_free off:0x14
        //acl_tx_free off:0x28
        rx_buf_ptr = (uint32_t *)(bt_util_buf_env_addr+0x14); //bt_util_buf_env.acl_rx_free
        tx_buf_ptr = (uint32_t *)(bt_util_buf_env_addr+0x28); //bt_util_buf_env.acl_tx_free
    }

    while(rx_buf_ptr && *rx_buf_ptr)
    {
        rx_free_buf_count++;
        rx_buf_ptr = (uint32_t *)(*rx_buf_ptr);
    }

    uint8_t em_buf_count = 0;
    for(i=0; i<4; i++)
    {
        em_buf = BT_DRIVER_GET_U16_REG_VAL(EM_BT_RXACLBUFPTR_ADDR + i*REG_EM_BT_RXDESC_SIZE);
        if(em_buf != 0)
            em_buf_count++;
    }
    BT_DRV_TRACE(4,"BT_REG_OP: rxbuff = %x,embuff =%x , host rx buff in controller = %x,%x", \
                 rx_free_buf_count, \
                 em_buf_count, \
                 hci_current_left_rx_packets_left(), \
                 bt_drv_reg_op_currentfreeaclbuf_get());

    //check tx buff
    while(tx_buf_ptr && *tx_buf_ptr)
    {
        tx_free_buf_count++;
        tx_buf_ptr = (uint32_t *)(*tx_buf_ptr);
    }

    BT_DRV_TRACE(2,"BT_REG_OP:txbuff = %x,acl tx host  in controller = %x ", \
                 tx_free_buf_count, \
                 hci_current_left_tx_packets_left());

    bt_drv_reg_op_controller_state_checker();

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

uint32_t *rxbuff;
uint16_t *send_count;
uint16_t *free_count;
uint16_t *bt_send_count;
void bt_drv_reg_op_ble_buffer_cleanup(void)
{
}
struct ke_timer
{
    /// next ke timer
    struct ke_timer *next;
    /// message identifier
    uint16_t     id;
    /// task identifier
    uint16_t    task;
    /// time value
    uint32_t    time;
};

struct co_list_hdr
{
    /// Pointer to next co_list_hdr
    struct co_list_hdr *next;
};

/// structure of a list
struct co_list_con
{
    /// pointer to first element of the list
    struct co_list_hdr *first;
    /// pointer to the last element
    struct co_list_hdr *last;

    /// number of element in the list
    uint32_t cnt;
    /// max number of element in the list
    uint32_t maxcnt;
    /// min number of element in the list
    uint32_t mincnt;
};

struct mblock_free
{
    /// Next free block pointer
    struct mblock_free* next;
    /// Previous free block pointer
    struct mblock_free* previous;
    /// Size of the current free block (including delimiter)
    uint16_t free_size;
    /// Used to check if memory block has been corrupted or not
    uint16_t corrupt_check;
};
bool bt_drv_reg_op_get_dbg_state(void)
{
    bool nRet = false;
    BT_DRV_REG_OP_CLK_ENB();
    //dbg_state
    if(dbg_state_addr)
    {
        nRet = (*(uint8_t *)dbg_state_addr);
    }
    else
    {
        nRet = false;
    }
    BT_DRV_REG_OP_CLK_DIB();
    return nRet;
}

uint8_t bt_drv_reg_op_get_controller_tx_free_buffer(void)
{
    uint32_t *tx_buf_ptr=NULL;
    uint8_t tx_free_buf_count = 0;

    BT_DRV_REG_OP_CLK_ENB();

    if(bt_util_buf_env_addr)
    {
        //acl_tx_free off:0x28
        tx_buf_ptr = (uint32_t *)(bt_util_buf_env_addr+0x28); //bt_util_buf_env.acl_tx_free
    }
    else
    {
        BT_DRV_TRACE(1, "BT_REG_OP:please fix %s", __func__);
        tx_free_buf_count = 0;
        goto exit;
    }

    //check tx buff
    while(tx_buf_ptr && *tx_buf_ptr)
    {
        tx_free_buf_count++;
        tx_buf_ptr = (uint32_t *)(*tx_buf_ptr);
    }

exit:
    BT_DRV_REG_OP_CLK_DIB();

    return tx_free_buf_count;
}

void bt_drv_reg_op_controller_state_checker(void)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(lc_state_addr != 0)
    {
        BT_DRV_TRACE(1,"BT_REG_OP: LC_STATE=0x%x",*(uint32_t *)lc_state_addr);
    }

#ifdef BT_UART_LOG
    BT_DRV_TRACE(1,"current controller clk=%x", bt_drv_reg_op_get_host_ref_clk());
#endif

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_tx_flow_checker(void)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint8_t link_id = 0;
    uint32_t acl_par_ptr = 0;
    for(link_id=0; link_id<3; link_id++)
    {
        if(ld_acl_env_addr)
        {
            acl_par_ptr = *(volatile uint32_t *)(ld_acl_env_addr+link_id*4);
        }

        if (acl_par_ptr)
        {
            //tx_flow
            BT_DRV_TRACE(2,"linkid =%d, tx flow=%d",link_id, *(uint8_t *)(acl_par_ptr+0xc2));
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

uint8_t bt_drv_reg_op_force_get_lc_state(uint16_t conhdl)
{
    uint8_t state = 0;

    BT_DRV_REG_OP_CLK_ENB();
    if(lc_state_addr != 0)
    {
        BT_DRV_TRACE(1,"BT_REG_OP: read LC_STATE=0x%x",*(uint32_t *)lc_state_addr);
        uint8_t idx = btdrv_conhdl_to_linkid(conhdl);

        if (btdrv_is_link_index_valid(idx))
        {
            uint8_t *lc_state = (uint8_t *)lc_state_addr;
            state = lc_state[idx];
        }
    }
    BT_DRV_REG_OP_CLK_DIB();

    return state;
}
void bt_drv_reg_op_force_set_lc_state(uint16_t conhdl, uint8_t state)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(lc_state_addr != 0)
    {
        BT_DRV_TRACE(1,"BT_REG_OP: read LC_STATE=0x%x",*(uint32_t *)lc_state_addr);
        uint8_t idx = btdrv_conhdl_to_linkid(conhdl);

        if (btdrv_is_link_index_valid(idx))
        {
            uint8_t * lc_state = (uint8_t *)lc_state_addr;
            lc_state[idx] = state;
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_crash_dump(void)
{
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t *bt_dump_mem_start = (uint8_t*)bt_ram_start_addr;
    uint32_t bt_dump_len_max   = 0x10000;

    //uint8_t *mcu_dump_mem_start = (uint8_t*)0x20000000;
    //uint32_t mcu_dump_len_max   = RAM_SIZE;

    //uint8_t *em_dump_area_1_start = (uint8_t*)0xd0220000;
    //uint32_t em_area_1_len_max    = 0x04A8;

    uint8_t *em_dump_area_2_start = (uint8_t*)0xd0210000;
    uint32_t em_area_2_len_max    = 0x8000;

    BT_DRV_TRACE(1,"BT_REG_OP:BT 2300A(1303): metal id=%d", hal_get_chip_metal_id());
    //first move R3 to R9, lost R9
    BT_DRV_TRACE(1,"BT controller BusFault_Handler:\nREG:[LR] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE));
    BT_DRV_TRACE(1,"REG:[R0] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +160));
    BT_DRV_TRACE(1,"REG:[R1] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +164));
    BT_DRV_TRACE(1,"REG:[R2] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +168));
    BT_DRV_TRACE(1,"REG:[R3] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +172));
    BT_DRV_TRACE(1,"REG:[R4] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +4));
    BT_DRV_TRACE(1,"REG:[R5] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +8));
    BT_DRV_TRACE(1,"REG:[R6] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +12));
    BT_DRV_TRACE(1,"REG:[R7] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +16));
    BT_DRV_TRACE(1,"REG:[R8] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +20));
    hal_sys_timer_delay(MS_TO_TICKS(100));

    //BT_DRV_TRACE(1,"REG:[R9] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +24));
    BT_DRV_TRACE(1,"REG:[sl] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +28));
    BT_DRV_TRACE(1,"REG:[fp] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +32));
    BT_DRV_TRACE(1,"REG:[ip] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +36));
    BT_DRV_TRACE(1,"REG:[SP,#0] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +40));
    BT_DRV_TRACE(1,"REG:[SP,#4] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +44));
    BT_DRV_TRACE(1,"REG:[SP,#8] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +48));
    BT_DRV_TRACE(1,"REG:[SP,#12] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +52));
    BT_DRV_TRACE(1,"REG:[SP,#16] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +56));
    BT_DRV_TRACE(1,"REG:[SP,#20] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +60));
    BT_DRV_TRACE(1,"REG:[SP,#24] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +64));
    BT_DRV_TRACE(1,"REG:[SP,#28] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +68));
    hal_sys_timer_delay(MS_TO_TICKS(100));

    BT_DRV_TRACE(1,"REG:[SP,#32] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +72));
    BT_DRV_TRACE(1,"REG:[SP,#36] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +76));
    BT_DRV_TRACE(1,"REG:[SP,#40] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +80));
    BT_DRV_TRACE(1,"REG:[SP,#44] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +84));
    BT_DRV_TRACE(1,"REG:[SP,#48] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +88));
    BT_DRV_TRACE(1,"REG:[SP,#52] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +92));
    BT_DRV_TRACE(1,"REG:[SP,#56] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +96));
    BT_DRV_TRACE(1,"REG:[SP,#60] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +100));
    BT_DRV_TRACE(1,"REG:[SP,#64] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +104));
    BT_DRV_TRACE(1,"REG:SP = 0x%08x",  BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +108));
    BT_DRV_TRACE(1,"REG:MSP = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +112));
    BT_DRV_TRACE(1,"REG:PSP = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +116));
    BT_DRV_TRACE(1,"REG:CFSR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +120));
    BT_DRV_TRACE(1,"REG:BFAR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +124));
    BT_DRV_TRACE(1,"REG:HFSR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +128));
    BT_DRV_TRACE(1,"REG:ICSR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +132));
    BT_DRV_TRACE(1,"REG:AIRCR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +136));
    BT_DRV_TRACE(1,"REG:SCR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +140));
    BT_DRV_TRACE(1,"REG:CCR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +144));
    BT_DRV_TRACE(1,"REG:SHCSR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +148));
    BT_DRV_TRACE(1,"REG:AFSR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +152));
    BT_DRV_TRACE(1,"REG:MMFAR = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +156));
    hal_sys_timer_delay(MS_TO_TICKS(100));
    //task_message_buffer
    uint32_t buff_addr = 0;

    if(task_message_buffer_addr)
    {
        buff_addr = task_message_buffer_addr;
    }

    BT_DRV_TRACE(2,"0xd0330050: 0x%x, 54:0x%x",*(uint32_t *)0xd0330050,*(uint32_t *)0xd0330054);
    BT_DRV_TRACE(2,"0x400000a0: 0x%x, a4:0x%x",*(uint32_t *)0x400000a0,*(uint32_t *)0x400000a4);

    if(g_mem_dump_ctrl_addr)
    {
        BT_DRV_TRACE(1,"LMP addr=0x%x",*(uint32_t *)(g_mem_dump_ctrl_addr+4));
        BT_DRV_TRACE(1,"STA addr=0x%x",*(uint32_t *)(g_mem_dump_ctrl_addr+0x10));
        BT_DRV_TRACE(1,"MSG addr=0x%x",*(uint32_t *)(g_mem_dump_ctrl_addr+0x1c));
        BT_DRV_TRACE(1,"SCH addr=0x%x",*(uint32_t *)(g_mem_dump_ctrl_addr+0x28));
        BT_DRV_TRACE(1,"ISR addr=0x%x",*(uint32_t *)(g_mem_dump_ctrl_addr+0x34));
    }

    BT_DRV_TRACE(0,"task msg buff:");
    if(buff_addr != 0)
    {
        for(uint8_t j=0; j<5; j++)
        {
            DUMP8("%02x ", (uint8_t *)(buff_addr+j*20), 20);
        }
    }
    hal_sys_timer_delay(MS_TO_TICKS(100));

    BT_DRV_TRACE(0," ");
    BT_DRV_TRACE(0,"lmp buff:");
    //lmp_message_buffer

    if(lmp_message_buffer_addr)
    {
        buff_addr = lmp_message_buffer_addr;
    }

    if(buff_addr != 0)
    {
        for(uint8_t j=0; j<10; j++)
        {
            DUMP8("%02x ",(uint8_t *)(buff_addr+j*20), 20);
        }
    }
    hal_sys_timer_delay(MS_TO_TICKS(100));

    uint8_t link_id = 0;
    uint32_t evt_ptr = 0;
    uint32_t acl_par_ptr = 0;
    for(link_id=0; link_id<3; link_id++)
    {
        BT_DRV_TRACE(1,"acl_par: link id %d",link_id);

        if(ld_acl_env_addr)
        {
            evt_ptr = *(uint32_t *)(ld_acl_env_addr+link_id*4);
        }


        if (evt_ptr)
        {
            acl_par_ptr = evt_ptr;
            BT_DRV_TRACE(5,"acl_par: acl_par_ptr 0x%x, clk off 0x%x, bit off 0x%x, last sync clk off 0x%x, last sync bit off 0x%x",
                         acl_par_ptr, *(uint32_t *)(acl_par_ptr+140),*(uint16_t *)(acl_par_ptr+150),
                         *(uint32_t *)(acl_par_ptr+136),((*(uint32_t *)(acl_par_ptr+150))&0xFFFF0000)>>16);
        }
    }
    hal_sys_timer_delay(MS_TO_TICKS(100));

    //ld_sco_env
    evt_ptr = 0;

    if(ld_sco_env_addr)
    {
        evt_ptr = *(uint32_t *)ld_sco_env_addr;
    }

    if(evt_ptr != 0)
    {
        BT_DRV_TRACE(1,"esco linkid :%d",*(uint8_t *)(evt_ptr+70));
        for(link_id=0; link_id<3; link_id++)
        {
            BT_DRV_TRACE(2,"bt_linkcntl_linklbl 0x%x: link id %d",*(uint16_t *)(EM_BT_LINKCNTL_ADDR+link_id*BT_EM_SIZE),link_id);
            BT_DRV_TRACE(1,"rxcount :%x",*(uint16_t *)(EM_BT_RXDESCCNT_ADDR+link_id*BT_EM_SIZE));
        }
    }
    btdrv_dump_mem(bt_dump_mem_start, bt_dump_len_max, BT_SUB_SYS_TYPE);
    //btdrv_dump_mem(em_dump_area_1_start, em_area_1_len_max, BT_EM_AREA_1_TYPE);
    btdrv_dump_mem(em_dump_area_2_start, em_area_2_len_max, BT_EM_AREA_2_TYPE);
    //btdrv_dump_mem(mcu_dump_mem_start, mcu_dump_len_max, MCU_SYS_TYPE);

    BT_DRV_REG_OP_CLK_DIB();
}


uint8_t bt_drv_reg_op_get_tx_pwr(uint16_t connHandle)
{
    uint8_t idx;
    uint16_t localVal;
    uint8_t tx_pwr = 0;

    BT_DRV_REG_OP_CLK_ENB();
    idx = btdrv_conhdl_to_linkid(connHandle);

    if (btdrv_is_link_index_valid(idx))
    {
        localVal = BT_DRIVER_GET_U16_REG_VAL(EM_BT_PWRCNTL_ADDR + idx * BT_EM_SIZE);
        tx_pwr = ((localVal & ((uint16_t)0x000000FF)) >> 0);
    }

    BT_DRV_REG_OP_CLK_DIB();
    return tx_pwr;
}

void bt_drv_reg_op_set_tx_pwr(uint16_t connHandle, uint8_t txpwr)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t idx = btdrv_conhdl_to_linkid(connHandle);

    if (btdrv_is_link_index_valid(idx))
    {
        BT_DRIVER_PUT_U16_REG_VAL(EM_BT_PWRCNTL_ADDR + idx * BT_EM_SIZE,
                                  (BT_DRIVER_GET_U16_REG_VAL(EM_BT_PWRCNTL_ADDR + idx * BT_EM_SIZE) & ~((uint16_t)0x000000FF)) | ((uint16_t)txpwr << 0));
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_fix_tx_pwr(uint16_t connHandle)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    bt_drv_reg_op_set_tx_pwr(connHandle, LBRT_TX_PWR_FIX);
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_set_accessible_mode(uint8_t mode)
{
}

void bt_drv_reg_op_set_swagc_mode(uint8_t mode)
{
}

void bt_drv_reg_op_set_reboot_pairing_mode(uint8_t mode)
{
}

void bt_drv_reg_op_force_sco_retrans(bool enable)
{
#ifdef __FORCE_SCO_MAX_RETX__
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    if (enable)
    {
        BTDIGITAL_REG_SET_FIELD(0xd0220468,3,24,3);
    }
    else
    {
        BTDIGITAL_REG_SET_FIELD(0xd0220468,3,24,0);
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
#endif
}

void bt_drv_reg_op_enable_pcm_tx_hw_cal(void)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    *(volatile uint32_t *)0xd0220468 |= 1<<22;
    *(volatile uint32_t *)0x400000f0 |= 1;
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_monitor_clk(void)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t env0 =0;
    uint32_t env1 =0;
    uint32_t env2 =0;

    if(ld_acl_env_addr)
    {
        env0 = *(uint32_t *)ld_acl_env_addr;
        env1 = *(uint32_t *)(ld_acl_env_addr+4);
        env1 = *(uint32_t *)(ld_acl_env_addr+8);
    }

    if(env0 & bt_ram_start_addr)
    {
        //clk_off off:0x8c
        env0 +=0x8c;
        BT_DRV_TRACE(2,"BT_REG_OP:env0 clk off=%x %x",*(uint32_t *)env0,*(uint16_t *)EM_BT_CLKOFF0_ADDR | (*(uint16_t *)EM_BT_CLKOFF1_ADDR <<16));
    }
    if(env1 & bt_ram_start_addr)
    {
        env1 +=0x8c;
        BT_DRV_TRACE(2,"env1 clk off=%x %x",*(uint32_t *)env1,*(uint16_t *)(EM_BT_CLKOFF0_ADDR+BT_EM_SIZE) | (*(uint16_t *)(EM_BT_CLKOFF1_ADDR+BT_EM_SIZE) <<16));
    }
    if(env2 & bt_ram_start_addr)
    {
        env2 +=0x8c;
        BT_DRV_TRACE(2,"env2 clk off=%x %x",*(uint32_t *)env2,*(uint16_t *)(EM_BT_CLKOFF0_ADDR+BT_EM_SIZE*2)| (*(uint16_t *)(EM_BT_CLKOFF1_ADDR+BT_EM_SIZE*2) <<16));
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

bool bt_drv_reg_op_read_rssi_in_dbm(uint16_t connHandle,rx_agc_t* rx_val)
{
#ifdef BT_RSSI_MONITOR
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if (connHandle == 0xFFFF)
    {
        goto exit;
    }

    if(hal_get_chip_metal_id()>=HAL_CHIP_METAL_ID_0)
    {
        uint8_t idx = btdrv_conhdl_to_linkid(connHandle);
        /// Accumulated RSSI (to compute an average value)
        //  int16_t rssi_acc = 0;
        /// Counter of received packets used in RSSI average
        //     uint8_t rssi_avg_cnt = 1;
        rx_agc_t * rx_monitor_env = NULL;
        uint32_t acl_par_ptr = 0;
        uint32_t rssi_record_ptr=0;

        if (!btdrv_is_link_index_valid(idx))
        {
            goto exit;
        }

        if(ld_acl_env_addr)
        {
            acl_par_ptr = *(uint32_t *)(ld_acl_env_addr+idx*4);
            if(acl_par_ptr)
                rssi_record_ptr = acl_par_ptr+0xb0;
        }

        //rx_monitor
        if(rx_monitor_addr)
        {
            rx_monitor_env = (rx_agc_t*)rx_monitor_addr;
        }

        if(rssi_record_ptr != 0)
        {
            // BT_DRV_TRACE(1,"addr=%x,rssi=%d",rssi_record_ptr,*(int8 *)rssi_record_ptr);
#ifdef __NEW_SWAGC_MODE__
            rx_val->rssi = *(int8_t *)rssi_record_ptr;
#else
            rx_val->rssi = rx_monitor_env[idx].rssi;
#endif
            rx_val->rxgain = rx_monitor_env[idx].rxgain;
        }
    }

exit:
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return true;
#else
    return false;
#endif
}

bool bt_drv_reg_op_read_ble_rssi_in_dbm(uint16_t connHandle,rx_agc_t* rx_val)
{
#ifdef BT_RSSI_MONITOR
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if (connHandle == 0xFFFF)
    {
        goto exit;
    }

    if(hal_get_chip_metal_id()>=HAL_CHIP_METAL_ID_0)
    {
        uint8_t idx = connHandle;
        /// Accumulated RSSI (to compute an average value)
        int16_t rssi_acc = 0;
        /// Counter of received packets used in RSSI average
        uint8_t rssi_avg_cnt = 1;
        rx_agc_t * rx_monitor_env = NULL;
        if(idx > 2)
        {
            goto exit;
        }

        //rx_monitor

        if(ble_rx_monitor_addr)
        {
            rx_monitor_env = (rx_agc_t*)ble_rx_monitor_addr;
        }

        if(rx_monitor_env != NULL)
        {
            for(int i=0; i< rssi_avg_cnt; i++)
            {
                rssi_acc += rx_monitor_env[idx].rssi;
            }
            rx_val->rssi = rssi_acc / rssi_avg_cnt;
            rx_val->rxgain = rx_monitor_env[idx].rxgain;
        }
    }
exit:
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return true;
#else
    return false;
#endif
}

void bt_drv_reg_op_acl_silence(uint16_t connHandle, uint8_t silence)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t idx = btdrv_conhdl_to_linkid(connHandle);

    if (btdrv_is_link_index_valid(idx))
    {
        BT_DRIVER_PUT_U16_REG_VAL(EM_BT_BT_EXT1_ADDR + idx * BT_EM_SIZE,
                                  (BT_DRIVER_GET_U16_REG_VAL(EM_BT_BT_EXT1_ADDR + idx * BT_EM_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)silence << 15));
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_call_monitor(uint16_t connHandle, uint8_t tws_role)
{
}

void bt_drv_reg_op_lock_sniffer_sco_resync(void)
{

}

void bt_drv_reg_op_unlock_sniffer_sco_resync(void)
{

}

void bt_drv_reg_op_ibrt_retx_att_nb_set(uint8_t retx_nb)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    int ret = -1;
    uint32_t sco_evt_ptr = 0x0;
    // TODO: [ld_sco_env address] based on CHIP id

    if(ld_sco_env_addr)
    {
        sco_evt_ptr = *(volatile uint32_t *)ld_sco_env_addr;
        ret = 0;
    }

    if(ret == 0)
    {
        uint32_t retx_ptr=0x0;
        if(sco_evt_ptr !=0)
        {
            //offsetof(struct ea_elt_tag, env) + sizeof(struct ld_sco_evt_params)
            //0x1c+0x28
            retx_ptr =sco_evt_ptr+0x44;
        }
        else
        {
            BT_DRV_TRACE(0,"BT_REG_OP:Error, ld_sco_env[0].evt ==NULL");
            ret = -2;
        }

        if(ret == 0)
        {
            *(volatile uint8_t *)retx_ptr = retx_nb;
        }
    }

    BT_DRV_TRACE(3,"BT_REG_OP:%s,ret=%d,retx nb=%d",__func__,ret,retx_nb);

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_acl_tx_type_set(uint16_t hciHandle, uint8_t slot_sel)
{
    uint32_t lc_ptr=0;
    uint32_t acl_par_ptr = 0;
    uint32_t packet_type_addr = 0;
    //lc_env and ld_acl_env

    BT_DRV_REG_OP_CLK_ENB();

    if(lc_env_addr)
    {
        lc_ptr = *(uint32_t *)(lc_env_addr+(hciHandle-0x80)*4);
    }

    if(ld_acl_env_addr)
    {
        acl_par_ptr = *(uint32_t *)(ld_acl_env_addr+(hciHandle-0x80)*4);
    }

    //sanity check
    if(lc_ptr == 0)
    {
        goto exit;
    }
    else
    {
        //    off=b0 rssi_rcord
        //        b1 br_type
        //        b2 edr_type
        packet_type_addr = (uint32_t)(acl_par_ptr+0xb2);
        BT_DRV_TRACE(3,"BT_REG_OP:%s   hdl=0x%x packet_types=0x%x",__func__,hciHandle, (*(uint8_t *)(packet_type_addr)));
        BT_DRV_TRACE(2,"BT_REG_OP:lc_ptr 0x%x, acl_par_ptr 0x%x",lc_ptr,acl_par_ptr);
        uint16_t edr_type = 0;
        if(slot_sel == USE_1_SLOT_PACKET)
        {
#if defined(__3M_PACK__)
            edr_type = (1 << DM1_IDX) | (1 << DH1_2_IDX) | (1 << DH1_3_IDX);
#else
            edr_type = (1 << DM1_IDX) | (1 << DH1_2_IDX);
#endif
        }
        else if(slot_sel == USE_3_SLOT_PACKET)
        {
#if defined(__3M_PACK__)
            edr_type = (1 << DM1_IDX) | (1 << DH1_2_IDX) | (1 << DH1_3_IDX) | \
                       (1 << DH3_2_IDX) |(1 << DH3_3_IDX);
#else
            edr_type = (1 << DM1_IDX) | (1 << DH1_2_IDX) | (1 << DH3_2_IDX);
#endif
        }
        else if(slot_sel == USE_5_SLOT_PACKET)
        {
#if defined(__3M_PACK__)
            edr_type = (1 << DM1_IDX) | (1 << DH1_2_IDX) | (1 << DH1_3_IDX) | \
                       (1 << DH3_2_IDX) |(1 << DH3_3_IDX) |(1 << DH5_2_IDX) | (1 << DH5_3_IDX);
#else
            edr_type = (1 << DM1_IDX) | (1 << DH1_2_IDX) | (1 << DH3_2_IDX)|(1 << DH5_2_IDX);
#endif
        }

        *(uint8_t *)(packet_type_addr) = edr_type;

        BT_DRV_TRACE(1,"BT_REG_OP:After op,packet_types 0x%x",*(uint8_t *)(packet_type_addr) );
    }

exit:
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_acl_tx_type_trace(uint16_t hciHandle)
{
    uint32_t acl_par_ptr = 0;
    uint32_t packet_type_addr = 0;
    //ld_acl_env

    BT_DRV_REG_OP_CLK_ENB();

    if(ld_acl_env_addr)
    {
        acl_par_ptr = *(uint32_t *)(ld_acl_env_addr+(hciHandle-0x80)*4);
    }

    //sanity check
    if(acl_par_ptr == 0)
    {
        goto exit;
    }
    else
    {
        //    off=b0 rssi_rcord
        //        b1 br_type
        //        b2 edr_type
        packet_type_addr = (uint32_t)(acl_par_ptr+0xb0);
        uint16_t type = ((*(uint32_t *)(packet_type_addr))>>8) & 0xffff;
        BT_DRV_TRACE(1,"BT_REG_OP:tx packet_types=0x%x",type);
    }

exit:
    BT_DRV_REG_OP_CLK_DIB();
}

uint8_t bt_drv_reg_op_acl_tx_type_get(uint16_t hciHandle, uint8_t* br_type, uint8_t* edr_type)
{
    uint8_t status = 0xff;
    uint32_t lc_ptr=0;
    uint32_t acl_par_ptr = 0;
    uint32_t packet_type_addr = 0;

    BT_DRV_REG_OP_CLK_ENB();


    if(lc_env_addr)
    {
        lc_ptr = *(uint32_t *)(lc_env_addr+(hciHandle-0x80)*4);
    }

    if(ld_acl_env_addr)
    {
        acl_par_ptr = *(uint32_t *)(ld_acl_env_addr+(hciHandle-0x80)*4);
    }


    //sanity check
    if(lc_ptr == 0)
    {
        BT_DRV_TRACE(2,"BT_REG_OP:%s hdl=0x%x,lc_env_ptr = NULL",__func__,hciHandle);
    }
    else
    {
        packet_type_addr = (uint32_t)(acl_par_ptr+0xb0);
        uint32_t packet_type = *(uint32_t *)(packet_type_addr);
        //off=b0 rssi_rcord
        //    b1 br_type
        //    b2 edr_type
        if(br_type)
        {
            *br_type = (packet_type>>8)&0xff;
        }
        if(edr_type)
        {
            *br_type = (packet_type>>16)&0xff;
        }
        status = 0;
        BT_DRV_TRACE(3,"BT_REG_OP:%s hdl=0x%x packet_types=0x%x",__func__,hciHandle, packet_type);
    }

    BT_DRV_REG_OP_CLK_DIB();
    return status;
}

uint16_t em_bt_bitoff_getf(int elt_idx)
{
    uint16_t localVal = 0;

    BT_DRV_REG_OP_CLK_ENB();
    localVal = BTDIGITAL_BT_EM(EM_BT_BITOFF_ADDR + elt_idx * BT_EM_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000003FF)) == 0);
    BT_DRV_REG_OP_CLK_DIB();

    return (localVal >> 0);
}

void em_bt_bitoff_setf(int elt_idx, uint16_t bitoff)
{
    BT_DRV_REG_OP_CLK_ENB();
    ASSERT_ERR((((uint16_t)bitoff << 0) & ~((uint16_t)0x000003FF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_BITOFF_ADDR + elt_idx * BT_EM_SIZE, (uint16_t)bitoff << 0);
    BT_DRV_REG_OP_CLK_DIB();
}

void em_bt_clkoff0_setf(int elt_idx, uint16_t clkoff0)
{
    BT_DRV_REG_OP_CLK_ENB();
    ASSERT_ERR((((uint16_t)clkoff0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_CLKOFF0_ADDR + elt_idx * BT_EM_SIZE, (uint16_t)clkoff0 << 0);
    BT_DRV_REG_OP_CLK_DIB();
}

uint16_t em_bt_clkoff0_getf(int elt_idx)
{
    uint16_t localVal = 0;

    BT_DRV_REG_OP_CLK_ENB();
    localVal = BTDIGITAL_BT_EM(EM_BT_CLKOFF0_ADDR + elt_idx * BT_EM_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    BT_DRV_REG_OP_CLK_DIB();

    return (localVal >> 0);
}
void em_bt_clkoff1_setf(int elt_idx, uint16_t clkoff1)
{
    ASSERT_ERR((((uint16_t)clkoff1 << 0) & ~((uint16_t)0x000007FF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_CLKOFF1_ADDR + elt_idx * BT_EM_SIZE, (uint16_t)clkoff1 << 0);
}

uint16_t em_bt_clkoff1_getf(int elt_idx)
{
    uint16_t localVal = 0;

    BT_DRV_REG_OP_CLK_ENB();
    localVal = BTDIGITAL_BT_EM(EM_BT_CLKOFF1_ADDR + elt_idx * BT_EM_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000007FF)) == 0);
    BT_DRV_REG_OP_CLK_DIB();

    return (localVal >> 0);
}

void em_bt_wincntl_pack(int elt_idx, uint8_t rxwide, uint16_t rxwinsz)
{
    BT_DRV_REG_OP_CLK_ENB();
    ASSERT_ERR((((uint16_t)rxwide << 15) & ~((uint16_t)0x00008000)) == 0);
    ASSERT_ERR((((uint16_t)rxwinsz << 0) & ~((uint16_t)0x00000FFF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_WINCNTL_ADDR + elt_idx * BT_EM_SIZE,  ((uint16_t)rxwide << 15) | ((uint16_t)rxwinsz << 0));
    BT_DRV_REG_OP_CLK_DIB();
}


void bt_drv_reg_op_update_sniffer_bitoffset(uint16_t mobile_conhdl,uint16_t master_conhdl)
{
}

void bt_drv_reg_op_modify_bitoff_timer(uint16_t time_out)
{
}

void bt_drv_reg_op_cs_monitor(void)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t addr;
    addr = BT_EM_ADDR_BASE+0x1e;
    BT_DRV_TRACE(0,"BT_REG_OP:AFH 0:");
    DUMP8("%02x ",(uint8_t *)addr,10);
    addr = BT_EM_ADDR_BASE + 0x1e + 110;
    BT_DRV_TRACE(0,"AFH 1:");
    DUMP8("%02x ",(uint8_t *)addr,10);
    addr = BT_EM_ADDR_BASE + 0x1e + 220;
    BT_DRV_TRACE(0,"AFH 2:");
    DUMP8("%02x ",(uint8_t *)addr,10);
    uint32_t tmp1,tmp2,tmp3;
    tmp1 = BT_EM_ADDR_BASE+0x8;
    tmp2 = BT_EM_ADDR_BASE+0x8+110;
    tmp3 = BT_EM_ADDR_BASE+0x8+220;
    BT_DRV_TRACE(3,"AFH EN:%x %x %x ",*(uint16_t *)tmp1,*(uint16_t *)tmp2,*(uint16_t *)tmp3);
    tmp1 = BT_EM_ADDR_BASE+0x28;
    tmp2 = BT_EM_ADDR_BASE+0x28+110;
    tmp3 = BT_EM_ADDR_BASE+0x28+220;
    BT_DRV_TRACE(3,"AFH ch num:%x %x %x ",*(uint16_t *)tmp1,*(uint16_t *)tmp2,*(uint16_t *)tmp3);

    tmp1 = BT_EM_ADDR_BASE+0x4;
    tmp2 = BT_EM_ADDR_BASE+0x4 + 110;
    tmp3 = BT_EM_ADDR_BASE+0x4 + 220;
    BT_DRV_TRACE(3,"clk off:%x %x %x ",*(uint32_t *)tmp1,*(uint32_t *)tmp2,*(uint32_t *)tmp3);

    tmp1 = BT_EM_ADDR_BASE+0x2;
    tmp2 = BT_EM_ADDR_BASE+0x2+110;
    tmp3 = BT_EM_ADDR_BASE+0x2+220;
    BT_DRV_TRACE(3,"bitoff:%x %x %x ",*(uint16_t *)tmp1,*(uint16_t *)tmp2,*(uint16_t *)tmp3);
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_ble_llm_substate_hacker(void)
{

}

bool  bt_drv_reg_op_check_esco_acl_sniff_conflict(uint16_t hciHandle)
{
    return false;
}

void bt_drv_reg_op_esco_acl_sniff_delay_cal(uint16_t hciHandle,bool enable)
{
}

uint8_t  bt_drv_reg_op_get_role(uint8_t linkid)
{
    uint32_t lc_evt_ptr=0;
    uint32_t role_ptr = 0;
    uint8_t role = 0xff;

    BT_DRV_REG_OP_CLK_ENB();
    if(lc_env_addr)
    {
        lc_evt_ptr = *(volatile uint32_t *)(lc_env_addr+linkid*4);//lc_env
    }

    if(lc_evt_ptr !=0)
    {
        //role off:0x40
        role_ptr = lc_evt_ptr+0x40;
        role = *(uint8_t *)role_ptr;
    }
    else
    {
        BT_DRV_TRACE(1,"BT_REG_OP:ERROR LINKID =%x",linkid);
        role = 0xff;
    }

    BT_DRV_REG_OP_CLK_DIB();

    return role;
}

void bt_drv_reg_op_set_tpoll(uint8_t linkid,uint16_t poll_interval)
{
    uint32_t acl_evt_ptr = 0x0;
    uint32_t poll_addr;

    BT_DRV_REG_OP_CLK_ENB();
    if(ld_acl_env_addr)
    {
        acl_evt_ptr = *(uint32_t *)(ld_acl_env_addr+linkid*4);
    }

    if (acl_evt_ptr != 0)
    {
        //poll_addr = acl_evt_ptr + 0xb8;
        //tpoll off:0x28+92
        poll_addr = acl_evt_ptr + 0xba;
        *(uint16_t *)poll_addr = poll_interval;
    }
    else
    {
        BT_DRV_TRACE(1,"BT_REG_OP:ERROR LINK ID FOR TPOLL %x", linkid);
    }
    BT_DRV_REG_OP_CLK_DIB();
}

int8_t  bt_drv_reg_op_rssi_correction(int8_t rssi)
{
    return rssi;
}

void bt_drv_reg_op_set_music_link(uint8_t link_id)
{
    BT_DRV_REG_OP_CLK_ENB();
    //dbg_bt_setting.music_playing_link
    if(dbg_bt_setting_addr)
    {
        *(uint8_t *)(dbg_bt_setting_addr+0x10) = link_id;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_set_music_link_duration_extra(uint8_t slot)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(dbg_bt_setting_addr)
    {
        *(uint32_t *)(dbg_bt_setting_addr+0x14) = slot*625;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_set_hwagc_read_en(uint8_t en)
{

}

uint32_t bt_drv_reg_op_get_reconnecting_flag()
{
    uint32_t ret = 0;

    BT_DRV_REG_OP_CLK_ENB();
    if(reconnecting_flag_addr)
    {
        ret = *(uint32_t *)reconnecting_flag_addr;
    }
    BT_DRV_REG_OP_CLK_DIB();

    return ret;
}

void bt_drv_reg_op_set_reconnecting_flag()
{
    BT_DRV_REG_OP_CLK_ENB();
    if(reconnecting_flag_addr)
    {
        *(uint32_t *)reconnecting_flag_addr = 1;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_clear_reconnecting_flag()
{
    BT_DRV_REG_OP_CLK_ENB();
    if(reconnecting_flag_addr)
    {
        *(uint32_t *)reconnecting_flag_addr = 0;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_music_link_config(uint16_t active_link,uint8_t active_role,uint16_t inactive_link,uint8_t inactive_role)
{
    BT_DRV_TRACE(4,"BT_REG_OP:bt_drv_reg_op_music_link_config %x %d %x %d",active_link,active_role,inactive_link,inactive_role);
    BT_DRV_REG_OP_CLK_ENB();
    if (active_role == 0) //MASTER
    {
        bt_drv_reg_op_set_tpoll(active_link-0x80, 0x10);
        if (inactive_role == 0)
        {
            bt_drv_reg_op_set_tpoll(inactive_link-0x80, 0x40);
        }
    }
    else
    {
        bt_drv_reg_op_set_music_link(active_link-0x80);
        bt_drv_reg_op_set_music_link_duration_extra(11);
        if (inactive_role == 0)
        {
            bt_drv_reg_op_set_tpoll(inactive_link-0x80, 0x40);
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}

bool bt_drv_reg_op_check_bt_controller_state(void)
{
    bool ret=true;
    if(hal_sysfreq_get() <= HAL_CMU_FREQ_32K)
        return ret;

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if((BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +0)==0x00)
       &&(BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +8)==MIC_BUFF_addr))
        ret = true;
    else
        ret = false;

    if (false == ret)
    {
        BT_DRV_TRACE(0,"controller dead!!!");
        btdrv_trigger_coredump();
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return ret;
}

void bt_drv_reg_op_piconet_clk_offset_get(uint16_t connHandle, int32_t *clock_offset, uint16_t *bit_offset)
{
    uint8_t index = 0;
    uint32_t clock_offset_raw = 0;

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    if (connHandle)
    {
        index = btdrv_conhdl_to_linkid(connHandle);

        if (btdrv_is_link_index_valid(index))
        {
            *bit_offset = em_bt_bitoff_getf(index);
            clock_offset_raw = (em_bt_clkoff1_getf(index) << 16) | em_bt_clkoff0_getf(index);
            *clock_offset = clock_offset_raw;
            *clock_offset = (*clock_offset << 5) >> 5;
        }
        else
        {
            *bit_offset = 0;
            *clock_offset = 0;
        }
    }
    else
    {
        *bit_offset = 0;
        *clock_offset = 0;
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_afh_env_reset(void)
{
}

void bt_drv_reg_op_dma_tc_clkcnt_get(uint32_t *btclk, uint16_t *btcnt)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    *btclk = *((volatile uint32_t*)0xd02201fc);
    *btcnt = *((volatile uint32_t*)0xd02201f8);
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void em_bt_bt_ext1_tx_silence_setf(int elt_idx, uint8_t txsilence)
{
    BT_DRV_REG_OP_CLK_ENB();
    ASSERT_ERR((((uint16_t)txsilence << 15) & ~((uint16_t)0x00008000)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE,
                       (BTDIGITAL_BT_EM(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)txsilence << 15));
    BT_DRV_REG_OP_CLK_DIB();
}

void em_bt_bt_ext1_bw_2m_setf(int elt_idx, uint8_t bw2m)
{
    BT_DRV_REG_OP_CLK_ENB();
    ASSERT_ERR((((uint16_t)bw2m << 11) & ~((uint16_t)0x00000800)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE,
                       (BTDIGITAL_BT_EM(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE) & ~((uint16_t)0x00000800)) | ((uint16_t)bw2m << 11));
    BT_DRV_REG_OP_CLK_DIB();
}

void em_bt_bt_ext1_sco_tx_silence_setf(int elt_idx, uint8_t scotxsilence)
{
    BT_DRV_REG_OP_CLK_ENB();
    ASSERT_ERR((((uint16_t)scotxsilence << 13) & ~((uint16_t)0x00002000)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE,
                       (BTDIGITAL_BT_EM(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE) & ~((uint16_t)0x00002000)) | ((uint16_t)scotxsilence << 13));
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_acl_tx_silence(uint16_t connHandle, uint8_t on)
{
}

void bt_drv_reg_op_acl_tx_silence_clear(uint16_t connHandle)
{
    bt_drv_reg_op_resume_fast_ack_with_mobile();
}

void bt_drv_reg_op_sw_seq_filter(uint16_t connHandle)
{
}

void bt_drv_reg_op_pcm_set(uint8_t en)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(en)
        *(volatile unsigned int *)(0xd02204a8) &= 0xffffffdf;
    else
        *(volatile unsigned int *)(0xd02204a8) |= 1<<5;
    BT_DRV_REG_OP_CLK_DIB();
}

uint8_t bt_drv_reg_op_pcm_get()
{
    uint8_t ret = 1;
    BT_DRV_REG_OP_CLK_ENB();
    ret = (*(volatile unsigned int *)(0xd02204a8) &0x00000020)>>5;
    BT_DRV_REG_OP_CLK_DIB();
    return ~ret;
}
void bt_drv_reg_op_clear_skip_flag()
{
}

void bt_drv_reg_op_clean_flags_of_ble_and_sco(void)
{
}
const uint8_t msbc_mute_patten[]=
{
    0x01,0x38,
    0xad, 0x0,  0x0,  0xc5, 0x0,  0x0, 0x0, 0x0,
    0x77, 0x6d, 0xb6, 0xdd, 0xdb, 0x6d, 0xb7,
    0x76, 0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77,
    0x6d, 0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76,
    0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d,
    0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb,
    0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d, 0xb6,
    0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6c,
    0x00
};
#define SCO_TX_FIFO_BASE (0xd0210000)

void bt_drv_reg_op_sco_txfifo_reset(uint16_t codec_id)
{
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t reg_val = BTDIGITAL_REG(0xd0220148);
    uint32_t reg_offset0, reg_offset1;
    uint16_t *patten_p = (uint16_t *)msbc_mute_patten;

    reg_offset0 = (reg_val & 0xffff);
    reg_offset1 = (reg_val >> 16) & 0xffff;

    for (uint8_t i=0; i<60; i+=2)
    {
        if (codec_id == 2)
        {
            BTDIGITAL_BT_EM(SCO_TX_FIFO_BASE+reg_offset0+i) = *patten_p;
            BTDIGITAL_BT_EM(SCO_TX_FIFO_BASE+reg_offset1+i) = *patten_p;
        }
        else
        {
            BTDIGITAL_BT_EM(SCO_TX_FIFO_BASE+reg_offset0+i) = 0x0;
            BTDIGITAL_BT_EM(SCO_TX_FIFO_BASE+reg_offset1+i) = 0x0;
        }
        patten_p++;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

/*****************************************************************************
 Prototype    : btdrv_set_tws_acl_poll_interval
 Description  : in ibrt mode, set tws acl poll interval
 Input        : uint16_t poll_interval
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2019/4/19
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void btdrv_reg_op_set_private_tws_poll_interval(uint16_t poll_interval, uint16_t poll_interval_in_sco)
{
    BT_DRV_TRACE(2,"BT_REG_OP:Set private tws interval,acl interv=%d,acl interv insco =%d", \
                 poll_interval,poll_interval_in_sco);

    BT_DRV_REG_OP_CLK_ENB();
    if(dbg_bt_setting_addr)
    {
        *(volatile uint16_t *)(dbg_bt_setting_addr+0x40) = poll_interval;
        *(volatile uint16_t *)(dbg_bt_setting_addr+0x42) = poll_interval_in_sco;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void btdrv_reg_op_set_tws_link_duration(uint8_t slot_num)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    //dbg_setting address
    uint32_t op_addr = 0;

    if(dbg_bt_setting_addr)
    {
        op_addr = dbg_bt_setting_addr;
    }

    if(op_addr != 0)
    {
        //uint16_t acl_slot = dbg_setting->acl_slot_in_snoop_mode;
        //acl_slot_in_snoop_mode off:0x44
        uint16_t val = *(volatile uint16_t *)(op_addr+0x44); //acl_slot_in_snoop_mode
        val&=0xff00;
        val|= slot_num;
        *(volatile uint16_t *)(op_addr+0x44) = val;
        BT_DRV_TRACE(1,"BT_REG_OP:Set private tws link duration,val=%d",*((volatile uint16_t*)(op_addr+0x44))&0xff);
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void btdrv_reg_op_enable_private_tws_schedule(bool enable)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t op_addr = 0;

    if(ld_ibrt_env_addr !=0)
    {
        op_addr = ld_ibrt_env_addr;
    }

    if(op_addr!=0)
    {
        //ld_ibrt_env.acl_switch_flag.in_process
        *((volatile uint8_t*)(op_addr+0x70)) = enable;
        BT_DRV_TRACE(1,"BT_REG_OP:Enable private tws function,flag=%d", *((volatile uint8_t*)(op_addr+0x70)));
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_decrease_tx_pwr_when_reconnect(bool enable)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(enable)
    {
        BT_DRV_TRACE(0,"BT_REG_OP:Decrese tx pwr");
        //drease defualt TX pwr
        BTDIGITAL_REG(0xd0350300) = 0x33;
    }
    else
    {
        BT_DRV_TRACE(0,"BT_REG_OP:Increase tx pwr");
        //resume defualt TX pwr
        BTDIGITAL_REG(0xd0350300) = 0x11;
    }
    BT_DRV_REG_OP_CLK_DIB();
}
/*
    rwip_heap_env
    from addr=0xc00027b8 left_length=6K
    rwip_heap_non_ret
    from addr=0xc00052b8 left_length=1.5K
*/
void bt_drv_reg_op_controller_mem_log_config(void)////
{
}

void bt_drv_reg_op_lm_nb_sync_hacker(uint8_t sco_status)
{
}

void bt_drv_reg_op_fastack_status_checker(uint16_t conhdl)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t elt_idx = btdrv_conhdl_to_linkid(conhdl);

    if (btdrv_is_link_index_valid(elt_idx))
    {
        uint32_t fast_cs_addr = EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE;

        BT_DRV_TRACE(3,"BT_DRV_REG: fastack cs=0x%x,fast ack reg=0x%x,Seq filter by pass=0x%x",
                     *(volatile uint16_t *)(fast_cs_addr), //fa rx bit10,tx bit9
                     *(volatile uint32_t *)(BT_FASTACK_ADDR),//fast ack reg bit22
                     *(volatile uint32_t *)(BT_SCO_TRIGGER_BYPASS_ADDR));//seq bypass reg bit 18
    }

    bt_drv_reg_op_bt_info_checker();

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_clear_ibrt_snoop_status(void)
{
}

void bt_drv_reg_op_hwagc_mode_set(uint8_t sync_mode)
{
}

uint8_t bt_drv_reg_op_linkcntl_aclltaddr_getf(uint16_t conhdl)
{
    uint8_t nRet = 0;

    BT_DRV_REG_OP_CLK_ENB();
    uint8_t elt_idx = btdrv_conhdl_to_linkid(conhdl);
    if (btdrv_is_link_index_valid(elt_idx))
    {
        uint16_t localVal = BTDIGITAL_BT_EM(EM_BT_LINKCNTL_ADDR + elt_idx * BT_EM_SIZE);
        nRet = ((localVal & ((uint16_t)0x00000700)) >> 8);
    }
    else
    {
        nRet = 0;
    }
    BT_DRV_REG_OP_CLK_DIB();
    return nRet;
}

uint8_t bt_drv_reg_op_sync_bt_param(uint8_t* param, uint16_t len)
{
    uint8_t status = 0xff;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    //link_connect_bak
    uint32_t op_addr = 0;


    if(link_connect_bak_addr)
    {
        op_addr = link_connect_bak_addr;
    }

    if(op_addr!=0)
    {
        BT_DRV_TRACE(1,"BT_REG_OP:sync bt param len=%d",len);

        memcpy((uint8_t *)(op_addr),param, len);
        status = 0;
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return status;
}

uint32_t bt_drv_reg_op_get_lc_env_ptr(uint16_t conhdl, uint8_t type)
{
    uint32_t lc_ptr = 0;
    uint32_t op_ptr = 0;

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(lc_env_addr)
    {
        lc_ptr = *(uint32_t *)(lc_env_addr+(conhdl-0x80)*4);
    }

    BT_DRV_TRACE(3,"BT_REG_OP:lc_env_ptr =0x%x, conhdl=0x%x,type=%d",lc_ptr,conhdl,type);

    if(lc_ptr == 0)
    {
        goto exit;
    }

    switch(type)
    {
        case LC_LINK_TAG:
            //link tag len=108
            op_ptr = lc_ptr;
            break;
        case LC_INFO_TAG:
            op_ptr = lc_ptr +0x6c;
            break;
        case LC_ENC_TAG:
            //enc tag len=166
            op_ptr = lc_ptr +0x196;
            break;
        case LC_AFH_TAG:
            op_ptr = lc_ptr +0x26c;
            break;
        case LC_SP_TAG:
            //link tag len=108
            op_ptr = lc_ptr +0x28c;
            break;
        case LC_EPR_TAG:
            op_ptr = lc_ptr +0x2f8;
            break;
        case LC_EPC_TAG:
            op_ptr = lc_ptr +0x2fc;
            break;
        default:
            break;
    }

exit:
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();

    return op_ptr;
}

#ifdef __BT_ONE_BRING_TWO__
void bt_drv_reg_op_set_music_ongong_flag()
{
}

void bt_drv_reg_op_clear_music_ongong_flag()
{
}
#endif

uint16_t bt_drv_reg_op_get_ble_conn_latency(uint16_t ble_conhdl)
{
    return 0xffff;
}

bool bt_drv_reg_op_set_ble_conn_latency(uint16_t ble_conhdl, uint16_t conn_latency)
{
    return false;
}

void bt_drv_reg_op_write_private_public_key(uint8_t* private_key,uint8_t* public_key)
{
    uint8_t* lm_env_ptr = 0;
    uint8_t* lm_private_key_ptr = 0;
    uint8_t* lm_public_key_ptr = 0;

    BT_DRV_REG_OP_CLK_ENB();
    //lm_env
    if(lm_env_addr)
    {
        lm_env_ptr = (uint8_t*)lm_env_addr;
    }
    else
    {
        goto exit;
    }
    //priv_key_192 off:6c
    lm_private_key_ptr = lm_env_ptr + 0x6c;
    //pub_key_192 off:0x84
    lm_public_key_ptr = lm_private_key_ptr + 0x18;
    memcpy(lm_private_key_ptr,private_key,24);
    memcpy(lm_public_key_ptr,public_key,48);
    BT_DRV_TRACE(0,"BT_REG_OP:private key");
    DUMP8("%02x",lm_private_key_ptr,24);
    BT_DRV_TRACE(0,"public key");
    DUMP8("%02x",lm_public_key_ptr,48);
exit:
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_for_test_mode_disable(void)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(dbg_bt_setting_addr)
    {
        *(volatile uint32_t *)(dbg_bt_setting_addr+0x64) &= ~(1<<8);////sw_seq_filter_en set 0
        *(volatile uint32_t *)(dbg_bt_setting_addr+0x64) &= ~(1<<0);////ecc_enable set 0
        *(volatile uint32_t *)(dbg_bt_setting_addr+0x1c) &= 0x00ffffff;////dbg_trace_level set 0
    }
    BT_DRV_REG_OP_CLK_DIB();
}

uint16_t bt_drv_reg_op_get_ibrt_sco_hdl(uint16_t acl_hdl)
{
    // FIXME
    return acl_hdl|0x100;
}


void bt_drv_reg_op_get_ibrt_address(uint8_t *addr)
{
    struct ld_device_info * mobile_device_info=0;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(ld_ibrt_env_addr)
    {
        mobile_device_info = (struct ld_device_info *)(ld_ibrt_env_addr+4);
    }

    if(mobile_device_info)
    {
        if (addr != NULL)
        {
            memcpy(addr, (uint8_t *)mobile_device_info->bd_addr.addr, 6);
            BT_DRV_TRACE(0,"BT_REG_OP:get mobile address");
            DUMP8("%02x ",addr,6);
        }
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_set_tws_link_id(uint8_t link_id)
{
    BT_DRV_TRACE(1,"set tws link id =%x",link_id);
    ASSERT(link_id <3 || link_id == 0xff,"BT_REG_OP:error tws link id set");
    uint32_t tws_link_id_addr = 0;

    BT_DRV_REG_OP_CLK_ENB();
    if(ld_ibrt_env_addr != 0)
    {
        tws_link_id_addr = ld_ibrt_env_addr+ 0x74;
    }

    if(tws_link_id_addr != 0)
    {
        if(link_id == 0xff)
        {
            link_id = 3;
        }
        *(uint8_t *)tws_link_id_addr = link_id;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_hack_max_slot(uint8_t link_id,uint8_t slot)
{
    uint32_t acl_evt_ptr = 0x0;
    uint32_t slot_addr;
    uint32_t stop_lantency_addr=0;
    BT_DRV_TRACE(2,"BT_REG_OP:hack slot id=%d,slot=%d",link_id,slot);
    if(link_id>=3)
        return;

    BT_DRV_REG_OP_CLK_ENB();
    if(ld_acl_env_addr)
    {
        acl_evt_ptr = *(uint32_t *)(ld_acl_env_addr+link_id*4);
    }

    if(acl_evt_ptr)
    {
        //rx_max_slot off:9e
        slot_addr = acl_evt_ptr+0xc6;
        stop_lantency_addr = acl_evt_ptr+0x18;//stop_latency1 offset = 0x17, stop_latency2 = 0x18 for 2300a
        *(uint8_t *)slot_addr = slot;
        *(uint8_t *)stop_lantency_addr = slot+3+(*(uint16_t *)(acl_evt_ptr+150)>128?1:0);
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_rx_slot_stop_lantency_checker(void)
{
    uint32_t acl_evt_ptr = 0x0;
    uint32_t slot_addr;
    uint32_t stop_addr1;
    uint32_t stop_addr2;

    BT_DRV_REG_OP_CLK_ENB();
    for(uint8_t i=0; i<3; i++)
    {
        if(ld_acl_env_addr)
        {
            acl_evt_ptr = *(uint32_t *)(ld_acl_env_addr+i*4);
        }

        if(acl_evt_ptr)
        {
            //rx_max_slot off:9e
            slot_addr = acl_evt_ptr+0xc6;
            //stop_latency1 offset = 0x17, stop_latency2 = 0x18 for 2300a
            stop_addr1 = acl_evt_ptr+0x17;
            stop_addr2 = acl_evt_ptr+0x18;
            //clk off:0x28+0x64
            //bit off:0x28+0x6e
            BT_DRV_TRACE(5,"BT_REG_OP:id=%d,rx slot=%d stop1=%d stop2=%d,acl_par: clk off 0x%x, bit off 0x%x",i,*(uint8_t *)slot_addr,
                         *(uint8_t *)stop_addr1, *(uint8_t *)stop_addr2,*(uint32_t *)(acl_evt_ptr+140),*(uint16_t *)(acl_evt_ptr+150));
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}


void bt_drv_reg_op_fa_set(uint8_t en)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(en)
    {
        BT_DRV_TRACE(1,"BT_REG_OP:%s", __func__);
        *(uint32_t *)0xd0220468 |=0x00400000;
    }
    else
    {
        *(uint32_t *)0xd0220468 &= ~0x00400000;
    }
    BT_DRV_REG_OP_CLK_DIB();
}


//////return true means controller had received a data packet
bool bt_drv_reg_op_check_seq_pending_status(void)
{
    bool nRet = true;
    BT_DRV_REG_OP_CLK_ENB();
    if(pending_seq_error_link_addr)
    {
        nRet = (*(uint8_t *)pending_seq_error_link_addr == 3);
    }
    else
    {
        nRet = true;
    }
    BT_DRV_REG_OP_CLK_DIB();
    return nRet;
}

///if the flag is not clear when disconnect happened  call this function
void bt_drv_reg_op_clear_seq_pending_status(void)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(pending_seq_error_link_addr)
    {
        *(uint8_t *)pending_seq_error_link_addr = 3;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_rssi_checker(void)
{
    uint32_t acl_evt_ptr = 0x0;
    uint32_t rssi_acc_addr;
    uint32_t rssi_count_addr;

    BT_DRV_REG_OP_CLK_ENB();

    for(uint8_t i=0; i<3; i++)
    {

        if(ld_acl_env_addr)
        {
            acl_evt_ptr = *(uint32_t *)(ld_acl_env_addr+i*4);
        }

        if(acl_evt_ptr)
        {
            //rssi_acc off:ac
            rssi_acc_addr = acl_evt_ptr+0xac;//ok
            //rssi_avg_cnt off:ae
            rssi_count_addr = acl_evt_ptr+0xae;//ok
            BT_DRV_TRACE(3,"BT_REG_OP:id=%d,rssi acc=%d count=%d",i,*(int16_t *)rssi_acc_addr,*(uint8_t *)rssi_count_addr);
        }

    }
    uint32_t lc_evt_ptr = 0x0;
    uint32_t recv_addr;

    for(uint8_t i=0; i<3; i++)
    {
        if(lc_env_addr)
        {
            lc_evt_ptr = *(volatile uint32_t *)(lc_env_addr+i*4);//lc_env
        }
        if(lc_evt_ptr)
        {
            recv_addr = lc_evt_ptr+0x60;// minPowerRcv ok
            BT_DRV_TRACE(2,"id=%d,recv=%x",i,*(uint32_t *)recv_addr);
        }
    }

    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_set_link_policy(uint8_t linkid, uint8_t policy)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t lc_evt_ptr = 0x0;
    uint32_t policy_addr;
    if(linkid>=0x3)
    {
        goto exit;
    }

    BT_DRV_TRACE(2,"BT_REG_OP: set link=%d, policy=%d",linkid,policy);

    if(lc_env_addr)
    {
        lc_evt_ptr = *(volatile uint32_t *)(lc_env_addr+linkid*4);//lc_env
    }

    if(lc_evt_ptr)
    {
        //LinkPlicySettings off:2c
        policy_addr = lc_evt_ptr+0x2c;
        *(uint8_t *)policy_addr = policy;
    }
exit:
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}


void bt_drv_reg_op_voice_settings_set(uint16_t voice_settings)
{
    uint32_t voice_settings_addr = 0;

    BT_DRV_REG_OP_CLK_ENB();
    if(hci_env_addr)
    {
        //voice_setting off:5a
        voice_settings_addr = hci_env_addr+0x5a;
    }

    if(voice_settings_addr != 0)
    {
        BT_DRV_TRACE(2,"BT_REG_OP:Fast_bt_init1:%s %d",__func__,voice_settings);
        *(uint16_t *)voice_settings_addr = voice_settings;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

uint8_t bt_drv_reg_op_lmp_sco_hdl_get(uint16_t sco_handle)
{
    return 0;
}

bool bt_drv_reg_op_lmp_sco_hdl_set(uint16_t sco_handle, uint8_t lmp_hdl)
{
    return false;
}

void bt_drv_reg_op_host_buff_and_flow_ctl_set
(uint16_t acl_pkt_len, uint16_t acl_pkt_num,uint16_t sync_pkt_len, uint16_t sync_pkt_num,bool flow_ctl_en)
{
    uint32_t acl_pkt_len_addr = 0;
    uint32_t acl_pkt_num_addr = 0;
    uint32_t sync_pkt_len_addr = 0;
    uint32_t sync_pkt_num_addr = 0;
    uint32_t flow_ctl_addr = 0;

    BT_DRV_REG_OP_CLK_ENB();

    if(hci_fc_env_addr)
    {
        flow_ctl_addr = hci_fc_env_addr;
    }

    if(flow_ctl_addr != 0)
    {
        //hci_fc_env.host_set.acl_pkt_len
        acl_pkt_len_addr = flow_ctl_addr+2;
        //hci_fc_env.host_set.acl_pkt_num
        acl_pkt_num_addr = flow_ctl_addr+4;
        //hci_fc_env.host_set.sync_pkt_len;
        sync_pkt_len_addr = flow_ctl_addr+9;
        //hci_fc_env.host_set.sync_pkt_nb;
        sync_pkt_num_addr = flow_ctl_addr+10;

        BT_DRV_TRACE(6,"Fast_bt_init2:%s,acl len=%x,acl num=%x,sync len=%x sync num=%x,fl_ctl=%d",
                     __func__,acl_pkt_len,acl_pkt_num,sync_pkt_len,sync_pkt_num,flow_ctl_en);

        *(uint8_t *)flow_ctl_addr = flow_ctl_en;
        *(uint16_t *)acl_pkt_len_addr = acl_pkt_len;
        *(uint16_t *)acl_pkt_num_addr = acl_pkt_num;
        *(uint8_t *)sync_pkt_len_addr = sync_pkt_len;
        *(uint16_t *)sync_pkt_num_addr = sync_pkt_num;
    }

    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_page_to_set(uint16_t page_to)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t page_to_addr = 0;

    if(lm_env_addr)
    {
        //page_to off:14
        page_to_addr = lm_env_addr + 0x14;
    }

    if(page_to_addr != 0)
    {
        BT_DRV_TRACE(2,"BT_REG_OP:Fast_bt_init3:%s,to=%x",__func__,page_to);
        *(uint16_t *)page_to_addr = page_to;
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_btdm_address_set(uint8_t* bt_addr, uint8_t* ble_addr)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t btaddr_addr = 0;
    uint32_t bleaddr_addr = 0;
    uint32_t bch_addr = 0;

    if(llm_le_env_addr)
    {
        //public off:8f
        bleaddr_addr = llm_le_env_addr+0x8f;
    }

    if(ld_env_addr)
    {
        btaddr_addr = ld_env_addr;
        //      bch_addr = btaddr_addr+451;
        //local bch:4db
        bch_addr = ld_env_addr  + 0x4db;
    }

    if(bleaddr_addr != 0)
    {
        BT_DRV_TRACE(1,"BT_REG_OP:Fast_bt_init4:%s",__func__);
        DUMP8("%02x ",bt_addr,6);
        DUMP8("%02x ",ble_addr,6);

        for(uint8_t i=0; i<6; i++)
        {
            *(uint8_t *)(bleaddr_addr+i) = ble_addr[i];
        }

        for(uint8_t i=0; i<6; i++)
        {
            *(uint8_t *)(btaddr_addr+i) = bt_addr[i];
        }
        ld_util_bch_create(bt_addr,(uint8_t *)bch_addr);
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}


void bt_drv_reg_op_evtmask_set(uint8_t ble_en)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t bt_evtmsk_addr = 0;
    uint32_t ble_evtmask_addr = 0;

    if(llm_le_env_addr)
    {
        //eventmast off:64
        ble_evtmask_addr = llm_le_env_addr+0x64;
    }

    if(hci_env_addr)
    {
        bt_evtmsk_addr = hci_env_addr;
    }

    if(bt_evtmsk_addr != 0)
    {
        BT_DRV_TRACE(2,"BT_REG_OP:Fast_bt_init5:%s,ble enable=%x",__func__,ble_en);
        uint8_t bt_mask[8]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1f};
        if(ble_en)
        {
            bt_mask[7] = 0x3f;
        }
        memcpy((uint8_t *)bt_evtmsk_addr,bt_mask,8);

        uint8_t ble_mask[8]= {0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        memcpy((uint8_t *)ble_evtmask_addr,ble_mask,8);
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_simplepair_mode_set(uint8_t en)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(lm_env_addr)
    {
        //sp_mode off:2c
        *(uint8_t *)(lm_env_addr+0x2c) = en;
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_class_of_dev_set(uint8_t* class_of_dev)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t class_of_dev_addr = 0;

    if(ld_env_addr)
    {
        //ld_env.class_of_dev.A
        class_of_dev_addr = ld_env_addr+6;
    }

    if(class_of_dev_addr != 0)
    {
        BT_DRV_TRACE(1,"BT_REG_OP:Fast_bt_init7:%s",__func__);
        DUMP8("%02x ",class_of_dev,3);
        memcpy((uint8_t *)class_of_dev_addr, class_of_dev, 3);
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}


void bt_drv_reg_op_sleep_set(bool en)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t sleep_addr = 0;

    if(rwip_env_addr)
    {
        //ext_wakeup_enable off:d
        sleep_addr = rwip_env_addr+0xd;
    }

    if(sleep_addr != 0)
    {
        BT_DRV_TRACE(2,"BT_REG_OP:Fast_bt_init:%s en=%d",__func__,en);
        *(uint8_t *)(sleep_addr) = en;
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_sco_fifo_reset(void)
{
    uint16_t sco_ptr0 = 0, sco_ptr1 = 0;
    //#define REG_EM_ET_BASE_ADDR 0xD0210000
    uint8_t *em_area_base = (uint8_t*)0xd0210000;

    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    BTDIGITAL_REG_GET_FIELD(0xd02201cc, 0xffff, 0, sco_ptr0);
    BTDIGITAL_REG_GET_FIELD(0xd02201cc, 0xffff, 16, sco_ptr1);
    BT_DRV_TRACE(3,"BT_REG_OP:bt_drv_reg_op_sco_fifo_reset %d/%d %08x",sco_ptr0, sco_ptr1, BTDIGITAL_REG(0xd02201cc));
    memset(em_area_base+sco_ptr0, 0x55, 60);
    memset(em_area_base+sco_ptr1, 0x55, 60);

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_sco_fifo_dump(void)
{
    uint16_t sco_ptr0 = 0, sco_ptr1 = 0;
    //#define REG_EM_ET_BASE_ADDR 0xD0210000
    uint8_t *em_area_base = (uint8_t*)0xd0210000;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    BTDIGITAL_REG_GET_FIELD(0xd02201cc, 0xffff, 0, sco_ptr0);
    BTDIGITAL_REG_GET_FIELD(0xd02201cc, 0xffff, 16, sco_ptr1);
    BT_DRV_TRACE(3,"BT_REG_OP:bt_drv_reg_op_sco_fifo_reset %d/%d %08x",sco_ptr0, sco_ptr1, BTDIGITAL_REG(0xd02201cc));
    DUMP8("%02x ", em_area_base+sco_ptr0, 10);
    DUMP8("%02x ", em_area_base+sco_ptr1, 10);

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

uint8_t bt_drv_reg_op_get_sync_id_op(uint8_t op, uint8_t sync_id)
{
    return 0xff;
}

void bt_drv_reg_op_disable_reporting_en(uint16_t conhdl)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t lc_evt_ptr = 0x0;
    uint32_t report_en_addr;
    uint8_t linkid = btdrv_conhdl_to_linkid(conhdl);

    if (btdrv_is_link_index_valid(linkid))
    {
        if(lc_env_addr)
        {
            //reporting_en off:28a
            lc_evt_ptr = *(volatile uint32_t *)(lc_env_addr+linkid*4);//lc_env
            report_en_addr = lc_evt_ptr+0x28a;
            *(uint8_t *)report_en_addr = 0;
            BT_DRV_TRACE(0,"BT_REG_OP:disable afh report");
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

uint8_t bt_drv_reg_opcheck_controller_ld_lc_layer_mismatch(void)
{
    uint8_t cnt = 0;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(ld_acl_env_addr != 0)
    {
        for(uint8_t link_id=0; link_id<3; link_id++)
        {
            uint32_t ld_acl_evt_ptr = 0;
            uint32_t lc_evt_ptr = 0;
            ld_acl_evt_ptr= *(uint32_t *)(ld_acl_env_addr+link_id*4);
            lc_evt_ptr = *(volatile uint32_t *)(lc_env_addr+link_id*4);
            if((ld_acl_evt_ptr && !lc_evt_ptr) ||
               (!ld_acl_evt_ptr && lc_evt_ptr) )
            {
                cnt ++;
            }
        }
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return cnt;
}
void bt_drv_reg_op_ble_sup_timeout_set(uint16_t ble_conhdl, uint16_t sup_to)
{
    uint32_t llc_env_ptr = 0;

    BT_DRV_REG_OP_CLK_ENB();

    if(llc_env_addr)
    {
        uint32_t llc_env_address = llc_env_addr+ble_conhdl*4;
        llc_env_ptr = *(volatile uint32_t *)llc_env_address;
    }

    if(llc_env_ptr != 0)
    {
        //sup_to off:54
        uint32_t llc_env_sup_to_address = llc_env_ptr + 0x54;
        *(volatile uint16_t *)llc_env_sup_to_address = sup_to;
        BT_DRV_TRACE(1,"BT_REG_OP:set ble sup_timeout to %d",sup_to);
    }

    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_disable_swagc_nosync_count(void)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(dbg_bt_setting_ext1_addr)
    {
        *(uint16_t *)(dbg_bt_setting_ext1_addr+0x14) = 0xa604;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_hw_sw_agc_select(uint8_t agc_mode)
{
    uint32_t hw_agc_select_addr = 0;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(rwip_rf_addr)
    {
        //hw_or_sw_agc off:37
        hw_agc_select_addr = rwip_rf_addr+0x37;
    }
    if(hw_agc_select_addr != 0)
    {
        *(volatile uint8_t *)hw_agc_select_addr = agc_mode;
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void ble_maxevtime_set(int elt_idx, uint16_t value)
{
    BT_DRV_REG_OP_CLK_ENB();
    BTDIGITAL_EM_BT_WR(BLE_MAXEVTIME_ADDR + elt_idx * BLE_EM_CS_SIZE, value);
    BT_DRV_REG_OP_CLK_DIB();
}


uint16_t ble_maxevtime_get(int elt_idx)
{
    uint16_t nRet = 0;

    BT_DRV_REG_OP_CLK_ENB();
    nRet = BTDIGITAL_BT_EM(BLE_MAXEVTIME_ADDR + elt_idx * BLE_EM_CS_SIZE);
    BT_DRV_REG_OP_CLK_DIB();

    return nRet;
}


void bt_drv_reg_op_afh_follow_en(bool enable, uint8_t be_followed_link_id, uint8_t follow_link_id)
{
#if BT_AFH_FOLLOW
    BT_DRV_REG_OP_CLK_ENB();
    *(uint8_t *)(lm_env_addr+0x168) = be_followed_link_id;//be follow follow link id //ok
    *(uint8_t *)(lm_env_addr+0x169) = follow_link_id;//follow link id
    *(uint8_t *)(lm_env_addr+0x167) = enable; //afh follow enable
    BT_DRV_REG_OP_CLK_DIB();
#endif
}

void bt_drv_reg_op_set_agc_thd(bool isMaster, bool isSco)
{
}

void bt_drv_reg_op_force_set_sniff_att(uint16_t conhdle)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t linkid = btdrv_conhdl_to_linkid(conhdle);
    uint32_t acl_evt_ptr = 0x0;

    if (ld_acl_env_addr != 0&& btdrv_is_link_index_valid(linkid))
    {
        acl_evt_ptr=*(volatile uint32_t *)(ld_acl_env_addr+linkid*4);//ld_acl_env

        if (acl_evt_ptr != 0)
        {
            uint32_t sniff_att_addr = acl_evt_ptr + 0xe6;
            if(*(uint16_t *)sniff_att_addr <3)
            {
                *(uint16_t *)sniff_att_addr = 3;
            }
            BT_DRV_TRACE(0,"BT_REG_OP:force set sniff att");
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_fa_gain_direct_set(uint8_t gain_idx)
{
}

uint8_t bt_drv_reg_op_fa_gain_direct_get(void)
{
    return 0;
}

struct rx_gain_fixed_t
{
    uint8_t     enable;//enable or disable
    uint8_t     bt_or_ble;//0:bt 1:ble
    uint8_t     cs_id;//link id
    uint8_t     gain_idx;//gain index
};

void bt_drv_reg_op_dgb_link_gain_ctrl_set(uint16_t connHandle, uint8_t bt_ble_mode, uint8_t gain_idx, uint8_t enable)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(rf_rx_gain_fixed_tbl_addr != 0)
    {
        //struct rx_gain_fixed_t RF_RX_GAIN_FIXED_TBL[HOST_GAIN_TBL_MAX];
        struct rx_gain_fixed_t *rx_gain_fixed_p = (struct rx_gain_fixed_t *)rf_rx_gain_fixed_tbl_addr;
        uint8_t cs_id = btdrv_conhdl_to_linkid(connHandle);

        if (btdrv_is_link_index_valid(cs_id))
        {
            for (uint8_t i=0; i<3; i++)
            {
                if ((rx_gain_fixed_p->cs_id == cs_id) &&
                    (rx_gain_fixed_p->bt_or_ble == bt_ble_mode))
                {
                    rx_gain_fixed_p->enable    = enable;
                    rx_gain_fixed_p->bt_or_ble = bt_ble_mode;
                    rx_gain_fixed_p->gain_idx = gain_idx;
                    BT_DRV_TRACE(5,"BT_REG_OP:%s hdl:%x/%x mode:%d idx:%d", __func__, connHandle, cs_id, rx_gain_fixed_p->bt_or_ble, gain_idx);
                }
                rx_gain_fixed_p++;
            }
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}


void bt_drv_reg_op_dgb_link_gain_ctrl_clear(uint16_t connHandle, uint8_t bt_ble_mode)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(rf_rx_gain_fixed_tbl_addr != 0)
    {
        //struct rx_gain_fixed_t RF_RX_GAIN_FIXED_TBL[HOST_GAIN_TBL_MAX];
        struct rx_gain_fixed_t *rx_gain_fixed_p = (struct rx_gain_fixed_t *)rf_rx_gain_fixed_tbl_addr;
        uint8_t cs_id = btdrv_conhdl_to_linkid(connHandle);

        if (btdrv_is_link_index_valid(cs_id))
        {
            for (uint8_t i=0; i<3; i++)
            {
                if ((rx_gain_fixed_p->cs_id == cs_id) &&
                    (rx_gain_fixed_p->bt_or_ble == bt_ble_mode))
                {
                    rx_gain_fixed_p->enable    = 0;
                    rx_gain_fixed_p->bt_or_ble = bt_ble_mode;
                    rx_gain_fixed_p->gain_idx = 0;
                }
                rx_gain_fixed_p++;
            }
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}


void bt_drv_reg_op_dgb_link_gain_ctrl_init(void)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(rf_rx_gain_fixed_tbl_addr != 0)
    {
        //struct rx_gain_fixed_t RF_RX_GAIN_FIXED_TBL[HOST_GAIN_TBL_MAX];
        struct rx_gain_fixed_t *rx_gain_fixed_p = (struct rx_gain_fixed_t *)rf_rx_gain_fixed_tbl_addr;

        for (uint8_t i=0; i<3; i++)
        {
            rx_gain_fixed_p->cs_id = i;
            rx_gain_fixed_p++;
        }
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_rx_gain_fix(uint16_t connHandle, uint8_t bt_ble_mode, uint8_t gain_idx, uint8_t enable, uint8_t table_idx)
{
    BT_DRV_REG_OP_CLK_ENB();
    if(rf_rx_gain_fixed_tbl_addr != 0)
    {
        //struct rx_gain_fixed_t RF_RX_GAIN_FIXED_TBL[HOST_GAIN_TBL_MAX];
        struct rx_gain_fixed_t *rx_gain_fixed_p = (struct rx_gain_fixed_t *)rf_rx_gain_fixed_tbl_addr;
        uint8_t cs_id;

        if(bt_ble_mode == 0)//bt
        {
            cs_id = btdrv_conhdl_to_linkid(connHandle);
        }
        else if(bt_ble_mode == 1)//ble
        {
            cs_id = connHandle;
        }
        else
        {
            BT_DRV_TRACE(1,"BT_REG_OP:%s:fix gain fail",__func__);
            goto exit;
        }

        if(table_idx < 3)
        {
            rx_gain_fixed_p[table_idx].enable = enable;
            rx_gain_fixed_p[table_idx].bt_or_ble = bt_ble_mode;
            rx_gain_fixed_p[table_idx].cs_id = cs_id;
            rx_gain_fixed_p[table_idx].gain_idx = gain_idx;
        }
    }
exit:
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_set_ibrt_auto_accept_sco(bool en)
{
}

void bt_drv_reg_op_update_dbg_state(void)
{
}

void bt_drv_reg_op_set_ibrt_reject_sniff_req(bool en)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint8_t *accept_remote_enter_sniff_ptr = NULL;

    if (hci_dbg_bt_setting_ext2_addr == 0)
    {
        BT_DRV_TRACE(1, "BT_REG_OP:set reject sniff req %d, please fix it", en);
        goto exit;
    }

    accept_remote_enter_sniff_ptr = (uint8_t *)(hci_dbg_bt_setting_ext2_addr + 0x49);

    *accept_remote_enter_sniff_ptr = en ? 0 : 1;
exit:
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

uint8_t bt_drv_reg_op_get_esco_nego_airmode(uint8_t sco_link_id)
{
    uint8_t airmode = 0;
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();

    if(lc_sco_env_addr != 0)
    {
        uint32_t p_nego_params_addr = *((uint32_t *)(lc_sco_env_addr + 4 + sco_link_id * 8));

        if (sco_link_id > 2 || p_nego_params_addr == 0)
        {
            airmode = 0xff;
            goto exit;
        }
        //air_mode off:5e
        airmode = *((uint8_t *)(p_nego_params_addr + 0x5e));

        BT_DRV_TRACE(1,"BT_REG_OP:Get nego esco airmode=%d",airmode);
    }

exit:
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
    return airmode;
}

void bt_drv_reg_op_set_max_pwr_rcv(uint16_t connHandle)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint32_t lc_evt_ptr = 0x0;
    uint32_t max_power_rcv_addr;
    uint8_t linkid = btdrv_conhdl_to_linkid(connHandle);

    if(lc_env_addr)
    {
        lc_evt_ptr = *(volatile uint32_t *)(lc_env_addr+linkid*4);//lc_env
    }

    if(lc_evt_ptr)
    {
        //lc_env_ptr->link.MaxPowerRcv
        max_power_rcv_addr = lc_evt_ptr+0x61; //ok
        *(bool *)max_power_rcv_addr = true;
        BT_DRV_TRACE(1,"BT_REG_OP:conhdl=%x, max pwr recieve",connHandle);
    }
    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}

#if defined(PCM_FAST_MODE) && defined(PCM_PRIVATE_DATA_FLAG)
void bt_drv_reg_op_set_pcm_flag()
{
    BT_DRV_REG_OP_CLK_ENB();
    BTDIGITAL_REG_SET_FIELD(0xd022046c, 1, 3, 1);
    BT_DRV_REG_OP_CLK_DIB();
}
#endif


void bt_drv_reg_op_ebq_test_setting(void)
{
    BT_DRV_REG_OP_CLK_ENB();
    struct dbg_set_ebq_test* set_ebq_test = (struct dbg_set_ebq_test*)ebq_test_setting_addr;
    if(set_ebq_test)
    {
        BT_DRV_TRACE(0,"BT_REG_OP:ebq_test_setting");
        set_ebq_test->ebq_testmode = 0x01;
        set_ebq_test->ble_set_privacy_mode = 0x01;
        set_ebq_test->ll_con_ini_bv2324 = 0x00;
        set_ebq_test->ll_con_ini_bv2021 = 0x00;
        set_ebq_test->ll_ddi_scn_bv26 = 0x00;
        set_ebq_test->ll_sec_adv_bv05 = 0x00;
        set_ebq_test->ll_sync_win_default = 0x20;
        set_ebq_test->auth_end_notify_host_lk = 0x01;
        set_ebq_test->ll_sec_adv_bv09 = 0x00;
        set_ebq_test->ll_sec_adv_bv17 = 0x00;
        set_ebq_test->ll_sec_adv_bv1819 = 0x00;
        set_ebq_test->LocEncKeyRefresh = 0x01;
        set_ebq_test->publickey_check = 0x00;
        set_ebq_test->Qos = 0x01;
        set_ebq_test->hv1 = 0x01;
        set_ebq_test->ssr = 0x01;
        set_ebq_test->aes_ccm_daycounter = 0x01;
        set_ebq_test->bb_prot_flh_bv01 = 0x01;
        set_ebq_test->bb_prot_arq_bv43 = 0x01;
        set_ebq_test->pause_aes_generate_rand = 0x01;
    }
    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_hw_spi_en_setf(int elt_idx, uint8_t hwspien)
{
    BT_DRV_REG_OP_CLK_ENB();

    BTDIGITAL_EM_BT_WR(EM_BT_BT_EXT2_ADDR + elt_idx * BT_EM_SIZE,
                       (BTDIGITAL_BT_EM(EM_BT_BT_EXT2_ADDR + elt_idx * BT_EM_SIZE) & ~((uint16_t)0x00000008)) | ((uint16_t)hwspien << 3));

    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_set_rand_seed(uint32_t seed)
{
}

uint8_t bt_drv_reg_op_get_controller_ble_tx_free_buffer(void)
{
    uint32_t *tx_buf_ptr=NULL;
    uint8_t tx_free_buf_count=0;

    if(em_buf_env_addr)
    {
        tx_buf_ptr = (uint32_t *)(em_buf_env_addr+0x14); //em_buf_env.tx_buff_free
    }
    else
    {
        BT_DRV_TRACE(1, "REG_OP: please fix %s", __func__);
        return 0;
    }

    //check tx buff
    while(tx_buf_ptr && *tx_buf_ptr)
    {
        tx_free_buf_count++;
        tx_buf_ptr = (uint32_t *)(*tx_buf_ptr);
    }

    return tx_free_buf_count;
}


void bt_drv_reg_op_bt_sync_swagc_en_set(uint8_t en)
{
    BT_DRV_REG_OP_CLK_ENB();

    uint8_t *bt_sync_swagc_en = NULL;

    if (hci_dbg_bt_setting_ext2_addr != 0)
    {
        bt_sync_swagc_en = (uint8_t *)(hci_dbg_bt_setting_ext2_addr + 0x2c);

        *bt_sync_swagc_en = en;
    }

    BT_DRV_REG_OP_CLK_DIB();
}

void bt_drv_reg_op_swagc_mode_set(uint8_t mode)
{
#ifdef __NEW_SWAGC_MODE__
    BT_DRV_REG_OP_CLK_ENB();

    uint32_t lock = int_lock_global();
    //disable hw spi
    btdrv_clear_spi_trig_pos_enable();
    if(mode == NEW_SYNC_SWAGC_MODE)
    {
        //open second rf spi:
        BTDIGITAL_REG_SET_FIELD(0xd02201e8, 0x1, 0, 1);
        //open rf new sync agc mode
        bt_drv_rf_set_bt_sync_agc_enable(true);
        //open BT sync AGC process cbk
        bt_drv_reg_op_bt_sync_swagc_en_set(1);
    }
    else if(mode == OLD_SWAGC_MODE)
    {
        //close second rf spi
        BTDIGITAL_REG_SET_FIELD(0xd02201e8, 0x1,  0, 0);
        //close rf new sync agc mode
        bt_drv_rf_set_bt_sync_agc_enable(false);
        //close BT sync AGC process cbk
        bt_drv_reg_op_bt_sync_swagc_en_set(0);

        //[19:8]: rrcgain
        //[7]: rrc_engain
        BTDIGITAL_REG_SET_FIELD(0xd03502c0, 0x1,  7, 0);
        BTDIGITAL_REG_SET_FIELD(0xd03502c0, 0xfff,  8, 0x80);
    }
    //enable hw spi
    btdrv_set_spi_trig_pos_enable();
    int_unlock_global(lock);
    BT_DRV_REG_OP_CLK_ENB();
#endif
}

void bt_drv_reg_op_set_fa_rx_gain_idx(uint8_t rx_gain_idx)
{
    BT_DRV_REG_OP_ENTER();
    BT_DRV_REG_OP_CLK_ENB();
    uint8_t * fa_gain_idx = NULL;

    if (hci_dbg_bt_setting_ext2_addr != 0)
    {
        fa_gain_idx = (uint8_t *)(hci_dbg_bt_setting_ext2_addr + 0x22);

        *fa_gain_idx = rx_gain_idx;
    }

    BT_DRV_REG_OP_CLK_DIB();
    BT_DRV_REG_OP_EXIT();
}
