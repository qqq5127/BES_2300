cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
obj-y += gesture/ wear_status/ vbus/ a2dp/ bt/ battery/ hfp/ product_test/

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
	-Izowee/driver/vbus \
	-Izowee/driver/battery \
	-Izowee/ \
	-Izowee/ui/gesture/ \
	-Izowee/ui/wear_status/ \
	-Izowee/product_test \
	-Izowee/ui/vbus/ \
	-Izowee/ui/a2dp/ \
	-Izowee/ui/hfp/ \
	-Izowee/ui/bt/ \
	-Izowee/ui/battery/ \
	-Izowee/ui/product_test/ \
	-Iservices/app_ibrt/inc \
	-Iservices/ibrt_core/inc \
	-Iservices/bt_if_enhanced/inc \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iplatform/drivers/bt \
	-Iservices/ibrt_ui/inc/ \
	-Iapps/key \
	-Izowee/driver/zowee_section \
	-Iapps/main \
	-Izowee/ui/product_test \