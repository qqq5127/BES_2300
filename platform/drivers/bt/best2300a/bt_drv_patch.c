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
#include "bt_drv.h"
#include "bt_drv_interface.h"
#include "bt_drv_2300a_internal.h"
#include "string.h"

#define INS_PATCH_MAX_NUM  (64)

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

/////////////2300a t0 instruction mode patch///////////////////////
const uint32_t bes2300a_t0_ins_data_0[] =
{
    0x601a4a01,
    0xbecaf600,
    0x000fc802,/*8d0c*/
};

///patch for open sync irq
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch0 =
{
    0,
#if defined(__NEW_SWAGC_MODE__) || defined(__BLE_NEW_SWAGC_MODE__)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_0),
    0x00009a98,
    0xb932f1ff,
    0xc0008d00,
    (uint8_t *)bes2300a_t0_ins_data_0
};

const uint32_t bes2300a_t0_ins_data_1[] =
{
    0x681b4b1a,/*8d40*/
    0xeba33b08,
    0xeb070308,
    0x49190247,
    0x1a9b5c8a,
    0xea23b25b,
    0x4a1773e3,
    0x1203fb82,
    0x72e3eba2,
    0xeb02b251,
    0xeba30242,
    0xb2db0342,
    0xbf842b02,
    0xb2493101,
    0xb2493101,
    0x408a2202,
    0x490eb292,
    0xf423680b,
    0xf423237f,
    0xf6406378,
    0x428270ff,
    0x4602bf28,
    0x2302ea43,
    0xbf00600b,
    0x46504639,
    0xf9fef5fc,
    0xbb4af5fc,
    0xc0008db0,
    0x00000003,
    0xc0000414,
    0x2aaaaaab,
    0xd03502c0,/*8dbc*/
};

///ADJUST MODEM REG
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch1 =
{
    1,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_1),
    0x00005450,
    0xbC76f203,
    0xc0008d40,
    (uint8_t *)bes2300a_t0_ins_data_1
};


const uint32_t bes2300a_t0_ins_data_2[] =
{
    0x4b036a1f,/*8e00*/
    0x1ac0681b,
    0xbf002300,
    0xb89af626,
    0xc0008e14,
    0x00000005,/*8e14*/
};

///-n control the packet receive
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch2=
{
    2,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_2),
    0x0002ef40,
    0xbf5ef1d9,
    0xc0008e00,
    (uint8_t *)bes2300a_t0_ins_data_2
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch3 =
{
    3,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002ef0c,
    0xe00e30ae,
    0,
    0
};

const uint32_t bes2300a_t0_ins_data_4[] =
{
    0xbf00380c,
    0xff0bf5f7,
    0xbb0cf624,/*8e28*/
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch4=
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_4),
    0x0002d440,
    0xbceef1db,
    0xc0008e20,
    (uint8_t *)bes2300a_t0_ins_data_4
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x00033da0,
    0x8ff8e8bd,
    0,
    NULL,
};//remove lmp flush

const uint32_t bes2300a_t0_ins_data_6[] =
{
    0x4b084649,
    0x1031f853,
    0xb94b798b,
    0xf8324a06,
    0xea6f3016,
    0xea6f4343,
    0xb29b4353,
    0xb9d8f626,
    0xbd5af626,
    0xc0006818,
    0xd021162e /*8e58*/
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch6=
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_6),
    0x0002f1f0,
    0xbe1ef1d9,
    0xc0008e30,
    (uint8_t *)bes2300a_t0_ins_data_6
};//repair EV3,3EV3 remove no lmp accept send

const uint32_t bes2300a_t0_ins_data_7[] =
{
    0xfdd8f633,
    0xf04f42b8,
    0xbf0c0200,
    0x4b034b02,
    0xbf00701a,
    0xb8a0f625,
    0xc00006c1,
    0xc00006c0/*8e7c*/
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch7=
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_7),
    0x0002dfb0,
    0xbf56f1da,
    0xc0008e60,
    (uint8_t *)bes2300a_t0_ins_data_7
};//sw agc rssi reduce



/////update new agc when link changed
const uint32_t bes2300a_t0_ins_data_8[] =
{
    0xf8524a06,
    0x429a2025,
    0xf625d901,
    0x4a04ba8a,
    0xf0436813,
    0x60130304,
    0xba8ff625,
    0xc0007488,
    0xd02204d0,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch8=
{
    8,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_8),
    0x0002e3d8,
    0xbd72f1da,
    0xc0008ec0,
    (uint8_t *)bes2300a_t0_ins_data_8
};

const uint32_t bes2300a_t0_ins_data_9[] =
{
    0x68134a03,
    0x0304f043,
    0xbf006013,
    0xbd47f623,
    0xd02204d0,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch9=
{
    9,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_9),
    0x0002ca14,
    0xbab4f1dc,
    0xc0008f80,
    (uint8_t *)bes2300a_t0_ins_data_9
};

const uint32_t bes2300a_t0_ins_data_10[] =
{

    0x46484629,   /*0xc0008fa0L*/
    0x400fe92d,   /*0xc0008fa4L*/
    0xf804f000,   /*0xc0008fa8L*/
    0x400fe8bd,   /*0xc0008facL*/
    0xbaaaf626,   /*0xc0008fb0L*/
    0x4b07b508,   /*0xc0008fb4L*/
    0xf013681b,   /*0xc0008fb8L*/
    0xd0080f02,   /*0xc0008fbcL*/
    0x68134a04,   /*0xc0008fc0L*/
    0x0302f043,   /*0xc0008fc4L*/
    0x21176013,   /*0xc0008fc8L*/
    0x4b02b2c0,   /*0xc0008fccL*/
    0xbd084798,   /*0xc0008fd0L*/
    0xd02204d0,   /*0xc0008fd4L*/
    0xa002e719,   /*0xc0008fd8L*/


};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch10=
{
    10,
    BTDRV_PATCH_INACT,
    sizeof(bes2300a_t0_ins_data_10),
    0x0002f504,
    0xbd4cf1d9,
    0xc0008fa0,
    (uint8_t *)bes2300a_t0_ins_data_10
};

const uint32_t bes2300a_t0_ins_data_11[] =
{
    0xf8da9804,
    0x9a0c1084,
    0xb4084673,
    0xbf00464b,
    0xf806f000,
    0x469ebc08,
    0x0f01f014,
    0xbc90f626,
    0x4c08b510,
    0x1a416824,
    0x4178f021,
    0x7ffaf5b1,
    0xf103d805,
    0xf8934340,
    0x2b1c36d0,
    0x4610d901,
    0xbd1047a0,
    0xc00003c8,/*9034*/
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch11=
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_11),
    0x0002f92c,
    0xbb60f1d9,//0xbb60f1d9
    0xc0008ff0,
    (uint8_t *)bes2300a_t0_ins_data_11
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch12 =
{
    12,
    BTDRV_PATCH_INACT,
    0,
    0X00031c6c,
    0x9301009b,
    0,
    NULL,
};//enlarge sniff rx drift


const uint32_t bes2300a_t0_ins_data_13[] =
{
    0xf8534b04,
    0x78037026,
    0x0301f003,
    0x3044f887,
    0xb869f615,
    0xc000683c,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch13=
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_13),
    0x0001e120,
    0xbf8ef1ea,//0xbb60f1d9
    0xc0009040,
    (uint8_t *)bes2300a_t0_ins_data_13
};//sniff req not accept

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch14 =
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0X0002f718,
    0x4b1de007,
    0,
    NULL,
};//close soft bit

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch15 =
{
    15,
    BTDRV_PATCH_ACT,
    0,
    0X0002c190,
    0x233230e4,
    0,
    NULL,
};


const uint32_t bes2300a_t0_ins_data_16[] =
{
    0x400fe92d,   /*0xc0009070L*/
    0xf804f000,   /*0xc0009074L*/
    0x400fe8bd,   /*0xc0009078L*/
    0xbaf9f622,   /*0xc000907cL*/
    0x68184b07,   /*0xc0009080L*/
    0x4a076a83,   /*0xc0009084L*/
    0xb2898811,   /*0xc0009088L*/
    0x2271f240,   /*0xc000908cL*/
    0xf303fb02,   /*0xc0009090L*/
    0xbf28428b,   /*0xc0009094L*/
    0xf8c0460b,   /*0xc0009098L*/
    0x47703012,   /*0xc000909cL*/
    0xc0006e5c,   /*0xc00090a0L*/
    0xc0008520,   /*0xc00090a4L*/

};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch16=
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_16),
    0x0002b66c,
    0xbd00f1dd,
    0xc0009070,
    (uint8_t *)bes2300a_t0_ins_data_16
};//pscan acl schedule confilct


const uint32_t bes2300a_t0_ins_data_17[] =
{
    0x68984b06,
    0x3108f8d3,
    0x28054418,
    0xf61cd801,
    0x2311bde1,
    0x3031f88d,
    0xbdfdf61c,
    0xc00063b8,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch17=
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_17),
    0x00025cc0,
    0xb9f6f1e3,
    0xc00090b0,
    (uint8_t *)bes2300a_t0_ins_data_17
};//classfication limit


const uint32_t bes2300a_t0_ins_data_18[] =
{
    0x400ee92d,   /*0xc00090e0L*/
    0xf804f000,   /*0xc00090e4L*/
    0x400ee8bd,   /*0xc00090e8L*/
    0xbe02f625,   /*0xc00090ecL*/
    0x68134a05,   /*0xc00090f0L*/
    0x0302f043,   /*0xc00090f4L*/
    0x4b046013,   /*0xc00090f8L*/
    0x30018818,   /*0xc00090fcL*/
    0x8018b280,   /*0xc0009100L*/
    0xbf004770,   /*0xc0009104L*/
    0xd02204d0,   /*0xc0009108L*/
    0xc00006a4,   /*0xc000910cL*/
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_patch18=
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_18),
    0x0002ece8,
    0xb9faf1da,
    0xc00090e0,
    (uint8_t *)bes2300a_t0_ins_data_18
};//ld_ecc_data_test_process 0xd02204d0 bit1



/////2300a t0 patch
static const uint32_t best2300a_t0_ins_patch_config[] =
{
    19,
    (uint32_t)&bes2300a_t0_ins_patch0,
    (uint32_t)&bes2300a_t0_ins_patch1,
    (uint32_t)&bes2300a_t0_ins_patch2,
    (uint32_t)&bes2300a_t0_ins_patch3,
    (uint32_t)&bes2300a_t0_ins_patch4,
    (uint32_t)&bes2300a_t0_ins_patch5,
    (uint32_t)&bes2300a_t0_ins_patch6,
    (uint32_t)&bes2300a_t0_ins_patch7,
    (uint32_t)&bes2300a_t0_ins_patch8,
    (uint32_t)&bes2300a_t0_ins_patch9,
    (uint32_t)&bes2300a_t0_ins_patch10,
    (uint32_t)&bes2300a_t0_ins_patch11,
    (uint32_t)&bes2300a_t0_ins_patch12,
    (uint32_t)&bes2300a_t0_ins_patch13,
    (uint32_t)&bes2300a_t0_ins_patch14,
    (uint32_t)&bes2300a_t0_ins_patch15,
    (uint32_t)&bes2300a_t0_ins_patch16,
    (uint32_t)&bes2300a_t0_ins_patch17,
    (uint32_t)&bes2300a_t0_ins_patch18,
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
    BT_PATCH_WR(BTDRV_PATCH_INS_REMAP_ADDR_START + ins_patch_p->patch_index*sizeof(uint32_t), ins_patch_p->patch_remap_value);
    if(ins_patch_p->patch_length != 0)  //have ram patch data
    {
        memcpy((uint8_t *)ins_patch_p->patch_start_address,ins_patch_p->patch_data,ins_patch_p->patch_length);
    }

    uint32_t remap_addr =  ins_patch_p->patch_remap_address | 1;
    BT_PATCH_WR(BTDRV_PATCH_INS_COMP_ADDR_START + ins_patch_p->patch_index*sizeof(uint32_t), remap_addr);
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

extern void btdrv_function_patch_init(void);

void btdrv_ins_patch_init(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0)
    {
        for(uint8_t i=0; i<best2300a_t0_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300a_t0_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300a_t0_ins_patch_config[i+1]);
        }
    }
    else
    {
        ASSERT(0, "%s:error chip id=%d", __func__, hal_get_chip_metal_id());
    }

    btdrv_function_patch_init();
}

//////////////////////////////patch enable////////////////////////

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

void btdrv_ins_patch_disable(uint8_t index)
{
    //disable patch, clear remap address and jump instruction
    BT_PATCH_WR(BTDRV_PATCH_INS_COMP_ADDR_START+index*sizeof(uint32_t), 0);
    BT_PATCH_WR(BTDRV_PATCH_INS_REMAP_ADDR_START + index*sizeof(uint32_t), 0);
}
/////////////2300a t0 test mode patch///////////////////////

const uint32_t bes2300a_t0_ins_data_testmode_0[] =
{
    0x601a4a01,
    0xbecaf600,
    0x000fc802,
};

///patch for open sync irq
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch0 =
{
    0,
#if defined(__NEW_SWAGC_MODE__) || defined(__BLE_NEW_SWAGC_MODE__)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_0),
    0x00009a98,
    0xb932f1ff,
    0xc0008d00,
    (uint8_t *)bes2300a_t0_ins_data_testmode_0
};

///patch for disable ld acl rx agc control
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch1 =
{
    1,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002ee74,
    0xbf00e033,
    0,
    0
};


///don't set the cs rx gain
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch2 =
{
    2,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000051dc,
    0xbf000104,
    0,
    0
};



const uint32_t bes2300a_t0_ins_data_testmode_3[] =
{
    0x681c4b06,
    0xd0012c07,
    0xe0002407,
    0x601c2406,
    0xbf004801,
    0xba42f5fc,
    0xd02204e4,
    0xc0008d30,
    0x00000007,
};

///change gain every packet
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch3 =
{
    3,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_3),
    0x000051a8,
    0xbdb2f203,
    0xc0008d10,
    (uint8_t *)bes2300a_t0_ins_data_testmode_3
};

const uint32_t bes2300a_t0_ins_data_testmode_4[] =
{
    0x681b4b1a,
    0xeba33b08,
    0xeb070308,
    0x49190247,
    0x1a9b5c8a,
    0xea23b25b,
    0x4a1773e3,
    0x1203fb82,
    0x72e3eba2,
    0xeb02b251,
    0xeba30242,
    0xb2db0342,
    0xbf842b02,
    0xb2493101,
    0xb2493101,
    0x408a2202,
    0x490eb292,
    0xf423680b,
    0xf423237f,
    0xf6406378,
    0x428270ff,
    0x4602bf28,
    0x2302ea43,
    0xbf00600b,
    0x46504639,
    0xf9fef5fc,
    0xbb4af5fc,
    0xc0008db0,
    0x00000003,
    0xc0000414,
    0x2aaaaaab,
    0xd03502c0,
};

///ADJUST MODEM REG
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch4 =
{
    4,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_4),
    0x00005450,
    0xbC76f203,
    0xc0008d40,
    (uint8_t *)bes2300a_t0_ins_data_testmode_4
};

#if 0
const uint32_t bes2300a_t0_ins_data_testmode_5[] =
{
    0x50b0f887,
    0x681b4b02,
    0xbf001aed,
    0xb8d4f626,
    0xc0008e14,
    0x00000005,
};

///-n control the packet receive
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch5=
{
    5,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_5),
    0x0002efb4,
    0xbf24f1d9,
    0xc0008e00,
    (uint8_t *)bes2300a_t0_ins_data_testmode_5
};
#else
const uint32_t bes2300a_t0_ins_data_testmode_5[] =
{
    0x4b036a1f,
    0x1ac0681b,
    0xbf002300,
    0xb89af626,
    0xc0008e14,
    0x00000005,
};

///-n control the packet receive
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch5=
{
    5,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_5),
    0x0002ef40,
    0xbf5ef1d9,
    0xc0008e00,
    (uint8_t *)bes2300a_t0_ins_data_testmode_5
};

#endif

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch6 =
{
    6,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002ef0c,
    0xe00e30ae,
    0,
    0
};


//lld_pdu_rx_handler:testmode disable swagc in lld_pdu_rx_handler func
const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch7 =
{
    7,
#ifdef __BLE_NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0004984c,
    0xf0402b22,
    0,
    0
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00000864,
    0x26004c0f,
    0,
    0
};////rwip_env.sleep_enable=false after hci reset

const uint32_t bes2300a_t0_ins_data_testmode_9[] =
{
    0x4618b510,
    0x2b1a880b,
    0x4a15d91e,
    0x2048f8b2,
    0xd31e429a,
    0xf5b2884a,
    0xd31c7fa4,
    0x68244c11,
    0x4c0fb964,
    0x404af8b4,
    0xd3164294,
    0xf8a24a0c,
    0x884b3044,
    0x3046f8a2,
    0xe0062100,
    0x2490f244,
    0xd9f342a2,
    0xe0002112,
    0xf6372112,
    0x2000feb5,
    0x2112bd10,
    0x2112e7f9,
    0x2112e7f7,
    0xbf00e7f5,

    0xc00087c8,//llm_le_env
    0xc0008e88,//bv11c
    0x00000000,


};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch9=
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_testmode_9),
    0x0003f890,
    0xbac6f1c9,
    0xc0008e20,
    (uint8_t *)bes2300a_t0_ins_data_testmode_9
};//hci_le_wr_suggted_dft_data_len_cmd_handler

const uint32_t bes2300a_t0_ins_data_testmode_10[] =
{
    0x20174606,
    0xfcbcf637,
    0x210cb130,
    0x46287031,
    0xfe9af637,
    0xba8df637,
    0xbf0079e3,
    0xba82f637,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch10=
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_testmode_10),
    0x000403b0,
    0xbd6ef1c8,
    0xc0008e90,
    (uint8_t *)bes2300a_t0_ins_data_testmode_10
};//hci_le_set_priv_mode_cmd_handler


const uint32_t bes2300a_t0_ins_data_testmode_11[] =
{
    0xbf00380c,
    0xfec3f5f7,
    0xbac4f624,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch11=
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes2300a_t0_ins_data_testmode_11),
    0x0002d440,
    0xbd36f1db,
    0xc0008eb0,
    (uint8_t *)bes2300a_t0_ins_data_testmode_11
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x00033da0,
    0x8ff8e8bd,
    0,
    NULL,
};//remove lmp flush



const uint32_t bes2300a_t0_ins_data_testmode_13[] =
{
    0xf8524a06,
    0x429a2025,
    0xf625d901,
    0x4a04ba8a,
    0xf0436813,
    0x60130304,
    0xba8ff625,
    0xc0007488,
    0xd02204d0,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch13=
{
    13,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_13),
    0x0002e3d8,
    0xbd72f1da,
    0xc0008ec0,
    (uint8_t *)bes2300a_t0_ins_data_testmode_13
};


const uint32_t bes2300a_t0_ins_data_testmode_14[] =
{
    0xd8052902,
    0xd8012c01,
    0x601d4b02,
    0xba6cf62f,
    0xba70f62f,
    0xc0008f08,
    0x000000ff,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch14=
{
    14,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_14),
    0x000383d4,
    0xbd8cf1d0,
    0xc0008ef0,
    (uint8_t *)bes2300a_t0_ins_data_testmode_14
};


const uint32_t bes2300a_t0_ins_data_testmode_15[] =
{
    0x681b4b0b,
    0xd00d2bff,
    0xfb02226e,
    0x4a09f303,
    0xf0035a9b,
    0xea43030f,
    0x4b051404,
    0x601a22ff,
    0xbe4cf625,
    0xbf00236e,
    0xf300fb03,
    0xbe84f625,
    0xc0008f08,
    0xd0211208,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch15=
{
    15,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_15),
    0x0002ec44,
    0xb964f1da,
    0xc0008f10,
    (uint8_t *)bes2300a_t0_ins_data_testmode_15
};

const uint32_t bes2300a_t0_ins_data_testmode_16[] =
{
    0x68184b07,
    0xd00328ff,
    0x4b06b2c0,
    0xe000601d,
    0xf5fc4620,
    0x4b02f951,
    0x601a22ff,
    0xb815f626,
    0xc0008f08,
    0xc0008f78,
    0x00000000
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch16=
{
    16,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_16),
    0x0002ef94,
    0xbfdcf1d9,
    0xc0008f50,
    (uint8_t *)bes2300a_t0_ins_data_testmode_16
};

const uint32_t bes2300a_t0_ins_data_testmode_17[] =
{
    0x68134a03,
    0x0304f043,
    0xbf006013,
    0xbd47f623,
    0xd02204d0,
};

const BTDRV_PATCH_STRUCT bes2300a_t0_ins_testmode_patch17=
{
    17,
#ifdef __NEW_SWAGC_MODE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300a_t0_ins_data_testmode_17),
    0x0002ca14,
    0xbab4f1dc,
    0xc0008f80,
    (uint8_t *)bes2300a_t0_ins_data_testmode_17
};



static const uint32_t ins_patch_2300a_t0_config_testmode[] =
{
    18,
    (uint32_t)&bes2300a_t0_ins_testmode_patch0,
    (uint32_t)&bes2300a_t0_ins_testmode_patch1,
    (uint32_t)&bes2300a_t0_ins_testmode_patch2,
    (uint32_t)&bes2300a_t0_ins_testmode_patch3,
    (uint32_t)&bes2300a_t0_ins_testmode_patch4,
    (uint32_t)&bes2300a_t0_ins_testmode_patch5,
    (uint32_t)&bes2300a_t0_ins_testmode_patch6,
    (uint32_t)&bes2300a_t0_ins_testmode_patch7,
    (uint32_t)&bes2300a_t0_ins_testmode_patch8,
    (uint32_t)&bes2300a_t0_ins_testmode_patch9,
    (uint32_t)&bes2300a_t0_ins_testmode_patch10,
    (uint32_t)&bes2300a_t0_ins_testmode_patch11,
    (uint32_t)&bes2300a_t0_ins_testmode_patch12,
    (uint32_t)&bes2300a_t0_ins_testmode_patch13,
    (uint32_t)&bes2300a_t0_ins_testmode_patch14,
    (uint32_t)&bes2300a_t0_ins_testmode_patch15,
    (uint32_t)&bes2300a_t0_ins_testmode_patch16,
    (uint32_t)&bes2300a_t0_ins_testmode_patch17,

};

extern void btdrv_function_testmode_patch_init(void);

void btdrv_ins_patch_test_init(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;

    btdrv_patch_en(0);

    for(uint8_t i=0; i< INS_PATCH_MAX_NUM; i++)
    {
        btdrv_ins_patch_disable(i);
    }
    bt_drv_reg_op_for_test_mode_disable();

    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 )
    {
        for(uint8_t i=0; i<ins_patch_2300a_t0_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2300a_t0_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2300a_t0_config_testmode[i+1]);
        }
    }
    else
    {
        ASSERT(0, "%s:error chip id=%d", __func__, hal_get_chip_metal_id());
    }
    btdrv_function_testmode_patch_init();

    btdrv_patch_en(1);
}
