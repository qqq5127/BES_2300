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
#include "bt_drv_1400_internal.h"
#include "bt_drv_internal.h"

///enable m4 patch func
#define BTDRV_PATCH_EN_REG                  0xe0002000

//set m4 patch remap adress
#define BTDRV_PATCH_REMAP_REG               0xe0002004

//instruction patch compare src address
#define BTDRV_PATCH_INS_COMP_ADDR_START     0xe0002008

#define BTDRV_PATCH_INS_REMAP_ADDR_START    0xc0000100

//data patch compare src address
#define BTDRV_PATCH_DATA_COMP_ADDR_START    0xe00020e8

#define BTDRV_PATCH_DATA_REMAP_ADDR_START   0xc00001e0


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

#ifdef IBRT
#define BT_CONTROLLER_DEBUG  1

const uint32_t bes1400_patch0_ibrt_ins_data_t6[] =
{
    0xd0082e02,
    0xd1092e10,
    0x681b4b05,
    0xbf142b00,
    0x20022004,
    0x2002e000,
    0xf860f635,
    0xb904f635,
    0xc0007384,/*6c04*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch0_ibrt_ins_data_t6),
    0x0003bdf8,
    0xbef4f1ca,
    0xc0006be4,
    (uint8_t *)bes1400_patch0_ibrt_ins_data_t6
};//ibrt sco auto accept (slave)

const uint32_t bes1400_patch1_ibrt_ins_data_t6[] =
{
    0x681b4b07,
    0xf632b133,
    0x2800ffdb,
    0x2001bf0c,
    0xe0002004,
    0xf6352001,
    0xbf00f84d,
    0xb80bf60a,
    0xc0007384,/*6c2c*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch1_ibrt_ins_data_t6),
    0x00010c3c,
    0xbfe6f1f5,
    0xc0006c0c,
    (uint8_t *)bes1400_patch1_ibrt_ins_data_t6
};//ibrt sco auto accept (master)

const uint32_t bes1400_patch2_ibrt_ins_data_t6[] =
{
    0x4b04b508,
    0x2b01681b,
    0x2001d102,
    0xfb00f5fa,
    0xbf00bd08,
    0xc0007668,
    0xfd56f60d,
    0xfff0f7ff,
    0xbe7af60f,/*6c58*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch2_ibrt_ins_data_t6),
    0x0001694c,
    0xb980f1f0,
    0xc0006c38,
    (uint8_t *)bes1400_patch2_ibrt_ins_data_t6
};// tws unsniff update ke state

const uint32_t bes1400_patch3_ibrt_ins_data_t6[] =
{
    0xbf004628,
    0xfdc4f60d,
    0x21144628,
    0xba4af60f,/*6c94*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch3_ibrt_ins_data_t6),
    0x00016128,
    0xbdaef1f0,
    0xc0006c88,
    (uint8_t *)bes1400_patch3_ibrt_ins_data_t6
};//start lmp to before sending LMP_HostConnectionReq

const uint32_t bes1400_patch4_ibrt_ins_data_t6[] =
{
    0xf633b940,/*69b8*/
    0x2801f86d,
    0xf633d104,
    0x2800f903,
    0xbf00d102,
    0xbd84f62b,
    0xbe02f62b,/*69d0*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch4_ibrt_ins_data_t6),
    0x000324d4,
    0xba70f1d4,
    0xc00069b8,
    (uint8_t *)bes1400_patch4_ibrt_ins_data_t6
};//filter afh map

const uint32_t bes1400_patch5_ibrt_ins_data_t6[] =
{
    0x79ab4688,/*69d4*/
    0x6383ea6f,
    0x4578f023,
    0xf8d39b09,
    0x9906308c,
    0x402b440b,
    0xbf004641,
    0xb8bdf62d,/*69f0*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch5_ibrt_ins_data_t6),
    0x00033b60,
    0xbf38f1d2,
    0xc00069d4,
    (uint8_t *)bes1400_patch5_ibrt_ins_data_t6
};//sco param init

const uint32_t bes1400_patch6_ibrt_ins_data_t6[] =
{
    0x4a033304,/*69f8*/
    0xbf008153,
    0x12c4f640,
    0xbeb9f632,
    0xc00062c0,/*6a08*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch6_ibrt_ins_data_t6),
    0x00039770,
    0xb942f1cd,
    0xc00069f8,
    (uint8_t *)bes1400_patch6_ibrt_ins_data_t6
};//pscan duration

const uint32_t bes1400_patch7_ibrt_ins_data_t6[] =
{
    0x71ab2300,/*6a10*/
    0x0000f8d9,
    0x380cb110,
    0xf8f6f5fa,
    0xbf002300,
    0xbf3ef608,/*6a24*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch7_ibrt_ins_data_t6),
    0x0000f8a0,
    0xb8b6f1f7,
    0xc0006a10,
    (uint8_t *)bes1400_patch7_ibrt_ins_data_t6
};//lm_page_scan_end_ind free msg

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00020640,
    0xe00dbf00,
    0,
    NULL,
};//detach directly

const uint32_t bes1400_patch9_ibrt_ins_data_t6[] =
{
    0xf62a4628,/*6a2c*/
    0x4620fef7,
    0xff76f60f,
    0xf8534b04,
    0x46203025,
    0x135ef893,
    0xfc68f5fa,
    0xbd702000,
    0xc0005de4,/*6a4c*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch9_ibrt_ins_data_t6),
    0x0002347c,
    0xbad6f1e3,
    0xc0006a2c,
    (uint8_t *)bes1400_patch9_ibrt_ins_data_t6
};

const uint32_t bes1400_patch10_ibrt_ins_data_t6[] =
{
    0x0818f104,
    0xbb9ff634,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch10_ibrt_ins_data_t6),
    0x0003b184,
    0xbc66f1cb,
    0xc0006a54,
    (uint8_t *)bes1400_patch10_ibrt_ins_data_t6
};//set afh

const uint32_t bes1400_patch11_ibrt_ins_data_t6[] =
{
    0xf8934b09,/*6a68*/
    0xf0133084,
    0xd0050f06,
    0xf6374620,
    0x4601f805,
    0xe0052400,
    0xf8934b03,
    0xbf003084,
    0xb941f636,
    0xb987f636,
    0xc0006568,/*6a90*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch11_ibrt_ins_data_t6),
    0x0003cd08,
    0xbeaef1c9,
    0xc0006a68,
    (uint8_t *)bes1400_patch11_ibrt_ins_data_t6
};// set ble adv data

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x0003cbf0,
    0xbf00e004,
    0,
    NULL
};// set ble scan response data

const uint32_t bes1400_patch13_ibrt_ins_data_t6[] =
{
    /*6a98*/ 0x99049803,
    /*6a9c*/ 0x4b12b470,
    /*6aa0*/ 0x010e5c5a,
    /*6aa4*/ 0x19734d11,
    /*6aa8*/ 0x78244c11,
    /*6aac*/ 0xd20342a2,
    /*6ab0*/ 0x32015498,
    /*6ab4*/ 0xe001b2d2,
    /*6ab8*/ 0x22015570,
    /*6abc*/ 0x5442480a,
    /*6ac0*/ 0x461ab14c,
    /*6ac4*/ 0x23001919,
    /*6ac8*/ 0x0b01f912,
    /*6acc*/ 0xbf004403,
    /*6ad0*/ 0xd1f9428a,
    /*6ad4*/ 0x2300e000,
    /*6ad8*/ 0xf0f4fb93,
    /*6adc*/ 0xbc70b240,
    /*6ae0*/ 0x22019904,
    /*6ae4*/ 0xba00f626, //jump a0206ae4 -> a002cee8
    /*6ae8*/ 0xc0006af8, //addr of rssi_idx_for_agc
    /*6aec*/ 0xc0006afc, //addr of rssi_set_for_agc
    /*6af0*/ 0xc0006af4, //addr of N
    /*6af4*/ 0x00000005, //N : 1 ~ 16
    /*6af8*/ 0x00000000, //rssi_idx_for_agc[3]
    /*6afc*/ 0xbabababa, //rssi_set_for_agc[3*16]
    /*6b00*/ 0xbabababa,
    /*6b04*/ 0xbabababa,
    /*6b08*/ 0xbabababa,
    /*6b0c*/ 0xbabababa,
    /*6b10*/ 0xbabababa,
    /*6b14*/ 0xbabababa,
    /*6b18*/ 0xbabababa,
    /*6b1c*/ 0xbabababa,
    /*6b20*/ 0xbabababa,
    /*6b24*/ 0xbabababa,
    /*6b28*/ 0xbabababa,
    /*6b2c*/ 0xbabababa,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch13 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch13_ibrt_ins_data_t6),
    0x0002cee4,
    0xbdd8f1d9, //jump a002cee4 -> a0206a98
    0xc0006a98,
    (uint8_t *)bes1400_patch13_ibrt_ins_data_t6
};//ld_acl_rx() average rssi

const uint32_t bes1400_patch14_ibrt_ins_data_t6[] =
{
    0x305af893,
    0xd1032b00,
    0x12f4f240,
    0xbf00e002,
    0x32b8f640,
    0xbe30f60d,/*6bdc*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch14 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch14_ibrt_ins_data_t6),
    0x0001483c,
    0xb9c4f1f2,
    0xc0006bc8,
    (uint8_t *)bes1400_patch14_ibrt_ins_data_t6
};//lmp to 5s before connection cmp sent

const uint32_t bes1400_patch15_ibrt_ins_data_t6[] =
{
    0x2028f843,/*6b3c*/
    0xf8434b08,
    0x4b052028,
    0x4b0e701a,
    0x2201701a,
    0xf8434b08,
    0xbf002028,
    0xba32f624,
    0xc0006b60,
    0x00000000,
    0xc0006b68,
    0x00000000,
    0x00000000,
    0x00000000,
    0xc0006b78,
    0x00000001,
    0x00000001,
    0x00000001,
    0xc0006b88,
    0x00000000,/*6b88*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch15 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch15_ibrt_ins_data_t6),
    0x0002afbc,
    0xbdbef1db,
    0xc0006b3c,
    (uint8_t *)bes1400_patch15_ibrt_ins_data_t6
};//sync clear count

const uint32_t bes1400_patch16_ibrt_ins_data_t6[] =
{
    0x0046f894,/*6b90*/
    0xfe94f625,
    0xd8012802,
    0xba3ef629,
    0xba57f629,/*6ba0*/
};
const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch16 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch16_ibrt_ins_data_t6),
    0x00030014,
    0xbdbcf1d6,
    0xc0006b90,
    (uint8_t *)bes1400_patch16_ibrt_ins_data_t6
};//ld_sco_evt_start_cbk

const uint32_t bes1400_patch17_ibrt_ins_data_t6[] =
{
    0xf6062002,/*6ba4*/
    0x2802f937,
    0xf898d108,
    0xf62500b2,
    0x2802fe85,
    0xbf00d802,
    0xbc93f629,
    0xbc98f629,/*6bc0*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch17_ibrt_ins_data_t6),
    0x000304d4,
    0xbb66f1d6,
    0xc0006ba4,
    (uint8_t *)bes1400_patch17_ibrt_ins_data_t6
};//ld_calculate_event_timestamp

#ifdef BT_CONTROLLER_DEBUG
const uint32_t bes1400_patch18_ibrt_ins_data_t6[] =
{
    0x68124a0e,
    0x2084f8c3,
    0x68124a0d,
    0x2088f8c3,
    0x68124a0c,
    0x208cf8c3,
    0x68124a0b,
    0x2090f8c3,
    0x68124a0a,
    0x2094f8c3,
    0x68124a09,
    0x2098f8c3,
    0x68124a08,
    0x209cf8c3,
    0xbf004770,
    0xe000ed04,
    0xe000ed0c,
    0xe000ed10,
    0xe000ed14,
    0xe000ed24,
    0xe000ed3c,
    0xe000ed34,/*6d10*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch18 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch18_ibrt_ins_data_t6),
    0x000077fc,
    0xba5ef1ff,
    0xc0006cbc,
    (uint8_t *)bes1400_patch18_ibrt_ins_data_t6
};//correct crash dump
#else

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch18 =
{
    18,
    BTDRV_PATCH_INACT,
    0,
    0x00000000,
    0x00000000,
    0,
    NULL
};//unused
#endif

const uint32_t bes1400_patch19_ibrt_ins_data_t6[] =
{
    0x4603b508,
    0x8210f3ef,
    0x0f01f012,
    0xb672d112,
    0xf5a34811,
    0x3b285343,
    0xfba14910,
    0xf3c32303,
    0xeb032387,
    0xeb000343,
    0x31940183,
    0xfdd0f5fc,
    0xbd08b662,
    0xf5a34808,
    0x3b285343,
    0xfba14907,
    0xf3c32303,
    0xeb032387,
    0xeb000343,
    0x31940183,
    0xfdbef5fc,
    0xbf00bd08,
    0xc0005b4c,
    0xff00ff01,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch19 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch19_ibrt_ins_data_t6),
    0x00008b24,
    0xb8f8f1fe,
    0xc0006d18,
    (uint8_t *)bes1400_patch19_ibrt_ins_data_t6
};//add lock interrupt when free acl buf

const uint32_t bes1400_patch20_ibrt_ins_data_t6[] =
{
    0x460cb510,
    0xfafaf5fa,
    0xd9042870,
    0x007ef1b4,
    0x2001bf18,
    0x2000bd10,
    0xbf00bd10,
    0xf7ff4621,
    0x2800ffef,
    0x4628d104,
    0xfae8f5fa,
    0xbd9af61c,
    0xb85ff61d,/*6df4*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch20 =
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch20_ibrt_ins_data_t6),
    0x00023924,
    0xba5cf1e3,
    0xc0006dc4,
    (uint8_t *)bes1400_patch20_ibrt_ins_data_t6
};//protect ext3 lmp tx cfm

const uint32_t bes1400_patch21_ibrt_ins_data_t6[] =
{
    0x2040f897,/*6e00*/
    0xd1112a01,
    0x0109f106,
    0xf5fc4608,
    0x2813fd4b,
    0x221ed802,
    0xbc54f614,
    0x2a017a32,
    0xf614d801,
    0x221ebbf6,
    0xbc4cf614,
    0xbc47f614,/*6e2c*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch21 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch21_ibrt_ins_data_t6),
    0x0001b604,
    0xbbfcf1eb,
    0xc0006e00,
    (uint8_t *)bes1400_patch21_ibrt_ins_data_t6
};//reject set afh lmp when channel number less than 20

const uint32_t bes1400_patch22_ibrt_ins_data_t6[] =
{
    0x2202729a,/*6e34*/
    0xbd38719a,/*6e38*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch22 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch22_ibrt_ins_data_t6),
    0x00011c90,
    0xb8d0f1f5,
    0xc0006e34,
    (uint8_t *)bes1400_patch22_ibrt_ins_data_t6
};//update sco sync type

const uint32_t bes1400_patch2a_ibrt_ins_data_t6[] =
{
    /*6e5c*/ 0xb10b4b04,
    /*6e60*/ 0xbb84f615, //jump a001c56c
    /*6e64*/ 0x783b2300,
    /*6e68*/ 0x0301f003,
    /*6e6c*/ 0xbb5ef615, //jump a001c52c
    /*6e70*/ 0x00000000, //reject sniff
};//LMP_MSG_HANDLER(sniff_req) LC_CONNECTED

const uint32_t bes1400_patch23_ibrt_ins_data_t6[] =
{
    0xf9c6f632,
    0x02194603,
    0x417ff401,
    0xbeeff632,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch23 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch23_ibrt_ins_data_t6),
    0x0003a52c,
    0xb90af1cd,
    0xc0007744,
    (uint8_t *)bes1400_patch23_ibrt_ins_data_t6
};//fix sco success index

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch24 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x00002490,
    0xbf00e012,
    0,
    NULL,
};//stop ibrt directly

const uint32_t bes1400_patch25_ibrt_ins_data_t6[] =
{
    0x2a020a02,
    0xbf00d804,
    0x427ff400,
    0xbf44f61c,
    0xb807f61d,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch25 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch25_ibrt_ins_data_t6),
    0x00023d28,
    0xb8b4f1e3,
    0xc0006e94,
    (uint8_t *)bes1400_patch25_ibrt_ins_data_t6
};//enc update2 idx error

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch26 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00013f44,
    0xbf00bd38,
    0,
    NULL
};//assert

#if defined(SYNC_BT_CTLR_PROFILE)
const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch27 =
{
    27,
    BTDRV_PATCH_ACT,
    0,
    0x000187b8,
    0x4628b123,
    0,
    NULL
};//send profile via ble
#else
const uint32_t bes1400_patch27_ibrt_ins_data_t6[] =
{
    0xfe88f632,
    0xffe8f7ff,
    0xb9a2f625,/*6c68*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch27 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch27_ibrt_ins_data_t6),
    0x0002bfac,
    0xbe58f1da,
    0xc0006c60,
    (uint8_t *)bes1400_patch27_ibrt_ins_data_t6
};//disconnect update ke state
#endif

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch28 =
{
    28,
    BTDRV_PATCH_ACT,
    0,
    0x0000e47a,
    0xe0027b20,
    0,
    NULL
};//lm_inq_res_ind_handler()  rssi

const uint32_t bes1400_patch29_ibrt_ins_data_t6[] =
{
    0xf8934b04,
    0x2b0231c2,
    0x230fbf0c,
    0x75ab2314,
    0xbac2f629,
    0xc0005ea8,/*6ec4*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch29 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch29_ibrt_ins_data_t6),
    0x00030444,
    0xbd34f1d6,
    0xc0006eb0,
    (uint8_t *)bes1400_patch29_ibrt_ins_data_t6
};///fix acl interv priority

const uint32_t bes1400_patch30_ibrt_ins_data_t6[] =
{
    0xd80a2802,
    0x4b05b508,
    0x3020f853,
    0xf893b123,
    0x220510c4,
    0xfe86f61d,
    0x4770bd08,
    0xc00009d4,
    0xffeef7ff,
    0xbda8f634,/*6ef0*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch30 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch30_ibrt_ins_data_t6),
    0x0003ba40,
    0xba54f1cb,
    0xc0006ecc,
    (uint8_t *)bes1400_patch30_ibrt_ins_data_t6
};///send max slot req when start ibrt

const uint32_t bes1400_patch31_ibrt_ins_data_t6[] =
{
    0xf632b920,
    0x2800fdc1,
    0xbf00d002,
    0xbb3af629,
    0xbba2f629,/*6f08*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch31 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch31_ibrt_ins_data_t6),
    0x00030578,
    0xbcbef1d6,
    0xc0006ef8,
    (uint8_t *)bes1400_patch31_ibrt_ins_data_t6
};//modify start ibrt timing

#if 0//def BT_CONTROLLER_DEBUG
const BTDRV_PATCH_STRUCT bes2300p_t3_ibrt_ins_patch32=
{
    32,
    BTDRV_PATCH_ACT,
    0,
    0x00000348,
    0xbf00e005,
    0,
    NULL
};///lmp trace
#else

const uint32_t bes1400_patch32_ibrt_ins_data_t6[] =
{
    0xfe4af5f9,
    0xbf002300,
    0x30c8f884,
    0xbaf8f624,/*6f18*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch32 =
{
    32,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch32_ibrt_ins_data_t6),
    0x0002b508,
    0xbd00f1db,
    0xc0006f0c,
    (uint8_t *)bes1400_patch32_ibrt_ins_data_t6
};///role switch fail
#endif

const uint32_t bes1400_patch33_ibrt_ins_data_t6[] =
{
    0x2300b430,
    0x4d072103,
    0x4290b2da,
    0xf855d004,
    0x2c004023,
    0x4611bf18,
    0x2b033301,
    0x4608d1f4,
    0x4770bc30,
    0xc00009d4,/*6f50*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch33 =
{
    33,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch33_ibrt_ins_data_t6),
    0x0002c8c0,
    0xbb2ef1da,
    0xc0006f20,
    (uint8_t *)bes1400_patch33_ibrt_ins_data_t6
};//find_other_acl_link

const uint32_t bes1400_patch34_ibrt_ins_data_t6[] =
{
    0xfe38f632,
    0xd0052800,
    0xbf002002,
    0xfcb0f61c,
    0xbb2ff634,
    0xbb25f634,/*6f6c*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch34 =
{
    34,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch34_ibrt_ins_data_t6),
    0x0003b5b4,
    0xbcd0f1cb,
    0xc0006f58,
    (uint8_t *)bes1400_patch34_ibrt_ins_data_t6
};//do not trans afh after IBRT switch

const uint32_t bes1400_patch35_ibrt_ins_data_t6[] =
{
    0xfd92f632,/*6f70*/
    0xd1092801,
    0xfe28f632,
    0xd0052800,
    0xfd9cf632,
    0xd10142a0,
    0xbc77f611,
    0x68294620,
    0xbc70f611,/*6f90*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch35 =
{
    35,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch35_ibrt_ins_data_t6),
    0x00018870,
    0xbb7ef1ee,
    0xc0006f70,
    (uint8_t *)bes1400_patch35_ibrt_ins_data_t6
};//acl data tx cfm

const uint32_t bes1400_patch36_ibrt_ins_data_t6[] =
{
    0x460db538,
    0x78034614,
    0x7842085b,
    0x2b7f78c1,
    0x2a01d10a,
    0x290dd11e,
    0xf632d11c,
    0x4601fd45,
    0xf6342000,
    0xe015ff1f,
    0xd1132b03,
    0xd1112a18,
    0xd80f2d02,
    0xf6104628,
    0xb158fcb3,
    0x3047f890,
    0xd1072b02,
    0xf62a4628,
    0x0228fc0b,
    0x0001f040,
    0xfc88f60f,
    0x000cf1a4,
    0xfdf8f5f9,
    0xbf00bd38,
    0xd1072800,
    0x4620bf00,
    0x46524639,
    0xffc8f7ff,
    0xbbd8f61a,
    0xbb62f61a,/*7034*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch36 =
{
    36,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch36_ibrt_ins_data_t6),
    0x000216f8,
    0xbc92f1e5,
    0xc0006fc0,
    (uint8_t *)bes1400_patch36_ibrt_ins_data_t6
};//lmp filter

const uint32_t bes1400_patch37_ibrt_ins_data_t6[] =
{
    0x47f0e92d,/*7050*/
    0x4b97b082,
    0x236e5c1e,
    0xf300fb03,
    0x5a9b4a95,
    0x4f00f413,
    0x8120f040,
    0x46044689,
    0x1800ea4f,
    0xeb084f91,
    0x4b910507,
    0xa020f853,
    0xf8524a90,
    0x33013020,
    0x3020f842,
    0xf860f635,
    0x42a04601,
    0x8088f040,
    0xf8534b8a,
    0x4b8a2024,
    0x429a681b,
    0xf8dad817,
    0xebc22084,
    0xf0230309,
    0xf1b34378,
    0xd9046f80,
    0x0302ebc9,
    0x4378f023,
    0x4a82425b,
    0x0024f852,
    0x68124a81,
    0xf200fb02,
    0xf2404293,
    0x4b7f80eb,
    0x3000f993,
    0x4b7eb9c3,
    0xc000f893,
    0x0f00f1bc,
    0x462ad00a,
    0x0e0ceb05,
    0xf9122300,
    0x44030b01,
    0xd1fa4572,
    0xe0014662,
    0x46132200,
    0xfcfcfb93,
    0xf8804872,
    0xe001c000,
    0x46132200,
    0xf890486f,
    0x4870e000,
    0x9000f890,
    0xf890486f,
    0xfa4fc000,
    0xebc9f08e,
    0xfa4f0000,
    0x4550fa8c,
    0xebc9dd05,
    0x48660e0e,
    0xe000f880,
    0x4864e002,
    0xc000f880,
    0x0e00f04f,
    0xf840485d,
    0xf8dfe024,
    0xf85ee178,
    0x30010024,
    0x0024f84e,
    0x7800485d,
    0xd2054286,
    0x783f4f5a,
    0x360155af,
    0xe004b2f6,
    0x78364e57,
    0x6007f808,
    0x4f4d2601,
    0x4282553e,
    0x442ada06,
    0xf9124405,
    0x44234b01,
    0xd1fa42aa,
    0xfb934a53,
    0x9200f0f0,
    0xb2406a14,
    0x23002201,
    0xe08147a0,
    0xf8534b46,
    0x4b4e2024,
    0x429a681b,
    0xf8dad816,
    0xebc22084,
    0xf0230309,
    0xf1b34378,
    0xd9046f80,
    0x0302ebc9,
    0x4378f023,
    0x4a3e425b,
    0x1024f852,
    0x68124a44,
    0xf201fb02,
    0xd9634293,
    0xf9934b42,
    0xb9c33000,
    0xf8934b3a,
    0xf1bee000,
    0xd00a0f00,
    0xeb05462a,
    0x2300000e,
    0x1b01f912,
    0x4282440b,
    0x4672d1fa,
    0x2200e001,
    0xfb934613,
    0x4936fefe,
    0xe000f881,
    0x2200e001,
    0x49334613,
    0x49337808,
    0xc000f891,
    0xf8914932,
    0xb241e000,
    0x0101ebcc,
    0xf98efa4f,
    0xdd044549,
    0x0000ebcc,
    0x7008492a,
    0x4929e002,
    0xe000f881,
    0x491c2000,
    0x0024f841,
    0xf850481c,
    0x31011024,
    0x1024f840,
    0x7808491c,
    0xd2054286,
    0x78094920,
    0x360155a9,
    0xe004b2f6,
    0x7809491d,
    0x1007f808,
    0x490c2601,
    0x4282550e,
    0x442ada06,
    0xf9124405,
    0x440b1b01,
    0xd1fa42aa,
    0xfb934a12,
    0x9200f0f0,
    0xb2406a15,
    0x22014621,
    0x47a82300,
    0xe8bdb002,
    0xbf0087f0,
    0xc0006af8,//rssi_idx_for_agc[link_id]  shenxin
    0xd02111f8,//EM_BT_RD(EM_BT_RXBIT_ADDR
    0xc0006afc,//rssi_set_for_agc   shenxin
    0xc00009d4,//ld_acl_env[link_id]
    0xc0006b68,//link_no_sync_count[link_id]
    0xc00072fc,//link_agc_thd_tws
    0xc0006b78,//link_no_sync_count_time[link_id]
    0xc0007300,//link_agc_thd_tws_time
    0xc0006b60,//rssi_store_tws
    0xc0006af4,//n   shenxin
    0xc0007304,//rssi_tws_step
    0xc0007308,//rssi_min_value_tws
    0xc0004268,//rwip_rf.rxgain_cntl
    0xc000730c,//link_agc_thd_mobile
    0xc0007310,//link_agc_thd_mobile_time
    0xc0006b88,//rssi_store_mobile
    0xc0007314,//rssi_mobile_step
    0xc0007318,//rssi_min_value_mobile
    0xffffffff,//link_agc_thd_tws
    0x00000030,//link_agc_thd_tws_time
    0x00000014,//rssi_tws_step
    0x0000009c,//rssi_min_value_tws
    0xffffffff,//link_agc_thd_mobile
    0x00000030,//link_agc_thd_mobile_time
    0x00000014,//rssi_mobile_step
    0x000000a1,//rssi_min_value_mobile
    0x99029804,
    0xfe96f7ff,
    0xfc48f632,
    0xb898f626,/*7328*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch37 =
{
    37,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch37_ibrt_ins_data_t6),
    0x0002d458,
    0xbf60f1d9,
    0xc0007050,
    (uint8_t *)bes1400_patch37_ibrt_ins_data_t6
};//swagc no sync

#if 0
const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch38 =
{
    38,
    BTDRV_PATCH_ACT,
    0,
    0x000340b0,
    0xbf00e003,
    0,
    NULL,
};//ld_sco_stop
#elif 1
const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch38 =
{
    38,
    BTDRV_PATCH_ACT,
    0,
    0x00039ecc,
    0xbf00e001,
    0,
    NULL,
};//remove lc_start_lmp_to
#else
const uint32_t bes1400_patch38_ibrt_ins_data_t6[] =
{
    0x30d8f8c4,
    0xfed6f61f,
    0xd2014283,
    0xbca7f62a,
    0xbcbaf62a,/*7048*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch38 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch38_ibrt_ins_data_t6),
    0x000319a4,
    0xbb48f1d5,
    0xc0007038,
    (uint8_t *)bes1400_patch38_ibrt_ins_data_t6
};//first sniff anchor
#endif

const uint32_t bes1400_patch39_ibrt_ins_data_t6[] =
{
    0x681b4b05,/*732c*/
    0xd1032b00,
    0x781a9b12,
    0xbeb8f60b,
    0xbec3f60b,
    0xbf00bf00,

    0xc0007348,//sniffer_sco_auto_accept
    0x00000000,/*7348*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch39 =
{
    39,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch39_ibrt_ins_data_t6),
    0x000130a8,
    0xb940f1f4,
    0xc000732c,
    (uint8_t *)bes1400_patch39_ibrt_ins_data_t6
};//lm_get_sync_param_hl_rsp

const uint32_t bes1400_patch40_ibrt_ins_data_t6[] =
{
    0xfba2f632,
    0xd11d2801,
    0xfc38f632,
    0x4b33b1d0,
    0x2b00681b,
    0xf632d013,
    0x4606fba9,
    0xd10e42b8,
    0xfb98f632,
    0xd15f2800,
    0xfb76f632,
    0x7a224605,
    0x42937a63,
    0x4620d103,
    0xf99af000,//
    0x2000b9d0,
    0xbdeff614,
    0xfb80f632,
    0xd10d2802,
    0xfc16f632,
    0x4b22b150,
    0x2b00681b,
    0x4620d006,
    0xf988f000,//
    0x4b1fb910,
    0x601a2201,
    0xf0037823,
    0xbf000301,
    0xbda9f614,
    0xf8852602,
    0x79a36054,
    0x3055f885,
    0xf844f5fc,
    0x7c4279e3,
    0xd802429a,
    0xf83ef5fc,
    0xf8857c43,
    0x79633056,
    0x3057f885,
    0xf3c37923,
    0xf8850340,
    0x7a233058,
    0x3059f885,
    0xf8a58963,
    0x7ba3305a,
    0x305cf885,
    0xf88578e3,
    0x78a3305d,
    0x305ff885,
    0xfb28f632,
    0x46384603,
    0xf1052110,
    0xf6340252,
    0x2000fc65,
    0xbda3f614,
    0xc0007348,//sniffer_sco_auto_accept
    0xc0007434,//sco_param_filter_status
    0x00000000,
    0xfc3cf634,
    0xb2c33801,
    0xd8022b02,
    0xf6342004,
    0x2000fc3b,
    0xbd91f614,
    0x3c260cff,
    0x5a3712ff,
    0xffffffff,
    0xffffffff,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch40 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch40_ibrt_ins_data_t6),
    0x0001bf10,
    0xba1ef1eb,
    0xc0007350,
    (uint8_t *)bes1400_patch40_ibrt_ins_data_t6
};//LMP_MSG_HANDLER(esco_link_req)

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch41 =
{
    41,
    BTDRV_PATCH_ACT,
    0,
    0x0003b818,
    0xfb1be001,
    0,
    NULL,
};//config fastack none mode

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch42 =
{
    42,
    BTDRV_PATCH_ACT,
    0,
    0x0002ceb4,
    0xe00b0f0f,
    0,
    NULL
};//ld_acl_rx() average rssi

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch43 =
{
    43,
    BTDRV_PATCH_ACT,
    0,
    0x0002a51c,
    0x3300f443,
    0,
    NULL,
};//bt_e_scoltcntl_retxnb_setf 2

const uint32_t bes1400_patch44_ibrt_ins_data_t6[] =
{
    0xbf00686b,
    0xd00f42b3,
    0x4a212100,
    0x6863e004,
    0x42b34629,
    0x4625d00a,
    0xd9264293,
    0xd8242e01,
    0x462c461e,
    0xe001460d,
    0x2500462c,
    0x8008f8d4,
    0xa00cf894,
    0x900df894,
    0x46384f16,
    0x46224629,
    0xfb12f5fc,
    0x0014f1a7,
    0xf5fc4621,
    0xf1b8fa1d,
    0xd0050f00,
    0x46514630,
    0x2300464a,
    0xe00847c0,
    0x21ff480d,
    0xfffaf5ff,
    0x682ce003,
    0xd1ce2c00,
    0xb924e000,
    0xf44f4808,
    0xf5ff7185,
    0xe8bdffef,
    0xbf0087f0,
    0xc0000954,
    0xd02200bc,
    0xa00508ec,
    0x07fffffc,
    0xc0000968,
    0xa00508b4,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch44 =
{
    44,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch44_ibrt_ins_data_t6),
    0x0002706c,
    0xb9f8f1e0,
    0xc0007460,
    (uint8_t *)bes1400_patch44_ibrt_ins_data_t6
};

const uint32_t bes1400_patch45_ibrt_ins_data_t6[] =
{
    0x4b05b508,
    0x2b01681b,
    0x2200d104,
    0x601a4b02,
    0xfe6cf633,
    0xbf00bd08,
    0xc00076a4,
    0x00000000,
    0xffeef7ff,
    0xd1012b04,
    0xbff0f5ff,
    0xbff5f5ff,/*76b4*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch45 =
{
    45,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch45_ibrt_ins_data_t6),
    0x00007690,
    0xb80af200,
    0xc0007688,
    (uint8_t *)bes1400_patch45_ibrt_ins_data_t6
};//dbg state

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch46 =
{
    46,
    BTDRV_PATCH_ACT,
    0,
    0x0003bdc0,
    0xbf00e010,
    0,
    NULL,
};//IBRT mode start sco donot set afh

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch47 =
{
    47,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch2a_ibrt_ins_data_t6),
    0x0001c528,
    0xbc98f1ea, //a001c528 -> a0206e5c
    0xc0006e5c,
    (uint8_t *)bes1400_patch2a_ibrt_ins_data_t6
};//LMP_MSG_HANDLER(sniff_req) LC_CONNECTED

const uint32_t bes1400_patch48_ibrt_ins_data_t6[] =
{
    0x681b4b1d,
    0xd1352b00,
    0xb083b530,
    0x8310f3ef,
    0x0f01f013,
    0xb672d117,
    0x25004c18,
    0x95019500,
    0xf1044620,
    0x2208013c,
    0xf5fc2307,
    0x9500f9ed,
    0xf5049501,
    0xf5047080,
    0x2208718a,
    0xf5fc2307,
    0xb662f9e3,
    0x4c0de014,
    0x95002500,
    0x46209501,
    0x013cf104,
    0x23072208,
    0xf9d6f5fc,
    0x95019500,
    0x7080f504,
    0x718af504,
    0x23072208,
    0xf9ccf5fc,
    0xbd30b003,
    0xbf004770,
    0xc0005df0,
    0xc0005b24,
    0xffbef7ff,
    0xe8bdb004,
    0xbf0081f0,/*75a4*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch48 =
{
    48,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch48_ibrt_ins_data_t6),
    0x000013b0,
    0xb8f2f206,
    0xc0007518,
    (uint8_t *)bes1400_patch48_ibrt_ins_data_t6
};
#if 0
const uint32_t bes1400_patch49_ibrt_ins_data_t6[] =
{
    0x78004804,
    0xbf00b120,
    0xfd8cf632,
    0xba1af609,
    0xba1af609,
    0xc0006221,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch49 =
{
    49,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch49_ibrt_ins_data_t6),
    0x000103d8,
    0xbddef1f6,
    0xc0006f98,
    (uint8_t *)bes1400_patch49_ibrt_ins_data_t6
};//lc_sco_ind
#else
const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch49 =
{
    49,
    BTDRV_PATCH_ACT,
    0,
    0x000103d8,
    0xbf00e002,
    0,
    NULL,
};//lc_sco_ind
#endif

const uint32_t bes1400_patch50_ibrt_ins_data_t6[] =
{
    0xf632d806,
    0x2801fb0b,
    0x0209d002,
    0xbbeef632,
    0xbf00bd08,/*75d0*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch50 =
{
    50,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch50_ibrt_ins_data_t6),
    0x00039d98,
    0xbc0af1cd,
    0xc00075b0,
    (uint8_t *)bes1400_patch50_ibrt_ins_data_t6
};//do not trans stop SCO

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch51 =
{
    51,
    BTDRV_PATCH_ACT,
    0,
    0x0002ea00,
    0x8ff8e8bd,
    0,
    NULL,
};//remove lmp flush

const uint32_t bes1400_patch52_ibrt_ins_data_t6[] =
{
    0x4605b5f8,
    0xf8544c19,
    0xf6327020,
    0x4606fb05,
    0x4020f854,
    0xfaecf632,
    0xf632b320,
    0x42a8fa61,
    0xf897d120,
    0x2b023047,
    0x2e02d11c,
    0xb1ccd81a,
    0x3047f894,
    0xd1152b02,
    0x3040f894,
    0x0234b973,
    0x0401f044,
    0x4620b2a4,
    0xfeccf5f9,
    0xd0052836,
    0x60a2f44f,
    0x46224621,
    0xfadaf5f9,
    0x4b042001,
    0xbdf86018,
    0x4b022000,
    0xbdf86018,
    0xc0005de4,
    0xc0007668,
    0x4638bf00,
    0xffc2f7ff,
    0x4628d104,
    0xfeb0f5f9,
    0xfc38f614,
    0xbc84f614,
    0x00000000,/*7668*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch52 =
{
    52,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch52_ibrt_ins_data_t6),
    0x0001bed0,
    0xbbbef1eb,
    0xc00075dc,
    (uint8_t *)bes1400_patch52_ibrt_ins_data_t6
};//delay mobile unsniff lmp

const uint32_t bes1400_patch53_ibrt_ins_data_t6[] =
{
    0x681b4b05,
    0xd1012b01,
    0xfb04f632,
    0x4b022000,
    0x6018bf00,
    0x81f0e8bd,
    0xc0007434,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch53 =
{
    53,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch53_ibrt_ins_data_t6),
    0x0003a088,
    0xbb3af1cd,
    0xc0007700,
    (uint8_t *)bes1400_patch53_ibrt_ins_data_t6
};//don not trans start ibrt sco

const uint32_t bes1400_patch54_ibrt_ins_data_t6[] =
{
    0xbf000a28,
    0xffc6f626,
    0x0a210a28,
    0xb8a4f611,/*767c*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch54 =
{
    54,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch54_ibrt_ins_data_t6),
    0x000187c4,
    0xbf54f1ee,
    0xc0007670,
    (uint8_t *)bes1400_patch54_ibrt_ins_data_t6
};//protect tws tx type

const BTDRV_PATCH_STRUCT bes1400_t6_ibrt_ins_patch55 =
{
    55,
    BTDRV_PATCH_ACT,
    0,
    0x00030758,
    0xbf00e03a,
    0,
    NULL,
};//decrease the connection time of TWS link in SCO

/////1402 t6 ibrt patch (chip id =1)
static const uint32_t best1400_t6_ibrt_ins_patch_config[] =
{
    56,
    (uint32_t)&bes1400_t6_ibrt_ins_patch0,
    (uint32_t)&bes1400_t6_ibrt_ins_patch1,
    (uint32_t)&bes1400_t6_ibrt_ins_patch2,
    (uint32_t)&bes1400_t6_ibrt_ins_patch3,
    (uint32_t)&bes1400_t6_ibrt_ins_patch4,
    (uint32_t)&bes1400_t6_ibrt_ins_patch5,
    (uint32_t)&bes1400_t6_ibrt_ins_patch6,
    (uint32_t)&bes1400_t6_ibrt_ins_patch7,
    (uint32_t)&bes1400_t6_ibrt_ins_patch8,
    (uint32_t)&bes1400_t6_ibrt_ins_patch9,
    (uint32_t)&bes1400_t6_ibrt_ins_patch10,
    (uint32_t)&bes1400_t6_ibrt_ins_patch11,
    (uint32_t)&bes1400_t6_ibrt_ins_patch12,
    (uint32_t)&bes1400_t6_ibrt_ins_patch13,
    (uint32_t)&bes1400_t6_ibrt_ins_patch14,
    (uint32_t)&bes1400_t6_ibrt_ins_patch15,
    (uint32_t)&bes1400_t6_ibrt_ins_patch16,
    (uint32_t)&bes1400_t6_ibrt_ins_patch17,
    (uint32_t)&bes1400_t6_ibrt_ins_patch18,
    (uint32_t)&bes1400_t6_ibrt_ins_patch19,
    (uint32_t)&bes1400_t6_ibrt_ins_patch20,
    (uint32_t)&bes1400_t6_ibrt_ins_patch21,
    (uint32_t)&bes1400_t6_ibrt_ins_patch22,
    (uint32_t)&bes1400_t6_ibrt_ins_patch23,
    (uint32_t)&bes1400_t6_ibrt_ins_patch24,
    (uint32_t)&bes1400_t6_ibrt_ins_patch25,
    (uint32_t)&bes1400_t6_ibrt_ins_patch26,
    (uint32_t)&bes1400_t6_ibrt_ins_patch27,
    (uint32_t)&bes1400_t6_ibrt_ins_patch28,
    (uint32_t)&bes1400_t6_ibrt_ins_patch29,
    (uint32_t)&bes1400_t6_ibrt_ins_patch30,
    (uint32_t)&bes1400_t6_ibrt_ins_patch31,
    (uint32_t)&bes1400_t6_ibrt_ins_patch32,
    (uint32_t)&bes1400_t6_ibrt_ins_patch33,
    (uint32_t)&bes1400_t6_ibrt_ins_patch34,
    (uint32_t)&bes1400_t6_ibrt_ins_patch35,
    (uint32_t)&bes1400_t6_ibrt_ins_patch36,
    (uint32_t)&bes1400_t6_ibrt_ins_patch37,
    (uint32_t)&bes1400_t6_ibrt_ins_patch38,
    (uint32_t)&bes1400_t6_ibrt_ins_patch39,
    (uint32_t)&bes1400_t6_ibrt_ins_patch40,
    (uint32_t)&bes1400_t6_ibrt_ins_patch41,
    (uint32_t)&bes1400_t6_ibrt_ins_patch42,
    (uint32_t)&bes1400_t6_ibrt_ins_patch43,
    (uint32_t)&bes1400_t6_ibrt_ins_patch44,
    (uint32_t)&bes1400_t6_ibrt_ins_patch45,
    (uint32_t)&bes1400_t6_ibrt_ins_patch46,
    (uint32_t)&bes1400_t6_ibrt_ins_patch47,
    (uint32_t)&bes1400_t6_ibrt_ins_patch48,
    (uint32_t)&bes1400_t6_ibrt_ins_patch49,
    (uint32_t)&bes1400_t6_ibrt_ins_patch50,
    (uint32_t)&bes1400_t6_ibrt_ins_patch51,
    (uint32_t)&bes1400_t6_ibrt_ins_patch52,
    (uint32_t)&bes1400_t6_ibrt_ins_patch53,
    (uint32_t)&bes1400_t6_ibrt_ins_patch54,
    (uint32_t)&bes1400_t6_ibrt_ins_patch55,
};
#else

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x000324d4,
    0xbf00bf00,
    0,
    NULL
};// afh filter disable

const uint32_t bes1400_patch1_ins_data_t6[] =
{
    0xf8934b09,
    0xf0133084,
    0xd0050f06,
    0xf6374620,
    0x4601f839,
    0xe0052400,
    0xf8934b03,
    0xbf003084,
    0xb975f636,
    0xb9bbf636,
    0xc0006568,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch1_ins_data_t6),
    0x0003cd08,
    0xbe7af1c9,
    0xc0006a00,
    (uint8_t *)bes1400_patch1_ins_data_t6
};// set ble adv data

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x0003cbf0,
    0xbf00e004,
    0,
    NULL
};// set ble scan response data

const uint32_t bes1400_patch3_ins_data_t6[] =
{
    /*6a98*/ 0x99049803,
    /*6a9c*/ 0x4b12b470,
    /*6aa0*/ 0x010e5c5a,
    /*6aa4*/ 0x19734d11,
    /*6aa8*/ 0x78244c11,
    /*6aac*/ 0xd20342a2,
    /*6ab0*/ 0x32015498,
    /*6ab4*/ 0xe001b2d2,
    /*6ab8*/ 0x22015570,
    /*6abc*/ 0x5442480a,
    /*6ac0*/ 0x461ab14c,
    /*6ac4*/ 0x23001919,
    /*6ac8*/ 0x0b01f912,
    /*6acc*/ 0xbf004403,
    /*6ad0*/ 0xd1f9428a,
    /*6ad4*/ 0x2300e000,
    /*6ad8*/ 0xf0f4fb93,
    /*6adc*/ 0xbc70b240,
    /*6ae0*/ 0x22019904,
    /*6ae4*/ 0xba00f626, //jump a0206ae4 -> a002cee8
    /*6ae8*/ 0xc0006af8, //addr of rssi_idx_for_agc
    /*6aec*/ 0xc0006afc, //addr of rssi_set_for_agc
    /*6af0*/ 0xc0006af4, //addr of N
    /*6af4*/ 0x00000005, //N : 1 ~ 16
    /*6af8*/ 0x00000000, //rssi_idx_for_agc[3]
    /*6afc*/ 0xbabababa, //rssi_set_for_agc[3*16]
    /*6b00*/ 0xbabababa,
    /*6b04*/ 0xbabababa,
    /*6b08*/ 0xbabababa,
    /*6b0c*/ 0xbabababa,
    /*6b10*/ 0xbabababa,
    /*6b14*/ 0xbabababa,
    /*6b18*/ 0xbabababa,
    /*6b1c*/ 0xbabababa,
    /*6b20*/ 0xbabababa,
    /*6b24*/ 0xbabababa,
    /*6b28*/ 0xbabababa,
    /*6b2c*/ 0xbabababa,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch3_ins_data_t6),
    0x0002cee4,
    0xbdd8f1d9, //jump a002cee4 -> a0206a98
    0xc0006a98,
    (uint8_t *)bes1400_patch3_ins_data_t6
};//ld_acl_rx() average rssi

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x0002ceb4,
    0xe00b0f0f,
    0,
    NULL
};//ld_acl_rx() average rssi

const uint32_t bes1400_patch5_ins_data_t6[] =
{
    0x2028f843,/*6b3c*/
    0xf8434b08,
    0x4b052028,
    0x4b0e701a,
    0x2201701a,
    0xf8434b08,
    0xbf002028,
    0xba32f624,
    0xc0006b60,
    0x00000000,
    0xc0006b68,
    0x00000000,
    0x00000000,
    0x00000000,
    0xc0006b78,
    0x00000001,
    0x00000001,
    0x00000001,
    0xc0006b88,
    0x00000000,/*6b88*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch5_ins_data_t6),
    0x0002afbc,
    0xbdbef1db,
    0xc0006b3c,
    (uint8_t *)bes1400_patch5_ins_data_t6
};//sync clear count

const uint32_t bes1400_patch6_ins_data_t6[] =
{
    0x47f0e92d,/*7050*/
    0x4b97b082,
    0x236e5c1e,
    0xf300fb03,
    0x5a9b4a95,
    0x4f00f413,
    0x8120f040,
    0x46044689,
    0x1800ea4f,
    0xeb084f91,
    0x4b910507,
    0xa020f853,
    0xf8524a90,
    0x33013020,
    0x3020f842,
    0xf860f635,
    0x42a04601,
    0x8088f040,
    0xf8534b8a,
    0x4b8a2024,
    0x429a681b,
    0xf8dad817,
    0xebc22084,
    0xf0230309,
    0xf1b34378,
    0xd9046f80,
    0x0302ebc9,
    0x4378f023,
    0x4a82425b,
    0x0024f852,
    0x68124a81,
    0xf200fb02,
    0xf2404293,
    0x4b7f80eb,
    0x3000f993,
    0x4b7eb9c3,
    0xc000f893,
    0x0f00f1bc,
    0x462ad00a,
    0x0e0ceb05,
    0xf9122300,
    0x44030b01,
    0xd1fa4572,
    0xe0014662,
    0x46132200,
    0xfcfcfb93,
    0xf8804872,
    0xe001c000,
    0x46132200,
    0xf890486f,
    0x4870e000,
    0x9000f890,
    0xf890486f,
    0xfa4fc000,
    0xebc9f08e,
    0xfa4f0000,
    0x4550fa8c,
    0xebc9dd05,
    0x48660e0e,
    0xe000f880,
    0x4864e002,
    0xc000f880,
    0x0e00f04f,
    0xf840485d,
    0xf8dfe024,
    0xf85ee178,
    0x30010024,
    0x0024f84e,
    0x7800485d,
    0xd2054286,
    0x783f4f5a,
    0x360155af,
    0xe004b2f6,
    0x78364e57,
    0x6007f808,
    0x4f4d2601,
    0x4282553e,
    0x442ada06,
    0xf9124405,
    0x44234b01,
    0xd1fa42aa,
    0xfb934a53,
    0x9200f0f0,
    0xb2406a14,
    0x23002201,
    0xe08147a0,
    0xf8534b46,
    0x4b4e2024,
    0x429a681b,
    0xf8dad816,
    0xebc22084,
    0xf0230309,
    0xf1b34378,
    0xd9046f80,
    0x0302ebc9,
    0x4378f023,
    0x4a3e425b,
    0x1024f852,
    0x68124a44,
    0xf201fb02,
    0xd9634293,
    0xf9934b42,
    0xb9c33000,
    0xf8934b3a,
    0xf1bee000,
    0xd00a0f00,
    0xeb05462a,
    0x2300000e,
    0x1b01f912,
    0x4282440b,
    0x4672d1fa,
    0x2200e001,
    0xfb934613,
    0x4936fefe,
    0xe000f881,
    0x2200e001,
    0x49334613,
    0x49337808,
    0xc000f891,
    0xf8914932,
    0xb241e000,
    0x0101ebcc,
    0xf98efa4f,
    0xdd044549,
    0x0000ebcc,
    0x7008492a,
    0x4929e002,
    0xe000f881,
    0x491c2000,
    0x0024f841,
    0xf850481c,
    0x31011024,
    0x1024f840,
    0x7808491c,
    0xd2054286,
    0x78094920,
    0x360155a9,
    0xe004b2f6,
    0x7809491d,
    0x1007f808,
    0x490c2601,
    0x4282550e,
    0x442ada06,
    0xf9124405,
    0x440b1b01,
    0xd1fa42aa,
    0xfb934a12,
    0x9200f0f0,
    0xb2406a15,
    0x22014621,
    0x47a82300,
    0xe8bdb002,
    0xbf0087f0,
    0xc0006af8,//rssi_idx_for_agc[link_id]  shenxin
    0xd02111f8,//EM_BT_RD(EM_BT_RXBIT_ADDR
    0xc0006afc,//rssi_set_for_agc   shenxin
    0xc00009d4,//ld_acl_env[link_id]
    0xc0006b68,//link_no_sync_count[link_id]
    0xc00072fc,//link_agc_thd_tws
    0xc0006b78,//link_no_sync_count_time[link_id]
    0xc0007300,//link_agc_thd_tws_time
    0xc0006b60,//rssi_store_tws
    0xc0006af4,//n   shenxin
    0xc0007304,//rssi_tws_step
    0xc0007308,//rssi_min_value_tws
    0xc0004268,//rwip_rf.rxgain_cntl
    0xc000730c,//link_agc_thd_mobile
    0xc0007310,//link_agc_thd_mobile_time
    0xc0006b88,//rssi_store_mobile
    0xc0007314,//rssi_mobile_step
    0xc0007318,//rssi_min_value_mobile
    0xffffffff,//link_agc_thd_tws
    0x00000030,//link_agc_thd_tws_time
    0x00000014,//rssi_tws_step
    0x0000009c,//rssi_min_value_tws
    0xffffffff,//link_agc_thd_mobile
    0x00000030,//link_agc_thd_mobile_time
    0x00000014,//rssi_mobile_step
    0x000000a1,//rssi_min_value_mobile
    0x99029804,
    0xfe96f7ff,
    0xfc48f632,
    0xb898f626,/*7328*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch6_ins_data_t6),
    0x0002d458,
    0xbf60f1d9,
    0xc0007050,
    (uint8_t *)bes1400_patch6_ins_data_t6
};//swagc no sync

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    0,
    0x0000e47a,
    0xe0027b20,
    0,
    NULL
};//lm_inq_res_ind_handler()  rssi

const uint32_t bes1400_patch8_ins_data_t6[] =
{
    0x2300b430,
    0x4d072103,
    0x4290b2da,
    0xf855d004,
    0x2c004023,
    0x4611bf18,
    0x2b033301,
    0x4608d1f4,
    0x4770bc30,
    0xc00009d4,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch8_ins_data_t6),
    0x0002c8c0,
    0xbb2ef1da,
    0xc0006f20,
    (uint8_t *)bes1400_patch8_ins_data_t6
};//find_other_acl_link

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x000123e8,
    0xf7f1bf00,
    0,
    NULL
};//ignore lm_get_nb_acl(MASTER_FLAG|SLAVE_FLAG) > 1 when cal wesco

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    0,
    0x0002ff48,
    0xd9590f05,
    0,
    NULL,
};//ignore bld_sco_retransmit_dec function

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x0002ea00,
    0x8ff8e8bd,
    0,
    NULL,
};//remove lmp flush

static const uint32_t best1400_t6_ins_patch_config[] =
{
    12,
    (uint32_t)&bes1400_t6_ins_patch0,
    (uint32_t)&bes1400_t6_ins_patch1,
    (uint32_t)&bes1400_t6_ins_patch2,
    (uint32_t)&bes1400_t6_ins_patch3,
    (uint32_t)&bes1400_t6_ins_patch4,
    (uint32_t)&bes1400_t6_ins_patch5,
    (uint32_t)&bes1400_t6_ins_patch6,
    (uint32_t)&bes1400_t6_ins_patch7,
    (uint32_t)&bes1400_t6_ins_patch8,
    (uint32_t)&bes1400_t6_ins_patch9,
    (uint32_t)&bes1400_t6_ins_patch10,
    (uint32_t)&bes1400_t6_ins_patch11,
};
#endif

#if defined(BLE_MESH)
/***************************************************************************
 *
 * instruction patch Information
 *
 * BT ROM Chip Version:1400 T5 (CHIP ID=2)
 *
 *  ONLY FOR BLE MESH
 *
 ****************************************************************************/

const BTDRV_PATCH_STRUCT bes1400_t5_mesh_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x0003cc94,
    0xd8052b13,
    0,
    NULL
};// llm_set_adv_param

static const uint32_t best1400_t5_mesh_ins_patch_config[] =
{
    1,
    (uint32_t)&bes1400_t5_mesh_ins_patch0,
};
#elif !defined(IBRT)

/***************************************************************************
 *
 * instruction patch Information
 *
 * BT ROM Chip Version:1400 T5 (CHIP ID=2)
 *
 * __BT_ONE_BRING_TWO__ function
 *
 ****************************************************************************/

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x00030d48,
    0xbf00bf00,
    0,
    NULL
};// set lsto when there is no connect

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x00019acc,
    0xbf00e003,
    0,
    NULL
};////iocap rsp

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x0001a7a4,
    0xbf00e003,
    0,
    NULL
};//pwer rsp in wrong state

const uint32_t bes1400_patch3_ins_data_t5[] =
{
    0xf5fa4620, /*6900*/
    0x285dfd4b,
    0xf240d10e,
    0x46215005,
    0xf84cf5fa, /*6910*/
    0xf8534b05,
    0x46203026,
    0x135ef893,
    0xfcd0f5fa, /*6920*/
    0xb9f3f617,
    0xb9ebf617,
    0xc0005cd8, /*692c*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch3_ins_data_t5),
    0x0001dcfc,
    0xbe00f1e8,
    0xc0006900,
    (uint8_t *)bes1400_patch3_ins_data_t5
};//not accept

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x0002a4a4,
    0xbf8c7f80,
    0,
    NULL
};//LD_ACL_STOP_NOTIF_BIT_OFF_THR

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x0002a7e4,
    0x7f80f5b3,
    0,
    NULL
};//LD_ACL_STOP_NOTIF_BIT_OFF_THR

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    0,
    0x0002a69c,
    0xbf947f80,
    0,
    NULL
};//LD_ACL_STOP_NOTIF_BIT_OFF_THR

const uint32_t bes1400_patch7_ins_data_t5[] =
{
    0xd01e0fdb, /*6930*/
    0x6307f44f,
    0xf304fb03,
    0x48168f6a,
    0x93011810, /*6940*/
    0x49154418,
    0xfea0f644,
    0x58fb4b14,
    0x4301f3c3, /*6950*/
    0xf1028f2a,
    0xf5024250,
    0xf2021205,
    0x8f6942fc, /*6960*/
    0x0241eb02,
    0x440a9901,
    0x600a490d,
    0xf44fe010, /*6970*/
    0xfb036307,
    0x9301f304,
    0x18184806,
    0x8f6a4906, /*6980*/
    0xfe82f644,
    0x58fb4b05,
    0x4381f3c3,
    0xba27f62e, /*6990*/
    0xba2df62e,

    0xd02144fc,
    0xc000593c,
    0xd0220144, /*69a0*/
    0xc000005c, /*69a4*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch7_ins_data_t5),
    0x00034d80,
    0xbdd6f1d1,
    0xc0006930,
    (uint8_t *)bes1400_patch7_ins_data_t5
};//ld_sco_frm_isr

const uint32_t bes1400_patch8_ins_data_t5[] =
{
    0xf6052002, /*69a8*/
    0x2802ffbf,
    0xf898d110, /*69b0*/
    0x230120b2,
    0x4a074093,
    0x21c0f8b2,
    0x0303ea22, /*69c0*/
    0x0341f3c3,
    0xf8524a04,
    0xbf003023,
    0xbbecf625, /*69d0*/
    0xbbeef625,

    0xc0005d9c,//ld_env
    0xc0000998,//ld_acl_env /*69dc*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch8_ins_data_t5),
    0x0002c188,
    0xbc0ef1da,
    0xc00069a8,
    (uint8_t *)bes1400_patch8_ins_data_t5
};//ld_event_timestamp_calculate

const uint32_t bes1400_patch9_ins_data_t5[] =
{
    0xf8934b07, /*69e0*/
    0xf0133084,
    0xd0070f06,
    0xf6364620,
    0x4601fa4b, /*69f0*/
    0xbf002400,
    0xbbd2f635,
    0xbb91f635,

    0xc0006460,//llm_le_env /*6a00*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch9_ins_data_t5),
    0x0003c11c,
    0xbc60f1ca,
    0xc00069e0,
    (uint8_t *)bes1400_patch9_ins_data_t5
};//hci_le_set_adv_data_cmd_handler

const uint32_t bes1400_patch10_ins_data_t5[] =
{
    0x123ff240, /*6a04*/
    0xd006421a,
    0x2b007aab,
    0x2201d003, /*6a10*/
    0x601a4b05,
    0x2201e002,
    0x601a4b03,
    0xf240892b, /*6a20*/
    0xbf00123f,
    0xbc1af63e,

    0xc0006a30,//le_wrong_packet
    0x00000000, /*6a30*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch10_ins_data_t5),
    0x0004525c,
    0xbbd2f1c1,
    0xc0006a04,
    (uint8_t *)bes1400_patch10_ins_data_t5
};//lld_pdu_rx_handler

const uint32_t bes1400_patch11_ins_data_t5[] =
{
    0xf894b952, /*6a34*/
    0xf01330ae,
    0xd1050f09,
    0x209ff894, /*6a40*/
    0x4a03b912,
    0xb10a6812,
    0xbc05f63f,
    0xbc09f63f, /*6a50*/

    0xc0006a30, /*6a54*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch11_ins_data_t5),
    0x00046248,
    0xbbf4f1c0,
    0xc0006a34,
    (uint8_t *)bes1400_patch11_ins_data_t5
};//lld_evt_restart

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch12 = //from bes1400_t5_ibrt_ins_patch2
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x00007024,
    0xbf00e011,
    0,
    NULL
};//dont call rwip_assert_err when assert error

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch13 = //from bes1400_t5_ibrt_ins_patch4
{
    13,
    BTDRV_PATCH_ACT,
    0,
    0x0002be90,
    0xbd702000,
    0,
    NULL
}; //ld_check_if_need_protect_sniff retrun 0

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch14 = //from bes1400_t5_ibrt_ins_patch9
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0x0002d82c,
    0x2b006013,
    0,
    NULL,
};//ld_acl_rsw_rx

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch15 = //from bes1400_t5_ibrt_ins_patch10
{
    15,
    BTDRV_PATCH_ACT,
    0,
    0x0002fe60,
    0xbf00e001,
    0,
    NULL,
};//ld_acl_flow_on

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch16 = //from bes1400_t5_ibrt_ins_patch12
{
    16,
    BTDRV_PATCH_ACT,
    0,
    0x0002a41c,
    0xbf000f01,
    0,
    NULL
};//mode == SYNC2_SCO_MODE

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch17 = //from bes1400_t5_ibrt_ins_patch17
{
    17,
    BTDRV_PATCH_ACT,
    0,
    0x00007010, //a0007010
    0xb9bef000, //jump a0007390
    0,
    NULL,
};//DEF_IRQHandler

const uint32_t bes1400_patch18_ins_data_t5[] =
{
    0x4681b083,/*6b30*/
    0x4b022200,
    0x2020f843,
    0xbddcf623,
    0xc0006b44,
    0x00000000,
    0x00000000,
    0x00000000,/*6b4c*/
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch18 = //from bes1400_t5_ibrt_ins_patch22
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch18_ins_data_t5),
    0x0002a6f4,
    0xba1cf1dc,
    0xc0006b30,
    (uint8_t *)bes1400_patch18_ins_data_t5
};//sync clear count

const uint32_t bes1400_patch19_ins_data_t5[] =
{
    /*6ba0*/ 0xbf0080c6,
    /*6ba4*/ 0x71022200,
    /*6ba8*/ 0xbb2af619, //jump a0020200
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch19 = //from bes1400_t5_ibrt_ins_patch29
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch19_ins_data_t5),
    0x000201f8,
    0xbcd2f1e6, //jump a00201f8 -> a0206ba0
    0xc0006ba0,
    (uint8_t *)bes1400_patch19_ins_data_t5
};//KE_MSG_HANDLER(lc_op_loc_sniff_req, void)

const uint32_t bes1400_patch20_ins_data_t5[] =
{
    /*6bac*/ 0x46286825,
    /*6bb0*/ 0x303ef895,
    /*6bb4*/ 0xd0012b02,
    /*6bb8*/ 0xbda2f622, //jump a0029700
    /*6bbc*/ 0xbf002300,
    /*6bc0*/ 0x303ef885,
    /*6bc4*/ 0xfea6f622, //call a0029914
    /*6bc8*/ 0xbdc1f622, //jump a002974e
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch20 = //from bes1400_t5_ibrt_ins_patch30
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch20_ins_data_t5),
    0x000296fc,
    0xba56f1dd, //jump a00296fc -> a0206bac
    0xc0006bac,
    (uint8_t *)bes1400_patch20_ins_data_t5
};//ld_pscan_frm_cbk


const uint32_t bes1400_patch21_ins_data_t5[] =
{
    0x4620461d, /*6d00*/
    0x463a4629,
    0xf5f94633,
    0xbdf8fc2b, /*6d0c*/
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch21 = //from bes1400_t5_ibrt_ins_patch0
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch21_ins_data_t5),
    0x000070ec,
    0xbe08f1ff,
    0xc0006d00,
    (uint8_t *)bes1400_patch21_ins_data_t5
};//assert warn

const uint32_t bes1400_patch22_ins_data_t5[] =
{
    0x4630461f, /*6d14*/
    0x462a4639,
    0xf5f94623,
    0xbdf8fc21, /*6d20*/
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch22 = //from bes1400_t5_ibrt_ins_patch1
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch22_ins_data_t5),
    0x00007070,
    0xbe50f1ff,
    0xc0006d14,
    (uint8_t *)bes1400_patch22_ins_data_t5
};//assert param

const uint32_t bes1400_patch23_ins_data_t5[] =
{
    0x2a0a0c92,/*1400 is not same as 1302 and 1402*/
    0x220abf28,/*1400 is not same as 1302 and 1402*/
    0xbf004691,
    0xbc44f623, /*6d3c*/
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch23 = //from bes1400_t5_ibrt_ins_patch40
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch23_ins_data_t5),
    0x0002a5c4,
    0xbbb4f1dc,
    0xc0006d30,
    (uint8_t *)bes1400_patch23_ins_data_t5
};//max_drift limit

const uint32_t bes1400_patch24_ins_data_t5[] =
{
    /*6d9c*/ 0x4b179a0b,
    /*6da0*/ 0xd023429a,
    /*6da4*/ 0x20b5f894,
    /*6da8*/ 0xb9fab2d2,
    /*6dac*/ 0xf9b4b22e,
    /*6db0*/ 0x1b9b3098,
    /*6db4*/ 0xf9b24a0f,
    /*6db8*/ 0x42932000,
    /*6dbc*/ 0x4251da05,
    /*6dc0*/ 0xdd02428b,
    /*6dc4*/ 0x7088f8d4,
    /*6dc8*/ 0x4293e00f,
    /*6dcc*/ 0xf8d4dd05,
    /*6dd0*/ 0x3f017088,
    /*6dd4*/ 0x4778f027,
    /*6dd8*/ 0x4252e007,
    /*6ddc*/ 0xda044293,
    /*6de0*/ 0x7088f8d4,
    /*6de4*/ 0xf0273701,
    /*6de8*/ 0xbf004778,
    /*6dec*/ 0x708cf8c4,
    /*6df0*/ 0xbce4f623, // jump a002a7bc
    /*6df4*/ 0xc0006df8,
    /*6df8*/ 0x00000032, // 50
    /*6dfc*/ 0xa003477b, // ld_acl_sniff_frm_isr lr
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch24 = //from bes1400_t5_ibrt_ins_patch46
{
    24,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch24_ins_data_t5),
    0x0002a7b8,
    0xbaf0f1dc, // jump a002a7b8 -> a0206d9c
    0xc0006d9c,
    (uint8_t *)bes1400_patch24_ins_data_t5
};//ld_acl_rx_sync clk_off cal active mode


const uint32_t bes1400_patch25_ins_data_t5[] =
{
    /*6e10*/ 0xbf00669a,
    /*6e14*/ 0x66da466a,//mov   r2, sp; str   r2, [r3, #108]
    /*6e18*/ 0x8208f3ef,//mrs   r2, MSP
    /*6e1c*/ 0xbf00671a,//str   r2, [r3, #112]
    /*6e20*/ 0x8209f3ef,//mrs   r2, PSP
    /*6e24*/ 0xbf00675a,//str   r2, [r3, #116]
    /*6e28*/ 0xbf00bf00,
    /*6e2c*/ 0xbf00bf00,
    /*6e30*/ 0xbf00bf00,
    /*6e34*/ 0xbf00bf00,
    /*6e38*/ 0xbf00bf00,
    /*6e3c*/ 0xbf00bf00,
    /*6e40*/ 0xbf00bf00,
    /*6e44*/ 0xbf00bf00,
    /*6e48*/ 0xbf00bf00,
    /*6e4c*/ 0xbf00bf00,
    /*6e50*/ 0xba90f600,//jump a0206e50 -> a0007374
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch25 = //from bes1400_t5_ibrt_ins_patch15
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch25_ins_data_t5),
    0x00007360,
    0xbd56f1ff, // jump a0007360 -> a0206e10
    0xc0006e10,
    (uint8_t *)bes1400_patch25_ins_data_t5
};//bt_assert_crash_dump

#define PATCH_IN_EXTRA_SPACE 1
#if PATCH_IN_EXTRA_SPACE
const uint32_t bes1400_patch26_ins_data_t5[] =
{
    0xd8092b01,/*70d0*/
    0xfb03236e,
    0x4a04f308,
    0xf4135a9b,
    0xd1016f00,/*70e0*/
    0xb919f629,
    0xb9aef629,
    0xd0211200,/*70ec*/
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch26 = //from bes1400_t5_ibrt_ins_patch44
{
    26,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch26_ins_data_t5),
    0x00030314,
    0xbedcf1d6,
    0xc00070d0,
    (uint8_t *)bes1400_patch26_ins_data_t5
};//lmp tx for tx desc error

const uint32_t bes1400_patch27_ins_data_t5[] =
{
    0xfeecf5f8, /*7300*/
    0xbf00bf00,
    0x8310f3ef,
    0x0401f013,
    0xb672d108, /*7310*/
    0xff22f5f8,
    0xbf30b110,
    0xd1ef2c00,
    0xe7edb662, /*7320*/
    0xff1af5f8,
    0xd1f62800,
    0xbf00e7e8, /*732c*/
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch27 = //from bes1400_t5_ibrt_ins_patch13
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch27_ins_data_t5),
    0x0000721c,
    0xb870f200,
    0xc0007300,
    (uint8_t *)bes1400_patch27_ins_data_t5
};

const uint32_t bes1400_patch28_ins_data_t5[] =
{
    0xf885b2db, /*7338*/
    0xf89530bd,
    0x2a0220b4, /*7340*/
    0xb98bd114,
    0xf8534b0a,
    0xf6430024,
    0xf895fd15, /*7350*/
    0x2b0430b5,
    0x4620d103,
    0xf6232116,
    0x4620fc61, /*7360*/
    0xf6242116,
    0xbf00f961,
    0xbc6bf62d,
    0xbc53f62d, /*7370*/
    0xc0000998, /*7374*/
};
const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch28 = //from bes1400_t5_ibrt_ins_patch15
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch28_ins_data_t5),
    0x00034c14,
    0xbb90f1d2,
    0xc0007338,
    (uint8_t *)bes1400_patch28_ins_data_t5
};//no disconnect evt
#endif /* PATCH_IN_EXTRA_SPACE */


/////1400 t5 patch
static const uint32_t best1400_t5_ins_patch_config[] =
{
    29,
    (uint32_t)&bes1400_t5_ins_patch0,
    (uint32_t)&bes1400_t5_ins_patch1,
    (uint32_t)&bes1400_t5_ins_patch2,
    (uint32_t)&bes1400_t5_ins_patch3,
    (uint32_t)&bes1400_t5_ins_patch4,
    (uint32_t)&bes1400_t5_ins_patch5,
    (uint32_t)&bes1400_t5_ins_patch6,
    (uint32_t)&bes1400_t5_ins_patch7,
    (uint32_t)&bes1400_t5_ins_patch8,
    (uint32_t)&bes1400_t5_ins_patch9,
    (uint32_t)&bes1400_t5_ins_patch10,
    (uint32_t)&bes1400_t5_ins_patch11,
    (uint32_t)&bes1400_t5_ins_patch12,
    (uint32_t)&bes1400_t5_ins_patch13,
    (uint32_t)&bes1400_t5_ins_patch14,
    (uint32_t)&bes1400_t5_ins_patch15,
    (uint32_t)&bes1400_t5_ins_patch16,
    (uint32_t)&bes1400_t5_ins_patch17,
    (uint32_t)&bes1400_t5_ins_patch18,
    (uint32_t)&bes1400_t5_ins_patch19,
    (uint32_t)&bes1400_t5_ins_patch20,
    (uint32_t)&bes1400_t5_ins_patch21,
    (uint32_t)&bes1400_t5_ins_patch22,
    (uint32_t)&bes1400_t5_ins_patch23,
    (uint32_t)&bes1400_t5_ins_patch24,
    (uint32_t)&bes1400_t5_ins_patch25,
    (uint32_t)&bes1400_t5_ins_patch26,
    (uint32_t)&bes1400_t5_ins_patch27,
    (uint32_t)&bes1400_t5_ins_patch28,
};

#else
/***************************************************************************
 *
 * instruction patch Information
 *
 * BT ROM Chip Version:1400 T5 (CHIP ID=2)
 *
 *  IBRT function
 *
 ****************************************************************************/
//#define ECC_LL_MONITOR

const uint32_t bes1400_patch0_ibrt_ins_data_t5[] =
{
    /*6d00*/ 0x4673b408,
    /*6d04*/ 0x0f20f013,
    /*6d08*/ 0xbc08d002,
    /*6d0c*/ 0xb9e5f600, //jump a00070da
    /*6d10*/ 0xbf00bc08,
    /*6d14*/ 0x4b014699,
    /*6d18*/ 0xbaeaf600, //jump a00072f0
    /*6d1c*/ 0xc0000050,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch0_ibrt_ins_data_t5),
    0x000072ec,
    0xbd08f1ff, //jump a00072ec -> a0206d00
    0xc0006d00,
    (uint8_t *)bes1400_patch0_ibrt_ins_data_t5
};//assert warn, assert param

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0001ff2c,
    0xb81af000, //jump a001ff2c -> a001ff64
    0,
    NULL
};//KE_MSG_HANDLER(lc_op_loc_unsniff_req)


#if 0
const uint32_t bes1400_patch2_ibrt_ins_data_t5[] =
{
    0x460a4603,/*6ae8*/
    0x46194802,
    0xfcb6f5fc,
    0xbf00bd38,
    0xc0006afc,/*6af8*/
    0x73617243,
    0x73252c68,
    0x0064252c,/*6b04*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch2_ibrt_ins_data_t5),
    0x00007018,
    0xbd66f1ff,
    0xc0006ae8,
    (uint8_t *)bes1400_patch2_ibrt_ins_data_t5
};//"Crash,%s,%d", file, line
#else
#if 0
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x00007024,
    0xbf00e011,
    0,
    NULL
};//dont call rwip_assert_err when assert error
#else
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x0002d8b0, //0xa002d8b0
    0xbf00bf00,
    0,
    NULL
};//ld_acl_rsw_frm_isr
#endif
#endif

const uint32_t bes1400_patch3_ibrt_ins_data_t5[] =
{
    0xd1052800,/*6900*/
    0x000cf1aa,
    0xf97af5fa,
    0xbc48f61a,
    0xbbd2f61a,/*6910*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch3_ibrt_ins_data_t5),
    0x000210b4,
    0xbc24f1e5,
    0xc0006900,
    (uint8_t *)bes1400_patch3_ibrt_ins_data_t5
};//lmp filter free heap

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x0002be90,
    0xbd702000,
    0,
    NULL
}; //ld_check_if_need_protect_sniff retrun 0

#if 0
const uint32_t bes1400_patch5_ibrt_ins_data_t5[] =
{
    /*6950*/ 0x0f01f013,
    /*6954*/ 0x4b06d00a,
    /*6958*/ 0x104ff893,
    /*695c*/ 0xd8052902,
    /*6960*/ 0x2a027a9a,
    /*6964*/ 0xbf00d802,
    /*6968*/ 0xbc73f632,
    /*696c*/ 0xbf00bd08,
    /*6970*/ 0xc00063c8,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch5_ibrt_ins_data_t5),
    0x0003924c,
    0xbb80f1cd, // jump a00384dc -> a0206950
    0xc0006950,
    (uint8_t *)bes1400_patch5_ibrt_ins_data_t5
}; //protect link id when send bitoffset

const uint32_t bes1400_patch6_ibrt_ins_data_t5[] =
{
    /*6978*/ 0x5023f852,
    /*697c*/ 0xd0012d00,
    /*6980*/ 0xbc04f611,// jump a0206980 -> a001818c
    /*6984*/ 0xbd382000,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch6_ibrt_ins_data_t5),
    0x00018188,
    0xbbf6f1ee, // jump a0018188 -> a0206978
    0xc0006978,
    (uint8_t *)bes1400_patch6_ibrt_ins_data_t5
};//protect null ptr when send afh
#else
const uint32_t bes1400_patch5_ibrt_ins_data_t5[] =
{
    /*694c*/ 0xf632b1c0,
    /*6950*/ 0xb1a8fc1b,
    /*6954*/ 0x70b2f898,
    /*6958*/ 0xfc2af632,
    /*695c*/ 0xd10d4287,
    /*6960*/ 0xfbfaf634,
    /*6964*/ 0xbf142804,
    /*6968*/ 0x22012200,
    /*696c*/ 0x46294640,
    /*6970*/ 0xf6259b03,
    /*6974*/ 0x2001fae5,
    /*6978*/ 0xbecef625,
    /*697c*/ 0xbc6cf625,
    /*6980*/ 0xbcc9f625,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch5_ibrt_ins_data_t5),
    0x0002c230,
    0xbb8cf1da, // jump a002c230 -> a020694c
    0xc000694c,
    (uint8_t *)bes1400_patch5_ibrt_ins_data_t5
};//ld_cal_timestamp

#if 1
const uint32_t bes1400_patch6_ibrt_ins_data_t5[] =
{
    0x79ab4688,
    0x6383ea6f,
    0x4578f023,
    0xf8d39b09,
    0x9906308c,
    0x402b440b,
    0xbf004641,
    0xbb87f62c,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch6_ibrt_ins_data_t5),
    0x000330f4,
    0xbc6ef1d3, // jump a00330f4 -> a02069d4
    0xc00069d4,
    (uint8_t *)bes1400_patch6_ibrt_ins_data_t5
};//sco param init
#else
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    0,
    0x00007010, //a0007010
    0xb9bef000, //jump a0007390
    0,
    NULL,
};//DEF_IRQHandler
#endif

#endif
const uint32_t bes1400_patch7_ibrt_ins_data_t5[] =
{
    /*7138*/ 0x28024604,
    /*713c*/ 0xf89ad812,
    /*7140*/ 0x28020046,
    /*7144*/ 0xf610d803,
    /*7148*/ 0x2800fb1b,
    /*714c*/ 0x2112d106,
    /*7150*/ 0xf61b20ff,
    /*7154*/ 0x2000fec9,
    /*7158*/ 0xbb0af633,
    /*715c*/ 0xbf004620,
    /*7160*/ 0xba4ef633,
    /*7164*/ 0xbad8f633,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch7_ibrt_ins_data_t5),
    0x0003a5f8,
    0xbd9ef1cc, // jump a003a5f8 -> a0207138
    0xc0007138,
    (uint8_t *)bes1400_patch7_ibrt_ins_data_t5
};//start simu link check linkid

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x0002e720,
    0xf8a62308,
    0,
    NULL,
};//ld_acl_start t_poll

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x0002d82c,
    0x2b006013,
    0,
    NULL,
};//ld_acl_rsw_rx

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    0,
    0x0002fe60,
    0xbf00e001,
    0,
    NULL,
};//ld_acl_flow_on

#ifdef ECC_LL_MONITOR
////patch 11 12 13 used for ecc count by set it to ll monitor parameter
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x0002dc94,
    0x7000f400,
    0,
    NULL
};////wrong flag

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x0002dc98,
    0x2800bf00,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch13 =
{
    13,
    BTDRV_PATCH_ACT,
    0,
    0x0002f78c,
    0x46288113,
    0,
    NULL
};
#else
const uint32_t bes1400_patch11_ibrt_ins_data_t5[] =
{
    0x0034f104,
    0x220a4649,
    0xfd86f644,
    0xf632b940,
    0x2801fa39,
    0xf632d104,
    0x2800fb0d,
    0xbf00d104,
    0x00c5f894,
    0xb97cf62b,
    0xb9eaf62b,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch11_ibrt_ins_data_t5),
    0x00031e70,
    0xbe70f1d4,
    0xc0006b54,
    (uint8_t *)bes1400_patch11_ibrt_ins_data_t5
};//filter afh map

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x0002a41c,
    0xbf000f01,
    0,
    NULL
};//mode == SYNC2_SCO_MODE

const uint32_t bes1400_patch13_ibrt_ins_data_t5[] =
{
    0xfeecf5f8,
    0xbf00bf00,
    0x8310f3ef,
    0x0401f013,
    0xb672d108,
    0xff22f5f8,
    0xbf30b110,
    0xd1ef2c00,
    0xe7edb662,
    0xff1af5f8,
    0xd1f62800,
    0xbf00e7e8,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch13 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch13_ibrt_ins_data_t5),
    0x0000721c,
    0xb870f200,
    0xc0007300,
    (uint8_t *)bes1400_patch13_ibrt_ins_data_t5
};
#endif

const uint32_t bes1400_patch14_ibrt_ins_data_t5[] =
{
    /*698c*/ 0x4604b510,
    /*6990*/ 0xfc0ef632,
    /*6994*/ 0xd00542a0,
    /*6998*/ 0x7a984b03,
    /*699c*/ 0xbf181b00,
    /*69a0*/ 0xbd102001,
    /*69a4*/ 0xbd102000,
    /*69a8*/ 0xc00063c8,
    /*69ac*/ 0x461f7a9e,
    /*69b0*/ 0x46104625,
    /*69b4*/ 0xffeaf7ff,
    /*69b8*/ 0xd1072800,
    /*69bc*/ 0x46392004,
    /*69c0*/ 0xfc74f634,
    /*69c4*/ 0x462c2003,
    /*69c8*/ 0xbd68f634,
    /*69cc*/ 0xbf00bdf8,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch14 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch14_ibrt_ins_data_t5),
    0x0003b498,
    0xba88f1cb, // jump a003b498 -> a02069ac
    0xc000698c,
    (uint8_t *)bes1400_patch14_ibrt_ins_data_t5
};//protect idx when snoop disconnect

#if 0
const uint32_t bes1400_patch15_ibrt_ins_data_t5[] =
{
    /*69D0*/ 0xbf00669a,
    /*69D4*/ 0x66da466a,//mov   r2, sp; str   r2, [r3, #108]
    /*69D8*/ 0x8208f3ef,//mrs   r2, MSP
    /*69DC*/ 0xbf00671a,//str   r2, [r3, #112]
    /*69E0*/ 0x8209f3ef,//mrs   r2, PSP
    /*69E4*/ 0xbf00675a,//str   r2, [r3, #116]
    /*69E8*/ 0xbf00bf00,
    /*69EC*/ 0xbf00bf00,
    /*69F0*/ 0xbf00bf00,
    /*69F4*/ 0xbf00bf00,
    /*69F8*/ 0xbf00bf00,
    /*69FC*/ 0xbf00bf00,
    /*6A00*/ 0xbf00bf00,
    /*6A04*/ 0xbf00bf00,
    /*6A08*/ 0xbf00bf00,
    /*6A0C*/ 0xbf00bf00,
    /*6A10*/ 0xbcb0f600,//jump a0206a10 -> a0007374
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch15 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch15_ibrt_ins_data_t5),
    0x00007360,
    0xbb36f1ff, // jump a0007360 -> a02069d0
    0xc00069d0,
    (uint8_t *)bes1400_patch15_ibrt_ins_data_t5
};//bt_assert_crash_dump
#else

const uint32_t bes1400_patch15_ibrt_ins_data_t5[] =
{
    0xf885b2db,/*7338*/
    0xf89530bd,
    0x2a0220b4,
    0xb98bd114,
    0xf8534b0a,/*7348*/
    0xf6430024,
    0xf895fd15,
    0x2b0430b5,
    0x4620d103,/*7358*/
    0xf6232116,
    0x4620fc61,
    0xf6242116,
    0xbf00f961,/*7368*/
    0xbc6bf62d,
    0xbc53f62d,
    0xc0000998,/*7374*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch15 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch15_ibrt_ins_data_t5),
    0x00034c14,
    0xbb90f1d2,
    0xc0007338,
    (uint8_t *)bes1400_patch15_ibrt_ins_data_t5
};//no disconnect evt
#endif

const uint32_t bes1400_patch16_ibrt_ins_data_t5[] =
{
    0x71ab2300,
    0x0000f8d9,
    0x380cb110,
    0xf96cf5fa,
    0xbf002300,
    0xbce2f608,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch16 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch16_ibrt_ins_data_t5),
    0x0000f2f0,
    0xbb12f1f7,
    0xc0006918,
    (uint8_t *)bes1400_patch16_ibrt_ins_data_t5
};//create connection free msg

#if 0
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    0,
    0x00007010, //a0007010
    0xb9bef000, //jump a0007390
    0,
    NULL,
};//DEF_IRQHandler
#elif 1
const uint32_t bes1400_patch17_ibrt_ins_data_t5[] =
{
    0x4605b538,/*72b4*/
    0xfe76f631,
    0x7a834604,
    0x2f15ebb3,
    0x7803d10a,/*72c4*/
    0xd1072b02,
    0xff5cf631,
    0xf894b120,
    0x2101004f,/*72d4*/
    0xfc78f61d,
    0xbf00bd38,
    0x3312f886,
    0xbf004620,/*72e4*/
    0xffe4f7ff,
    0xbcfaf614,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch17_ibrt_ins_data_t5),
    0x0001bce0,
    0xbafef1eb,
    0xc00072b4,
    (uint8_t *)bes1400_patch17_ibrt_ins_data_t5
};//send unsniff cmd

#else
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    0,
    0x0002e674,
    0x53faf44f,
    0,
    NULL,
};//lc start lsto 5s
#endif

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch18 =
{
    18,
    BTDRV_PATCH_ACT,
    0,
    0x0002f264,
    0xbf00e001,
    0,
    NULL,
};//close tx seq force set after start IBRT

const uint32_t bes1400_patch19_ibrt_ins_data_t5[] =
{
    0xd01e0fdb,/*6a2c*/
    0x6307f44f,
    0xf304fb03,
    0x48168f6a,
    0x93011810,
    0x49154418,
    0xfe22f644,
    0x58fb4b14,
    0x4301f3c3,
    0xf1028f2a,
    0xf5024250,
    0xf2021205,
    0x8f6942fc,
    0x0241eb02,
    0x440a9901,
    0x600a490d,
    0xf44fe010,
    0xfb036307,
    0x9301f304,
    0x18184806,
    0x8f6a4906,
    0xfe04f644,
    0x58fb4b05,
    0x4381f3c3,
    0xb9a9f62e,
    0xb9aff62e,
    0xd02144fc,
    0xc000593c,
    0xd0220144,
    0xc000005c,/*6a9c*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch19 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch19_ibrt_ins_data_t5),
    0x00034d80,
    0xbe54f1d1,
    0xc0006a2c,
    (uint8_t *)bes1400_patch19_ibrt_ins_data_t5
};//ld_sco_frm_isr

const uint32_t bes1400_patch20_ibrt_ins_data_t5[] =
{
    0xbf002004,/*6aa4*/
    0xfb5cf634,/*6aa8*/
    0xb8c4f618,/*6aac*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch20 =
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch20_ibrt_ins_data_t5),
    0x0001ec10,
    0xbf48f1e7,
    0xc0006aa4,
    (uint8_t *)bes1400_patch20_ibrt_ins_data_t5
};//ibrt sco status update

const uint32_t bes1400_patch21_ibrt_ins_data_t5[] =
{
    0xf6414631,
    0x4620fc1b,
    0xf6064631,
    0x4a03faaf,
    0xf4436813,
    0x60130380,
    0xbdddf633,
    0xd0220468,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch21 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch21_ibrt_ins_data_t5),
    0x0003a684,
    0xba16f1cc, // jump a003a684 -> a0206ab4
    0xc0006ab4,
    (uint8_t *)bes1400_patch21_ibrt_ins_data_t5
};//register address at LM layer and open fa en register

#if 0
const uint32_t bes1400_patch22_ibrt_ins_data_t5[] =
{
    0x4681b083,/*6b30*/
    0x4b022200,
    0x2020f843,
    0xbddcf623,
    0xc0006b44,
    0x00000000,
    0x00000000,
    0x00000000,/*6b4c*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch22 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch22_ibrt_ins_data_t5),
    0x0002a6f4,
    0xba1cf1dc,
    0xc0006b30,
    (uint8_t *)bes1400_patch22_ibrt_ins_data_t5
};//sync clear count
#else
const uint32_t bes1400_patch22_ibrt_ins_data_t5[] =
{
    0x4681b083,
    0x4b042200,
    0x2020f843,
    0x601a4b0d,
    0x601a4b0d,
    0xb8b0f623,
    0xc000759c,
    0x00000000,//link_no_sync_count
    0x00000000,
    0x00000000,
    0x00000018,//link_agc_thd
    0x00000000, //rssi_store_tws
    0x00000006, //rssi_tws_step
    0x00000000, //rssi_store_mobile
    0x00000006, //rssi_mobile_step
    0x0000009c, //rssi_min_value_tws
    0x0000009c, //rssi_min_value_mobile
    0xc00075ac, //rssi_store_tws
    0xc00075b4, //rssi_store_mobile
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch22 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch22_ibrt_ins_data_t5),
    0x0002a6f4,
    0xbf44f1dc,
    0xc0007580,
    (uint8_t *)bes1400_patch22_ibrt_ins_data_t5
};//sync clear count
#endif

#if 0
const uint32_t bes1400_patch23_ibrt_ins_data_t5[] =
{
    0x6020f853,/*6b50*/
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
    0xbce4f623,
    0xc0006b44,
    0xc0006b94,
    0x00000050,
    0xc00041b4,/*6b98*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch23 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch23_ibrt_ins_data_t5),
    0x0002a550,
    0xbafef1dc,
    0xc0006b50,
    (uint8_t *)bes1400_patch23_ibrt_ins_data_t5
};////swagc no sync
#elif 0

const uint32_t bes1400_patch23_ibrt_ins_data_t5[] =
{
    0xf6324620,
    0xb480fce9,
    0xfb07276e,
    0x4a1ff704,
    0xf837bf00,
    0xf4133002,
    0xd12b4f00,
    0xf8524a17,
    0x33013024,
    0x3024f842,
    0x68124a15,
    0xd8124293,
    0x3084f8d5,
    0xf0221af2,
    0xf1b14178,
    0xd9066f80,
    0xf0231b9b,
    0x425b4378,
    0xdc042b64,
    0xf022e012,
    0x2a644278,
    0x2300dd0e,
    0xf8424a08,
    0xb0823024,
    0x92004a09,
    0xc020f8d2,
//    0x0036f06f,//-55
    0x004cf06f,//-77
    0x22014621,
    0xb00247e0,
    0xbf00bc80,
    0xbbeff62d,
    0xc0006b44,
    0xc0007204,
    0x0000003c,
    0xc00041b4,
    0xd02111f8,//720c
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch23 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch23_ibrt_ins_data_t5),
    0x000349d4,
    0xbbd4f1d2,
    0xc0007180,
    (uint8_t *)bes1400_patch23_ibrt_ins_data_t5
};////swagc no sync

#else
const uint32_t bes1400_patch23_ibrt_ins_data_t5[] =
{
    0xfb03236e,
    0x4a3ef300,
    0xf4135a9b,
    0xd1764f00,
    0xb082b570,
    0xf8534b3b,
    0x4a3b4020,
    0x3020f852,
    0xf8423301,
    0x4a393020,
    0x42936812,
    0xf8d4d812,
    0x1aca3084,
    0x4578f022,
    0x6f80f1b5,
    0x1a5bd906,
    0x4378f023,
    0x2b64425b,
    0xe04ddc04,
    0x4278f022,
    0xdd492a64,
    0x681b4b2e,
    0xd11d4298,
    0xf9934b2d,
    0xb93b3000,
    0x30acf9b4,
    0x20aef894,
    0xf3f2fb93,
    0x70134a28,
    0x78134a27,
    0x78094927,
    0xb2db1a5b,
    0x4a267013,
    0xb25b7812,
    0x428bb251,
    0x4b21bfbc,
    0x4b20701a,
    0xe01c781e,
    0xf9934b21,
    0xb93b3000,
    0x30acf9b4,
    0x20aef894,
    0xf3f2fb93,
    0x70134a1c,
    0x78134a1b,
    0x7809491b,
    0xb2db1a5b,
    0x4a1a7013,
    0xb25b7812,
    0x428bb251,
    0x4b15bfbc,
    0x4b14701a,
    0x4601781e,
    0x4a0c2300,
    0x3020f842,
    0x92004a13,
    0xb2706a15,
    0x47a82201,
    0x30aef894,
    0xd9042b95,
    0xf8a42300,
    0xf88430ac,
    0xb00230ae,
    0x4770bd70,
    0xd02111f8,
    0xc0000998,
    0xc000759c,
    0xc00075a8,
    0xc0006e2c,
    0xc00075ac,
    0xc00075b0,
    0xc00075bc,
    0xc00075b4,
    0xc00075b8,
    0xc00075c0,
    0xc00041b4,
    0xf6324620,
    0x4620f9f7,
    0xf7ff4631,
    0xbf00ff61,
    0xb931f62d
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch23 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch23_ibrt_ins_data_t5),
    0x000349d4,
    0xbec6f1d2,
    0xc0007634,
    (uint8_t *)bes1400_patch23_ibrt_ins_data_t5
};////swagc no sync
#endif

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch24 =
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x0000fe0c,
    0xf029bf00,
    0,
    NULL,
};//filter tws link lc_sco_ind ke message

const uint32_t bes1400_patch25_ibrt_ins_data_t5[] =
{
    /*6f90*/ 0xbf009904,
    /*6f94*/ 0x4b12b470,
    /*6f98*/ 0x010e5c5a,
    /*6f9c*/ 0x19734d11,
    /*6fa0*/ 0x78244c11,
    /*6fa4*/ 0xd20342a2,
    /*6fa8*/ 0x32015498,
    /*6fac*/ 0xe001b2d2,
    /*6fb0*/ 0x22015570,
    /*6fb4*/ 0x5442480a,
    /*6fb8*/ 0x461ab14c,
    /*6fbc*/ 0x23001919,
    /*6fc0*/ 0x0b01f912,
    /*6fc4*/ 0xbf004403,
    /*6fc8*/ 0xd1f9428a,
    /*6fcc*/ 0x2300e000,
    /*6fd0*/ 0xf0f4fb93,
    /*6fd4*/ 0xbc70b240,
    /*6fd8*/ 0x22019904,
    /*6fdc*/ 0xbb8cf628, //jump a002f6f8
    /*6fe0*/ 0xc0006ff0, //addr of rssi_idx_for_agc
    /*6fe4*/ 0xc0006ff4, //addr of rssi_set_for_agc
    /*6fe8*/ 0xc0006fec, //addr of N
    /*6fec*/ 0x00000005, //N : 1 ~ 16
    /*6ff0*/ 0x00000000, //rssi_idx_for_agc[3]
    /*6ff4*/ 0xbabababa, //rssi_set_for_agc[3*16]
    /*6ff8*/ 0xbabababa,
    /*6ffc*/ 0xbabababa,
    /*7000*/ 0xbabababa,
    /*7004*/ 0xbabababa,
    /*7008*/ 0xbabababa,
    /*700c*/ 0xbabababa,
    /*7010*/ 0xbabababa,
    /*7014*/ 0xbabababa,
    /*7018*/ 0xbabababa,
    /*701c*/ 0xbabababa,
    /*7020*/ 0xbabababa,
    /*7024*/ 0xbabababa,
    /*7028*/ 0xbabababa,
    /*702c*/ 0xbabababa,
};//0xc0006f90--->0xc0007074

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch25 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch25_ibrt_ins_data_t5),
    0x0002f6f4,
    0xbc4cf1d7, //jump a002f6f4 -> a0206f90
    0xc0006f90,
    (uint8_t *)bes1400_patch25_ibrt_ins_data_t5
}; //ld_acl_rx() average rssi

#if 0
//ECC patch 1
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch26 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00034790,
    0xbf00bf00,
    0,
    NULL,
};//ld_acl_sniff_frm_isr ld_get_rx_isr_counter  ignore
#else
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch26 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00029d50,
    0x3300f443,
    0,
    NULL,
};//bt_e_scoltcntl_retxnb_setf
#endif

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch27 =
{
    27,
    BTDRV_PATCH_ACT,
    0,
    0x000236e0,
    0xf0172004,
    0,
    NULL,
};//LC_WAIT_EXCHANGE_SCO_STATUS_CFM

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch28 =
{
    28,
    BTDRV_PATCH_ACT,
    0,
    0x00039144,
    0x47702001,
    0,
    NULL
};//ld_get_sniffer_enter_sniff_flag() forece return 1

#if 0
const uint32_t bes1400_patch29_ibrt_ins_data_t5[] =
{
    /*6ba0*/ 0xbf0080c6,
    /*6ba4*/ 0x71022200,
    /*6ba8*/ 0xbb2af619, //jump a0020200
};
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch29 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch29_ibrt_ins_data_t5),
    0x000201f8,
    0xbcd2f1e6, //jump a00201f8 -> a0206ba0
    0xc0006ba0,
    (uint8_t *)bes1400_patch29_ibrt_ins_data_t5
};//KE_MSG_HANDLER(lc_op_loc_sniff_req, void)
#else

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch29 =
{
    29,
    BTDRV_PATCH_ACT,
    0,
    0x00020024,
    0xe00dbf00,
    0,
    NULL,
};//detach directly
#endif

const uint32_t bes1400_patch30_ibrt_ins_data_t5[] =
{
    /*6bac*/ 0x46286825,
    /*6bb0*/ 0x303ef895,
    /*6bb4*/ 0xd0012b02,
    /*6bb8*/ 0xbda2f622, //jump a0029700
    /*6bbc*/ 0xbf002300,
    /*6bc0*/ 0x303ef885,
    /*6bc4*/ 0xfea6f622, //call a0029914
    /*6bc8*/ 0xbdc1f622, //jump a002974e
};
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch30 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch30_ibrt_ins_data_t5),
    0x000296fc,
    0xba56f1dd, //jump a00296fc -> a0206bac
    0xc0006bac,
    (uint8_t *)bes1400_patch30_ibrt_ins_data_t5
};//ld_pscan_frm_cbk

const uint32_t bes1400_patch31_ibrt_ins_data_t5[] =
{
    0x7021f853,
    0xb12fb136,
    0x017ff000,
    0xf8534b06,
    0xb93b3021,
    0x210c20ff,
    0xfda8f61b,
    0xbf002004,
    0xbffcf62b,
    0xbf76f62b,
    0xc0005cd8,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch31 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch31_ibrt_ins_data_t5),
    0x0003328c,
    0xb876f1d4, // jump a003328c -> a020737c
    0xc000737c,
    (uint8_t *)bes1400_patch31_ibrt_ins_data_t5
}; //protect null ptr when acl tws switch

const uint32_t bes1400_patch32_ibrt_ins_data_t5[] =
{
    /*6be8*/ 0x46514620,
    /*6bec*/ 0x4b09464a,
    /*6bf0*/ 0x3022f853,
    /*6bf4*/ 0xd10b2901,
    /*6bf8*/ 0x0184f103,
    /*6bfc*/ 0xbf002206,
    /*6c00*/ 0xfd34f644,
    /*6c04*/ 0xbf00b920,
    /*6c08*/ 0x0802f05f,
    /*6c0c*/ 0xba4af641, //jump a00480a4
    /*6c10*/ 0xbaf6f641, //jump a0048200
    /*6c14*/ 0xc0005cd8,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch32 =
{
    32,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch32_ibrt_ins_data_t5),
    0x00048008,
    0xbdeef1be, // jump a0048008 -> a0206be8
    0xc0006be8,
    (uint8_t *)bes1400_patch32_ibrt_ins_data_t5
};//hci_evt_filter_con_check

const uint32_t bes1400_patch33_ibrt_ins_data_t5[] =
{
    /*6c1c*/ 0x0024f855,
    /*6c20*/ 0xbf003038,
    /*6c24*/ 0xfd9ef643,
    /*6c28*/ 0x0024f855,
    /*6c2c*/ 0xbe9ef60c,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch33 =
{
    33,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch33_ibrt_ins_data_t5),
    0x00013968,
    0xb958f1f3, // jump a0013968 -> a0206c1c
    0xc0006c1c,
    (uint8_t *)bes1400_patch33_ibrt_ins_data_t5
};//ea_interval_remove

const uint32_t bes1400_patch34_ibrt_ins_data_t5[] =
{
    /*6c30*/ 0xf632d108,
    /*6c34*/ 0x2100f9e3,
    /*6c38*/ 0xf802f634,
    /*6c3c*/ 0xfa8cf634,
    /*6c40*/ 0xbb8bf5fb,
    /*6c44*/ 0xbbc3f5fb,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch34 =
{
    34,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch34_ibrt_ins_data_t5),
    0x00002354,
    0xbc6cf204, // jump a0002354 -> a0206c30
    0xc0006c30,
    (uint8_t *)bes1400_patch34_ibrt_ins_data_t5
};//close fastack when stop ibrt

const uint32_t bes1400_patch35_ibrt_ins_data_t5[] =
{
    /*6c48*/ 0x4b0a0a02,
    /*6c4c*/ 0x3022f853,
    /*6c50*/ 0xb500b173,
    /*6c54*/ 0x4601b083,
    /*6c58*/ 0xf88d230f,
    /*6c5c*/ 0x23133004,
    /*6c60*/ 0x3005f88d,
    /*6c64*/ 0xf616a801,
    /*6c68*/ 0xb003fbc1,
    /*6c6c*/ 0xfb04f85d,
    /*6c70*/ 0xbf004770,
    /*6c74*/ 0xc0005cd8,
    /*6c78*/ 0xbf000230,
    /*6c7c*/ 0x0001f040,
    /*6c80*/ 0xffe2f7ff,
    /*6c84*/ 0xbc5df634
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch35 =
{
    35,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch35_ibrt_ins_data_t5),
    0x0003b538,
    0xbb9ef1cb, // jump a003b538 -> a0206c78
    0xc0006c48,
    (uint8_t *)bes1400_patch35_ibrt_ins_data_t5
};//snoop disconnect lc_release

#if 0
//ECC patch 2
const uint32_t bes1400_patch36_ibrt_ins_data_t5[] =
{
    /*6c88*/ 0x701c4b03,
    /*6c8c*/ 0x78134a01,
    /*6c90*/ 0xbcfcf62d,
    /*6c94*/ 0xc0000971,
    /*6c98*/ 0xc000032a,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch36 =
{
    36,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch36_ibrt_ins_data_t5),
    0x00034688,
    0xbafef1d2, // jump a0034688 -> a0206c88
    0xc0006c88,
    (uint8_t *)bes1400_patch36_ibrt_ins_data_t5
};//ld_acl_rx_isr set_ecc_recheck_link_id
#else
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch36 =
{
    36,
    BTDRV_PATCH_ACT,
    0,
    0x0002ddc0,
    0x4a07bf00,
    0,
    NULL,
};// 1st ibrt ok, ignore sniffer role
#endif

const uint32_t bes1400_patch37_ibrt_ins_data_t5[] =
{
    0x4605b538,/*6ca0*/
    0xf643460c,
    0x6a00fc79,
    0xd2034285,
    0x28061b40,/*6cb0*/
    0x2402bf98,
    0xb9fdf627,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch37 =
{
    37,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch37_ibrt_ins_data_t5),
    0x0002e0b0,
    0xbdf6f1d8,
    0xc0006ca0,
    (uint8_t *)bes1400_patch37_ibrt_ins_data_t5
};//decrease retx

#if 0
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch38 =
{
    38,
    BTDRV_PATCH_ACT,
    0,
    0x00000878,
    0x2300bf00,
    0,
    NULL
};//SLEEP EN
#else
const uint32_t bes1400_patch38_ibrt_ins_data_t5[] =
{
    0x4604b510,/*6cc0*/
    0xf6104608,
    0x2c03fd25,
    0x2803d001,
    0x200cd104,/*6cd0*/
    0xfa4ef61c,
    0xbd102001,
    0xbd102000,
    0xfd18f610,/*6ce0*/
    0xf1044605,
    0x46280145,
    0xffe8f7ff,
    0xd1012800,/*6cf0*/
    0xbeb7f633,
    0xbf19f633,/*6cf8*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch38 =
{
    38,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch38_ibrt_ins_data_t5),
    0x0003aa60,
    0xb93ef1cc, // jump a003aa60 -> a0206cc0
    0xc0006cc0,
    (uint8_t *)bes1400_patch38_ibrt_ins_data_t5
};//set env
#endif

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch39 =
{
    39,
    BTDRV_PATCH_ACT,
    0,
    0x0003b450,
    0xbf00bd08,
    0,
    NULL
};//do not send ld_sniffer_connect_sco_forward

const uint32_t bes1400_patch40_ibrt_ins_data_t5[] =
{
    0x2a0a0c92,/*1400 is not same as 1302 and 1402*/
    0x220abf28,/*1400 is not same as 1302 and 1402*/
    0xbf004691,
    0xbc44f623,/*6d40*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch40 =
{
    40,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch40_ibrt_ins_data_t5),
    0x0002a5c4,
    0xbbb4f1dc,
    0xc0006d30,
    (uint8_t *)bes1400_patch40_ibrt_ins_data_t5
};//max_drift limit

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch41 =
{
    41,
    BTDRV_PATCH_ACT,
    0,
    0x0003acd8,
    0xbf00e005,
    0,
    NULL
};///acl tx silence

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch42 =
{
    42,
    BTDRV_PATCH_ACT,
    0,
    0x00033668,
    0xbf00e001,
    0,
    NULL
};///do not clear acl env during sco stop

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch43 =
{
    43,
    BTDRV_PATCH_ACT,
    0,
    0x0002a62c,
    0xe001021a,
    0,
    NULL
};//rx window adjust for start ibrt

#if defined(SYNC_BT_CTLR_PROFILE)
const uint32_t bes1400_patch44_ibrt_ins_data_t5[] =
{
    /*6d24*/ 0xbf004628,
    /*6d28*/ 0xfff8f633,
    /*6d2c*/ 0xb8f7f611,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch44 =
{
    44,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch44_ibrt_ins_data_t5),
    0x00017f10,
    0xbf08f1ee, // jump a0017f10 -> a0206d24
    0xc0006d24,
    (uint8_t *)bes1400_patch44_ibrt_ins_data_t5
};//send profile via BLE
#else

const uint32_t bes1400_patch44_ibrt_ins_data_t5[] =
{
    0xd8092b01,/*70d0*/
    0xfb03236e,
    0x4a04f308,
    0xf4135a9b,
    0xd1016f00,/*70e0*/
    0xb919f629,
    0xb9aef629,
    0xd0211200,/*70ec*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch44 =
{
    44,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch44_ibrt_ins_data_t5),
    0x00030314,
    0xbedcf1d6,
    0xc00070d0,
    (uint8_t *)bes1400_patch44_ibrt_ins_data_t5
};//lmp tx for tx desc error
#endif

#if 0
const uint32_t bes1400_patch45_ibrt_ins_data_t5[] =
{
    0x20aef89b,/*6d50*/
    0xd1052aa0,
    0xf88b2200,
    0xf8ab20ae,
    0xe01320ac,/*6d60*/
    0x68194b0b,
    0xf3f1fbb2,
    0x2313fb01,
    0x4b07b963,/*6d70*/
    0x00acf9bb,
    0x6a1e9300,
    0xfb90bf00,
    0x9904f0f2,/*6d80*/
    0x2201b240,
    0x47b02300,
    0xbcb6f628,
    0xc00041b4,/*6d90*/
    0xc0006d98,
    0x0000000a,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch45 =
{
    45,
    BTDRV_PATCH_INACT,
    sizeof(bes1400_patch45_ibrt_ins_data_t5),
    0x0002f6ec,
    0xbb30f1d7,
    0xc0006d50,
    (uint8_t *)bes1400_patch45_ibrt_ins_data_t5
};//RSSI AVERAGE 10
#else
const uint32_t bes1400_patch45_ibrt_ins_data_t5[] =
{
    0x4a033304,
    0xbf008153,
    0x12c4f640,
    0xb9bdf632,
    0xc00061b8,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch45 =
{
    45,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch45_ibrt_ins_data_t5),
    0x00038cb0,
    0xbe3ef1cd,
    0xc0006930,
    (uint8_t *)bes1400_patch45_ibrt_ins_data_t5
};//pscan duration
#endif

const uint32_t bes1400_patch46_ibrt_ins_data_t5[] =
{
    /*6d9c*/ 0x4b179a0b,
    /*6da0*/ 0xd023429a,
    /*6da4*/ 0x20b5f894,
    /*6da8*/ 0xb9fab2d2,
    /*6dac*/ 0xf9b4b22e,
    /*6db0*/ 0x1b9b3098,
    /*6db4*/ 0xf9b24a0f,
    /*6db8*/ 0x42932000,
    /*6dbc*/ 0x4251da05,
    /*6dc0*/ 0xdd02428b,
    /*6dc4*/ 0x7088f8d4,
    /*6dc8*/ 0x4293e00f,
    /*6dcc*/ 0xf8d4dd05,
    /*6dd0*/ 0x3f017088,
    /*6dd4*/ 0x4778f027,
    /*6dd8*/ 0x4252e007,
    /*6ddc*/ 0xda044293,
    /*6de0*/ 0x7088f8d4,
    /*6de4*/ 0xf0273701,
    /*6de8*/ 0xbf004778,
    /*6dec*/ 0x708cf8c4,
    /*6df0*/ 0xbce4f623, // jump a002a7bc
    /*6df4*/ 0xc0006df8,
    /*6df8*/ 0x00000032, // 50
    /*6dfc*/ 0xa003477b, // ld_acl_sniff_frm_isr lr
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch46 =
{
    46,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch46_ibrt_ins_data_t5),
    0x0002a7b8,
    0xbaf0f1dc, // jump a002a7b8 -> a0206d9c
    0xc0006d9c,
    (uint8_t *)bes1400_patch46_ibrt_ins_data_t5
};//ld_acl_rx_sync clk_off cal active mode

const uint32_t bes1400_patch47_ibrt_ins_data_t5[] =
{
    0xd10b2800,/*6e00*/
    0x0f00f1ba,
    0x4a07d10a,
    0x2aff7812,
    0x4615d006,/*6e10*/
    0x30b2f894,
    0xd10342ab,
    0xb8aff625,
    0xb974f625,/*6e20*/
    0xb952f625,
    0xc0006e2c,
    0x000000ff,/*6e28*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch47 =
{
    47,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch47_ibrt_ins_data_t5),
    0x0002bf78,
    0xbf42f1da,
    0xc0006e00,
    (uint8_t *)bes1400_patch47_ibrt_ins_data_t5
};///adjust slot share for tws without ibrt

const uint32_t bes1400_patch48_ibrt_ins_data_t5[] =
{
    0x781b4b13,
    0xd0212bff,
    0x4604b510,
    0xf946f632,
    0x4b0fb1d8,
    0x4b0f781a,
    0x3022f853,
    0x0f28f113,
    0xf893d013,
    0xeb0330c1,
    0xb2db0342,
    0x0383eb03,
    0x4a09005b,
    0xf3c35a9b,
    0x2b5303c9,
    0x7ea2d905,
    0x441368e3,
    0x4378f023,
    0xbd1060e3,
    0xbf004770,
    0xc0006e2c,
    0xc0000998,
    0xd0211632,
    0x0028f104,
    0xffcef7ff,
    0x60a36b63,
    0xb9fcf627,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch48 =
{
    48,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch48_ibrt_ins_data_t5),
    0x0002e290,
    0xbdfcf1d8,
    0xc0006e30,
    (uint8_t *)bes1400_patch48_ibrt_ins_data_t5
};//big packet send in sco v1

const uint32_t bes1400_patch49_ibrt_ins_data_t5[] =
{
    /*6ec0*/ 0xf6232100,
    /*6ec4*/ 0xf643feaf,
    /*6ec8*/ 0x2300fb69,
    /*6ecc*/ 0xbf006243,
    /*6ed0*/ 0xbcf4f626,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch49 =
{
    49,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch49_ibrt_ins_data_t5),
    0x0002d88c,
    0xbb18f1d9, // jump a0018188 -> a0206978
    0xc0006ec0,
    (uint8_t *)bes1400_patch49_ibrt_ins_data_t5
};///role switch fail to disconnect

#if 0
const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch50 =
{
    50,
    BTDRV_PATCH_INACT,
    0,
    0x0003ea94,
    0xbf00e04a,
    0,
    NULL
};//ble channel map disable
#elif 0
const uint32_t bes1400_patch50_ibrt_ins_data_t5[] =
{
    0xf8948263,
    0xbf0030b3,
    0xbf00bf00,
    0xbf00bf00,
    0xbf002304,
    0xb8b8f625,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch50 =
{
    50,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch50_ibrt_ins_data_t5),
    0x0002c024,
    0xbf3cf1da,
    0xc0006ea0,
    (uint8_t *)bes1400_patch50_ibrt_ins_data_t5
};
#else
const uint32_t bes1400_patch50_ibrt_ins_data_t5[] =
{
    0x75a32312,/*6ed8*/
    0x001bf898,
    0x1088f8d1,
    0x300cf8d8,
    0xf023440b,/*6ee8*/
    0xea834378,
    0xf8986380,
    0x1a5b1019,
    0x101af898,/*6ef8*/
    0xf3f1fbb3,
    0xfbb3b2db,
    0xfb02f1f2,
    0x1ad23311,/*6f08*/
    0xf894b2d6,
    0xb91330b3,
    0xf8842301,
    0xf5fc30bb,/*6f18*/
    0xf890fa05,
    0xf5c33025,
    0x3304631c,
    0x23048263,/*6f28*/
    0xb87cf625,/*6f2c*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch50 =
{
    50,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch50_ibrt_ins_data_t5),
    0x0002bfc4,
    0xbf88f1da,
    0xc0006ed8,
    (uint8_t *)bes1400_patch50_ibrt_ins_data_t5
};////ibrt connect slot adjust in sco
#endif

const uint32_t bes1400_patch51_ibrt_ins_data_t5[] =
{
    0x490a60e3,/*6f30*/
    0x428f6809,
    0x4620d10c,
    0x22002100,
    0xf914f625,/*6f40*/
    0x429d68a3,
    0x60a5bf38,
    0x134cf640,
    0x3012f8a4,/*6f50*/
    0xbf004620,
    0xbc20f624,
    0xc0006e2c,/*6f5c*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch51 =
{
    51,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch51_ibrt_ins_data_t5),
    0x0002b798,
    0xbbcaf1db,
    0xc0006f30,
    (uint8_t *)bes1400_patch51_ibrt_ins_data_t5
};//sniff trans mode timing

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch52 =
{
    52,
    BTDRV_PATCH_ACT,
    0,
    0x0001bf54,
    0xbff2f7ff, //jump a001bf54 -> a001bf3c
    0,
    NULL
};//LMP_MSG_HANDLER(switch_req) LC_CONNECTED

const uint32_t bes1400_patch53_ibrt_ins_data_t5[] =
{
    /*6f74*/ 0xf632b140,
    /*6f78*/ 0x42b0f841,
    /*6f7c*/ 0xf632d104,
    /*6f80*/ 0x2801f82b,
    /*6f84*/ 0xbf00d002,
    /*6f88*/ 0xbea8f619,
    /*6f8c*/ 0xbf08f619,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch53 =
{
    53,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch53_ibrt_ins_data_t5),
    0x00020cd8,
    0xb94cf1e6, // jump a0020cd8 -> a0206f74
    0xc0006f74,
    (uint8_t *)bes1400_patch53_ibrt_ins_data_t5
};//disconnect cmp

#if 0
const uint32_t bes1400_patch54_ibrt_ins_data_t5[] =
{
    0xb3384606,/*6f90*/
    0x0a0388a0,
    0xd1212b05,
    0xf1034b14,
    0xb2c10528,/*6fa0*/
    0xe044f8df,
    0x428a781a,
    0x785ad114,
    0x0f01f012,/*6fb0*/
    0x88e7d006,
    0x2f020a3f,
    0xf85ed814,
    0xb18f7027,/*6fc0*/
    0x0f02f012,
    0x8922d006,
    0x2a020a12,
    0xf85ed80a,/*6fe0*/
    0xb13a2022,
    0x42ab3302,
    0xbf00d1e4,
    0xb8e5f5fa,/*6ff0*/
    0xb8dcf5fa,
    0xb8ecf5fa,
    0xc0005cd8,
    0xc0006ff4,/*7000*/
    0x0327032a,
    0x010e0326,
    0x0000010f,
    0x00000000,/*7010*/
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,/*7020*/
    0x00000000,
    0x00000000,/*7024*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch54 =
{
    54,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch54_ibrt_ins_data_t5),
    0x0000119c,
    0xbef8f205,
    0xc0006f90,
    (uint8_t *)bes1400_patch54_ibrt_ins_data_t5
};//msg filter
#else
const uint32_t bes1400_patch54_ibrt_ins_data_t5[] =
{
    0x8885b5f8,
    0x2b050a2b,
    0x4b49d121,
    0x0428f103,
    0x4f48b2e9,
    0x428a781a,
    0x785ad116,
    0x0f01f012,
    0x88c6d007,
    0x2e020a36,
    0xf857d86b,
    0x2e006026,
    0xf012d069,
    0xd0070f02,
    0x0a128902,
    0xd8642a02,
    0x2022f857,
    0xd0622a00,
    0x42a33302,
    0xf240d1e2,
    0x429d5335,
    0x4605d15d,
    0x08637b44,
    0xd1142b04,
    0xf5f988c0,
    0x2832ffb5,
    0xf240d155,
    0x88e95006,
    0xfb2ef5f9,
    0x0a0288e8,
    0xf8534b2e,
    0xf8933022,
    0xf5f9135e,
    0x2401ff39,
    0x2b18e050,
    0x88c0d126,
    0xff9ef5f9,
    0xd1402836,
    0x02fef004,
    0x0a3688ee,
    0xf8534b24,
    0xf8844026,
    0xb2f62044,
    0x21184630,
    0xff1cf61c,
    0x4040f894,
    0xd1302c01,
    0xf6294630,
    0x88e8fa29,
    0xf800f60f,
    0x0a0288e8,
    0xf8534b19,
    0xf8933022,
    0xf5f9135e,
    0xe027ff0f,
    0xd1202b7e,
    0x3b097b83,
    0x2b01b2db,
    0xf631d81d,
    0x7a83fd77,
    0xd81a2b02,
    0xf8524a0f,
    0xfab44023,
    0x0964f484,
    0x2401e014,
    0x2401e012,
    0x2401e010,
    0x2401e00e,
    0x2400e00c,
    0x2400e00a,
    0x2400e008,
    0x2401e006,
    0x2400e004,
    0x2400e002,
    0x2401e000,
    0xbdf84620,
    0xa0207520,
    0xc0005cd8,
    0xb1484606,
    0xf7ff4620,
    0x2801ff5f,
    0xbf00d102,
    0xbe56f5f9,
    0xbe49f5f9,
    0xbe40f5f9,
    0x0327032a,
    0x010e0326,
    0x0000010f,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch54 =
{
    54,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch54_ibrt_ins_data_t5),
    0x0000119c,
    0xb9b2f206,
    0xc00073cc,
    (uint8_t *)bes1400_patch54_ibrt_ins_data_t5
};//msg filter

#endif

const uint32_t bes1400_patch55_ibrt_ins_data_t5[] =
{
    0x49079804,/*70a4*/
    0x42886809,
    0xf632d103,
    0x2800f86b,
    0x3301d004,/*70b4*/
    0xf88bb2db,
    0xbf0030bb,
    0xbaf4f628,
    0xc0006e2c,/*70c4*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ibrt_ins_patch55 =
{
    55,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch55_ibrt_ins_data_t5),
    0x0002f6a4,
    0xbcfef1d7,
    0xc00070a4,
    (uint8_t *)bes1400_patch55_ibrt_ins_data_t5
};//tws link dont rx traffic++

/////1400 t5 ibrt patch (chip id =2)
static const uint32_t best1400_t5_ibrt_ins_patch_config[] =
{
    56,
    (uint32_t)&bes1400_t5_ibrt_ins_patch0,
    (uint32_t)&bes1400_t5_ibrt_ins_patch1,
    (uint32_t)&bes1400_t5_ibrt_ins_patch2,
    (uint32_t)&bes1400_t5_ibrt_ins_patch3,
    (uint32_t)&bes1400_t5_ibrt_ins_patch4,
    (uint32_t)&bes1400_t5_ibrt_ins_patch5,
    (uint32_t)&bes1400_t5_ibrt_ins_patch6,
    (uint32_t)&bes1400_t5_ibrt_ins_patch7,
    (uint32_t)&bes1400_t5_ibrt_ins_patch8,
    (uint32_t)&bes1400_t5_ibrt_ins_patch9,
    (uint32_t)&bes1400_t5_ibrt_ins_patch10,
    (uint32_t)&bes1400_t5_ibrt_ins_patch11,
    (uint32_t)&bes1400_t5_ibrt_ins_patch12,
    (uint32_t)&bes1400_t5_ibrt_ins_patch13,
    (uint32_t)&bes1400_t5_ibrt_ins_patch14,
    (uint32_t)&bes1400_t5_ibrt_ins_patch15,
    (uint32_t)&bes1400_t5_ibrt_ins_patch16,
    (uint32_t)&bes1400_t5_ibrt_ins_patch17,
    (uint32_t)&bes1400_t5_ibrt_ins_patch18,
    (uint32_t)&bes1400_t5_ibrt_ins_patch19,
    (uint32_t)&bes1400_t5_ibrt_ins_patch20,
    (uint32_t)&bes1400_t5_ibrt_ins_patch21,
    (uint32_t)&bes1400_t5_ibrt_ins_patch22,
    (uint32_t)&bes1400_t5_ibrt_ins_patch23,
    (uint32_t)&bes1400_t5_ibrt_ins_patch24,
    (uint32_t)&bes1400_t5_ibrt_ins_patch25,
    (uint32_t)&bes1400_t5_ibrt_ins_patch26,
    (uint32_t)&bes1400_t5_ibrt_ins_patch27,
    (uint32_t)&bes1400_t5_ibrt_ins_patch28,
    (uint32_t)&bes1400_t5_ibrt_ins_patch29,
    (uint32_t)&bes1400_t5_ibrt_ins_patch30,
    (uint32_t)&bes1400_t5_ibrt_ins_patch31,
    (uint32_t)&bes1400_t5_ibrt_ins_patch32,
    (uint32_t)&bes1400_t5_ibrt_ins_patch33,
    (uint32_t)&bes1400_t5_ibrt_ins_patch34,
    (uint32_t)&bes1400_t5_ibrt_ins_patch35,
    (uint32_t)&bes1400_t5_ibrt_ins_patch36,
    (uint32_t)&bes1400_t5_ibrt_ins_patch37,
    (uint32_t)&bes1400_t5_ibrt_ins_patch38,
    (uint32_t)&bes1400_t5_ibrt_ins_patch39,
    (uint32_t)&bes1400_t5_ibrt_ins_patch40,
    (uint32_t)&bes1400_t5_ibrt_ins_patch41,
    (uint32_t)&bes1400_t5_ibrt_ins_patch42,
    (uint32_t)&bes1400_t5_ibrt_ins_patch43,
    (uint32_t)&bes1400_t5_ibrt_ins_patch44,
    (uint32_t)&bes1400_t5_ibrt_ins_patch45,
    (uint32_t)&bes1400_t5_ibrt_ins_patch46,
    (uint32_t)&bes1400_t5_ibrt_ins_patch47,
    (uint32_t)&bes1400_t5_ibrt_ins_patch48,
    (uint32_t)&bes1400_t5_ibrt_ins_patch49,
    (uint32_t)&bes1400_t5_ibrt_ins_patch50,
    (uint32_t)&bes1400_t5_ibrt_ins_patch51,
    (uint32_t)&bes1400_t5_ibrt_ins_patch52,
    (uint32_t)&bes1400_t5_ibrt_ins_patch53,
    (uint32_t)&bes1400_t5_ibrt_ins_patch54,
    (uint32_t)&bes1400_t5_ibrt_ins_patch55,
};
#endif
/*****************************************************************************
CHIP ID                : 1400
Metal id               : 0
BT controller tag      : t1
*****************************************************************************/
const uint32_t bes1400_patch0_ins_data[] =
{
    0x601a4b01,
    0xbbe2f600,
    0x4000007C
}; // intersys mcu2bt interrupt clear

const BTDRV_PATCH_STRUCT bes1400_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch0_ins_data),
    0x00006ec8,
    0xbc1af1ff,
    0xc0006700,
    (uint8_t *)bes1400_patch0_ins_data
};

const uint32_t bes1400_patch1_ins_data[] =
{
    0x601a4b01,
    0xbbf8f600,
    0x4000007C
}; // intersys mcu2bt interrupt clear

const BTDRV_PATCH_STRUCT bes1400_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch1_ins_data),
    0x00006f04,
    0xbc04f1ff,
    0xc0006710,
    (uint8_t *)bes1400_patch1_ins_data
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x00023f28,
    0xbf00bd10,
    0,
    NULL
}; // remove assert

const uint32_t bes1400_patch3_ins_data[] =
{
    /*6720*/ 0x4a18b2e8,
    /*6724*/ 0x21cbf892,
    /*6728*/ 0x4b150112,
    /*672c*/ 0xbf005a9b,
    /*6730*/ 0x0309f3c3,
    /*6734*/ 0x2280ea4f,
    /*6738*/ 0x0003ea42,
    /*673c*/ 0xf826f000,
    /*6740*/ 0x0c00ea4f,
    /*6744*/ 0xb040f8df,
    /*6748*/ 0xb000f8cd,
    /*674c*/ 0x22019903,
    /*6750*/ 0xbf002300,
    /*6754*/ 0xf804f5fd,
    /*6758*/ 0xc000f8cd,
    /*675c*/ 0x0307f018,
    /*6760*/ 0xd00b9306,
    /*6764*/ 0x30acf9b6,
    /*6768*/ 0x44189800,
    /*676c*/ 0x00acf8a6,
    /*6770*/ 0x30aef896,
    /*6774*/ 0xbf003301,
    /*6778*/ 0x30aef886,
    /*677c*/ 0xbde6f625,
    /*6780*/ 0xd02115b4,
    /*6784*/ 0xc0005c28,
    /*6788*/ 0xc0004108,
    /*678c*/ 0x2385f3c0,
    /*6790*/ 0x490c3b48,
    /*6794*/ 0x0207f000,
    /*6798*/ 0x1a9b5c8a,
    /*679c*/ 0x02c3f3c0,
    /*67a0*/ 0x5c8a490b,
    /*67a4*/ 0x4a0f1a9b,
    /*67a8*/ 0x10c2f3c0,
    /*67ac*/ 0x1a1b5c10,
    /*67b0*/ 0xb25ab2db,
    /*67b4*/ 0xbfb42a07,
    /*67b8*/ 0x202f202e,
    /*67bc*/ 0xbf001a18,
    /*67c0*/ 0x4770b240,
    /*67c4*/ 0xc00067c8, // RF_RX_HWAGC_FLT_TBL
    /*67c8*/ 0xf4f1eeeb,
    /*67cc*/ 0x00fdfaf7,
    /*67d0*/ 0xc00067d4, // RF_RX_HWAGC_I2V_TBL
    /*67d4*/ 0xeeebe8e5,
    /*67d8*/ 0xfaf7f4f1,
    /*67dc*/ 0x7f7f00fd,
    /*67e0*/ 0x7f7f7f7f,
    /*67e4*/ 0xc00067e8, // RF_RX_HWAGC_LNA_TBL
    /*67e8*/ 0xfaf3ede7,
    /*67ec*/ 0x7f7f7f00
}; // acl rx rssi

const BTDRV_PATCH_STRUCT bes1400_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch3_ins_data),
    0x0002c300,
    0xba0ef1da,
    0xc0006720,
    (uint8_t *)bes1400_patch3_ins_data
};

const uint32_t bes1400_patch4_ins_data[] =
{
    /*67f0*/ 0x30acf9b6,
    /*67f4*/ 0x20aef896,
    /*67f8*/ 0xf2f2fb93,
    /*67fc*/ 0x20aff886,
    /*6800*/ 0xbf002300,
    /*6804*/ 0x30aef886,
    /*6808*/ 0xb81af626
}; // acl report avg rssi

const BTDRV_PATCH_STRUCT bes1400_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch4_ins_data),
    0x0002c83c,
    0xbfd8f1d9,
    0xc00067f0,
    (uint8_t *)bes1400_patch4_ins_data
};

const uint32_t bes1400_patch5_ins_data[] =
{
    /*6810*/ 0xbf00b404,
    /*6814*/ 0x2083ea4f,
    /*6818*/ 0x4b0c7929,
    /*681c*/ 0x01c1ebc1,
    /*6820*/ 0x1011f833,
    /*6824*/ 0x01c9f3c1,
    /*6828*/ 0x0001ea40,
    /*682c*/ 0xffaef7ff,
    /*6830*/ 0x2814bc04,
    /*6834*/ 0x7328bfd2,
    /*6838*/ 0x732b237f,
    /*683c*/ 0x4b047b28,
    /*6840*/ 0x54587929,
    /*6844*/ 0x23004628,
    /*6848*/ 0xbcd6f639,
    /*684c*/ 0xd02104d6,
    /*6850*/ 0xc0006854,
    /*6854*/ 0x7f7f7f7f,
    /*6858*/ 0x7f7f7f7f
}; // ble rx rssi

const BTDRV_PATCH_STRUCT bes1400_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch5_ins_data),
    0x000401f4,
    0xbb0cf1c6,
    0xc0006810,
    (uint8_t *)bes1400_patch5_ins_data
};

const uint32_t bes1400_patch6_ins_data[] =
{
    /*6860*/ 0x4b040870,
    /*6864*/ 0x3000fba3,
    /*6868*/ 0x4b030880,
    /*686c*/ 0xbf005c18,
    /*6870*/ 0xbf9cf630,
    /*6874*/ 0x92492493,
    /*6878*/ 0xc0006854
}; // adv rssi report

const BTDRV_PATCH_STRUCT bes1400_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch6_ins_data),
    0x000377a4,
    0xb85cf1cf,
    0xc0006860,
    (uint8_t *)bes1400_patch6_ins_data
};

const uint32_t bes1400_patch7_ins_data[] =
{
    /*687c*/ 0x4b040870,
    /*6880*/ 0x3000fba3,
    /*6884*/ 0x4b030880,
    /*6888*/ 0xbf005c18,
    /*688c*/ 0xbffaf630,
    /*6890*/ 0x92492493,
    /*6894*/ 0xc0006854
}; // adv rssi report

const BTDRV_PATCH_STRUCT bes1400_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch7_ins_data),
    0x0003787c,
    0xbffef1ce,
    0xc000687c,
    (uint8_t *)bes1400_patch7_ins_data
};

const uint32_t bes1400_patch8_ins_data[] =
{
    0xf5fa4628,
    0x4903fd79,
    0x46286008,
    0xbf002101,
    0xbf9cf613,
    0xc00068b8,
    0x00000000

};

const BTDRV_PATCH_STRUCT bes1400_ins_patch8=
{
    8,
    BTDRV_PATCH_INACT,
    sizeof(bes1400_patch8_ins_data),
    0x0001a7e8,
    0xb85af1ec,
    0xc00068a0,
    (uint8_t *)bes1400_patch8_ins_data
};

const uint32_t bes1400_patch9_ins_data[] =
{
    0x0f00f1b8,
    0x4628d004,
    0x68094904,
    0xfcf8f5fa,
    0x21134630,
    0x3044f894,
    0xbfd5f613,
    0xc00068b8


};

const BTDRV_PATCH_STRUCT bes1400_ins_patch9 =
{
    9,
    BTDRV_PATCH_INACT,
    sizeof(bes1400_patch9_ins_data),
    0x0001a880,
    0xb81ef1ec,
    0xc00068c0,
    (uint8_t *)bes1400_patch9_ins_data
};

const uint32_t bes1400_patch10_ins_data[] =
{
    /*68e0*/ 0x2000b081, //"sub sp, #4\n\t"                 // [b081]
    //"movs r0, #0\n\t"                // [2000]
    /*68e4*/ 0x88ca9000, //"str  r0, [sp]\n\t"              // [9000]
    //"ldrh r2, [r1, #6]\n\r"          // [88ca] r1 is desc_to_be_freed, load desc_to_be_freed->buffer_idx to r2
    /*68e8*/ 0x4f00f412, //"tst.w r2, #32768\n\r"           // [f412 4f00] test buffer_idx & 0x8000
    /*68ec*/ 0x2001d010, //"beq.n 0xa02069e0\n\r"           // [d010] if is zero then jump
    //"movs r0, #1\n\t"                // [2001]
    /*68f0*/ 0xbf009000, //"str  r0, [sp]\n\t"              // [9000]
    /*68f4*/ 0x020ef3c2, //"ubfx r2, r2, #0, #15\n\r"       // [f3c2 020e] r2 = buffer_idx & 0x7fff
    /*68f8*/ 0x323b4807, //"ldr r0, [pc, #28]\n\r"          // [4807] load address of em_buf_env (c00063f8) to r0
    //"adds r2, #59\n\r"               // [323b] (idx + 59) << 3 = 8 * idx + 59 * 8 = 8 * idx + 472
    /*68fc*/ 0x0c01ea5f, //"movs ip, r1\n\r"                // [ea5f 0c01] backup r1 to ip
    /*6900*/ 0x01c2eb00, //"add.w r1, r0, r2, lsl #3\n\r"   // [eb00 01c2] r1 is address of em_buf_env.tx_buff_node[idx]
    /*6904*/ 0xbf003014, //"adds r0, #20\n\r"               // [3014] r0 is address of em_buf_env.tx_buff_free
    /*6908*/ 0xfc58f5fc, //"bl 0xa00031bc\n\r"              // call co_list_push_back a0206908 -> a00031bc
    /*690c*/ 0x010cea5f, //"movs r1, ip\n\r"                // [ea5f 010c] restore r1
    /*6910*/ 0x23008888, //"ldrh r0, [r1, #4]\n\r"          // [8888]
    //"movs r3, #0\n\r"                // [2300]
    /*6914*/ 0xb8a6f63c, // jump a0206914 -> a0042a64
    /*6918*/ 0xc00063f8, // address of em_buf_env
};


const BTDRV_PATCH_STRUCT bes1400_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch10_ins_data),
    0x00042a60,
    0xbf3ef1c3, // jump a0042a60 -> a02068e0
    0xc00068e0,
    (uint8_t *)bes1400_patch10_ins_data
};


const uint32_t bes1400_patch11_ins_data[] =
{
    /*691c*/ 0xb662b904,
    /*6920*/ 0xb0019900, // ldr r1, [sp]; add sp, #4
    /*6924*/ 0xb8aaf63c, // jump a0206924 -> a0042a7c
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch11_ins_data),
    0x00042a78,
    0xbf50f1c3, // jump a0042a78 -> a020691c
    0xc000691c,
    (uint8_t *)bes1400_patch11_ins_data
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x00042a7c,
    0xbd100008,  // movs r0, r1; pop {r4, pc}
    0,
    NULL
};

#if 0
const uint32_t bes1400_patch13_ins_data[] =
{
    /*6b10*/ 0x3078f886,
    /*6b14*/ 0xbf0088e3,
    /*6b18*/ 0x4343ea6f,
    /*6b1c*/ 0x4353ea6f,
    /*6b20*/ 0xbf0080e3,
    /*6b24*/ 0xb85cf639, // jump a0206b24 -> a003fbe0
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch13 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch13_ins_data),
    0x0003fbdc,
    0xbf98f1c6, // jump a003fbdc -> a0206b10
    0xc0006b10,
    (uint8_t *)bes1400_patch13_ins_data
};
#endif

const uint32_t bes1400_patch13_ins_data[] =
{
    /*6928*/ 0x3078f886,
    /*692c*/ 0xbf0088e3,
    /*6930*/ 0x4343ea6f,
    /*6934*/ 0x4353ea6f,
    /*6938*/ 0xbf0080e3,
    /*693c*/ 0xb950f639, // jump a020693c -> a003fbe0
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch13 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch13_ins_data),
    0x0003fbdc,
    0xbea4f1c6, // jump a003fbdc -> a0206928
    0xc0006928,
    (uint8_t *)bes1400_patch13_ins_data
};

/* 1400 t1 (CHIP_ID: 0) patches */
static const uint32_t best1400_ins_patch_config[] =
{
    14,
    (uint32_t)&bes1400_ins_patch0,
    (uint32_t)&bes1400_ins_patch1,
    (uint32_t)&bes1400_ins_patch2,
    (uint32_t)&bes1400_ins_patch3,
    (uint32_t)&bes1400_ins_patch4,
    (uint32_t)&bes1400_ins_patch5,
    (uint32_t)&bes1400_ins_patch6,
    (uint32_t)&bes1400_ins_patch7,
    (uint32_t)&bes1400_ins_patch8,
    (uint32_t)&bes1400_ins_patch9,

    (uint32_t)&bes1400_ins_patch10,
    (uint32_t)&bes1400_ins_patch11,
    (uint32_t)&bes1400_ins_patch12,
    (uint32_t)&bes1400_ins_patch13,
};


#ifndef IBRT

/***************************************************************************
 *
 * instruction patch Information
 *
 * BT ROM Chip Version:1400 T4
 * CHIP ID = 1
 *
 * Used Patch Space:
 *
 * patch 2a: 6820 ~ 6880    ld_acl_rx_sync clk_off cal
 *           <empty-space>
 * patch 2e: 68c4 ~ 68cc    lc_op_loc_sniff_req
 * patch 45: 68d0 ~ 68e4    set rx_max_slot
 * patch 47: 68e8 ~ 68f8    ld_acl_set_max_slot set stop_latency2
 * patch  3: 6900 ~ 6940    adjust music_playing_link traffic
 * patch  4: 6948 ~ 6978    llc_util_get_free_conhdl() set nesn sn 0
 * patch  5: 6980 ~ 6990    ld_sco_end set inactive_sco_en 0
 * patch 5a: 6994 ~ 69ac    ld_sco_dynamic_switch
 * patch  6: 69c0 ~ 69ec    for ble and sco:  sco_find_sync_flag=1 in ld_sco_frm_isr
 * patch  7: 69f4 ~ 6a8c    for ble and sco:  d0220144 setting  &  OVERLAP_RESERVE2 in ld_sco_avoid_ble_connect
 * patch  8: 6aa0 ~ 6ab8    for ble and sco:  prev_conflict_type_record in ld_sco_avoid_ble_connect
 * patch  9: 6ac0 ~ 6ad0    for ble and sco:  OVERLAP_RESERVE2 in lld_check_conflict_with_ble
 * patch 10: 6ae0 ~ 6b28    for ble and sco:  ld_fm_prog_push LD_FRAME_TYPE_ESCO
 * patch 11: 6b40 ~ 6b4c    lpclk
 * patch 12: 6b54 ~ 6b64    LMP_MSG_HANDLER(feats_res_ext)
 * patch 14: 6b68 ~ 6bac    ld_sco_end
 * patch 15: 6bb8 ~ 6bf0    ld_acl_frm_isr
 * patch 16: 6bf4 ~ 6c18    ld_sco_dynamic_switch, set switch to link
 * patch 25: 6c3c ~ 6c78    rwip_sleep() pscan assert when sniff
 *           <empty-space>
 * patch 17: 6ca8 ~ 6cd0    ld_sco_switch_to_handler, set switch to link
 * patch 30: 6cd4 ~ 6d1c    ld_acl_clk_isr, set switch to link
 * patch 31: 6d24 ~ 6d34    lc_send_lmp
 * patch 32: 6d3c ~ 6d54    ld_acl_sched a002c4ac
 * patch 48: 6d58 ~ 6da4    ld_sco_start_cbk
 * patch 19: 6da8 ~ 6dfc    ld_acl_sched
 * patch 20: 6e00 ~ 6e24    ld_calculate_event_timestamp
 *           <empty-space>
 * patch 46: 6e88 ~ 6ef0    ld_acl_sched set stop_latency2
 * patch 49: 6f40 ~ 6f5c    ld_pscan_frm_cbk
 ****************************************************************************/

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_0=
{
    0,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_1 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0002f2bc,
    0xbf0030bb,
    0,
    NULL
}; // when EM_BT_RXSEQERR_BIT, no rx traffic++

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x00025ef4,
    0xbf00bf00,
    0,
    NULL
};// remove the memset of ldenv

const uint32_t bes1400_t4_patch2a_ins_data[] =
{
    /*6820*/ 0x4b139a0b,
    /*6824*/ 0xd01f429a,
    /*6828*/ 0x20b5f894,
    /*682c*/ 0xb9dab2d2,
    /*6830*/ 0x3098f9b4,
    /*6834*/ 0x1a9bb22a,
    /*6838*/ 0x0227f103,
    /*683c*/ 0xd8022a4e,
    /*6840*/ 0x7088f8d4,
    /*6844*/ 0x2b28e00f,
    /*6848*/ 0xf8d4dd05,
    /*684c*/ 0x3f017088,
    /*6850*/ 0x4778f027,
    /*6854*/ 0xf113e007,
    /*6858*/ 0xda040f28,
    /*685c*/ 0x7088f8d4,
    /*6860*/ 0xf0273701,
    /*6864*/ 0xbf004778,
    /*6868*/ 0x708cf8c4,
    /*686c*/ 0xbc8cf623, //jump a002a188
    /*6870*/ 0xa002f765, //ld_acl_sniff_frm_isr lr
};//ld_acl_rx_sync clk_off cal

const uint32_t bes1400_t4_patch2e_ins_data[] =
{
    /*68c4*/ 0xbf0080c6,
    /*68c8*/ 0x71022200,
    /*68cc*/ 0xba76f619, //jump a001fdbc
};//lc_op_loc_sniff_req

const uint32_t bes1400_t4_patch45_ins_data[] =
{
    0xf895212e, /*68d0*/
    0xbf002044,
    0xfa7ef61d,
    0x78714620,
    0xfdc6f62b,
    0xbc6ef614, /*68e4*/
};//set rx_max_slot

const uint32_t bes1400_t4_patch47_ins_data[] =
{
    0xf5fc7de2, /*68e8*/
    0x78c1fc77,
    0x44311a51,
    0x7621b2c9,
    0xbdcef62b, /*68f8*/
};//ld_acl_set_max_slot set stop_latency2

const uint32_t bes1400_t4_patch_3_ins_data[] =
{
    /*6900*/ 0xbf002002,
    /*6904*/ 0xfef4f605,
    /*6908*/ 0xd1132802, // bne.n 19
    /*690c*/ 0xfdf4f60c,
    /*6910*/ 0xbf00b980, // cbnz 16
    /*6914*/ 0xfc62f5fc,
    /*6918*/ 0x2bff7c03,
    /*691c*/ 0xbf00d00a, // beq.n 10
    /*6920*/ 0x40b2f89b,
    /*6924*/ 0xd00542a3,
    /*6928*/ 0xbf002300,
    /*692c*/ 0x30bbf88b,
    /*6930*/ 0xbf00e006, // b.n 6
    /*6934*/ 0x30bbf89b, // rx_traffic--
    /*6938*/ 0xbf003b01,
    /*693c*/ 0x30bbf88b,
    /*6940*/ 0xbc88f628, // jump a0206940 -> a002f254
}; // adjust music_playing_link traffic

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_3_ins_data),
    0x0002f24c,
    0xbb58f1d7, // jump a002f24c -> a0206900
    0xc0006900,
    (uint8_t *)bes1400_t4_patch_3_ins_data
};

const uint32_t bes1400_t4_patch_4_ins_data[] =
{
    /*6948*/ 0x8035d114,
    /*694c*/ 0x235a4628,
    /*6950*/ 0xf000fb03,
    /*6954*/ 0x5a834a08,
    /*6958*/ 0x5380f423,
    /*695c*/ 0x0c1b041b,
    /*6960*/ 0x5a835283,
    /*6964*/ 0x5300f423,
    /*6968*/ 0x0c1b041b,
    /*696c*/ 0xbf005283,
    /*6970*/ 0xbebcf637, // jump a0206970 -> a003e6ec
    /*6974*/ 0xbebcf637, // jump a0206974 -> a003e6f0
    /*6978*/ 0xd02100c8,
}; // llc_util_get_free_conhdl() set nesn sn 0

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_4_ins_data),
    0x0003e6e8,
    0xb92ef1c8, // jump a003e6e8 -> a0206948
    0xc0006948,
    (uint8_t *)bes1400_t4_patch_4_ins_data
};

const uint32_t bes1400_t4_patch_5_ins_data[] =
{
    /*6980*/ 0x4b032200,
    /*6984*/ 0xbf00701a,
    /*6988*/ 0x23011ce2,
    /*698c*/ 0xbe2af624, // jump a020698c -> a002b5e4
    /*6990*/ 0xc000099a,
}; // ld_sco_end set inactive_sco_en 0

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_5_ins_data),
    0x0002b5e0,
    0xb9cef1db, // jump a002b5e0 -> a0206980
    0xc0006980,
    (uint8_t *)bes1400_t4_patch_5_ins_data
};

const uint32_t bes1400_t4_patch_5a_ins_data[] =
{
    /*6994*/ 0x2b0310db,
    /*6998*/ 0x4b04d104,
    /*699c*/ 0x01c2f883,
    /*69a0*/ 0xb804f627, // jump a02069a0 -> a002d9ac
    /*69a4*/ 0x2b0210db,
    /*69a8*/ 0xbff4f626, // jump a02069a8 -> a002d994
    /*69ac*/ 0xc0005d34,
};//ld_sco_dynamic_switch

const uint32_t bes1400_t4_patch_6_ins_data[] =
{
    0xd00d2b00, /*69c0*/
    0xf8384b07,
    0xf4133003,
    0xd0024f00,
    0x4b052201,
    0xf899701a,
    0x2b0130b3,
    0xba2af629,
    0xba9cf629,
    0xd02111f8,
    0xc00069ec,////sco_find_sync_flag
    0x00000000, /*69ec*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_6_ins_data),
    0x0002fe2c,
    0xbdc8f1d6,
    0xc00069c0,
    (uint8_t *)bes1400_t4_patch_6_ins_data
};//for ble and sco:  sco_find_sync_flag=1 in ld_sco_frm_isr

const uint32_t bes1400_t4_patch_7_ins_data[] =
{
    0x781b4b22, /*69f4*/
    0x2200b18b,
    0x701a4b20,
    0x681b4b1b,
    0x4a1b4293,
    0xbfb96813,
    0x2340f423,
    0x2300f443,
    0x3340f423,
    0x3300f443,
    0x4b176013,
    0x2b00781b,
    0x2201d020,
    0x701a4b14,
    0xb1d84680,
    0x781b4b11,
    0x1f03b9c3,
    0x2b01b2db,
    0xf1b8d903,
    0xd1130f09,
    0x2201e00c,
    0x701a4b0d,
    0x0f09f1b8,
    0x2209d103,
    0x701a4b0b,
    0x68eb2704,
    0xb86ef627,
    0x4b072201,
    0xe7f4701a,
    0xb88af627,
    0xb877f627,
    0xd0220140,
    0xd0220144,
    0xc0000978,
    0xc00069ec,////////sco_find_sync_flag
    0xc0006a88,////ble_sco_need_move_flag
    0xc0006a8c,////prev_conflict_type_record
    0x00000000,
    0x00000000, /*6a8c*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_7_ins_data),
    0x0002db28,
    0xbf64f1d8,
    0xc00069f4,
    (uint8_t *)bes1400_t4_patch_7_ins_data
};//for ble and sco:  d0220144 setting  &  OVERLAP_RESERVE2 in ld_sco_avoid_ble_connect

const uint32_t bes1400_t4_patch_8_ins_data[] =
{
    0x781b4b05, /*6aa0*/
    0xd1032b09,
    0x4b032200,
    0x2704701a,
    0x60eb1be3,
    0xb828f627,
    0xc0006a8c,////prev_conflict_type_record /*6ab8*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_8 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_8_ins_data),
    0x0002db04,
    0xbfccf1d8,
    0xc0006aa0,
    (uint8_t *)bes1400_t4_patch_8_ins_data
};//for ble and sco:  prev_conflict_type_record in ld_sco_avoid_ble_connect

const uint32_t bes1400_t4_patch_9_ins_data[] =
{
    0x2c021b64, /*6ac0*/
    0x2009d101,
    0x3402e002,
    0xbb0af63c,
    0xbb66f63c, /*6ad0*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_9 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_9_ins_data),
    0x000430e0,
    0xbceef1c3,
    0xc0006ac0,
    (uint8_t *)bes1400_t4_patch_9_ins_data
};//for ble and sco:  OVERLAP_RESERVE2 in lld_check_conflict_with_ble

const uint32_t bes1400_t4_patch_10_ins_data[] =
{
    0xd1084298, /*6ae0*/
    0x3047f894,
    0x92004a0f,
    0x22024631,
    0xfc7af61f,
    0x4b0be014,
    0xb143781b,
    0x3047f894,
    0x92004a09,
    0x22024631,
    0xfc6ef61f,
    0xf894e008,
    0x4a053047,
    0x46319200,
    0xf61f2204,
    0xbf00fc65,
    0xb97af62b,
    0xc0006a88,///ble_sco_need_move_flag
    0xa002fc25, /*6b28*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_10_ins_data),
    0x00031e04,
    0xbe6cf1d4,
    0xc0006ae0,
    (uint8_t *)bes1400_t4_patch_10_ins_data
};//for ble and sco:  ld_fm_prog_push LD_FRAME_TYPE_ESCO when ble_sco_need_move_flag in ld_sco_evt_start_cbk

const uint32_t bes1400_t4_patch_11_ins_data[] =
{
    0xfbb24a02, /*6b40*/
    0xf5f9f3f3,
    0xbf00bd86,
    0x9af8da00, /*6b4c*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_11_ins_data),
    0x00000650,
    0xba76f206,
    0xc0006b40,
    (uint8_t *)bes1400_t4_patch_11_ins_data
};//lpclk

const uint32_t bes1400_t4_patch_12_ins_data[] =
{
    /*6b54*/ 0xd1052b01,
    /*6b58*/ 0x2b0178f3,
    /*6b5c*/ 0xbf00d902,
    /*6b60*/ 0xbc5af618, // jump a0206b60 -> a001f418
    /*6b64*/ 0xbc62f618, // jump a0206b64 -> a001f42c
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_12 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_12_ins_data),
    0x0001f414,
    0xbb9ef1e7, // jump a001f414 -> a0206b54
    0xc0006b54,
    (uint8_t *)bes1400_t4_patch_12_ins_data
};//LMP_MSG_HANDLER(feats_res_ext)

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_13 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_5a_ins_data),
    0x0002d990,
    0xb800f1d9, // jump a002d990 -> a0206994
    0xc0006994,
    (uint8_t *)bes1400_t4_patch_5a_ins_data
};//ld_sco_dynamic_switch


const uint32_t bes1400_t4_patch_14_ins_data[] =
{
    0x781b4b0b, /*6b68*/
    0x0f53ebb4,
    0x4b0ed00e,
    0x31c0f8b3,
    0x0318f003,
    0xd0072b00,
    0x4b062201,
    0x4b07601a,
    0x0000601c,
    0xbd7cf624,
    0x22000164,
    0xbd76f624,
    0xc00060bd,
    0xc0006ba0,//acl_end
    0x00000000,
    0xc0006ba8,//background_sco_link
    0x000000ff,
    0xc0005d34, /*6bac*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_14 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch_14_ins_data),
    0x0002b680,
    0xba72f1db,
    0xc0006b68,
    (uint8_t *)bes1400_t4_patch_14_ins_data
};//ld_sco_end

const uint32_t bes1400_t4_patch15_ins_data[] =
{
    0x681b4b0a, /*6bb8*/
    0xd00c2b00,
    0x681b4b09,
    0xd0082bff,
    0x2200015b,
    0x505a4907,
    0x601a4b04,
    0x4b0422ff,
    0x60af601a,
    0x00004620,
    0xbf10f628,
    0xc0006ba0,
    0xc0006ba8,
    0xd0220140,
    0xc0000980, /*6bf0*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_15 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch15_ins_data),
    0x0002fa00,
    0xb8daf1d7,
    0xc0006bb8,
    (uint8_t *)bes1400_t4_patch15_ins_data
};//ld_acl_frm_isr
//if acl_end and link, bt_e_scochancntl_pack(background_sco_link, 0, 0, 0, 0, 0);

const uint32_t bes1400_t4_patch16_ins_data[] =
{
    0x4b052201, /*6bf4*/
    0x2200601a,
    0x701a4b05,
    0xf8b34b05,
    0x000031c0,
    0xbebff626,
    0xc0006c10,//switch_sco_flag
    0x00000000,
    0xc000099a,//inactive_sco_en
    0xc0005d34, /*6c18*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_16 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch16_ins_data),
    0x0002d984,
    0xb936f1d9,
    0xc0006bf4,
    (uint8_t *)bes1400_t4_patch16_ins_data
};//ld_sco_dynamic_switch, set switch to link
//inactive_sco_en = 0,switch_sco_flag = 1

const uint32_t bes1400_t4_patch25_ins_data[] =
{
    /*6c3c*/ 0xbf00b9d0,
    /*6c40*/ 0x8310f3ef,
    /*6c44*/ 0x0301f003,
    /*6c48*/ 0xb672b903,
    /*6c4c*/ 0xfa92f601, //call a0008174
    /*6c50*/ 0xb408b148,
    /*6c54*/ 0xff36f63f, //call a0046ac4
    /*6c58*/ 0xb120bc08,
    /*6c5c*/ 0xfce6f5fa, //call a000162c
    /*6c60*/ 0xb91bb108,
    /*6c64*/ 0xb92be004,
    /*6c68*/ 0xe0052000,
    /*6c6c*/ 0xbbf5f5f9, //jump a000045a
    /*6c70*/ 0xbbdbf5f9, //jump a000042a
    /*6c74*/ 0xbbc6f5f9, //jump a0000404
    /*6c78*/ 0xbbaef5f9, //jump a00003d8
};

const uint32_t bes1400_t4_patch17_ins_data[] =
{
    0x3a700147, /*6ca8*/
    0xf44358bb,
    0x50bb4380,
    0xf8b34b05,
    0xf3c331c0,
    0x2b0303c1,
    0x2201d102,
    0x701a4b02,
    0xbffff626,
    0xc0005d34,
    0xc000099a, /*6cd0*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_17 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch17_ins_data),
    0x0002dcc0,
    0xbff2f1d8,
    0xc0006ca8,
    (uint8_t *)bes1400_t4_patch17_ins_data
};//ld_sco_switch_to_handler, set switch to link
//(bt_e_scochancntl_e_scochanen_setf 1)

const uint32_t bes1400_t4_patch30_ins_data[] =
{
    0x68124a0d, /*6cd4*/
    0xd1132a01,
    0x68104a0c,
    0x60103001,
    0xd10d280c,
    0xf890480b,
    0x014001c2,
    0x5842490a,
    0x4280f422,
    0x20005042,
    0x60104a04,
    0x60104a02,
    0x46294630,
    0xbcc0f623,
    0xc0006c10,//switch_sco_flag
    0xc0006d14,//switch_delay_counter
    0x00000000,
    0xc0005d34,
    0xd0220140, /*6d1c*/
};

const uint32_t bes1400_t4_patch31_ins_data[] =
{
    /*6d24*/ 0x20014a02,
    /*6d28*/ 0xb0045590,
    /*6d2c*/ 0xb850f61d, //jump a0023dd0
    /*6d30*/ 0xc0006d34,
    /*6d34*/ 0x00000000, //lmp_no_buf
};

const uint32_t bes1400_t4_patch32_ins_data[] =
{
    /*6d3c*/ 0x5d914a05,
    /*6d40*/ 0x2100b921,
    /*6d44*/ 0xbf00460a,
    /*6d48*/ 0xbbb2f625, //jump a002c4b0
    /*6d4c*/ 0x55902000,
    /*6d50*/ 0xbbdaf625, //jump a002c508
    /*6d54*/ 0xc0006d34, //lmp_no_buf
};

const uint32_t bes1400_t4_patch48_ins_data[] =
{
    0x20024680, /*6d58*/
    0xfcc8f605,
    0xd1172802,
    0x681b4b0e,
    0xf894b1a3,
    0x20013046,
    0x4b0c4098,
    0x31c0f8b3,
    0x0000ea23,
    0x0041f3c0,
    0xfb5af62b,
    0xd1052805,
    0x9047f894,
    0x3880f44f,
    0xb8c6f62b,
    0x9047f894,
    0x4808ea4f,
    0xb8b8f62b,

    0xc0006ef0,//music_ongoing_flag
    0xc0005d34, /*6da4*/

};//ld_sco_start_cbk

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_18 =
{
    18,
    BTDRV_PATCH_ACT,
    0,
    0x0002b660,
    0xe0066013,
    0,
    NULL
};//ld_sco_end2

const uint32_t bes1400_t4_patch19_ins_data[] =
{
    0x681b4b11, /*6da8*/
    0xf5fcb173,
    0x7bc3fa15,
    0xf5fcb153,
    0x7c03fa11,
    0xd10542b3,
    0xf5fc8a67,
    0x6943fa0b,
    0x8263443b,
    0xfa06f5fc,
    0x3035f890,
    0x2002b14b,
    0xfc8af605,
    0xd1042802,
    0x681a4b05,
    0x44138a63,
    0xbf008263,
    0xbb5df625,

    0xc0006df4,//reconnecting_flag
    0x00000000,
    0xc0006dfc,//dual_slave_slot_add
    0x000009c4,//4*slot_size  /*6dfc*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_19=
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch19_ins_data),
    0x0002c488,
    0xbc8ef1da,
    0xc0006da8,
    (uint8_t *)bes1400_t4_patch19_ins_data,
};//ld_acl_sched

const uint32_t bes1400_t4_patch20_ins_data[] =
{
    0x20b2f898, /*6e00*/
    0x40932301,
    0xf8b24a05,
    0xea2221c0,
    0xf3c30303,
    0x4a030341,
    0xb023f852,
    0xb832f625,

    0xc0005d34,
    0xc000098c, /*6e24*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_20=
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch20_ins_data),
    0x0002be80,
    0xbfbef1da,
    0xc0006e00,
    (uint8_t *)bes1400_t4_patch20_ins_data,
};//ld_calculate_event_timestamp

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_21 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch2a_ins_data),
    0x0002a184,
    0xbb4cf1dc, // jump a002a184 -> a0206820
    0xc0006820,
    (uint8_t *)bes1400_t4_patch2a_ins_data
};//ld_acl_rx_sync clk_off cal


const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_22=
{
    22,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_23=
{
    23,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_24=
{
    24,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_25 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch25_ins_data),
    0x000003c4,
    0xbc3af206, // jump a00003c4 -> a0206c3c
    0xc0006c3c,
    (uint8_t *)bes1400_t4_patch25_ins_data
}; // rwip_sleep() pscan assert when sniff

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_26=
{
    26,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_27=
{
    27,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_28=
{
    28,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_29=
{
    29,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_30 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch30_ins_data),
    0x0002a688,
    0xbb24f1dc,
    0xc0006cd4,
    (uint8_t *)bes1400_t4_patch30_ins_data
};//ld_acl_clk_isr, set switch to link
//(bt_e_scochancntl_e_scochanen_setf 0)

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_31 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch31_ins_data),
    0x00023da0,
    0xbfc0f1e2, //jump a0023da0 -> a0206d24
    0xc0006d24,
    (uint8_t *)bes1400_t4_patch31_ins_data
};//lc_send_lmp

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_32 =
{
    32,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch32_ins_data),
    0x0002c4ac,
    0xbc46f1da, //jump a002c4ac -> a0206d3c
    0xc0006d3c,
    (uint8_t *)bes1400_t4_patch32_ins_data
};//ld_acl_sched

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_33=
{
    33,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_34=
{
    34,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_35=
{
    35,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_36=
{
    36,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_37=
{
    38,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_38 =
{
    38,
    BTDRV_PATCH_ACT,
    0,
    0x00029e70,
    0xbf8c7f80,
    0,
    NULL,
};//ld_acl_rx_sync2

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_39 =
{
    39,
    BTDRV_PATCH_ACT,
    0,
    0x0002a068,
    0xbf947f80,
    0,
    NULL,
};//ld_acl_rx_no_sync

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_40 =
{
    40,
    BTDRV_PATCH_ACT,
    0,
    0x0002a1b0,
    0x7f80f5b3,
    0,
    NULL,
};//ld_acl_rx_sync

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_41=
{
    41,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_42 =
{
    42,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch2e_ins_data),
    0x0001fdb4,
    0xbd86f1e6,//jump a001fdb4 -> a02068c4
    0xc00068c4,
    (uint8_t *)bes1400_t4_patch2e_ins_data
};//lc_op_loc_sniff_req

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_43=
{
    43,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_44=
{
    44,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_45 =
{
    45,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch45_ins_data),
    0x0001b178,
    0xbbaaf1eb,
    0xc00068d0,
    (uint8_t *)bes1400_t4_patch45_ins_data
};//set rx_max_slot

const uint32_t bes1400_t4_patch46_ins_data[] =
{
    0xf8534b15, /*6e88*/
    0x46304020,
    0xfad2f62b,
    0xf8b34b13,
    0xf00331c0,
    0xb19b0318,
    0xd0112801,
    0xf8934b0f,
    0x4b0f21c2,
    0x2022f853,
    0x681b4b0e,
    0x2b01b12b,
    0xf892d106,
    0x42b33046,
    0x7de3d102,
    0xe0097623,
    0x20c4f894,
    0x441a7de3,
    0xf5fcb2d2,
    0x78c3f983,
    0x76221ad2,
    0xba51f625,

    0xc000098c,
    0xc0005d34,
    0xc0000980,
    0xc0006ef0,//music_ongoing_flag
    0x00000000, /*6ef0*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_46 =
{
    46,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch46_ins_data),
    0x0002c37c,
    0xbd84f1da,
    0xc0006e88,
    (uint8_t *)bes1400_t4_patch46_ins_data
};//ld_acl_sched set stop_latency2

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_47 =
{
    47,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch47_ins_data),
    0x0003248c,
    0xba2cf1d4,
    0xc00068e8,
    (uint8_t *)bes1400_t4_patch47_ins_data
};//ld_acl_set_max_slot set stop_latency2

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_48 =
{
    48,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch48_ins_data),
    0x00031f08,
    0xbf26f1d4,
    0xc0006d58,
    (uint8_t *)bes1400_t4_patch48_ins_data
};//ld_sco_start_cbk

const uint32_t bes1400_t4_patch49_ins_data[] =
{
    /*6f40*/ 0x46286825,
    /*6f44*/ 0x303ef895,
    /*6f48*/ 0xd0012b02,
    /*6f4c*/ 0xb8c4f622, //jump a00290d8
    /*6f50*/ 0xbf002300,
    /*6f54*/ 0x303ef885,
    /*6f58*/ 0xf9c4f622, //call a00292e4
    /*6f5c*/ 0xb8e3f622, //jump a0029126
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_49 =
{
    49,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_patch49_ins_data),
    0x000290d4,
    0xbf34f1dd,//jump a00290d4 -> a0206f40
    0xc0006f40,
    (uint8_t *)bes1400_t4_patch49_ins_data
};//ld_pscan_frm_cbk

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_50=
{
    50,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_51 =
{
    51,
    BTDRV_PATCH_ACT,
    0,
    0x000005ec, //a00005ec
    0xbc87f006, //jump a0006efe
    0,
    NULL,
};//rwip_assert_err

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_52 =
{
    52,
    BTDRV_PATCH_ACT,
    0,
    0x00006ec4,
    0xb9bef000, //jump a0007244
    0,
    NULL,
};//DEF_IRQHandler

static const uint32_t best1400_t4_ins_patch_config[] =
{
    53,
    (uint32_t)&bes1400_t4_ins_patch_0,
    (uint32_t)&bes1400_t4_ins_patch_1,
    (uint32_t)&bes1400_t4_ins_patch_2,
    (uint32_t)&bes1400_t4_ins_patch_3,
    (uint32_t)&bes1400_t4_ins_patch_4,
    (uint32_t)&bes1400_t4_ins_patch_5,
    (uint32_t)&bes1400_t4_ins_patch_6,
    (uint32_t)&bes1400_t4_ins_patch_7,
    (uint32_t)&bes1400_t4_ins_patch_8,
    (uint32_t)&bes1400_t4_ins_patch_9,
    (uint32_t)&bes1400_t4_ins_patch_10,
    (uint32_t)&bes1400_t4_ins_patch_11,
    (uint32_t)&bes1400_t4_ins_patch_12,
    (uint32_t)&bes1400_t4_ins_patch_13,
    (uint32_t)&bes1400_t4_ins_patch_14,
    (uint32_t)&bes1400_t4_ins_patch_15,
    (uint32_t)&bes1400_t4_ins_patch_16,
    (uint32_t)&bes1400_t4_ins_patch_17,
    (uint32_t)&bes1400_t4_ins_patch_18,
    (uint32_t)&bes1400_t4_ins_patch_19,
    (uint32_t)&bes1400_t4_ins_patch_20,
    (uint32_t)&bes1400_t4_ins_patch_21,
    (uint32_t)&bes1400_t4_ins_patch_22,
    (uint32_t)&bes1400_t4_ins_patch_23,
    (uint32_t)&bes1400_t4_ins_patch_24,
    (uint32_t)&bes1400_t4_ins_patch_25,
    (uint32_t)&bes1400_t4_ins_patch_26,
    (uint32_t)&bes1400_t4_ins_patch_27,
    (uint32_t)&bes1400_t4_ins_patch_28,
    (uint32_t)&bes1400_t4_ins_patch_29,
    (uint32_t)&bes1400_t4_ins_patch_30,
    (uint32_t)&bes1400_t4_ins_patch_31,
    (uint32_t)&bes1400_t4_ins_patch_32,
    (uint32_t)&bes1400_t4_ins_patch_33,
    (uint32_t)&bes1400_t4_ins_patch_34,
    (uint32_t)&bes1400_t4_ins_patch_35,
    (uint32_t)&bes1400_t4_ins_patch_36,
    (uint32_t)&bes1400_t4_ins_patch_37,
    (uint32_t)&bes1400_t4_ins_patch_38,
    (uint32_t)&bes1400_t4_ins_patch_39,
    (uint32_t)&bes1400_t4_ins_patch_40,
    (uint32_t)&bes1400_t4_ins_patch_41,
    (uint32_t)&bes1400_t4_ins_patch_42,
    (uint32_t)&bes1400_t4_ins_patch_43,
    (uint32_t)&bes1400_t4_ins_patch_44,
    (uint32_t)&bes1400_t4_ins_patch_45,
    (uint32_t)&bes1400_t4_ins_patch_46,
    (uint32_t)&bes1400_t4_ins_patch_47,
    (uint32_t)&bes1400_t4_ins_patch_48,
    (uint32_t)&bes1400_t4_ins_patch_49,
    (uint32_t)&bes1400_t4_ins_patch_50,
    (uint32_t)&bes1400_t4_ins_patch_51,
    (uint32_t)&bes1400_t4_ins_patch_52,
};
#else
/***************************************************************************
 *
 * instruction patch Information
 *
 * BT ROM Chip Version:1400 T4
 * CHIP ID = 1
 * IBRT VERISON PATCH
 *
 * patch  0:                check afh mismatch 1
 * patch  1:                when EM_BT_RXSEQERR_BIT, no rx traffic++
 * patch  2:                remove the memset of ldenv
 * patch 2a: 6810 ~ 6874    __SW_SEQ_FILTER__
 * patch 2b: 6880 ~ 6890    lc_check_bt_device_conected : null ptr protect
 * patch 2c: 6894 ~ 68ac    hack ibrt switch tx seq
 * patch 2d: 68b0 ~ 68c0    __SW_SEQ_FILTER__
 * patch 2e: 68c4 ~ 68cc    lc_op_loc_sniff_req
 * patch 2f: 68d0 ~ 68e4    set rx_max_slot
 * patch 2g: 68e8 ~ 68f8    ld_acl_set_max_slot set stop_latency2
 * patch  3: 6900 ~ 6910    lc_lmp_sniff_start_handler null ptr
                    <empty>
 * patch  4: 6948 ~ 6978    llc_util_get_free_conhdl() set nesn sn 0
 * patch  5: 6980 ~ 6990    is_in_sniff_mode use NULL ptr
 * patch 5a: 6994 ~ 69ac    ld_sco_dynamic_switch
 * patch  6: 69c0 ~ 69ec    for ble and sco: sco_find_sync_flag=1 in ld_sco_frm_isr
 * patch  7: 69f4 ~ 6a8c    for ble and sco: d0220144 setting  &  OVERLAP_RESERVE2 in ld_sco_avoid_ble_connect
 * patch  8: 6aa0 ~ 6ab8    for ble and sco: prev_conflict_type_record in ld_sco_avoid_ble_connect
 * patch  9: 6ac0 ~ 6ad0    for ble and sco: OVERLAP_RESERVE2 in lld_check_conflict_with_ble
 * patch 10: 6ae0 ~ 6b28    for ble and sco: ld_fm_prog_push LD_FRAME_TYPE_ESCO when ble_sco_need_move_flag in ld_sco_evt_start_cbk
 * patch 11: 6b40 ~ 6b4c    lpclk
 * patch 12: 6b54 ~ 6b64    LMP_MSG_HANDLER(feats_res_ext)
 * patch 13:                ignore ld_get_sniffer_enter_sniff_flag()
 * patch 14:                ld_sniffer_start_monitored_link
 * patch 15:                pwr ctrl rsp (ASSERT_WARN)
 * patch 16:                in used
 * patch 17:                in used
 * patch 18:                for controller dump
 * patch 19:                __SW_SEQ_FILTER__
 * patch 20:                ibrt switch op =2
 * patch 21:                lc_check_bt_device_conected
 * patch 22:                hack ibrt switch tx seq
 * patch 23:                ibrt open afh update
 * patch 24:                __SW_SEQ_FILTER__
 * patch 25: 6c3c ~ 6c78    rwip_sleep() pscan assert when sniff
 * patch 26:                acl end assert HACK
 * patch 27: 6c80 ~ 6c90    snoop disconnect
 * patch 28: 6c98 ~ 6ca4    start snoop using bitoff
 * patch 29:                ld_acl_start t_poll when ibrt
 * patch 30:                rwip_assert_err
 * patch 31: 6d24 ~ 6d34    lc_send_lmp
 * patch 32: 6d3c ~ 6d54    ld_acl_sched
 * patch 33:                ld_sniffer_update_status: SNIFFER_EVT_SCO_CONNECTED
 * patch 34: 6d5c ~ 6d74    ibrt can not detach 1
 * patch 35: 6d7c ~ 6da8    ibrt can not detach 2
 * patch 36: 6dac ~ 6dbc    ibrt can not detach 3
 * patch 37: 6dc0 ~ 6dcc    ibrt stop sco process
 * patch 38:                ld_acl_rx_sync2: evt->delay >370
 * patch 39:                ld_acl_rx_no_sync: evt->delay >370
 * patch 40:                ld_acl_rx_sync: evt->delay >370
 * patch 41: 6dd4 ~ 6e6c    ibrt sco fix acl interval
 * patch 42:                lc_op_loc_sniff_req
 * patch 43:                check afh mismatch 2
 * patch 44: 6e74 ~ 6e84    retx manage
 * patch 45:                set rx_max_slot
 * patch 46: 6e88 ~ 6ef0    ld_acl_sched set stop_latency2
 * patch 47:                ld_acl_set_max_slot set stop_latency2
 * patch 48: 6ef4 ~ 6f3c    ld_sco_start_cbk
 * patch 49: 6f40 ~ 6f5c    ld_pscan_frm_cbk
 * patch 50:                lm_page_scan_end_ind
 * patch 51:                DEF_IRQHandler
 * patch 52: 6f60 ~ 6f78    lc_connect_info_req_handler
 * patch 53: 6f80 ~ 6f88    ld_send_tws_switch_cmd_after_both_ready(assert)
 * patch 54: 6f90 ~ 6fa0    filter 1st ibrt packet
 * patch 55: 6fa8 ~ 6fb4    protect null ptr when send afh
 ****************************************************************************/

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_0 =
{
    0,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000389b8,
    0x7a22d206,
    0,
    NULL
}; //check afh mismatch 1

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_1 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0002f2bc,
    0xbf0030bb,
    0,
    NULL
}; // when EM_BT_RXSEQERR_BIT, no rx traffic++

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x00025ef4,
    0xbf00bf00,
    0,
    NULL
};// remove the memset of ldenv

const uint32_t bes1400_t4_ibrt_patch2a_ins_data[] =
{
    0x681b4b0e, /*6810*/
    0x4b0eb1ab,
    0x31cbf893,
    0x4a0d011b,
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
    0xc000685c,
    0xc0005d34,
    0xd02115f2,
    0xc0006860,
    0x00000001,
    0x00020202,
    0x9805bf00,
    0xffd2f7ff,
    0xd1012800,
    0xbd61f628,
    0xbd9df628, /*6874*/
};

const uint32_t bes1400_t4_ibrt_patch2b_ins_data[] =
{
    /*6880*/ 0xbf00b2e6,
    /*6884*/ 0x1b04f855,
    /*6888*/ 0xb109bf00,
    /*688c*/ 0xbd77f610, //jump a001737e
    /*6890*/ 0xbd7ef610, //jump a0017390
};

const uint32_t bes1400_t4_ibrt_patch2c_ins_data[] =
{
    /*6894*/ 0x21057a98,
    /*6898*/ 0xbf002201,
    /*689c*/ 0xfb5af61d, // call a020689c -> a0023f54
    /*68a0*/ 0x7a984b02,
    /*68a4*/ 0xbf002102,
    /*68a8*/ 0xbb36f633, // jump a02068a8 -> a0039f18
    /*68ac*/ 0xc0006358, // sniffer_env
}; // ld_report_master_snoop_switch_result_ack_callback

const uint32_t bes1400_t4_ibrt_patch2d_ins_data[] =
{
    /*68b0*/ 0x328af885,
    /*68b4*/ 0x22024b02,
    /*68b8*/ 0xbf00559a,
    /*68bc*/ 0xbe78f614, // jump a02068bc -> a001b5b0
    /*68c0*/ 0xc0006860, // link_id_seq
}; // lc_rsw_end_ind link_id_seq[link_id]=2

const uint32_t bes1400_t4_ibrt_patch2e_ins_data[] =
{
    /*68c4*/ 0xbf0080c6,
    /*68c8*/ 0x71022200,
    /*68cc*/ 0xba76f619, //jump a001fdbc
};//lc_op_loc_sniff_req

const uint32_t bes1400_t4_ibrt_patch_3_ins_data[] =
{
    /*6900*/ 0x5024f853,
    /*6904*/ 0xd0012d00,
    /*6908*/ 0xb860f617,
    /*690c*/ 0xbf002000,
    /*6910*/ 0xb894f617,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_3_ins_data),
    0x0001d9c8,
    0xbf9af1e8, // jump a001d9c8 -> a0206900
    0xc0006900,
    (uint8_t *)bes1400_t4_ibrt_patch_3_ins_data
};// lc_lmp_sniff_start_handler null ptr

const uint32_t bes1400_t4_ibrt_patch_4_ins_data[] =
{
    /*6948*/ 0x8035d114,
    /*694c*/ 0x235a4628,
    /*6950*/ 0xf000fb03,
    /*6954*/ 0x5a834a08,
    /*6958*/ 0x5380f423,
    /*695c*/ 0x0c1b041b,
    /*6960*/ 0x5a835283,
    /*6964*/ 0x5300f423,
    /*6968*/ 0x0c1b041b,
    /*696c*/ 0xbf005283,
    /*6970*/ 0xbebcf637, // jump a0206970 -> a003e6ec
    /*6974*/ 0xbebcf637, // jump a0206974 -> a003e6f0
    /*6978*/ 0xd02100c8,
}; // llc_util_get_free_conhdl() set nesn sn 0

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_4_ins_data),
    0x0003e6e8,
    0xb92ef1c8, // jump a003e6e8 -> a0206948
    0xc0006948,
    (uint8_t *)bes1400_t4_ibrt_patch_4_ins_data
};

const uint32_t bes1400_t4_ibrt_patch_5_ins_data[] =
{
    /*6980*/ 0xf8534b02,
    /*6984*/ 0xb11b3020,
    /*6988*/ 0xbce7f610,
    /*698c*/ 0xc0005c70,
    /*6990*/ 0x47702000,
}; //is_in_sniff_mode use NULL ptr

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_5_ins_data),
    0x00017354,
    0xbb14f1ef, // jump a0017354 -> a0206980
    0xc0006980,
    (uint8_t *)bes1400_t4_ibrt_patch_5_ins_data
};

const uint32_t bes1400_t4_ibrt_patch_5a_ins_data[] =
{
    /*6994*/ 0x2b0310db,
    /*6998*/ 0x4b04d104,
    /*699c*/ 0x01c2f883,
    /*69a0*/ 0xb804f627, // jump a02069a0 -> a002d9ac
    /*69a4*/ 0x2b0210db,
    /*69a8*/ 0xbff4f626, // jump a02069a8 -> a002d994
    /*69ac*/ 0xc0005d34,
};//ld_sco_dynamic_switch

const uint32_t bes1400_t4_ibrt_patch_6_ins_data[] =
{
    0xd00d2b00, /*69c0*/
    0xf8384b07,
    0xf4133003,
    0xd0024f00,
    0x4b052201,
    0xf899701a,
    0x2b0130b3,
    0xba2af629,
    0xba9cf629,
    0xd02111f8,
    0xc00069ec,////sco_find_sync_flag
    0x00000000, /*69ec*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_6_ins_data),
    0x0002fe2c,
    0xbdc8f1d6,
    0xc00069c0,
    (uint8_t *)bes1400_t4_ibrt_patch_6_ins_data
};//for ble and sco:  sco_find_sync_flag=1 in ld_sco_frm_isr

const uint32_t bes1400_t4_ibrt_patch_7_ins_data[] =
{
    0x781b4b22, /*69f4*/
    0x2200b18b,
    0x701a4b20,
    0x681b4b1b,
    0x4a1b4293,
    0xbfb96813,
    0x2340f423,
    0x2300f443,
    0x3340f423,
    0x3300f443,
    0x4b176013,
    0x2b00781b,
    0x2201d020,
    0x701a4b14,
    0xb1d84680,
    0x781b4b11,
    0x1f03b9c3,
    0x2b01b2db,
    0xf1b8d903,
    0xd1130f09,
    0x2201e00c,
    0x701a4b0d,
    0x0f09f1b8,
    0x2209d103,
    0x701a4b0b,
    0x68eb2704,
    0xb86ef627,
    0x4b072201,
    0xe7f4701a,
    0xb88af627,
    0xb877f627,
    0xd0220140,
    0xd0220144,
    0xc0000978,
    0xc00069ec,////////sco_find_sync_flag
    0xc0006a88,////ble_sco_need_move_flag
    0xc0006a8c,////prev_conflict_type_record
    0x00000000,
    0x00000000, /*6a8c*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_7_ins_data),
    0x0002db28,
    0xbf64f1d8,
    0xc00069f4,
    (uint8_t *)bes1400_t4_ibrt_patch_7_ins_data
};//for ble and sco:  d0220144 setting  &  OVERLAP_RESERVE2 in ld_sco_avoid_ble_connect


const uint32_t bes1400_t4_ibrt_patch_8_ins_data[] =
{
    0x781b4b05, /*6aa0*/
    0xd1032b09,
    0x4b032200,
    0x2704701a,
    0x60eb1be3,
    0xb828f627,
    0xc0006a8c,////prev_conflict_type_record /*6ab8*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_8 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_8_ins_data),
    0x0002db04,
    0xbfccf1d8,
    0xc0006aa0,
    (uint8_t *)bes1400_t4_ibrt_patch_8_ins_data
};//for ble and sco:  prev_conflict_type_record in ld_sco_avoid_ble_connect

const uint32_t bes1400_t4_ibrt_patch_9_ins_data[] =
{
    0x2c021b64, /*6ac0*/
    0x2009d101,
    0x3402e002,
    0xbb0af63c,
    0xbb66f63c, /*6ad0*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_9 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_9_ins_data),
    0x000430e0,
    0xbceef1c3,
    0xc0006ac0,
    (uint8_t *)bes1400_t4_ibrt_patch_9_ins_data
};//for ble and sco:  OVERLAP_RESERVE2 in lld_check_conflict_with_ble

const uint32_t bes1400_t4_ibrt_patch_10_ins_data[] =
{
    0xd1084298, /*6ae0*/
    0x3047f894,
    0x92004a0f,
    0x22024631,
    0xfc7af61f,
    0x4b0be014,
    0xb143781b,
    0x3047f894,
    0x92004a09,
    0x22024631,
    0xfc6ef61f,
    0xf894e008,
    0x4a053047,
    0x46319200,
    0xf61f2204,
    0xbf00fc65,
    0xb97af62b,
    0xc0006a88,///ble_sco_need_move_flag
    0xa002fc25, /*6b28*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_10_ins_data),
    0x00031e04,
    0xbe6cf1d4,
    0xc0006ae0,
    (uint8_t *)bes1400_t4_ibrt_patch_10_ins_data
};//for ble and sco:  ld_fm_prog_push LD_FRAME_TYPE_ESCO when ble_sco_need_move_flag in ld_sco_evt_start_cbk

const uint32_t bes1400_t4_ibrt_patch_11_ins_data[] =
{
    0xfbb24a02, /*6b40*/
    0xf5f9f3f3,
    0xbf00bd86,
    0x9af8da00, /*6b4c*/
};


const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_11_ins_data),
    0x00000650,
    0xba76f206,
    0xc0006b40,
    (uint8_t *)bes1400_t4_ibrt_patch_11_ins_data
};//lpclk

const uint32_t bes1400_t4_ibrt_patch_12_ins_data[] =
{
    /*6b54*/ 0xd1052b01,
    /*6b58*/ 0x2b0178f3,
    /*6b5c*/ 0xbf00d902,
    /*6b60*/ 0xbc5af618, // jump a0206b60 -> a001f418
    /*6b64*/ 0xbc62f618, // jump a0206b64 -> a001f42c
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_12 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch_12_ins_data),
    0x0001f414,
    0xbb9ef1e7, // jump a001f414 -> a0206b54
    0xc0006b54,
    (uint8_t *)bes1400_t4_ibrt_patch_12_ins_data
};//LMP_MSG_HANDLER(feats_res_ext)

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_13 =
{
    13,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0001fb10,
    0x2000bf00,
    0,
    NULL
}; //ignore ld_get_sniffer_enter_sniff_flag()

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_14 =
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0x00039a98,
    0xf2002802,
    0,
    NULL,
};//ld_sniffer_start_monitored_link

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_15 =
{
    15,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0001a3ec,
    0xbf00e007,
    0,
    NULL
}; //pwr ctrl rsp (ASSERT_WARN)

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_16 =
{
    16,
    BTDRV_PATCH_ACT,
    0,
    0x00011334,
    0xbf00bf00,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_17 =
{
    17,
    BTDRV_PATCH_ACT,
    0,
    0x000324a8,
    0xbf00bf00,
    0,
    NULL,
};

const uint32_t bes1400_t4_ibrt_patch18_ins_data[] =
{
    /*6b68*/ 0xbf00669a,
    /*6b6c*/ 0x66da466a,//mov r2, sp; str   r2, [r3, #108]
    /*6b70*/ 0x8208f3ef,//mrs r2, MSP
    /*6b74*/ 0xbf00671a,//str r2, [r3, #112]
    /*6b78*/ 0x8209f3ef,//mrs r2, PSP
    /*6b7c*/ 0xbf00675a,//str r2, [r3, #116]
    /*6b80*/ 0xbf00bf00,
    /*6b84*/ 0xbf00bf00,
    /*6b88*/ 0xbf00bf00,
    /*6b8c*/ 0xbb4cf600,//jump a0007228
};
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_18=
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch18_ins_data),
    0x00007214,
    0xbca8f1ff,//jump a0007214 -> a0206b68
    0xc0006b68,
    (uint8_t *)bes1400_t4_ibrt_patch18_ins_data,
};//for controller dump

// __SW_SEQ_FILTER__ - add software seqn filter to protect recive identical repeating packages
#if defined(IBRT)
#define __SW_SEQ_FILTER__
#endif
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_19 =
{
    19,
#ifdef __SW_SEQ_FILTER__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes1400_t4_ibrt_patch2a_ins_data),
    0x0002f330,
    0xba98f1d7,
    0xc0006810,
    (uint8_t *)bes1400_t4_ibrt_patch2a_ins_data
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_20 =
{
    20,
    BTDRV_PATCH_ACT,
    0,
    0x00039cb8,
    0xd13b2902,
    0,
    NULL
};//ibrt switch op

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_21 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch2b_ins_data),
    0x00017378,
    0xba82f1ef,
    0xc0006880,
    (uint8_t *)bes1400_t4_ibrt_patch2b_ins_data
};//lc_check_bt_device_conected

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_22 =
{
    22,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch2c_ins_data),
    0x00039f14,
    0xbcbef1cc, // jump a0039f14 -> a0206894
    0xc0006894,
    (uint8_t *)bes1400_t4_ibrt_patch2c_ins_data
}; // hack ibrt switch tx seq

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_23 =
{
    23,
    BTDRV_PATCH_ACT,
    0,
    0x000387e8,
    0xbf00e001,
    0,
    NULL
};//open afh update

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_24 =
{
    24,
#ifdef __SW_SEQ_FILTER__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(bes1400_t4_ibrt_patch2d_ins_data),
    0x0001b5ac,
    0xb980f1eb, // jump a001b5ac -> a02068b0
    0xc00068b0,
    (uint8_t *)bes1400_t4_ibrt_patch2d_ins_data
}; // lc_rsw_end_ind link_id_seq[link_id]=2

const uint32_t bes1400_t4_ibrt_patch25_ins_data[] =
{
    /*6c3c*/ 0xbf00b9d0,
    /*6c40*/ 0x8310f3ef,
    /*6c44*/ 0x0301f003,
    /*6c48*/ 0xb672b903,
    /*6c4c*/ 0xfa92f601, //call a0008174
    /*6c50*/ 0xb408b148,
    /*6c54*/ 0xff36f63f, //call a0046ac4
    /*6c58*/ 0xb120bc08,
    /*6c5c*/ 0xfce6f5fa, //call a000162c
    /*6c60*/ 0xb91bb108,
    /*6c64*/ 0xb92be004,
    /*6c68*/ 0xe0052000,
    /*6c6c*/ 0xbbf5f5f9, //jump a000045a
    /*6c70*/ 0xbbdbf5f9, //jump a000042a
    /*6c74*/ 0xbbc6f5f9, //jump a0000404
    /*6c78*/ 0xbbaef5f9, //jump a00003d8
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_25 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch25_ins_data),
    0x000003c4,
    0xbc3af206, // jump a00003c4 -> a0206c3c
    0xc0006c3c,
    (uint8_t *)bes1400_t4_ibrt_patch25_ins_data
}; // rwip_sleep() pscan assert when sniff

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_26 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x00030180,
    0xe0022000,
    0,
    NULL
};//acl end assert

const uint32_t bes1400_t4_ibrt_patch27_ins_data[] =
{
    0x781b4b03, /*6c80*/
    0xd1012b01,
    0xbbf8f633,
    0xbbfff633,
    0xc0006358, /*6c90*/
};
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_27 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch27_ins_data),
    0x0003a468,
    0xbc0af1cc,
    0xc0006C80,
    (uint8_t *)bes1400_t4_ibrt_patch27_ins_data
};//snoop disconnect

const uint32_t bes1400_t4_ibrt_patch28_ins_data[] =
{
    0x0f01f1b8, /*6c98*/
    0x380ebf0c,
    0xb280301c,
    0xbaccf632, /*6ca4*/
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_28 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch28_ins_data),
    0x00039218,
    0xbd3ef1cd,
    0xc0006C98,
    (uint8_t *)bes1400_t4_ibrt_patch28_ins_data
};//start snoop using bitoff

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_29 =
{
    29,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002e094,
    0xf8a62308,
    0,
    NULL,
};//ld_acl_start t_poll

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_30 =
{
    30,
    BTDRV_PATCH_ACT,
    0,
    0x000005ec, //a00005ec
    0xbc87f006, //jump a0006efe
    0,
    NULL,
};//rwip_assert_err

const uint32_t bes1400_t4_ibrt_patch31_ins_data[] =
{
    /*6d24*/ 0x20014a02,
    /*6d28*/ 0xb0045590,
    /*6d2c*/ 0xb850f61d, //jump a0023dd0
    /*6d30*/ 0xc0006d34,
    /*6d34*/ 0x00000000, //lmp_no_buf
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_31 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch31_ins_data),
    0x00023da0,
    0xbfc0f1e2, //jump a0023da0 -> a0206d24
    0xc0006d24,
    (uint8_t *)bes1400_t4_ibrt_patch31_ins_data
};//lc_send_lmp

const uint32_t bes1400_t4_ibrt_patch32_ins_data[] =
{
    /*6d3c*/ 0x5d914a05,
    /*6d40*/ 0x2100b921,
    /*6d44*/ 0xbf00460a,
    /*6d48*/ 0xbbb2f625, //jump a002c4b0
    /*6d4c*/ 0x55902000,
    /*6d50*/ 0xbbdaf625, //jump a002c508
    /*6d54*/ 0xc0006d34, //lmp_no_buf
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_32 =
{
    32,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch32_ins_data),
    0x0002c4ac,
    0xbc46f1da, //jump a002c4ac -> a0206d3c
    0xc0006d3c,
    (uint8_t *)bes1400_t4_ibrt_patch32_ins_data
};//ld_acl_sched

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_33 =
{
    33,
    BTDRV_PATCH_ACT,
    0,
    0x0003884c,
    0xbf00e008,
    0,
    NULL,
};// ld_sniffer_update_status: SNIFFER_EVT_SCO_CONNECTED

const uint32_t bes1400_t4_ibrt_patch34_ins_data[] =
{
    /*6d5c*/ 0x4a042302,
    /*6d60*/ 0x30b4f884,
    /*6d64*/ 0x55902001,
    /*6d68*/ 0xbf002000,
    /*6d6c*/ 0xb9fef629, //jump a003016c
    /*6d70*/ 0xc0006d74,
    /*6d74*/ 0x00000000,
};
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_34 =
{
    34,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch34_ins_data),
    0x00030164,
    0xbdfaf1d6, //jump a0030164 -> a0206d5c
    0xc0006d5c,
    (uint8_t *)bes1400_t4_ibrt_patch34_ins_data
};//ld_acl_stop

const uint32_t bes1400_t4_ibrt_patch35_ins_data[] =
{
    /*6d7c*/ 0x30bdf885,
    /*6d80*/ 0xd10f2b00,
    /*6d84*/ 0x20004a08,
    /*6d88*/ 0xb1585d10,
    /*6d8c*/ 0x20b4f895,
    /*6d90*/ 0xd1042a01,
    /*6d94*/ 0xb4084620,
    /*6d98*/ 0xf9b2f629, //call a0030100
    /*6d9c*/ 0x2002bc08,
    /*6da0*/ 0x00b4f885,
    /*6da4*/ 0xbd6cf628, //jump a002f880
    /*6da8*/ 0xc0006d74,
};
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_35 =
{
    35,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch35_ins_data),
    0x0002f87c,
    0xba7ef1d7, //jump a002f87c -> a0206d7c
    0xc0006d7c,
    (uint8_t *)bes1400_t4_ibrt_patch35_ins_data
};//ld_acl_frm_isr

const uint32_t bes1400_t4_ibrt_patch36_ins_data[] =
{
    /*6dac*/ 0x46884607,
    /*6db0*/ 0x23004a02,
    /*6db4*/ 0xbf005413,
    /*6db8*/ 0xb9e4f624, //jump a002b184
    /*6dbc*/ 0xc0006d74,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_36 =
{
    36,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch36_ins_data),
    0x0002b180,
    0xbe14f1db, //jump a002b180 -> a0206dac
    0xc0006dac,
    (uint8_t *)bes1400_t4_ibrt_patch36_ins_data
};//ld_acl_end

const uint32_t bes1400_t4_ibrt_patch37_ins_data[] =
{
    /*6dc0*/0xfa4af631,
    /*6dc4*/0x20004601,
    /*6dc8*/0xfa32f633,
    /*6dcc*/0xb978f618,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_37 =
{
    37,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch37_ins_data),
    0x0001f0bc,
    0xbe80f1e7,//jump a001f0bc -> A0206dc0
    0xc0006dc0,
    (uint8_t *)bes1400_t4_ibrt_patch37_ins_data
};//ibbt slave stop sco

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_38 =
{
    38,
    BTDRV_PATCH_ACT,
    0,
    0x00029e70,
    0xbf8c7f80,
    0,
    NULL,
};//ld_acl_rx_sync2

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_39 =
{
    39,
    BTDRV_PATCH_ACT,
    0,
    0x0002a068,
    0xbf947f80,
    0,
    NULL,
};//ld_acl_rx_no_sync

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_40 =
{
    40,
    BTDRV_PATCH_ACT,
    0,
    0x0002a1b0,
    0x7f80f5b3,
    0,
    NULL,
};//ld_acl_rx_sync

const uint32_t bes1400_t4_ibrt_patch41_ins_data[] =
{
    0x4606b570,/*6dd4*/
    0x7e8d460c,
    0xf5f5fb92,
    0x2114b2e8,
    0x68e175b1,
    0xf023440b,
    0x7ee14378,
    0x6381ea83,
    0x1a5b7e61,
    0xfbb37ea1,
    0xb2dbf3f1,
    0xf1f0fbb3,
    0x3311fb00,
    0xb2c31ac0,
    0xd8042a3c,
    0x622ef640,
    0x22048272,
    0xf640e003,
    0x8272124c,
    0x7ea02206,
    0xfb0068e1,
    0x44101003,
    0x4078f020,
    0x210060b0,
    0xf92ef63c,
    0xd9072806,
    0xb2ed7ea3,
    0xfb0568b2,
    0xf0252503,
    0x60b54578,
    0xbf00bd70,
    0x20b3f893,
    0xd1092a01,
    0x46514620,
    0x2042f8b8,
    0x3088f8d3,
    0xffb6f7ff,
    0xbe9ef624,
    0xbee8f624,/*6e6c*/
};


const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_41 =
{
    41,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch41_ins_data),
    0x0002bac4,
    0xb9c4f1db, //jump a002bac4 -> A0206E50
    0xc0006dd4,
    (uint8_t *)bes1400_t4_ibrt_patch41_ins_data
};//ibrt fix acl interval in sco

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_42 =
{
    42,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch2e_ins_data),
    0x0001fdb4,
    0xbd86f1e6,//jump a001fdb4 -> a02068c4
    0xc00068c4,
    (uint8_t *)bes1400_t4_ibrt_patch2e_ins_data
};//lc_op_loc_sniff_req

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_43 =
{
    43,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000389c8,
    0xbf00e00e,
    0,
    NULL
}; //check afh mismatch 2

const uint32_t bes1400_t4_ibrt_patch44_ins_data[] =
{
    /*6e74*/ 0xf9b2f5fc,
    /*6e78*/ 0x3042f8b0,
    /*6e7c*/ 0xbf8c2b3c,
    /*6e80*/ 0x20024620,
    /*6e84*/ 0xbf00bd38,
};


const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_44 =
{
    44,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch44_ins_data),
    0x0002dad4,
    0xb9cef1d9, //jump a002dad4 -> a0206e74
    0xc0006e74,
    (uint8_t *)bes1400_t4_ibrt_patch44_ins_data
};//retx manage

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_45 =
{
    45,
    BTDRV_PATCH_ACT,
    0,
    0x0001ff00,
    0xbf00e007,
    0,
    NULL
}; //KE_MSG_HANDLER(lc_op_loc_switch_req, void) use null ptr

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_46 =
{
    46,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0001bb98, //a001bb98
    0xbff2f7ff, //jump a001bb80
    0,
    NULL
};//LMP_MSG_HANDLER(switch_req)

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_47 =
{
    47,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0002089c,
    0xf640bf00,
    0,
    NULL
};//no disconnect evt

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_48 =
{
    48,
#if defined(IBRT)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x0000fa6c,
    0xf028bf00,
    0,
    NULL
};//filter tws link lc_sco_ind ke message

const uint32_t bes1400_t4_ibrt_patch49_ins_data[] =
{
    /*6f40*/ 0x46286825,
    /*6f44*/ 0x303ef895,
    /*6f48*/ 0xd0012b02,
    /*6f4c*/ 0xb8c4f622, //jump a00290d8
    /*6f50*/ 0xbf002300,
    /*6f54*/ 0x303ef885,
    /*6f58*/ 0xf9c4f622, //call a00292e4
    /*6f5c*/ 0xb8e3f622, //jump a0029126
};
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_49 =
{
    49,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch49_ins_data),
    0x000290d4,
    0xbf34f1dd,//jump a00290d4 -> a0206f40
    0xc0006f40,
    (uint8_t *)bes1400_t4_ibrt_patch49_ins_data
};//ld_pscan_frm_cbk

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_50 =
{
    50,
    BTDRV_PATCH_INACT,
    0,
    0,
    0,
    0,
    NULL,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_51 =
{
    51,
    BTDRV_PATCH_ACT,
    0,
    0x00006ec4,
    0xb9bef000, //jump a0007244
    0,
    NULL,
};//DEF_IRQHandler

const uint32_t bes1400_t4_ibrt_patch52_ins_data[] =
{
    /*6f60*/ 0xf8534b05,
    /*6f64*/ 0xb12e6027,
    /*6f68*/ 0x3025f853,
    /*6f6c*/ 0xb10bbf00,
    /*6f70*/ 0xbf6bf610, //jump a0017e4a
    /*6f74*/ 0xbf87f610, //jump a0017e86
    /*6f78*/ 0xc0005c70,
};
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_52 =
{
    52,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch52_ins_data),
    0x00017e40,
    0xb88ef1ef,//jump a0017e40 -> a0206f60
    0xc0006f60,
    (uint8_t *)bes1400_t4_ibrt_patch52_ins_data
};//lc_connect_info_req_handler

const uint32_t bes1400_t4_ibrt_patch53_ins_data[] =
{
    /*6f80*/ 0x210c4638,
    /*6f84*/ 0xfd6ef61b,
    /*6f88*/ 0xb9b7f632,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_53 =
{
    53,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch53_ins_data),
    0x00039294,
    0xbe74f1cd,//jump a0039294 -> a0206f80
    0xc0006f80,
    (uint8_t *)bes1400_t4_ibrt_patch53_ins_data
};//ld_send_tws_switch_cmd_after_both_ready(assert)

const uint32_t bes1400_t4_ibrt_patch54_ins_data[] =
{
    /*6f90*/ 0xfd24f632,
    /*6f94*/ 0xf6312001,
    /*6f98*/ 0x4620fa15,
    /*6f9c*/ 0xfa1ef631,
    /*6fa0*/ 0xbe58f632,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_54 =
{
    54,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch54_ins_data),
    0x00039c50,
    0xb99ef1cd,//jump a0039c50 -> a0206f90
    0xc0006f90,
    (uint8_t *)bes1400_t4_ibrt_patch54_ins_data
};//filter 1st ibrt packet

const uint32_t bes1400_t4_ibrt_patch55_ins_data[] =
{
    /*6fa8*/ 0x5023f852,
    /*6fac*/ 0xd0012d00,
    /*6fb0*/ 0xbf12f610,// jump a0206fb0 -> a0017dd8
    /*6fb4*/ 0xbd382000,
};
const BTDRV_PATCH_STRUCT bes1400_t4_ibrt_ins_patch_55 =
{
    55,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ibrt_patch55_ins_data),
    0x00017dd4,
    0xb8e8f1ef, // jump a001f414 -> a0206b54
    0xc0006fa8,
    (uint8_t *)bes1400_t4_ibrt_patch55_ins_data
};//protect null ptr when send afh


static const uint32_t best1400_t4_ins_patch_ibrt_config[] =
{
    56,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_0,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_1,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_2,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_3,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_4,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_5,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_6,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_7,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_8,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_9,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_10,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_11,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_12,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_13,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_14,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_15,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_16,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_17,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_18,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_19,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_20,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_21,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_22,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_23,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_24,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_25,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_26,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_27,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_28,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_29,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_30,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_31,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_32,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_33,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_34,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_35,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_36,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_37,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_38,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_39,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_40,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_41,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_42,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_43,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_44,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_45,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_46,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_47,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_48,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_49,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_50,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_51,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_52,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_53,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_54,
    (uint32_t)&bes1400_t4_ibrt_ins_patch_55,
};
#endif //__BT_ONE_BRING_TWO__


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
    uint8_t i=0;
    remap_addr =   ins_patch_p->patch_remap_address | 1;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_REMAP_ADDR_START + ins_patch_p->patch_index*4),
                       (uint8_t *)&ins_patch_p->patch_remap_value,4);
    if(ins_patch_p->patch_length != 0)  //have ram patch data
    {
        for( ; i<(ins_patch_p->patch_length)/128; i++)
        {
            btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,
                               (ins_patch_p->patch_data+i*128),128);
        }

        btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,ins_patch_p->patch_data+i*128,
                           ins_patch_p->patch_length%128);
    }

    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_COMP_ADDR_START + ins_patch_p->patch_index*4),
                       (uint8_t *)&remap_addr,4);

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
    enum HAL_CHIP_METAL_ID_T metal_id;
    const BTDRV_PATCH_STRUCT *ins_patch_p;

    metal_id = hal_get_chip_metal_id();

    if(metal_id == HAL_CHIP_METAL_ID_0)
    {
        for(uint8_t i=0; i<best1400_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_ins_patch_config[i+1]);
            }
            if (1 == i)
            {
                /*when intersys patch send done,enable patch directly to avoid interrupt missing*/
                btdrv_patch_en(1);
                btdrv_delay(100);
                hal_intersys_peer_irq_auto_clear(false);
            }
        }
    }
    else if(metal_id == HAL_CHIP_METAL_ID_1)
    {
#ifndef IBRT
        for(uint8_t i=0; i<best1400_t4_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_t4_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_t4_ins_patch_config[i+1]);
            }
        }
#else
        for(uint8_t i=0; i<best1400_t4_ins_patch_ibrt_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_t4_ins_patch_ibrt_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_t4_ins_patch_ibrt_config[i+1]);
            }
        }
#endif
        btdrv_patch_en(1);
    }
    else if(metal_id == HAL_CHIP_METAL_ID_2 || metal_id == HAL_CHIP_METAL_ID_3)
    {
#if defined(BLE_MESH)
        for(uint8_t i=0; i<best1400_t5_mesh_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_t5_mesh_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_t5_mesh_ins_patch_config[i+1]);
            }
        }
#elif !defined(IBRT)
        for(uint8_t i=0; i<best1400_t5_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_t5_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_t5_ins_patch_config[i+1]);
            }
        }
#else
        for(uint8_t i=0; i<best1400_t5_ibrt_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_t5_ibrt_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_t5_ibrt_ins_patch_config[i+1]);
            }
        }
#endif
        btdrv_patch_en(1);
    }
    else if(metal_id == HAL_CHIP_METAL_ID_4 ||metal_id == HAL_CHIP_METAL_ID_5)
    {
#ifdef IBRT
        for(uint8_t i=0; i<best1400_t6_ibrt_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_t6_ibrt_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_t6_ibrt_ins_patch_config[i+1]);
            }
        }
#else
        for(uint8_t i=0; i<best1400_t6_ins_patch_config[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best1400_t6_ins_patch_config[i+1];
            if(ins_patch_p->patch_state == BTDRV_PATCH_ACT)
            {
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best1400_t6_ins_patch_config[i+1]);
            }
        }
#endif
        btdrv_patch_en(1);
    }
    else
    {
        ASSERT(0, "%s:error chip id=%d", __func__, hal_get_chip_metal_id());
    }

}

static const uint32_t bes1400_dpatch0_data[] =
{
    0x4000007C,
};

static const BTDRV_PATCH_STRUCT bes1400_data_patch0 =
{
    0,
    BTDRV_PATCH_INACT,
    sizeof(bes1400_dpatch0_data),
    0x00006EF0,
    0xc0006720,
    0xc0006720,
    (uint8_t *)&bes1400_dpatch0_data
};

static const BTDRV_PATCH_STRUCT bes1400_data_patch1 =
{
    1,
    BTDRV_PATCH_INACT,
    sizeof(bes1400_dpatch0_data),
    0x00006F70,
    0xc0006720,
    0xc0006720,
    (uint8_t *)&bes1400_dpatch0_data
};

static const uint32_t best1400_data_patch_config[] =
{
    2,
    (uint32_t)&bes1400_data_patch0,
    (uint32_t)&bes1400_data_patch1,
};
/*****************************************************************************
 Prototype    : btdrv_data_patch_write
 Description  :  btdrv data patch write function
 Input        : const BTDRV_PATCH_STRUCT *d_patch_p
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2018/12/6
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
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
/*****************************************************************************
 Prototype    : btdrv_data_patch_init
 Description  : btdrv data data patch init function
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
void btdrv_data_patch_init(void)
{
    enum HAL_CHIP_METAL_ID_T metal_id;
    const BTDRV_PATCH_STRUCT *data_patch_p;

    metal_id = hal_get_chip_metal_id();

    if(metal_id == HAL_CHIP_METAL_ID_0)
    {
        for(uint8_t i=0; i<best1400_data_patch_config[0]; i++)
        {
            data_patch_p = (BTDRV_PATCH_STRUCT *)best1400_data_patch_config[i+1];
            if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
                btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)best1400_data_patch_config[i+1]);
        }
    }
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
    uint32_t value[2];

    //set patch enable
    value[0] = 0x2f02 | en;
    //set patch remap address  to 0xc0000100
    value[1] = 0x20000100;
    btdrv_write_memory(_32_Bit,BTDRV_PATCH_EN_REG,(uint8_t *)&value,8);
}

/////////////1400 t6 test mode patch///////////////////////

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x0002ceb4,
    0xe00b0f0f,
    0,
    NULL
};//ld_acl_rx() average rssi

const uint32_t bes1400_t6_ins_patch_data_testmode_1[] =
{
    /*6a98*/ 0x99049803,
    /*6a9c*/ 0x4b12b470,
    /*6aa0*/ 0x010e5c5a,
    /*6aa4*/ 0x19734d11,
    /*6aa8*/ 0x78244c11,
    /*6aac*/ 0xd20342a2,
    /*6ab0*/ 0x32015498,
    /*6ab4*/ 0xe001b2d2,
    /*6ab8*/ 0x22015570,
    /*6abc*/ 0x5442480a,
    /*6ac0*/ 0x461ab14c,
    /*6ac4*/ 0x23001919,
    /*6ac8*/ 0x0b01f912,
    /*6acc*/ 0xbf004403,
    /*6ad0*/ 0xd1f9428a,
    /*6ad4*/ 0x2300e000,
    /*6ad8*/ 0xf0f4fb93,
    /*6adc*/ 0xbc70b240,
    /*6ae0*/ 0x22019904,
    /*6ae4*/ 0xba00f626, //jump a0206ae4 -> a002cee8
    /*6ae8*/ 0xc0006af8, //addr of rssi_idx_for_agc
    /*6aec*/ 0xc0006afc, //addr of rssi_set_for_agc
    /*6af0*/ 0xc0006af4, //addr of N
    /*6af4*/ 0x00000005, //N : 1 ~ 16
    /*6af8*/ 0x00000000, //rssi_idx_for_agc[3]
    /*6afc*/ 0xbabababa, //rssi_set_for_agc[3*16]
    /*6b00*/ 0xbabababa,
    /*6b04*/ 0xbabababa,
    /*6b08*/ 0xbabababa,
    /*6b0c*/ 0xbabababa,
    /*6b10*/ 0xbabababa,
    /*6b14*/ 0xbabababa,
    /*6b18*/ 0xbabababa,
    /*6b1c*/ 0xbabababa,
    /*6b20*/ 0xbabababa,
    /*6b24*/ 0xbabababa,
    /*6b28*/ 0xbabababa,
    /*6b2c*/ 0xbabababa,
};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t6_ins_patch_data_testmode_1),
    0x0002cee4,
    0xbdd8f1d9, //jump a002cee4 -> a0206a98
    0xc0006a98,
    (uint8_t *)bes1400_t6_ins_patch_data_testmode_1
};//ld_acl_rx() average rssi

const uint32_t bes1400_t6_ins_patch_data_testmode_2[] =
{
    0x2028f843,/*6b3c*/
    0xf8434b08,
    0x4b052028,
    0x4b0e701a,
    0x2201701a,
    0xf8434b08,
    0xbf002028,
    0xba32f624,
    0xc0006b60,
    0x00000000,
    0xc0006b68,
    0x00000000,
    0x00000000,
    0x00000000,
    0xc0006b78,
    0x00000001,
    0x00000001,
    0x00000001,
    0xc0006b88,
    0x00000000,/*6b88*/

};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t6_ins_patch_data_testmode_2),
    0x0002afbc,
    0xbdbef1db,
    0xc0006b3c,
    (uint8_t *)bes1400_t6_ins_patch_data_testmode_2
};//sync clear count

const uint32_t bes1400_t6_ins_patch_data_testmode_3[] =
{

    0x47f0e92d,/*7050*/
    0x4b97b082,
    0x236e5c1e,
    0xf300fb03,
    0x5a9b4a95,
    0x4f00f413,
    0x8120f040,
    0x46044689,
    0x1800ea4f,
    0xeb084f91,
    0x4b910507,
    0xa020f853,
    0xf8524a90,
    0x33013020,
    0x3020f842,
    0xf860f635,
    0x42a04601,
    0x8088f040,
    0xf8534b8a,
    0x4b8a2024,
    0x429a681b,
    0xf8dad817,
    0xebc22084,
    0xf0230309,
    0xf1b34378,
    0xd9046f80,
    0x0302ebc9,
    0x4378f023,
    0x4a82425b,
    0x0024f852,
    0x68124a81,
    0xf200fb02,
    0xf2404293,
    0x4b7f80eb,
    0x3000f993,
    0x4b7eb9c3,
    0xc000f893,
    0x0f00f1bc,
    0x462ad00a,
    0x0e0ceb05,
    0xf9122300,
    0x44030b01,
    0xd1fa4572,
    0xe0014662,
    0x46132200,
    0xfcfcfb93,
    0xf8804872,
    0xe001c000,
    0x46132200,
    0xf890486f,
    0x4870e000,
    0x9000f890,
    0xf890486f,
    0xfa4fc000,
    0xebc9f08e,
    0xfa4f0000,
    0x4550fa8c,
    0xebc9dd05,
    0x48660e0e,
    0xe000f880,
    0x4864e002,
    0xc000f880,
    0x0e00f04f,
    0xf840485d,
    0xf8dfe024,
    0xf85ee178,
    0x30010024,
    0x0024f84e,
    0x7800485d,
    0xd2054286,
    0x783f4f5a,
    0x360155af,
    0xe004b2f6,
    0x78364e57,
    0x6007f808,
    0x4f4d2601,
    0x4282553e,
    0x442ada06,
    0xf9124405,
    0x44234b01,
    0xd1fa42aa,
    0xfb934a53,
    0x9200f0f0,
    0xb2406a14,
    0x23002201,
    0xe08147a0,
    0xf8534b46,
    0x4b4e2024,
    0x429a681b,
    0xf8dad816,
    0xebc22084,
    0xf0230309,
    0xf1b34378,
    0xd9046f80,
    0x0302ebc9,
    0x4378f023,
    0x4a3e425b,
    0x1024f852,
    0x68124a44,
    0xf201fb02,
    0xd9634293,
    0xf9934b42,
    0xb9c33000,
    0xf8934b3a,
    0xf1bee000,
    0xd00a0f00,
    0xeb05462a,
    0x2300000e,
    0x1b01f912,
    0x4282440b,
    0x4672d1fa,
    0x2200e001,
    0xfb934613,
    0x4936fefe,
    0xe000f881,
    0x2200e001,
    0x49334613,
    0x49337808,
    0xc000f891,
    0xf8914932,
    0xb241e000,
    0x0101ebcc,
    0xf98efa4f,
    0xdd044549,
    0x0000ebcc,
    0x7008492a,
    0x4929e002,
    0xe000f881,
    0x491c2000,
    0x0024f841,
    0xf850481c,
    0x31011024,
    0x1024f840,
    0x7808491c,
    0xd2054286,
    0x78094920,
    0x360155a9,
    0xe004b2f6,
    0x7809491d,
    0x1007f808,
    0x490c2601,
    0x4282550e,
    0x442ada06,
    0xf9124405,
    0x440b1b01,
    0xd1fa42aa,
    0xfb934a12,
    0x9200f0f0,
    0xb2406a15,
    0x22014621,
    0x47a82300,
    0xe8bdb002,
    0xbf0087f0,
    0xc0006af8,//rssi_idx_for_agc[link_id]  shenxin
    0xd02111f8,//EM_BT_RD(EM_BT_RXBIT_ADDR
    0xc0006afc,//rssi_set_for_agc   shenxin
    0xc00009d4,//ld_acl_env[link_id]
    0xc0006b68,//link_no_sync_count[link_id]
    0xc00072fc,//link_agc_thd_tws
    0xc0006b78,//link_no_sync_count_time[link_id]
    0xc0007300,//link_agc_thd_tws_time
    0xc0006b60,//rssi_store_tws
    0xc0006af4,//n   shenxin
    0xc0007304,//rssi_tws_step
    0xc0007308,//rssi_min_value_tws
    0xc0004268,//rwip_rf.rxgain_cntl
    0xc000730c,//link_agc_thd_mobile
    0xc0007310,//link_agc_thd_mobile_time
    0xc0006b88,//rssi_store_mobile
    0xc0007314,//rssi_mobile_step
    0xc0007318,//rssi_min_value_mobile
    0xffffffff,//link_agc_thd_tws
    0x00000030,//link_agc_thd_tws_time
    0x00000014,//rssi_tws_step
    0x0000009c,//rssi_min_value_tws
    0xffffffff,//link_agc_thd_mobile
    0x00000030,//link_agc_thd_mobile_time
    0x00000014,//rssi_mobile_step
    0x000000a1,//rssi_min_value_mobile
    0x99029804,
    0xfe96f7ff,
    0xfc48f632,
    0xb898f626,/*7328*/
};

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t6_ins_patch_data_testmode_3),
    0x0002d458,
    0xbf60f1d9,
    0xc0007050,
    (uint8_t *)bes1400_t6_ins_patch_data_testmode_3
};//swagc no sync

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_4 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x0000e47a,
    0xe0027b20,
    0,
    NULL
};//lm_inq_res_ind_handler()  rssi

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_5 =
{
    5,
    BTDRV_PATCH_INACT,
    0,
    0x0002ad78,
    0xbf00e009,
    0,
    NULL
};//ld_acl_rx_no_sync()  rssi

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_6 =
{
    6,
    BTDRV_PATCH_ACT,
    0,
    0x00000880,
    0x26004c0e,
    0,
    NULL,
};//rwip_env.sleep_enable=false after hci reset

const uint32_t bes1400_t6_ins_patch_data_testmode_7[] =
{
    0xf8802300,
    0xf8803024,
    0xbf003025,
    0x30b3f894,
    0xbc18f62d,
};/* 6b90-6ba0 */

const BTDRV_PATCH_STRUCT bes1400_t6_ins_patch_testmode_7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t6_ins_patch_data_testmode_7),
    0x000343d0,
    0xbbdef1d2,
    0xc0006b90,
    (uint8_t *)bes1400_t6_ins_patch_data_testmode_7
};//nosig test pattern problem

static const uint32_t ins_patch_1400_t6_config_testmode[] =
{
    8,
    (uint32_t)&bes1400_t6_ins_patch_testmode_0,
    (uint32_t)&bes1400_t6_ins_patch_testmode_1,
    (uint32_t)&bes1400_t6_ins_patch_testmode_2,
    (uint32_t)&bes1400_t6_ins_patch_testmode_3,
    (uint32_t)&bes1400_t6_ins_patch_testmode_4,
    (uint32_t)&bes1400_t6_ins_patch_testmode_5,
    (uint32_t)&bes1400_t6_ins_patch_testmode_6,
    (uint32_t)&bes1400_t6_ins_patch_testmode_7,
};

/*****************************************************************************
 Patch Description  : lc_get_ptt will always return 0 when in no-signal rx mode
 Patch function     : lc_get_ptt
 Patch Line         : line4428
 Patch Level        : test mode patch, not used in offical release

 History      :
 Date         : 2018/12/12
 Author       : lufang.qin
*****************************************************************************/
const uint32_t bes1400_ins_patch_testmode_data[] =
{
    /*60*/ 0xbf002000,//mov r0 0
    /*64*/ 0xbf004b02,//ldr r3,[pc,#2]
    /*68*/ 0xbf006018,//str r0 [r3,#0]
    /*6c*/ 0xba90f624,//B 0xa002ac90
    /*70*/ 0xc0006774,//data
    /*74*/ 0xabcdabcd,//data
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch_testmode =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_ins_patch_testmode_data),
    0x0002ac8c,
    0xbd68f1db,
    0xc0006760,
    (uint8_t *)bes1400_ins_patch_testmode_data
};
/*****************************************************************************
 Prototype    : btdrv_ins_patch_test_init
 Description  : test mode patch init function
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
const uint32_t bes1400_patch3_ins_test_data[] =
{
    0xf0025bf2,
    0xbf00080f,
    0xbc05f622,
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch3_testmode =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch3_ins_test_data),
    0x00028f30,
    0xbbf6f1dd,
    0xc0006720,
    (uint8_t *)bes1400_patch3_ins_test_data
};


const uint32_t bes1400_patch4_ins_test_data[] =
{
    0xf0025bf2,
    0xbf00020f,
    0xbc22f622,
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch4_testmode =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch4_ins_test_data),
    0x00028f7c,
    0xbbd8f1dd,
    0xc0006730,
    (uint8_t *)bes1400_patch4_ins_test_data
};



const uint32_t bes1400_patch5_ins_test_data[] =
{
    0xf0035bf3,
    0xbf00080f,
    0xbc32f622,

};

const BTDRV_PATCH_STRUCT bes1400_ins_patch5_testmode =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch5_ins_test_data),
    0x00028fac,
    0xbbc8f1dd,
    0xc0006740,
    (uint8_t *)bes1400_patch5_ins_test_data
};



const uint32_t bes1400_patch6_ins_test_data[] =
{
    0xf0035bf3,
    0xbf00010f,
    0xbc52f622,

};

const BTDRV_PATCH_STRUCT bes1400_ins_patch6_testmode =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch6_ins_test_data),
    0x00028ffc,
    0xbba8f1dd,
    0xc0006750,
    (uint8_t *)bes1400_patch6_ins_test_data
};


const uint32_t bes1400_patch7_ins_test_data[] =
{
    0x22445243,
    0x601a4b06,
//   0x020cf641,
    0x020cf243,
    0x601a3308,
    0x33382204,
    0x220d601a,
    0x601a3304,
    0xfb04f85d,
    0xd0350300,

};

const BTDRV_PATCH_STRUCT bes1400_ins_patch7_testmode =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch7_ins_test_data),
    0x000038c8,
    0xbf4af202,
    0xc0006760,
    (uint8_t *)bes1400_patch7_ins_test_data
};


const uint32_t bes1400_patch8_ins_test_data[] =
{
    0xd00e2b02,
    0x4b0f2244,
//    0xf641601a,
    0xf242601a,
//   0x33080209,
    0x33080208,
    0x2204601a,
    0x601a3338,
    //  0x33042209,
    0x33042208,
    0x2000601a,
    0x2244e00d,
    0x601a4b07,
//    0x020cf641,
    0x020cf243,
    0x601a3308,
    0x33382204,
    0x220d601a,
    0x601a3304,
    0xf85d2001,
    0x47704b04,
    0xd0350300,

};

const BTDRV_PATCH_STRUCT bes1400_ins_patch8_testmode =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch8_ins_test_data),
    0x000035a0,
    0xb8f0f203,
    0xc0006784,
    (uint8_t *)bes1400_patch8_ins_test_data
};


const uint32_t bes1400_patch9_ins_test_data[] =
{
    0xf0035243,
    0x2b00030f,
    0x2244d00e,
    0x601a4b0e,
//    0x0209f641,
    0x0208f242,
    0x601a3308,
    0x33382204,
//    0x2209601a,
    0x2208601a,
    0x601a3304,
    0xe00d2000,
    0x4b072244,
    //  0xf641601a,
    0xf241601a,
//    0x33080206,
    0x33084205,
    0x2204601a,
    0x601a3338,
//   0x33042206,
    0x33042205,
    0x2001601a,
    0xbf004770,
    0xd0350300,


};

const BTDRV_PATCH_STRUCT bes1400_ins_patch9_testmode =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch9_ins_test_data),
    0x00003568,
    0xb932f203,
    0xc00067d0,
    (uint8_t *)bes1400_patch9_ins_test_data
};

////hw loopback disable and en
const uint32_t bes1400_patch10_ins_test_data[] =
{
    0x30fcf8d4,
    0xd0092b00,
    0x680b4905,
    0x3f00f413,
    0x680bd004,
    0x3300f423,
    0xbf00600b,
    0xbf48f626,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch10_testmode =
{
    10,
    BTDRV_PATCH_INACT,
    sizeof(bes1400_patch10_ins_test_data),
    0x0002d6c8,
    0xb8aaf1d9,
    0xc0006820,
    (uint8_t *)bes1400_patch10_ins_test_data
};



const uint32_t bes1400_patch11_ins_test_data[] =
{
    0x30fcf8d4,
    0xd00b2b00,
    0xf003789b,
    0x2b0503fd,
    0x4b04d106,
    0xf443681b,
    0x4a023300,
    0xbf006013,
    0xba9ff625,
    0xd02200a4,
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch11_testmode =
{
    11,
    BTDRV_PATCH_INACT,
    sizeof(bes1400_patch11_ins_test_data),
    0x0002bcd0,
    0xbdbef1da,
    0xc0006850,
    (uint8_t *)bes1400_patch11_ins_test_data
};


const uint32_t bes1400_patch12_ins_test_data[] =
{
    /*687c*/0x6d584b07,
    /*6880*/0xbf002800,
    /*6884*/0xbf00d108,
    /*6888*/0xbf004805,
    /*688c*/0xbf004905,
    /*6890*/0xff86f606,
    /*6894*/0xbf004802,
    /*6898*/0xb932f605,
    /*689c*/0xc00059f0,
    /*68a0*/0xc00068a4,
    /*68a4*/0x00000c13,
    /*68a8*/0x41004142
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch12_testmode =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch12_ins_test_data),
    0x0000bafc,
    0xbebef1fa,//jump 0xa000bafc 0xa020687c
    0xc000687c,
    (uint8_t *)bes1400_patch12_ins_test_data
};//write local name when name_ptr is null pointer


const uint32_t bes1400_patch13_ins_test_data[] =
{
    /*68b0*/ 0xbf004b05,
    /*68b4*/ 0x3020f853,
    /*68b8*/ 0xbf002b00,
    /*68bc*/ 0xbf00d002,
    /*68c0*/ 0xfd7cf60f,
    /*68c4*/ 0xb9e4f624,
    /*68c8*/ 0xc0005b64,
};
const BTDRV_PATCH_STRUCT bes1400_ins_patch13_testmode =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch13_ins_test_data),
    0x0002ac8c,
    0xbe10f1db,//jump 0xa002ac8c 0xa02068b0
    0xc00068b0,
    (uint8_t *)bes1400_patch13_ins_test_data
};//no sig_rx lc_get_ptt assert

////for power control test mode
const BTDRV_PATCH_STRUCT bes1400_ins_patch14_testmode =
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0x00028d1c,
    0xe0cfbf00,
    0,
    NULL
};

const uint32_t bes1400_patch15_ins_test_data[] =
{
    0xd02f2b00,
    0x33fff10b,
    0x2b01b2db,
    0xf8d6d827,
    0x788b10fc,
    0x03fdf003,
    0xd1202b05,
    0x68134a14,
    0x60133301,
    0xf8834b13,
    0x9b058000,
    0x02c9f3c3,
    0x801a4b11,
    0xf8834b11,
    0x8c08b000,
    0x4050f100,
    0x1004f500,
    0xf8934b08,
    0x011b31cb,
    0x5a594907,
    0xf101b289,
    0xf5014150,
    0xf63f1104,
    0x2401fe73,
    0xbeccf625,
    0xbe7cf625,
    0xc0005c28,
    0xd02115b0,
    0xc0006958,
    0xc000695c,
    0xc0006960,
    0xc0006964,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch15_testmode =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch15_ins_test_data),
    0x0002c634,
    0xb950f1da,
    0xc00068d8,
    (uint8_t *)bes1400_patch15_ins_test_data
};


const uint32_t bes1400_patch16_ins_test_data[] =
{
    0xbf00789a,
    0xb2d23a05,
    0xd9002a03,
    0x2a00e040,
    0x2a02d001,
    0xbf00d13e,
    0x681b4b1f,
    0xd0392b00,
    0xeb04bf00,
    0xf8370484,
    0xb29b3014,
    0x0378f023,
    0x88124a1a,
    0x03c2ea43,
    0xf827b29b,
    0x4b193014,
    0xf043681b,
    0x4a160304,
    0xea438812,
    0xb29b03c2,
    0x3014f82a,
    0x30fcf8d5,
    0x4a138c1b,
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
    0xb8daf625,
    0xbf6af624,
    0xb8dbf625,
    0xc0006958,
    0xc000695c,
    0xc0006960,
    0xc0006964,
    0xd02115ec,
};

const BTDRV_PATCH_STRUCT bes1400_ins_patch16_testmode =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch16_ins_test_data),
    0x0002b8d4,
    0xb850f1db,
    0xc0006978,
    (uint8_t *)bes1400_patch16_ins_test_data
};
////end for power control test mode


const uint32_t bes1400_patch17_ins_test_data[] =
{
    0x60bb9b01,
    0xfb00206e,
    0x4a0bf004,
    0xf0436813,
    0x60130302,
    0x4b092202,
    0xf5a3601a,
    0x3ba6436e,
    0x3003f830,
    0x030cf3c3,
    0x5300f443,
    0xf8204a04,
    0xbf003002,
    0xbe4ef624,
    0xD022000C,
    0xd0220018,
    0xd0211172,

};

const BTDRV_PATCH_STRUCT bes1400_ins_patch17_testmode =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_patch17_ins_test_data),
    0x0002b700,
    0xb996f1db,
    0xc0006a30,
    (uint8_t *)bes1400_patch17_ins_test_data
};


static const uint32_t ins_patch_1400_config_testmode[] =
{
    15,
    (uint32_t)&bes1400_ins_patch3_testmode,
    (uint32_t)&bes1400_ins_patch4_testmode,
    (uint32_t)&bes1400_ins_patch5_testmode,
    (uint32_t)&bes1400_ins_patch6_testmode,
    (uint32_t)&bes1400_ins_patch7_testmode,
    (uint32_t)&bes1400_ins_patch8_testmode,
    (uint32_t)&bes1400_ins_patch9_testmode,
    (uint32_t)&bes1400_ins_patch10_testmode,
    (uint32_t)&bes1400_ins_patch11_testmode,
    (uint32_t)&bes1400_ins_patch12_testmode,
    (uint32_t)&bes1400_ins_patch13_testmode,
    (uint32_t)&bes1400_ins_patch14_testmode,
    (uint32_t)&bes1400_ins_patch15_testmode,
    (uint32_t)&bes1400_ins_patch16_testmode,
    (uint32_t)&bes1400_ins_patch17_testmode,

};

/*****************************************************************************
CHIP ID                : 1400
Metal id               : 1
BT controller tag      : t4
*****************************************************************************/

const uint32_t bes1400_t4_ins_patch_data_testmode_0[] =
{
    /*6900*/ 0x6d5b4b0d,
    /*6904*/ 0xbf00b9a3, // cbnz local_name
    /*6908*/ 0x70132307,
    /*690c*/ 0x70032344,
    /*6910*/ 0x70432355,
    /*6914*/ 0x70832354,
    /*6918*/ 0x70c32331,
    /*691c*/ 0x71022234,
    /*6920*/ 0x71422230,
    /*6924*/ 0xbf007183,
    /*6928*/ 0x71c32300,
    /*692c*/ 0xb85ef606, // jump a000c9ec
    /*6930*/ 0x6d584b01,
    /*6934*/ 0xb81cf606, // jump a000c970
    /*6938*/ 0xc0005afc,
}; // send default name when null pointer

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_0),
    0x0000c96c,
    0xbfc8f1f9, // jump a000c96c -> a0206900
    0xc0006900,
    (uint8_t*)bes1400_t4_ins_patch_data_testmode_0
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_1 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x0002f438,
    0x789abf00,
    0,
    NULL
};///test mode: remove rxllid's judging condition in ld_acl_rx

const uint32_t bes1400_t4_ins_patch_data_testmode_2[] =
{
    0x20fcf8d5,
    0xf8928811,
    0xf0088007,
    0xb909080f,
    0x88194b11,
    0x0484eb04,
    0x3014f837,
    0xf423b29b,
    0x4a0f7300,
    0xea438812,
    0xb29b2342,
    0x3014f827,
    0x3014f837,
    0xf023b29b,
    0xea430378,
    0xf82708c8,
    0x4b068014,
    0xf043781b,
    0xea430304,
    0xb28b01c1,
    0x3014f82a,
    0xbe6af627,
    0xc000099c,////loopback_length
    0xc000097b,////loopback_llid
    0xc00069a4,////rxseq_flag
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_2),
    0x0002e604,
    0xb99cf1d8,
    0xc0006940,
    (uint8_t*)bes1400_t4_ins_patch_data_testmode_2
};///test mode: ld_acl_tx_prog


const uint32_t bes1400_t4_ins_patch_data_testmode_3[] =
{
    0xfc14f5fc,
    0x3060f890,
    0xd01f2b00,
    0x30fcf8db,
    0xd01b2b00,
    0x0f03f1b9,
    0xf1b9d018,
    0xd0150f08,
    0xf8934b0b,
    0x011b31cb,
    0x5a9b4a0a,
    0x7f80f413,
    0x4b09d10a,
    0x4b0a781a,
    0x4b08601a,
    0x4b09881a,
    0x2201601a,
    0x601a4b08,
    0xbc66f628,
    0xbc62f628,
    0xc0005d34,
    0xd02115f2,
    0xc000097b,////loopback_llid
    0xc000099c,////loopback_length
    0xc0006a1c,////rxllid_flag
    0xc0006a20,////rxlength_flag
    0xc0006a24,////unack_seqerr_flag
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_3),
    0x0002f2c0,
    0xbb76f1d7,
    0xc00069b0,
    (uint8_t*)bes1400_t4_ins_patch_data_testmode_3
};///test mode: skip rx seq err in ld_acl_rx

#if 0
const uint32_t bes1400_t4_ins_patch_data_testmode_4[] =
{
    0xf015d002,
    0xd0010f24,
    0xbc46f628,
    0xbd28f628,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_4),
    0x0002f2cc,
    0xbbb4f1d7,
    0xc0006a38,
    (uint8_t*)bes1400_t4_ins_patch_data_testmode_4
};///test mode: skip crc err in ld_acl_rx
#else
const uint32_t bes1400_t4_ins_patch_data_testmode_4[] =
{
    0xf015d006,
    0xd0050f24,
    0x0f03f1b9,
    0xbf00d002,
    0xbc42f628,
    0xbd24f628,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_4),
    0x0002f2cc,
    0xbbb4f1d7,
    0xc0006a38,
    (uint8_t*)bes1400_t4_ins_patch_data_testmode_4
};///test mode: skip crc err in ld_acl_rx

#endif

const uint32_t bes1400_t4_ins_patch_data_testmode_5[] =
{
    0x700d4914,
    0xf891490c,
    0x010911cb,
    0x5a8a4a0b,
    0x2240f3c2,
    0x6002480a,
    0x6800480a,
    0x2100b150,
    0x60014808,
    0x68014808,
    0x80014808,
    0x68014808,
    0x70014808,
    0xbce6f628,
    0xc0005d34,
    0xd02115f2,
    0xc00069a4,////rxseq_flag
    0xc0006a24,////unack_seqerr_flag
    0xc0006a20,////rxlength_flag
    0xc000099c,////loopback_length
    0xc0006a1c,////rxllid_flag
    0xc000097b,////loopback_llid
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_5),
    0x0002f45c,
    0xbafef1d7,
    0xc0006a5c,
    (uint8_t*)bes1400_t4_ins_patch_data_testmode_5
};


const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_6 =
{
    6,
    BTDRV_PATCH_ACT,
    0,
    0x0003a6e4,
    0x2005e004,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_7 =
{
    7,
    BTDRV_PATCH_ACT,
    0,
    0x00006f8c,
    0xbdf8bf00,
    0,
    NULL
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00006f9c,
    0xbf00e016,
    0,
    NULL
};//ignore assert warning


const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_9 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x00000100,
    0x20004b15,
    0,
    NULL,
};////rwip_env.sleep_enable=false after hci reset




const uint32_t bes1400_t4_ins_patch_data_testmode_10[] =
{
    0x4681b083,
    0x4b022200,
    0x2020f843,
    0xb842f623,
    0xc0007044,
    0x00000000,
    0x00000000,
    0x00000000,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_10),
    0x0002a0c0,
    0xbfb6f1dc,
    0xc0007030,
    (uint8_t *)bes1400_t4_ins_patch_data_testmode_10
};//swagc patch



const uint32_t bes1400_t4_ins_patch_data_testmode_11[] =
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
    0xbf4af622,
    0xc0007044,
    0xc0007094,
    0x00000010,
    0xc00041a0,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_11),
    0x00029f1c,
    0xb898f1dd,
    0xc0007050,
    (uint8_t *)bes1400_t4_ins_patch_data_testmode_11
};

const uint32_t bes1400_t4_ins_patch_data_testmode_12[] =
{
    0xb082b510,
    0x69e34c0b,
    0x47982000,
    0x6a249400,
    0x22002103,
    0x47a02301,
    0x49084a07,
    0xb29b8813,
    0x0307f023,
    0xb29b4303,
    0x3b0ef822,
    0xd1f5428a,
    0xbd10b002,
    0xc00041a0,
    0xd021051e,
    0xd021058e,
    0xffdef7ff,
    0x68134a01,
    0xb806f635,
    0xd0220200,
};

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_12 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t4_ins_patch_data_testmode_12),
    0x0003bb54,
    0xbff4f1ca,
    0xc0006b00,
    (uint8_t *)bes1400_t4_ins_patch_data_testmode_12
};///ble test mode sw gainidx=0

const BTDRV_PATCH_STRUCT bes1400_t4_ins_patch_testmode_13 =
{
    13,
    BTDRV_PATCH_ACT,
    0,
    0x0003ab1c,
    0xf7c62100,
    0,
    NULL,
};///ble tx test mode switch freq channel problem

static const uint32_t ins_patch_1400_t4_config_testmode[] =
{
    14,
    (uint32_t)&bes1400_t4_ins_patch_testmode_0,
    (uint32_t)&bes1400_t4_ins_patch_testmode_1,
    (uint32_t)&bes1400_t4_ins_patch_testmode_2,
    (uint32_t)&bes1400_t4_ins_patch_testmode_3,
    (uint32_t)&bes1400_t4_ins_patch_testmode_4,
    (uint32_t)&bes1400_t4_ins_patch_testmode_5,
    (uint32_t)&bes1400_t4_ins_patch_testmode_6,
    (uint32_t)&bes1400_t4_ins_patch_testmode_7,
    (uint32_t)&bes1400_t4_ins_patch_testmode_8,
    (uint32_t)&bes1400_t4_ins_patch_testmode_9,
    (uint32_t)&bes1400_t4_ins_patch_testmode_10,
    (uint32_t)&bes1400_t4_ins_patch_testmode_11,
    (uint32_t)&bes1400_t4_ins_patch_testmode_12,
    (uint32_t)&bes1400_t4_ins_patch_testmode_13,
};



const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x00000874,
    0x26004c0e,
    0,
    NULL,
};////rwip_env.sleep_enable=false after hci reset

const uint32_t bes1400_t5_ins_patch_data_testmode_1[] =
{
    0x4681b083,/*6b30*/
    0x4b022200,
    0x2020f843,
    0xbddcf623,
    0xc0006b44,
    0x00000000,
    0x00000000,
    0x00000000,/*6b4c*/
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_1=
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t5_ins_patch_data_testmode_1),
    0x0002a6f4,
    0xba1cf1dc,
    0xc0006b30,
    (uint8_t *)bes1400_t5_ins_patch_data_testmode_1
};//sync clear count

const uint32_t bes1400_t5_ins_patch_data_testmode_2[] =
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
    0xbce4f623,
    0xc0006b44,
    0xc0006b94,
    0x00000010,
    0xc00041b4,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t5_ins_patch_data_testmode_2),
    0x0002a550,
    0xbafef1dc,
    0xc0006b50,
    (uint8_t *)bes1400_t5_ins_patch_data_testmode_2
};


const uint32_t bes1400_t5_ins_patch_data_testmode_3[] =
{
    /*6f90*/ 0xbf009904,
    /*6f94*/ 0x4b12b470,
    /*6f98*/ 0x010e5c5a,
    /*6f9c*/ 0x19734d11,
    /*6fa0*/ 0x78244c11,
    /*6fa4*/ 0xd20342a2,
    /*6fa8*/ 0x32015498,
    /*6fac*/ 0xe001b2d2,
    /*6fb0*/ 0x22015570,
    /*6fb4*/ 0x5442480a,
    /*6fb8*/ 0x461ab14c,
    /*6fbc*/ 0x23001919,
    /*6fc0*/ 0x0b01f912,
    /*6fc4*/ 0xbf004403,
    /*6fc8*/ 0xd1f9428a,
    /*6fcc*/ 0x2300e000,
    /*6fd0*/ 0xf0f4fb93,
    /*6fd4*/ 0xbc70b240,
    /*6fd8*/ 0x22019904,
    /*6fdc*/ 0xbb8cf628, //jump a002f6f8
    /*6fe0*/ 0xc0006ff0, //addr of rssi_idx_for_agc
    /*6fe4*/ 0xc0006ff4, //addr of rssi_set_for_agc
    /*6fe8*/ 0xc0006fec, //addr of N
    /*6fec*/ 0x00000005, //N : 1 ~ 16
    /*6ff0*/ 0x00000000, //rssi_idx_for_agc[3]
    /*6ff4*/ 0xbabababa, //rssi_set_for_agc[3*16]
    /*6ff8*/ 0xbabababa,
    /*6ffc*/ 0xbabababa,
    /*7000*/ 0xbabababa,
    /*7004*/ 0xbabababa,
    /*7008*/ 0xbabababa,
    /*700c*/ 0xbabababa,
    /*7010*/ 0xbabababa,
    /*7014*/ 0xbabababa,
    /*7018*/ 0xbabababa,
    /*701c*/ 0xbabababa,
    /*7020*/ 0xbabababa,
    /*7024*/ 0xbabababa,
    /*7028*/ 0xbabababa,
    /*702c*/ 0xbabababa,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_3 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t5_ins_patch_data_testmode_3),
    0x0002f6f4,
    0xbc4cf1d7, //jump a002f6f4 -> a0206f90
    0xc0006f90,
    (uint8_t *)bes1400_t5_ins_patch_data_testmode_3
};


const uint32_t bes1400_t5_ins_patch_data_testmode_4[] =
{
    0x4620461d,
    0x463a4629,
    0xf5f94633,
    0xbdf8fc2b,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t5_ins_patch_data_testmode_4),
    0x000070ec,
    0xbe08f1ff,
    0xc0006d00,
    (uint8_t *)bes1400_t5_ins_patch_data_testmode_4
};//assert warn

const uint32_t bes1400_t5_ins_patch_data_testmode_5[] =
{
    0x4630461f,
    0x462a4639,
    0xf5f94623,
    0xbdf8fc21,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t5_ins_patch_data_testmode_5),
    0x00007070,
    0xbe50f1ff,
    0xc0006d14,
    (uint8_t *)bes1400_t5_ins_patch_data_testmode_5
};//assert param

const uint32_t bes1400_t5_ins_patch_data_testmode_6[] =
{
    0x41f0e92d,
    0x4c12b082,
    0x200069e3,
    0x94004798,
    0x21036a24,
    0x23012200,
    0x460547a0,
    0xf0204c0d,
    0xf8df0707,
    0x4e0c8038,
    0x4640b127,
    0x3135f240,
    0xf952f600,
    0xb29b8823,
    0x0307f023,
    0xb29b432b,
    0x3b0ef824,
    0xd1ef42b4,
    0xe8bdb002,
    0xbf0081f0,
    0xc00041b4,
    0xd021051e,
    0xd021058e,
    0xa00525b0,
    0xffcef7ff,
    0x68134a01,
    0xbe52f635,
    0xd0220200,
};

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t5_ins_patch_data_testmode_6),
    0x0003cc18,
    0xb9a8f1ca,
    0xc0006f0c,
    (uint8_t *)bes1400_t5_ins_patch_data_testmode_6
};///ble test mode fix swagc idx0

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_7 =
{
    7,
    BTDRV_PATCH_ACT,
    0,
    0x0003bbe0,
    0xf7c52100,
    0,
    NULL,
};///ble tx test mode switch freq channel problem

const uint32_t bes1400_t5_ins_patch_data_testmode_8[] =
{
    0xf8802300,
    0xf8803024,
    0xbf003025,
    0x30b3f894,
    0xbe0ef62c,
};/* 6d30-6d40 */

const BTDRV_PATCH_STRUCT bes1400_t5_ins_patch_testmode_8 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes1400_t5_ins_patch_data_testmode_8),
    0x0003395c,
    0xb9e8f1d3,
    0xc0006d30,
    (uint8_t *)bes1400_t5_ins_patch_data_testmode_8
};//nosig test pattern problem

static const uint32_t ins_patch_1400_t5_config_testmode[] =
{
    9,
    (uint32_t)&bes1400_t5_ins_patch_testmode_0,
    (uint32_t)&bes1400_t5_ins_patch_testmode_1,
    (uint32_t)&bes1400_t5_ins_patch_testmode_2,
    (uint32_t)&bes1400_t5_ins_patch_testmode_3,
    (uint32_t)&bes1400_t5_ins_patch_testmode_4,
    (uint32_t)&bes1400_t5_ins_patch_testmode_5,
    (uint32_t)&bes1400_t5_ins_patch_testmode_6,
    (uint32_t)&bes1400_t5_ins_patch_testmode_7,
    (uint32_t)&bes1400_t5_ins_patch_testmode_8,
};



void btdrv_ins_patch_test_init(void)
{
    enum HAL_CHIP_METAL_ID_T metal_id;
    const BTDRV_PATCH_STRUCT *ins_patch_p;

    metal_id = hal_get_chip_metal_id();

    if(metal_id == HAL_CHIP_METAL_ID_0 )
    {
        //btdrv_patch_en(0);
        btdrv_ins_patch_disable(3);
        btdrv_ins_patch_disable(4);
        btdrv_ins_patch_disable(5);
        btdrv_ins_patch_disable(6);
        btdrv_ins_patch_disable(7);
        btdrv_ins_patch_disable(8);
        btdrv_ins_patch_disable(9);
        btdrv_ins_patch_disable(10);
        btdrv_ins_patch_disable(11);
        btdrv_ins_patch_disable(12);
        btdrv_ins_patch_disable(13);

        for(uint8_t i=0; i<ins_patch_1400_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_1400_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_1400_config_testmode[i+1]);
        }
        //btdrv_patch_en(1);
    }
    else if (metal_id == HAL_CHIP_METAL_ID_1)
    {
        btdrv_patch_en(0);

        for (int i = 3; i < 56; i += 1)
        {
            btdrv_ins_patch_disable(i);
        }

        *(volatile uint8_t*)0xc0005873 = 0x00; // dbg_trace_level

        for(uint8_t i = 0; i < ins_patch_1400_t4_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_1400_t4_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_1400_t4_config_testmode[i+1]);
        }

        btdrv_patch_en(1);
    }
    else if (metal_id == HAL_CHIP_METAL_ID_2 || metal_id == HAL_CHIP_METAL_ID_3)
    {
        btdrv_patch_en(0);

        for (int i = 0; i < 56; i += 1)
        {
            btdrv_ins_patch_disable(i);
        }

        bt_drv_reg_op_for_test_mode_disable();

        for(uint8_t i = 0; i < ins_patch_1400_t5_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_1400_t5_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_1400_t5_config_testmode[i+1]);
        }

        btdrv_patch_en(1);
    }
    else if (metal_id == HAL_CHIP_METAL_ID_4 || metal_id == HAL_CHIP_METAL_ID_5)
    {
        btdrv_patch_en(0);

        for (int i = 0; i < 56; i += 1)
        {
            btdrv_ins_patch_disable(i);
        }

        bt_drv_reg_op_for_test_mode_disable();

        for(uint8_t i = 0; i < ins_patch_1400_t6_config_testmode[0]; i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_1400_t6_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_1400_t6_config_testmode[i+1]);
        }

        btdrv_patch_en(1);
        bt_drv_reg_op_disable_swagc_nosync_count();
    }
    else
    {
        ASSERT(0, "%s:error chip id=%d", __func__, hal_get_chip_metal_id());
    }
}
