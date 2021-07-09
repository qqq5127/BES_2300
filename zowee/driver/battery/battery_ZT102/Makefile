cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iapps/common \
	-Iplatform/hal \
	-Iplatform\hal\best2300p \
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
	-Izowee/bus \
	-Izowee/ui \
	-Izowee \
	-Izowee/driver/battery \
	-Izowee/ui/battery \
	-Izowee/driver/vbus \
	-Izowee/driver/zowee_section \
	-Iapps/battery \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/ana\best2300p \
	-Iapps/main \
	-Izowee/ui/bt \
	-Iservices/app_ibrt/inc \
	-Iservices/ibrt_core/inc \
	-Iservices/bt_if_enhanced/inc \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iplatform/drivers/bt \
	-Iservices/ibrt_ui/inc \
	-Iapps/key \