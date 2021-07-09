cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(CHIP_SUBTYPE),best1402)
src_dir := $(cur_dir)../best1402/
obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(src_dir)*.c $(src_dir)*.cpp $(src_dir)*.S))
else
obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
endif
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

CFLAGS_bt_drv_uart_bridge_intsys.o += $(BT_DRV_UART_BRIDGE_FLAGS)

CFLAGS_bt_drv.o += $(BT_DRV_FLAGS)

CFLAGS_bt_drv_patch.o += $(BT_DRV_PATCH_FLAGS)

CFLAGS_bt_drv_config.o += $(BT_DRV_CONFIG_FLAGS)

subdir-ccflags-y += -Iutils/crash_catcher/include

