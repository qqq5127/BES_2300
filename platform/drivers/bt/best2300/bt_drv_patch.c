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
#include "bt_drv_2300_internal.h"

extern void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length);

///enable m4 patch func
#define BTDRV_PATCH_EN_REG   0xe0002000

//set m4 patch remap adress
#define BTDRV_PATCH_REMAP_REG    0xe0002004

////instruction patch compare src address
#define BTDRV_PATCH_INS_COMP_ADDR_START   0xe0002008

#define BTDRV_PATCH_INS_REMAP_ADDR_START   0xc0000100

////data patch compare src address
#define BTDRV_PATCH_DATA_COMP_ADDR_START   0xe00020e8

#define BTDRV_PATCH_DATA_REMAP_ADDR_START   0xc00001e0



#define BTDRV_PATCH_ACT   0x1
#define BTDRV_PATCH_INACT   0x0



typedef struct
{
    uint8_t patch_index;   //patch position
    uint8_t patch_state;   //is patch active
    uint16_t patch_length;     ///patch length 0:one instrution replace  other:jump to ram to run more instruction
    uint32_t patch_remap_address;   //patch occured address
    uint32_t patch_remap_value;      //patch replaced instuction
    uint32_t patch_start_address;    ///ram patch address for lenth>0
    uint8_t *patch_data;                  //ram patch date for length >0

} BTDRV_PATCH_STRUCT;

///////////////////ins  patch ..////////////////////////////////////



const uint32_t bes2300_patch0_ins_data[] =
{
    0x68134a04,
    0x3300f423,
    0x46206013,
    0xfa48f628,
    0xbcbdf618,
    0xd02200a4
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data),
    0x0001ed88,
    0xbb3af1e7,
    0xc0006400,
    (uint8_t *)bes2300_patch0_ins_data
};/////test mode



const uint32_t bes2300_patch1_ins_data[] =
{
    0x52434313,
    0x03fff003,
    0xd0012b04,
    0x47702000,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch1_ins_data),
    0x00003044,
    0xb9ecf203,
    0xc0006420,
    (uint8_t *)bes2300_patch1_ins_data
};//inc power



const uint32_t bes2300_patch2_ins_data[] =
{
    0xf0035243,
    0x2b0003ff,
    0x2000d001,
    0x20014770,
    0xbf004770,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch2_ins_data),
    0x00003014,
    0xba14f203,
    0xc0006440,
    (uint8_t *)bes2300_patch2_ins_data
};//dec power


const BTDRV_PATCH_STRUCT bes2300_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    0,
    0x00000494,
    0xbD702000,
    0,
    NULL
};//sleep


const uint32_t bes2300_patch4_ins_data[] =
{
    0x0008f109,
    0xbf0060b0,
    0xba30f626,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_data),
    0x0002c8c8,
    0xbdcaf1d9,
    0xc0006460,
    (uint8_t *)bes2300_patch4_ins_data
};//ld data tx


const BTDRV_PATCH_STRUCT bes2300_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x00019804,
    0xe0092b01,
    0,
    NULL
};//max slot req



const uint32_t bes2300_patch6_ins_data[] =
{
    0x0301f043,
    0xd1012b07,
    0xbc8af624,
    0xbc78f624,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6=
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_data),
    0x0002ad88,
    0xbb72f1db,
    0xc0006470,
    (uint8_t *)bes2300_patch6_ins_data
};//ld data tx 3m



const BTDRV_PATCH_STRUCT bes2300_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    0,
    0x0003520c,
    0xbf00e006,
    0,
    NULL
};///disable for error


const BTDRV_PATCH_STRUCT bes2300_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00035cf4,
    0xbf00e038,
    0,
    NULL
};//bt address ack

const BTDRV_PATCH_STRUCT bes2300_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x000273a0,
    0x789a3100,
    0,
    NULL
};///no sig tx test

static const uint32_t best2300_ins_patch_config[] =
{
    10,
    (uint32_t)&bes2300_ins_patch0,
    (uint32_t)&bes2300_ins_patch1,
    (uint32_t)&bes2300_ins_patch2,
    (uint32_t)&bes2300_ins_patch3,
    (uint32_t)&bes2300_ins_patch4,
    (uint32_t)&bes2300_ins_patch5,
    (uint32_t)&bes2300_ins_patch6,
    (uint32_t)&bes2300_ins_patch7,
    (uint32_t)&bes2300_ins_patch8,
    (uint32_t)&bes2300_ins_patch9,
};


///PATCH RAM POSITION
///PATCH0   0xc0006690--C000669C
///PATCH2   0XC0006920--C0006928
///PATCH3   0XC0006930--C0006944
///PATCH4   0XC0006950--C0006958
///PATCH5   0XC0006980--C00069E4
///PATCH6   0XC00069F0--C0006A0C
///PATCH7   0XC0006A28--C0006A4C
///PATCH9   0xc0006960 --0xc0006978
///PATCH11  0xc0006ca0 --0xc0006cb4
///PATCH12  0xc0006cc0 --0xc0006cd4
///PATCH14  0XC0006800--C0006810
///PATCH15  0XC0006810--C0006820
///PATCH16  0XC0006a60--C0006a74 //(0XC00067f0--C0006800)empty
///PATCH17  0XC00068f0--C0006914
///PATCH19  0xc0006704--c00067ec
///PATCH22  0xc0006848--0xc00068e0
///PATCH25  0xc0006a18--0xc0006a20
///PATCH26  0xc0006670--0xc0006680
///PATCH28  0xc00066b0--0xc00066F8
///PATCH35  0XC0006A54--0xc0006a60
///PATCH42  0XC0006848--0xc0006864
///patch36  0xc0006a80-0xc0006a94
///patch37  0xc0006aa0 --0xc0006ab0
///patch38  0xc0006ac0--0xc0006aec
///patch40  0xc0006af0--0xc0006b00
///patch43  0xc0006be0--0xc0006bf8
///patch44  0xc0006c00--0xc0006c44
///patch46  0xc0006c50--0xc0006c8c
///patch47  0xc0006c98--0xc0006db0

#define BT_FORCE_3M
#define BT_VERSION_50
#define BT_SNIFF_SCO
#define BT_MS_SWITCH
#define BT_SW_SEQ

//#define BT_DEBUG_PATCH
////
const uint32_t bes2300_patch0_ins_data_2[] =
{
    0x0008f109,
    0xbf0060b0,
    0xba12f627,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch0_2 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data_2),
    0x0002dabc,
    0xbde8f1d8,
    0xc0006690,
    (uint8_t *)bes2300_patch0_ins_data_2
};//ld data tx


const BTDRV_PATCH_STRUCT bes2300_ins_patch1_2 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0000e40c,
    0xbf00e0ca,
    0,
    NULL
};///fix sniff re enter

const uint32_t bes2300_patch2_ins_data_2[] =
{
    0x22024b03,
    0x555a463d,
    0x4b022200,
    0xbe2ef622,
    0xc00067a4,
    0xc00008fc
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_2 =
{
    2,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch2_ins_data_2),
    0x00029438,
    0xb9caf1dd,
    0xc00067d0,
    (uint8_t *)bes2300_patch2_ins_data_2
};/*ld_acl_end*/


const uint32_t bes2300_patch3_ins_data_2[] =
{
    0x0080f105,
    0xf60f2101,
    0x8918fe59,
    0xfe8ef600,
    0xb90ef610,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch3_2 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch3_ins_data_2),
    0x00016b28,
    0xbf02f1ef,
    0xc0006930,
    (uint8_t *)bes2300_patch3_ins_data_2
};//free buff

const uint32_t bes2300_patch4_ins_data_2[] =
{
    0x091ff04f,
    0xbd93f628,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch4_2 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_data_2),
    0x0002f474,
    0xba6cf1d7,
    0xc0006950,
    (uint8_t *)bes2300_patch4_ins_data_2
};//curr prio assert




const uint32_t bes2300_patch5_ins_data_2[] =
{
    0x3002f898,
    0x03c3eb07,
    0x306cf893,
    0x4630b333,
    0xf60b2100,
    0xf640fedd,
    0x21000003,
    0x230e2223,
    0xf918f5fa,
    0x70042400,
    0x80453580,
    0x2002f898,
    0xeb07320d,
    0xf85707c2,
    0x687a1f04,
    0x1006f8c0,
    0x200af8c0,
    0x2002f898,
    0xf8987102,
    0x71422002,
    0xff28f63c,
    0xe8bd2000,
    0xbf0081f0,
    0xe8bd2002,
    0xbf0081f0,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_2 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch5_ins_data_2),
    0x00020b00,
    0xbf3ef1e5,
    0xc0006980,
    (uint8_t *)bes2300_patch5_ins_data_2
};//slave feature rsp


const uint32_t bes2300_patch6_ins_data_2[] =
{
    0xf4038873,
    0xf5b34370,
    0xd0055f80,
    0xf1058835,
    0xbf004550,
    0xbe1bf61a,
    0xbe42f61a,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch6_2 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_data_2),
    0x00021638,
    0xb9daf1e5,
    0xc00069f0,
    (uint8_t *)bes2300_patch6_ins_data_2
};///continue packet error


const uint32_t bes2300_patch7_ins_data_2[] =
{
    0xf003783b,
    0x2b000301,
    0x4630d108,
    0x22242117,
    0xfbe4f61b,
    0xbf002000,
    0xbf36f613,
    0xf86cf5fa,
    0xbec6f613,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch7_2 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch7_ins_data_2),
    0x0001a7d4,
    0xb928f1ec,
    0xc0006a28,
    (uint8_t *)bes2300_patch7_ins_data_2
};//sniff req collision


const BTDRV_PATCH_STRUCT bes2300_ins_patch8_2 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00035948,
    0x124cf640,
    0,
    NULL
};//acl evt dur 4 slot


#if 0

const uint32_t bes2300_patch9_ins_data_2[] =
{
    0x2b00b29b,
    0xf896d006,
    0x330130bb,
    0x30bbf886,
    0xb9c0f626,
    0xb8c8f626,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch9_2 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch9_ins_data_2),
    0x0002cb00,
    0xbf2ef1d9,
    0xc0006960,
    (uint8_t *)bes2300_patch9_ins_data_2
};//seq error
#else

const uint32_t bes2300_patch9_ins_data_2[] =
{
    0xbf00d106,
    0xfdb0f609,
    0xe0012801,
    0xbe30f621,
    0xbee6f621,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch9_2 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch9_ins_data_2),
    0x000285cc,
    0xb9c8f1de,
    0xc0006960,
    (uint8_t *)bes2300_patch9_ins_data_2
};
#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch10_2 =
{
    10,
    BTDRV_PATCH_ACT,
    0,
    0x0004488c,
    0x0212bf00,
    0,
    NULL
};///acl data rx error when disconnect


#ifdef BT_VERSION_50

#if 1
const BTDRV_PATCH_STRUCT bes2300_ins_patch11_2 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x000227ec,
    0xf88d2309,
    0,
    NULL
};//version



const BTDRV_PATCH_STRUCT bes2300_ins_patch12_2 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x000170a0,
    0x23093004,
    0,
    NULL
};//version
#else

const uint32_t bes2300_patch11_ins_data_2[] =
{
    0xf88d2309,
    0x4b023001,
    0xbf00681b,
    0xbda2f61b,
    0xc0006cb4,
    0x000002b0
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch11_2 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch11_ins_data_2),
    0x000227ec,
    0xba58f1e4,
    0xc0006ca0,
    (uint8_t *)bes2300_patch11_ins_data_2
};

const uint32_t bes2300_patch12_ins_data_2[] =
{
    0xf88d2309,
    0x4b023001,
    0xbf00681b,
    0xb9edf610,
    0xc0006cd4,
    0x000002b0
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch12_2 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch12_ins_data_2),
    0x000170a4,
    0xbe0cf1ef,
    0xc0006cc0,
    (uint8_t *)bes2300_patch12_ins_data_2
};

#endif
#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch11_2 =
{
    11,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_2 =
{
    12,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};
#endif


#ifdef BLE_POWER_LEVEL_0
const BTDRV_PATCH_STRUCT bes2300_ins_patch13_2 =
{
    13,
    BTDRV_PATCH_ACT,
    0,
    0x0003e4b8,
    0x7f032600,
    0,
    NULL
};//adv power level


const uint32_t bes2300_patch14_ins_data_2[] =
{
    0x32082100, ///00 means used the power level 0
    0x250052a1, //00 means used the power level 0
    0xb812f639,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch14_2 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch14_ins_data_2),
    0x0003f824,
    0xbfecf1c6,
    0xc0006800,
    (uint8_t *)bes2300_patch14_ins_data_2
};//slave power level

const uint32_t bes2300_patch15_ins_data_2[] =
{
    0xf8282100, ///00 means used the power level 0
    0xf04f1002,
    0xbf000a00,//00 means used the power level 0
    0xbbdcf638,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch15_2 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_data_2),
    0x0003efd0,
    0xbc1ef1c7,
    0xc0006810,
    (uint8_t *)bes2300_patch15_ins_data_2
};//master

#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch13_2 =
{
    13,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch14_2 =
{
    14,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch15_2 =
{
    15,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

#endif


#ifdef BT_SNIFF_SCO
const uint32_t bes2300_patch16_ins_data_2[] =
{
    0xf62fb508,
    0xf62ff9cb,
    0x4b01f9e5,
    0xba00f62f,
    0xc00061c8,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch16_2 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch16_ins_data_2),
    0x00035e6c,
    0xbdf8f1d0,
    0xc0006a60,
    (uint8_t *)bes2300_patch16_ins_data_2
};//send bitoff when start sniffer sco


const uint32_t bes2300_patch17_ins_data_2[] =
{
    0x4b07b955,
    0x2042f893,
    0xbf004b04,
    0x3022f853,
    0x4630b113,
    0xbc7cf62f,
    0xbc83f62f,
    0xc0005b48,
    0xc00061c8,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_2 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch17_ins_data_2),
    0x000361fc,
    0xbb78f1d0,
    0xc00068f0,
    (uint8_t *)bes2300_patch17_ins_data_2
};//stop sco Assert

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_2 =
{
    18,
    BTDRV_PATCH_ACT,
    0,
    0x00030420,
    0xbd702001,
    0,
    NULL
};//sco param assert


#ifdef __3RETX_SNIFF__
const uint32_t bes2300_patch19_ins_data_2[] =
{
    0x47f0e92d,
    0x4614460f,
    0xd9492a02,
    0x25004682,
    0xf04f46a8,
    0xb2ee0901,
    0x42b37fbb,
    0x4630d03d,
    0xf6054641,
    0xb920fbd7,
    0x46494630,
    0xfbd2f605,
    0x4b26b398,
    0x0026f853,
    0x2b036883,
    0x2100bf94,
    0xebca2101,
    0xf0220201,
    0xf1b54578,
    0xbf886f80,
    0x020aebc1,
    0x4278f022,
    0xbf183200,
    0xb1fa2201,
    0x0203ebca,
    0x4178f022,
    0x6f80f1b1,
    0xebc3bf83,
    0xf023030a,
    0x425b4378,
    0x4378f022,
    0x42937eba,
    0xf890dc0e,
    0x2b0330b5,
    0x2c04d104,
    0x2c03d019,
    0xe010d106,
    0xd1032c04,
    0x3501e00f,
    0xd1ba2d03,
    0x04247ffb,
    0x490a015b,
    0xf422585a,
    0x4314027f,
    0xe8bd505c,
    0x240387f0,
    0x2403e000,
    0x04247ffb,
    0x7ffbe7f0,
    0x3400f44f,
    0xbf00e7ec,
    0xc00008fc,
    0xd0220150,
    0x68a04602,
    0x0128f104,
    0xff90f7ff,
    0x30b3f895,
    0xbe8af628,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_2 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch19_ins_data_2),
    0x0002f4f0,
    0xb972f1d7,
    0xc0006704,
    (uint8_t *)bes2300_patch19_ins_data_2
};//esco retx 3 dec 2
#else

const uint32_t bes2300_patch19_ins_data_2[] =
{
    0x3026f853,
    0xf8842000,
    0xbf0002fa,
    0xba2af60f,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_2 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch19_ins_data_2),
    0x00015b64,
    0xbdcef1f0,
    0xc0006704,
    (uint8_t *)bes2300_patch19_ins_data_2
};//bt role switch clear rswerror at lc_epr_cmp

#endif


const BTDRV_PATCH_STRUCT bes2300_ins_patch20_2 =
{
    20,
#ifdef __3RETX_SNIFF__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x00010be8,
    0xb81ef000,
    0,
    NULL
};//support 2/3 esco retx

const uint32_t bes2300_patch21_ins_data_2[] =
{
    0x681b4b0f,
    0x4b0fb1bb,
    0x31cbf893,
    0x03c3ebc3,
    0x4a0d005b,
    0xf3c35a9b,
    0x4a0c2340,
    0x2a025c12,
    0x4a0ad103,
    0x20005413,
    0x42934770,
    0x4a07d005,
    0x20005413,
    0x20004770,
    0x20014770,
    0xbf004770,
    0xc00067a0,
    0xc0005c0c,
    0xd021159a,
    0xc00067a4,
    0x00000001,
    0x00020202,
    0x9803bf00,
    0xffd0f7ff,
    0xf0039b05,
    0x28000403,
    0xbf00d102,
    0xb9f3f626,
    0xbaa7f626

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_2 =
{
    21,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch21_ins_data_2),
    0x0002cba0,
    0xbe02f1d9,
    0xc0006750,
    (uint8_t *)bes2300_patch21_ins_data_2
};
/*ld_sw_seqn_filter*/


#if 0
const uint32_t bes2300_patch22_ins_data_2[] =
{
    0xf8534b1e,
    0x4b1e2020,
    0xb913681b,
    0x685b4b1c,
    0xf893b393,
    0x42811046,
    0xb430d02e,
    0x00d4f8d2,
    0x0401f000,
    0x42886b59,
    0xf893d30a,
    0x1d0d3042,
    0xfbb11a41,
    0xfb03f1f3,
    0xf0235301,
    0xe0094378,
    0x3042f893,
    0x1a091f0d,
    0xf1f3fbb1,
    0x5311fb03,
    0x4378f023,
    0xf013b934,
    0xd00a0f01,
    0xf0233b01,
    0xe0064378,
    0x0f01f013,
    0xf103bf04,
    0xf02333ff,
    0xf8c24378,
    0xbc3030d4,
    0xbf004770,
    0xc00008fc,
    0xc00008ec,
    0xbf004638,
    0xffbaf7ff,
    0x236ebf00,
    0xf707fb03,
    0xb9f4f621,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch22_2 =
{
    22,
#ifdef __3RETX_SNIFF__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch22_ins_data_2),
    0x00027cc4,
    0xbe02f1de,
    0xc0006848,
    (uint8_t *)bes2300_patch22_ins_data_2
};//In eSCO sniff anchor point calculate
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch22_2 =
{
    22,
    BTDRV_PATCH_ACT,
    0,
    0x0002b3f8,
    0xbf00bf00,
    0,
    NULL
};///remove PCM EN in controller
#endif
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch16_2 =
{
    16,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_2 =
{
    17,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_2 =
{
    18,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_2 =
{
    19,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch20_2 =
{
    20,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch21_2 =
{
    21,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch22_2 =
{
    22,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

#endif

#ifdef BT_MS_SWITCH

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_2 =
{
    23,
    BTDRV_PATCH_ACT,
    0,
    0x000362c0,
    0x93012300,
    0,
    NULL
};//wipe acl lt addr

const BTDRV_PATCH_STRUCT bes2300_ins_patch24_2 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x0002f1bc,
    0xb804f000,
    0,
    NULL
};//clear fnak bit


const uint32_t bes2300_patch25_ins_data_2[] =
{
    0x30fff04f,
    0xbf00bd70,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_2 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch25_ins_data_2),
    0x00030cc4,
    0xbea8f1d5,
    0xc0006a18,
    (uint8_t *)bes2300_patch25_ins_data_2
};// acl switch



#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_2 =
{
    23,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch24_2 =
{
    24,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_2 =
{
    25,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


#endif


const uint32_t bes2300_patch26_ins_data_2[] =
{
    0x0301f043,
    0xd1012b07,
    0xbc00f625,
    0xbbeef625,
};



const BTDRV_PATCH_STRUCT bes2300_ins_patch26_2=
{
    26,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch26_ins_data_2),
    0x0002be74,
    0xbbfcf1da,
    0xc0006670,
    (uint8_t *)bes2300_patch26_ins_data_2
};//ld data tx 3m


const BTDRV_PATCH_STRUCT bes2300_ins_patch27_2 =
{
    27,
#ifdef __CLK_GATE_DISABLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000061a8,
    0xbf30bf00,
    0,
    NULL
};///disable clock gate

#ifdef BT_DEBUG_PATCH
const uint32_t bes2300_patch28_ins_data_2[] =
{
    0x2b0b7b23,
    0xbf00d118,
    0x3b017b63,
    0x2b01b2db,
    0xbf00d812,
    0x010ef104,
    0xb1437ba3,
    0x33012200,
    0xb2d2441a,
    0x780b4419,
    0xd1f82b00,
    0x2200e000,
    0xf8ad3202,
    0xbf002006,
    0x72a323ff,
    0x3006f8bd,
    0xbf0072e3,
    0xbe10f63d,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_data_2),
    0x00044310,
    0xb9cef1c2,
    0xc00066b0,
    (uint8_t *)bes2300_patch28_ins_data_2
};//hci_build_dbg_evt for lmp record   ///can close when release sw

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_2 =
{
    29,
    BTDRV_PATCH_ACT,
    0,
    0x00000650,
    0xbdfcf005,
    0,
    NULL
};//enter default handler when assert

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_2 =
{
    30,
    BTDRV_PATCH_ACT,
    0,
    0x000062c4,
    0xfffef3ff,
    0,
    NULL
};//bus fault handler while(1)

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_2 =
{
    31,
    BTDRV_PATCH_INACT,
    0,
    0x0002f4d4,
    0xbf002301,
    0,
    NULL
};//force retx to 1


const BTDRV_PATCH_STRUCT bes2300_ins_patch32_2 =
{
    32,
    BTDRV_PATCH_INACT,
    0,
    0x00021de0,
    0xdd212b05,
    0,
    NULL
}; ///lmp record


const BTDRV_PATCH_STRUCT bes2300_ins_patch33_2 =
{
    33,
    BTDRV_PATCH_INACT,
    0,
    0x00021e78,
    0xdd212b05,
    0,
    NULL
};//lmp record

#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch29_2 =
{
    29,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch30_2 =
{
    30,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch31_2 =
{
    31,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch32_2 =
{
    32,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch33_2 =
{
    33,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};




#endif


///can close when there is no patch position
const BTDRV_PATCH_STRUCT bes2300_ins_patch34_2 =
{
    34,
    BTDRV_PATCH_ACT,
    0,
    0x00017708,
    0x700ef247,
    0,
    NULL
};//set afh timeout to 20s

const uint32_t bes2300_patch35_ins_data_2[] =
{
    0x4778f027,
    0x708cf8c4,
    0xbc86f621,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch35_2 =
{
    35,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch35_ins_data_2),
    0x00028368,
    0xbb74f1de,
    0xc0006a54,
    (uint8_t *)bes2300_patch35_ins_data_2
};//ld_acl_rx_sync

const uint32_t bes2300_patch36_ins_data_2[] =
{
    0xfbb4f61d,
    0x60184b01,
    0x81f0e8bd,
    0xc0006a90,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch36_2 =
{
    36,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch36_ins_data_2),
    0x0004438c,
    0xbb78f1c2,
    0xc0006a80,
    (uint8_t *)bes2300_patch36_ins_data_2
};//hci timout 1:h4tl_write record clock


const uint32_t bes2300_patch37_ins_data_2[] =
{
    0xfba4f61d,
    0x60184b01,
    0xbf00bd70,
    0xc0006a90,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch37_2 =
{
    37,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch37_ins_data_2),
    0x00005e10,
    0xbe46f200,
    0xc0006aa0,
    (uint8_t *)bes2300_patch37_ins_data_2
};//hci timout 2:h4tl_rx_done recored clock

const uint32_t bes2300_patch38_ins_data_2[] =
{
    0x2b007b1b,
    0xbf00d00c,
    0xfb90f61d,
    0x681b4b05,
    0xf0201ac0,
    0xf5b04078,
    0xd9016f80,
    0xbbb6f5f9,
    0xbcdaf5f9,
    0xc0006a90,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch38_2 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch38_ins_data_2),
    0x00000244,
    0xbc3cf206,
    0xc0006ac0,
    (uint8_t *)bes2300_patch38_ins_data_2
};//hci timout 3,sleep check

#if 0
const uint32_t bes2300_patch39_ins_data_2[] =
{
    0xf8534b22,
    0x4b222020,
    0xb913681b,
    0x685b4b20,
    0xf893b3d3,
    0x42811046,
    0xb410d036,
    0x00d4f8d2,
    0x42886b59,
    0x1a41d317,
    0x4042f893,
    0xf3f4fbb1,
    0x1113fb04,
    0xd8042904,
    0xf0231c83,
    0xf8c24378,
    0x290730d4,
    0xf8d2d91e,
    0x3b0230d4,
    0x4378f023,
    0x30d4f8c2,
    0x1a09e016,
    0x4042f893,
    0xf3f4fbb1,
    0x1113fb04,
    0xd8042904,
    0xf0231e83,
    0xf8c24378,
    0x290730d4,
    0xf8d2d906,
    0x330230d4,
    0x4378f023,
    0x30d4f8c2,
    0x4b04f85d,
    0xbf004770,
    0xc00008fc,
    0xc00008ec,
    0xbf009801,
    0xffb2f7ff,
    0x30d4f8d4,
    0xbf1af622,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch39_2 =
{
    39,
#ifdef __3RETX_SNIFF__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch39_ins_data_2),
    0x000299e0,
    0xb8def1dd,
    0xc0006b0c,
    (uint8_t *)bes2300_patch39_ins_data_2
};//ld_acl_sniff_sched
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch39_2 =
{
    39,
    BTDRV_PATCH_ACT,
    0,
    0x0002b0dc,
    0x491d0300,
    0,
    NULL
};///remove PCM EN in controller
#endif

//////////////////////////////////////////////////
///meybe no used patch
#if 0
const BTDRV_PATCH_STRUCT bes2300_ins_patch16_2 =
{
    16,
#ifdef APB_PCM
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002b3f8,
    0xbf00bf00,
    0,
    NULL
};///remove PCM EN in controller

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_2 =
{
    17,
#ifdef APB_PCM
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002b0dc,
    0x491d0300,
    0,
    NULL
};///remove PCM EN in controller



const uint32_t bes2300_patch18_ins_data_2[] =
{
    0x134cf640,
    0xf8b48263,
    0xbf0020b8,
    0xbe62f622,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch18_2 =
{
    18,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch18_ins_data_2),
    0x00029500,
    0xb996f1dd,
    0xc0006830,
    (uint8_t *)bes2300_patch18_ins_data_2
};///set acl in esco to 2 frame

#endif



const uint32_t bes2300_patch40_ins_data_2[] =
{
    0x134cf640,
    0xf8b48263,
    0x68a320b8,
    0xbd03f622,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch40_2 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch40_ins_data_2),
    0x00029500,
    0xbaf6f1dd,
    0xc0006af0,
    (uint8_t *)bes2300_patch40_ins_data_2
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch41_2 =
{
    41,
    BTDRV_PATCH_ACT,
    0,
    0x00027884,
    0xbf00e012,
    0,
    NULL
};//disable sscan

const uint32_t bes2300_patch42_ins_data_2[] =
{
    0x44137da3,
    0xbf0075a3,
    0xfa06f62f,
    0x4620b920,
    0xf0002101,
    0xb908fa1d,
    0xbb5bf623,
    0xbb68f623,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch42_2 =
{
    42,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch42_ins_data_2),
    0x00029f14,
    0xbc98f1dc,
    0xc0006848,
    (uint8_t *)bes2300_patch42_ins_data_2
};

const uint32_t bes2300_patch43_ins_data_2[] =
{
    0x781b4b05,
    0xd00342bb,
    0x66a3231a,
    0xbf00e002,
    0x66a32354,
    0xbdd0f620,
    0xc0006bfc,
    0x000000ff,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch43_2 =
{
    43,
#ifdef __LARGE_SYNC_WIN__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch43_ins_data_2),
    0x00027794,
    0xba24f1df,
    0xc0006be0,
    (uint8_t *)bes2300_patch43_ins_data_2
};

const uint32_t bes2300_patch44_ins_data_2[] =
{
    0x78124a10,
    0xd003454a,
    0x310d0c89,
    0xbe5af620,
    0xf1a12178,
    0xeba60354,
    0xb29b0353,
    0x4f00f413,
    0x2600d00c,
    0x2371f203,
    0x1c3ab29b,
    0x4778f022,
    0xb2f63601,
    0x4f00f413,
    0xe001d1f4,
    0xbe58f620,
    0xbe57f620,
    0xc0006bfc,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch44_2 =
{
    44,
#ifdef __LARGE_SYNC_WIN__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch44_ins_data_2),
    0x000278c0,
    0xb99ef1df,
    0xc0006c00,
    (uint8_t *)bes2300_patch44_ins_data_2
};

#ifndef __SW_SEQ_FILTER__
const BTDRV_PATCH_STRUCT bes2300_ins_patch45_2 =
{
    45,
    BTDRV_PATCH_ACT,
    0,
    0x0001e3d0,
    0xbf00bf00,
    0,
    NULL,
};//lmp_accept_ext_handle assert
#else
const uint32_t bes2300_patch45_ins_data_2[] =
{
    0xf8934b0b,
    0xebc331cb,
    0x005b03c3,
    0x5a9b4a09,
    0xf3c3b29b,
    0x4a082340,
    0x5c509903,
    0xd00328ff,
    0xd0034298,
    0xbf005453,
    0xbf92f625,
    0xb83af626,
    0xc0005c0c,
    0xd021159a,
    0xc0006c8c,
    0x02020202
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch45_2 =
{
    45,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch45_ins_data_2),
    0x0002cb7c,
    0xb868f1da,
    0xc0006c50,
    (uint8_t *)bes2300_patch45_ins_data_2
};


#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch46_2 =
{
    46,
    BTDRV_PATCH_ACT,
    0,
    0x0002b300,
    0xb2ad0540,
    0,
    NULL
};//sco duration

const uint32_t bes2300_patch47_ins_data_2[] =
{
    0x4605b5f8,//ld_calculate_acl_timestamp
    0xf609460e,
    0x2800fc13,
    0x4b2fd056,
    0x2c00681c,
    0xf894d054,
    0xf61d7044,
    0xf894fa9b,
    0x6b621042,
    0xf0221a52,
    0x42904278,
    0x1853d904,
    0x4298461a,
    0xe000d8fb,
    0x2e004613,
    0xf895d135,
    0x2a0120b3,
    0xf640d119,
    0x826a124c,
    0x4a203701,
    0xeb026812,
    0x44130247,
    0x4a1e60ab,
    0x42936812,
    0x1ad2bf34,
    0xf0221a9a,
    0x2a044278,
    0xf894d82a,
    0x44132042,
    0x200160ab,
    0xf640bdf8,
    0x826a124c,
    0x2042f894,
    0xbf282f02,
    0x37012702,
    0x10b8f8b5,
    0xf1f2fb91,
    0x2202fb01,
    0x0747eb02,
    0x68124a0e,
    0x443b4417,
    0x200160ab,
    0x2e01bdf8,
    0x68abd10c,
    0x60ab330c,
    0x75ab2306,
    0xbdf82001,
    0xbdf82000,
    0xbdf82000,
    0xbdf82001,
    0xbdf82001,
    0xc00008ec,
    0xc0006d74,
    0xc0006d78,// master timestamp addr
    0xc0006d7c,
    0x00000000,// extra_slave
    0x00000000,// master timestamp
    0x00000000,// extra_master
    0xff6ef62e,// jump function
    0x4620b118,
    0xf7ff2100,
    0x4620ff85,
    0xfbe0f63f,
    0x2300b950,
    0x30b4f884,
    0x30b3f894,
    0x68a2b913,
    0x601a4b02,
    0xbf00bdf8,
    0xbbe1f622,
    0xc0006d78,//master timestamp addr
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch47_2 =
{
    47,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch47_ins_data_2),
    0x00029564,
    0xbc0cf1dd,
    0xc0006c98,
    (uint8_t *)bes2300_patch47_ins_data_2
};//ld_calculate_acl_timestamp


#if 0
const uint32_t bes2300_patch48_ins_data_2[] =
{
    0x30abf894,
    0x490fb9d3,
    0x4a10880b,
    0xf4037810,
    0x4303437f,
    0x490d800b,
    0x02006808,
    0x88134a0a,
    0xf423b29b,
    0x430363e0,
    0x68088013,
    0x3001bf00,
    0xd8012802,
    0xe0016008,
    0x600a2200,
    0x1090f8b4,
    0xb9c2f63a,
    0xd02101d6,
    0xd02101d0,
    0xc0006e10,
    0x00000000,


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch48_2 =
{
    48,
#ifdef __DYNAMIC_ADV_POWER__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch48_ins_data_2),
    0x00041184,
    0xbe1cf1c5,
    0xc0006dc0,
    (uint8_t *)bes2300_patch48_ins_data_2
};
#else

const uint32_t bes2300_patch48_ins_data_2[] =
{
    0xf3efbf00,
    0xf0138310,
    0x2e000601,
    0xb672d100,
    0xfaeaf600,
    0xd1032800,
    0xd1002e00,
    0xbd70b662,
    0xfbc6f63c,
    0xd1032800,
    0xd1002e00,
    0xbd70b662,
    0xd1012e00,
    0xbb25f5f9,
    0xbb6ef5f9,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch48_2 =
{
    48,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch48_ins_data_2),
    0x0000041c,
    0xbcd0f206,
    0xc0006dc0,
    (uint8_t *)bes2300_patch48_ins_data_2
};

#endif

const uint32_t bes2300_patch49_ins_data_2[] =
{

    0xb083b500,
    0x0101f001,
    0x0148f041,
    0x1004f88d,
    0x2005f88d,
    0xf61ba901,
    0xb003f959,
    0xfb04f85d,
    0xbf004d0f,
    0x28ff6828,
    0x0200d016,
    0x407ff400,
    0x0001f040,
    0xfafaf5fa,
    0xd10d2801,
    0x4b076828,
    0x3020f853,
    0xf893b2c0,
    0x4b071040,
    0xf7ff781a,
    0x23ffffd7,
    0xbf00602b,
    0xb986f5ff,
    0xc0005b48,
    0xc0006e84,
    0x000000ff,
    0xc0006e8c,
    0x00000000,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch49_2 =
{
    49,
#ifdef __SEND_PREFERRE_RATE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch49_ins_data_2),
    0x00006174,
    0xbe64f200,
    0xc0006E20,
    (uint8_t *)bes2300_patch49_ins_data_2
};


const uint32_t bes2300_patch50_ins_data_2[] =
{
    0xf5fa4628,
    0x4903fad3,
    0x46286008,
    0xbf002101,
    0xbd4af613,
    0xc0006eb8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch50_2 =
{
    50,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch50_ins_data_2),
    0x0001a944,
    0xbaacf1ec,
    0xc0006ea0,
    (uint8_t *)bes2300_patch50_ins_data_2
};



const uint32_t bes2300_patch51_ins_data_2[] =
{
    0x0f00f1b8,
    0x4628d004,
    0x68094904,
    0xfa3af5fa,
    0x21134630,
    0x3044f894,
    0xbd6bf613,
    0xc0006eb8


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch51_2 =
{
    51,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch51_ins_data_2),
    0x0001a9dc,
    0xba88f1ec,
    0xc0006ef0,
    (uint8_t *)bes2300_patch51_ins_data_2
};


const uint32_t bes2300_patch52_ins_data_2[] =
{
    0xbf00251f,
    0xbe88f61e,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch52_2 =
{
    52,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch52_ins_data_2),
    0x00025c20,
    0xb976f1e1,
    0xc0006f10,
    (uint8_t *)bes2300_patch52_ins_data_2
};

#ifdef __POWER_CONTROL_TYPE_1__
const uint32_t bes2300_patch53_ins_data_2[] =
{
    0xf0035243,
    0x2b00030f,
    0x2000bf14,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch53_2 =
{
    53,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch53_ins_data_2),
    0x000033bc,
    0xbdb0f203,
    0xc0006f20,
    (uint8_t *)bes2300_patch53_ins_data_2
};//pwr controll
#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch53_2 =
{
    53,
    BTDRV_PATCH_ACT,
    0,
    0x000033f4,
    0xe0072000,
    0,
    NULL
};
#endif

const uint32_t bes2300_patch54_ins_data_2[] =
{
    0xfbb24a02,
    0xf5f9f3f3,
    0xbf00bbc0,
    0x9af8da00,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch54_2 =
{
    54,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch54_ins_data_2),
    0x000006b4,
    0xbc3cf206,
    0xc0006f30,
    (uint8_t *)bes2300_patch54_ins_data_2
};//bitoff check

const uint32_t bes2300_patch55_ins_data_2[] =
{
    0x3044f885,
    0x22024b02,
    0x541a4630,
    0xba40f613,
    0xc00067a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch55_2 =
{
    55,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch55_ins_data_2),
    0x0001a3d4,
    0xbdb8f1ec,
    0xc0006f48,
    (uint8_t *)bes2300_patch55_ins_data_2
};/*lc_rsw_end_ind*/

static const uint32_t best2300_ins_patch_config_2[] =
{
    56,
    (uint32_t)&bes2300_ins_patch0_2,
    (uint32_t)&bes2300_ins_patch1_2,
    (uint32_t)&bes2300_ins_patch2_2,
    (uint32_t)&bes2300_ins_patch3_2,
    (uint32_t)&bes2300_ins_patch4_2,
    (uint32_t)&bes2300_ins_patch5_2,
    (uint32_t)&bes2300_ins_patch6_2,
    (uint32_t)&bes2300_ins_patch7_2,
    (uint32_t)&bes2300_ins_patch8_2,
    (uint32_t)&bes2300_ins_patch9_2,
    (uint32_t)&bes2300_ins_patch10_2,
    (uint32_t)&bes2300_ins_patch11_2,
    (uint32_t)&bes2300_ins_patch12_2,
    (uint32_t)&bes2300_ins_patch13_2,
    (uint32_t)&bes2300_ins_patch14_2,
    (uint32_t)&bes2300_ins_patch15_2,
    (uint32_t)&bes2300_ins_patch16_2,
    (uint32_t)&bes2300_ins_patch17_2,
    (uint32_t)&bes2300_ins_patch18_2,
    (uint32_t)&bes2300_ins_patch19_2,
    (uint32_t)&bes2300_ins_patch20_2,
    (uint32_t)&bes2300_ins_patch21_2,
    (uint32_t)&bes2300_ins_patch22_2,
    (uint32_t)&bes2300_ins_patch23_2,
    (uint32_t)&bes2300_ins_patch24_2,
    (uint32_t)&bes2300_ins_patch25_2,
    (uint32_t)&bes2300_ins_patch26_2,
    (uint32_t)&bes2300_ins_patch27_2,
    (uint32_t)&bes2300_ins_patch28_2,
    (uint32_t)&bes2300_ins_patch29_2,
    (uint32_t)&bes2300_ins_patch30_2,
    (uint32_t)&bes2300_ins_patch31_2,
    (uint32_t)&bes2300_ins_patch32_2,
    (uint32_t)&bes2300_ins_patch33_2,
    (uint32_t)&bes2300_ins_patch34_2,
    (uint32_t)&bes2300_ins_patch35_2,
    (uint32_t)&bes2300_ins_patch36_2,
    (uint32_t)&bes2300_ins_patch37_2,
    (uint32_t)&bes2300_ins_patch38_2,
    (uint32_t)&bes2300_ins_patch39_2,
    (uint32_t)&bes2300_ins_patch40_2,
    (uint32_t)&bes2300_ins_patch41_2,
    (uint32_t)&bes2300_ins_patch42_2,
    (uint32_t)&bes2300_ins_patch43_2,
    (uint32_t)&bes2300_ins_patch44_2,
    (uint32_t)&bes2300_ins_patch45_2,
    (uint32_t)&bes2300_ins_patch46_2,
    (uint32_t)&bes2300_ins_patch47_2,
    (uint32_t)&bes2300_ins_patch48_2,
    (uint32_t)&bes2300_ins_patch49_2,
    (uint32_t)&bes2300_ins_patch50_2,
    (uint32_t)&bes2300_ins_patch51_2,
    (uint32_t)&bes2300_ins_patch52_2,
    (uint32_t)&bes2300_ins_patch53_2,
    (uint32_t)&bes2300_ins_patch54_2,
    (uint32_t)&bes2300_ins_patch55_2,
};


/*****************************************
2300 t6 patch
patch 0  0xc0006800-0xc0006848
patch 1  0xc0006860-0xc00068cc
patch 2  0xc00068d0-0xc00068ec
patch 3  0xc00068f0-0xc0006910
patch 4  0xc0006914-0xc0006920
patch 5  0xc0006920-0xc000692c
patch 6  0xc000692c-0xc0006938
patch 7  0xc000693c-0xc000694c
patch 8  0xc000694c-0xc000695c
patch 9  0xc0006960-0xc0006978
patch 10 0xc00069b0-0xc00069d0
patch 11
patch 12
patch 13 0xc00069d0-0xc00069fc
patch 14 0xc000430C-0xc0004408
patch 15 0xc0004414-0xc00047e0
patch 16 0xc0006d20-0xc0006d48
patch 17 0xc0006be8-0xc0006c94
patch 18 0xc0006fb0-0xc0006fd4
patch 19 0xc00047e4-0xc00048d0
patch 20
patch 21 0xc0006a58-0xc0006abc
patch 22
patch 23 0xc0006b00-0xc0006b94
patch 24
patch 25 0xc0006ee0-0xc0006ef8
patch 26
patch 27 0xc0006f0c-0xc0006fb0
patch 28 0xc0006ce0-0xc0006d18
patch 29
patch 30
patch 31 0xc0006fd8-0xc0006fe8
patch 32 0xc0006fec-0xc0007000
patch 33 0xc0007000-0xc0007014
patch 34 0xc00042c0-0xc00042dc
patch 35 0xc0007018-0xc000703c
patch 36 0xc000703c-0xc0007068
patch 37 0xc000706c-0xc0007080
patch 38 0xc0007080-0xc00070c8
patch 39 0xc00070c8-0xc0007104
patch 40 0xc0006eb8-0xc0006ecc
patch 41 0xc0006bd8-0xc0006be4
patch 42 0xc0004250-0xc00042b8
patch 43 0xc0006978-0xc000698c
patch 44 0xc000716c-0xc00071a8
patch 45 0xc00071a8-0xc00071c8
patch 46 0xc00071c8-0xc00071e0
patch 47 0xc00071e0-0xc0007200
patch 48 0xc0006a00-0xc0006a54
patch 49 0xc0006990-0xc00069a0
patch 50
patch 51 0xc0004200-0xc0004248
patch 52
patch 53
patch 54
patch 55 0xc0006ad8-0xc0006af4
//blank c0006ac0--c0006ad0
//blank c0006c98--c0006ce0
//blank c0006ed0--c0006ed8
//blank c0006d50--c0006d8c
//blank 0xc0006da4-0xc0006db4
//blank 0xc0006b98-0xc0006bd4
//blank 0xc0006d90-0xc0006da4
*******************************************/

#if 0
const uint32_t bes2300_patch0_ins_data_3[] =
{
    0x30abf894,
    0x490fb9d3,
    0x4a10880b,
    0xf4037810,
    0x4303437f,
    0x490d800b,
    0x02006808,
    0x88134a0a,
    0xf423b29b,
    0x430363e0,
    0x68088013,
    0x3001bf00,
    0xd8012802,
    0xe0016008,
    0x600a2200,
    0x90a4f8b4,
    0xbadcf63b,
    0xd02101d6,
    0xd02101d0,
    0xc0006e10,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch0_3 =
{
    0,
#ifdef __DYNAMIC_ADV_POWER__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch0_ins_data_3),
    0x00041df8,
    0xbd02f1c4,
    0xc0006800,
    (uint8_t *)bes2300_patch0_ins_data_3
};


#elif 0
const uint32_t bes2300_patch0_ins_data_3[] =
{
    0xd11f2800,
    0x8310f3ef,
    0x0301f003,
    0xd1002b00,
    0xf601b672,
    0x2800fabb,
    0x2b00d103,
    0xb662d100,
    0xb408bd38,
    0xfcaef63d,
    0x2800bc08,
    0x2b00d103,
    0xb662d100,
    0x2b00bd38,
    0xbf00d102,
    0xbdc9f5f9,
    0xbde2f5f9,
    0xbddef5f9,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch0_3 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data_3),
    0x000003c4,
    0xba1cf206,
    0xc0006800,
    (uint8_t *)bes2300_patch0_ins_data_3
};
#else

const uint32_t bes2300_patch0_ins_data_3[] =
{
    0xb2db7823,
    0xd1042b01,
    0x70212100,
    0xf5fa2008,
    0xf3eff83f,
    0xf0138310,
    0xd1080701,
    0xf5f8b672,
    0xb110ffc7,
    0x2f00bf30,
    0xb662d106,
    0xf5f8e004,
    0x2800ffbf,
    0xbf00d1f6,
    0xbd3af5ff,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch0_3 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data_3),
    0x00006cf8,
    0xba9af200,
    0xc0007230,
    (uint8_t *)bes2300_patch0_ins_data_3
};


#endif

#if 0
const uint32_t bes2300_patch1_ins_data_3[] =
{

    0xb083b500,
    0x0101f001,
    0x0148f041,
    0x1004f88d,
    0x2005f88d,
    0xf61ca901,
    0xb003fad5,
    0xfb04f85d,
    0xbf004d0f,
    0x28ff6828,
    0x0200d016,
    0x407ff400,
    0x0001f040,
    0xfd80f5fa,
    0xd10d2801,
    0x4b076828,
    0x3020f853,
    0xf893b2c0,
    0x4b071040,
    0xf7ff781a,
    0x23ffffd7,
    0xbf00602b,
    0xba1ef600,
    0xc0005bcc,
    0xc00068c4,
    0x000000ff,
    0xc00068cc,
    0x00000000,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch1_3 =
{
    1,
#ifdef __SEND_PREFERRE_RATE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch1_ins_data_3),
    0x00006ce4,
    0xbdccf1ff,
    0xc0006860,
    (uint8_t *)bes2300_patch1_ins_data_3
};
#endif

const uint32_t bes2300_patch1_ins_data_3[] =
{
    0x681b4b0f,
    0x4b0fb1bb,
    0x31cbf893,
    0x03c3ebc3,
    0x4a0d005b,
    0xf3c35a9b,
    0x4a0c2340,
    0x2a025c12,
    0x4a0ad103,
    0x20005413,
    0x42934770,
    0x4a07d005,
    0x20005413,
    0x20004770,
    0x20014770,
    0xbf004770,
    0xc00068b0,
    0xc0005c90,
    0xd021159a,
    0xc00068b4,
    0x00000001,
    0x00020202,
    0x9803bf00,
    0xffd0f7ff,
    0xd1012800,
    0xbf97f626,
    0xb81af627
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch1_3 =
{
    1,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch1_ins_data_3),
    0x0002d7f0,
    0xb862f1d9,
    0xc0006860,
    (uint8_t *)bes2300_patch1_ins_data_3
};/*ld_sw_seqn_filter*/

const uint32_t bes2300_patch2_ins_data_3[] =
{
    0xf5fa4628,
    0x4903fd61,
    0x46286008,
    0xbf002101,
    0xbe20f614,
    0xc00068e8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_3=
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch2_ins_data_3),
    0x0001b520,
    0xb9d6f1eb,
    0xc00068d0,
    (uint8_t *)bes2300_patch2_ins_data_3
};//role switch



const uint32_t bes2300_patch3_ins_data_3[] =
{
    0x0f00f1b8,
    0x4628d004,
    0x68094904,
    0xfce0f5fa,
    0x21134630,
    0x3044f894,
    0xbe59f614,
    0xc00068e8


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch3_3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch3_ins_data_3),
    0x0001b5b8,
    0xb99af1eb,
    0xc00068f0,
    (uint8_t *)bes2300_patch3_ins_data_3
};//role switch

const uint32_t bes2300_patch4_ins_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbdeef626

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch4_3 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_data_3),
    0x0002d4f4,
    0xba0ef1d9,
    0xc0006914,
    (uint8_t *)bes2300_patch4_ins_data_3
};//ld_acl_rx rssi

const uint32_t bes2300_patch5_ins_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xb818f61f

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_3 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch5_ins_data_3),
    0x00025954,
    0xbfe4f1e0,
    0xc0006920,
    (uint8_t *)bes2300_patch5_ins_data_3
};//ld_inq_rx rssi

const uint32_t bes2300_patch6_ins_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbf56f63a

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6_3 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_data_3),
    0x000417dc,
    0xb8a6f1c5,
    0xc000692c,
    (uint8_t *)bes2300_patch6_ins_data_3
};//lld_pdu_rx_handler rssi


const BTDRV_PATCH_STRUCT bes2300_ins_patch7_3 =
{
    7,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002c22c,
    0xbf00e010,
    0,
    NULL
};


#if 0
const uint32_t bes2300_patch8_ins_data_3[] =
{
    0x68109a05,
    0x68926851,
    0x0006f8c3,
    0xbbf4f61b
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch8_3 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch8_ins_data_3),
    0x00022140,
    0xbc04f1e4,
    0xc000694c,
    (uint8_t *)bes2300_patch8_ins_data_3
};//ld_send_host_get_mobile_rssi_evt
#else
const uint32_t bes2300_patch8_ins_data_3[] =
{
    0xf8934b04,
    0xb11b3037,
    0x30acf8bb,
    0xbe92f626,
    0xbea1f626,
    0xc0004168,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch8_3 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch8_ins_data_3),
    0x0002d578,
    0xb966f1d9,
    0xc0006848,
    (uint8_t *)bes2300_patch8_ins_data_3
};////ld_acl_rx not accumulate rssi when hw_agc flag is false
#endif

#if 0
const uint32_t bes2300_patch9_ins_data_3[] =
{
    0xf894b408,
    0xb97330b5,
    0xf8b4b2b2,
    0x429a3098,
    0x1ad3bf8c,
    0xf5b31a9b,
    0xbf847f00,
    0x731cf5c3,
    0x2b0d3301,
    0xbc08d808,
    0xf5b2b21a,
    0xdd017f9c,
    0xb9a4f622,
    0xb9a6f622,
    0xbf002000,
    0x87f0e8bd,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch9_3 =
{
    9,
#ifdef BT_RF_OLD_CORR_MODE
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch9_ins_data_3),
    0x00028cd4,
    0xbe44f1dd,
    0xc0006960,
    (uint8_t *)bes2300_patch9_ins_data_3
};//bitoff check
#endif

const uint32_t bes2300_patch9_ins_data_3[] =
{
    0x22024b03,
    0x555a463d,
    0x4b022200,
    0xbb62f623,
    0xc00068b4,
    0xc000095c
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch9_3 =
{
    9,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch9_ins_data_3),
    0x0002a030,
    0xbc96f1dc,
    0xc0006960,
    (uint8_t *)bes2300_patch9_ins_data_3
};/*ld_acl_end*/

const uint32_t bes2300_patch10_ins_data_3[] =
{
    0xf8957812,
    0xf89500bc,
    0x287810bb,
    0x2978d001,
    0x4613d102,
    0xba32f627,
    0xbf004413,
    0xba2ef627
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch10_3 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch10_ins_data_3),
    0x0002de28,
    0xbdc2f1d8,
    0xc00069b0,
    (uint8_t *)bes2300_patch10_ins_data_3
};//ld_acl_frm_isr


const BTDRV_PATCH_STRUCT bes2300_ins_patch11_3 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x00003444,
    0xe00f382d,
    0,
    NULL
};///hw rssi read

#if 0
/*******************************************
 patch 12 and 13 is used for gg role switch
 usually not enable
*******************************************/
const uint32_t bes2300_patch12_ins_data_3[] =
{
    0xd0012800,
    0xbe68f623,
    0x4b18bb66,
    0xf7ffb353,
    0xf895ffe1,
    0x428330b2,
    0xbb1ed124,
    0xf986f61e,
    0x30b3f895,
    0x2b01b13b,
    0xf8d5d107,
    0x4403308c,
    0x4378f023,
    0x4603e002,
    0x2300e000,
    0x031ff003,
    0x0320f1c3,
    0xf0234403,
    0x60ab4378,
    0x799b4b06,
    0xf5fc75ab,
    0xf890f87f,
    0xf5c33025,
    0x330c53ea,
    0xbf00826b,
    0xbf01f623,
    0xc0000200,
    0xc0006d98,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_3 =
{
    12,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch12_ins_data_3),
    0x0002a9fc,
    0xb994f1dc,
    0xc0006d28,
    (uint8_t *)bes2300_patch12_ins_data_3
};//acl fix interval

#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_3 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x0001b77c,
    0xbf00e001,
    0,
    NULL
};//start enc always accept
#endif

#if 0
const uint32_t bes2300_patch13_ins_data_3[] =
{
    0xf8842300,
    0xf89430b4,
    0xf00050b2,
    0x4285f807,
    0xbf00d102,
    0xb808f624,
    0xb820f624,
    0x781b4b07,
    0xd1032b01,
    0xf8934b05,
    0x47700042,
    0xbf062b02,
    0xf8934b02,
    0x2003004a,
    0xbf004770,
    0xc00062ac,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch13_3 =
{
    13,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch13_ins_data_3),
    0x0002acfc,
    0xbff0f1db,
    0xc0006ce0,
    (uint8_t *)bes2300_patch13_ins_data_3
};//sco role switch record slave acl bt clk
#else



const uint32_t bes2300_patch13_ins_data_3[] =
{
    0xf5fa4620,
    0x285dfce1,
    0xf240d10e,
    0x46215005,
    0xf85af5fa,
    0xf8534b05,
    0x46203026,
    0x135ef893,
    0xfc66f5fa,
    0xbc73f616,
    0xbc6bf616,
    0xc0005bcc,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch13_3 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch13_ins_data_3),
    0x0001d2cc,
    0xbb80f1e9,
    0xc00069d0,
    (uint8_t *)bes2300_patch13_ins_data_3
};

#endif//power control not accept

const uint32_t bes2300_patch14_ins_data_3[] =
{
    0xf8534b39,
    0xf8d22020,
    0xb12330fc,
    0x2b06789b,
    0x2b08d063,
    0xf892d063,
    0xb11330c6,
    0xb2c01e58,
    0x4b324770,
    0x2b01681b,
    0xe92dd15b,
    0x4b3047f0,
    0x087f781f,
    0x3cfff117,
    0xf04fbf18,
    0x4d2d0c01,
    0xf1a52401,
    0xf8550e08,
    0x2a002d04,
    0xf892d03e,
    0x42833046,
    0xf892d13a,
    0xbbbb304a,
    0x6027f85e,
    0x8034f8d6,
    0x302cf85e,
    0x45986b5b,
    0xebc3bf8c,
    0xebc80808,
    0xf8960803,
    0x6b539042,
    0xd9094299,
    0xa042f892,
    0xe0004453,
    0xeb034633,
    0x4299060a,
    0x6353d8fa,
    0xf0231a5b,
    0xf8924378,
    0xfbb36042,
    0xfb06f2f6,
    0xb98b3312,
    0xf3f9fbb8,
    0x8313fb09,
    0x2b063b03,
    0xb2e0d80a,
    0x4a0f0164,
    0xf42358a3,
    0xf443037f,
    0x50a33380,
    0x87f0e8bd,
    0xf1b43c01,
    0xd1b83fff,
    0xe8bd2002,
    0x200087f0,
    0x20004770,
    0x20024770,
    0xbf004770,
    0xc000095c,//ld_acl_env
    0xc0006d44,//inactive_sco_en
    0xc0006009,//bt_voice_active
    0xc0000954,//ld_sco_env+0x08
    0xd0220150,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch14_3 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch14_ins_data_3),
    0x000282fc,
    0xb806f1dc,
    0xc000430C,
    (uint8_t *)bes2300_patch14_ins_data_3
};//ld_acl_sco_rsvd_check


const uint32_t bes2300_patch15_ins_data_3[] =
{
    0x781b4bc9,
    0xf0002b00,
    0xe92d81c9,
    0xb0834ff0,
    0x460f9201,
    0x20024606,
    0xff66f607,
    0xd1102802,
    0x20b2f896,
    0x40932301,
    0xf8b24ac0,
    0xea2221c0,
    0xf3c30303,
    0x4abe0341,
    0xa023f852,
    0x0928f10a,
    0xf04fe002,
    0x46ca0900,
    0xf8b34bb8,
    0xf3c331c0,
    0x2b0303c1,
    0x4bb5d103,
    0x41c2f893,
    0x2b00e005,
    0x81a4f000,
    0xf8934bb1,
    0xf62041c2,
    0x4605fde9,
    0xf0002c02,
    0x4baf8195,
    0x8024f853,
    0x0f00f1b8,
    0x8190f000,
    0xb044f898,
    0x1042f898,
    0xf8d8460a,
    0x1a5b3034,
    0x4378f023,
    0xd9044298,
    0x4623189c,
    0xd8fb42a5,
    0x461ce000,
    0xf0402f00,
    0x290b8152,
    0x8146f240,
    0x781b4ba0,
    0xf89675b3,
    0x2b0130b3,
    0x80cdf040,
    0xf8b34b99,
    0xf3c331c0,
    0x2b0303c1,
    0xf896d142,
    0xf89820b2,
    0x429a3046,
    0xf5fed112,
    0x4a91fcc3,
    0xf89068d3,
    0x440b1025,
    0x636af5c3,
    0x82733306,
    0x3004f992,
    0x44233304,
    0x4378f023,
    0xe05560b3,
    0x681b4b8d,
    0xd1122b01,
    0xfcacf5fe,
    0x69134a85,
    0x1025f890,
    0xf5c3440b,
    0x3306636a,
    0xf9928273,
    0x33043005,
    0xf0234423,
    0x60b34378,
    0xf5fee03e,
    0x4a7cfc99,
    0xf8906913,
    0x440b1025,
    0x636af5c3,
    0x82733306,
    0x3005f992,
    0x44233302,
    0x4378f023,
    0xe02b60b3,
    0x20b2f896,
    0x3046f898,
    0xd112429a,
    0xfc80f5fe,
    0x68d34a6f,
    0x1025f890,
    0xf5c3440b,
    0x3306636a,
    0xf9928273,
    0x33043004,
    0xf0234423,
    0x60b34378,
    0xf8dfe012,
    0xf8bbb19c,
    0xf5fe7010,
    0xf607fc69,
    0xf89067a6,
    0x1aff3025,
    0xf8db8277,
    0x33023010,
    0xf0234423,
    0x60b34378,
    0x0f00f1ba,
    0xf1b9d02d,
    0xd02a0f00,
    0x308bf899,
    0xd1262b01,
    0x3008f8da,
    0x429368b2,
    0x1a99bf8c,
    0xf0211ad1,
    0x29064178,
    0xf898d809,
    0xeb033042,
    0xeb020343,
    0xf0230343,
    0x60b34378,
    0x429de02e,
    0x1aebbf8c,
    0xf0231b5b,
    0x2b054378,
    0xf898d809,
    0xeb033042,
    0xeb020343,
    0xf0230343,
    0x60b34378,
    0xf642e01c,
    0x68b4faa3,
    0x42a36803,
    0xf642d907,
    0x6803fa9d,
    0x1a9b68b2,
    0x4378f023,
    0xf642e005,
    0x6803fa95,
    0xf0231ae3,
    0x2b064378,
    0xf898d806,
    0x68b32042,
    0xf0234413,
    0x60b34378,
    0x210068b0,
    0xf8f0f63c,
    0xf2002806,
    0x200180ad,
    0x4b33e0b2,
    0x31c0f8b3,
    0x03c1f3c3,
    0xd1392b03,
    0x20b2f896,
    0x3046f898,
    0xd115429a,
    0x63a6f640,
    0xf8988273,
    0x4a293042,
    0x2006f992,
    0xf8b63204,
    0xfb9110b8,
    0xfb01f1f3,
    0x44133303,
    0xf0234423,
    0x60b34378,
    0xe08f2001,
    0x13c4f640,
    0xf8988273,
    0xf1bb3042,
    0xbf280f01,
    0x0b01f04f,
    0x0201f10b,
    0x10b8f8b6,
    0xf1f3fb91,
    0x3303fb01,
    0x0242eb03,
    0xf9934b16,
    0x44133006,
    0xf0234423,
    0x60b34378,
    0xe0712001,
    0xfbc2f5fe,
    0x3025f890,
    0x631cf5c3,
    0x82733304,
    0x3042f898,
    0x0f02f1bb,
    0xf04fbf28,
    0xf10b0b02,
    0xf8b60201,
    0xfb9110b8,
    0xfb01f1f3,
    0xeb033303,
    0x4b050242,
    0x3006f993,
    0x44234413,
    0x4378f023,
    0x200160b3,
    0xbf00e04e,

    0xc0006054,
    0xc0005c90,//ld_env
    0xc000095c,//ld_acl_env
    0xc000094c,//ld_sco_env
    0xc0000200,//ld_sco_env
    0xc0006c94,//acl_rearrange_flag

    0x43e2f240,
    0x34028273,
    0x4478f024,
    0x200160b4,
    0x2f01e038,
    0x68b3d106,
    0xf023440b,
    0x60b34378,
    0xe02f2001,
    0xd11f2f02,
    0x0b02f10b,
    0xf5b38ab3,
    0xbf947fb9,
    0x23012300,
    0x0b4bebc3,
    0xf9934b13,
    0x445b3007,
    0xfb929a01,
    0xfb02f2f1,
    0x440b1101,
    0xf0234423,
    0x60b34378,
    0xe0132001,
    0x47702000,
    0xe00f2000,
    0xe00d2000,
    0xe00b2001,
    0xfc48f620,
    0xe0072000,
    0x2042f898,
    0x441368b3,
    0x4378f023,
    0x200160b3,
    0xe8bdb003,
    0xbf008ff0,

    0xc0006054,//dbg_dualmode
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch15_3 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_data_3),
    0x0002a9dc,
    0xbd1af1d9,
    0xc0004414,
    (uint8_t *)bes2300_patch15_ins_data_3
};


const uint32_t bes2300_patch16_ins_data_3[] =
{
    0xf8b34b06,
    0xf3c331c0,
    0x2b0303c1,
    0x2201d102,
    0x601a4b03,
    0xe8bdb011,
    0xbf008ff0,
    0xc0005c90,
    0xc0006d44, //inactive_sco_en
    0x00000000
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch16_3 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch16_ins_data_3),
    0x00031000,
    0xbe8ef1d5,
    0xc0006d20,
    (uint8_t *)bes2300_patch16_ins_data_3
};///ld sco start



const uint32_t bes2300_patch17_ins_data_3[] =
{
    0xf6254651,
    0x4680f9d7,
    0xf6052002,
    0x2802fb83,
    0xf894d13b,
    0xf04f3046,
    0xfa090901,
    0x4b1ef903,
    0x31c0f8b3,
    0x0909ea23,
    0x0941f3c9,
    0x681b4b1b,
    0x4648b133,
    0xfd12f629,
    0xbf082805,
    0x0801f04f,
    0x2096f8b5,
    0xf8534b17,
    0xf8b33029,
    0xb2103096,
    0x4288b219,
    0x1a43dd01,
    0xb21be002,
    0x1a9bb212,
    0x02e1f1a3,
    0xd9032a0e,
    0x1381f2a3,
    0xd8092b0e,
    0x4b0d2201,
    0xf894601a,
    0xf44f9047,
    0xbf003800,
    0xba75f629,
    0x4b082200,
    0xf894601a,
    0xea4f9047,
    0xbf004808,
    0xba63f629,

    0xc0005c90,//ld_env
    0xc0006c88,//music_ongoing_flag
    0x00000000,
    0xc000095c,//ld_acl_env
    0xc0006c94,//acl_rearrange_flag
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_3 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch17_ins_data_3),
    0x00030138,
    0xbd56f1d6,
    0xc0006be8,
    (uint8_t *)bes2300_patch17_ins_data_3
};//ld_sco_evt_start_cbk

#if 0
const uint32_t bes2300_patch18_ins_data_3[] =
{
    0x781b4b05,
    0x0f53ebb7,
    0x2201d004,
    0x701a4b03,
    0x81f0e8bd,
    0xba17f625,
    0xc0006009, //bt_voice_active
    0xc000096a,//inactive_sco_return
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_3 =
{
    18,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch18_ins_data_3),
    0x0002c0c4,
    0xbe54f1da,
    0xc0006d70,
    (uint8_t *)bes2300_patch18_ins_data_3
};//ld_sco_sched 1
#else
const uint32_t bes2300_patch18_ins_data_3[] =
{
    0x781b4b06,
    0x0f53ebb7,
    0x2201d006,
    0x701a4b04,
    0xe8bdb003,
    0xbf008ff0,
    0xb8f5f625,
    0xc0006009, //bt_voice_active
    0xc000096a,//inactive_sco_return
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_3 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch18_ins_data_3),
    0x0002c0c4,
    0xbf74f1da,
    0xc0006fb0,
    (uint8_t *)bes2300_patch18_ins_data_3
};//ld_sco_sched 1

#endif


const uint32_t bes2300_patch19_ins_data_3[] =
{
    0x4b332201,
    0x2200601a,
    0x601a4b32,
    0xf8b34b32,
    0xf01331c0,
    0xd0570318,
    0x2b0310db,
    0x4b2ed103,
    0x01c2f883,
    0x2b02e00c,
    0x2201d104,
    0xf8834b2a,
    0xe00521c2,
    0xd1032b01,
    0x4b272200,
    0x21c2f883,
    0xf8934b25,
    0x4b2521c2,
    0x4022f853,
    0xf6072002,
    0xb1d8fd63,
    0xf8934b20,
    0x230151c2,
    0x4a1b40ab,
    0x429a7812,
    0x2101d012,
    0x70114a1a,
    0x70134a17,
    0x3042f894,
    0xd9092b02,
    0xfb0ef5fe,
    0x78820229,
    0x500bf240,
    0x0101f041,
    0xf8b6f5fc,
    0xf6072001,
    0x2802fd43,
    0xf894d128,
    0x2801004b,
    0x4c27d13e,
    0x4b236822,
    0x51b9f893,
    0x11b8f893,
    0x2105ea41,
    0x51c2f893,
    0x2105ea41,
    0x0201f002,
    0x6022430a,
    0x21c2f893,
    0xbb11f627,
    0xbb1cf627,

    0xc0006009,
    0xc00048d0,
    0xc0006d44,
    0xc0005c90,
    0xc000094c,
    0xc000096a,
    0xd02201b0,

    0xbb29f627,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_3 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch19_ins_data_3),
    0x0002be30,
    0xbcd8f1d8,
    0xc00047e4,
    (uint8_t *)bes2300_patch19_ins_data_3
};//ld_sco_dynamic_switch


const uint32_t bes2300_patch20_ins_data_3[] =
{
    0x0201f042,
    0xf893600a,
    0xf62521c2,
    0xbf00b88e,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch20_3 =
{
    20,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch20_ins_data_3),
    0x0002bec8,
    0xbf6cf1da,
    0xc0006da4,
    (uint8_t *)bes2300_patch20_ins_data_3
};//ld_sco_dynamic_switch


const uint32_t bes2300_patch21_ins_data_3[] =
{
    0xf8534b15,
    0xf6294020,
    0x4b14fdf1,
    0x31c0f8b3,
    0x0318f003,
    0x2801b19b,
    0x4b10d011,
    0x21c2f893,
    0xf8534b0f,
    0x4b0f2022,
    0xb12b681b,
    0xd1102b01,
    0x3046f892,
    0xd10c42b3,
    0x76237de3,
    0xf894e009,
    0x7de350c4,
    0xb2ed441d,
    0xf9ecf5fc,
    0x1aed78c3,
    0xbf007625,
    0xb88bf624,

    0xc000095c,
    0xc0005c90,
    0xc000094c,
    0xc0006c88,//music_ongoing_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_3 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch21_ins_data_3),
    0x0002abc0,
    0xbf4af1db,
    0xc0006a58,
    (uint8_t *)bes2300_patch21_ins_data_3
};//ld_acl_sched


#if 0
const uint32_t bes2300_patch22_ins_data_3[] =
{
    0xf8534b08,
    0x4a064020,
    0xf0436813,
    0x60130301,
    0x68133a70,
    0x4380f443,
    0xbf006013,
    0xb9eff625,
    0xd02201b0,
    0xc000094c
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch22_3 =
{
    22,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch22_ins_data_3),
    0x0002c288,
    0xbe02f1da,
    0xc0006e90,
    (uint8_t *)bes2300_patch22_ins_data_3
};//ld_sco_switch_to_handler 1
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch22_3 =
{
    22,
    BTDRV_PATCH_ACT,
    0,
    0x000190a8,
    0xbf00e003,
    0,
    NULL
};////iocap rsp

#endif

const uint32_t bes2300_patch23_ins_data_3[] =
{
    0xf8534b20,
    0x01474020,
    0x58bb4a1f,
    0x4380f443,
    0x4b1e50bb,
    0x31c0f8b3,
    0x03c1f3c3,
    0xd1022b03,
    0x4b1b2201,
    0x4b1b601a,
    0xb1c3781b,
    0x4b192200,
    0xe006701a,
    0x2042f894,
    0x44136b63,
    0x4378f023,
    0x6b656363,
    0xfa0df640,
    0xd3f34285,
    0x2042f894,
    0xeb036b63,
    0x63630342,
    0xf6254630,
    0xf894fa8d,
    0x2b01304b,
    0x4a09d10c,
    0x11b9f892,
    0x31b8f892,
    0x2301ea43,
    0x2306ea43,
    0x0301f043,
    0xbbaaf625,
    0xbbb3f625,

    0xc000094c,
    0xd0220140,
    0xc0005c90,
    0xc0006d44,
    0xc000096a,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_3 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch23_ins_data_3),
    0x0002c288,
    0xbc3af1da,
    0xc0006b00,
    (uint8_t *)bes2300_patch23_ins_data_3
};//ld_sco_switch_to_handler all


const uint32_t bes2300_patch24_ins_data_3[] =
{
    0x4b0b2201,
    0x2200601a,
    0x601a4b0b,
    0xf8b34b0b,
    0xf01331c0,
    0xd0090318,
    0x2b0310db,
    0x4b07d104,
    0x01c2f883,
    0xb94cf625,
    0xb93cf625,
    0xb990f625,

    0xc0006bcc,//switch_sco_flag
    0x00000000,
    0xc0006d44,//inactive_sco_en
    0xc0005c90,//ld_env
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch24_3 =
{
    24,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch24_ins_data_3),
    0x0002be30,
    0xbeb2f1da,
    0xc0006b98,
    (uint8_t *)bes2300_patch24_ins_data_3
};//ld_sco_dynamic_switch


#if 0
const uint32_t bes2300_patch25_ins_data_3[] =
{
    0x0301f023,
    0x3a706013,
    0xf4236813,
    0x60134380,
    0x4b022200,
    0xbf00601a,
    0xbc17f623,
    0xc0006d44
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_3 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch25_ins_data_3),
    0x0002a724,
    0xbbdcf1dc,
    0xc0006ee0,
    (uint8_t *)bes2300_patch25_ins_data_3
};//ld_sco_end
#else
const uint32_t bes2300_patch25_ins_data_3[] =
{
    0x4b042200,
    0x0000701a,
    0x8310f3ef,
    0x0401f013,
    0xbdf4f62a,
    0xc0006d44,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_3 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch25_ins_data_3),
    0x00031ad4,
    0xba04f1d5,
    0xc0006ee0,
    (uint8_t *)bes2300_patch25_ins_data_3
};//ld_sco_stop_link set inactive_sco_en=0
#endif


#if 0
const BTDRV_PATCH_STRUCT bes2300_ins_patch26_3 =
{
    26,
    BTDRV_PATCH_INACT,
    0,
    0x000294a4,
    0xbf00e00e,
    0,
    NULL
};//ld_acl_clk_isr
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch26_3 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00019d80,
    0xbf00e003,
    0,
    NULL
};//power rsp processs
#endif


const uint32_t bes2300_patch27_ins_data_3[] =
{
    0x4c1cb538,
    0x5020f854,
    0xf8b44c1b,
    0xf3c331c0,
    0x2b0303c1,
    0xf895d126,
    0x2b0130b3,
    0xf894d124,
    0x428331c2,
    0x4615d022,
    0x2001460c,
    0xf9e0f605,
    0xd11d2801,
    0xf8934b10,
    0x4b1021c2,
    0x2022f853,
    0x42a36893,
    0xf892d207,
    0x44132042,
    0xbf88429c,
    0x70fff64f,
    0x1b18d801,
    0x42a8e7ff,
    0x2000bf8c,
    0xbd382001,
    0xbd382000,
    0xbd382000,
    0xbd382000,
    0xbd382000,
    0xc000095c,
    0xc0005c90,
    0xc000094c,
    0x30c7f895,
    0xd10b2b00,
    0xf85ef61e,
    0x46204601,
    0xf7ff2205,
    0x2800ffb5,
    0xbf00d102,
    0xbfc1f626,
    0xbfabf626,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch27_3 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch27_ins_data_3),
    0x0002df00,
    0xb844f1d9,
    0xc0006f0c,
    (uint8_t *)bes2300_patch27_ins_data_3
};//acl frm isr

const uint32_t bes2300_patch28_ins_data_3[] =
{
    0xf6052002,
    0x2802fb0b,
    0xf89bd10d,
    0xf5fc40b2,
    0x7c03f8c5,
    0xd00642a3,
    0xf9e6f60c,
    0x2300b918,
    0x30bbf88b,
    0xf89be006,
    0x3b0130bb,
    0xf88bb2db,
    0xbf0030bb,
    0xbd03f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch28_3 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_data_3),
    0x0002d718,
    0xbae2f1d9,
    0xc0006ce0,
    (uint8_t *)bes2300_patch28_ins_data_3
};//ld_acl_rx rx_traffic--

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_3 =
{
    29,
#ifdef __CLK_GATE_DISABLE__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x00006d14,
    0xbf30bf00,
    0,
    NULL
};///disable clock gate

const uint32_t bes2300_patch30_ins_data_3[] =
{
    0x20b4f895,
    0xd1132a02,
    0x4b0ab983,
    0x0024f853,
    0xfb4cf640,
    0x30b5f895,
    0xd1032b04,
    0x21164620,
    0xfc7ef622,
    0x21164620,
    0xf942f623,
    0xb97ef627,
    0xb968f627,

    0xc000095c,//ld_acl_env
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_3 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_data_3),
    0x0002df98,
    0xbe7ef1d8,
    0xc0006c98,
    (uint8_t *)bes2300_patch30_ins_data_3
};//ld_acl_sket_isr


const uint32_t bes2300_patch31_ins_data_3[] =
{
    0xfbb24a02,
    0xf5f9f3f3,
    0xbf00bb3a,
    0x9af8da00,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_3 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch31_ins_data_3),
    0x00000650,
    0xbcc2f206,
    0xc0006fd8,
    (uint8_t *)bes2300_patch31_ins_data_3
};//lpo timer

const uint32_t bes2300_patch32_ins_data_3[] =
{
    0x781b4b02,
    0xf8807583,
    0x477030ad,
    0xc0006ffc,
    0x0000000f,//priority
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch32_3 =
{
    32,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch32_ins_data_3),
    0x00043eb8,
    0xb898f1c3,
    0xc0006fec,
    (uint8_t *)bes2300_patch32_ins_data_3
};//change rwip_priority[RWIP_PRIO_LE_CON_IDLE_IDX].value

const uint32_t bes2300_patch33_ins_data_3[] =
{
    0x781b4b02,
    0xf8807583,
    0x477030ad,
    0xc0007010,
    0x0000000f,//priority
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch33_3 =
{
    33,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch33_ins_data_3),
    0x00043ec8,
    0xb89af1c3,
    0xc0007000,
    (uint8_t *)bes2300_patch33_ins_data_3
};//change rwip_priority[RWIP_PRIO_LE_CON_ACT_IDX].value

const uint32_t bes2300_patch34_ins_data_3[] =
{
    0x736c60ab,
    0x681b4b04,
    0x0f53ebb4,
    0x4b03bf04,
    0xbf00601f,
    0xb8bef621,

    0xc0006009,//bt_voice_active
    0xc00042d4,//sco_timestamp_push
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch34_3 =
{
    34,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch34_ins_data_3),
    0x000253e0,
    0xbf36f1de,
    0xc0004250,
    (uint8_t *)bes2300_patch34_ins_data_3
};//ld_fm_prog_push

const uint32_t bes2300_patch35_ins_data_3[] =
{
    0x892b4806,
    0x123ff240,
    0xd001421a,
    0xe0002201,
    0x60022200,
    0x3086f897,
    0xbc14f63a,
    0xc0007038,//no_packet flag
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch35_3 =
{
    35,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch35_ins_data_3),
    0x00041858,
    0xbbdef1c5,
    0xc0007018,
    (uint8_t *)bes2300_patch35_ins_data_3
};//lld_pdu_rx_handler check ble rx

const uint32_t bes2300_patch36_ins_data_3[] =
{
    0x4628d805,
    0xf8b52102,
    0xf62320a8,
    0x4b06fcc9,
    0xb12b681b,
    0x23130000,
    0x220075ab,
    0x601a4b02,
    0x00004628,
    0xbc8cf63c,
    0xc0007038,//no_packet flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch36_3 =
{
    36,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch36_ins_data_3),
    0x0004396c,
    0xbb66f1c3,
    0xc000703c,
    (uint8_t *)bes2300_patch36_ins_data_3
};//lld_evt_end


const uint32_t bes2300_patch37_ins_data_3[] =
{
    /*706c*/ 0xd1052b01,
    /*7070*/ 0x2b0178f3,
    /*7074*/ 0xbf00d902,
    /*7078*/ 0xbc28f617, // jump a0207078 -> a001e8cc
    /*707c*/ 0xbc30f617, // jump a020707c -> a001e8e0
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch37_3 =
{
    37,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch37_ins_data_3),
    0x0001e8c8,
    0xbbd0f1e8, // jump a001e8c8 -> a020706c
    0xc000706c,
    (uint8_t *)bes2300_patch37_ins_data_3
};//LMP_MSG_HANDLER(feats_res_ext)

const uint32_t bes2300_patch38_ins_data_3[] =
{
    0x781b4b0b,
    0x0f53ebb4,
    0x4b0ed00e,
    0x31c0f8b3,
    0x0318f003,
    0xd0072b00,
    0x4b062201,
    0x4b07601a,
    0x0000601c,
    0xbb4cf623,
    0x22000164,
    0xbb46f623,
    0xc0006009,
    0xc00070b8,//acl_end
    0x00000000,
    0xc00070c0,//background_sco_link
    0x000000ff,
    0xc0005c90,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch38_3 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch38_ins_data_3),
    0x0002a738,
    0xbca2f1dc,
    0xc0007080,
    (uint8_t *)bes2300_patch38_ins_data_3
};//ld_sco_end:
//two sco,background sco end, set link and acl_end

const uint32_t bes2300_patch39_ins_data_3[] =
{
    0x681b4b0a,
    0xd00c2b00,
    0x681b4b09,
    0xd0082bff,
    0x2200015b,
    0x505a4907,
    0x601a4b04,
    0x4b0422ff,
    0x4620601a,
    0x00004639,
    0xbea0f626,
    0xc00070b8,
    0xc00070c0,
    0xd0220140,
    0xc000094c,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch39_3 =
{
    39,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch39_ins_data_3),
    0x0002de30,
    0xb94af1d9,
    0xc00070c8,
    (uint8_t *)bes2300_patch39_ins_data_3
};//ld_acl_frm_isr
//if acl_end and link, bt_e_scochancntl_pack(background_sco_link, 0, 0, 0, 0, 0);

const uint32_t bes2300_patch40_ins_data_3[] =
{
    0xf895212e,
    0xf61c2044,
    0xb2e4f819,
    0x78714620,
    0xfbd8f629,
    0xbe40f613,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch40_3 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch40_ins_data_3),
    0x0001ab04,
    0xb9d8f1ec,
    0xc0006eb8,
    (uint8_t *)bes2300_patch40_ins_data_3
};//LMP_MSG_HANDLER(max_slot_req)

const uint32_t bes2300_patch41_ins_data_3[] =
{
    0xf5fc7de2,
    0x78c1f94f,
    0xbf001a51,
    0xbd5bf629,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch41_3 =
{
    41,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch41_ins_data_3),
    0x00030698,
    0xba9ef1d6,
    0xc0006bd8,
    (uint8_t *)bes2300_patch41_ins_data_3
};//ld_acl_rx_max_slot_set

const uint32_t bes2300_patch42_ins_data_3[] =
{
    0x4b1768a2,
    0x429a681b,
    0x1ad3bf8c,
    0xf8941a9b,
    0x1d912042,
    0xd801428b,
    0xd21d4293,
    0x3046f894,
    0x4a110159,
    0x2a00588a,
    0x8f61da08,
    0xf44f0049,
    0xfb026207,
    0x480d1303,
    0xe0094418,
    0x4a0b8f21,
    0xf44f440a,
    0xfb006007,
    0x8f602303,
    0x0040eb03,
    0x8f622100,
    0xfcd1f643,
    0x6b6368a2,
    0xbe72f62b,

    0xc00042d4,//sco_timestamp_push
    0x00000000,
    0xd0220140,
    0xd021449c,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch42_3 =
{
    42,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch42_ins_data_3),
    0x0002ffb0,
    0xb95ef1d4,
    0xc0004270,
    (uint8_t *)bes2300_patch42_ins_data_3
};//ld_sco_evt_start_cbk

const uint32_t bes2300_patch43_ins_data_3[] =
{
    0x3044f885,
    0x22024b02,
    0x541a4630,
    0xbb06f614,
    0xc00068b4
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch43_3 =
{
    43,
#ifdef BT_SW_SEQ
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch43_ins_data_3),
    0x0001af90,
    0xbcf2f1eb,
    0xc0006978,
    (uint8_t *)bes2300_patch43_ins_data_3
};/*lc_rsw_end_ind*/

const uint32_t bes2300_patch44_ins_data_3[] =
{
    0xf0135a9b,
    0xd00f0f80,
    0xd10d2c00,
    0x30abf890,
    0xb2db3b03,
    0xd8052b01,
    0x4b062201,
    0x7d83601a,
    0x75834413,
    0xbc02f63c,
    0x4b022200,
    0x0000601a,
    0xbb53f63c,
    0xc00071a4,//ble_unstarted_flag
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch44_3 =
{
    44,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch44_ins_data_3),
    0x00043838,
    0xbc98f1c3,
    0xc000716c,
    (uint8_t *)bes2300_patch44_ins_data_3
};//lld_evt_end

const uint32_t bes2300_patch45_ins_data_3[] =
{
    0xd1092d00,
    0x68124a05,
    0xd1052a00,
    0x49032200,
    0x0000600a,
    0xbd39f63b,
    0xbdbaf63b,
    0xc00071a4,//ble_unstarted_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch45_3 =
{
    45,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch45_ins_data_3),
    0x00042c2c,
    0xbabcf1c4,
    0xc00071a8,
    (uint8_t *)bes2300_patch45_ins_data_3
};//lld_evt_restart

const uint32_t bes2300_patch46_ins_data_3[] =
{
    0x4b032101,
    0xf8946019,
    0x0000309d,
    0xbb9ef63b,
    0xc00071dc,//ble_param_update
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch46_3 =
{
    46,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch46_ins_data_3),
    0x00042910,
    0xbc5af1c4,
    0xc00071c8,
    (uint8_t *)bes2300_patch46_ins_data_3
};//lld_evt_restart

const uint32_t bes2300_patch47_ins_data_3[] =
{
    0x4b06460e,
    0x2b00681b,
    0x2200d004,
    0x601a4b03,
    0x75832313,
    0x00002501,
    0xbdc6f63b,
    0xc00071dc,//ble_param_update
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch47_3 =
{
    47,
#ifdef __BLE_PRIO_FOR_SCO__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch47_ins_data_3),
    0x00042d6c,
    0xba38f1c4,
    0xc00071e0,
    (uint8_t *)bes2300_patch47_ins_data_3
};//lld_evt_elt_insert

const uint32_t bes2300_patch48_ins_data_3[] =
{
    0x681b4b11,
    0xf5fcb173,
    0x7bc3fa39,
    0xf5fcb153,
    0x7c03fa35,
    0xd10542b3,
    0xf5fc8a67,
    0x6943fa2f,
    0x8263443b,
    0xfa2af5fc,
    0x3035f890,
    0x2002b14b,
    0xfc64f605,
    0xd1042802,
    0x681a4b05,
    0x44138a63,
    0xbf008263,
    0xb951f624,
    0xc0006a4c,//reconnecting_flag
    0x00000000,
    0xc0006a54,//dual_slave_slot_add
    0x000009c4,//4*slot_size
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch48_3 =
{
    48,
#if defined(__BT_ONE_BRING_TWO__)&&defined(MULTIPOINT_DUAL_SLAVE)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes2300_patch48_ins_data_3),
    0x0002acc8,
    0xbe9af1db,
    0xc0006a00,
    (uint8_t *)bes2300_patch48_ins_data_3
};//ld_acl_sched

const uint32_t bes2300_patch49_ins_data_3[] =
{
    0x02004638,
    0x0001f040,
    0xf83cf60d,
    0xbaa4f61c
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch49_3 =
{
    49,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch49_ins_data_3),
    0x00022ebc,
    0xbd68f1e3,
    0xc0006990,
    (uint8_t *)bes2300_patch49_ins_data_3
};//lc_send_lmp

const BTDRV_PATCH_STRUCT bes2300_ins_patch50_3 =
{
    50,
    BTDRV_PATCH_ACT,
    0,
    0x0002e430,
    0xe0022000,
    0,
    NULL
};///ld_acl_stop

const uint32_t bes2300_patch51_ins_data_3[] =
{
    0x68124a0c,
    0xd1132a01,
    0x68104a0b,
    0x60103001,
    0xd10d280c,//12
    0xf890480a,
    0x014001c2,
    0x58424909,
    0x4280f422,
    0x20005042,
    0x60104a03,
    0x60104a01,
    0xb948f625,
    0xc00048d0,//switch_sco_flag
    0xc000423c,//switch_delay_counter,
    0x00000000,
    0xc0005c90,//ld_env
    0xd0220140,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch51_3 =
{
    51,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch51_ins_data_3),
    0x000294a4,
    0xbeacf1da,
    0xc0004200,
    (uint8_t *)bes2300_patch51_ins_data_3
};//ld_acl_clk_isr, set switch to link
//(bt_e_scochancntl_e_scochanen_setf 1)

const uint32_t bes2300_patch52_ins_data_3[] =
{
    0xf8934b07,
    0xf0133084,
    0xd0070f06,
    0xf6324620,
    0x4601fb5f,
    0xbf002400,
    0xbceaf631,
    0xbca9f631,

    0xc0006344,//llm_le_env
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch52_3 =
{
    52,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch52_ins_data_3),
    0x000386bc,
    0xbb48f1ce,
    0xc0006d50,
    (uint8_t *)bes2300_patch52_ins_data_3
};//hci_le_set_adv_data_cmd_handler


const uint32_t bes2300_patch53_ins_data_3[] =
{
    0xd8092b01,
    0xfb03236e,
    0x4a04f308,
    0xf4135a9b,
    0xd1016f00,
    0xbb85f627,
    0xbc0df627,
    0xd02111a8
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch53_3 =
{
    53,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch53_ins_data_3),
    0x0002e92c,
    0xbc70f1d8,
    0xc0007210,
    (uint8_t *)bes2300_patch53_ins_data_3
};//tx null


const BTDRV_PATCH_STRUCT bes2300_ins_patch54_3 =
{
    54,
    BTDRV_PATCH_INACT,
    0,
    0x00028f0c,
    0x7f80f5b4,
    0,
    NULL
};//LD_ACL_STOP_NOTIF_BIT_OFF_THR

#if 0
const uint32_t bes2300_patch55_ins_data_3[] =
{
    0x0109f107,
    0xf5fc4608,
    0x2813fabd,
    0x7a3ad904,
    0xd8012a01,
    0xbcc9f613,
    0xbcc5f613,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch55_3 =
{
    55,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch55_ins_data_3),
    0x0001a478,
    0xbb2ef1ec,
    0xc0006ad8,
    (uint8_t *)bes2300_patch55_ins_data_3
};////LMP_MSG_HANDLER(set_afh) for afh < 20
#else
const uint32_t bes2300_patch55_ins_data_3[] =
{
    0x2040f896,
    0xd1112a01,
    0x0109f107,
    0xf5fc4608,
    0x2813fac5,
    0x221ed802,
    0xbd16f613,
    0x2b017a3b,
    0xf613d801,
    0x221ebcce,
    0xbd0ef613,
    0xbd0af613,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch55_3 =
{
    55,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch55_ins_data_3),
    0x0001a470,
    0xbb26f1ec,
    0xc0006ac0,
    (uint8_t *)bes2300_patch55_ins_data_3
};
#endif


/////2300 t6 patch
static const uint32_t best2300_ins_patch_config_3[] =
{
    56,
    (uint32_t)&bes2300_ins_patch0_3,
    (uint32_t)&bes2300_ins_patch1_3,
    (uint32_t)&bes2300_ins_patch2_3,
    (uint32_t)&bes2300_ins_patch3_3,
    (uint32_t)&bes2300_ins_patch4_3,
    (uint32_t)&bes2300_ins_patch5_3,
    (uint32_t)&bes2300_ins_patch6_3,
    (uint32_t)&bes2300_ins_patch7_3,
    (uint32_t)&bes2300_ins_patch8_3,
    (uint32_t)&bes2300_ins_patch9_3,
    (uint32_t)&bes2300_ins_patch10_3,
    (uint32_t)&bes2300_ins_patch11_3,
    (uint32_t)&bes2300_ins_patch12_3,
    (uint32_t)&bes2300_ins_patch13_3,
    (uint32_t)&bes2300_ins_patch14_3,
    (uint32_t)&bes2300_ins_patch15_3,
    (uint32_t)&bes2300_ins_patch16_3,
    (uint32_t)&bes2300_ins_patch17_3,
    (uint32_t)&bes2300_ins_patch18_3,
    (uint32_t)&bes2300_ins_patch19_3,
    (uint32_t)&bes2300_ins_patch20_3,
    (uint32_t)&bes2300_ins_patch21_3,
    (uint32_t)&bes2300_ins_patch22_3,
    (uint32_t)&bes2300_ins_patch23_3,
    (uint32_t)&bes2300_ins_patch24_3,
    (uint32_t)&bes2300_ins_patch25_3,
    (uint32_t)&bes2300_ins_patch26_3,
    (uint32_t)&bes2300_ins_patch27_3,
    (uint32_t)&bes2300_ins_patch28_3,
    (uint32_t)&bes2300_ins_patch29_3,
    (uint32_t)&bes2300_ins_patch30_3,
    (uint32_t)&bes2300_ins_patch31_3,
    (uint32_t)&bes2300_ins_patch32_3,
    (uint32_t)&bes2300_ins_patch33_3,
    (uint32_t)&bes2300_ins_patch34_3,
    (uint32_t)&bes2300_ins_patch35_3,
    (uint32_t)&bes2300_ins_patch36_3,
    (uint32_t)&bes2300_ins_patch37_3,
    (uint32_t)&bes2300_ins_patch38_3,
    (uint32_t)&bes2300_ins_patch39_3,
    (uint32_t)&bes2300_ins_patch40_3,
    (uint32_t)&bes2300_ins_patch41_3,
    (uint32_t)&bes2300_ins_patch42_3,
    (uint32_t)&bes2300_ins_patch43_3,
    (uint32_t)&bes2300_ins_patch44_3,
    (uint32_t)&bes2300_ins_patch45_3,
    (uint32_t)&bes2300_ins_patch46_3,
    (uint32_t)&bes2300_ins_patch47_3,
    (uint32_t)&bes2300_ins_patch48_3,
    (uint32_t)&bes2300_ins_patch49_3,
    (uint32_t)&bes2300_ins_patch50_3,
    (uint32_t)&bes2300_ins_patch51_3,
    (uint32_t)&bes2300_ins_patch52_3,
    (uint32_t)&bes2300_ins_patch53_3,
    (uint32_t)&bes2300_ins_patch54_3,
    (uint32_t)&bes2300_ins_patch55_3,

};

void btdrv_ins_patch_write(BTDRV_PATCH_STRUCT *ins_patch_p)
{
    uint32_t remap_addr;
    // uint8_t i=0;
    remap_addr =   ins_patch_p->patch_remap_address | 1;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_REMAP_ADDR_START + ins_patch_p->patch_index*4),
                       (uint8_t *)&ins_patch_p->patch_remap_value,4);
    if(ins_patch_p->patch_length != 0)  //have ram patch data
    {
#if 0
        for( ; i<(ins_patch_p->patch_length)/128; i++)
        {
            btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,
                               (ins_patch_p->patch_data+i*128),128);
        }

        btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,ins_patch_p->patch_data+i*128,
                           ins_patch_p->patch_length%128);
#endif
        btdrv_memory_copy((uint32_t *)ins_patch_p->patch_start_address,(uint32_t *)ins_patch_p->patch_data,ins_patch_p->patch_length);
    }

    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_COMP_ADDR_START + ins_patch_p->patch_index*4),
                       (uint8_t *)&remap_addr,4);

}

void btdrv_ins_patch_init(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0; i<best2300_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_ins_patch_config[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        //HAL_CHIP_METAL_ID_2
        for(uint8_t i=0; i<best2300_ins_patch_config_2[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_2[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_2[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        for(uint8_t i=0; i<best2300_ins_patch_config_3[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_3[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_ins_patch_config_3[i+1]);
        }
    }
}

///////////////////data  patch ..////////////////////////////////////
#if 0
0106 0109 010a 0114 0114 010a 0212 0105     ................
010a 0119 0119 0105 0105 0108 0214 0114     ................
0108 0114 010a 010a 0105 0105 0114 010a     ................
0b0f 0105 010a 0000
#endif

static const uint32_t bes2300_dpatch0_data[] =
{
    0x01090106,
    0x0114010a,
    0x010a0114,
    0x01050212,
    0x0119010a,
    0x01050119,
    0x01080105,
    0x01140214,
    0x01140108,
    0x010a010a,
    0x01050105,
    0x0b0f0114,
    0x01050b0f,
    0x0000010a,
};

static const BTDRV_PATCH_STRUCT bes2300_data_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_dpatch0_data),
    0x00043318,
    0xc0000058,
    0xc0000058,
    (uint8_t *)&bes2300_dpatch0_data
};

static const uint32_t best2300_data_patch_config[] =
{
    1,
    (uint32_t)&bes2300_data_patch0,

};






void btdrv_data_patch_write(const BTDRV_PATCH_STRUCT *d_patch_p)
{

    uint32_t remap_addr;
    uint8_t i=0;

    remap_addr = d_patch_p->patch_remap_address |1;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_DATA_COMP_ADDR_START + d_patch_p->patch_index*4),
                       (uint8_t *)&remap_addr,4);
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_DATA_REMAP_ADDR_START + d_patch_p->patch_index*4),
                       (uint8_t *)&d_patch_p->patch_remap_value,4);

    if(d_patch_p->patch_length != 0)  //have ram patch data
    {
        for( ; i<(d_patch_p->patch_length-1)/128; i++)
        {
            btdrv_write_memory(_32_Bit,d_patch_p->patch_start_address+i*128,
                               (d_patch_p->patch_data+i*128),128);

        }

        btdrv_write_memory(_32_Bit,d_patch_p->patch_start_address+i*128,d_patch_p->patch_data+i*128,
                           d_patch_p->patch_length%128);
    }

}


void btdrv_ins_patch_disable(uint8_t index)
{
    uint32_t addr=0;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_COMP_ADDR_START + index*4),
                       (uint8_t *)&addr,4);

}


void btdrv_data_patch_init(void)
{
    const BTDRV_PATCH_STRUCT *data_patch_p;
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        for(uint8_t i=0; i<best2300_data_patch_config[0]; i++)
        {
            data_patch_p = (BTDRV_PATCH_STRUCT *)best2300_data_patch_config[i+1];
            if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
                btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)best2300_data_patch_config[i+1]);
        }
    }
}


//////////////////////////////patch enable////////////////////////


void btdrv_patch_en(uint8_t en)
{
    uint32_t value[2];

    //set patch enable
    value[0] = 0x2f02 | en;
    //set patch remap address  to 0xc0000100
    value[1] = 0x20000100;
    btdrv_write_memory(_32_Bit,BTDRV_PATCH_EN_REG,(uint8_t *)&value,8);
}


const BTDRV_PATCH_STRUCT bes2300_ins_patch0_testmode_2 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x0002816c,
    0x789a3100,
    0,
    NULL
};///no sig tx test


const BTDRV_PATCH_STRUCT bes2300_ins_patch1_testmode_2 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0000b7ec,
    0xbf00e007,
    0,
    NULL
};///lm init

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_testmode_2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x0003faa4,
    0xbf002202,
    0,
    NULL
};//ble test power

const BTDRV_PATCH_STRUCT bes2300_ins_patch3_testmode_2 =
{
    3,
    BTDRV_PATCH_ACT,
    0,
    0x00023d64,
    0xbf00bd08,
    0,
    NULL
};//ld_channel_assess


const BTDRV_PATCH_STRUCT bes2300_ins_patch4_testmode_2 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x0003E280,
#ifdef BLE_POWER_LEVEL_0
    0x23008011,
#else
    0x23028011,
#endif
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_testmode_2 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x0003E284,
    0x021CbF00,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6_testmode_2 =
{
    6,
    BTDRV_PATCH_ACT,
    0,
    0x00018450,
    0x78b3bF00,
    0,
    NULL
};


const uint32_t bes2300_patch7_ins_test_data_2[] =
{
    0x30fcf8d4,
    0xd0092b00,
    0x680b4905,
    0x3f00f413,
    0x680bd004,
    0x3300f423,
    0xbf00600b,
    0xbaeff627,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch7_testmode_2 =
{
    7,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch7_ins_test_data_2),
    0x0002df58,
    0xbd12f1d8,
    0xc0006980,
    (uint8_t *)bes2300_patch7_ins_test_data_2
};



const uint32_t bes2300_patch8_ins_test_data_2[] =
{
    0x30fcf8d4,
    0xd00b2b00,
    0xf003789b,
    0x2b0503fd,
    0x4b04d106,
    0xf443681b,
    0x4a023300,
    0xbf006013,
    0xbc50f625,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch8_testmode_2 =
{
    8,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch8_ins_test_data_2),
    0x0002c140,
    0xbc36f1da,
    0xc00069b0,
    (uint8_t *)bes2300_patch8_ins_test_data_2
};




const uint32_t bes2300_patch9_ins_test_data_2[] =
{
    0x5bf24a03,
    0x080ff002,
    0xbccbf622,
    0xd0211160,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch9_testmode_2 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch9_ins_test_data_2),
    0x000290A0,
    0xbb30f1dd,
    0xc0006704,
    (uint8_t *)bes2300_patch9_ins_test_data_2
};


const uint32_t bes2300_patch10_ins_test_data_2[] =
{
    0xf0025bf2,
    0xbf00020f,
    0xbce8f622,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch10_testmode_2 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch10_ins_test_data_2),
    0x000290ec,
    0xbb12f1dd,
    0xc0006714,
    (uint8_t *)bes2300_patch10_ins_test_data_2
};



const uint32_t bes2300_patch11_ins_test_data_2[] =
{
    0xf0035bf3,
    0xbf00080f,
    0xbcfaf622,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch11_testmode_2 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch11_ins_test_data_2),
    0x0002911c,
    0xbb00f1dd,
    0xc0006720,
    (uint8_t *)bes2300_patch11_ins_test_data_2
};



const uint32_t bes2300_patch12_ins_test_data_2[] =
{
    0xf0035bf3,
    0xbf00010f,
    0xbd1af622,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_testmode_2 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch12_ins_test_data_2),
    0x0002916c,
    0xbae0f1dd,
    0xc0006730,
    (uint8_t *)bes2300_patch12_ins_test_data_2
};

#ifdef __POWER_CONTROL_TYPE_1__
const uint32_t bes2300_patch13_ins_test_data_2[] =
{
    0xf0035243,
    0x2b00030f,
    0x2000bf14,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch13_testmode_2 =
{
    13,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch13_ins_test_data_2),
    0x000033bc,
    0xbdb0f203,
    0xc0006f20,
    (uint8_t *)bes2300_patch13_ins_test_data_2
};//pwr controll
#else

const BTDRV_PATCH_STRUCT bes2300_ins_patch13_testmode_2 =
{
    13,
    BTDRV_PATCH_INACT,
    0,
    0x000033f4,
    0xe0072000,
    0,
    NULL
};
#endif

////for power control test mode
const BTDRV_PATCH_STRUCT bes2300_ins_patch14_testmode_2 =
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0x00028e8c,
    0xe0cfbf00,
    0,
    NULL
};

const uint32_t bes2300_patch15_ins_test_data_2[] =
{
    0xb2db1e63,
    0xbf002b01,
    0x30fcf8d6,
    0xf002789a,
    0x2a0502fd,
    0x4a2ad14a,
    0x11cbf892,
    0x4a2a0109,
    0xf3c25a8a,
    0x48292240,
    0x49216002,
    0x3201680a,
    0x4a20600a,
    0x8000f8c2,
    0xf3c29a05,
    0x491e02c9,
    0x491f600a,
    0x11cbf891,
    0x01c1ebc1,
    0xf830481e,
    0xf3c11011,
    0xbf002100,
    0xb119bf00,
    0x68014816,
    0x6001481f,
    0x600c4915,
    0x68004819,
    0x2100b150,
    0x60014817,
    0x68014817,
    0x6001480f,
    0x68014816,
    0x6001480e,
    0xf1008c18,
    0xf5004050,
    0x4b0c1004,
    0x31cbf893,
    0x03c3ebc3,
    0xf831490a,
    0xb2891013,
    0x4150f101,
    0x1104f501,
    0xfc6af640,
    0xba71f626,
    0xc0006864,///test_mode_acl_rx_flag
    0xc0006868,///loopback_type
    0xc000686c,///loopback_length
    0xc0006870,///loopback_llid
    0xc0005c0c,
    0xd02115a0,
    0xd021159a,
    0xc0006874,///rxseq_flag
    0xc0006878,///unack_seqerr_flag
    0xc000687c,///rxlength_flag
    0xc0006880,///rxllid_flag
    0xc0006884,///rxarqn_flag
    0xc0006888,///ok_rx_length_flag
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch15_testmode_2 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_test_data_2),
    0x0002cd04,
    0xbd3cf1d9,
    0xc0006780,
    (uint8_t *)bes2300_patch15_ins_test_data_2
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch16_testmode_2 =
{
    16,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};
////end for power control test mode


const uint32_t bes2300_patch17_ins_test_data_2[] =
{
    0x20014b04,
    0x4b026018,
    0xbf00689b,
    0xbc0ef610,
    0xc0004100,
    0xc00068d8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_testmode_2 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch17_ins_test_data_2),
    0x000170E8,
    0xbbeaf1ef,
    0xc00068C0,
    (uint8_t *)bes2300_patch17_ins_test_data_2
};


const uint32_t bes2300_patch18_ins_test_data_2[] =
{
    0x20024b04,
    0x4b026018,
    0xbf00689b,
    0xbdbaf611,
    0xc0004100,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_testmode_2 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch18_ins_test_data_2),
    0x00018460,
    0xba3ef1ee,
    0xc00068e0,
    (uint8_t *)bes2300_patch18_ins_test_data_2
};


const uint32_t bes2300_patch19_ins_test_data_2[] =
{
    0x20014b04,
    0x4b026018,
    0xbf0068db,
    0xbc1af610,
    0xc0004100,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_testmode_2 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch19_ins_test_data_2),
    0x00017140,
    0xbbdef1ef,
    0xc0006900,
    (uint8_t *)bes2300_patch19_ins_test_data_2
};


const uint32_t bes2300_patch20_ins_test_data_2[] =
{
    0x20024b04,
    0x4b026018,
    0x462068db,
    0xbda4f611,
    0xc0004100,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch20_testmode_2 =
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch20_ins_test_data_2),
    0x00018474,
    0xba54f1ee,
    0xc0006920,
    (uint8_t *)bes2300_patch20_ins_test_data_2
};


const uint32_t bes2300_patch21_ins_test_data_2[] =
{
    0xd91b429a,
    0x5a42b410,
    0xb2dc3301,
    0x427ff402,
    0x52424322,
    0x68104a0c,
    0xd00a2801,
    0xf8924a0b,
    0x42830030,
    0x2000bf14,
    0x4a072001,
    0x60112100,
    0x2000e001,
    0xf85d6010,
    0x47704b04,
    0x20004b02,
    0x20016018,
    0xbf004770,
    0xc00068d8,
    0xc0004100,


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_testmode_2 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch21_ins_test_data_2),
    0x000033e0,
    0xbaaef203,
    0xc0006940,
    (uint8_t *)bes2300_patch21_ins_test_data_2
};



const uint32_t bes2300_patch22_ins_test_data_2[] =
{
    0xf0135a43,
    0xd016030f,
    0x3b015a42,
    0xf402b2db,
    0x4313427f,
    0xf0135243,
    0x4a08030f,
    0x28016810,
    0x2b00d006,
    0x2000bf14,
    0x21002001,
    0xe0016011,
    0x60102000,
    0x20014770,
    0xbf004770,
    0xc00068d8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch22_testmode_2 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch22_ins_test_data_2),
    0x000033a8,
    0xbafaf203,
    0xc00069a0,
    (uint8_t *)bes2300_patch22_ins_test_data_2
};


#if 0
const uint32_t bes2300_patch23_ins_test_data_2[] =
{
    0xf8934b14,
    0xebc331cb,
    0x005b03c3,
    0x4812461f,
    0x4a124910,
    0xf2414d12,
    0x5a3b5496,
    0x030ef3c3,
    0xf891523b,
    0x330131cb,
    0xd5034013,
    0xf0633b01,
    0x33010303,
    0x31cbf881,
    0xebc3b2db,
    0x005b03c3,
    0x682e461f,
    0xf3c64423,
    0x42b3060e,
    0xbf00d1e5,
    0xba3df626,
    0xc0005c0c,
    0xd0211596,
    0x80000003,
    0xd022002c,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_testmode_2 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch23_ins_test_data_2),
    0x0002cdf4,
    0xbdfcf1d9,
    0xc00069f0,
    (uint8_t *)bes2300_patch23_ins_test_data_2
};
#else

const uint32_t bes2300_patch23_ins_test_data_2[] =
{
    0x60b39b00,
    0xfb00206e,
    0x4a0bf004,
    0xf0436813,
    0x60130302,
    0x4b092202,
    0xf5a3601a,
    0x3bb6436e,
    0x3003f830,
    0x030cf3c3,
    0x5300f443,
    0xf8204a04,
    0xbf003002,
    0xb840f625,
    0xD022000C,
    0xd0220018,
    0xd0211162,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_testmode_2 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch23_ins_test_data_2),
    0x0002baa4,
    0xbfa4f1da,
    0xc00069f0,
    (uint8_t *)bes2300_patch23_ins_test_data_2
};
#endif
const BTDRV_PATCH_STRUCT bes2300_ins_patch24_testmode_2 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x0003772c,
    0xf7c92100,
    0,
    NULL,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch25_testmode_2 =
{
    25,
    BTDRV_PATCH_ACT,
    0,
    0x00024148,
    0xbf00e002,
    0,
    NULL,
};

#if 0
const BTDRV_PATCH_STRUCT bes2300_ins_patch26_testmode_2 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00024024,
    0xbf00bf00,
    0,
    NULL,
};
#else
const uint32_t bes2300_patch26_ins_test_data_2[] =
{
    0x3a063006,
    0xfa57f640,
    0xb9def61d,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch26_testmode_2 =
{
    26,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch26_ins_test_data_2),
    0x00024024,
    0xbe1cf1e2,
    0xc0006c60,
    (uint8_t *)bes2300_patch26_ins_test_data_2
};
#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch27_testmode_2 =
{
    27,
#ifdef __CLK_GATE_DISABLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000061a8,
    0xbf30bf00,
    0,
    NULL
};///disable clock gate

const uint32_t bes2300_patch28_ins_test_data_2[] =
{
    0x0f03f1b8,
    0xf1b8d002,
    0xd1010f08,
    0xe0192401,
    0xf8934b0d,
    0xebc331cb,
    0x4a0c03c3,
    0x3013f832,
    0x2300f3c3,
    0x60134a0f,
    0x4b09b953,
    0x4b0c681a,
    0x4b08601a,
    0x4b08681a,
    0x2201601a,
    0x601a4b07,
    0xb842f626,
    0xb945f626,
    0xc0005c0c,
    0xd021159a,
    0xc0006870,///loopback_llid
    0xc000686c,///loopback_length
    0xc000687c,///rxlength_flag
    0xc0006878,///unack_seqerr_flag
    0xc0006880,///rxllid_flag
    0xc0006884,////rxarqn_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch28_testmode_2 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_test_data_2),
    0x0002ccf4,
    0xbea4f1d9,
    0xc0006a40,
    (uint8_t *)bes2300_patch28_ins_test_data_2
};///ld_acl_rx:skip rxseq_err


const uint32_t bes2300_patch29_ins_test_data_2[] =
{
    0xb2d23a05,
    0xd9012a03,
    0xb8a2f000,
    0xd0022a00,
    0xf0402a02,
    0xbf00809f,
    0x681b4b4f,
    0xf0002b00,
    0xf8d58099,
    0x881a30fc,
    0xf00179d9,
    0xb90a010f,
    0x881a4b4a,
    0x236e9804,
    0xf000fb03,
    0x5ac34b4b,
    0x6f00f413,
    0x2904d02b,
    0x2a36d103,
    0x2236bf28,
    0x2908e03e,
    0x2a53d103,
    0x2253bf28,
    0x290ae038,
    0xf240d105,
    0x429a136f,
    0x461abf28,
    0x290be030,
    0xf5b2d105,
    0xbf287f0a,
    0x720af44f,
    0x290ee028,
    0xf240d105,
    0x429a23a7,
    0x461abf28,
    0x290fe020,
    0xf240d11e,
    0x429a33fd,
    0x461abf28,
    0x2902e018,
    0x2a12d103,
    0x2212bf28,
    0x2904e012,
    0x2a1bd103,
    0x221bbf28,
    0x290be00c,
    0x2ab7d103,
    0x22b7bf28,
    0x290fe006,
    0xf240d104,
    0x429a1353,
    0x461abf28,
    0x601a4b22,
    0x0088eb08,
    0x3010f837,
    0xf423b29b,
    0x4c217300,
    0xea438824,
    0xb29b2344,
    0x3010f827,
    0x3010f837,
    0xf023b29b,
    0xea430378,
    0xf82701c1,
    0x4b171010,
    0xf043681b,
    0xea430304,
    0xb29b03c2,
    0x3010f829,
    0x30fcf8d5,
    0x4a128c1b,
    0x3010f822,
    0x3010f83a,
    0x030ef3c3,
    0x3010f82a,
    0x30c2f895,
    0xf383fab3,
    0xf885095b,
    0xf89530c2,
    0x330130c3,
    0x30c3f885,
    0x68134a04,
    0x60133b01,
    0xb9f9f625,
    0xb855f625,
    0xb9faf625,
    0xc0006864,///test_mode_acl_rx_flag
    0xc000686c,///loopback_length
    0xc0006870,///loopback_llid
    0xd02115d4,
    0xc0006874,///rxseq_flag
    0xd0211152,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_testmode_2 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch29_ins_test_data_2),
    0x0002bca0,
    0xbf04f1da,
    0xc0006aac,
    (uint8_t *)bes2300_patch29_ins_test_data_2
};///test mode: ld_acl_tx_prog


#if 0
const uint32_t bes2300_patch30_ins_test_data_2[] =
{
    0xf014d002,
    0xd0010f24,
    0xbf62f625,
    0xb854f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_testmode_2 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_test_data_2),
    0x0002cb0c,
    0xb898f1da,
    0xc0006c40,
    (uint8_t *)bes2300_patch30_ins_test_data_2
};///ld_acl_rx:skip rx crcerr
#else
const uint32_t bes2300_patch30_ins_test_data_2[] =
{
    0xf014d006,
    0xd0050f24,
    0x0f03f1b8,
    0xbf00d002,
    0xbf5ef625,
    0xb850f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_testmode_2 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_test_data_2),
    0x0002cb0c,
    0xb898f1da,
    0xc0006c40,
    (uint8_t *)bes2300_patch30_ins_test_data_2
};///ld_acl_rx:skip rx crcerr

#endif

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_testmode_2 =
{
    31,
    BTDRV_PATCH_ACT,
    0,
    0x00000100,
    0x20004b15,
    0,
    NULL,
};////rwip_env.sleep_enable=false after hci reset


static const uint32_t ins_patch_2300_config_testmode[] =
{
    32,
    (uint32_t)&bes2300_ins_patch0_testmode_2,
    (uint32_t)&bes2300_ins_patch1_testmode_2,
    (uint32_t)&bes2300_ins_patch2_testmode_2,
    (uint32_t)&bes2300_ins_patch3_testmode_2,
    (uint32_t)&bes2300_ins_patch4_testmode_2,
    (uint32_t)&bes2300_ins_patch5_testmode_2,
    (uint32_t)&bes2300_ins_patch6_testmode_2,
    (uint32_t)&bes2300_ins_patch7_testmode_2,
    (uint32_t)&bes2300_ins_patch8_testmode_2,
    (uint32_t)&bes2300_ins_patch9_testmode_2,
    (uint32_t)&bes2300_ins_patch10_testmode_2,
    (uint32_t)&bes2300_ins_patch11_testmode_2,
    (uint32_t)&bes2300_ins_patch12_testmode_2,
    (uint32_t)&bes2300_ins_patch13_testmode_2,
    (uint32_t)&bes2300_ins_patch14_testmode_2,
    (uint32_t)&bes2300_ins_patch15_testmode_2,
    (uint32_t)&bes2300_ins_patch16_testmode_2,
    (uint32_t)&bes2300_ins_patch17_testmode_2,
    (uint32_t)&bes2300_ins_patch18_testmode_2,
    (uint32_t)&bes2300_ins_patch19_testmode_2,
    (uint32_t)&bes2300_ins_patch20_testmode_2,
    (uint32_t)&bes2300_ins_patch21_testmode_2,
    (uint32_t)&bes2300_ins_patch22_testmode_2,
    (uint32_t)&bes2300_ins_patch23_testmode_2,
    (uint32_t)&bes2300_ins_patch24_testmode_2,
    (uint32_t)&bes2300_ins_patch25_testmode_2,
    (uint32_t)&bes2300_ins_patch26_testmode_2,
    (uint32_t)&bes2300_ins_patch27_testmode_2,
    (uint32_t)&bes2300_ins_patch28_testmode_2,
    (uint32_t)&bes2300_ins_patch29_testmode_2,
    (uint32_t)&bes2300_ins_patch30_testmode_2,
    (uint32_t)&bes2300_ins_patch31_testmode_2,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch0_testmode_3 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x00024bcc,
    0xbf00bd10,
    0,
    NULL
};//ld_channel_assess


const BTDRV_PATCH_STRUCT bes2300_ins_patch1_testmode_3 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0003ed7c,
#ifdef BLE_POWER_LEVEL_0
    0xbf002300,
#else
    0xbf002302,
#endif
    0,
    NULL
};//ble power level



const uint32_t bes2300_patch2_ins_test_data_3[] =
{
    0x30fcf8d4,
    0xd0092b00,
    0x680b4905,
    0x3f00f413,
    0x680bd004,
    0x3300f423,
    0xbf00600b,
    0xb8c0f628,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch2_testmode_3 =
{
    2,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch2_ins_test_data_3),
    0x0002e998,
    0xbf32f1d7,
    0xc0006800,
    (uint8_t *)bes2300_patch2_ins_test_data_3
};



const uint32_t bes2300_patch3_ins_test_data_3[] =
{
    0x30fcf8d4,
    0xd00b2b00,
    0xf003789b,
    0x2b0503fd,
    0x4b04d106,
    0xf443681b,
    0x4a023300,
    0xbf006013,
    0xbbcbf626,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch3_testmode_3 =
{
    3,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch3_ins_test_data_3),
    0x0002cf08,
    0xbc92f1d9,
    0xc0006830,
    (uint8_t *)bes2300_patch3_ins_test_data_3
};


const uint32_t bes2300_patch4_ins_test_data_3[] =
{
    0xf0025bf2,
    0xbf00080f,
    0xba2bf623,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch4_testmode_3 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch4_ins_test_data_3),
    0x00029cbc,
    0xbdd0f1dc,
    0xc0006860,
    (uint8_t *)bes2300_patch4_ins_test_data_3
};


const uint32_t bes2300_patch5_ins_test_data_3[] =
{
    0xf0025bf2,
    0xbf00020f,
    0xba49f623,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch5_testmode_3 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch5_ins_test_data_3),
    0x00029d08,
    0xbdb2f1dc,
    0xc0006870,
    (uint8_t *)bes2300_patch5_ins_test_data_3
};



const uint32_t bes2300_patch6_ins_test_data_3[] =
{
    0xf0035bf3,
    0xbf00080f,
    0xba58f623,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch6_testmode_3 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch6_ins_test_data_3),
    0x00029d38,
    0xbda2f1dc,
    0xc0006880,
    (uint8_t *)bes2300_patch6_ins_test_data_3
};



const uint32_t bes2300_patch7_ins_test_data_3[] =
{
    0xf0035bf3,
    0xbf00010f,
    0xba78f623,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch7_testmode_3 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch7_ins_test_data_3),
    0x00029d88,
    0xbd82f1dc,
    0xc0006890,
    (uint8_t *)bes2300_patch7_ins_test_data_3
};


#ifdef __POWER_CONTROL_TYPE_1__

const uint32_t bes2300_patch8_ins_test_data_3[] =
{
    0xf0135243,
    0x2b00030f,
    0x2000bf14,
    0x47702001,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch8_testmode_3 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch8_ins_test_data_3),
    0x00003498,
    0xba50f203,
    0xc000693c,
    (uint8_t *)bes2300_patch8_ins_test_data_3
};//pwr controll
#else
const BTDRV_PATCH_STRUCT bes2300_ins_patch8_testmode_3 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x000034d0,
    0xe0072200,
    0,
    NULL
};

#endif


/////for test mode power control
const BTDRV_PATCH_STRUCT bes2300_ins_patch9_testmode_3 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x00029aa8,
    0xe0cfbf00,
    0,
    NULL
};


const uint32_t bes2300_patch10_ins_test_data_3[] =
{
    0xb2db1e6b,
    0xbf002b01,
    0x30fcf8db,
    0xf002789a,
    0x2a0502fd,
    0x4a2ad14a,
    0x11cbf892,
    0x4a2a0109,
    0xf3c25a8a,
    0x48292240,
    0x49216002,
    0x3201680a,
    0x4a20600a,
    0x8000f8c2,
    0xf3c29a05,
    0x491e02c9,
    0x491f600a,
    0x11cbf891,
    0x01c1ebc1,
    0xf830481e,
    0xf3c11011,
    0xbf002100,
    0xb119bf00,
    0x68014816,
    0x6001481f,
    0x600d4915,
    0x68004819,
    0x2100b150,
    0x60014817,
    0x68014817,
    0x6001480f,
    0x68014816,
    0x6001480e,
    0xf1008c18,
    0xf5004050,
    0x4b0c1004,
    0x31cbf893,
    0x03c3ebc3,
    0xf831490a,
    0xb2891013,
    0x4150f101,
    0x1104f501,
    0xf8f4f641,
    0xbf4ff626,
    0xc0006aa4,///test_mode_acl_rx_flag
    0xc0006aa8,///loopback_type
    0xc0006aac,///loopback_length
    0xc0006ab0,///loopback_llid
    0xc0005c90,
    0xd02115a0,
    0xd021159a,
    0xc0006ab4,///rxseq_flag
    0xc0006ab8,///unack_seqerr_flag
    0xc0006abc,///rxlength_flag
    0xc0006ac0,///rxllid_flag
    0xc0006ac4,///rxarqn_flag
    0xc0006ac8,///ok_rx_length_flag
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch10_testmode_3 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch10_ins_test_data_3),
    0x0002d904,
    0xb85cf1d9,
    0xc00069c0,
    (uint8_t *)bes2300_patch10_ins_test_data_3
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch11_testmode_3 =
{
    11,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL
};
/////end for test mode power control



const uint32_t bes2300_patch12_ins_test_data_3[] =
{
    0x20014b04,
    0x4b026018,
    0xbf00689b,
    0xb958f611,
    0xc0004168,
    0xc00068b8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch12_testmode_3 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch12_ins_test_data_3),
    0x00017b5c,
    0xbea0f1ee,
    0xc00068a0,
    (uint8_t *)bes2300_patch12_ins_test_data_3
};


const uint32_t bes2300_patch13_ins_test_data_3[] =
{
    0x20024b04,
    0x4b026018,
    0xbf00689b,
    0xbaecf612,
    0xc0004168,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch13_testmode_3 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch13_ins_test_data_3),
    0x00018ea4,
    0xbd0cf1ed,
    0xc00068c0,
    (uint8_t *)bes2300_patch13_ins_test_data_3
};


const uint32_t bes2300_patch14_ins_test_data_3[] =
{
    0x20014b04,
    0x4b026018,
    0xbf0068db,
    0xb964f611,
    0xc0004168,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch14_testmode_3 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch14_ins_test_data_3),
    0x00017bb4,
    0xbe94f1ee,
    0xc00068e0,
    (uint8_t *)bes2300_patch14_ins_test_data_3
};


const uint32_t bes2300_patch15_ins_test_data_3[] =
{
    0x20024b04,
    0x4b026018,
    0x462068db,
    0xbad6f612,
    0xc0004168,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch15_testmode_3 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch15_ins_test_data_3),
    0x00018eb8,
    0xbd22f1ed,
    0xc0006900,
    (uint8_t *)bes2300_patch15_ins_test_data_3
};


const uint32_t bes2300_patch16_ins_test_data_3[] =
{
    0xd91b429a,
    0x5a42b410,
    0xb2dc3301,
    0x427ff402,
    0x52424322,
    0x68104a0c,
    0xd00a2801,
    0xf8924a0b,
    0x42830030,
    0x2000bf14,
    0x4a072001,
    0x60112100,
    0x2000e001,
    0xf85d6010,
    0x47704b04,
    0x20004b02,
    0x20016018,
    0xbf004770,
    0xc00068b8,
    0xc0004168,


};

const BTDRV_PATCH_STRUCT bes2300_ins_patch16_testmode_3 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch16_ins_test_data_3),
    0x000034bc,
    0xba40f203,
    0xc0006940,
    (uint8_t *)bes2300_patch16_ins_test_data_3
};



const uint32_t bes2300_patch17_ins_test_data_3[] =
{
    0xf0135a43,
    0xd016030f,
    0x3b015a42,
    0xf402b2db,
    0x4313427f,
    0xf0135243,
    0x4a08030f,
    0x28016810,
    0x2b00d006,
    0x2000bf14,
    0x21002001,
    0xe0016011,
    0x60102000,
    0x20014770,
    0xbf004770,
    0xc00068b8,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch17_testmode_3 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch17_ins_test_data_3),
    0x00003484,
    0xbb44f203,
    0xc0006b10,
    (uint8_t *)bes2300_patch17_ins_test_data_3
};


const uint32_t bes2300_patch18_ins_test_data_3[] =
{
    0x60b39b01,
    0xfb00206e,
    0x4a0bf004,
    0xf0436813,
    0x60130302,
    0x4b092202,
    0xf5a3601a,
    0x3bb6436e,
    0x3003f830,
    0x030cf3c3,
    0x5300f443,
    0xf8204a04,
    0xbf003002,
    0xbeaaf625,
    0xD022000C,
    0xd0220018,
    0xd0211162,

};

const BTDRV_PATCH_STRUCT bes2300_ins_patch18_testmode_3 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch18_ins_test_data_3),
    0x0002c8e8,
    0xb93af1da,
    0xc0006b60,
    (uint8_t *)bes2300_patch18_ins_test_data_3
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch19_testmode_3 =
{
    19,
    BTDRV_PATCH_ACT,
    0,
    0x00038180,
    0xf7c92100,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch20_testmode_3 =
{
    20,
    BTDRV_PATCH_ACT,
    0,
    0x00003444,
    0xe00f382d,
    0,
    NULL
};///hw rssi read

const uint32_t bes2300_patch21_ins_test_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbca0f626
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch21_testmode_3 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch21_ins_test_data_3),
    0x0002d4f4,
    0xbb5cf1d9,
    0xc0006bb0,
    (uint8_t *)bes2300_patch21_ins_test_data_3
};//ld_acl_rx

const uint32_t bes2300_patch22_ins_test_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbecaf61e
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch22_testmode_3 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch22_ins_test_data_3),
    0x00025954,
    0xb932f1e1,
    0xc0006bbc,
    (uint8_t *)bes2300_patch22_ins_test_data_3
};//ld_inq_rx

const uint32_t bes2300_patch23_ins_test_data_3[] =
{
    0xf2400992,
    0x401333ff,
    0xbe08f63a
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch23_testmode_3 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch23_ins_test_data_3),
    0x000417dc,
    0xb9f4f1c5,
    0xc0006bc8,
    (uint8_t *)bes2300_patch23_ins_test_data_3
};//lld_pdu_rx_handler


const BTDRV_PATCH_STRUCT bes2300_ins_patch24_testmode_3 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x00024fb0,
    0xbf00e002,
    0,
    NULL,
};

const uint32_t bes2300_patch25_ins_test_data_3[] =
{
    0x3a063006,
    0xf847f641,
    0xb958f61e,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch25_testmode_3 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch25_ins_test_data_3),
    0x00024e8c,
    0xbea2f1e1,
    0xc0006bd4,
    (uint8_t *)bes2300_patch25_ins_test_data_3
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch26_testmode_3 =
{
    26,
#ifdef __CLK_GATE_DISABLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x00006d14,
    0xbf30bf00,
    0,
    NULL
};///disable clock gate

const uint32_t bes2300_patch27_ins_test_data_3[] =
{
    0x0f03f1b8,
    0xf1b8d002,
    0xd1070f08,
    0x30bbf89b,
    0xbf003301,
    0x2401bf00,
    0xe019bf00,
    0xf8934b0d,
    0xebc331cb,
    0x4a0c03c3,
    0x3013f832,
    0x2300f3c3,
    0x60134a0f,
    0x4b09b953,
    0x4b0c681a,
    0x4b08601a,
    0x4b08681a,
    0x2201601a,
    0x601a4b07,
    0xbdacf626,
    0xbe6df626,
    0xc0005c90,
    0xd021159a,
    0xc0006ab0,///loopback_llid
    0xc0006aac,///loopback_length
    0xc0006abc,///rxlength_flag
    0xc0006ab8,///unack_seqerr_flag
    0xc0006ac0,///rxllid_flag
    0xc0006ac4,////rxarqn_flag
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch27_testmode_3 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch27_ins_test_data_3),
    0x0002d780,
    0xba2ef1d9,
    0xc0006be0,
    (uint8_t *)bes2300_patch27_ins_test_data_3
};///ld_acl_rx:skip rxseq_err


const uint32_t bes2300_patch28_ins_test_data_3[] =
{
    0xbf00789a,
    0xb2d23a05,
    0xd9012a03,
    0xb8a2f000,
    0xd0022a00,
    0xf0402a02,
    0xbf00809f,
    0x681b4b4f,
    0xf0002b00,
    0xf8d58099,
    0x881a30fc,
    0xf00179d9,
    0xb90a010f,
    0x881a4b4a,
    0x236e9804,
    0xf000fb03,
    0x5ac34b4b,
    0x6f00f413,
    0x2904d02b,
    0x2a36d103,
    0x2236bf28,
    0x2908e03e,
    0x2a53d103,
    0x2253bf28,
    0x290ae038,
    0xf240d105,
    0x429a136f,
    0x461abf28,
    0x290be030,
    0xf5b2d105,
    0xbf287f0a,
    0x720af44f,
    0x290ee028,
    0xf240d105,
    0x429a23a7,
    0x461abf28,
    0x290fe020,
    0xf240d11e,
    0x429a33fd,
    0x461abf28,
    0x2902e018,
    0x2a12d103,
    0x2212bf28,
    0x2904e012,
    0x2a1bd103,
    0x221bbf28,
    0x290be00c,
    0x2ab7d103,
    0x22b7bf28,
    0x290fe006,
    0xf240d104,
    0x429a1353,
    0x461abf28,
    0x601a4b22,
    0x0484eb04,
    0x3014f837,
    0xf423b29b,
    0x48217300,
    0xea438800,
    0xb29b2340,
    0x3014f827,
    0x3014f837,
    0xf023b29b,
    0xea430378,
    0xf82701c1,
    0x4b171014,
    0xf043681b,
    0xea430304,
    0xb29b03c2,
    0x3014f82a,
    0x30fcf8d5,
    0x4a128c1b,
    0x3014f822,
    0x3014f836,
    0x030ef3c3,
    0x3014f826,
    0x30c2f895,
    0xf383fab3,
    0xf885095b,
    0xf89530c2,
    0x330130c3,
    0x30c3f885,
    0x68134a04,
    0x60133b01,
    0xb81af626,
    0xbeaaf625,
    0xb81bf626,
    0xc0006aa4,///test_mode_acl_rx_flag
    0xc0006aac,///loopback_length
    0xc0006ab0,///loopback_llid
    0xd02115d4,
    0xc0006ab4,///rxseq_flag
    0xd0211152,
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch28_testmode_3 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch28_ins_test_data_3),
    0x0002cb0c,
    0xb8aef1da,
    0xc0006c6c,
    (uint8_t *)bes2300_patch28_ins_test_data_3
};///test mode: ld_acl_tx_prog


#if 0
const uint32_t bes2300_patch29_ins_test_data_3[] =
{
    0xf014d002,
    0xd0010f24,
    0xbcc2f626,
    0xbd76f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_testmode_3 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch29_ins_test_data_3),
    0x0002d78c,
    0xbb38f1d9,
    0xc0006e00,
    (uint8_t*)bes2300_patch29_ins_test_data_3
};///test mode: skip crc err in ld_acl_rx
#else

const uint32_t bes2300_patch29_ins_test_data_3[] =
{
    0xf014d006,
    0xd0050f24,
    0x0f03f1b8,
    0xbf00d002,
    0xbc9ef626,
    0xbd52f626,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch29_testmode_3 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch29_ins_test_data_3),
    0x0002d78c,
    0xbb58f1d9,
    0xc0006e40,
    (uint8_t*)bes2300_patch29_ins_test_data_3
};///test mode: skip crc err in ld_acl_rx


#endif

const uint32_t bes2300_patch30_ins_test_data_3[] =
{
    0xfbb24a02,
    0xf5f9f3f3,
    0xbf00bb3a,
    0x9af8da00,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch30_testmode_3 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch30_ins_test_data_3),
    0x00000650,
    0xbcc2f206,
    0xc0006fd8,
    (uint8_t *)bes2300_patch30_ins_test_data_3
};//lpo timer

const uint32_t bes2300_patch31_ins_test_data_3[] =
{
    0x02004638,
    0x0001f040,
    0xfcf8f60c,
    0xbf60f61b,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch31_testmode_3 =
{
    31,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_patch31_ins_test_data_3),
    0x00022ebc,
    0xb8acf1e4,
    0xc0007018,
    (uint8_t *)bes2300_patch31_ins_test_data_3
};//lc_send_lmp

const BTDRV_PATCH_STRUCT bes2300_ins_patch32_testmode_3 =
{
    32,
    BTDRV_PATCH_INACT,
    0,
    0x0002e430,
    0xe0022000,
    0,
    NULL
};///ld_acl_stop


const BTDRV_PATCH_STRUCT bes2300_ins_patch33_testmode_3 =
{
    33,
    BTDRV_PATCH_INACT,
    0,
    0x0002d9f4,
    0xe053bf00,
    0,
    NULL
};//disable power control



const BTDRV_PATCH_STRUCT bes2300_ins_patch34_testmode_3 =
{
    34,
    BTDRV_PATCH_ACT,
    0,
    0x00006bcc,
    0xbf00e016,
    0,
    NULL
};///disable assert warning



const uint32_t bes2300_patch35_ins_test_data_3[] =
{
    0x880b4904,
    0xf423b29b,
    0x021263e0,
    0x800b4313,
    0xbbc5f639,
    0xd02101d0,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch35_testmode_3 =
{
    35,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch35_ins_test_data_3),
    0x000405b4,
    0xbc34f1c6,
    0xc0006e20,
    (uint8_t *)bes2300_patch35_ins_test_data_3
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch36_testmode_3 =
{
    36,
    BTDRV_PATCH_ACT,
    0,
    0x000406f4,
    0x0106f248,
    0,
    NULL,
};//ble win cntl

const BTDRV_PATCH_STRUCT bes2300_ins_patch37_testmode_3 =
{
    37,
    BTDRV_PATCH_ACT,
    0,
    0x00000100,
    0x20004b15,
    0,
    NULL,
};////rwip_env.sleep_enable=false after hci reset


const uint32_t bes2300_patch38_ins_test_data_3[] =
{
    0x4681b083,
    0x4b022200,
    0x2020f843,
    0xbf94f621,
    0xc0007044,
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch38_testmode_3 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch38_ins_test_data_3),
    0x00028f64,
    0xb864f1de,
    0xc0007030,
    (uint8_t *)bes2300_patch38_ins_test_data_3
};//swagc patch


const uint32_t bes2300_patch39_ins_test_data_3[] =
{
    0x6020f853,
    0xf8524a0d,
    0x33013020,
    0x3020f842,
    0x68124a0b,
    0xd90f4293,
    0x4a082300,
    0x3020f842,
    0xb082b402,
    0x92004a08,
    0xf06f6a14,
    0x46290059,
    0x47a02201,
    0xbc02b002,
    0xbeb2f621,
    0xc0007044,
    0xc0007094,
    0x00000010,
    0xc0004168,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch39_testmode_3 =
{
    39,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch39_ins_test_data_3),
    0x00028dec,
    0xb930f1de,
    0xc0007050,
    (uint8_t *)bes2300_patch39_ins_test_data_3
};

const uint32_t bes2300_patch40_ins_test_data_3[] =
{
    0x2255f24c,
    0x601a4b03,
    0x68134a01,
    0xb9a2f632,
    0xd0220200,
    0xd02101d0,
};

const BTDRV_PATCH_STRUCT bes2300_ins_patch40_testmode_3 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch40_ins_test_data_3),
    0x000391b0,
    0xbe56f1cd,
    0xc0006e60,
    (uint8_t *)bes2300_patch40_ins_test_data_3
};///ble rx test mode: set rxgain idx = 0

const uint32_t bes2300_patch41_ins_test_data_3[] =
{
    0xf8802300,
    0xf8803020,
    0xf8943021,
    0x2b0130b3,
    0x2300d102,
    0xbfd2f62a,
    0xbfdef62a,
};/* 6e80-6e98 */

const BTDRV_PATCH_STRUCT bes2300_ins_patch41_testmode_3 =
{
    41,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch41_ins_test_data_3),
    0x00031e38,
    0xb822f1d5,
    0xc0006e80,
    (uint8_t *)bes2300_patch41_ins_test_data_3
};//nosig test pattern problem

static const uint32_t ins_patch_2300_config_testmode_3[] =
{
    42,
    (uint32_t)&bes2300_ins_patch0_testmode_3,
    (uint32_t)&bes2300_ins_patch1_testmode_3,
    (uint32_t)&bes2300_ins_patch2_testmode_3,
    (uint32_t)&bes2300_ins_patch3_testmode_3,
    (uint32_t)&bes2300_ins_patch4_testmode_3,
    (uint32_t)&bes2300_ins_patch5_testmode_3,
    (uint32_t)&bes2300_ins_patch6_testmode_3,
    (uint32_t)&bes2300_ins_patch7_testmode_3,
    (uint32_t)&bes2300_ins_patch8_testmode_3,
    (uint32_t)&bes2300_ins_patch9_testmode_3,
    (uint32_t)&bes2300_ins_patch10_testmode_3,
    (uint32_t)&bes2300_ins_patch11_testmode_3,
    (uint32_t)&bes2300_ins_patch12_testmode_3,
    (uint32_t)&bes2300_ins_patch13_testmode_3,
    (uint32_t)&bes2300_ins_patch14_testmode_3,
    (uint32_t)&bes2300_ins_patch15_testmode_3,
    (uint32_t)&bes2300_ins_patch16_testmode_3,
    (uint32_t)&bes2300_ins_patch17_testmode_3,
    (uint32_t)&bes2300_ins_patch18_testmode_3,
    (uint32_t)&bes2300_ins_patch19_testmode_3,
    (uint32_t)&bes2300_ins_patch20_testmode_3,
    (uint32_t)&bes2300_ins_patch21_testmode_3,
    (uint32_t)&bes2300_ins_patch22_testmode_3,
    (uint32_t)&bes2300_ins_patch23_testmode_3,
    (uint32_t)&bes2300_ins_patch24_testmode_3,
    (uint32_t)&bes2300_ins_patch25_testmode_3,
    (uint32_t)&bes2300_ins_patch26_testmode_3,
    (uint32_t)&bes2300_ins_patch27_testmode_3,
    (uint32_t)&bes2300_ins_patch28_testmode_3,
    (uint32_t)&bes2300_ins_patch29_testmode_3,
    (uint32_t)&bes2300_ins_patch30_testmode_3,
    (uint32_t)&bes2300_ins_patch31_testmode_3,
    (uint32_t)&bes2300_ins_patch32_testmode_3,
    (uint32_t)&bes2300_ins_patch33_testmode_3,
    (uint32_t)&bes2300_ins_patch34_testmode_3,
    (uint32_t)&bes2300_ins_patch35_testmode_3,
    (uint32_t)&bes2300_ins_patch36_testmode_3,
    (uint32_t)&bes2300_ins_patch37_testmode_3,
    (uint32_t)&bes2300_ins_patch38_testmode_3,
    (uint32_t)&bes2300_ins_patch39_testmode_3,
    (uint32_t)&bes2300_ins_patch40_testmode_3,
    (uint32_t)&bes2300_ins_patch41_testmode_3,
};


void btdrv_ins_patch_test_init(void)
{

    const BTDRV_PATCH_STRUCT *ins_patch_p;

    btdrv_patch_en(0);

    for(uint8_t i=0; i<56; i++)
    {
        btdrv_ins_patch_disable(i);
    }

    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        for(uint8_t i=0; i<ins_patch_2300_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode[i+1]);
        }
#ifdef BLE_POWER_LEVEL_0
        *(uint32_t *)0xd02101d0 = 0xf855;
#endif


    }
    else if(hal_get_chip_metal_id()>=HAL_CHIP_METAL_ID_5)
    {
        for(uint8_t i=0; i<ins_patch_2300_config_testmode_3[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode_3[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode_3[i+1]);
        }
    }
    btdrv_patch_en(1);
}

void btdrv_dynamic_patch_moble_disconnect_reason_hacker(uint16_t hciHandle)
{
    uint32_t lc_ptr=0;
    uint32_t disconnect_reason_address = 0;
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        lc_ptr = *(uint32_t *)(0xc0005b48+(hciHandle-0x80)*4);
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        lc_ptr = *(uint32_t *)(0xc0005bcc+(hciHandle-0x80)*4);
    }
    //BT_DRV_TRACE(2,"lc_ptr %x, disconnect_reason_addr %x",lc_ptr,lc_ptr+66);
    if(lc_ptr == 0)
    {
        return;
    }
    else
    {
        disconnect_reason_address = (uint32_t)(lc_ptr+66);
        BT_DRV_TRACE(3,"%s:hdl=%x reason=%d",__func__,hciHandle,*(uint8_t *)(disconnect_reason_address));
        *(uint8_t *)(disconnect_reason_address) = 0x0;
        BT_DRV_TRACE(1,"After op,reason=%d",*(uint8_t *)(disconnect_reason_address));
        return;
    }
}

void btdrv_preferre_rate_send(uint16_t conhdl,uint8_t rate)
{
#ifdef __SEND_PREFERRE_RATE__
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        ASSERT(*(uint32_t *)0xc0006e80 ==0xc0006e84,"ERROR PATCH FOR PREFERRE RATE!");
        ASSERT(conhdl>=0x80,"ERROR CONNECT HANDLE");
        ASSERT(conhdl<=0x83,"ERROR CONNECT HANDLE");
        *(uint32_t *)0xc0006e8c = rate;
        *(uint32_t *)0xc0006e84 = conhdl-0x80;
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        ASSERT(*(uint32_t *)0xc00068c0 ==0xc00068c4,"ERROR PATCH FOR PREFERRE RATE!");
        ASSERT(conhdl>=0x80,"ERROR CONNECT HANDLE");
        ASSERT(conhdl<=0x83,"ERROR CONNECT HANDLE");
        *(uint32_t *)0xc00068cc = rate;
        *(uint32_t *)0xc00068c4 = conhdl-0x80;
    }

#endif
}

void btdrv_preferre_rate_clear(void)
{
#ifdef __SEND_PREFERRE_RATE__
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        ASSERT(*(uint32_t *)0xc0006e80 ==0xc0006e84,"ERROR PATCH FOR PREFERRE RATE!");
        *(uint32_t *)0xc0006e84 = 0xff;
    }
    else if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_5)
    {
        ASSERT(*(uint32_t *)0xc00068c0 ==0xc00068c4,"ERROR PATCH FOR PREFERRE RATE!");
        *(uint32_t *)0xc00068c0 = 0xff;
    }
#endif
}
void btdrv_dynamic_patch_sco_status_clear(void)
{
    return;
}


void btdrv_seq_bak_mode(uint8_t mode,uint8_t linkid)
{
#ifdef __SW_SEQ_FILTER__
    if(hal_get_chip_metal_id() > HAL_CHIP_METAL_ID_1 && hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_4)
    {
        uint32_t seq_bak_ptr = 0xc0006c8c;
        BT_DRV_TRACE(2,"btdrv_seq_bak_mode mode=%x,linkid = %x",mode,linkid);
        uint32_t val = *(uint32_t *)seq_bak_ptr;
        val &= ~(0xff<<(linkid*8));
        if(mode ==1)//en
        {
            val |=(0x2<<linkid*8);
        }
        else if(mode ==0)
        {
            val =0xffffffff;
        }
        *(uint32_t *)seq_bak_ptr = val;

        BT_DRV_TRACE(1,"val=%x",val);
    }
#endif
}



void bt_drv_patch_cvsd_check_check(void)
{
    if(*(uint32_t *)0xc000430c !=0xf8534b39  || *(uint32_t *)0xc0004404 !=0xd0220150)
    {
        ASSERT(0,"patch is rewrite on the address 0xc000430c");

    }
}


////////////////////////////////bt 5.0 /////////////////////////////////

const BTDRV_PATCH_STRUCT bes2300_50_ins_patch0 =
{
    0,
    BTDRV_PATCH_INACT,
    0,
    0x00056138,
    0xe0042300,
    0,
    NULL
};//disable reslove to



const uint32_t bes2300_50_patch1_ins_data[] =
{
    0x0f02f013,
    0xf008bf12,
    0x79330303,
    0x0303ea08,
    0x300bf889,
    0xf01378b3,
    0xbf120f01,
    0x0303f008,
    0xea0878f3,
    0xf8890303,
    0xbf00300a,
    0xbf6ff659,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch1 =
{
    1,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_50_patch1_ins_data),
    0x0005f6e8,
    0xb88af1a6,
    0xc0005800,
    (uint8_t *)bes2300_50_patch1_ins_data
};


const uint32_t bes2300_50_patch2_ins_data[] =
{
    0xf01378cb,
    0xd1080f04,
    0xf013790b,
    0xd1040f04,
    0xf003788b,
    0xf6590202,
    0x2211bf16,
    0xbf99f659,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch2 =
{
    2,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_50_patch2_ins_data),
    0x0005f680,
    0xb8def1a6,
    0xc0005840,
    (uint8_t *)bes2300_50_patch2_ins_data
};///SET PHY CMD



const uint32_t bes2300_50_patch3_ins_data[] =
{
    0x4b034902,
    0x4b036019,
    0xb8c5f64b,
    0x00800190,
    0xc0004af4,
    0xc00002d4,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch3_ins_data),
    0x00050a70,
    0xbf36f1b4,
    0xc00058e0,
    (uint8_t *)bes2300_50_patch3_ins_data
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x000676c4,
    0x0301f043,
    0,
    NULL
};



static const uint32_t best2300_50_ins_patch_config[] =
{
    5,
    (uint32_t)&bes2300_50_ins_patch0,
    (uint32_t)&bes2300_50_ins_patch1,
    (uint32_t)&bes2300_50_ins_patch2,
    (uint32_t)&bes2300_50_ins_patch3,
    (uint32_t)&bes2300_50_ins_patch4,

};


const uint32_t bes2300_50_patch0_ins_data_2[] =
{
    0x4b034902,
    0x4b036019,
    0xbe25f64b,
    0x00800190,
    0xc0004bc0,
    0xc0000390,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch0_2 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch0_ins_data_2),
    0x00050f30,
    0xb9d6f1b4,
    0xc00052e0,
    (uint8_t *)bes2300_50_patch0_ins_data_2
};//sleep patch


const uint32_t bes2300_50_patch1_ins_data_2[] =
{
    0xf8934b1e,
    0x2b0030dc,
    0xb470d132,
    0x4b1cb082,
    0x4d1c8818,
    0x4e1c882b,
    0x2303f3c3,
    0x69f2b2c0,
    0x1000ea43,
    0x882c4790,
    0x9600b2a4,
    0x21006a36,
    0x2301460a,
    0x882b47b0,
    0x2403f3c4,
    0x2303f3c3,
    0xd013429c,
    0x68194b11,
    0x4b112200,
    0x3bd0601a,
    0xf422681a,
    0x601a4280,
    0xf042681a,
    0x601a6280,
    0x4b0c2201,
    0x2302601a,
    0x302af881,
    0xbc70b002,
    0xbf00bf00,
    0xff26f65a,
    0xbc61f662,
    0xc00050d0,
    0xd061074c,
    0xd0610160,
    0xc00048b8,
    0xc00008c4,
    0xd06200d0,
    0xc00053a4,
    0x00000000,






};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch1_2 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch1_ins_data_2),
    0x00067c44,
    0xbb62f19d,
    0xc000530c,
    (uint8_t *)bes2300_50_patch1_ins_data_2
};

const uint32_t bes2300_50_patch2_ins_data_2[] =
{

    0xf6684620,
    0x4b09fd2d,
    0x2b01681b,
    0xf894d10c,
    0x2b02302a,
    0xf662d108,
    0x4805fbc5,
    0xfc58f662,
    0x4b022200,
    0xbd38601a,
    0xbbd1f662,
    0xc00053a4,
    0xc00053e4,
    0x00000000,
    0x00000000,


};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch2_2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch2_ins_data_2),
    0x00067b78,
    0xbc1af19d,
    0xc00053b0,
    (uint8_t *)bes2300_50_patch2_ins_data_2
};



const uint32_t bes2300_50_patch3_ins_data_2[] =
{

    0x4b046013,
    0x601a6822,
    0x711a7922,
    0xbf002000,
    0xbc7cf662,
    0xc00053e4,


};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch3_2 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch3_ins_data_2),
    0x00067d08,
    0xbb7af19d,
    0xc0005400,
    (uint8_t *)bes2300_50_patch3_ins_data_2
};


static const uint32_t best2300_50_ins_patch_config_2[] =
{
    4,
    (uint32_t)&bes2300_50_ins_patch0_2,
    (uint32_t)&bes2300_50_ins_patch1_2,
    (uint32_t)&bes2300_50_ins_patch2_2,
    (uint32_t)&bes2300_50_ins_patch3_2,
};

void btdrv_ins_patch_init_50(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0; i<best2300_50_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() < HAL_CHIP_METAL_ID_5)
    {
        for(uint8_t i=0; i<best2300_50_ins_patch_config_2[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config_2[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config_2[i+1]);
        }
    }
}

const uint32_t dpatch0_data_2300_50[] =
{
    0x29B00033,
    0x0020B000,
    0xB00015B0,
    0x05B0000B,
    0x00F9B000,
    0x7F7F7FB0,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x0000007F

};

const BTDRV_PATCH_STRUCT data_patch0_2300_50 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(dpatch0_data_2300_50),
    0x00051930,
    0xc0005870,
    0xc0005870,
    (uint8_t *)&dpatch0_data_2300_50
};



const uint32_t dpatch1_data_2300_50[] =
{
    0xC1C1BCBC,
    0xDcDcd3d3,
    0xE7E7e2e2,
    0x7f7fEeEe,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x7F7F7F7F,
    0x00007F7F,

};

const BTDRV_PATCH_STRUCT data_patch1_2300_50 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(dpatch1_data_2300_50),
    0x00051aa8,
    0xc00058b0,
    0xc00058b0,
    (uint8_t *)&dpatch1_data_2300_50
};


uint32_t data_patch_config_2300_50[] =
{
    2,
    (uint32_t)&data_patch0_2300_50,
    (uint32_t)&data_patch1_2300_50,

};





void btdrv_data_patch_init_50(void)
{
    const BTDRV_PATCH_STRUCT *data_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {

        for(uint8_t i=0; i<data_patch_config_2300_50[0]; i++)
        {
            data_patch_p = (BTDRV_PATCH_STRUCT *)data_patch_config_2300_50[i+1];
            if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
                btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)data_patch_config_2300_50[i+1]);
        }
    }

}


