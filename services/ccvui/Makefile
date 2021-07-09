
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s   := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.s))
obj_c   := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

CCVUI_LIB_NAME := libXS-A1RN0324-8A

$(CCVUI_LIB_NAME)-y :=

obj-y :=  $(CCVUI_LIB_NAME).a  $(src_obj)

subdir-ccflags-y += \
    -Iservices/ccvui/inc \
	-Iservices/bt_app	\
	-Iapps/common  \
	-Iservices/multimedia/speech/inc \
	-Iutils/heap \
	-Iplatform/hal \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/norflash\
	-Iservices/app_ai/inc
ccflags-y := \



