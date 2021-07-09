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
#ifndef __BT_DRV_REG_OP_H__
#define  __BT_DRV_REG_OP_H__

#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"
#include "plat_types.h"



#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int8_t rssi;
    uint8_t rxgain;
} rx_agc_t;

typedef struct
{
    uint8_t agc_idx0;
    int8_t rssi0;
    int8_t rssi0_max;
    int8_t rssi0_min;
    uint8_t agc_idx1;
    int8_t rssi1;
    int8_t rssi1_max;
    int8_t rssi1_min;
    uint8_t agc_idx2;
    int8_t rssi2;
    int8_t rssi2_max;
    int8_t rssi2_min;
    uint16_t ser;
    uint16_t rx_data_sum;
} __attribute__((packed)) rssi_t;

struct ld_afh_env
{
/// RSSI threshold for interference detection (in dBm)
    int8_t rf_rssi_interf_thr;
/// AFH update period (in sec)
    uint8_t afh_update_period;
/// Maximum duration without receiving a packet before considering a channel quality unknown  (in slots) (2 secs)
    uint16_t afh_access_valid_to;
/// Maximum duration without receiving a packet before reintroducing a channel to the map (in slots) (20 secs)
    uint16_t afh_reaccess_to;
/// Maximum counter value for channel assessment
    int8_t afh_access_count_max;
/// Counter threshold to consider a channel good
    int8_t afh_access_count_thr_good;
/// Counter threshold to consider a channel bad
    int8_t afh_access_count_thr_bad;
/// Minimum counter value for channel assessment
    int8_t afh_access_count_min;
};

/// RSSI threshold for interference detection (in dBm)
#define BT_AFH_RSSI_INTERF_THR       (-70)

/// AFH update period (in sec)
#define BT_AFH_UPDATE_PERIOD         4

/// Maximum duration without receiving a packet before considering a channel quality unknown  (in slots) (2 secs)
#define BT_AFH_ASSESS_VALID_TO       0x1900

/// Maximum duration without receiving a packet before reintroducing a channel to the map (in slots) (20 secs)
#define BT_AFH_REASSESS_TO           0x7D00

/// Maximum counter value for channel assessment
#define BT_AFH_ASSESS_COUNT_MAX      4
/// Counter threshold to consider a channel good
#define BT_AFH_ASSESS_COUNT_THR_GOOD 2
/// Counter threshold to consider a channel bad
#define BT_AFH_ASSESS_COUNT_THR_BAD  -2
/// Minimum counter value for channel assessment
#define BT_AFH_ASSESS_COUNT_MIN      -4

#define RETX_NB_0                 0
#define RETX_NB_1                 1
#define RETX_NB_2                 2
#define RETX_NEGO                 3
#ifdef CHIP_BEST2000
#define MIC_BUFF_ADRR_REG        (0xc0000040)
#define RX_BUFF_ADRR        (0xc0000044)
#elif defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || defined(CHIP_BEST2300P)
#define MIC_BUFF_ADRR_REG        (0xc0000058)
#define RX_BUFF_ADRR        (0xc000005c)
#elif defined(CHIP_BEST2300A)
#define MIC_BUFF_ADRR_REG        (0xc0000058)
#define RX_BUFF_INFO_ADRR        (0xc000005c)
#define RX_BUFF_ADRR        (*(volatile uint32_t *)(RX_BUFF_INFO_ADRR))
#endif

//BT acl tx silence
#define TX_SILENCE_ON                   1
#define TX_SILENCE_OFF                  0

struct dbg_set_ebq_test
{
    uint8_t ebq_testmode;
    uint8_t ble_set_privacy_mode;
    uint8_t ll_con_ini_bv2324;
    uint8_t ll_con_ini_bv2021;
    uint8_t ll_ddi_scn_bv26;
    uint8_t ll_sec_adv_bv05;
    uint8_t ll_sync_win_default;
    uint8_t auth_end_notify_host_lk;
    uint8_t ll_sec_adv_bv09;
    uint8_t ll_sec_adv_bv17;
    uint8_t ll_sec_adv_bv1819;
    uint8_t LocEncKeyRefresh;
    uint8_t publickey_check;
    uint8_t Qos;
    uint8_t hv1;
    uint8_t ssr;
    uint8_t aes_ccm_daycounter;
    uint8_t bb_prot_flh_bv01;
    uint8_t bb_prot_arq_bv43;
    uint8_t pause_aes_generate_rand;
};


void bt_drv_reg_op_rssi_set(uint16_t rssi);
void bt_drv_reg_op_scan_intv_set(uint32_t scan_intv);
void bt_drv_reg_op_encryptchange_errcode_reset(uint16_t hci_handle);
void bt_drv_reg_op_sco_sniffer_checker(void);
void bt_drv_reg_op_trigger_time_checker(void);
void bt_drv_reg_op_tws_output_power_fix_separate(uint16_t hci_handle, uint16_t pwr);
bool bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_get(void);
void bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_set(bool state);
void bt_drv_reg_op_ld_sniffer_master_addr_set(uint8_t * addr);
int bt_drv_reg_op_currentfreeaclbuf_get(void);
void bt_drv_reg_op_save_mobile_airpath_info(uint16_t hciHandle);
void bt_drv_reg_op_block_xfer_with_mobile(uint16_t hciHandle);
void bt_drv_reg_op_resume_xfer_with_mobile(uint16_t hciHandle);
void bt_drv_reg_op_block_fast_ack_with_mobile(void);
void bt_drv_reg_op_resume_fast_ack_with_mobile(void);
int bt_drv_reg_op_packet_type_checker(uint16_t hciHandle);
void bt_drv_reg_op_max_slot_setting_checker(uint16_t hciHandle);
void bt_drv_reg_op_force_task_dbg_idle(void);
void bt_drv_reg_op_afh_follow_mobile_mobileidx_set(uint16_t hciHandle);
void bt_drv_reg_op_afh_follow_mobile_twsidx_set(uint16_t hciHandle);
void bt_drv_reg_op_sco_status_store(void);
void bt_drv_reg_op_sco_status_restore(void);
bool bt_drv_reg_op_sco_tx_buf_restore(uint8_t *trigger_test);
void bt_drv_reg_op_afh_bak_reset(void);
void bt_drv_reg_op_afh_bak_save(uint8_t role, uint16_t mobileHciHandle);
void bt_drv_reg_op_connection_checker(void);
void bt_drv_reg_op_bt_info_checker(void);
uint8_t bt_drv_reg_op_get_controller_tx_free_buffer(void);
uint8_t bt_drv_reg_op_get_controller_ble_tx_free_buffer(void);
void bt_drv_reg_op_ble_buffer_cleanup(void);
bool bt_drv_reg_op_get_dbg_state(void);
void bt_drv_reg_op_crash_dump(void);
void bt_drv_reg_op_set_tx_pwr(uint16_t connHandle, uint8_t txpwr);
uint8_t bt_drv_reg_op_get_tx_pwr(uint16_t connHandle);
void bt_drv_reg_op_fix_tx_pwr(uint16_t connHandle);
void bt_drv_reg_op_lsto_hack(uint16_t hciHandle, uint16_t lsto);
uint16_t bt_drv_reg_op_get_lsto(uint16_t hciHandle);
void bt_drv_reg_op_enable_emsack_mode(uint16_t connHandle, uint8_t master, uint8_t enable);
void bt_drv_reg_op_set_accessible_mode(uint8_t mode);
void bt_drv_reg_op_force_sco_retrans(bool enable);
void bt_drv_reg_op_enable_pcm_tx_hw_cal(void);
void bt_drv_reg_op_monitor_clk(void);
bool bt_drv_reg_op_read_rssi_in_dbm(uint16_t connHandle,rx_agc_t* rx_val);
bool bt_drv_reg_op_read_ble_rssi_in_dbm(uint16_t connHandle,rx_agc_t* rx_val);
void bt_drv_reg_op_set_swagc_mode(uint8_t mode);
void bt_drv_reg_op_set_reboot_pairing_mode(uint8_t mode);

void bt_drv_reg_op_acl_silence(uint16_t connHandle, uint8_t silence);
void bt_drv_reg_op_call_monitor(uint16_t connHandle, uint8_t tws_role);
void bt_drv_reg_op_lock_sniffer_sco_resync(void);
void bt_drv_reg_op_unlock_sniffer_sco_resync(void);
void bt_drv_reg_op_afh_set_default(void);
void bt_drv_reg_op_update_sniffer_bitoffset(uint16_t mobile_conhdl,uint16_t master_conhdl);
void bt_drv_reg_op_modify_bitoff_timer(uint16_t time_out);
uint16_t em_bt_bitoff_getf(int elt_idx);
void bt_drv_reg_op_ble_llm_substate_hacker(void);
void bt_drv_reg_op_esco_acl_sniff_delay_cal(uint16_t hciHandle,bool enable);
bool  bt_drv_reg_op_check_esco_acl_sniff_conflict(uint16_t hciHandle);
void bt_drv_reg_op_set_tpoll(uint8_t linkid,uint16_t poll_interval);
uint8_t  bt_drv_reg_op_get_role(uint8_t linkid);
int8_t  bt_drv_reg_op_rssi_correction(int8_t rssi);
void bt_drv_reg_set_rssi_seed(uint32_t seed);
void bt_drv_reg_op_piconet_clk_offset_get(uint16_t connHandle, int32_t *clock_offset, uint16_t *bit_offset);
void bt_drv_reg_op_sw_seq_filter(uint16_t connHandle);
bool bt_drv_reg_op_check_bt_controller_state(void);
void bt_drv_reg_op_dma_tc_clkcnt_get(uint32_t *btclk, uint16_t *btcnt);

void bt_drv_reg_op_clean_flags_of_ble_and_sco(void);
void bt_drv_reg_op_sco_txfifo_reset(uint16_t codec_id);
void bt_drv_reg_op_ibrt_retx_att_nb_set(uint8_t retx_nb);
void bt_drv_reg_op_acl_tx_type_set(uint16_t hciHandle, uint8_t slot_sel);
void bt_drv_reg_op_acl_tx_type_trace(uint16_t hciHandle);
uint8_t bt_drv_reg_op_acl_tx_type_get(uint16_t hciHandle, uint8_t* br_type, uint8_t* edr_type);
void bt_drv_reg_op_lm_nb_sync_hacker(uint8_t sco_status);

void bt_drv_reg_op_afh_env_reset(void);
void bt_drv_reg_op_acl_tx_silence(uint16_t connHandle, uint8_t on);
void bt_drv_reg_op_acl_tx_silence_clear(uint16_t connHandle);

void bt_drv_reg_op_ibrt_env_reset(void);
void btdrv_reg_op_set_private_tws_poll_interval(uint16_t poll_interval, uint16_t poll_interval_in_sco);
void btdrv_reg_op_enable_private_tws_schedule(bool enable);
void btdrv_reg_op_set_tws_link_duration(uint8_t slot_num);
void bt_drv_reg_op_decrease_tx_pwr_when_reconnect(bool enable);
void bt_drv_reg_op_controller_mem_log_config(void);
void bt_drv_reg_op_clear_ibrt_snoop_status(void);
uint8_t bt_drv_reg_op_linkcntl_aclltaddr_getf(uint16_t conhdl);
uint8_t bt_drv_reg_op_sync_bt_param(uint8_t* param, uint16_t len);
uint32_t bt_drv_reg_op_get_lc_env_ptr(uint16_t conhdl, uint8_t type);
void bt_drv_reg_op_set_tws_link_id(uint8_t link_id);
void bt_drv_reg_op_rx_slot_stop_lantency_checker(void);
void bt_drv_reg_op_hack_max_slot(uint8_t link_id,uint8_t slot);
void bt_drv_reg_op_fa_set(uint8_t en);
bool bt_drv_reg_op_check_seq_pending_status(void);
void bt_drv_reg_op_clear_seq_pending_status(void);
void bt_dev_clear_verison_receive_flag(uint8_t linkid);
void bt_drv_reg_op_fastack_status_checker(uint16_t conhdl);
void bt_drv_reg_op_set_link_policy(uint8_t linkid, uint8_t policy);
uint8_t bt_drv_reg_op_lmp_sco_hdl_get(uint16_t sco_handle);
bool bt_drv_reg_op_lmp_sco_hdl_set(uint16_t sco_handle, uint8_t lmp_hdl);
void bt_drv_reg_op_controller_state_checker(void);
void bt_drv_reg_op_controller_ble_flow_checker(void);
void bt_drv_reg_op_force_set_lc_state(uint16_t conhdl, uint8_t state);
uint8_t bt_drv_reg_op_force_get_lc_state(uint16_t conhdl);
void bt_drv_digital_config_for_ble_adv(bool en);
void bt_drv_reg_op_fa_gain_direct_set(uint8_t gain_idx);
uint8_t bt_drv_reg_op_fa_gain_direct_get(void);
void bt_drv_reg_op_dgb_link_gain_ctrl_set(uint16_t connHandle, uint8_t bt_ble_mode, uint8_t gain_idx, uint8_t enable);
void bt_drv_reg_op_dgb_link_gain_ctrl_clear(uint16_t connHandle, uint8_t bt_ble_mode);
void bt_drv_reg_op_dgb_link_gain_ctrl_init(void);
void bt_drv_reg_op_rx_gain_fix(uint16_t connHandle, uint8_t bt_ble_mode, uint8_t gain_idx, uint8_t enable, uint8_t table_idx);
void bt_drv_reg_op_afh_follow_en(bool enable, uint8_t be_followed_link_id, uint8_t follow_link_id);
/// Index for DM1 packet type (used in both BR and EDR)
#define DM1_IDX         0
/// Enhanced Data Rate packet types indexes (classified by packet size)
#define DH1_2_IDX       1
#define DH1_3_IDX       2
#define DH3_2_IDX       3
#define DH3_3_IDX       4
#define DH5_2_IDX       5
#define DH5_3_IDX       6

#define USE_1_SLOT_PACKET       0x02
#define USE_3_SLOT_PACKET       0x04
#define USE_5_SLOT_PACKET       0x06

///Sniffer sco status
#define SNIFFER_SCO_STOP                  0
#define SNIFFER_SCO_START                 1
#define SNIFFER_SCO_RESTART               2
#define SNIFFER_SCO_RESUME_AFTER_TO       3
#define SNIFFER_ACL_DISCONNECT_STOP_SCO   4
#define SNIFFER_IBRT_START                5
#define SNIFFER_IBRT_STOP                 6
#define IDEL_CHECK                        0xff

#define SNIFF_SYNC_MODE    0
#define ACTIVE_SYNC_MODE   1

enum REG_OP
{
    READ_OP,
    WRITE_OP,
};

enum LC_ENV_TAG
{
    LC_LINK_TAG,
    LC_INFO_TAG,
    LC_ENC_TAG,
    LC_AFH_TAG,
    LC_SP_TAG,
    LC_EPR_TAG,
    LC_EPC_TAG,
};

void bt_drv_reg_op_hw_sw_agc_select(uint8_t agc_mode);
void bt_drv_reg_op_force_set_sniff_att(uint16_t conhdle);
void bt_drv_reg_op_voice_settings_set(uint16_t voice_settings);
void bt_drv_reg_op_host_buff_and_flow_ctl_set
(uint16_t acl_pkt_len, uint16_t acl_pkt_num,uint16_t sync_pkt_len, uint16_t sync_pkt_num,bool flow_ctl_en);

void bt_drv_reg_op_page_to_set(uint16_t page_to);
void bt_drv_reg_op_btdm_address_set(uint8_t* bt_addr, uint8_t* ble_addr);
void bt_drv_reg_op_evtmask_set(uint8_t ble_en);
void bt_drv_reg_op_simplepair_mode_set(uint8_t en);
void bt_drv_reg_op_class_of_dev_set(uint8_t* class_of_dev);
void bt_drv_reg_op_sleep_set(bool en);
void bt_drv_reg_op_sco_fifo_reset(void);
void bt_drv_reg_sco_fifo_dump(void);
uint8_t bt_drv_reg_op_get_sync_id_op(uint8_t op, uint8_t sync_id);
void bt_drv_reg_op_set_agc_thd(bool isMaster, bool isSco);
void bt_drv_reg_op_global_symbols_init(void);
void bt_drv_tx_pwr_init(void);
void bt_drv_tx_pwr_init_for_testmode(void);
void bt_drv_reg_op_set_ibrt_auto_accept_sco(bool en);
void bt_drv_reg_op_update_dbg_state(void);
void bt_drv_reg_op_set_ibrt_reject_sniff_req(bool en);
uint8_t bt_drv_reg_op_get_esco_nego_airmode(uint8_t sco_link_id);
void bt_drv_reg_op_hci_vender_ibrt_ll_monitor(uint8_t* ptr, uint16_t* p_sum_err,uint16_t* p_rx_total);
uint32_t bt_drv_reg_op_get_host_ref_clk(void);
void bt_drv_reg_op_set_accept_new_mobile_enable(void);
void bt_drv_reg_op_clear_accept_new_mobile_enable(void);
void bt_drv_reg_op_ecc_softbit_process(uint16_t conn_handle, uint16_t length, uint8_t *data);
void bt_drv_reg_op_ebq_test_setting(void);
void bt_drv_reg_op_hw_spi_en_setf(int elt_idx, uint8_t hwspien);
void bt_drv_enhance_fa_mode(bool enable);
void bt_drv_reg_op_set_rand_seed(uint32_t seed);
void bt_drv_reg_op_swagc_mode_set(uint8_t mode);
void bt_drv_reg_op_set_max_pwr_rcv(uint16_t connHandle);

#ifdef __cplusplus
}
#endif

#endif

