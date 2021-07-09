cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += libscpi/

ifeq ($(AUTOTEST_SCPI),1)
CFLAGS_app_scpi_test.o += -D__SCPI_TEST__
CFLAGS_app_scpi_stub.o += -D__SCPI_TEST__
endif

ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_app_scpi_test.o += -D__AUDIO_RESAMPLE__
CFLAGS_app_scpi_stub.o += -D__AUDIO_RESAMPLE__
endif


subdir-ccflags-y += \
	-Izowee/product_test/libscpi/inc \
	-Izowee/product_test/libscpi \
	-Iservices/bt_app \
	-Iplatform/hal \
	-Iservices/bluedroid/adapter/src \
	-Iapps/key \
	-Iservices/audio_eq \
	-Iservices/fs/fat \
	-Iservices/fs/sd \
	-Iservices/fs/fat/ChaN \
	$(BT_IF_INCLUDES) \
	-Iservices/overlay \
	-Iservices/nvrecord \
	-Iservices/nv_section/factory_section \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Iservices/resources \
	-Iplatform/drivers/uarthci \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/bt \
	-Iplatform/drivers/main \
	-Iutils/cqueue \
	-Iutils/heap \
	-Iutils/retention_ram \
	-Iservices/audioflinger \
	-Iutils/lockcqueue \
	-Iutils/hwtimer_list/ \
	-Iutils/intersyshci \
	-Iutils/boot_struct \
	-Iapps/main \
	-Iapps/common \
	-Iapps/audioplayers \
	-Iapps/factory \
	-Iapps/anc/inc \
	-Iservices/bridge/ \
	-Iservices/bt_app \
	-Iservices/bt_app/a2dp_codecs/include \
	-Iservices/anc_spp_tool \
	-Iutils/crc32 \
	-Ithirdparty/userapi \
  -Iservices/multimedia/audio/codec/sbc/inc \
  -Iservices/multimedia/audio/codec/sbc/src/inc \
	-Iservices/norflash_api \
	-Iservices/ibrt_core/inc \
	-Iconfig/best2300p_ibrt_anc \
	-Iplatform/cmsis/inc \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Iutils/crc32	\
	-Iservices/bt_profiles_enhanced/inc \
	-Iservices/bt_if_enhanced/inc \
	-Izowee/ui/product_test \
	-Izowee/ui \
	-Izowee/driver/touch \
	-Izowee/driver/IR \
	-Izowee/driver/vbus \
	-Izowee/driver/battery \
	-Izowee/ui/gesture \
	-Izowee \
	-Iapps/key \
	-Iservices/nv_section/factory_section \
	-Izowee/ui/vbus \
	-Iservices/norflash_api \
	-Iservices/nv_section/userdata_section \
	-Iservices/tota \
						
ifeq ($(IBRT),1)
ccflags-y += -Iservices/tws_ibrt/inc
ccflags-y += -Iservices/app_ibrt/inc
ccflags-y += -Iservices/bt_if/inc
ccflags-y += -Iservices/ibrt_ui/inc
ccflags-y += -Iservices/app_tws/inc
endif

ifneq ($(PROJ_NAME),)	
ccflags-y += -Iapps/voca/ui/$(PROJ_NAME)
endif	

