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
#include "wifidpd_best2001.h"
#include "hal_spi.h"

#define DPD_READ_CMD(r)                 ((1 << 26) | (((r) & 0x1FF) << 17))
#define DPD_WRITE_CMD(r, v)             ((((r) & 0x1FF) << 16) | ((v) & 0xFFFF))
#define DPD_READ_VAL(v)                 ((v) & 0xFFFF)

static const struct HAL_SPI_CFG_T spi_cfg = {
    .clk_delay_half = false,
    .clk_polarity = false,
    .slave = false,
    .dma_rx = false,
    .dma_tx = false,
    .rx_sep_line = false,
    .cs = 0,
    .rate = 6500000,
    .tx_bits = 26,
    .rx_bits = 27,
    .rx_frame_bits = 0,
};

int wifidpd_open(void)
{
    int ret;

    ret = hal_spidpd_open(&spi_cfg);
    if (ret) {
        return ret;
    }

    return ret;
}

int wifidpd_read(unsigned short reg, unsigned short *val)
{
    int ret;
    unsigned int data;
    unsigned int cmd;

    data = 0;
    cmd = DPD_READ_CMD(reg);
    ret = hal_spidpd_recv(&cmd, &data, 4);
    if (ret) {
        return ret;
    }
    *val = DPD_READ_VAL(data);
    return 0;
}

int wifidpd_write(unsigned short reg, unsigned short val)
{
    int ret;
    unsigned int cmd;

    cmd = DPD_WRITE_CMD(reg, val);
    ret = hal_spidpd_send(&cmd, 4);
    if (ret) {
        return ret;
    }
    return 0;
}
