cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += -Iplatform/drivers/ana \
                    $(BT_IF_INCLUDES) \
                    -Iservices/ble_app/app_main \
                    -Iservices/app_ibrt/inc \
                    -Iservices/ibrt_ui/inc \
                    -Iservices/ibrt_core/inc	\
                    -Izowee/ui \
					-Izowee/ui/vbus/ \
					-Izowee/ui/battery/ \
	     -Iservices/ibrt_ui/inc/ \
	     -Iservices/app_ibrt/inc/ \
	     -Iservices/ibrt_core/inc \
	     -Iservices/bt_if_enhanced/inc \
	     -Iservices/multimedia/audio/codec/sbc/inc \
	     -Iplatform/drivers/bt \
	     -Iapps/key \
	     -Iutils/cqueue \

ifeq ($(VOICE_PROMPT),1)
CFLAGS_app_battery.o += -DMEDIA_PLAYER_SUPPORT
endif

