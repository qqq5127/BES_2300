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
#ifndef __USBPHY_BEST3001_H__
#define __USBPHY_BEST3001_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_USBPHY_REG(reg)                (((reg) & 0xFFF) | 0x2000)

void usbphy_usb_clk_default_sel(void);

void usbphy_pll_codec_enable (bool enable);
void usbphy_pll_config(uint32_t freq, uint32_t div);
void usbphy_pll_tune(float ratio);
void usbphy_pll_set_dig_div(uint32_t div);

void pll_config_update(uint32_t tgt_freq);


#ifdef __cplusplus
}
#endif

#endif

