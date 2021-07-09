cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iapps/common \
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
	-Iservices/ibrt_core/inc \
	-Iservices/bt_if_enhanced/inc \
	-Izowee/ui/bt/ \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iplatform/drivers/bt \
	-Iapps/key \
	-Iservices/bt_app \
	-Iservices/bt_app/a2dp_codecs/include \
	-Iservices/app_ibrt/inc \
	-Iservices/ibrt_ui/inc \	
