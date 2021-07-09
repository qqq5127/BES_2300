
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

ifneq ($(BES_OTA_MODE),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/hci/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)besport/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hcitrans/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/l2cap/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/me/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/rfcomm/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/sdp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/hid/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/a2dp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/avctp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/avdtp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/avrcp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/common/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/dip/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/hf/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/hsp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/spp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hcitrans/modinit/*.c))
else
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)besport/bes_os.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/hci/hci.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/utils.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/rxbuff.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)stack/btalloc.c))
endif

ifeq ($(BES_AUD),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)profiles/besaud/*.c))
endif

ifeq ($(IBRT), 1)
BTAPI_LIB_NAME := ibrt_libbt_profiles
else
BTPROFILE_LIB_NAME := $(CHIP)_libbt_profiles
endif

ifeq ($(HFP_1_6_ENABLE), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_sbc_enc
endif

ifeq ($(ROM_UTILS_ON),1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_romaac
endif

ifeq ($(BLE),1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_ble
endif

ifeq ($(BT_RF_PREFER), 2M)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_2m
endif

ifeq ($(BT_RF_PREFER), 3M)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_3m
endif

$(BTPROFILE_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

-include $(cur_dir)lib_proc/Makefile

obj-y += $(BTPROFILE_LIB_NAME).a

CFLAGS_a2alloc.o += -O3
CFLAGS_a2dp.o += -O3
CFLAGS_a2dp_sdp.o += -O3
CFLAGS_a2dp_sm.o += -O3

subdir-ccflags-y += \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    $(BT_IF_INCLUDES) \
    $(BT_PROFILES_INCLUDES) \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iservices/bt_app \
    -Iservices/audioflinger \
    -Iservices/nvrecord \
    -Iservices/osif \
    -Iutils/lockcqueue \
    -Iapps/key \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iapps/audioplayers

