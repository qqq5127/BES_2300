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
#ifndef __HAL_DMACFG_BEST1400_H__
#define __HAL_DMACFG_BEST1400_H__

#define AUDMA_PERIPH_NUM                        16
#define AUDMA_CHAN_NUM                          6
#define AUDMA_CHAN_START                        (0)

// Defined by cmu->DMA_CFG0/1/2 in hal_cmu_module_init_state()

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x1C,      // CODEC RX
    CODEC_BASE + 0x1C,      // CODEC TX
    BTPCM_BASE + 0x1C0,     // BTPCM RX
    BTPCM_BASE + 0x1C8,     // BTPCM TX
    UART0_BASE + 0x00,      // UART0 RX
    UART0_BASE + 0x00,      // UART0 TX
    UART1_BASE + 0x00,      // UART1 RX
    UART1_BASE + 0x00,      // UART1 TX
    I2C0_BASE + 0x010,      // I2C0 RX
    I2C0_BASE + 0x010,      // I2C0 TX
    UART2_BASE + 0x00,      // UART2 RX
    UART2_BASE + 0x00,      // UART2 TX
#ifdef BTDUMP_ENABLE
    BTDUMP_BASE + 0x34,     // BTDUMP
#else
    SPI_BASE + 0x008,       // SPI RX
#endif
    SPI_BASE + 0x008,       // SPI TX
    I2S0_BASE + 0x200,      // I2S0 RX
    I2S0_BASE + 0x240,      // I2S0 TX
};

static enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_BTPCM_RX,
    HAL_AUDMA_BTPCM_TX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_UART2_RX,
    HAL_GPDMA_UART2_TX,
#ifdef BTDUMP_ENABLE
    HAL_AUDMA_BTDUMP,
#else
    HAL_GPDMA_SPI_RX,
#endif
    HAL_GPDMA_SPI_TX,
    HAL_AUDMA_I2S0_RX,
    HAL_AUDMA_I2S0_TX,
};

#endif

