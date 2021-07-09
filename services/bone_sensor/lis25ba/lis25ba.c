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
#include <stdlib.h>
#include <string.h>

#include "hal_trace.h"
#include "hal_i2c.h"
#include "hal_iomux.h"

#define IS25_DEV_ADDR             (0x19)

struct I2C_CTRL_CONFIG_T{
    enum HAL_I2C_ID_T i2c_id;
    uint32_t is_init;
};
struct DEV_REG_DEF
{
    uint8_t reg_addr;
    uint8_t val;
};

static struct I2C_CTRL_CONFIG_T POSSIBLY_UNUSED i2c_ctrl_config = {HAL_I2C_ID_NUM, 0};
// static uint32_t I2S_TDM_BUF_ALIGN i2s_tdm_rx_buf[I2S_TDM_FRAME_NUM][I2S_TDM_FRAME_SIZE/4];
// static uint32_t rf_frame_num = I2S_TDM_FRAME_NUM;

static struct DEV_REG_DEF reg_def[] =
 {
    // reg 0x26:
    // bit 7: reserved
    // bit 6: reserved
    // bit 5: PD Device in disabled mode. Default value: 1
    //        (0: Normal mode; 1: Disabled mode: minimum power consumption, I2C still active)
    // bit 4: reserved
    // bit 3: ST Self-test mode. Default value: 0
    //        (0: disable; 1: enable)
    // bit 2 reserved
    // bit 1 reserved
    // bit 0 reserved
    {0x26,0},
    // reg 0x2e:
    // bit 7:TDM_pd TDM enable. Default value: 1.
    // (0: TDM on; 1: TDM off)
    // bit 6:Delayed TDM delayed configuration. Default value: 1.
    // (0: TDM no delayed configuration; 1: TDM delayed configuration)
    // bit 5:data_valid TDM data valid. Default value: 1.
    // (0: data valid on the rise edge of BCLK; 1: data valid on the falling edge of BCLK)
    // bit 4:mapping TDM mapping. Default value: 1.
    // bit(0: AXEX --> SLOT0; AXEY --> SLOT1; AXEZ --> SLOT2;
    //     1: AXEX --> SLOT4; AXEY --> SLOT5; AXEZ --> SLOT6)
    // bit 3:reserved.
    // bit 2-1:WCLK_fq [0:1] TDM clock frequencies. Default value: 00.
    //       (00: WCLK = 8 kHz;
    // 01: WCLK = 16 kHz;
    // 10: WCLK = 24 kHz)
    // bit 0:reserved.
    {0x2e,0x02},
    // AXISZ_EN AXISY_EN AXISX_EN reserved reserved Interp_byp reserved ODR_AUTO_EN
    // bit 7: AXISZ_EN Z-axis enable. Default value: 1.
    //         (0: disabled; 1: enabled)
    // bit 6: AXISY_EN Y-axis enable. Default value: 1.
    //         (0: disabled; 1: enabled)
    // bit 5: AXISX_EN X-axis enable. Default value: 1.
    //         (0: disabled; 1: enabled)
    // bit 4: reserved.
    // bit 3: reserved.
    // bit 2: Interp_byp Interpolator bypass. Default value: 0.
    //        (0: 24 kHz linear interpolation mode - interpolator is active;
    //         1: 24 kHz repeated sample mode - interpolator is bypassed)
    // bit 1: reserved.
    // bit 0: ODR_AUTO_EN ODR AUTO enable. Default value: 1.
    //        (0: ODR and BCLK auto disabled(1); 1: ODR and BCLK auto enabled(2))
#if 1
    // X/Y/Z
    // {0x2f,0xe1},
    // X/Y
    {0x2f, 0xe1 & 0x7f},
    // X
    // {0x2f, 0xe1 & 0x3f},
    // Y
    // {0x2f, 0xe1 & 0x5f},
#endif
    // {0x2f,0x41},{0x2f,0x21}
 };

static void i2c_pin_init(enum HAL_I2C_ID_T id)
{
    TRACE(2,"%s, id=%d\n", __func__, id);
#if defined(CHIP_BEST2300) || defined(CHIP_BEST2300P)
    if (id == HAL_I2C_ID_0) {
        hal_iomux_set_i2c0();
    } else if (id == HAL_I2C_ID_1) {
        hal_iomux_set_i2c1();
    }
#endif
}

// I2C Initialization.
// i2c_id: i2c id.
// cfg: i2c config.
static int32_t i2c_init(enum HAL_I2C_ID_T i2c_id, struct HAL_I2C_CONFIG_T *cfg)
{
    uint32_t ret;

    i2c_pin_init(i2c_id);
    // I2C open.
    ret = (int32_t)hal_i2c_open(i2c_id, cfg);
    if (ret) {
        TRACE(3,"%s:%d,hal_i2c_open failed, ret = %d.",
              __func__,__LINE__,ret);
        return (int32_t)ret;
    }
    else{
        TRACE(2,"%s:%d,i2c open ok.",
              __func__,__LINE__);
    }
    i2c_ctrl_config.i2c_id = i2c_id;
    i2c_ctrl_config.is_init = 1;

    return 0;
}

static int32_t i2c_deinit(enum HAL_I2C_ID_T i2c_id)
{
    uint32_t ret;

    i2c_pin_init(i2c_id);

    ret = (int32_t)hal_i2c_close(i2c_id);
    if (ret)
    {
        TRACE(2,"[%s] i2s close failed, ret = %d.", __func__, ret);
        return (int32_t)ret;
    }
    else
    {
        TRACE(1,"[%s] i2c close ok.", __func__);
    }
    i2c_ctrl_config.i2c_id = 0;
    i2c_ctrl_config.is_init = 0;

    return 0;
}

// Set sensor register.
// dev_addr:  device address.
// reg_def[]: cmd list.
// len: list length.
static int32_t i2c_set_reg(uint32_t dev_addr,struct DEV_REG_DEF reg_def[],uint32_t len)
{
    enum HAL_I2C_ID_T i2c_id;
    uint8_t buf[2];
    uint32_t i;
    int32_t ret;

    if(i2c_ctrl_config.is_init != 1)
    {
        TRACE(2,"%s:%d,i2c i2s_tdm_ctrl uninit!",
                __func__,__LINE__);
        return -1;
    }
    i2c_id = i2c_ctrl_config.i2c_id;
    for(i = 0; i < len; i++)
    {
        buf[0] = reg_def[i].reg_addr;
        buf[1] = reg_def[i].val;
        ret = hal_i2c_send(i2c_id, dev_addr, buf, 1, 1, 0, 0);
        if (ret) {
            TRACE(4,"%s:%d,i2c send failed,dev_addr = 0x%x,ret = %d\n",
                  __func__,__LINE__,dev_addr,ret);
            goto _func_fail;
        }
        else
        {
            TRACE(5,"%s:%d,send i2c ok, dev_addr = 0x%x,buf[0] = 0x%x,buf[1] = 0x%x.",
                  __func__,__LINE__,dev_addr,buf[0],buf[1]);
        }
        buf[0] = reg_def[i].reg_addr;
        buf[1] = 0x0;

        ret = hal_i2c_recv(i2c_id, dev_addr, buf, 1, 1, HAL_I2C_RESTART_AFTER_WRITE, 0, 0);
        if (ret) {
            TRACE(6,"%s:%d,i2c recv failed,dev_addr =0x%x, ,buf[0] = 0x%x,buf[1] = 0x%x,ret = %d.\n",
                   __func__, __LINE__, dev_addr, buf[0], buf[1], ret);
            goto _func_fail;
        }
        else
        {
            TRACE(5,"%s:%d,i2c recv ok, dev_addr = 0x%x, buf[0] = 0x%x, buf[1] = 0x%x.\n",
                  __func__, __LINE__,dev_addr, buf[0],buf[1]);
        }
    }
    return 0;
_func_fail:
    return 1;
}


// Check sensor. send the command 'who am i'.
// dev_addr:  device address.
// reg_addr: register address.
// def_val:  chip id.
static int32_t who_am_i(uint32_t dev_addr,uint8_t reg_addr, uint8_t def_val)
{
    enum HAL_I2C_ID_T i2c_id;
    uint8_t buf[2];
    int32_t ret;

    if(i2c_ctrl_config.is_init != 1)
    {
        TRACE(2,"%s:%d,i2c i2s_tdm_ctrl uninit!",
                __func__,__LINE__);
        return -1;
    }
    i2c_id = i2c_ctrl_config.i2c_id;
    buf[0] = reg_addr;
    buf[1] = 0x0;

    ret = hal_i2c_recv(i2c_id, dev_addr, buf, 1, 1, HAL_I2C_RESTART_AFTER_WRITE, 0, 0);
    if (ret) {
        TRACE(6,"%s:%d,i2c recv failed,dev_addr =0x%x, ,buf[0] = 0x%x,buf[1] = 0x%x,ret = %d.\n",
              __func__, __LINE__, dev_addr, buf[0], buf[1], ret);
        ret = -2;
    }
    else
    {
        TRACE(5,"%s:%d,i2c recv ok, dev_addr = 0x%x, buf[0] = 0x%x, buf[1] = 0x%x.\n",
              __func__, __LINE__,dev_addr, buf[0],buf[1]);
        if(buf[1] == def_val)
        {
            TRACE(2,"%s:%d,check chip id ok.\n",
                  __func__, __LINE__);
            ret = 0;
        }
        else
        {
            TRACE(4,"%s:%d, check chip id fail!chip id = 0x%x , def_val = 0x%x.\n",
                   __func__, __LINE__,buf[1],def_val);
            ret = 1;
        }
    }
    return ret;
}

int lis25ba_init(void)
{
    struct HAL_I2C_CONFIG_T i2c_cfg;
    int32_t ret;

    // i2s tmd ctrl init.
    i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
    i2c_cfg.use_dma  = 0;
    i2c_cfg.use_sync = 1;
    i2c_cfg.speed = 20000;
    i2c_cfg.as_master = 1;
    ret = i2c_init(HAL_I2C_ID_0, &i2c_cfg);
    if(ret){
        TRACE(3,"%s:%d, i2c_init fail! ret = %d.",__func__,__LINE__,ret);
        return 1;
    }

    TRACE(2,"%s:%d, i2c_init done.",__func__,__LINE__);

    ret = who_am_i(IS25_DEV_ADDR,0x0f,0x20);
    if(ret){
        TRACE(3,"%s:%d, who_am_i fail! ret = %d.",__func__,__LINE__,ret);
        return 2;
    }
    TRACE(2,"%s:%d, who_am_i done.",__func__,__LINE__);
    ret = i2c_set_reg(IS25_DEV_ADDR,reg_def,sizeof(reg_def)/sizeof(struct DEV_REG_DEF));
    if(ret){
        TRACE(3,"%s:%d, i2c_set_reg fail! ret = %d.",__func__,__LINE__,ret);
        return 3;
    }
    TRACE(2,"%s:%d, i2c_set_reg done.",__func__,__LINE__);

    return 0;
}

int lis25ba_deinit(void)
{
    i2c_deinit(HAL_I2C_ID_0);

    return 0;
}

