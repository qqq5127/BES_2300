#ifndef _DCO_BEST3003_H_
#define _DCO_BEST3003_H_

#define ISPI_DCO_REG(reg)       (((reg) & 0xFFF) | 0x3000)      // CS = 3, DCO interface

#define dco_read(reg, val)      hal_analogif_reg_read(ISPI_DCO_REG(reg), val)
#define dco_write(reg, val)     hal_analogif_reg_write(ISPI_DCO_REG(reg), val)

// DCO tuning mode
#define DCO_TUNING_MODE_COARSE          0
#define DCO_TUNING_MODE_FINE            1       // resolution: 10ppm
#define DCO_TUNING_MODE_SDM             2       // resolution: 1ppm

// DCO tuning reference clock source
#define DCO_TUNING_REF_SOF_CLK          0
#define DCO_TUNING_REF_EXT_13MHz        1

#ifdef CHIP_HAS_DCO
bool dco_enabled(void);
void dco_open (void);

void dco_set_tuning_mode (uint8_t);
void dco_set_tuning_reference_clk (uint8_t);
#endif  // CHIP_HAS_DCO

#endif // _DCO_BEST3003_H_
