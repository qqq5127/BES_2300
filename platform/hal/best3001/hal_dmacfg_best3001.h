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
#ifndef __HAL_DMACFG_BEST3001_H__
#define __HAL_DMACFG_BEST3001_H__

#define AUDMA_PERIPH_NUM                        16
#define AUDMA_CHAN_NUM                          6
#define AUDMA_CHAN_START                        (0)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x1C,      // CODEC RX
    CODEC_BASE + 0x1C,      // CODEC TX
    CODEC_BASE + 0x28,      // DSD RX
    CODEC_BASE + 0x28,      // DSD TX
    UART0_BASE + 0x00,      // UART0 RX
    UART0_BASE + 0x00,      // UART0 TX
    UART1_BASE + 0x00,      // UART1 RX
    UART1_BASE + 0x00,      // UART1 TX
    I2C0_BASE + 0x010,      // I2C0 RX
    I2C0_BASE + 0x010,      // I2C0 TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    SPDIF0_BASE+ 0x1c0,     // SPDIF0 RX
    SPDIF0_BASE+ 0x1c8,     // SPDIF0 TX
    I2S0_BASE + 0x200,      // I2S0 RX
    I2S0_BASE + 0x240,      // I2S0 TX
};

static enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_DSD_TX,
    HAL_AUDMA_DSD_RX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
    HAL_AUDMA_I2S0_RX,
    HAL_AUDMA_I2S0_TX,
};

#endif

