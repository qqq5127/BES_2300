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
#include "string.h"
#include "plat_types.h"
#include "plat_addr_map.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_intersys.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "hal_iomux.h"
#include "pmu.h"
#include "nvrecord_dev.h"
#include "bt_drv.h"
#include "bt_drv_internal.h"
#include "bt_drv_interface.h"
#include "bt_drv_reg_op.h"
#include "hal_intersys.h"

void btdrv_hciopen(void)
{
}

////open intersys interface for hci data transfer
void btdrv_hcioff(void)
{
}

/*  btdrv power on or off the bt controller*/
void btdrv_poweron(uint8_t en)
{
}

void btdrv_rf_init_ext(void)
{
}

void bt_drv_extra_config_after_init(void)
{
}

///start active bt controller
void btdrv_start_bt(void)
{
}

void btdrv_testmode_start(void)
{
}

void btdrv_write_localinfo(char *name, uint8_t len, uint8_t *addr)
{
}

void btdrv_enable_dut(void)
{
}

void btdrv_enable_autoconnector(void)
{
}

void btdrv_enable_nonsig_tx(uint8_t index)
{
}

void btdrv_enable_nonsig_rx(uint8_t index)
{
}

void btdrv_vco_test_start(uint8_t chnl)
{
}

void btdrv_vco_test_stop(void)
{
}

void btdrv_stop_bt(void)
{
}

void btdrv_SendData(const uint8_t *buff,uint8_t len)
{

    hal_intersys_send(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, buff, len);
    BT_DRV_TRACE(1,"%s", __func__);
    BT_DRV_DUMP("%02x ", buff, len);
    btdrv_delay(1);
}

extern "C" void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length)
{
    uint8_t buff[256];
    if(length ==0 || length >128)
        return;
    buff[0] = 0x01;
    buff[1] = 0x02;
    buff[2] = 0xfc;
    buff[3] = length + 6;
    buff[4] = address & 0xff;
    buff[5] = (address &0xff00)>>8;
    buff[6] = (address &0xff0000)>>16;
    buff[7] = address>>24;
    buff[8] = wr_type;
    buff[9] = length;
    memcpy(&buff[10],value,length);
    btdrv_SendData(buff,length+10);
    btdrv_delay(2);


}

const uint8_t hci_cmd_hci_reset[] =
{
    0x01,0x03,0x0c,0x00
};

void btdrv_hci_reset(void)
{
    btdrv_SendData(hci_cmd_hci_reset, sizeof(hci_cmd_hci_reset));
    btdrv_delay(10);
}

const uint8_t hci_cmd_hci_patch_init[] =
{
    0x01,0xB8,0xFc,0x00
};

void btdrv_hci_send_patch_init_cmd(void)
{
    btdrv_SendData(hci_cmd_hci_patch_init, sizeof(hci_cmd_hci_patch_init));
    btdrv_delay(100);
}

extern "C" void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param)
{
}

void btdrv_rxdpd_sample_init(void)
{
}

void btdrv_rxdpd_sample_deinit(void)
{
}

void btdrv_rxdpd_sample_init_tx(void)
{

}

void btdrv_enable_dual_slave_configurable_slot_mode(bool isEnable,
        uint16_t activeDevHandle, uint8_t activeDevRole,
        uint16_t idleDevHandle, uint8_t idleDevRole)
{
    // TODO
}

void btdrv_rxdpd_sample_enable(uint8_t rxon, uint8_t txon)
{
}
void btdrv_btcore_extwakeup_irq_enable(bool on)
{
}

void btdrv_set_bt_pcm_triggler_delay(uint8_t  delay)
{
    if(delay > 0x3f)
    {
        BT_DRV_TRACE(0,"delay is error value");
        return;
    }
    BT_DRV_TRACE(1,"0XD0224024=%x",BTDIGITAL_REG(0xd0224024));
    BTDIGITAL_REG(0xd0224024) &= ~0x3f00;
    BTDIGITAL_REG(0xd0224024) |= (delay<<8);
    BT_DRV_TRACE(1,"exit :0XD0224024=%x",BTDIGITAL_REG(0xd0224024));


}
void btdrv_set_powerctrl_rssi_low(uint16_t rssi)
{
}
#if defined(TX_RX_PCM_MASK)
uint8_t  btdrv_is_pcm_mask_enable(void)
{
    return 0;
}
#endif
//below only for FPGA

//[26:0] 0x07ffffff
//[27:0] 0x0fffffff
uint32_t btdrv_syn_get_curr_ticks(void)
{
    uint32_t value;
#if defined( __FPGA_BT_2000__)
    value = BTDIGITAL_REG(0xd0220020) & 0x07ffffff;
    return value * 2;
#elif defined(__FPGA_BT_2300__)
    value = BTDIGITAL_REG(0xd022001C) & 0x0fffffff;
#elif defined(__FPGA_BT_1400__)
    value = BTDIGITAL_REG(0xd0220490) & 0x0fffffff;
#elif defined(__FPGA_BT_1500__)
    value = BTDIGITAL_REG(0xd0220e04) & 0x0fffffff;
    //BT_DRV_TRACE(1,"get curr ticks:%d\n",value);
#else
    value = 0;
#endif
    return value;
}

char app_tws_mode_is_master(void);
// offset btclk = Master - Slave
static int32_t btdrv_syn_get_offset_ticks(uint16_t conidx)
{
    int32_t offset;

#if defined( __FPGA_BT_2000__)
    offset = BTDIGITAL_REG(0xd02101c0+conidx*96) & 0x07ffffff;
    offset = (offset << 5)>>5;
    if(offset)
    {
        offset -= 1;    // Slave
        return offset *2;
    }
    else
    {
        return 0;       //Master
    }
#elif defined(__FPGA_BT_1400__)
    uint32_t local_offset;
    uint16_t offset0;
    uint16_t offset1;
    offset0 = BTDIGITAL_BT_EM(EM_BT_CLKOFF0_ADDR + conidx*BT_EM_SIZE);
    offset1 = BTDIGITAL_BT_EM(EM_BT_CLKOFF1_ADDR + conidx*BT_EM_SIZE);

    local_offset = (offset0 | offset1 << 16) & 0x07ffffff;
    offset = local_offset;
    offset = (offset << 5)>>5;
    BT_DRV_TRACE(5,"conidx:%d,offset0:%x,offset1:%x,offset:%x,(offset*2):%x\n",conidx,offset0,offset1,offset,(offset*2));
    if (offset)
    {
        return offset*2;
    }
    else
    {
        return 0;
    }
#elif defined(__FPGA_BT_1500__)
    uint32_t local_offset;
    uint16_t offset0;
    uint16_t offset1;
    offset0 = BTDIGITAL_BT_EM(0xd0213000 + 0x2 + conidx*104);
    offset1 = BTDIGITAL_BT_EM(0xd0213000 + 0x4 + conidx*104);

    local_offset = (offset0 | offset1 << 16) & 0x0fffffff;
    offset = local_offset;
    offset = (offset << 5)>>5;
    BT_DRV_TRACE(5,"conidx:%d,offset0:%x,offset1:%x,offset(x):%x,offset(d):%d,\n",conidx,offset0,offset1,offset,offset);
    if (offset){
        return offset;
    }else{
        return 0;
    }
#elif defined(__FPGA_BT_2300__)
    uint32_t local_offset;
    uint16_t offset0;
    uint16_t offset1;
    offset0 = BTDIGITAL_BT_EM(0xd0211282 + conidx*110);
    offset1 = BTDIGITAL_BT_EM(0xd0211284 + conidx*110);

    local_offset = (offset0 | offset1 << 16) & 0x07ffffff;
    offset = local_offset;
    offset = (offset << 5)>>5;

    if (offset)
    {
        return offset*2;
    }
    else
    {
        return 0;
    }
#else
    offset = 0;
    return offset;
#endif
}

// Clear trigger signal with software
void  btdrv_syn_clr_trigger(void)
{
#if defined( __FPGA_BT_2000__)
    BTDIGITAL_REG(0xd022007c) = BTDIGITAL_REG(0xd022007c) | (1<<31);
#elif defined(__FPGA_BT_1500__)
    //BTDIGITAL_REG(0xd0220c7c) = BTDIGITAL_REG(0xd0220c7c) | (1<<1);
    BTDIGITAL_REG(0xd0220c18) |= 0x1;
#else
    BTDIGITAL_REG(0xd02201f0) = BTDIGITAL_REG(0xd02201f0) | (1<<31);
    BT_DRV_TRACE(1,"btdrv_syn_clr_trigger 0xd02201f0:%x\n",*(volatile uint32_t *)(0xd02201f0));
#endif
}

static void btdrv_syn_set_tg_ticks(uint32_t num, uint8_t mode)
{
//#ifdef __TWS__
    if (mode == BT_TRIG_MASTER_ROLE)
    {
#if defined( __FPGA_BT_2000__)
        BTDIGITAL_REG(0xd022007c) = (BTDIGITAL_REG(0xd022007c) & 0x70000000) | (num & 0x0fffffff) | 0x60000000;
#elif defined(__FPGA_BT_1400__)
        BTDIGITAL_REG(0xd02204a4) = 0x80000006;
        BTDIGITAL_REG(0xd02201f0) = (BTDIGITAL_REG(0xd02201f0) & 0x70000000) | (num & 0x0fffffff) | 0x10000000;
        BT_DRV_TRACE(1,"master 0xd02201f0:%x\n",*(volatile uint32_t *)(0xd02201f0));
#elif defined(__FPGA_BT_2300__)
        BTDIGITAL_REG(0xd02201f0) = (BTDIGITAL_REG(0xd02201f0) & 0x70000000) | (num & 0x0fffffff) | 0x10000000;
        BT_DRV_TRACE(1,"master 0xd02201f0:%x\n",*(volatile uint32_t *)(0xd02201f0));
#elif defined(__FPGA_BT_1500__)
        BTDIGITAL_REG(0xd0220c84) = (BTDIGITAL_REG(0xd0220c84) & 0xf0000000) | (num & 0x0fffffff);
        BTDIGITAL_REG(0xd0220c7c) = (BTDIGITAL_REG(0xd0220c7c) & 0xfffffffd) | (1<<2) | (0x1);
        BTDIGITAL_REG(0xd0220c18) = (BTDIGITAL_REG(0xd0220c18) & 0xFFFFFFFE);
        BT_DRV_TRACE(2,"master 0xd0220c84:%x,0xd0220c7c:%x\n",*(volatile uint32_t *)(0xd0220c84),*(volatile uint32_t *)(0xd0220c7c));
#endif
    }
    else
//#endif
    {
#if defined( __FPGA_BT_2000__)
        BTDIGITAL_REG(0xd022007c) = (BTDIGITAL_REG(0xd022007c) & 0x70000000) | (num & 0x0fffffff);
#elif defined(__FPGA_BT_1400__)
        BTDIGITAL_REG(0xd02204a4) = 0x80000006;
        BTDIGITAL_REG(0xd02201f0) = (BTDIGITAL_REG(0xd02201f0) & 0x60000000) | (num & 0x0fffffff);
        BT_DRV_TRACE(1,"slave 0xd02201f0:%x\n",*(volatile uint32_t *)(0xd02201f0));
#elif defined(__FPGA_BT_2300__)
        BTDIGITAL_REG(0xd02201f0) = (BTDIGITAL_REG(0xd02201f0) & 0x70000000) | (num & 0x0fffffff);
        BT_DRV_TRACE(1,"slave 0xd02201f0:%x\n",*(volatile uint32_t *)(0xd02201f0));
#elif defined(__FPGA_BT_1500__)
        BTDIGITAL_REG(0xd0220c84) = (BTDIGITAL_REG(0xd0220c84) & 0xf0000000) | (num & 0x0fffffff);
        BTDIGITAL_REG(0xd0220c7c) = (BTDIGITAL_REG(0xd0220c7c) & 0xfffffffd) | (0x1);
        BTDIGITAL_REG(0xd0220c18) = (BTDIGITAL_REG(0xd0220c18) & 0xFFFFFFFE);
        BT_DRV_TRACE(2,"slave 0xd0220c84:%x,0xd0220c7c:%x\n",*(volatile uint32_t *)(0xd0220c84),*(volatile uint32_t *)(0xd0220c7c));
#endif
    }
}

void btdrv_syn_trigger_codec_en(uint32_t v)
{
}

uint32_t btdrv_get_syn_trigger_codec_en(void)
{
#if defined( __FPGA_BT_2000__)
    return BTDIGITAL_REG(0xd02201b4);
#elif defined(__FPGA_BT_1500__)
    return BTDIGITAL_REG(0xd0220c7c);
#else
    return BTDIGITAL_REG(0xd02201f0);
#endif

}

uint32_t btdrv_get_trigger_ticks(void)
{
#if defined( __FPGA_BT_2000__)
    return BTDIGITAL_REG(0xd022007c);
#elif defined(__FPGA_BT_1500__)
    return BTDIGITAL_REG(0xd0220c7c);
#else
    return BTDIGITAL_REG(0xd02201f0);
#endif

}

// Can be used by master or slave
// Ref: Master bt clk
uint32_t bt_syn_get_curr_ticks(uint16_t conhdl)
{
    int32_t curr,offset;

    curr = btdrv_syn_get_curr_ticks();
    if(conhdl>=0x80)
        offset = btdrv_syn_get_offset_ticks(conhdl-0x80);
    else
        offset = 0;

    BT_DRV_TRACE(4,"[%s] curr(%d) + offset(%d) = %d", __func__, curr, offset,curr + offset);
    BT_DRV_TRACE(1,"conhdl:%x\n",conhdl);

#if defined( __FPGA_BT_2000__) || defined(__FPGA_BT_1500__)
    return (curr + offset) & 0x0fffffff;
#else
    return (curr + offset) & 0x1fffffff;
#endif

}
void bt_syn_set_tg_ticks(uint32_t val,uint16_t conhdl, uint8_t mode)
{
    int32_t offset;
    if(conhdl>=0x80)
        offset = btdrv_syn_get_offset_ticks(conhdl-0x80);
    else
        offset = 0;

    //if ((mode == BT_TRIG_MASTER_ROLE) && (offset !=0))
    //    BT_DRV_TRACE(0,"ERROR OFFSET !!");

    //BT_DRV_TRACE(3,"bt_syn_set_tg_ticks conhdl:%x,val:%d,offset:%d\n",conhdl,val,offset);
    //btdrv_syn_set_tg_ticks(val - offset);
#ifndef __FPGA_BT_1500__
    if(conhdl==0x80)
    {
        BTDIGITAL_REG(0xd0220498)=(BTDIGITAL_REG(0xd0220498)&0xfffffff0)|0x1;
    }
    else if(conhdl==0x81)
    {
        BTDIGITAL_REG(0xd0220498)=(BTDIGITAL_REG(0xd0220498)&0xfffffff0)|0x2;
    }
    else if(conhdl==0x82)
    {
        BTDIGITAL_REG(0xd0220498)=(BTDIGITAL_REG(0xd0220498)&0xfffffff0)|0x3;
    }
#endif

    if ((mode == BT_TRIG_MASTER_ROLE) && (offset !=0))
        BT_DRV_TRACE(0,"ERROR OFFSET !!");

    val = val>>1;
    val = val<<1;
    val += 1;

    BT_DRV_TRACE(5,"bt_syn_set_tg_ticks val:%d offset:%d num:%d mode:%d conhdl:%02x", val, offset, val - offset, mode, conhdl);
    btdrv_syn_set_tg_ticks(val - offset, mode);
    bt_syn_trig_checker(conhdl);

}
void btdrv_enable_playback_triggler(uint8_t triggle_mode)
{
#if defined( __FPGA_BT_2000__)
    if(triggle_mode == ACL_TRIGGLE_MODE)
    {
        BTDIGITAL_REG(0xd02201cc) &= (~0x4000000);
    }
    else if(triggle_mode == SCO_TRIGGLE_MODE)
    {
        BTDIGITAL_REG(0xd02201cc) |= 0x4000000;
    }
#elif defined(__FPGA_BT_1500__)
    if(triggle_mode == ACL_TRIGGLE_MODE)
    {
        BTDIGITAL_REG(0xd0220c7c) |= (1<<3);
    }
    else if(triggle_mode == SCO_TRIGGLE_MODE)
    {
        BTDIGITAL_REG(0xd0220c7c) |= (1<<4);
    }

#else
    if(triggle_mode == ACL_TRIGGLE_MODE)
    {
        //clear SCO trigger
        BTDIGITAL_REG(0xd02201f0) &= (~0x60000000);
        //set ACL trigger
        BTDIGITAL_REG(0xd02201f0) |= 0x20000000;
    }
    else if(triggle_mode == SCO_TRIGGLE_MODE)
    {
        //clear ACL trigger
        BTDIGITAL_REG(0xd02201f0) &= (~0x60000000);
        //set SCO trigger
        BTDIGITAL_REG(0xd02201f0) |= 0x40000000;
    }
#endif
}

/*
bit23  1
bit17  1:master  0:slave
*/
void btdrv_set_tws_role_triggler(uint8_t tws_mode)
{
    if(tws_mode == 1 || tws_mode == 3)
    {
        BTDIGITAL_REG(0xd02201f0) |= 0x10000000;
    }
    else if(tws_mode == 2)
    {
        BTDIGITAL_REG(0xd02201f0) &= (~0x10000000);
    }
}

void btdrv_set_bt_pcm_triggler_en(uint8_t  en)
{
}

void bt_syn_trig_checker(uint16_t conhdl)
{
    int32_t clock_offset;
    uint16_t bit_offset;
    bt_drv_reg_op_piconet_clk_offset_get(conhdl, &clock_offset, &bit_offset);

#if defined(__FPGA_BT_1500__)
/*
    BT_DRV_TRACE(2,"bt_syn_set_tg_tick checker d0220c7c=0x%08x d0220c84=0x%08x", \
            BTDIGITAL_REG(0xd0220c7c), BTDIGITAL_REG(0xd0220c84));
    BT_DRV_TRACE(3,"bt_syn_set_tg_tick checker curr_ticks:0x%08x bitoffset=0x%04x rxbit=0x%04x", \
                            btdrv_syn_get_curr_ticks(),\
                            BTDIGITAL_REG(EM_BT_BITOFF_ADDR+(conhdl - 0x80)*BT_EM_SIZE) & 0x3ff,\
                            BTDIGITAL_REG(EM_BT_RXBIT_ADDR+(conhdl - 0x80)*BT_EM_SIZE) & 0x3ff);
    BT_DRV_TRACE(2,"bt_syn_set_tg_tick checker clock_offset:0x%08x bit_offset=0x%04x", clock_offset, bit_offset);
*/
#else
    BT_DRV_TRACE(3,"bt_syn_set_tg_tick checker d0220498=0x%08x d02204a4=0x%08x d02201f0=0x%08x", \
            BTDIGITAL_REG(0xd0220498), BTDIGITAL_REG(0xd02204a4), BTDIGITAL_REG(0xd02201f0));
    BT_DRV_TRACE(3,"bt_syn_set_tg_tick checker curr_ticks:0x%08x bitoffset=0x%04x rxbit=0x%04x", \
                            btdrv_syn_get_curr_ticks(),\
                            BTDIGITAL_REG(EM_BT_BITOFF_ADDR+(conhdl - 0x80)*BT_EM_SIZE) & 0x3ff,\
                            BTDIGITAL_REG(EM_BT_RXBIT_ADDR+(conhdl - 0x80)*BT_EM_SIZE) & 0x3ff);
    BT_DRV_TRACE(2,"bt_syn_set_tg_tick checker clock_offset:0x%08x bit_offset=0x%04x", clock_offset, bit_offset);
#endif
}


#ifdef __SW_TRIG__

void btdrv_play_trig_mode_set(uint8_t mode)
{
    ///0xc7c bit[14:13] 00:hw trig, 01:sw trig, 1x:self trig
    BTDIGITAL_REG(0xd0220c7c) = (BTDIGITAL_REG(0xd0220c7c) & 0xFFFF9FFF) | (mode << 13);
    BT_DRV_TRACE(1,"btdrv_play_trig_mode_set:%d\n",mode);
}

#define ROUNDDOWN(x)   ((int)(x))
#define ROUNDUP(x)     ((int)(x) + ((x-(int)(x)) > 0 ? 1 : 0))
#define ROUND(x)       ((int)((x) + ((x) > 0 ? 0.5 : -0.5)))

uint16_t btdrv_Tbit_M_h_get(uint32_t Tclk_M, int16_t Tbit_M_h_ori)
{
    uint16_t Tbit_M_h;
    if(Tclk_M % 2) ///if Tclk_M is odd
    {
        if(Tbit_M_h_ori == 624)
        {
            Tbit_M_h = 624;
        }
        else
        {
            Tbit_M_h = (uint16_t)(ROUNDDOWN(Tbit_M_h_ori/2)*2 + 1);
        }
    }
    else ///if Tclk_M is even
    {
        Tbit_M_h = (uint16_t)(ROUNDUP(Tbit_M_h_ori/2)*2);
        if(Tbit_M_h_ori % 2)//if Tbit_M_h_ori is odd
        {
            Tbit_M_h += 2;
        }
    }
    BT_DRV_TRACE(1,"btdrv_Tbit_M_h_get:%d\n",Tbit_M_h);
    return Tbit_M_h;
}


uint16_t btdrv_Tbit_M_get(uint32_t Tclk_M, uint16_t Tbit_M_h)
{
    uint16_t Tbit_M;
    if(Tclk_M % 2)///if Tclk_M is odd
    {
        Tbit_M = (uint16_t)ROUNDDOWN(Tbit_M_h/2);
    }
    else
    {
        Tbit_M = (uint16_t)ROUND((Tbit_M_h/2 + 312));
        if(Tbit_M_h % 2)//if Tbit_M_h is odd
        {
            Tbit_M += 1;
        }
    }
    BT_DRV_TRACE(1,"btdrv_Tbit_M_get:%d\n",Tbit_M);
    return Tbit_M;
}


uint16_t btdrv_rxbit_get(uint16_t conhdl)
{
    uint16_t rxbit;
    uint8_t conidx;
    conidx = btdrv_conhdl_to_linkid(conhdl);
    rxbit = BTDIGITAL_REG(EM_BT_RXBIT_ADDR+conidx*BT_EM_SIZE) & 0x3ff;
    BT_DRV_TRACE(2,"btdrv_rxbit_get conhdl=%x, rxbit=%d\n",conhdl,rxbit);
    return rxbit;
}

#define RF_DELAY  18
int16_t btdrv_bitoff_get(uint16_t rxbit)
{
    int16_t bitoff;
    bitoff = rxbit - 68 - RF_DELAY;
    BT_DRV_TRACE(1,"btdrv_bitoff_get:%d\n",bitoff);
    return bitoff;
}

uint16_t btdrv_Tbit_S_get(uint16_t Tbit_M, int bitoff)
{
    uint16_t Tbit_S;
    int temp;

    temp = Tbit_M - bitoff;
    if(temp < 0)
    {
        Tbit_S = temp + 625;
    }
    else if(temp > 624)
    {
        Tbit_S = temp - 625;
    }
    else
    {
        Tbit_S = temp;
    }
    BT_DRV_TRACE(1,"btdrv_Tbit_S_get:%d\n",Tbit_S);
    return Tbit_S;
}


int32_t btdrv_slotoff_get(uint16_t conhdl)
{
    int32_t clkoff;
    int32_t slotoff;

    if(conhdl>=0x80)
        clkoff = btdrv_syn_get_offset_ticks(conhdl-0x80);
    else
        clkoff = 0;

    if(clkoff < 0)
    {
        slotoff = -(int32_t)ROUNDUP((-clkoff)/2) - 1;
    }
    else
    {
        slotoff = (int32_t)ROUNDDOWN(clkoff/2);
    }
    BT_DRV_TRACE(1,"btdrv_slotoff_get:%d\n",slotoff);
    return slotoff;
}


uint8_t btdrv_clk_adj_M_get(uint32_t Tclk_M)
{
    uint8_t clk_adj_M;
    if(Tclk_M % 2)///if Tclk_M is odd
    {
        clk_adj_M = 1;
    }
    else
    {
        clk_adj_M = 0;
    }
    BT_DRV_TRACE(1,"btdrv_clk_adj_M_get:%d\n",clk_adj_M);
    return clk_adj_M;
}

uint8_t btdrv_clk_adj_S_get(uint16_t Tbit_S)
{
    uint8_t clk_adj_S;
    if(Tbit_S >= 312)
    {
        clk_adj_S = 0;
    }
    else
    {
        clk_adj_S = 1;
    }
    BT_DRV_TRACE(1,"btdrv_clk_adj_S_get:%d\n",clk_adj_S);
    return clk_adj_S;
}

uint32_t btdrv_Tclk_S_get(uint32_t Tclk_M, uint8_t clk_adj_M,
                                uint8_t clk_adj_S, int32_t slotoff, int16_t bitoff)
{
    int temp;
    uint32_t Tclk_S;
    if(clk_adj_M == clk_adj_S)
    {
        temp = 0;
    }
    else
    {
        if(bitoff > 0)
        {
            temp = 1;
        }
        else
        {
            temp = -1;
        }
    }
    Tclk_S = Tclk_M - slotoff*2 + temp;
    BT_DRV_TRACE(1,"btdrv_Tclk_S_get:%d\n",Tclk_S);
    return Tclk_S;
}


uint16_t btdrv_Tclk_S_h_get(uint32_t Tclk_S, uint16_t Tbit_S)
{
    uint16_t Tclk_S_h;
    if(Tbit_S == 312)
    {
        Tclk_S_h = 0;
    }
    else if(Tbit_S > 312)
    {
        Tclk_S_h = (Tbit_S - 312)*2 + (Tclk_S % 2);
    }
    else
    {
        Tclk_S_h = Tbit_S*2 + (Tclk_S % 2);
    }
    BT_DRV_TRACE(1,"btdrv_Tclk_S_h_get:%d\n",Tclk_S_h);
    return Tclk_S_h;
}

void btdrv_sw_trig_tg_finecnt_set(uint16_t tg_bitcnt)
{
    //0xc98 bit[9:0]
    BTDIGITAL_REG(0xd0220c98) = (BTDIGITAL_REG(0xd0220c98) & 0x3ff) | tg_bitcnt;
    BT_DRV_TRACE(3,"[%s] 0xd0220c98=%x, tg_bitcnt=%d\n",__func__,BTDIGITAL_REG(0xd0220c98), tg_bitcnt);
}


void btdrv_sw_trig_tg_clkncnt_set(uint32_t num)
{
    //0xc94 bit[27:0]
    BTDIGITAL_REG(0xd0220c94) = (BTDIGITAL_REG(0xd0220c94) & 0xf0000000) | (num & 0x0fffffff);
    BT_DRV_TRACE(3,"[%s] 0xd0220c94=%x, num=%d\n",__func__,BTDIGITAL_REG(0xd0220c94), num);
}

void btdrv_sw_trig_slave_calculate_and_set(uint16_t conhdl, uint32_t Tclk_M, uint16_t Tbit_M_h_ori)
{
    uint16_t Tbit_M_h;
    uint16_t Tbit_M;
    uint16_t rxbit;
    int16_t bitoff;
    uint16_t Tbit_S;
    int32_t slotoff;
    uint8_t clk_adj_M;
    uint8_t clk_adj_S;
    uint32_t Tclk_S;
    uint32_t Tclk_S_h;

    Tbit_M_h = btdrv_Tbit_M_h_get(Tclk_M,Tbit_M_h_ori);
    Tbit_M = btdrv_Tbit_M_get(Tclk_M,Tbit_M_h);
    rxbit = btdrv_rxbit_get(conhdl);
    bitoff = btdrv_bitoff_get(rxbit);
    Tbit_S = btdrv_Tbit_S_get(Tbit_M,bitoff);
    slotoff = btdrv_slotoff_get(conhdl);
    clk_adj_M = btdrv_clk_adj_M_get(Tclk_M);
    clk_adj_S = btdrv_clk_adj_S_get(Tbit_S);
    Tclk_S = btdrv_Tclk_S_get(Tclk_M,clk_adj_M,clk_adj_S,slotoff,bitoff);
    Tclk_S_h = btdrv_Tclk_S_h_get(Tclk_S,Tbit_S);
    btdrv_sw_trig_tg_clkncnt_set(Tclk_S);
    btdrv_sw_trig_tg_finecnt_set(Tclk_S_h);
    BTDIGITAL_REG(0xd0220c7c) |= 0x1;
}

void btdrv_sw_trig_master_set(uint32_t Tclk_M, uint16_t Tbit_M_h_ori)
{
    btdrv_sw_trig_tg_clkncnt_set(Tclk_M);
    btdrv_sw_trig_tg_finecnt_set(Tbit_M_h_ori);
    BTDIGITAL_REG(0xd0220c7c) |= 0x1;
}

#endif

void btdrv_set_bt_pcm_en(uint8_t  en)
{
    *(volatile uint32_t *)(0xd02201b0) |= 0x01; //pcm enable
}

void btdrv_open_pcm_fast_mode_enable(void)
{
    //BT_DRV_TRACE(0,"pcm fast mode\n");
    *(volatile uint32_t *)(0xd0220464) |= 1<<22;///pcm fast mode en bit22
    *(volatile uint32_t *)(0xd02201b8) = (*(volatile uint32_t *)(0xd02201b8)&0xFFFFFF00)|0x8;///pcm clk [8:0]
    *(volatile uint32_t *)(0xd0220460) = (*(volatile uint32_t *)(0xd0220460)&0xFFFE03FF)|0x0000EC00;///sample num in one frame [16:10]
}

//[31:24]
#define FA_TXPWR_UP (0x29)
//[23:16]
#define FA_RXPWR_UP (0x33)
//[14:8]
#define TX_PWR_DOWN (0x07)
//[7:0]
#define TX_PWR_UP       (0x3c)

//0xD0350230
#define NEW_CORR_SYNC_OUTPUT_POS   15

//0xD0350230
#define NEW_CORR_SEL_POS   1

//0xD0220C04
#define TX_GIAN_DR_POS  21
#define RX_GIAN_DR_POS   22
#define FIX_RX_GIAN_IDX  1

//D0220C10
//[26:24]
#define FA_TX_GAIN_IDX 7
//[23:21]
#define FA_RX_GAIN_IDX  0

//D0330038
#define BT_SYS_48M_SEL_POS 7
#define BT_SYS_48M_ENABLE_2M_POS 19
#define FPGA_1303

static void fpg_digital_init(void)
{
#ifdef __FPGA_BT_1500__
    BTDIGITAL_REG_SET_FIELD(0xD022048C,0xffff,0, (TX_PWR_DOWN<<8|TX_PWR_UP));
    BTDIGITAL_REG_SET_FIELD(0xD0220C0C,0xffff,16, (FA_TXPWR_UP<<8|FA_RXPWR_UP));
    //BTDIGITAL_REG(0xD0220C04) |= 1<<TX_GIAN_DR_POS | 1<<RX_GIAN_DR_POS;
    BTDIGITAL_REG(0xD0220C04) |= 1<<TX_GIAN_DR_POS;

    //BTDIGITAL_REG_SET_FIELD(0xD0220C04,0xf, 12, FIX_RX_GIAN_IDX);

    //BTDIGITAL_REG_SET_FIELD(0xD0220C10,0x7, 21, FA_RX_GAIN_IDX);
    //BTDIGITAL_REG_SET_FIELD(0xD0220C10,0x7, 24, FA_TX_GAIN_IDX);

    BTDIGITAL_REG(0xD0220c8c) &= ~(1<<31);
    BTDIGITAL_REG(0xD0350230) &= ~0x1;
    // BTDIGITAL_REG(0xD0330038) |= (1<<BT_SYS_48M_SEL_POS |1<<BT_SYS_48M_ENABLE_2M_POS);

    //[below for tprots]
    //BTDIGITAL_REG(0xD0350240) |= 1<<SYNC_WORD_OUTPUT_POS;
    //BTDIGITAL_REG(0xD0220050) = 0xb8b8;

#else

#ifdef FPGA_1303
    BTDIGITAL_REG(0xD0220460) |= (1<<29);//for bus wait mask

//pcm hw write en
    BTDIGITAL_REG(0xD02204a8) |= (1<<5);

//set fa rx ramp time
   BTDIGITAL_REG(0xd035020c)&=~(0xff<<20);
   BTDIGITAL_REG(0xd035020c)|=(0x0f<<20);

#if 0  //old corr for primary pkt & aux fa Under BT mode
//set old corr mode
    BTDIGITAL_REG(0xD03503A0) &=~(1); //old corr for primary pkt
    BTDIGITAL_REG(0xD03503A0) &=~(1<<1);//old corr for aux fa
    BTDIGITAL_REG(0xD03502c4) &= ~0x7ff;//set time init under old corr mode
    BTDIGITAL_REG(0xD03502c4) |= 0x306; //set time init under old corr mode
    BTDIGITAL_REG(0xD03502c4) &= ~(0x2ff<<22); //set time init under old corr mode, for fa
     BTDIGITAL_REG(0xD03502c4) |=  (0x2a<<22); //set time init under old corr mode, for fa
#else // new corr  for primary pkt & aux fa Under BT mode
   BTDIGITAL_REG(0xd03503cc)|= (1<<7); //set err check mode
   BTDIGITAL_REG(0xd03503a0)&= (0xf<<4);//
   BTDIGITAL_REG(0xd03503a0)|= (0x7<<4);//set corr dly for new corr mode
   BTDIGITAL_REG(0xD03502c4) &= ~0x7ff;//
   BTDIGITAL_REG(0xD03502c4) |= 0x2ff; //	26m sys, new corr mode should use it
#endif

#if 0
//set new corr mode for BLE
   BTDIGITAL_REG(0xd03503a0)|= (1<<3); //set ble newcorr mode
    BTDIGITAL_REG(0xd03503cc) &= ~(0x1e<<25);
    BTDIGITAL_REG(0xd03503cc) |= (0x4<<25);
#endif

//52m sys
    BTDIGITAL_REG(0xD0330038)|=(1<<7);
    BTDIGITAL_REG(0xD03300f0)|= 1;
    BTDIGITAL_REG(0xD03502c4) &= ~0x7ff;//
    BTDIGITAL_REG(0xD03502c4) |= 0x306; //	52m sys, new corr mode should use it

//Pri PKT TRX PWRUP
   BTDIGITAL_REG(0xd0220080)&=~(0xff);
   BTDIGITAL_REG(0xd0220080)|= 0x38;//TX

//FA TRX PWRUP. Note: The timing is only fit for FPGA, because there are 18 us trx_delay on k7410(9us more than FPGA SIM,because TRX FIFO on board)
   BTDIGITAL_REG(0xd0220468)&=~(0xffff);
   BTDIGITAL_REG(0xd0220468)|= 0x2a;//TX
   BTDIGITAL_REG(0xd0220468)|=(0x38<<8);//RX

//FA TRX GAIN
   BTDIGITAL_REG(0xd0220474)&=~(0x1ff<<16);
   BTDIGITAL_REG(0xd0220474)|=(1<<16);//enable fa rxgain cntl
   BTDIGITAL_REG(0xd0220474)|=(1<<20);//enable fa txpwr cntl
   BTDIGITAL_REG(0xd0220474)|=(0<<17);//set fa rxgain idx=0
   BTDIGITAL_REG(0xd0220474)|=(0x7<<21);//set fa txgain idx=7

//FA/ECC Syncword mode
   BTDIGITAL_REG(0xd022048c)&= ~(1<<30);//32bit sw

//ECC mode
   BTDIGITAL_REG(0xd0220464) |= (0x1<<15);//ecc enable
   BTDIGITAL_REG(0xd02204a8) |=(1<<24);//set ECC 8bytes mode
   BTDIGITAL_REG_SET_FIELD(0xd02204a8,3,27,3);//cfg xsco fa rate mode; ECC 8PSK
   BTDIGITAL_REG_SET_FIELD(0xd02204a8,3,13,3);//cfg acl fa rate mode; ECC 8PSK
   BTDIGITAL_REG(0xd022048c) &= ~0x3ff;
   //BTDIGITAL_REG(0xd022048c) |= 0xef;//set ecc length, protect 1 block
   BTDIGITAL_REG(0xd022048c) |= 0x1de;//set ecc length, protect 2 block
   //BTDIGITAL_REG(0xd022048c) |= 0x2cd;//set ecc length, protect 3 block

 //set fa sync err
 BTDIGITAL_REG(0xd0220000)&= ~(0xf <<4);
 BTDIGITAL_REG(0xd0220000)|= (0x02<<4);

 //ecc usr def
// BTDIGITAL_REG(0xd0220498)|=(1<<31);//enable ecc usr def

//BTDIGITAL_REG(0xd02204a8) &=~(1<<20);//disable new fa logic
//BTDIGITAL_REG(0xd022046c) &=~(1<<31);//dis null dm1 logic for old fa mode

#else
    *(volatile uint32_t *)(0xD03503A0) = 0x1C070050; //old corr mode
    *(volatile uint32_t *)(0xD02204a8) |= (1<<5);
    //*(volatile uint32_t *)(0xD03502c4) = 0x127f0305;
#endif

//pcm sample setting
 *(volatile uint32_t *)0xd02201b8 |= (1<<31);
#if defined(IBRT)
    //BTDIGITAL_REG(0xd022046c)&=~(1<<31);//enable DM1 empty fastack

    //BTDIGITAL_REG(0xd0220468) |= (1<<22);
    //BTDIGITAL_REG(0xD035020c)=0x00f1002c;
    //BTDIGITAL_REG(0xD0350210)=0x00f1002c;
#if 0//snoop Error Correcting Code
    BTDIGITAL_REG0xd0220468 =0x10403226;
    BTDIGITAL_REG(0xd0220464) &= ~(0x1F<<24);//ecc time adj
    BTDIGITAL_REG(0xd0220464) |= 0x1<<25;//ecc time adj
    BTDIGITAL_REG(0xd0220464) |= 0x1<<15;//en ecc mode
    //BTDIGITAL_REG(0xd0220464) &=~(0x1<<20);//close lmp ecc
    BTDIGITAL_REG(0xd02204a8) |= 0x3<<13;//fa 1m qpsks
    BTDIGITAL_REG(0xd022048c) |= 0xed;//set ecc length
    BTDIGITAL_REG(0xd022048c) |= 0x1<<27;//fix ecc length to 16byte
    BTDIGITAL_REG(0xd0220498) = 0xA01B91;//fastack timeout
#endif
#endif
#endif
}

void fpga_init(void)
{
#if INTERSYS_DEBUG
    btdrv_trace_config(BT_CONTROLER_TRACE_TYPE_INTERSYS);
#endif
    fpg_digital_init();

}
