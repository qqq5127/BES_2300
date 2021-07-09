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
#include "plat_types.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "bt_drv.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "cmsis.h"
#include "hal_cmu.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_btdump.h"
#include "tgt_hardware.h"
#include "bt_drv_internal.h"
#include "bt_drv_2001_internal.h"
#include "hal_chipid.h"
#include "bt_drv_interface.h"
#include "iqcorrect.h"

#define AUTO_CAL                                0

#ifndef BT_RF_MAX_XTAL_TUNE_PPB
// Default 10 ppm/bit or 10000 ppb/bit
#define BT_RF_MAX_XTAL_TUNE_PPB                 10000
#endif

#ifndef BT_RF_XTAL_TUNE_FACTOR
// Default 0.2 ppm/bit or 200 ppb/bit
#define BT_RF_XTAL_TUNE_FACTOR                  200
#endif

#undef DEFAULT_XTAL_FCAP
#define DEFAULT_XTAL_FCAP                       0xFF00

#define XTAL_FCAP_NORMAL_SHIFT                  0
#define XTAL_FCAP_NORMAL_MASK                   (0x1FF << XTAL_FCAP_NORMAL_SHIFT)
#define XTAL_FCAP_NORMAL(n)                     BITFIELD_VAL(XTAL_FCAP_NORMAL, n)

#define BT_XTAL_CMOM_DR                         (1 << 13)

#define RF_REG_XTAL_CMOM_DR                     0x58

#define RF_REG_XTAL_FCAP                        0x35
#define ISPI_WFRF_REG(reg)                      (((reg) & 0xFFF) | 0x3000)
#define btdrv_read_wifi_reg(reg,val)            hal_analogif_reg_read(ISPI_WFRF_REG(reg),val)
#define btdrv_write_wifi_reg(reg,val)           hal_analogif_reg_write(ISPI_WFRF_REG(reg),val)

/* ljh add for sync modify rf RF_REG_XTAL_FCAP value*/
#define SPI_TRIG_RX_NEG_TIMEOUT                 MS_TO_TICKS(3)

#define SPI_TRIG_NEG_TIMEOUT                    SPI_TRIG_RX_NEG_TIMEOUT


enum BIT_OFFSET_CMD_TYPE_T
{
    BIT_OFFSET_CMD_STOP = 0,
    BIT_OFFSET_CMD_START,
    BIT_OFFSET_CMD_ACK,
};

static uint16_t xtal_fcap = DEFAULT_XTAL_FCAP;
static uint16_t init_xtal_fcap = DEFAULT_XTAL_FCAP;



//rampup start     1--->2->3 rampup ok
//rampdown start 3-->2-->1-->0  rampdown ok


//3a06=3b3f
//for pa@1.8v
struct bt_drv_tx_table_t
{
    uint16_t tbl[16][3];
};

const struct bt_drv_tx_table_t rf_tx_pwr_tbl[] =
{
    {{
        }
    },
    {{
        }
    },
    {{
        }
    },
    {{
        }
    },
    {{
        }
    }
};

struct RF_SYS_INIT_ITEM
{
    uint16_t reg;
    uint16_t set;
    uint16_t mask;
    uint16_t delay;
};

const uint16_t rf_init_tbl[][3] =
{
    {0x03,0x2a01,0},
    {0x04,0x1238,0},
    {0x07,0x4124,0},
    {0x08,0x00d8,0},
    {0x09,0x1a22,0}, // 0x3e62
    {0x0a,0x3fc0,0},
    {0x0b,0x7fff,0},
    {0x0c,0x3efe,0},
    {0x0d,0x0724,0},
#if 0
    {0x0f,0xae22,0},
    {0x10,0x8c2c,0},
#endif
#if 0
    {0x11,0x0412,0},
#else
    {0x11,0x0402,0},
#endif
    {0x14,0x06c8,0},
    {0x15,0x300c,0},
    {0x1b,0x0f0a,0},
    {0x1c,0x3010,0},
    {0x1d,0x3010,0},
    {0x2c,0x0210,0},
#if 0
    {0x2f,0x13ff,0},
#else
    {0x2f,0x03ff,0},
#endif
    {0x36,0x008c,0},
    {0x37,0x001f,0},
    {0x38,0x0dff,0},
    {0x41,0x50c7,0},
    {0x42,0x6c09,0},
#if 1
    {0x43,0xdc73,0},
#else
    {0x43,0x1c71,0},
#endif
    {0x46,0x0974,0},
    {0x66,0xffff,0},
    {0x88,0xba00,0},
    {0x89,0x0600,0},
    {0xaa,0xfde0,0},
    {0xc0,0x0063,0},
    {0xc2,0x0039,0},
    {0xc3,0x001c,0},
    {0xc5,0x04bc,0},
    {0xcc,0x001f,0},
    {0xd3,0x0202,0},
    {0xd6,0x0205,0},
    {0xd8,0x0240,0},
    {0x0e,0x38bb,0},
};

const uint16_t rf_init_tbl_1[][3] =
{
    {0x01,0x9dbb,0},
    {0x03,0x2a01,0},
    {0x04,0x1238,0},
    {0x06,0x59b3,0},
    {0x07,0x4124,0},
    {0x08,0x00d0,0}, // max tx flt BW
    {0x09,0x1a24,0}, // 0x3e62
    {0x0a,0x3fc0,0},
    {0x0b,0x7ff8,0}, // tx inband flatness
    {0x0c,0x22fe,0}, // vco current
    {0x0d,0x0724,0},
#if 0
    {0x11,0x0412,0},
#else
    {0x11,0x0402,0},
#endif
    {0x14,0x06c8,0},
    {0x15,0x300c,0},
    {0x1b,0x0f0a,0},
    {0x1c,0x2d10,0}, // 0x3010->0x2d10 by luobin
    {0x1d,0x2d10,0}, // 0x3010->0x2d10 by luobin
    {0x2e,0x00ea,0}, // max tx flt current
    {0x2c,0x0210,0},
#if 0
    {0x2f,0x13ff,0},
#else
    {0x2f,0x03ff,0},
#endif
    {0x36,0x008c,0},
    {0x37,0x001f,0},
    {0x38,0x0dff,0},
    {0x41,0x50c7,0},
    {0x42,0x6c09,0},
#if 1
    {0x43,0xdc73,0},
#else
    {0x43,0x1c71,0},
#endif
    {0x46,0x0974,0},
    {0x64,0x9034,0}, //fastlock en
    {0x66,0xffff,0},
    {0x88,0xba00,0},
    {0x89,0x0600,0},
    {0xaa,0xfde0,0},
    {0xc0,0x0063,0},
    {0xc2,0x0039,0},
    {0xc3,0x001c,0},
    {0xc5,0x04bc,0},
    {0xcc,0x001f,0},
    {0xd3,0x0202,0},
    {0xd6,0x0205,0},
    {0xd7,0x0a00,0}, // 0x0900 -> 0x0a00 luobin: improve RX in high channels
    {0xd8,0x0240,0},
    {0x0e,0x38bf,0}, //jiangpeng, 0x38bb -> 0x38bf 3M_DEVM fail

#ifndef __HW_AGC__
    {0x01,0x993B,0},  //SW_AGC_EN
#ifdef __1402_3_AGC__
    {0x5a,0x001c,0},
    {0x5b,0x000f,0},
    {0x5c,0x0007,0},
    {0x6e,0x003f,0},
    {0x6f,0x003f,0},
    {0x70,0x0030,0},
#elif defined(__1402_3_AGC_for_test_mode)
    {0x5a,0x001c,0},
    {0x5b,0x000f,0},
    {0x5c,0x0007,0},
    {0x6e,0x003f,0},
    {0x6f,0x003e,0},
    {0x70,0x0030,0},
#else
    {0x5a,0x001c,0},  //Rx gain section_1
    {0x5b,0x001d,0},
    {0x5c,0x001f,0},
    {0x5d,0x000f,0},
    {0x5e,0x0007,0},
    {0x5f,0x0003,0},
    {0x60,0x0003,0},
    {0x61,0x0003,0},
    {0x6e,0x00Bf,0},  //Rx gain section_2
    {0x6f,0x00bf,0},
    {0x70,0x00bf,0},
    {0x71,0x00bf,0},
    {0x72,0x00bf,0},
    {0x73,0x00bf,0},
    {0x74,0x00af,0},
    {0x75,0x009f,0},
#endif
#endif

    {0x13,0x2100,0},
    //{0x16,0x1087,0},
    //{0x17,0x0321,0},

    //{0x01,0xf93b},
    //{0x02,0x013c},
    //{0x11,0x0782},
    {0x12,0x16ff},
    //{0x21,0x1c0b},
    {0xd7,0x0400},
    {0xd5,0x01c7},
};

uint32_t btdrv_rf_get_max_xtal_tune_ppb(void)
{
    return BT_RF_MAX_XTAL_TUNE_PPB;
}


uint32_t btdrv_rf_get_xtal_tune_factor(void)
{
    return BT_RF_XTAL_TUNE_FACTOR;
}

void btdrv_rf_init_xtal_fcap(uint32_t fcap)
{
    xtal_fcap = SET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL, fcap);
    btdrv_write_wifi_reg(RF_REG_XTAL_FCAP, xtal_fcap);
    init_xtal_fcap = xtal_fcap;
}

uint32_t btdrv_rf_get_init_xtal_fcap(void)
{
    return GET_BITFIELD(init_xtal_fcap, XTAL_FCAP_NORMAL);
}

uint32_t btdrv_rf_get_xtal_fcap(void)
{
    return GET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL);
}

void btdrv_rf_set_xtal_fcap(uint32_t fcap, uint8_t is_direct)
{
    xtal_fcap = SET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL, fcap);
    btdrv_write_wifi_reg(RF_REG_XTAL_FCAP, xtal_fcap);
}
int btdrv_rf_xtal_fcap_busy(uint8_t is_direct)
{
    return 0;
}

void btdrv_rf_bit_offset_track_enable(bool enable)
{
}

uint32_t btdrv_rf_bit_offset_get(void)
{
    return 0;
}

uint16_t btdrv_rf_bitoffset_get(uint8_t conidx)
{
    uint16_t localVal = BTDIGITAL_BT_EM(EM_BT_BITOFF_ADDR + conidx * BT_EM_SIZE);
    return (localVal >> 0);
}

void btdrv_rf_log_delay_cal(void)
{
    unsigned short read_value;
    unsigned short write_value;
    unsigned short read_value_tmp;
    unsigned short read_value_tmp_reg05;
    unsigned short read_value_tmp_reg01;
    unsigned short read_value_tmp_reg02;
    unsigned short read_value_tmp_reg11;
    unsigned short read_value_tmp_reg21;

    //unsigned short read_value_tmp3;
    uint32_t val_record;

    BT_DRV_TRACE(0,"btdrv_rf_log_delay_cal\n");

    ///improve CI perfomance
    btdrv_read_rf_reg(0x01,&read_value_tmp_reg01);
    btdrv_write_rf_reg(0x01, (read_value_tmp_reg01 | 0xe000));

    btdrv_read_rf_reg(0x02,&read_value_tmp_reg02);
    btdrv_write_rf_reg(0x02, ((read_value_tmp_reg02 & 0xc7ff) | 0x2038));

    btdrv_read_rf_reg(0x05,&read_value_tmp_reg05);
    //read_value_tmp_reg05 = ((read_value_tmp_reg05 & 0x0070) | 0x0070);//bit6:4
    btdrv_write_rf_reg(0x05, read_value_tmp_reg05 | 0x0070);

    btdrv_read_rf_reg(0x11,&read_value_tmp_reg11);
 //   btdrv_write_rf_reg(0x11, (read_value_tmp_reg11 | 0x0380));
    btdrv_write_rf_reg(0x11, (0x0382));

    btdrv_read_rf_reg(0x21,&read_value_tmp_reg21);
    btdrv_write_rf_reg(0x21, (0x1c0b));
    ///improve CI perfomance

    btdrv_write_rf_reg(0x43, 0x0000);

    //btdrv_read_rf_reg(0xd7, &read_value_tmp2);
    //BT_DRV_TRACE(1,"0xd7 read=%x\n",read_value_tmp2);
    //btdrv_write_rf_reg(0xd7, 0x0f00);

    btdrv_read_rf_reg(0x42, &read_value_tmp);
    BT_DRV_TRACE(1,"0x42 read=%x\n",read_value_tmp);
    read_value_tmp = ((read_value_tmp & 0xfe1f) | 0x01e0);
    BT_DRV_TRACE(1,"0x42 write=%x\n",read_value_tmp);
    btdrv_write_rf_reg(0x42, read_value_tmp);

    //btdrv_read_rf_reg(0xa7, &read_value_tmp3);
    //BT_DRV_TRACE(1,"0xa7 read=%x\n",read_value_tmp3);
    //btdrv_write_rf_reg(0xa7, 0x0027);

    //val_record = *(volatile uint32_t*)(0xd0340020);
    //BT_DRV_TRACE(1,"0xd0340020 = %x\n",val_record);
    //*(volatile uint32_t*)(0xd0340020) = 0x030e01c1;

    val_record = *(volatile uint32_t*)(0xd02201e4);
    *(volatile uint32_t*)(0xd02201e4) = 0x000a00a7;

    btdrv_delay(1);

    btdrv_read_rf_reg(0xb4, &read_value);
    BT_DRV_TRACE(1,"0xb4 read=%x\n",read_value);
    //write_value = (1<<14) | (1<<15) | (1<<1) | ((read_value & 0x1000)>>12) | ((read_value & 0x0fc0)<<2) | ((read_value & 0x003f)<<2);
    write_value = (1<<14) | (1<<15) | (1<<1) | ((read_value & 0x1000)>>12) | ((((read_value & 0x0fc0)>>6) + 2)<<8) | (((read_value & 0x003f) + 2)<<2);////update by xulicheng 2019.10.11
    BT_DRV_TRACE(1,"0x43 write=%x\n",write_value);
    btdrv_write_rf_reg(0x43, write_value);

    btdrv_delay(1);

    btdrv_write_rf_reg(0x21, (read_value_tmp_reg21));

    BT_DRV_TRACE(1,"0x42 write=%x\n",(read_value_tmp & 0xfe1f));
    btdrv_write_rf_reg(0x42, (read_value_tmp & 0xfe1f));

    //btdrv_write_rf_reg(0xd7, read_value_tmp2);

    //btdrv_write_rf_reg(0xa7, read_value_tmp3);

    //*(volatile uint32_t*)(0xd0340020) = val_record;
    *(volatile uint32_t*)(0xd02201e4) = val_record;

}

void btdrv_rf_rx_gain_adjust_regdump(void)
{
}

void btdrv_rf_rx_gain_adjust_default(void)
{
}

void btdrv_rf_rx_gain_adjust_lowgain(void)
{
}

void btdrv_rf_rx_gain_adjust_req(uint32_t user, bool lowgain)
{
}

uint8_t btdrv_rf_init(void)
{
    const uint16_t (*p_rf_tx_init_tbl)[3];
    uint32_t tbl_size;
    uint8_t i = 0;

    if(0)// (hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0)
    {
        p_rf_tx_init_tbl = &rf_init_tbl[0];
        tbl_size = sizeof(rf_init_tbl)/sizeof(rf_init_tbl[0]);
    }
    else
    {
        p_rf_tx_init_tbl = &rf_init_tbl_1[0];
        tbl_size = sizeof(rf_init_tbl_1)/sizeof(rf_init_tbl_1[0]);
    }

    for(; i<tbl_size; i++)
    {
        BT_DRV_TRACE(1,"rf init %d",i);
        btdrv_delay(10);
        btdrv_write_rf_reg(p_rf_tx_init_tbl[i][0],p_rf_tx_init_tbl[i][1]);
        if(p_rf_tx_init_tbl[i][2] !=0)
        {
            btdrv_delay(p_rf_tx_init_tbl[i][2]);//delay
        }
        //btdrv_read_rf_reg(p_rf_tx_init_tbl[i][0],&rf_value);
        //BT_DRV_TRACE(2,"reg=%x,v=%x",p_rf_tx_init_tbl[i][0],rf_value);
    }

    btdrv_rf_log_delay_cal();
//    btdrv_spi_trig_init();

    return 1;
}

void btdrv_rf_txcal(void)
{
    //unsigned short rfreg_18 = 0;
    unsigned short rfreg_28 = 0;
    unsigned short rfreg_04 = 0;
    unsigned short rfreg_d3 = 0;
    unsigned short rfreg_d4 = 0;
    unsigned short rfreg_27 = 0;
    unsigned short rfreg_05 = 0;
    unsigned short rfreg_2a = 0;
    unsigned short rfreg_42 = 0;
    unsigned short rfreg_06 = 0;
    unsigned short rfreg_aa = 0;
    unsigned short rfreg_03 = 0;

    uint32_t dig_d0350208 = 0;
    uint32_t dig_d0350228 = 0;
    uint32_t dig_d0330038 = 0;
    uint32_t dig_d0350364 = 0;
    uint32_t dig_d0350360 = 0;
    uint32_t dig_d035037c = 0;
    uint32_t dig_d02201e4 = 0;
    uint32_t dig_d0350218 = 0;
    uint32_t dig_d0350240 = 0;
    uint32_t dig_d0330034 = 0;

    // Backup registers
    //btdrv_read_rf_reg(0x18, &rfreg_18);
    btdrv_read_rf_reg(0x28, &rfreg_28);
    btdrv_read_rf_reg(0x04, &rfreg_04);
    btdrv_read_rf_reg(0xd3, &rfreg_d3);
    btdrv_read_rf_reg(0xd4, &rfreg_d4);
    btdrv_read_rf_reg(0x27, &rfreg_27);
    btdrv_read_rf_reg(0x05, &rfreg_05);
    btdrv_read_rf_reg(0x2a, &rfreg_2a);
    btdrv_read_rf_reg(0x42, &rfreg_42);
    btdrv_read_rf_reg(0x06, &rfreg_06);
    btdrv_read_rf_reg(0xaa, &rfreg_aa);
    btdrv_read_rf_reg(0x03, &rfreg_03);

    dig_d0350208 = BTDIGITAL_REG(0xd0350208);
    dig_d0350228 = BTDIGITAL_REG(0xd0350228);
    dig_d0330038 = BTDIGITAL_REG(0xd0330038);
    dig_d0350364 = BTDIGITAL_REG(0xd0350364);
    dig_d0350360 = BTDIGITAL_REG(0xd0350360);
    dig_d035037c = BTDIGITAL_REG(0xd035037c);
    dig_d02201e4 = BTDIGITAL_REG(0xd02201e4);
    dig_d0350218 = BTDIGITAL_REG(0xd0350218);
    dig_d0350240 = BTDIGITAL_REG(0xd0350240);
    dig_d0330034 = BTDIGITAL_REG(0xd0330034);

    // Write registers
    //btdrv_write_rf_reg(0x18,0x29f8);
    btdrv_write_rf_reg(0x28,0x8023);
    btdrv_write_rf_reg(0x04,0x1278);
    btdrv_write_rf_reg(0xd3,0x0cc1);
    btdrv_write_rf_reg(0xd4,0x0c34);
    btdrv_write_rf_reg(0x27,0x3003);//adc en
    btdrv_write_rf_reg(0x05,0x9fc5);
    btdrv_write_rf_reg(0x2a,0x3000);
    btdrv_write_rf_reg(0x42,0xec09);
    btdrv_write_rf_reg(0x06,0x5937);
    btdrv_write_rf_reg(0xaa,0xfea0);
    btdrv_write_rf_reg(0x03,0x0b01);

    BTDIGITAL_REG(0xd0350208)=0xffffffff;//set dump clock 52M
    BTDIGITAL_REG(0xd0350228)=0xffffffff;//set dump clock 52M
    BTDIGITAL_REG(0xd0330038)=0x0030010d;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    BTDIGITAL_REG(0xd0350364)=0x002eb948;
    BTDIGITAL_REG(0xd0350360)=0x007fc240;
    BTDIGITAL_REG(0xd035037c)=0x20415;
    BTDIGITAL_REG(0xd02201e4)=0x000a002b;
    BTDIGITAL_REG(0xd0350218)=0x404;
    BTDIGITAL_REG(0xd0350240)=0x2007;
    BTDIGITAL_REG(0xd0330034)=0x6da3a3aa;
    hal_sys_timer_delay(MS_TO_TICKS(10));

#if !defined(DSP_USE_AUDMA) || defined(CHIP_BEST2001_DSP)
    hal_btdump_clk_enable();
    hal_btdump_enable();
    bt_iqimb_ini();
    bt_iqimb_test_ex(0);
    hal_btdump_disable();
    hal_btdump_clk_disable();
#endif

    // Restore registers
    //btdrv_write_rf_reg(0x18,rfreg_18);
    btdrv_write_rf_reg(0x28,rfreg_28);
    btdrv_write_rf_reg(0x04,rfreg_04);
    btdrv_write_rf_reg(0xd3,rfreg_d3);
    btdrv_write_rf_reg(0xd4,rfreg_d4);
    btdrv_write_rf_reg(0x27,rfreg_27);
    btdrv_write_rf_reg(0x05,rfreg_05);
    btdrv_write_rf_reg(0x2a,rfreg_2a);
    btdrv_write_rf_reg(0x42,rfreg_42);
    btdrv_write_rf_reg(0x06,rfreg_06);
    btdrv_write_rf_reg(0xaa,rfreg_aa);
    btdrv_write_rf_reg(0x03,rfreg_03);

    BTDIGITAL_REG(0xd0350208)=dig_d0350208;
    BTDIGITAL_REG(0xd0350228)=dig_d0350228;
    BTDIGITAL_REG(0xd0330038)=dig_d0330038;
    hal_sys_timer_delay(MS_TO_TICKS(1));
    BTDIGITAL_REG(0xd0350364)=dig_d0350364;
    BTDIGITAL_REG(0xd0350360)=dig_d0350360;
    BTDIGITAL_REG(0xd035037c)=dig_d035037c;
    BTDIGITAL_REG(0xd02201e4)=dig_d02201e4;
    BTDIGITAL_REG(0xd0350218)=dig_d0350218;
    BTDIGITAL_REG(0xd0350240)=dig_d0350240;
    BTDIGITAL_REG(0xd0330034)=dig_d0330034;
    hal_sys_timer_delay(MS_TO_TICKS(10));

    // if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_1)
    {
        unsigned short rfreg_42 = 0;
        unsigned short rfreg_a7 = 0;
        btdrv_read_rf_reg(0x42, &rfreg_42);
        btdrv_write_rf_reg(0x42, rfreg_42&(~1));   // clear bit 0
        btdrv_read_rf_reg(0xa7, &rfreg_a7);
        btdrv_write_rf_reg(0xa7, rfreg_a7|(2<<8)); // a7[10:8] tx gain index
        btdrv_write_rf_reg(0x76, 0x000c); // min tx gain by walker 2019.02.26
        btdrv_write_rf_reg(0x77, 0x0009); // mid tx gain by walker 2019.02.26
        btdrv_write_rf_reg(0x78, 0x0002); // max tx gain by walker 2019.02.26
    }
}

uint16_t swagc_[][3] =
{
#ifdef __SWAGC_2_LEVEL__
    {0x5a,0x001C,0},  //Rx gain section_1
    {0x5b,0x001C,0},
    {0x6e,0x003f,0},  //Rx gain section_2
    {0x6f,0x0034,0},
#else
    {0x5a,0x001c,0},  //Rx gain section_1
    {0x5b,0x001d,0},
    {0x5c,0x001f,0},
    {0x5d,0x000f,0},
    {0x5e,0x0007,0},
    {0x5f,0x0003,0},
    {0x60,0x0003,0},
    {0x61,0x0003,0},

    {0x6e,0x00Bf,0},  //Rx gain section_2
    {0x6f,0x00bf,0},
    {0x70,0x00bf,0},
    {0x71,0x00bf,0},
    {0x72,0x00bf,0},
    {0x73,0x00bf,0},
    {0x74,0x00af,0},
    {0x75,0x009f,0},
#endif
};

uint16_t hwagc_[][3] =
{
#ifdef __SWAGC_2_LEVEL__
    {0x5a,0x001c,0},  //Rx gain section_1
    {0x5b,0x000f,0},
    {0x6e,0x003f,0},  //Rx gain section_2
    {0x6f,0x003e,0},
#else
    {0x5a,0x001c,0},  //Rx gain section_1
    {0x5b,0x001d,0},
    {0x5c,0x001f,0},
    {0x5d,0x000f,0},
    {0x5e,0x0007,0},
    {0x5f,0x0003,0},
    {0x60,0x0003,0},
    {0x61,0x0003,0},

    {0x6e,0x00Bf,0},  //Rx gain section_2
    {0x6f,0x00bf,0},
    {0x70,0x00bf,0},
    {0x71,0x00bf,0},
    {0x72,0x00bf,0},
    {0x73,0x00bf,0},
    {0x74,0x00af,0},
    {0x75,0x009f,0},
#endif
};

void btdrv_init_hwagc_default(void)
{
    uint16_t (*p_rf_tx_init_tbl)[3];
    uint32_t tbl_size;
    uint8_t i = 0;

    p_rf_tx_init_tbl = &hwagc_[0];
    tbl_size = sizeof(hwagc_)/sizeof(hwagc_[0]);

    for(i = 0; i<tbl_size; i++)
    {
        btdrv_read_rf_reg(p_rf_tx_init_tbl[i][0],&p_rf_tx_init_tbl[i][1]);
    }

}


void btdrv_switch_agc_mode(enum BT_WORK_MODE_T mode)
{
#ifdef __HYBIRD_AGC__
    uint16_t (*p_rf_tx_init_tbl)[3];
    uint32_t tbl_size;
    static bool mode_bak = true;
    uint16_t i = 0;

    if(mode_bak != mode)
    {
        mode_bak = mode;
        uint32_t lock = int_lock_global();
        if(mode)
        {
            *(volatile uint8_t *)(0xc00041EB) = 1; //hw_agc
            p_rf_tx_init_tbl = &hwagc_[0];
            tbl_size = sizeof(hwagc_)/sizeof(hwagc_[0]);
            btdrv_write_rf_reg(0x01, 0x9D3B); // max tx gain by walker 2019.02.26
        }
        else
        {
            *(volatile uint8_t *)(0xc00041EB) = 0; //sw_agc
            p_rf_tx_init_tbl = &swagc_[0];
            tbl_size = sizeof(swagc_)/sizeof(swagc_[0]);
            btdrv_write_rf_reg(0x01, 0x993B); // max tx gain by walker 2019.02.26
        }
        for( i=0; i< tbl_size; i++ )
        {
            btdrv_write_rf_reg(p_rf_tx_init_tbl[i][0],p_rf_tx_init_tbl[i][1]);
        }

        int_unlock_global(lock);
    }
#endif
}
