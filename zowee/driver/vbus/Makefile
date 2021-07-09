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
	-Iapps/main \
	-Izowee/ui/vbus	\
	-Iplatform/drivers/ana \
	-Iapps/battery	\
	-Izowee/driver/battery \
	-Izowee/ui/product_test \
	-Izowee/product_test \
	
