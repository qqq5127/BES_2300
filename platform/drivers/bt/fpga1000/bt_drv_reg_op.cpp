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
#include "bt_drv_interface.h"
#include "bt_drv.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include <string.h>
#include "CrashCatcher.h"

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

uint8_t  sleep_param[] =
{
#ifdef __TWS__
   0,
#else
   1,    // sleep_en;
#endif
   1,    // exwakeup_en;
   0xc8,0,    //  lpo_calib_interval;   lpo calibration interval
   0xa,0,0,0,    // lpo_calib_time;  lpo count lpc times
};

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
}

void bt_drv_reg_op_tws_output_power_fix_separate(uint16_t hci_handle, uint16_t pwr)
{
}

#define SNIFF_IN_SCO    2
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

int bt_drv_reg_op_currentfreeaclbuf_get(void)
{
    return 0;
}

//static uint16_t mobile_sync_info;
void bt_drv_reg_op_save_mobile_airpath_info(uint16_t hciHandle)
{
}

void bt_drv_reg_op_block_xfer_with_mobile(uint16_t hciHandle)
{
}

void bt_drv_reg_op_resume_xfer_with_mobile(uint16_t hciHandle)
{
}

int bt_drv_reg_op_packet_type_checker(uint16_t hciHandle)
{
    return 0;
}

void bt_drv_reg_op_max_slot_setting_checker(uint16_t hciHandle)
{}

void bt_drv_reg_op_force_task_dbg_idle(void)
{
}

void bt_drv_reg_op_afh_follow_mobile_mobileidx_set(uint16_t hciHandle)
{}


void bt_drv_reg_op_afh_follow_mobile_twsidx_set(uint16_t hciHandle)
{}

void bt_drv_reg_op_afh_bak_reset(void)
{}

void bt_drv_reg_op_afh_bak_save(uint8_t role, uint16_t mobileHciHandle)
{}

void bt_drv_reg_op_connection_checker(void)
{}

void bt_drv_reg_op_sco_status_store(void)
{}

void bt_drv_reg_op_sco_status_restore(void)
{}

void bt_drv_reg_op_afh_set_default(void)
{
}

void bt_drv_reg_op_ld_sniffer_master_addr_set(uint8_t * addr)
{
}

void bt_drv_reg_op_lsto_hack(uint16_t hciHandle, uint16_t lsto)
{
}

uint8_t msbc_find_tx_sync(uint8_t *buff)
{
    return 0;
}

bool bt_drv_reg_op_sco_tx_buf_restore(uint8_t *trigger_test)
{
    return 0;
}

void bt_drv_reg_op_bt_info_checker(void)
{
}

void bt_drv_reg_op_ibrt_env_reset(void)
{
}

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
    return 0;
}

void bt_drv_reg_op_controller_state_checker(void)
{
}

void bt_drv_reg_op_crash_dump(void)
{
}

uint8_t bt_drv_reg_op_get_tx_pwr(uint16_t connHandle)
{
    return 0;
}

void bt_drv_reg_op_set_tx_pwr(uint16_t connHandle, uint8_t txpwr)
{
}

void bt_drv_reg_op_fix_tx_pwr(uint16_t connHandle)
{
}


void bt_drv_reg_op_enable_emsack_mode(uint16_t connHandle, uint8_t master, uint8_t enable)
{
}

#define __REBOOT_PAIRING_MODE_ADJUST_GAIN__

void bt_drv_reg_op_set_accessible_mode(uint8_t mode)
{

}

void bt_drv_reg_op_set_swagc_mode(uint8_t mode)
{
}

void bt_drv_reg_op_set_reboot_pairing_mode(uint8_t mode)
{
}

void bt_drv_reg_op_force_retrans(bool enalbe)
{
    return;

    BT_DRV_REG_OP_ENTER();

    if (enalbe)
    {
        BTDIGITAL_REG_SET_FIELD(0xd0220468,3,23,3);
    }
    else
    {
        BTDIGITAL_REG_SET_FIELD(0xd0220468,3,23,0);
    }
    BT_DRV_REG_OP_EXIT();
}

void bt_drv_reg_op_enable_pcm_tx_hw_cal(void)
{
}



void bt_drv_reg_op_monitor_clk(void)
{
}

struct rx_monitor
{
    int8_t rssi;
    uint8_t rxgain;
};

bool bt_drv_reg_op_read_rssi_in_dbm(uint16_t connHandle,rx_agc_t* rx_val)
{
    return true;
}

void bt_drv_reg_op_save_fast_ack_sync_bit(void)
{
    /*
     *  save ibrt master's fast ack sync bit
     * 1. snoop connected(ibrt master)
     * 2. or tws switch complete(new ibrt master)
     */

}

void bt_drv_reg_op_block_fast_ack_with_mobile(void)
{
    /*
     * ibrt master set its fast ack sync bit wrong,let itself NACK mobile link always
     */
}

void bt_drv_reg_op_resume_fast_ack_with_mobile(void)
{

    /*
     *  old master recovery its fast ack sync bit right
     */
}

void bt_drv_reg_op_acl_silence(uint16_t connHandle, uint8_t silence)
{
}

///sniffer connect information environment structure
struct ld_sniffer_connect_env
{
    ///
    uint16_t bitoff;
    ///
    uint32_t clk_offset;
    ///
    uint8_t map[10];
    ///
    uint32_t afh_instant;
    ///
    uint8_t afh_mode;
    ///
    uint8_t enc_mode;
    ///
    uint8_t  ltk[16];
    ///
    uint8_t  role;
};

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
}

void bt_drv_reg_op_acl_tx_type_set(uint16_t hciHandle, uint8_t slot_sel)
{
}

void bt_drv_reg_op_acl_tx_type_trace(uint16_t hciHandle)
{
}

uint16_t em_bt_bitoff_getf(int elt_idx)
{
#if 0
    uint16_t localVal = BTDIGITAL_BT_EM(EM_BT_BITOFF_ADDR + elt_idx * BT_EM_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000003FF)) == 0);
    return (localVal >> 0);
#else
    return 0;
#endif
}

void em_bt_bitoff_setf(int elt_idx, uint16_t bitoff)
{
#if 0

    ASSERT_ERR((((uint16_t)bitoff << 0) & ~((uint16_t)0x000003FF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_BITOFF_ADDR + elt_idx * BT_EM_SIZE, (uint16_t)bitoff << 0);
#endif
}

void em_bt_clkoff0_setf(int elt_idx, uint16_t clkoff0)
{
#if 0
    ASSERT_ERR((((uint16_t)clkoff0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_CLKOFF0_ADDR + elt_idx * BT_EM_SIZE, (uint16_t)clkoff0 << 0);
#endif
}

uint16_t em_bt_clkoff0_getf(int elt_idx)
{
    return 0;
#if 0
    uint16_t localVal = BTDIGITAL_BT_EM(EM_BT_CLKOFF0_ADDR + elt_idx * BT_EM_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
#endif
}
void em_bt_clkoff1_setf(int elt_idx, uint16_t clkoff1)
{
#if 0

    ASSERT_ERR((((uint16_t)clkoff1 << 0) & ~((uint16_t)0x000007FF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_CLKOFF1_ADDR + elt_idx * BT_EM_SIZE, (uint16_t)clkoff1 << 0);
#endif
}

uint16_t em_bt_clkoff1_getf(int elt_idx)
{
    return 0;
#if 0

    uint16_t localVal = BTDIGITAL_BT_EM(EM_BT_CLKOFF1_ADDR + elt_idx * BT_EM_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000007FF)) == 0);
    return (localVal >> 0);
#endif

}

void em_bt_wincntl_pack(int elt_idx, uint8_t rxwide, uint16_t rxwinsz)
{
#if 0
    ASSERT_ERR((((uint16_t)rxwide << 15) & ~((uint16_t)0x00008000)) == 0);
    ASSERT_ERR((((uint16_t)rxwinsz << 0) & ~((uint16_t)0x00000FFF)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_WINCNTL_ADDR + elt_idx * BT_EM_SIZE,  ((uint16_t)rxwide << 15) | ((uint16_t)rxwinsz << 0));
#endif
}

void bt_drv_reg_op_update_sniffer_bitoffset(uint16_t mobile_conhdl,uint16_t master_conhdl)
{
}

void bt_drv_reg_op_modify_bitoff_timer(uint16_t time_out)
{
}

void bt_drv_reg_op_cs_monitor(void)
{
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
    return 0xff;
}
extern "C" void bt_drv_reg_op_data_tx_flow(uint8_t linkid, uint8_t on_off)
{
}
void bt_drv_reg_op_set_tpoll(uint8_t linkid,uint16_t poll_interval)
{
}

void bt_drv_set_music_link(uint8_t link_id)
{
}

void bt_drv_set_music_link_duration_extra(uint8_t slot)
{
    return;
}

uint32_t bt_drv_reg_op_get_reconnecting_flag()
{
    return 0;
}

void bt_drv_reg_op_set_reconnecting_flag()
{
}

void bt_drv_reg_op_clear_reconnecting_flag()
{
}
void bt_drv_reg_op_music_link_config(uint16_t active_link,uint8_t active_role,uint16_t inactive_link,uint8_t inactive_role)
{
}

void bt_drv_reg_op_afh_env_reset(void)
{
}

void bt_drv_reg_op_pcm_set(uint8_t en)
{
    return;
}

uint8_t bt_drv_reg_op_pcm_get()
{
    return 0;
}
void bt_drv_clear_skip_flag()
{
}
bool bt_drv_reg_op_check_bt_controller_state(void)
{
    return 0;
}

void bt_drv_reg_op_piconet_clk_offset_get(uint16_t connHandle, int32_t *clock_offset, uint16_t *bit_offset)
{
}

void bt_drv_reg_op_sw_seq_filter(uint16_t existConnHandle)
{
}

void bt_drv_reg_op_clean_flags_of_ble_and_sco(void)
{
}

void bt_drv_reg_op_sco_txfifo_reset(uint16_t codec_id)
{
}

void bt_drv_reg_op_dma_tc_clkcnt_get(uint32_t *btclk, uint16_t *btcnt)
{
}

#if defined(IBRT)
void em_bt_bt_ext1_tx_silence_setf(int elt_idx, uint8_t txsilence)
{
#if 0
    ASSERT_ERR((((uint16_t)txsilence << 15) & ~((uint16_t)0x00008000)) == 0);
    BTDIGITAL_EM_BT_WR(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE,
                       (BTDIGITAL_BT_EM(EM_BT_BT_EXT1_ADDR + elt_idx * BT_EM_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)txsilence << 15));
#endif
}

void bt_drv_reg_op_acl_tx_silence(uint16_t connHandle, uint8_t on)
{
    return;
    uint8_t idx = btdrv_conhdl_to_linkid(connHandle);
    BT_DRV_TRACE(2,"BT_REG_OP:BT ACL tx silence idx=%d,on=%d\n",idx,on);

    if(TX_SILENCE_ON == on)
    {
        em_bt_bt_ext1_tx_silence_setf(idx, TX_SILENCE_ON);
    }
    else if(TX_SILENCE_OFF == on)
    {
        em_bt_bt_ext1_tx_silence_setf(idx, TX_SILENCE_OFF);
    }
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
}

void btdrv_reg_op_set_tws_link_duration(uint8_t slot_num)
{
}

void btdrv_reg_op_enable_private_tws_schedule(bool enable)
{
    return;
}

void bt_drv_reg_op_decrease_tx_pwr_when_reconnect(bool enable)
{
}

int8_t  bt_drv_reg_op_rssi_correction(int8_t rssi)
{
    return 0;
}

void bt_drv_reg_op_lm_nb_sync_hacker(uint8_t sco_status)
{
}

#define BT_FASTACK_ADDR   0xD0220468
#define BT_SCO_TRIGGER_BYPASS_ADDR   0xD022046C
void bt_drv_reg_op_fastack_status_checker(uint16_t conhdl)
{
}

void bt_drv_reg_op_clear_ibrt_snoop_status(void)
{
}
#endif


void bt_drv_reg_op_acl_tx_silence_clear(uint16_t connHandle)
{
#if 0//defined(IBRT)

    uint8_t idx = btdrv_conhdl_to_linkid(connHandle);

    BT_DRV_TRACE(1,"BT_REG_OP:ACL tx silence clear idx=%d\n",idx);
    em_bt_bt_ext1_tx_silence_setf(idx, TX_SILENCE_OFF);
#endif
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
    uint16_t curr_conn_latency = 0xffff;
    return curr_conn_latency;
}
bool bt_drv_reg_op_set_ble_conn_latency(uint16_t ble_conhdl, uint16_t conn_latency)
{
    return false;
}

void bt_drv_reg_op_write_private_public_key(uint8_t* private_key,uint8_t* public_key)
{

}


void bt_drv_reg_op_hwagc_mode_set(uint8_t sync_mode)
{
}

void btdrv_for_test_mode_disable(void)
{
}

uint16_t bt_drv_reg_op_get_ibrt_sco_hdl(uint16_t acl_hdl)
{
    // FIXME
    return acl_hdl|0x100;
}

void bt_drv_reg_op_get_ibrt_address(uint8_t *addr)
{
}


void bt_drv_reg_op_set_tws_link_id(uint8_t link_id)
{
}

void bt_drv_reg_op_hack_max_slot(uint8_t link_id,uint8_t slot)
{
}

void bt_drv_reg_op_rx_slot_stop_lantency_checker(void)
{

}

void bt_drv_reg_op_fa_set(uint8_t en)
{
#if 0
    if(hal_get_chip_metal_id()>=HAL_CHIP_METAL_ID_2)
    {
        if(en)
        {
            *(uint32_t *)0xd0220468 |=0x00400000;
            BT_DRV_TRACE(1,"%s", __func__);
        }
        else
        {
            *(uint32_t *)0xd0220468 &= ~0x00400000;
        }
    }
#endif
}

//////return true means controller had received a data packet
bool bt_drv_reg_op_check_seq_pending_status(void)
{
    return 0;
}


///if the flag is not clear when disconnect happened  call this function
void bt_drv_reg_op_clear_seq_pending_status(void)
{
}

void bt_drv_reg_op_set_link_policy(uint8_t linkid, uint8_t policy)
{
}

void bt_drv_reg_op_voice_settings_set(uint16_t voice_settings)
{
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
}

void bt_drv_reg_op_page_to_set(uint16_t page_to)
{
}

void bt_drv_reg_op_btdm_address_set(uint8_t* bt_addr, uint8_t* ble_addr)
{
}

void bt_drv_reg_op_evtmask_set(uint8_t ble_en)
{
}

void bt_drv_reg_op_simplepair_mode_set(uint8_t en)
{

}
void bt_drv_reg_op_class_of_dev_set(uint8_t* class_of_dev)
{
}

void bt_drv_reg_op_sleep_set(bool en)
{
}

uint8_t btdrv_conhdl_to_linkid(uint16_t connect_hdl)
{
    if(connect_hdl<0x80||connect_hdl>0x83)
    {
        ASSERT(0, "ERROR Connect Handle=0x%x", connect_hdl);
        return 3;
    }

    return (connect_hdl - 0x80);
}

void bt_drv_reg_op_force_sco_retrans(bool enalbe)
{
}

uint8_t bt_drv_reg_op_get_sync_id_op(uint8_t op, uint8_t sync_id)
{
    return  0xff;
}

void bt_drv_reg_op_force_set_sniff_att(uint16_t conhdle)
{
}

void bt_drv_reg_op_afh_follow_en(bool enable, uint8_t be_followed_link_id, uint8_t follow_link_id)
{
}

void bt_drv_reg_op_force_set_lc_state(uint16_t conhdl, uint8_t state)
{
}

void bt_drv_reg_op_set_agc_thd(bool isMaster, bool isSco)
{

}

void bt_drv_reg_op_set_ibrt_auto_accept_sco(bool en)
{
    BT_DRV_TRACE(1,"%s REG_OP: not support", __func__);
}

void bt_drv_reg_op_set_ibrt_reject_sniff_req(bool en)
{
    BT_DRV_TRACE(1,"%s REG_OP: not support", __func__);
}

uint8_t bt_drv_reg_op_get_esco_nego_airmode(uint8_t sco_link_id)
{
    BT_DRV_TRACE(1,"%s REG_OP: not support", __func__);
    return 0xff;
}

void bt_drv_regop_set_max_pwr_rcv(uint16_t connHandle)
{
    BT_DRV_TRACE(1,"%s REG_OP: not support", __func__);
}

void bt_drv_reg_op_set_rand_seed(uint32_t seed)
{
}