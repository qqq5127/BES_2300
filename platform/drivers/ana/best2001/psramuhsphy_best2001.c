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
#include "string.h"
#include "psramuhsphy.h"
#include "hal_cmu.h"
#include "hal_phyif.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include CHIP_SPECIFIC_HDR(reg_psramuhsphy)

enum PSRAM_ID_T {
    PSRAM_ID_0      = (1 << 0),
#ifdef PSRAMUHS_DUAL_8BIT
    PSRAM_ID_1      = (1 << 1),
#endif
    PSRAM_ID_ALL    = (3 << 0),
};

//#define PSRAMUHS_DEBUG
#ifdef PSRAMUHS_DEBUG
#define PSRAMUHS_TRACE TRACE_IMM
#else
#define PSRAMUHS_TRACE(...)
#endif

static void psramuhs_read(enum PSRAM_ID_T id, uint16_t reg, uint16_t *val)
{
#ifdef PSRAMUHS_DUAL_SWITCH
    if (id == PSRAM_ID_0) {
        psramuhsphy1_read(reg, val);
        return;
    }
#else /*PSRAMUHS_DUAL_SWITCH*/
    if (id == PSRAM_ID_0) {
        psramuhsphy0_read(reg, val);
        return;
    }
#ifdef PSRAMUHS_DUAL_8BIT
    if (id == PSRAM_ID_1) {
        psramuhsphy1_read(reg, val);
        return;
    }
#endif
#endif /*PSRAMUHS_DUAL_SWITCH*/
    ASSERT(false, "%s: Bad psram id: %d", __func__, id);
}

static void psramuhs_write(enum PSRAM_ID_T id, uint16_t reg, uint16_t val)
{
#ifdef PSRAMUHS_DUAL_SWITCH
    if (id & PSRAM_ID_0) {
        psramuhsphy1_write(reg, val);
    }
#else /*PSRAMUHS_DUAL_SWITCH*/
    if (id & PSRAM_ID_0) {
        psramuhsphy0_write(reg, val);
    }
#ifdef PSRAMUHS_DUAL_8BIT
    if (id & PSRAM_ID_1) {
        psramuhsphy1_write(reg, val);
    }
#endif
#endif /*PSRAMUHS_DUAL_SWITCH*/
}

static void psramuhs_dual_write(uint16_t reg, uint16_t val)
{
    psramuhs_write(PSRAM_ID_ALL, reg, val);
}

void psramuhsphy_open(uint32_t clk)
{
    int ret;
    uint16_t val;
    uint16_t range;
#ifdef PSRAMUHS_DUAL_8BIT
    const enum PSRAM_ID_T id[] = { PSRAM_ID_0, PSRAM_ID_1, };
#else
    const enum PSRAM_ID_T id[] = { PSRAM_ID_0, };
#endif

    ret = hal_phyif_open(SPIPHY_PSRAMUHS0_CS);
    if (ret) {
        return;
    }
    //clk = 200*1000*1000;

    if (clk <= 300000000 / 2) {
        range = 3;
    } else if (clk <= 500000000 / 2) {
        range = 2;
    } else if (clk <= 1066000000 / 2) {
        range = 1;
    } else {
        range = 0;
    }

    for (int i = 0; i < ARRAY_SIZE(id); i++) {
        psramuhs_read(id[i], PSRAMPHY_REG_03, &val);
        val = SET_BITFIELD(val, REG_DLL_RANGE, range);
        psramuhs_write(id[i], PSRAMPHY_REG_03, val);

        psramuhs_read(id[i], PSRAMPHY_REG_02, &val);
        val = SET_BITFIELD(val, REG_LDO_IEN2, 0xf);
        val = SET_BITFIELD(val, REG_LDO_IEN1, 0xf);

        //val = SET_BITFIELD(val, REG_LDO_VTUNE, 0x2);//1.15
        val = SET_BITFIELD(val, REG_LDO_VTUNE, 0x3);//1.2
        //val = SET_BITFIELD(val, REG_LDO_VTUNE, 0x4);//1.3
        //val = SET_BITFIELD(val, REG_LDO_VTUNE, 0x6);//1.35

        //val = SET_BITFIELD(val, REG_PSRAM_SWRC, 0x3);
        psramuhs_write(id[i], PSRAMPHY_REG_02, val);

        psramuhs_read(id[i], PSRAMPHY_REG_05, &val);
        val = SET_BITFIELD(val, REG_PSRAM_TXDRV, 0x7);
        val = SET_BITFIELD(val, REG_PSRAM_RX_BIAS, 0x3);
        psramuhs_write(id[i], PSRAMPHY_REG_05, val);
    }

    val = REG_LDO_PU;
    psramuhs_dual_write(PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(10);
    val |= REG_LDO_PRECHARGE;
    psramuhs_dual_write(PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(10);
    val &= ~REG_LDO_PRECHARGE;
    psramuhs_dual_write(PSRAMPHY_REG_01, val);
    val |= (REG_PSRAM_PU | REG_DLL_PU);
    psramuhs_dual_write(PSRAMPHY_REG_01, val);
    hal_sys_timer_delay_us(2);
    val |= REG_RESETB | REG_CLK_RDY;
    psramuhs_dual_write(PSRAMPHY_REG_01, val);
}

void psramuhsphy_init_calib(void)
{
    uint16_t val, dly_in, rx_dqs, tx_clk, tx_ceb, tx_dm, tx_dq, tx_cmd, tx_dqs, rx_adq;
#ifdef PSRAMUHS_DUAL_8BIT
    const enum PSRAM_ID_T id[] = { PSRAM_ID_0, PSRAM_ID_1, };
#else
    const enum PSRAM_ID_T id[] = { PSRAM_ID_0, };
#endif
    uint16_t dll_state;

    for (int i = 0; i < ARRAY_SIZE(id); i++) {
        do {
            psramuhs_read(id[i], PSRAMPHY_REG_03, &dll_state);
        } while ((dll_state & DLL_LOCK) == 0);

        dly_in = GET_BITFIELD(dll_state, DLL_DLY_IN);
        //ASSERT(dly_in < (DLL_DLY_IN_MASK >> DLL_DLY_IN_SHIFT), "[%d] Bad DLL_DLY_IN=0x%X reg=0x%04X", i, dly_in, dll_state[i]);

        tx_clk = dly_in / 2;
        tx_ceb = dly_in / 2;
        tx_dm = 0;
        tx_dq = 0;
        tx_cmd = dly_in / 2;
        tx_dqs = dly_in / 2;
        rx_adq = 0;
        rx_dqs = dly_in / 2;
        PSRAMUHS_TRACE(1,"init cali :T/4:0x%x", dly_in / 2);
        val = REG_PSRAM_TX_DM_DLY(tx_dm) | REG_PSRAM_TX_DQ_DLY_4_0(tx_dq) | REG_PSRAM_TX_DQ_DLY_9_5(tx_dq);
        psramuhs_write(id[i], PSRAMPHY_REG_06, val);
        val = REG_PSRAM_TX_DQ_DLY_14_10(tx_dq) | REG_PSRAM_TX_DQ_DLY_19_15(tx_dq) | REG_PSRAM_TX_DQ_DLY_24_20(tx_dq);
        psramuhs_write(id[i], PSRAMPHY_REG_07, val);
        val = REG_PSRAM_TX_DQ_DLY_29_25(tx_dq) | REG_PSRAM_TX_DQ_DLY_34_30(tx_dq) | REG_PSRAM_TX_DQ_DLY_39_35(tx_dq);
        psramuhs_write(id[i], PSRAMPHY_REG_08, val);
        val = REG_PSRAM_TX_CMD_DLY_4_0(tx_cmd) | REG_PSRAM_TX_CMD_DLY_9_5(tx_cmd) | REG_PSRAM_TX_CMD_DLY_14_10(tx_cmd);
        psramuhs_write(id[i], PSRAMPHY_REG_09, val);
        val = REG_PSRAM_TX_CMD_DLY_19_15(tx_cmd) | REG_PSRAM_TX_CMD_DLY_24_20(tx_cmd) | REG_PSRAM_TX_CMD_DLY_29_25(tx_cmd);
        psramuhs_write(id[i], PSRAMPHY_REG_0A, val);
        val = REG_PSRAM_TX_CMD_DLY_34_30(tx_cmd) | REG_PSRAM_TX_CMD_DLY_39_35(tx_cmd);
        psramuhs_write(id[i], PSRAMPHY_REG_0B, val);
        val = REG_PSRAM_TX_DQS_DLY(tx_dqs) | REG_PSRAM_RX_ADQ_DLY_4_0(rx_adq) | REG_PSRAM_RX_ADQ_DLY_9_5(rx_adq);
        psramuhs_write(id[i], PSRAMPHY_REG_0C, val);
        val = REG_PSRAM_RX_ADQ_DLY_14_10(rx_adq) | REG_PSRAM_RX_ADQ_DLY_19_15(rx_adq) | REG_PSRAM_RX_ADQ_DLY_24_20(rx_adq);
        psramuhs_write(id[i], PSRAMPHY_REG_0D, val);
        val = REG_PSRAM_RX_ADQ_DLY_29_25(rx_adq) | REG_PSRAM_RX_ADQ_DLY_34_30(rx_adq) | REG_PSRAM_RX_ADQ_DLY_39_35(rx_adq);
        psramuhs_write(id[i], PSRAMPHY_REG_0E, val);
        val = REG_PSRAM_RX_DQS_DLY(rx_dqs) | REG_PSRAM_TX_CLK_DLY(tx_clk) | REG_PSRAM_TX_CEB_DLY(tx_ceb);
        psramuhs_write(id[i], PSRAMPHY_REG_0F, val);
    }
}
static bool psramuhsphy_check_read_valid(enum PSRAM_ID_T id)
{
    uint32_t mr32, mr40;
    psramuhs_read_reg(32, &mr32);//decimalism 32
    psramuhs_read_reg(40, &mr40);//decimalism 40

    PSRAMUHS_TRACE(3,"%s, mr32:0x%x, mr40:0x%x", __FUNCTION__, mr32, mr40);
#ifdef PSRAMUHS_DUAL_8BIT
#define DIE0_MR_MASK 0x00ff00ff
#define DIE1_MR_MASK 0xff00ff00
#define DIE0_MR32_VAL 0x000000ff
#define DIE1_MR32_VAL 0x0000ff00
#define DIE0_MR40_VAL 0x00000000
#define DIE1_MR40_VAL 0x00000000
    if (id == PSRAM_ID_0) {
        if ((mr32&DIE0_MR_MASK)==DIE0_MR32_VAL && (mr40&DIE0_MR_MASK)==DIE0_MR40_VAL)
            return true;
    } else if (id == PSRAM_ID_1) {
        if ((mr32&DIE1_MR_MASK)==DIE1_MR32_VAL && (mr40&DIE1_MR_MASK)==DIE1_MR40_VAL)
            return true;
    } else {
        ASSERT(false, "cat calibrate dual die at same time");
    }
#else
    if (mr32 == 0x00ff00ff && mr40 == 0xffff0000)
        return true;
#endif
    return false;

}
static void psramuhsphy_read_calib_bank(enum PSRAM_ID_T id, uint16_t range)
{
    uint16_t val_01, val, dly_in, rx_dqs, tx_clk, tx_ceb, tx_dm, tx_dq, tx_cmd, tx_dqs, rx_adq;
    uint16_t dll_state;

    PSRAMUHS_TRACE(0,"\n####\n");
    ASSERT(range <= (REG_DLL_RANGE_MASK>>REG_DLL_RANGE_SHIFT), "ERROR, bad ana phy range:%d", range);

    psramuhs_read(id, PSRAMPHY_REG_01, &val_01);
    val_01 &= ~(REG_RESETB | REG_CLK_RDY);
    psramuhs_write(id, PSRAMPHY_REG_01, val_01);

    psramuhs_read(id, PSRAMPHY_REG_03, &val);
    val = SET_BITFIELD(val, REG_DLL_RANGE, range);
    psramuhs_write(id, PSRAMPHY_REG_03, val);

    val_01 |= (REG_RESETB | REG_CLK_RDY);
    psramuhs_write(id, PSRAMPHY_REG_01, val_01);

    do {
        psramuhs_read(id, PSRAMPHY_REG_03, &dll_state);
    } while ((dll_state & DLL_LOCK) == 0);

    dly_in = GET_BITFIELD(dll_state, DLL_DLY_IN);
    if (dly_in == (DLL_DLY_IN_MASK >> DLL_DLY_IN_SHIFT)) {
        PSRAMUHS_TRACE(3,"%s, range:0x%x,  dly_in = 0x%x", __FUNCTION__, range, dly_in);
        return psramuhsphy_read_calib_bank(id, range+1);
    }

    tx_clk = dly_in / 2;
    tx_ceb = dly_in / 2;
    tx_dm = 0;
    tx_dq = 0;
    tx_cmd = dly_in / 2;
    tx_dqs = dly_in / 2;
    rx_adq = 0;

    PSRAMUHS_TRACE(3,"%s, range:0x%x,  T/4 = 0x%x", __FUNCTION__, range, dly_in / 2);
re_cali:
    val = REG_PSRAM_TX_DM_DLY(tx_dm) | REG_PSRAM_TX_DQ_DLY_4_0(tx_dq) | REG_PSRAM_TX_DQ_DLY_9_5(tx_dq);
    psramuhs_write(id, PSRAMPHY_REG_06, val);
    val = REG_PSRAM_TX_DQ_DLY_14_10(tx_dq) | REG_PSRAM_TX_DQ_DLY_19_15(tx_dq) | REG_PSRAM_TX_DQ_DLY_24_20(tx_dq);
    psramuhs_write(id, PSRAMPHY_REG_07, val);
    val = REG_PSRAM_TX_DQ_DLY_29_25(tx_dq) | REG_PSRAM_TX_DQ_DLY_34_30(tx_dq) | REG_PSRAM_TX_DQ_DLY_39_35(tx_dq);
    psramuhs_write(id, PSRAMPHY_REG_08, val);
    val = REG_PSRAM_TX_CMD_DLY_4_0(tx_cmd) | REG_PSRAM_TX_CMD_DLY_9_5(tx_cmd) | REG_PSRAM_TX_CMD_DLY_14_10(tx_cmd);
    psramuhs_write(id, PSRAMPHY_REG_09, val);
    val = REG_PSRAM_TX_CMD_DLY_19_15(tx_cmd) | REG_PSRAM_TX_CMD_DLY_24_20(tx_cmd) | REG_PSRAM_TX_CMD_DLY_29_25(tx_cmd);
    psramuhs_write(id, PSRAMPHY_REG_0A, val);
    val = REG_PSRAM_TX_CMD_DLY_34_30(tx_cmd) | REG_PSRAM_TX_CMD_DLY_39_35(tx_cmd);
    psramuhs_write(id, PSRAMPHY_REG_0B, val);
    val = REG_PSRAM_TX_DQS_DLY(tx_dqs) | REG_PSRAM_RX_ADQ_DLY_4_0(rx_adq) | REG_PSRAM_RX_ADQ_DLY_9_5(rx_adq);
    psramuhs_write(id, PSRAMPHY_REG_0C, val);
    val = REG_PSRAM_RX_ADQ_DLY_14_10(rx_adq) | REG_PSRAM_RX_ADQ_DLY_19_15(rx_adq) | REG_PSRAM_RX_ADQ_DLY_24_20(rx_adq);
    psramuhs_write(id, PSRAMPHY_REG_0D, val);
    val = REG_PSRAM_RX_ADQ_DLY_29_25(rx_adq) | REG_PSRAM_RX_ADQ_DLY_34_30(rx_adq) | REG_PSRAM_RX_ADQ_DLY_39_35(rx_adq);
    psramuhs_write(id, PSRAMPHY_REG_0E, val);
    val = REG_PSRAM_TX_CLK_DLY(tx_clk) | REG_PSRAM_TX_CEB_DLY(tx_ceb);
    psramuhs_write(id, PSRAMPHY_REG_0F, val);

    int8_t left_bound = -1;
    int8_t right_bound = -1;
    bool valid_array[(REG_PSRAM_RX_DQS_DLY_MASK>>REG_PSRAM_RX_DQS_DLY_SHIFT)+1];
    memset(valid_array, 0, sizeof(valid_array));
    for (rx_dqs=0; rx_dqs<=dly_in && rx_dqs<=(REG_PSRAM_RX_DQS_DLY_MASK>>REG_PSRAM_RX_DQS_DLY_SHIFT); ++rx_dqs) {
        bool valid;
        val = SET_BITFIELD(val, REG_PSRAM_RX_DQS_DLY, rx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0F, val);
        valid = psramuhsphy_check_read_valid(id);
        PSRAMUHS_TRACE(3,"%s, rx_dqs:0x%x, valid:%d", __FUNCTION__, rx_dqs, valid);
        valid_array[rx_dqs] = valid;
        if (rx_dqs == 0) {
            if (valid) {
                if (rx_adq <= dly_in) {
                    rx_adq ++;
                    PSRAMUHS_TRACE(1,"left boud is valid, increase rx_adq(0x%x)", rx_adq);
                    goto re_cali;
                } else {
                    left_bound = rx_dqs;
                    PSRAMUHS_TRACE(1,"left_bound:0x%x", left_bound);
                }
                //return psramuhsphy_read_calib_bank(id, range+1);
            }
        } else if (rx_dqs==dly_in) {
            if (valid) {
                right_bound = rx_dqs;//find right bound
                PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
            } else {
                if (valid_array[rx_dqs-1]) {
                    right_bound = rx_dqs-1;//find right bound
                    PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
                }
            }
        } else if (rx_dqs == REG_PSRAM_RX_DQS_DLY_MASK>>REG_PSRAM_RX_DQS_DLY_SHIFT) {
            if (valid) {
                if (range < (REG_DLL_RANGE_MASK>>REG_DLL_RANGE_SHIFT)) {
                    return psramuhsphy_read_calib_bank(id, range+1);
                } else {
                    right_bound = rx_dqs;//find right bound
                    PSRAMUHS_TRACE(1, "right_bound:0x%x", right_bound);
                }
            } else {
                if (valid_array[rx_dqs-1]) {
                    right_bound = rx_dqs-1;//find right bound
                    PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
                }
            }
        } else if (valid && !valid_array[rx_dqs-1]) {
            left_bound = rx_dqs;//find left bound
            PSRAMUHS_TRACE(1,"left_bound:0x%x", left_bound);
        } else if (!valid && valid_array[rx_dqs-1]) {
            right_bound = rx_dqs-1;//find right bound
            PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
            break;
        }
    }
    if (left_bound!=-1 && right_bound!=-1 && (right_bound-left_bound>1)) {
        rx_dqs = (left_bound+right_bound)/2;
        PSRAMUHS_TRACE(4,"cali done, rx_dqs:0x%x(0x%x-0x%x), range:0x%x", rx_dqs, left_bound, right_bound, range);
        if (rx_dqs > rx_adq) {
            rx_dqs = rx_dqs - rx_adq;
            rx_adq = 0;
        } else {
            rx_adq = rx_adq - rx_dqs;
            rx_dqs = 0;
        }
        PSRAMUHS_TRACE(2,"cali done, rx_dqs:0x%x, rx_adq:0x%x", rx_dqs, rx_adq);
        val = SET_BITFIELD(val, REG_PSRAM_RX_DQS_DLY, rx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0F, val);
        val = REG_PSRAM_TX_DQS_DLY(tx_dqs) | REG_PSRAM_RX_ADQ_DLY_4_0(rx_adq) | REG_PSRAM_RX_ADQ_DLY_9_5(rx_adq);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);
        val = REG_PSRAM_RX_ADQ_DLY_14_10(rx_adq) | REG_PSRAM_RX_ADQ_DLY_19_15(rx_adq) | REG_PSRAM_RX_ADQ_DLY_24_20(rx_adq);
        psramuhs_write(id, PSRAMPHY_REG_0D, val);
        val = REG_PSRAM_RX_ADQ_DLY_29_25(rx_adq) | REG_PSRAM_RX_ADQ_DLY_34_30(rx_adq) | REG_PSRAM_RX_ADQ_DLY_39_35(rx_adq);
        psramuhs_write(id, PSRAMPHY_REG_0E, val);
    } else {
        ///TODO:
        ASSERT(false, "ERROR, read need do more cali");
    }
}
static void psramuhsphy_read_calib(uint32_t clk)
{
    uint16_t range;

    if (clk <= 300000000 / 2) {
        range = 3;
    } else if (clk <= 500000000 / 2) {
        range = 2;
    } else if (clk <= 1066000000 / 2) {
        range = 1;
    } else {
        range = 0;
    }
    PSRAMUHS_TRACE(3,"%s, clk:%d, range:%d", __FUNCTION__, clk, range);
    psramuhsphy_read_calib_bank(PSRAM_ID_0, range);
#ifdef PSRAMUHS_DUAL_8BIT
    psramuhsphy_read_calib_bank(PSRAM_ID_1, range);
#endif
}
static bool psramuhsphy_check_write_valid()
{
    int i;
    volatile uint32_t *psram_base = (volatile uint32_t *)PSRAMUHS_NC_BASE;
    volatile uint32_t *psram_base1 = psram_base + 0x40000;//offset 1MB
    for (i=0; i<0x32; ++i) {
        *(psram_base+i) = 0xffffffff;
        *(psram_base1+i) = 0xffffffff;
    }
    for (i=0; i<0x32; ++i) {
        *(psram_base+i) = ((i << 0) | (i << 8) | (i << 16) | (i << 24));
    }
    for (i=0; i<0x32; ++i) {
        *(psram_base1+i) = ((i << 0) | (i << 8) | (i << 16) | (i << 24));
    }
    hal_psramuhsip_wb_busy_wait();
    hal_psramuhsip_mc_busy_wait();
    for (i=0; i<0x32; ++i) {
        if (*(psram_base+i) != ((i << 0) | (i << 8) | (i << 16) | (i << 24)))
            return false;
        if (*(psram_base1+i) != ((i << 0) | (i << 8) | (i << 16) | (i << 24)))
            return false;
    }
    return true;
}
static void psramuhsphy_write_calib_bank(enum PSRAM_ID_T id)
{
    uint16_t val, val_0f;
    uint16_t dly_in, tx_dqs, tx_ceb, tx_clk;

    psramuhs_read(id, PSRAMPHY_REG_03, &val);
    dly_in = GET_BITFIELD(val, DLL_DLY_IN);
    ASSERT(dly_in < (DLL_DLY_IN_MASK >> DLL_DLY_IN_SHIFT), "[%d] Bad DLL_DLY_IN=0x%X reg=0x%04X", id, dly_in, val);

    PSRAMUHS_TRACE(2,"%s, T/4 = 0x%x", __FUNCTION__, dly_in / 2);

    psramuhs_read(id, PSRAMPHY_REG_0C, &val);

    tx_clk = dly_in / 2;
    tx_ceb = dly_in / 2;


    psramuhs_read(id, PSRAMPHY_REG_0F, &val_0f);
    tx_clk = GET_BITFIELD(val_0f, REG_PSRAM_TX_CLK_DLY);
    tx_ceb = GET_BITFIELD(val_0f, REG_PSRAM_TX_CEB_DLY);

    int8_t left_bound = -1;
    int8_t right_bound = -1;
    bool valid_array[(REG_PSRAM_TX_DQS_DLY_MASK>>REG_PSRAM_TX_DQS_DLY_SHIFT)+1];
    memset(valid_array, 0, sizeof(valid_array));
    for (tx_dqs=0; tx_dqs<=dly_in && tx_dqs<=(REG_PSRAM_TX_DQS_DLY_MASK>>REG_PSRAM_TX_DQS_DLY_SHIFT); ++tx_dqs) {
        bool valid;

        val = SET_BITFIELD(val, REG_PSRAM_TX_DQS_DLY, tx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);

        tx_clk = tx_dqs;
        tx_ceb = tx_dqs;
        val_0f = SET_BITFIELD(val_0f, REG_PSRAM_TX_CLK_DLY, tx_clk);
        val_0f = SET_BITFIELD(val_0f, REG_PSRAM_TX_CEB_DLY, tx_ceb);
        psramuhs_write(id, PSRAMPHY_REG_0F, val_0f);

        valid = psramuhsphy_check_write_valid();
        PSRAMUHS_TRACE(3,"%s, tx_dqs:0x%x, valid:%d", __FUNCTION__, tx_dqs, valid);
        valid_array[tx_dqs] = valid;
        if (tx_dqs == 0) {
            if (valid) {
                left_bound = tx_dqs;
                PSRAMUHS_TRACE(1,"left_bound:0x%x", left_bound);
            }
        } else if (tx_dqs==dly_in || tx_dqs == REG_PSRAM_TX_DQS_DLY_MASK>>REG_PSRAM_TX_DQS_DLY_SHIFT) {
            if (valid) {
                right_bound = tx_dqs;//find right bound
                PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
            } else {
                if (valid_array[tx_dqs-1]) {
                    right_bound = tx_dqs-1;//find right bound
                    PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
                }
            }
        } else {
            if (valid && !valid_array[tx_dqs-1]) {
                left_bound = tx_dqs;//find left bound
                PSRAMUHS_TRACE(1,"left_bound:0x%x", left_bound);
            } else if (!valid && valid_array[tx_dqs-1]) {
                right_bound = tx_dqs-1;//find right bound
                PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
                break;
            }
        }
    }
    if (left_bound!=-1 && right_bound!=-1 && (right_bound-left_bound>1)) {
        tx_dqs = (left_bound+right_bound)/2;

        val = SET_BITFIELD(val, REG_PSRAM_TX_DQS_DLY, tx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);

        tx_clk = tx_dqs;
        tx_ceb = tx_dqs;
        val_0f = SET_BITFIELD(val_0f, REG_PSRAM_TX_CLK_DLY, tx_clk);
        val_0f = SET_BITFIELD(val_0f, REG_PSRAM_TX_CEB_DLY, tx_ceb);
        psramuhs_write(id, PSRAMPHY_REG_0F, val_0f);
        PSRAMUHS_TRACE(3,"cali done, tx_dqs:0x%x(0x%x-0x%x)", tx_dqs, left_bound, right_bound);
    } else {
        ///TODO:, range+=1, recalib read and write
        ASSERT(false, "ERROR, write need do more cali");
    }

    left_bound = -1;
    right_bound = -1;
    memset(valid_array, 0, sizeof(valid_array));
    for (tx_dqs=0; tx_dqs<=dly_in && tx_dqs<=(REG_PSRAM_TX_DQS_DLY_MASK>>REG_PSRAM_TX_DQS_DLY_SHIFT); ++tx_dqs) {
        bool valid;

        val = SET_BITFIELD(val, REG_PSRAM_TX_DQS_DLY, tx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);

        valid = psramuhsphy_check_write_valid();
        PSRAMUHS_TRACE(3,"%s, tx_dqs:0x%x, valid:%d", __FUNCTION__, tx_dqs, valid);
        valid_array[tx_dqs] = valid;
        if (tx_dqs == 0) {
            if (valid) {
                left_bound = tx_dqs;
                PSRAMUHS_TRACE(1,"left_bound:0x%x", left_bound);
            }
        } else if (tx_dqs==dly_in || tx_dqs == REG_PSRAM_TX_DQS_DLY_MASK>>REG_PSRAM_TX_DQS_DLY_SHIFT) {
            if (valid) {
                right_bound = tx_dqs;//find right bound
                PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
            } else {
                if (valid_array[tx_dqs-1]) {
                    right_bound = tx_dqs-1;//find right bound
                    PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
                }
            }
        } else {
            if (valid && !valid_array[tx_dqs-1]) {
                left_bound = tx_dqs;//find left bound
                PSRAMUHS_TRACE(1,"left_bound:0x%x", left_bound);
            } else if (!valid && valid_array[tx_dqs-1]) {
                right_bound = tx_dqs-1;//find right bound
                PSRAMUHS_TRACE(1,"right_bound:0x%x", right_bound);
                break;
            }
        }
    }
    if (left_bound!=-1 && right_bound!=-1 && (right_bound-left_bound>1)) {
        tx_dqs = (left_bound+right_bound)/2;
        val = SET_BITFIELD(val, REG_PSRAM_TX_DQS_DLY, tx_dqs);
        psramuhs_write(id, PSRAMPHY_REG_0C, val);
        PSRAMUHS_TRACE(3,"cali done, tx_dqs:0x%x(0x%x-0x%x)", tx_dqs, left_bound, right_bound);
    } else {
        ///TODO:, range+=1, recalib read and write
        ASSERT(false, "ERROR, write need do more cali");
    }

}

static void psramuhsphy_write_calib()
{
    PSRAMUHS_TRACE(1,"%s", __FUNCTION__);
    psramuhsphy_write_calib_bank(PSRAM_ID_0);
#ifdef PSRAMUHS_DUAL_8BIT
    psramuhsphy_write_calib_bank(PSRAM_ID_1);
#endif
}
void psramuhsphy_calib(uint32_t clk)
{
    psramuhsphy_read_calib(clk);
    psramuhsphy_write_calib();
}
void psramuhsphy_close(void)
{
    hal_phyif_close(SPIPHY_PSRAMUHS0_CS);
}

void psramuhsphy_sleep(void)
{
}

void psramuhsphy_wakeup(void)
{
}

