cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iservices/bt_if_enhanced/inc \
	-Iservices/bt_app/a2dp_codecs/include \
	-Iapps/common \
	-Iapps/key \
	-Iapps/anc/inc \
	-Iplatform/hal \
	-Iservices/bt_profiles/inc \
	-Iservices/nvrecord \
	-Iservices/wear_detect\
	-Iservices/bt_profiles_enhanced/inc\
	-Iapps/anc/inc	\
	-Iservices/nv_section/factory_section	\
	-Iutils/lockcqueue \
	-Iutils/cqueue \
	-Iservices/bt_app \
	-Iutils/hwtimer_list \
	-Izowee/driver/touch \
	-Izowee/driver/ir \
	-Izowee \
	-Izowee/ui/ \
	-Izowee/ui/wear_status/ \
	-Izowee/ui/bt \
	-Izowee/ui/a2dp \
	-Izowee/ui/hfp \
	-Izowee/driver/vbus \
	-Izowee/driver/zowee_section \
	-Iapps/main \
	-Iservices/app_ibrt/inc \
	-Iservices/ibrt_core/inc \
	-Iservices/bt_if_enhanced/inc \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iplatform/drivers/bt \
	-Iapps/key \
	-Iservices/ibrt_ui/inc \
	-Iapps/audioplayers/a2dp_decoder \
	-Iapps/battery \
	-Iservices/bt_app/ \
	-Iservices/multimedia/audio/codec/sbc/inc \


ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_ui_event_gesture.o += -D__AUDIO_RESAMPLE__
endif