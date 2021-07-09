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
#ifndef __HAL_DMACFG_BEST3003_H__
#define __HAL_DMACFG_BEST3003_H__

#define AUDMA_PERIPH_NUM                        16
#define GPDMA_PERIPH_NUM                        9

#define AUDMA_CHAN_NUM                          6
#define GPDMA_CHAN_NUM                          4

#define AUDMA_CHAN_START                        (0)
#define GPDMA_CHAN_START                        (0)

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
    0,                      // FMDUMP0
    0,                      // FMDUMP1
    CODEC_BASE + 0x038,     // MC RX
    CODEC_BASE + 0x324,     // FM RX
    ISPI_BASE + 0x008,      // ISPI RX
    ISPI_BASE + 0x008,      // ISPI TX
};

static enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_DSD_RX,
    HAL_AUDMA_DSD_TX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_AUDMA_FMDUMP0,
    HAL_AUDMA_FMDUMP1,
    HAL_AUDMA_MC_RX,
    HAL_AUDMA_FM_RX,
    HAL_GPDMA_ISPI_RX,
    HAL_GPDMA_ISPI_TX,
};

static const uint32_t gpdma_fifo_addr[GPDMA_PERIPH_NUM] = {
    0,                      // FIR RX
    0,                      // FIR TX
    SPDIF0_BASE + 0x1C0,    // SPDIF0 RX
    SPDIF0_BASE + 0x1C8,    // SPDIF0 TX
    I2S0_BASE + 0x200,      // I2S0 RX
    I2S0_BASE + 0x240,      // I2S0 TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    FLASH_CTRL_BASE + 0x008, // FLASH CTRL
};

static const enum HAL_DMA_PERIPH_T gpdma_fifo_periph[GPDMA_PERIPH_NUM] = {
    HAL_AUDMA_FIR_RX,
    HAL_AUDMA_FIR_TX,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
    HAL_AUDMA_I2S0_RX,
    HAL_AUDMA_I2S0_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_GPDMA_FLASH_TX,
};

#endif

