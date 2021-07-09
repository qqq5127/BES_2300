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
#ifndef __HAL_DMACFG_BEST2000_H__
#define __HAL_DMACFG_BEST2000_H__

#define AUDMA_PERIPH_NUM                        16
#define GPDMA_PERIPH_NUM                        16

#define AUDMA_CHAN_NUM                          8
#define GPDMA_CHAN_NUM                          8

#define AUDMA_CHAN_START                        (0)
#define GPDMA_CHAN_START                        (0)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x008,     // CODEC RX
    CODEC_BASE + 0x008,     // CODEC TX
    BTPCM_BASE + 0x1C0,     // BTPCM RX
    BTPCM_BASE + 0x1C8,     // BTPCM TX
    I2S0_BASE + 0x200,      // I2S0 RX
    I2S0_BASE + 0x240,      // I2S0 TX
    0,                      // FIR RX
    0,                      // FIR TX
    SPDIF0_BASE + 0x1C0,    // SPDIF0 RX
    SPDIF0_BASE + 0x1C8,    // SPDIF0 TX
    SPDIF1_BASE + 0x1C0,    // SPDIF1 RX
    SPDIF1_BASE + 0x1C8,    // SPDIF1 TX
    DSD_BASE + 0x000,       // DSD RX (TODO: Correct FIFO address)
    CODEC_BASE + 0x054,     // MC RX
    FFT_BASE + 0x000,       // FFT RX (TODO: Correct FIFO address)
    FFT_BASE + 0x000,       // FFT TX (TODO: Correct FIFO address)
};

static const enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_BTPCM_RX,
    HAL_AUDMA_BTPCM_TX,
    HAL_AUDMA_I2S0_RX,
    HAL_AUDMA_I2S0_TX,
    HAL_AUDMA_FIR_RX,
    HAL_AUDMA_FIR_TX,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
    HAL_AUDMA_SPDIF1_RX,
    HAL_AUDMA_SPDIF1_TX,
    HAL_AUDMA_DSD_RX,
    HAL_AUDMA_MC_RX,
    HAL_AUDMA_FFT_RX,
    HAL_AUDMA_FFT_TX,
};

static const uint32_t gpdma_fifo_addr[GPDMA_PERIPH_NUM] = {
    SDIO_BASE + 0x200,      // SDIO
    SDMMC_BASE + 0x200,     // SDMMC
    I2C0_BASE + 0x010,      // I2C0 RX
    I2C0_BASE + 0x010,      // I2C0 TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    SPILCD_BASE + 0x008,    // SPILCD RX
    SPILCD_BASE + 0x008,    // SPILCD TX
    UART0_BASE + 0x000,     // UART0 RX
    UART0_BASE + 0x000,     // UART0 TX
    UART1_BASE + 0x000,     // UART1 RX
    UART1_BASE + 0x000,     // UART1 TX
    ISPI_BASE + 0x008,      // ISPI TX
    ISPI_BASE + 0x008,      // ISPI RX
    UART2_BASE + 0x000,     // UART2 RX
    UART2_BASE + 0x000,     // UART2 TX
};

static const enum HAL_DMA_PERIPH_T gpdma_fifo_periph[GPDMA_PERIPH_NUM] = {
    HAL_GPDMA_SDIO,
    HAL_GPDMA_SDMMC,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_GPDMA_SPILCD_RX,
    HAL_GPDMA_SPILCD_TX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_ISPI_RX,
    HAL_GPDMA_ISPI_TX,
    HAL_GPDMA_UART2_RX,
    HAL_GPDMA_UART2_TX,
};

#define audma_fifo_periph_remap                 gpdma_fifo_periph
#define gpdma_fifo_periph_remap                 audma_fifo_periph

#endif

