cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
ifeq ($(CHIP_SUBTYPE),best1402)
obj-y := $(filter-out hal_codec_best1400.c,$(obj-y))
else
obj-y := $(filter-out hal_codec_best1402.c,$(obj-y))
endif
obj-y := $(obj-y:.c=.o)

USB_CLK_SRC_24M_X2 ?= 1
ifeq ($(USB_CLK_SRC_24M_X2),1)
CMU_CFG_FLAGS += -DUSB_CLK_SRC_24M_X2
endif

ifeq ($(ANA_26M_X6_ENABLE),1)
CMU_CFG_FLAGS += -DANA_26M_X6_ENABLE
endif

ifeq ($(DAC_CLASSG_ENABLE),1)
CODEC_CFG_FLAGS += -DDAC_CLASSG_ENABLE
endif

ifeq ($(DAC_DRE_ENABLE),1)
CODEC_CFG_FLAGS += -DDAC_DRE_ENABLE
endif

ifeq ($(JTAG_TDI_TDO_PIN),1)
IOMUX_CFG_FLAGS += -DJTAG_TDI_TDO_PIN
endif

ifeq ($(JTAG_RESET_PIN),1)
IOMUX_CFG_FLAGS += -DJTAG_RESET_PIN
endif

CFLAGS_hal_analogif_$(CHIP).o += $(ANAIF_CFG_FLAGS)

CFLAGS_hal_cmu_$(CHIP).o += $(CMU_CFG_FLAGS)

CFLAGS_hal_codec_$(CHIP).o += $(CODEC_CFG_FLAGS)
CFLAGS_hal_codec_$(CHIP_SUBTYPE).o += $(CODEC_CFG_FLAGS)

CFLAGS_hal_iomux_$(CHIP).o += $(IOMUX_CFG_FLAGS)

