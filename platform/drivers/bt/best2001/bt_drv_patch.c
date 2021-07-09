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
#include "hal_chipid.h"
#include "hal_intersys.h"
#include "bt_drv.h"
#include "bt_drv_interface.h"
#include "bt_drv_2001_internal.h"
#include "bt_drv_internal.h"
#include <stdio.h>
#include "string.h"

///enable m4 patch func
#define BTDRV_PATCH_EN_REG                  0xa01ffe00


//instruction patch compare src address
#define BTDRV_PATCH_INS_COMP_ADDR_START     0xa01ffe00

#define BTDRV_PATCH_INS_REMAP_ADDR_START    0xa01fff00



#define BTDRV_PATCH_ACT     0x1
#define BTDRV_PATCH_INACT   0x0

typedef struct
{
    uint8_t patch_index;            //patch position
    uint8_t patch_state;            //is patch active
    uint16_t patch_length;          //patch length 0:one instrution replace  other:jump to ram to run more instruction
    uint32_t patch_remap_address;   //patch occured address
    uint32_t patch_remap_value;     //patch replaced instuction
    uint32_t patch_start_address;   //ram patch address for lenth>0
    uint8_t *patch_data;            //ram patch date for length >0

} BTDRV_PATCH_STRUCT;


/***************************************************************************
 *
 * instruction patch Information
 *
 * BT ROM Chip Version:1402 T1
 *
 * __BT_ONE_BRING_TWO__ function
 *
 ****************************************************************************/

const BTDRV_PATCH_STRUCT bes2001_t1_ins_patch0 =
{
    0,
    BTDRV_PATCH_INACT,
    0,
    0x00030d48,
    0xbf00bf00,
    0,
    NULL
};//

const uint32_t bes2001_patch1_ins_data_1[] =
{
    0xf8934b09,
    0xf0133084,
    0xd0050f06,
    0xf6364630,
    0x4604fb9d,
    0xe0052600,
    0xf8934b03,
    0xbf003084,
    0xbceff635,
    0xbd21f635,
    
    0xc000652c,
};

const BTDRV_PATCH_STRUCT bes2001_t1_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes2001_patch1_ins_data_1),
    0x0003c3fc,
    0xbb00f1ca,
    0xc0006a00,
    (uint8_t *)bes2001_patch1_ins_data_1
};


/////1402 t1 patch
static const uint32_t best2001_t1_ins_patch_config[] =
{
    2,
    (uint32_t)&bes2001_t1_ins_patch0,
    (uint32_t)&bes2001_t1_ins_patch1,

};

/*****************************************************************************
 Prototype    : btdrv_ins_patch_write
 Description  : bt driver instruction patch write
 Input        : BTDRV_PATCH_STRUCT *ins_patch_p
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2018/12/6
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void btdrv_ins_patch_write(BTDRV_PATCH_STRUCT *ins_patch_p)
{
    uint32_t remap_addr;
    remap_addr =   ins_patch_p->patch_remap_address | 1;

    *(uint32_t *)(BTDRV_PATCH_INS_REMAP_ADDR_START + ins_patch_p->patch_index*4) = ins_patch_p->patch_remap_value;

    if(ins_patch_p->patch_length != 0)  //have ram patch data
    {
        memcpy((uint8_t *)ins_patch_p->patch_start_address,ins_patch_p->patch_data,ins_patch_p->patch_length);
    }

    *(uint32_t *)(BTDRV_PATCH_INS_COMP_ADDR_START + ins_patch_p->patch_index*4) |= remap_addr;
}
/*****************************************************************************
 Prototype    : btdrv instruction patch init
 Description  : btdrv_ins_patch_init
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2018/12/6
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void btdrv_ins_patch_init(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;


    //else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2)
    {
        for(uint8_t i=0; i<best2001_t1_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2001_t1_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2001_t1_ins_patch_config[i+1]);
            }
        }

        btdrv_patch_en(1);
    }
}



void btdrv_ins_patch_disable(uint8_t index)
{

    *(volatile uint32_t *)(BTDRV_PATCH_INS_COMP_ADDR_START+index*4) &= ~0x1;

}


/*****************************************************************************
 Prototype    : btdrv_patch_en
 Description  : patch enable function
 Input        : uint8_t en
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2018/12/6
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void btdrv_patch_en(uint8_t en)
{

    if(en)
    {
        *(volatile uint32_t *)BTDRV_PATCH_EN_REG |= 0x80000000;
    }
    else
    {
        *(volatile uint32_t *)BTDRV_PATCH_EN_REG &= ~0x80000000;
    }
}




/*****************************************************************************
CHIP ID                : 2001
Metal id               : 0
BT controller tag      : t1
*****************************************************************************/



const BTDRV_PATCH_STRUCT bes2001_t1_ins_patch_testmode_0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x00000840,
    0x26004c0e,
    0,
    NULL,
};////rwip_env.sleep_enable=false after hci reset

const uint32_t bes2001_t1_ins_patch_data_testmode_1[] =
{
    0xbf282f07,
    0x4a012707,
    0xbe8af63e,
    0xd0210514,
};/* 6990-699c */

const BTDRV_PATCH_STRUCT bes2001_t1_ins_patch_testmode_1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes2001_t1_ins_patch_data_testmode_1),
    0x000456ac,
    0xb970f1c1,
    0xc0006990,
    (uint8_t *)bes2001_t1_ins_patch_data_testmode_1
};////ble test mode high pwr disconnect problem

static const uint32_t ins_patch_2001_t1_config_testmode[] =
{
    2,
    (uint32_t)&bes2001_t1_ins_patch_testmode_0,
    (uint32_t)&bes2001_t1_ins_patch_testmode_1,
};



void btdrv_ins_patch_test_init(void)
{

    //return;
    const BTDRV_PATCH_STRUCT *ins_patch_p;

    // else if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2)
    {
        btdrv_patch_en(0);

        for (int i = 0; i < 56; i += 1)
        {
            btdrv_ins_patch_disable(i);
        }


        for(uint8_t i = 0; i < ins_patch_2001_t1_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2001_t1_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2001_t1_config_testmode[i+1]);
        }

        btdrv_patch_en(1);
    }
}
