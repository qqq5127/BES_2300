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
#include "bt_drv.h"
#include "bt_drv_internal.h"
#include "bt_drv_2300_internal.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_interface.h"
#include "hal_timer.h"
#include "hal_intersys.h"
#include "hal_trace.h"
#include "hal_psc.h"
#include "hal_cmu.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "pmu.h"
#include "nvrecord_dev.h"

#if 0
typedef enum
{
    BTDRV_INIT_IDLE,
    BTDRV_RF_INIT,
    BTDRV_INS_PATCH_INIT,
    BTDRV_DATA_PATCH_INIT,
    BTDRV_CONFIG_INIT,
    BTDRV_INIT_INIT_COMPLETE
} BTDRV_INIT_STATE;


typedef struct
{
    volatile BTDRV_INIT_STATE  step;
} BTDRV_STRUCT;


BTDRV_STRUCT  btdrv_cb= {0};
#endif

#if 0
extern void btdrv_intersys_rx_thread(void const *argument);

uint32_t os_thread_def_stack_btdrv_rx [1024/ sizeof(uint32_t)];
osThreadDef_t os_thread_def_btdrv_intersys_rx_thread = { (btdrv_intersys_rx_thread), (osPriorityHigh), (1024), (os_thread_def_stack_btdrv_rx)};

//bt
static osThreadId btdrv_intersys_rx_thread_id;
static osThreadId btdrv_intersys_tx_thread_id;
#endif

bool btdrv_dut_mode_enable = false;

static volatile uint32_t btdrv_tx_flag = 1;
void btdrv_tx(const unsigned char *data, unsigned int len)
{
//  HciPacketSent(intersys_tx_pkt);
    BT_DRV_TRACE(0,"tx");
//   osSignalSet(btdrv_intersys_tx_thread_id, 0x1);
    btdrv_tx_flag = 1;
}

void btdrv_dut_accessible_mode_manager(const unsigned char *data);

static unsigned int btdrv_rx(const unsigned char *data, unsigned int len)
{
    hal_intersys_stop_recv(HAL_INTERSYS_ID_0);

    BT_DRV_TRACE(2,"%s len:%d", __func__, len);
    BT_DRV_DUMP("%02x ", data, len>7?7:len);
    btdrv_dut_accessible_mode_manager(data);
    hal_intersys_start_recv(HAL_INTERSYS_ID_0);

    return len;
}

void btdrv_SendData(const uint8_t *buff,uint8_t len)
{
    btdrv_tx_flag = 0;

    hal_intersys_send(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, buff, len);
    BT_DRV_TRACE(1,"%s", __func__);
    BT_DRV_DUMP("%02x ", buff, len);
//    btdrv_delay(1);
    while( (btdrv_dut_mode_enable==0) && btdrv_tx_flag == 0);
}



////open intersys interface for hci data transfer
static bool hci_has_opened = false;

void btdrv_hciopen(void)
{
    int ret = 0;

    if (hci_has_opened)
    {
        return;
    }

    hci_has_opened = true;

    ret = hal_intersys_open(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, btdrv_rx, btdrv_tx, false);

    if (ret)
    {
        BT_DRV_TRACE(0,"Failed to open intersys");
        return;
    }

    hal_intersys_start_recv(HAL_INTERSYS_ID_0);
}

////open intersys interface for hci data transfer
void btdrv_hcioff(void)
{
    if (!hci_has_opened)
    {
        return;
    }
    hci_has_opened = false;

    hal_intersys_close(HAL_INTERSYS_ID_0,HAL_INTERSYS_MSG_HCI);
}



/*  btdrv power on or off the bt controller*/
void btdrv_poweron(uint8_t en)
{
    //power on bt controller
    if(en)
    {
        hal_psc_bt_enable();
        hal_cmu_bt_clock_enable();
        hal_cmu_bt_reset_clear();
        hal_cmu_bt_module_init();
        btdrv_delay(10);
        // BTDM mode 4.2
#ifdef BT_50_FUNCTION
        BTDIGITAL_REG(0xC0000050) = 0x50;
        btdrv_delay(100);
#else
        BTDIGITAL_REG(0xC0000050) = 0x42;
        btdrv_delay(100);
#endif
    }
    else
    {
        btdrv_delay(10);
        hal_cmu_bt_reset_set();
        hal_cmu_bt_clock_disable();
        hal_psc_bt_disable();
    }
}

void btdrv_rf_init_ext(void)
{
    unsigned int xtal_fcap;

    if (!nvrec_dev_get_xtal_fcap(&xtal_fcap))
    {
        btdrv_rf_init_xtal_fcap(xtal_fcap);
        btdrv_delay(1);
        BT_DRV_TRACE(2,"%s 0xc2=0x%x", __func__, xtal_fcap);
    }
    else
    {
        btdrv_rf_init_xtal_fcap(DEFAULT_XTAL_FCAP);
        BT_DRV_TRACE(1,"%s failed", __func__);
    }
}

void tx_ramp_new(void)
{
    return;

    BTDIGITAL_REG(0xd0220080) = 0x18421656;
    BTDIGITAL_REG(0xd0350360) = 0x00000120;
    BTDIGITAL_REG(0xd0350300) = 0x00000005;
    BTDIGITAL_REG(0xd0350340) = 0x00000005;
}

extern void bt_drv_disable_sco_avoid_ble();
void bt_drv_extra_config_after_init(void)
{
//   BTDIGITAL_REG(0xc0000868) = 0x00000001;  ///metal id  2---4
    BTDIGITAL_BT_EM(0xD021028A) = 0x00000000;//tx pwr init
    bt_drv_reg_op_afh_env_reset();

    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        *(uint16_t *)0xc0005810 = 0x1000;  ///free 1.5k ram for patch
        *(uint32_t *)0xc00041b8 = 0x1000a55a;
        *(uint32_t *)0xc0006d44 = 0;
    }
#ifndef BT_RSSI_MONITOR
    bt_drv_set_hwagc_read_en(0);
#endif
#ifdef __BLE_PRIO_FOR_SCO__
    bt_drv_disable_sco_avoid_ble();
#endif

}



#ifdef __HW_AGC__

uint16_t btdrv_2300_get_10bit_add_val(uint32_t val, uint8_t count)
{
    uint8_t i;
    uint32_t new_val[32];
    uint16_t add_val = 0;

    for(i=0; i<32 ; i++)
    {
        new_val[i] = (val & 1);
        val = (val >> 1);
    }//get val's each bit to new_val

    for(i=(0+count); i<(10+count) ; i++)
    {
        add_val |= (new_val[i] << (i-count));
    }

    return add_val;
}


uint16_t btdrv_2300_val_get_max_min_num(uint16_t *val)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint16_t max = val[0];
    uint16_t min = val[0];

    for(i=1; i<10; i++)
    {
        if(max < val[i])
        {
            max = val[i];
            j = i;
        }
        else if(min > val[i])
        {
            min = val[i];
            k = i;
        }
    }
    BT_DRV_TRACE(2,"The max is:%x, j:%d\n", max,j);
    BT_DRV_TRACE(2,"The min is:%x, k:%d\n", min,k);
    return ((j<<8) | k);
}

uint16_t btdrv_2300_get_average_val(uint16_t *val)
{
    uint8_t i=0;
    uint16_t num;
    uint8_t max_num,min_num;
    uint16_t add_val = 0;
    uint16_t average_val;

    num = btdrv_2300_val_get_max_min_num(val);

    max_num = (uint8_t)((num & 0xff00)>>8);
    min_num = (uint8_t)(num & 0x00ff);
    BT_DRV_TRACE(1,"max_num:%d\n", max_num);
    BT_DRV_TRACE(1,"min_num:%d\n", min_num);

    for(i=0; i<10 ; i++)
    {
        BT_DRV_TRACE(2,"val[%x]:%x\n",i,val[i]);
        add_val += val[i];
    }
    add_val = (add_val-val[max_num]-val[min_num]);
    BT_DRV_TRACE(1,"add_val:%x\n",add_val);

    average_val = add_val / 8;
    return average_val;
}

union cal_val
{
    struct
    {
        int32_t val1  :10;
        int32_t val2  :10;
        int32_t val3  :10;
        uint32_t  reverd_bit  :2;
    };
    volatile uint32_t reg;
};
void btdrv_2300_hwagc_dc_cal_1(void)
{
    uint32_t first_val;
    volatile uint32_t value = 0;
//    int16_t bit_value1[10],bit_value2[10],bit_value3[10];//10bit
    int32_t bit_average_val1=0,bit_average_val2=0,bit_average_val3=0;
    uint32_t real_val;

    union cal_val dccal_val;

    btdrv_write_rf_reg(0xf3,0x2c41);//i2v reset dr=1

    btdrv_write_rf_reg(0xcf,0xff32);//lna gain dr=1
    btdrv_write_rf_reg(0xdf,0x210f);//lna gain dr=1 //210f
    btdrv_write_rf_reg(0xcd,0x0040);//lna pdt gain


    btdrv_write_rf_reg(0xd0,0xf99f);//i2v flt gain dr=1

    btdrv_write_rf_reg(0xaf, 0x00c0);//rx input pull down

    //BTDIGITAL_REG(0xc000033c) = 0x00000100;

    BTDIGITAL_REG(0xd02201e4) = 0x00000000;//rx continue
    btdrv_delay(10);
    BTDIGITAL_REG(0xd02201e4) = 0x000a0080;
    btdrv_delay(10);

    for(uint8_t i = 0; i < 10; i++)
    {
        BTDIGITAL_REG(0xd03503b0) = 0x80000000;
        btdrv_delay(1);
        first_val = BTDIGITAL_REG(0xd03503b4);
        BT_DRV_TRACE(1,"first_val:%x\n",first_val);
        BTDIGITAL_REG(0xd03503b0) = 0x80000000;
    }

    for(uint8_t i = 0; i < 10; i++)
    {
        BTDIGITAL_REG(0xd03503b0) = 0x80000000;
        btdrv_delay(1);
        BT_DRV_TRACE(1,"BTDIGITAL_REG(0xd03503b4):%x\n",BTDIGITAL_REG(0xd03503b4));
        value = BTDIGITAL_REG(0xd03503b4);
        BT_DRV_TRACE(2,"%x:  value:%x\n",i,value);
        dccal_val.reg = value;
        bit_average_val1 +=dccal_val.val1;
        bit_average_val2 +=dccal_val.val2;
        bit_average_val3 +=dccal_val.val3;
        BT_DRV_TRACE(2,"val1=%d,val3=%d",dccal_val.val1, dccal_val.val3);
    }

    bit_average_val1 /= 10;

#ifdef __HW_AGC_I2V_DISABLE_DC_CAL__
    bit_average_val2 = 0;
#else
    bit_average_val2 /=10;
#endif
    bit_average_val3 /= 10;

    BT_DRV_TRACE(3,"vaer1=%d,vaer2=%d,vaer3=%d",bit_average_val1, bit_average_val2, bit_average_val3);

    dccal_val.val1 = bit_average_val1;
    dccal_val.val2 = bit_average_val2;
    dccal_val.val3 = bit_average_val3;
    dccal_val.reverd_bit = 0;
    //  real_val = (bit_average_val1 | (bit_average_val2 << 10) | (bit_average_val3 << 20));
    real_val = dccal_val.reg;
    BT_DRV_TRACE(1,"real_val:%x\n",real_val);
    BTDIGITAL_REG(0xd03503b0) = real_val;

    BTDIGITAL_REG(0xd02201e4) = 0x00000000;

    btdrv_write_rf_reg(0xaf, 0x0000);

    btdrv_write_rf_reg(0xf3,0x0c41);//i2v reset dr=0

    btdrv_write_rf_reg(0xcf,0x7f32);//lna gain dr=0
    btdrv_write_rf_reg(0xdf,0x2006);//lna gain dr=0
    btdrv_write_rf_reg(0xd0,0xe91f);//i2v flt gain dr=0
}

void btdrv_2300_hwagc_dc_cal(void)
{
    uint32_t first_val;
    uint32_t value = 0;
    uint16_t bit_value1[10],bit_value2[10],bit_value3[10];//10bit
    uint16_t bit_average_val1,bit_average_val2,bit_average_val3;
    uint16_t real_val;

    btdrv_write_rf_reg(0xf3,0x2c41);//i2v reset dr=1

    btdrv_write_rf_reg(0xcf,0xff32);//lna gain dr=1
    btdrv_write_rf_reg(0xdf,0x213f);//lna gain dr=1 //210f
    btdrv_write_rf_reg(0xd0,0xf99f);//i2v flt gain dr=1

    btdrv_write_rf_reg(0xaf, 0xc0c0);//rx input pull down
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        BTDIGITAL_REG(0xc000033c) = 0x00000100;
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        BTDIGITAL_REG(0xc00003a0) = 0x00000100;
    }

    BTDIGITAL_REG(0xd02201e4) = 0x00000000;//rx continue
    btdrv_delay(10);
    BTDIGITAL_REG(0xd02201e4) = 0x000a0080;
    btdrv_delay(10);

    for(uint8_t i = 0; i < 10; i++)
    {
        BTDIGITAL_REG(0xd03503b0) = 0x80000000;
        btdrv_delay(1);
        first_val = BTDIGITAL_REG(0xd03503b4);
        BT_DRV_TRACE(1,"first_val:%x\n",first_val);
        BTDIGITAL_REG(0xd03503b0) = 0x80000000;
    }

    for(uint8_t i = 0; i < 10; i++)
    {
        BTDIGITAL_REG(0xd03503b0) = 0x80000000;
        btdrv_delay(1);
        BT_DRV_TRACE(1,"BTDIGITAL_REG(0xd03503b4):%x\n",BTDIGITAL_REG(0xd03503b4));
        value = BTDIGITAL_REG(0xd03503b4);
        BT_DRV_TRACE(2,"%x:  value:%x\n",i,value);

        bit_value1[i] = btdrv_2300_get_10bit_add_val(value,0);
        bit_value2[i] = btdrv_2300_get_10bit_add_val(value,10);
        bit_value3[i] = btdrv_2300_get_10bit_add_val(value,20);
        BT_DRV_TRACE(4,"%x:  bit_value1:%x,bit_value2:%x,bit_value3:%x\n",i,bit_value1[i],bit_value2[i],bit_value3[i]);
        BTDIGITAL_REG(0xd03503b0) = 0x80000000;
    }

    bit_average_val1 = btdrv_2300_get_average_val(bit_value1);
    bit_average_val2 = btdrv_2300_get_average_val(bit_value2);
    bit_average_val3 = btdrv_2300_get_average_val(bit_value3);
    BT_DRV_TRACE(3,"bit_average_val1:%x,bit_average_val2:%x,bit_average_val3:%x\n",bit_average_val1,bit_average_val2,bit_average_val3);

    real_val = (bit_average_val1 | (bit_average_val2 << 10) | (bit_average_val3 << 20));
    BT_DRV_TRACE(1,"real_val:%x\n",real_val);
    BTDIGITAL_REG(0xd03503b0) = real_val;

    BTDIGITAL_REG(0xd02201e4) = 0x00000000;

    btdrv_write_rf_reg(0xaf, 0xc000);

    btdrv_write_rf_reg(0xf3,0x0c41);//i2v reset dr=0

    btdrv_write_rf_reg(0xcf,0x7f32);//lna gain dr=0
    btdrv_write_rf_reg(0xdf,0x2006);//lna gain dr=0
    btdrv_write_rf_reg(0xd0,0xe91f);//i2v flt gain dr=0
}

#endif

void btdrv_2300_rccal(void)
{
    uint16_t value;
    uint16_t value_tmp;
    uint16_t val_tmp1;
    uint16_t val_tmp2;

    btdrv_write_rf_reg(0x80, 0xa010);
    btdrv_write_rf_reg(0xc4, 0xffff);//[5:4]=11,open rcosc bias
    btdrv_write_rf_reg(0x80, 0xa000);

    btdrv_write_rf_reg(0xb3,0x33f3);//[9:8]=11,pwup rcosc

    btdrv_delay(1);

    BTDIGITAL_REG(0xd02201e4) = 0x00000000;
    btdrv_delay(10);
    BTDIGITAL_REG(0xd02201e4) = 0x000a0080;
    btdrv_delay(10);

    btdrv_write_rf_reg(0x80, 0xa010);
    btdrv_write_rf_reg(0xd6,0xf858);//[15]=1,enable clk counter
    btdrv_write_rf_reg(0x80, 0xa000);

    btdrv_delay(10);

    btdrv_read_rf_reg(0xc0,&value);
    BT_DRV_TRACE(1,"btdrv_rccal 0xc0 value:%x\n",value);

    btdrv_read_rf_reg(0x8b,&val_tmp1);
    BT_DRV_TRACE(1,"0x8b val_tmp1=%x\n",val_tmp1);

    btdrv_read_rf_reg(0x8d,&val_tmp2);
    BT_DRV_TRACE(1,"0x8d val_tmp2=%x\n",val_tmp2);

    value_tmp = value & 0x0fff;
    if((value_tmp < 0x0ff0)&&(value_tmp > 0x0200)&&((value|0xefff)==0xffff))
    {
        BT_DRV_TRACE(0,"0xc0 0x200 < value < 0xff0 done \n");
        btdrv_write_rf_reg(0x8b,(((( 0x7c4 * 1000 / (value & 0x0fff)) * 0x90 / 1000) << 8) | (val_tmp1 & 0x00ff)));
        BT_DRV_TRACE(1,"0x8b:%x\n",(((( 0x7c4 * 1000 / (value & 0x0fff)) * 0x90 / 1000) << 8) | (val_tmp1 & 0x00ff)));
        btdrv_read_rf_reg(0x8b,&val_tmp1);
        BT_DRV_TRACE(1,"chk 0x8b val_tmp1=%x\n",val_tmp1);
        btdrv_write_rf_reg(0x8d,((((0x7c4 * 1000 / (value & 0x0fff)) *  0x28 / 1000) << 10) | (val_tmp2 & 0x03ff)));
        BT_DRV_TRACE(1,"0x8d:%x\n",((((0x7c4 * 1000 / (value & 0x0fff)) *  0x28 / 1000) << 10) | (val_tmp2 & 0x03ff)));
        btdrv_read_rf_reg(0x8d,&val_tmp2);
        BT_DRV_TRACE(1,"chk 0x8d val_tmp2=%x\n",val_tmp2);
    }
    else
    {
        btdrv_write_rf_reg(0x8b,((0x9c << 8) | (val_tmp1 & 0x00ff)));
        BT_DRV_TRACE(1,"0x8b:%x\n",((0x9c << 8) | (val_tmp1 & 0x00ff)));
        btdrv_write_rf_reg(0x8d,((0x28 << 10) | (val_tmp2 & 0x03ff)));
        BT_DRV_TRACE(1,"0x8d:%x\n",((0x28 << 10) | (val_tmp2 & 0x03ff)));
    }

    BTDIGITAL_REG(0xd02201e4) = 0x00000000;

    btdrv_write_rf_reg(0x80, 0xa010);
    btdrv_write_rf_reg(0xc4, 0xffcf);//[5:4]=00,close rcosc bias
    btdrv_write_rf_reg(0x80, 0xa000);

    btdrv_write_rf_reg(0xb3,0x30f3);//[9:8]=00,pwup rcosc
}

#ifdef __PWR_FLATNESS__
#define PWR_FLATNESS_CONST_VAL                         0xE
void btdrv_2300_channel_pwr_flatness(void)
{
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
    {
        uint16_t read_value;
        uint16_t tmp_val;

        btdrv_read_rf_reg(0xc0,&read_value);
        BT_DRV_TRACE(1,"btdrv_2300_channel_pwr_flatness 0xc0=%x\n",read_value);

        read_value = (read_value & 0x0f00)>>8;//[11:8]
        int16_t calib_val = PWR_FLATNESS_CONST_VAL - read_value;
        if(calib_val<0)
        {
            BT_DRV_TRACE(2,"calib_val<0 const_val=%d,read_val=%x",PWR_FLATNESS_CONST_VAL,read_value);
            btdrv_read_rf_reg(0x92,&tmp_val);
            tmp_val &= 0xf0ff;//[11:8]
            BT_DRV_TRACE(1,"0x92=%x\n",tmp_val);
            btdrv_write_rf_reg(0x92,tmp_val);
            return;
        }
        else
        {
            BT_DRV_TRACE(2,"const_val=%d,calib_val =%x",PWR_FLATNESS_CONST_VAL,calib_val);
        }

        //write calibrated value into 0x92 register
        btdrv_read_rf_reg(0x92,&tmp_val);
        tmp_val &= 0xf0ff;
        tmp_val |= ((calib_val & 0xffff)<<8);
        BT_DRV_TRACE(1,"write reg 0x92 val=%x",tmp_val);
        btdrv_write_rf_reg(0x92,tmp_val);
    }
    else if(hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_3)
    {
        uint16_t effuse_val = 0;
        uint16_t reg_val = 0;
        //read calibrated val from efuse 0x08 register
        pmu_get_efuse(PMU_EFUSE_PAGE_BT_POWER, &effuse_val);

        effuse_val &= 0xF;//get bit[3:0]
        btdrv_read_rf_reg(0x92,&reg_val);

        BT_DRV_TRACE(2,"btdtv_rf_tx_calib preset=%02x calib %02x", reg_val,effuse_val);

        if(effuse_val !=0 )
        {
            reg_val = (reg_val&0xF0FF)|(effuse_val<<8);//REG[11:8]=EFUSE[3:0]
        }
        else
        {
            reg_val = (reg_val&0xF0FF)|(8<<8);
        }
        btdrv_write_rf_reg(0x92,reg_val);

        {
            //check trace
            reg_val = 0;
            btdrv_read_rf_reg(0x92,&reg_val);

            BT_DRV_TRACE(2,"btdtv_rf_tx_calib set reg=%02x calib %02x", reg_val,effuse_val);
        }
    }
}
#endif

///start active bt controller
void btdrv_start_bt(void)
{
    enum HAL_IOMUX_ISPI_ACCESS_T access;

    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_26M);

#if INTERSYS_DEBUG
    btdrv_trace_config(BT_CONTROLER_TRACE_TYPE_INTERSYS   |
                       BT_CONTROLER_TRACE_TYPE_CONTROLLER /*| BT_CONTROLER_FILTER_TRACE_TYPE_A2DP_STREAM*/);
#endif

#if defined(BLE_ONLY_ENABLED)
    btdrv_enable_sleep_checker(false);
#else
    btdrv_enable_sleep_checker(true);
#endif

    access = hal_iomux_ispi_access_enable(HAL_IOMUX_ISPI_MCU_RF);

#ifndef NO_SLEEP
    pmu_sleep_en(0);
#endif

    btdrv_poweron(BT_POWERON);

    btdrv_hciopen();

    btdrv_rf_init();

    btdrv_rf_init_ext();
#ifdef BT_50_FUNCTION
    btdrv_config_init_ble5();
    btdrv_ins_patch_init_50();
    btdrv_data_patch_init_50();
    btdrv_patch_en(1);

#else
    btdrv_config_init();
    //rom patch init
    btdrv_ins_patch_init();
    btdrv_data_patch_init();
    btdrv_patch_en(1);
#ifdef __HW_AGC__
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1)
    {
        btdrv_2300_hwagc_dc_cal_1();
    }
    else
    {
        btdrv_2300_hwagc_dc_cal();
    }
#endif

    btdrv_2300_rccal();
#ifdef __PWR_FLATNESS__
    btdrv_2300_channel_pwr_flatness();
#endif

#ifdef BT_XTAL_SYNC
    btdrv_bt_spi_xtal_init();
#endif
    btdrv_sync_config();

#ifdef BT_REDUCE_EMI_CFG
//   btdrv_bt_spi_rawbuf_init();
#endif
#endif

    btdrv_hcioff();

#ifndef NO_SLEEP
    pmu_sleep_en(1);
#endif

    if ((access & HAL_IOMUX_ISPI_MCU_RF) == 0)
    {
        hal_iomux_ispi_access_disable(HAL_IOMUX_ISPI_MCU_RF);
    }

    hal_trace_crash_dump_register(HAL_TRACE_CRASH_DUMP_MODULE_BT, bt_drv_reg_op_crash_dump);

    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_32K);
}

const uint8_t hci_cmd_enable_dut[] =
{
    0x01,0x03, 0x18, 0x00
};
const uint8_t hci_cmd_enable_allscan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x03
};
const uint8_t hci_cmd_disable_scan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x00
};
const uint8_t hci_cmd_enable_pagescan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x02
};
const uint8_t hci_cmd_autoaccept_connect[] =
{
    0x01,0x05, 0x0c, 0x03, 0x02, 0x00, 0x02
};
const uint8_t hci_cmd_hci_reset[] =
{
    0x01,0x03,0x0c,0x00
};


const uint8_t hci_cmd_nonsig_tx_dh1_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x04, 0x1b, 0x00
};
const uint8_t hci_cmd_nonsig_tx_2dh1_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x04, 0x36, 0x00
};
const uint8_t hci_cmd_nonsig_tx_3dh1_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x04, 0x53, 0x00
};
const uint8_t hci_cmd_nonsig_tx_2dh3_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x04, 0x6f, 0x01
};
const uint8_t hci_cmd_nonsig_tx_3dh3_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x04, 0x28, 0x02
};

const uint8_t hci_cmd_nonsig_rx_dh1_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x00, 0x1b, 0x00
};
const uint8_t hci_cmd_nonsig_rx_2dh1_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x00, 0x36, 0x00
};
const uint8_t hci_cmd_nonsig_rx_3dh1_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x00, 0x53, 0x00
};
const uint8_t hci_cmd_nonsig_rx_2dh3_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x00, 0x6f, 0x01
};
const uint8_t hci_cmd_nonsig_rx_3dh3_pn9[] =
{
    0x01, 0x87, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x00, 0x28, 0x02
};
//vco test
const uint8_t hci_cmd_start_bt_vco_test[] =
{
    0x01, 0xaa, 0xfc, 0x02, 0x00,0x02
};
const uint8_t hci_cmd_stop_bt_vco_test[] =
{
    0x01, 0xaa, 0xfc, 0x02, 0x00,0x04
};

void btdrv_testmode_start(void)
{
    *(volatile uint32_t *)0xd03503a0 |= 1;
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        btdrv_set_lpo_times();
    }
}

void btdrv_write_localinfo(char *name, uint8_t len, uint8_t *addr)
{
    uint8_t hci_cmd_write_addr[5+6] =
    {
        0x01, 0x72, 0xfc, 0x07, 0x00
    };

    uint8_t hci_cmd_write_name[248+4] =
    {
        0x01, 0x13, 0x0c, 0xF8
    };
    memset(&hci_cmd_write_name[4], 0, sizeof(hci_cmd_write_name)-4);
    memcpy(&hci_cmd_write_name[4], name, len);
    btdrv_SendData(hci_cmd_write_name, sizeof(hci_cmd_write_name));
    btdrv_delay(50);
    memcpy(&hci_cmd_write_addr[5], addr, 6);
    btdrv_SendData(hci_cmd_write_addr, sizeof(hci_cmd_write_addr));
    btdrv_delay(20);
}


void btdrv_enable_dut(void)
{
    btdrv_SendData(hci_cmd_enable_dut, sizeof(hci_cmd_enable_dut));
    btdrv_delay(20);
    btdrv_SendData(hci_cmd_enable_allscan, sizeof(hci_cmd_enable_allscan));
    btdrv_delay(20);
    btdrv_SendData(hci_cmd_autoaccept_connect, sizeof(hci_cmd_autoaccept_connect));
    btdrv_delay(20);
    bt_drv_reg_op_set_accessible_mode(3);

    btdrv_dut_mode_enable = true;
}

void btdrv_disable_scan(void)
{
    btdrv_SendData(hci_cmd_disable_scan, sizeof(hci_cmd_disable_scan));
    btdrv_delay(20);
}

static uint32_t dut_connect_status = DUT_CONNECT_STATUS_DISCONNECTED;

uint32_t btdrv_dut_get_connect_status(void)
{
    return dut_connect_status;
}

void btdrv_dut_accessible_mode_manager(const unsigned char *data)
{
    if(btdrv_dut_mode_enable)
    {
        if(data[0]==0x04&&data[1]==0x03&&data[2]==0x0b&&data[3]==0x00)
        {
            bt_drv_reg_op_set_accessible_mode(0);
            btdrv_disable_scan();
            dut_connect_status = DUT_CONNECT_STATUS_CONNECTED;
        }
        else if(data[0]==0x04&&data[1]==0x05&&data[2]==0x04&&data[3]==0x00)
        {
            btdrv_enable_dut();
            dut_connect_status = DUT_CONNECT_STATUS_DISCONNECTED;
        }
    }
}

void btdrv_hci_reset(void)
{
    btdrv_SendData(hci_cmd_hci_reset, sizeof(hci_cmd_hci_reset));
    btdrv_delay(350);
}

void btdrv_enable_nonsig_tx(uint8_t index)
{
    BT_DRV_TRACE(1,"%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh1_pn9, sizeof(hci_cmd_nonsig_tx_2dh1_pn9));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh1_pn9, sizeof(hci_cmd_nonsig_tx_3dh1_pn9));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh3_pn9, sizeof(hci_cmd_nonsig_tx_2dh1_pn9));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh3_pn9, sizeof(hci_cmd_nonsig_tx_3dh1_pn9));
    else
        btdrv_SendData(hci_cmd_nonsig_tx_dh1_pn9, sizeof(hci_cmd_nonsig_tx_dh1_pn9));
    btdrv_delay(20);

}

void btdrv_enable_nonsig_rx(uint8_t index)
{
    BT_DRV_TRACE(1,"%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh1_pn9, sizeof(hci_cmd_nonsig_rx_2dh1_pn9));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh1_pn9, sizeof(hci_cmd_nonsig_rx_3dh1_pn9));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh3_pn9, sizeof(hci_cmd_nonsig_rx_2dh1_pn9));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh3_pn9, sizeof(hci_cmd_nonsig_rx_3dh1_pn9));
    else
        btdrv_SendData(hci_cmd_nonsig_rx_dh1_pn9, sizeof(hci_cmd_nonsig_rx_dh1_pn9));
    btdrv_delay(20);
}

static bool btdrv_vco_test_running = false;
static unsigned short vco_test_reg_val_b6 = 0;
static unsigned short vco_test_reg_val_1f3 = 0;
#ifdef VCO_TEST_TOOL
static unsigned short vco_test_hack_flag = 0;
static unsigned short vco_test_channel = 0xff;

extern "C" unsigned short btdrv_get_vco_test_process_flag(void)
{
    return vco_test_hack_flag;
}

extern "C" bool btdrv_vco_test_bridge_intsys_callback(const unsigned char *data)
{
    bool status = false;
    if(data[0]==0x01 &&data[1]==0xaa&&data[2]==0xfc &&data[3]==0x02)
    {
        status = true;
        vco_test_hack_flag = data[5];
        vco_test_channel = data[4];
    }

    return status;
}

extern "C" void btdrv_vco_test_process(uint8_t op)
{
    if(op == 0x02)//vco test start
    {
        if(vco_test_channel != 0xff)
            btdrv_vco_test_start(vco_test_channel);
    }
    else if(op ==0x04)//vco test stop
    {
        btdrv_vco_test_stop();
    }
    vco_test_channel =0xff;
    vco_test_hack_flag = 0;
}
#endif

void btdrv_vco_test_start(uint8_t chnl)
{
    if (!btdrv_vco_test_running)
    {
        btdrv_vco_test_running = true;
        hal_analogif_reg_read(0xb6, &vco_test_reg_val_b6);
        hal_analogif_reg_read(0x1f3, &vco_test_reg_val_1f3);
        hal_analogif_reg_write(0x1f3, 0);
        hal_analogif_reg_write(0xb6, vco_test_reg_val_b6|(0x03<<14));
        hal_analogif_reg_write(0x1d7, 0xc4f8);

#if 1
        uint16_t tmp_val = 0;
        hal_analogif_reg_read(0xa7, &tmp_val);
        tmp_val = (tmp_val & 0xff80) | (chnl & 0x7f);
        hal_analogif_reg_write(0xa7, tmp_val);
#endif

        BTDIGITAL_REG(0xd02201e4) = (chnl & 0x7f) | 0xa0000;
        btdrv_delay(10);
        BTDIGITAL_REG(0xd02201e4) = 0;
        btdrv_delay(10);
        BTDIGITAL_REG(0xd0340020) &= (~0x7);
        BTDIGITAL_REG(0xd0340020) |= 6;
        btdrv_delay(10);
    }
}

void btdrv_vco_test_stop(void)
{
    if (btdrv_vco_test_running)
    {
        btdrv_vco_test_running = false;
        BTDIGITAL_REG(0xd02201bc) = 0;
        BTDIGITAL_REG(0xd0340020) &=(~0x7);
        if (vco_test_reg_val_b6 != 0)
        {
            hal_analogif_reg_write(0xb6, vco_test_reg_val_b6);
        }
        if (vco_test_reg_val_1f3 != 0)
        {
            hal_analogif_reg_write(0x1f3, vco_test_reg_val_1f3);
        }
        btdrv_delay(10);
    }
}


void btdrv_stop_bt(void)
{
    btdrv_poweron(BT_POWEROFF);

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

extern "C" void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param)
{
    uint8_t buff[256];
    buff[0] = 0x01;
    buff[1] = opcode & 0xff;
    buff[2] = (opcode &0xff00)>>8;
    buff[3] = cmdlen;
    if(cmdlen>0)
        memcpy(&buff[4],param,cmdlen);
    btdrv_SendData(buff,cmdlen+4);
}

void btdrv_rxdpd_sample_init(void)
{
    return;

    //0x40000048,0x03ffffff; //release btcpu reset
    BTDIGITAL_REG(0xc00003ac)=0x00060020;//turn off bt sleep
    btdrv_delay(2000);
    //0xd0220050,0x0000b7b7;
    BTDIGITAL_REG(0xd0350350)=0x70810081;
    BTDIGITAL_REG(0xd0350208)=0x8;
    BTDIGITAL_REG(0xd0350228)=0x8;

    BTDIGITAL_REG(0xd035035c)=0x50000000; // enable dc cancel
    BTDIGITAL_REG(0xd02201bc)=0x800a00c4; //rx continue for dpdsample and freq 2470
    BTDIGITAL_REG(0xd0340020)=0x010E01C5;// open ana rxon for open adc clk
    BTDIGITAL_REG(0xd0330038)=0x950d; //dpd enable 4 open modem adcclk

    btdrv_write_rf_reg(0xe0,0x470d);//fix gain
    btdrv_write_rf_reg(0x0b,0x470d);//fix gain
    btdrv_write_rf_reg(0xc7,0xf0f8);//gen single tone inside
    btdrv_write_rf_reg(0x04,0x5255);//gen single tone inside

}

void btdrv_rxdpd_sample_deinit(void)
{
    return;

//  [switch to RX normal]
    BTDIGITAL_REG(0xd02201bc) = 0x0;
    BTDIGITAL_REG(0xd0340020) = 0x010E01C0;
    BTDIGITAL_REG(0xd0330038) = 0x850d;

//  [SET inside tone off]
    btdrv_write_rf_reg(0x04,0x5154);
    btdrv_write_rf_reg(0xc7,0xf008);
    btdrv_write_rf_reg(0xe0,0x470d);
}

#define BTTX_PATTEN (1)
#define BTTX_FREQ(freq) ((freq-2402)&0x7f)

void btdrv_rxdpd_sample_init_tx(void)
{
    return;

    //0x40000048,0x03ffffff; //release btcpu reset
    BTDIGITAL_REG(0xc00003ac)=0x00060020;//turn off bt sleep
    btdrv_delay(2000);
    //0xd0220050,0x0000b7b7;

    BTDIGITAL_REG(0xd0340020)=0x010E01C7;// open TRX path
#if BTTX_PATTEN
    BTDIGITAL_REG(0xd02201bc)=0x000aff00 | BTTX_FREQ(2479); //tx continue for dpdsample
#else
    BTDIGITAL_REG(0xd02201bc)=0x000a0000 | BTTX_FREQ(2479); //tx continue for dpdsample
#endif
#ifdef CHIP_BEST1000
    BTDIGITAL_REG(0x4001f034)=0x00001000; //open p1_4 for agpio
#endif
    BTDIGITAL_REG(0xd0330038)=0x950d; //dpd enable 4 open modem adcclk
#if 0
    //monitor adcclk from p2_6
    BTDIGITAL_REG(0x4001f004)=0x02000000;
    BTDIGITAL_REG(0x40000074)=0x00f000ff;
    BTDIGITAL_REG(0xd033002c)=0x00000060;
#endif

    btdrv_write_rf_reg(0x2d,0x7fa); //open ldo for ad/da
    btdrv_write_rf_reg(0x0e,0x700); //set dac gain
    btdrv_write_rf_reg(0x0f,0x2a18);//set dac gain dr
    btdrv_write_rf_reg(0x10,0x00be);//sel dac output

#if 0
    btdrv_write_rf_reg(0x12,0x1188);
    btdrv_write_rf_reg(0x04,0x52fe);
    btdrv_write_rf_reg(0x14,0x01fc);
    btdrv_write_rf_reg(0x0c,0x6d84);
    btdrv_write_rf_reg(0x07,0x02bb);
    btdrv_write_rf_reg(0x0a,0x0223);

    btdrv_write_rf_reg(0x02,0x2694);
    btdrv_write_rf_reg(0x0b,0x402d);
    btdrv_write_rf_reg(0x10,0x00b5);
#else
    //set ad/da loop
    BTDIGITAL_REG(0xd0350308)=0x05;// digital gain * 5
    BTDIGITAL_REG(0xd0350300)=0x04;// digital gain / 16
    btdrv_write_rf_reg(0x07,0x2bd); //
    btdrv_write_rf_reg(0x02,0x2294); //
    btdrv_write_rf_reg(0x0b,0x4028);//
    btdrv_write_rf_reg(0x0c,0x6584);//
#endif
}

void btdrv_rxdpd_sample_enable(uint8_t rxon, uint8_t txon)
{
}
extern "C" void btdrv_btcore_extwakeup_irq_enable(bool on)
{
    if (on)
    {
        *(volatile uint32_t *)(0xd033003c) |= (1<<14);
    }
    else
    {
        *(volatile uint32_t *)(0xd033003c) &= ~(1<<14);
    }
}

//[26:0] 0x07ffffff
//[27:0] 0x0fffffff

uint32_t btdrv_syn_get_curr_ticks(void)
{
    uint32_t value;

    value = BTDIGITAL_REG(0xd022001C) & 0x0fffffff;
    return value;
}

extern "C" char app_tws_mode_is_master(void);
// offset btclk = Master - Slave
static int32_t btdrv_syn_get_offset_ticks(uint16_t conidx)
{
    int32_t offset;
    uint32_t local_offset;
    uint16_t offset0;
    uint16_t offset1;
    offset0 = BTDIGITAL_BT_EM(EM_BT_CLKOFF0_ADDR + conidx*110);
    offset1 = BTDIGITAL_BT_EM(EM_BT_CLKOFF1_ADDR + conidx*110);

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

}

// Clear trigger signal with software
extern "C" void  btdrv_syn_clr_trigger(void)
{
    BTDIGITAL_REG(0xd02201f0) = BTDIGITAL_REG(0xd02201f0) | (1<<31);
}


extern "C" char app_tws_mode_is_master(void);
extern "C" char app_tws_mode_is_only_mobile(void);


static void btdrv_syn_set_tg_ticks(uint32_t num, uint8_t mode)
{
    if (mode == BT_TRIG_MASTER_ROLE)
    {
        BTDIGITAL_REG(0xd02201f0) = (BTDIGITAL_REG(0xd02201f0) & 0x70000000) | (num & 0x0fffffff) | 0x10000000;
        //BT_DRV_TRACE(1,"master mode d02201f0:0x%x\n",BTDIGITAL_REG(0xd02201f0));
    }
    else
    {
        BTDIGITAL_REG(0xd02201f0) = (BTDIGITAL_REG(0xd02201f0) & 0x70000000) | (num & 0x0fffffff);
        //BT_DRV_TRACE(1,"slave mode d02201f0:0x%x\n",BTDIGITAL_REG(0xd02201f0));
    }
}

extern "C" void btdrv_syn_trigger_codec_en(uint32_t v)
{
}


extern "C" uint32_t btdrv_get_syn_trigger_codec_en(void)
{

    return BTDIGITAL_REG(0xd02201f0);

}


extern "C" uint32_t btdrv_get_trigger_ticks(void)
{

    return BTDIGITAL_REG(0xd02201f0);

}


// Can be used by master or slave
// Ref: Master bt clk
extern "C" uint32_t bt_syn_get_curr_ticks(uint16_t conhdl)
{
    int32_t curr,offset;

    curr = btdrv_syn_get_curr_ticks();
    if(conhdl>=0x80)
        offset = btdrv_syn_get_offset_ticks(btdrv_conhdl_to_linkid(conhdl));
    else
        offset = 0;
//    BT_DRV_TRACE(4,"[%s] curr(%d) + offset(%d) = %d", __func__, curr , offset,curr + offset);
    return (curr + offset) & 0x0fffffff;
}

// Can be used by master or slave
// Ref: Master bt clk
extern "C" void bt_syn_set_tg_ticks(uint32_t val,uint16_t conhdl, uint8_t mode)
{
    int32_t offset;
    if(conhdl>=0x80)
        offset = btdrv_syn_get_offset_ticks(btdrv_conhdl_to_linkid(conhdl));
    else
        offset = 0;

    if ((mode == BT_TRIG_MASTER_ROLE) && (offset !=0))
        BT_DRV_TRACE(0,"ERROR OFFSET !!");
    BT_DRV_TRACE(4,"bt_syn_set_tg_ticks val:%d num:%d mode:%d conhdl:%02x", val, val - offset, mode, conhdl);
    btdrv_syn_set_tg_ticks(val - offset, mode);
}

extern "C"  void btdrv_enable_playback_triggler(uint8_t triggle_mode)
{
    if(triggle_mode == ACL_TRIGGLE_MODE)
    {
        //clear SCO trigger
        BTDIGITAL_REG(0xd02201f0) &= (~0x40000000);
        //set ACL trigger
        BTDIGITAL_REG(0xd02201f0) |= 0x20000000;
    }
    else if(triggle_mode == SCO_TRIGGLE_MODE)
    {
        //clear ACL trigger
        BTDIGITAL_REG(0xd02201f0) &= (~0x20000000);
        //set SCO trigger
        BTDIGITAL_REG(0xd02201f0) |= 0x40000000;
    }
}

/*
bit28  1:master  0:slave
*/

extern "C"  void btdrv_set_tws_role_triggler(uint8_t tws_mode)
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





extern "C"  void btdrv_set_bt_pcm_triggler_en(uint8_t  en)
{
    if(en)
    {
        BTDIGITAL_REG(0xd022046c) &= (~0x1);
    }
    else
    {
        BTDIGITAL_REG(0xd022046c) |= 0x1;
    }
}

extern "C" void btdrv_set_bt_pcm_triggler_delay(uint8_t  delay)
{
    if(delay > 0x3f)
    {
        BT_DRV_TRACE(0,"delay is error value");
        return;
    }
    BT_DRV_TRACE(1,"0XD022045c=%x",BTDIGITAL_REG(0xd022045c));
    BTDIGITAL_REG(0xd022045c) &= ~0x7f;
    BTDIGITAL_REG(0xd022045c) |= (delay);
    BT_DRV_TRACE(1,"exit :0XD022045c=%x",BTDIGITAL_REG(0xd022045c));
}


extern "C" void btdrv_set_bt_pcm_en(uint8_t  en)
{
    if(en)
        BTDIGITAL_REG(0xd02201b0) |= 1;
    else
        BTDIGITAL_REG(0xd02201b0) &= (~1);
}


extern "C" void btdrv_set_bt_pcm_triggler_delay_reset(uint8_t  delay)
{
    return;

    if(delay > 0x3f)
    {
        BT_DRV_TRACE(0,"delay is error value");
        return;
    }
    BT_DRV_TRACE(1,"0XD022045c=%x",BTDIGITAL_REG(0xd0224024));
    BTDIGITAL_REG(0XD022045c) &= ~0x3f;
    BTDIGITAL_REG(0XD022045c) |= delay|1;
    //  BTDIGITAL_REG(0xd0224024) |= 6;  //bypass sco trig
    BT_DRV_TRACE(1,"exit :0xd022045c=%x",BTDIGITAL_REG(0xd022045c));
}

extern "C" void btdrv_set_pcm_data_ignore_crc(void)
{

    BTDIGITAL_REG(0xD0220144) &= ~0x800000;
}

extern "C" uint8_t btdrv_conhdl_to_linkid(uint16_t connect_hdl)
{
    if(connect_hdl<0x80||connect_hdl>0x83)
    {
        ASSERT(0, "ERROR Connect Handle=0x%x", connect_hdl);
        return 3;
    }

    return (connect_hdl - 0x80);
}

extern "C" void btdrv_linear_format_16bit_set(void)
{
    *(volatile uint32_t *)(0xd02201a0) |= 0x00300000;
}

extern "C" void btdrv_pcm_enable(void)
{
    *(volatile uint32_t *)(0xd02201b0) |= 0x01; //pcm enable
    if(hal_get_chip_metal_id() < HAL_CHIP_METAL_ID_2)
    {
        *(volatile uint32_t *)(0xd02201b4) |= 0x72000; //set slot
        *(volatile uint32_t *)(0xd02201b8) &= (~0x1ff);
        *(volatile uint32_t *)(0xd02201b8) |= 0x80000006;//set pcmclk 2.048m and inv pcmclk
        *(volatile uint32_t *)(0xd02201b0) |= 0x00000021;//sel stereo right channel
    }
}

extern "C" void btdrv_pcm_disable(void)
{
    *(volatile uint32_t *)(0xd02201b0) &= 0xfffffffe; //pcm disable
}

// Trace tport
static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_tport[] =
{
    {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
};

int btdrv_host_gpio_tport_open(void)
{
    uint32_t i;

    for (i=0; i<sizeof(pinmux_tport)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP); i++)
    {
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&pinmux_tport[i], 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pinmux_tport[i].pin, HAL_GPIO_DIR_OUT, 0);
    }
    return 0;
}

int btdrv_gpio_port_set(int port)
{
    hal_gpio_pin_set((enum HAL_GPIO_PIN_T)pinmux_tport[port].pin);
    return 0;
}

int btdrv_gpio_tport_clr(int port)
{
    hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)pinmux_tport[port].pin);
    return 0;
}
void btdrv_set_powerctrl_rssi_low(uint16_t rssi)
{
}

extern void bt_drv_set_music_link(uint8_t link_id);

void btdrv_enable_dual_slave_configurable_slot_mode(bool isEnable,
        uint16_t activeDevHandle, uint8_t activeDevRole,
        uint16_t idleDevHandle, uint8_t idleDevRole)
{
    if(isEnable)
    {
        bt_drv_set_music_link(activeDevHandle-0x80);

    }
    else
    {
        bt_drv_set_music_link(0xff);
    }
}

#if defined(TX_RX_PCM_MASK)
uint8_t  btdrv_is_pcm_mask_enable(void)
{
    return 0;
}
#endif

#if defined(CVSD_BYPASS)
void btdrv_cvsd_bypass_enable(void)
{
    BTDIGITAL_REG(0xD0220144) |= 0x5555;
    // BTDIGITAL_REG(0xD02201E8) |= 0x04000000; //test sequecy
    BTDIGITAL_REG(0xD02201A0) &= ~(1<<7); //soft cvsd
//   BTDIGITAL_REG(0xD02201b8) |= (1<<31); //revert clk
}
#endif

void btdrv_enable_rf_sw(int rx_on, int tx_on)
{
    hal_iomux_set_bt_rf_sw(rx_on, tx_on);
    BTDIGITAL_REG(0xD0220050) = (BTDIGITAL_REG(0xD0220050) & ~0xFF) | 0xA6;
}

