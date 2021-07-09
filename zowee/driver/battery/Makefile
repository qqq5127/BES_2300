cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o) 
obj-y += battery_ZT203/
obj-y += battery_ZT102/

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
	-Izowee/driver/battery/battery_ZT102 \
	-Izowee/driver/battery/battery_ZT203 \
	
ifeq ($(IIC_ENABLE),1)
ccflags-y += -Iplatform/drivers/iic 
endif