cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(CODEC_HIGH_QUALITY),1)
VCODEC_VOLT ?= 2.5V
#VCODEC_VOLT ?= 1.8V
else
VCODEC_VOLT ?= 1.66V
endif

ifneq ($(filter-out 1.5V 1.6V 1.66V 1.7V 1.8V 1.9V 2.0V 2.5V 2.8V,$(VCODEC_VOLT)),)
$(error Invalid VCODEC_VOLT=$(VCODEC_VOLT))
endif

ifneq ($(VCODEC_VOLT),)
ANA_CFG_FLAGS += -DVCODEC_VOLT=$(subst V,,$(VCODEC_VOLT))
PMU_CFG_FLAGS += -DVCODEC_VOLT=$(subst V,,$(VCODEC_VOLT))
ifeq ($(VCODEC_VOLT), 1.9V)
ANA_CFG_FLAGS += -DDAC_HIGH_OUTPUT
endif
ifneq ($(filter 1.5V 1.6V 1.66V 1.7V 1.8V 1.9V,$(VCODEC_VOLT)),)
VCODEC_OFF ?= 1
ifeq ($(VANA_VOLT),)
VANA_VOLT := $(VCODEC_VOLT)
endif
ifneq ($(VCODEC_VOLT),$(VANA_VOLT))
$(error Incompatible VCODEC_VOLT=$(VCODEC_VOLT) with VANA_VOLT=$(VANA_VOLT))
endif
endif
endif

ifeq ($(VCODEC_OFF),1)
PMU_CFG_FLAGS += -DVCODEC_OFF
endif

VANA_VOLT ?= 1.66V
ifneq ($(filter-out 1.5V 1.6V 1.66V 1.7V 1.8V 1.9V,$(VANA_VOLT)),)
$(error Invalid VANA_VOLT=$(VANA_VOLT))
endif
ifneq ($(VANA_VOLT),)
VANA_STEM := $(subst .,P,$(VANA_VOLT))
PMU_CFG_FLAGS += -DVANA_$(VANA_STEM)
endif

ifeq ($(PMU_IRQ_UNIFIED),1)
PMU_CFG_FLAGS += -DPMU_IRQ_UNIFIED
endif

ifeq ($(PSRAM_ENABLE),1)
PMU_CFG_FLAGS += -DPSRAM_ENABLE
endif
ifeq ($(PSRAMUHS_ENABLE),1)
PMU_CFG_FLAGS += -DPSRAMUHS_ENABLE
endif
ifeq ($(DSP_ENABLE),1)
PMU_CFG_FLAGS += -DDSP_ENABLE
endif

LOW_POWER_ADC ?= 1
ifeq ($(LOW_POWER_ADC),1)
ANA_CFG_FLAGS += -DLOW_POWER_ADC
endif

ifeq ($(DAC_DRE_GAIN_DC_UPDATE),1)
ANA_CFG_FLAGS += -DDAC_DRE_GAIN_DC_UPDATE
endif

ifeq ($(PSRAMUHS_DUAL_8BIT),1)
PSRAMUHS_CFG_FLAGS += -DPSRAMUHS_DUAL_8BIT
endif

ifeq ($(PSRAMUHS_DUAL_SWITCH),1)
PSRAMUHS_CFG_FLAGS += -DPSRAMUHS_DUAL_SWITCH
endif

ifneq ($(MCU_HIGH_PERFORMANCE_MODE), 0)
PMU_CFG_FLAGS += -DMCU_HIGH_PERFORMANCE_MODE_$(MCU_HIGH_PERFORMANCE_MODE)
endif

PLL_CFG_FLAGS +=-DPSRAMUHS_SPEED=$(PSRAMUHS_SPEED)

PMU_CFG_FLAGS +=-DPSRAMUHS_SPEED=$(PSRAMUHS_SPEED)

CFLAGS_pll_$(CHIP).o += $(PLL_CFG_FLAGS)

CFLAGS_pmu_$(CHIP).o += $(PMU_CFG_FLAGS)

CFLAGS_analog_$(CHIP).o += $(ANA_CFG_FLAGS)

CFLAGS_usbphy_$(CHIP).o += $(USBPHY_CFG_FLAGS)

CFLAGS_psramuhsphy_$(CHIP).o += $(PSRAMUHS_CFG_FLAGS)

