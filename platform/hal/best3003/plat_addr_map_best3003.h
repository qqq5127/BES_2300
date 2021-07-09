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
#ifndef __PLAT_ADDR_MAP_BEST3003_H__
#define __PLAT_ADDR_MAP_BEST3003_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_BASE                                0x00000000

#ifndef ROM_SIZE
#define ROM_SIZE                                0x0000C000
#endif

#define RAM0_BASE                               0x20000000
#define RAMX0_BASE                              0x00200000
#define RAM1_BASE                               0x20008000
#define RAMX1_BASE                              0x00208000
#define RAM2_BASE                               0x20010000
#define RAMX2_BASE                              0x00210000
#define RAM3_BASE                               0x20018000
#define RAMX3_BASE                              0x00218000
#define RAM4_BASE                               0x20020000
#define RAMX4_BASE                              0x00220000
#define RAM5_BASE                               0x20028000
#define RAMX5_BASE                              0x00228000
#define RAM6_BASE                               0x20030000
#define RAMX6_BASE                              0x00230000
#define RAM_BASE                                RAM0_BASE
#define RAMX_BASE                               RAMX0_BASE

#ifndef RAM_SIZE
#define RAM_SIZE                                0x00038000
#endif

#define FLASH_BASE                              0x3C000000
#define FLASH_NC_BASE                           0x38000000
#define FLASHX_BASE                             0x0C000000
#define FLASHX_NC_BASE                          0x08000000

#define ICACHE_CTRL_BASE                        0x07FFE000
/* No data cache */

#define CMU_BASE                                0x40000000
#define GPIO_BASE                               0x40001000
#define WDT_BASE                                0x40002000
#define PWM_BASE                                0x40003000
#define TIMER0_BASE                             0x40004000
#define TIMER1_BASE                             0x40005000
#define I2C0_BASE                               0x40006000
#define SPI_BASE                                0x40007000
#define ISPI_BASE                               0x40008000
#define UART0_BASE                              0x40009000
#define UART1_BASE                              0x4000A000
#define I2S0_BASE                               0x4000B000
#define SPDIF0_BASE                             0x4000C000
#define IOMUX_BASE                              0x4000D000
#define FM_BASE                                 0x4000E000

#define AUDMA_BASE                              0x40130000
#define FLASH_CTRL_BASE                         0x40140000
#define I2C_SLAVE_BASE                          0x40160000
#define GPDMA_BASE                              0x40170000
#define USB_BASE                                0x40180000
#define FMDUMP0_BASE                            0x401E0000
#define FMDUMP1_BASE                            0x401F0000

#define CODEC_BASE                              0x40300000
#define BT_INTESYS_MEM_OFFSET                   0x00000000

/* For linker scripts */

#define VECTOR_SECTION_SIZE                     256
#define REBOOT_PARAM_SECTION_SIZE               64
#define ROM_BUILD_INFO_SECTION_SIZE             40
#define ROM_EXPORT_FN_SECTION_SIZE              128

/* For driver specifics */

#define GPADC_CHIP_SPECIFIC

#ifdef __cplusplus
}
#endif

#endif
