cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)
obj-y := $(obj-y:.cpp=.o)

obj-y += SP_EC_MV_DEBUG.a


ccflags-y := \
  -Iplatform/cmsis/inc  \
	-Iapps/anc/inc    \
	-Iservices/bt_app \
	-Iutils/list \
	-Iutils/heap \
	-Iservices/bt_profiles/inc \
	-Iapps/key \
	-Izowee/ui \
