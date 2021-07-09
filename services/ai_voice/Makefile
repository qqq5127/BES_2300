cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)algorithm/src/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio/src/*.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)manager/src/*.cpp))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)transport/src/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)transport/src/*.cpp))

framework_obj :=

framework_obj += codec/

ifeq ($(IOS_MFI),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)iAP2/src/*.c))
endif

AI_FRAMEWORK_LIB_NAME := libAI_api

ifeq ($(AMA_VOICE),1)
framework_obj += protocol/ama/
endif

ifeq ($(DMA_VOICE),1)
framework_obj += protocol/dma/
endif

ifeq ($(SMART_VOICE),1)
framework_obj += protocol/smartvoice/
endif

ifeq ($(TENCENT_VOICE),1)
framework_obj += protocol/tencent/
endif

ifeq ($(GMA_VOICE),1)
framework_obj += protocol/gma/
endif

ifeq ($(CUSTOMIZE_VOICE),1)
framework_obj += protocol/common_api/
AI_FRAMEWORK_LIB_NAME := $(AI_FRAMEWORK_LIB_NAME)_COMMON
endif

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

$(AI_FRAMEWORK_LIB_NAME)-y := $(src_obj)

obj-y := $(AI_FRAMEWORK_LIB_NAME).a
obj-y += $(framework_obj)

subdir-ccflags-y += -Iapps/common \
                    -Iapps/key \
                    -Iapps/main \
                    -Iapps/audioplayers \
                    -Iapps/factory \
                    -Iplatform/drivers/ana \
                    -Iplatform/drivers/bt \
                    -Iplatform/hal \
                    -Irtos\rtx/TARGET_CORTEX_M \
                    -Iservices/ai_voice/algorithm \
                    -Iservices/ai_voice/codec/compression \
                    -Iservices/ai_voice/codec/opus121/include \
                    -Iservices/ai_voice/manager \
                    -Iservices/ai_voice/transport \
                    -Iservices/ai_voice/audio \
                    -Iservices/ai_voice/iAP2 \
                    -Iservices/app_ai/voice_sbc \
                    -Iservices/app_ai/inc \
                    -Iservices/audio_eq \
                    -Iservices/audio_dump/include \
                    -Iservices/audioflinger \
                    -Iservices/bt_app \
                    -Iservices/bt_app/a2dp_codecs/include \
                    -Iservices/ble_app/app_main \
                    -Iservices/ble_stack/ble_ip \
                    -Iservices/ble_stack/hci/inc/ \
                    -Iservices/ble_stack/hl/api \
                    -Iservices/ble_stack/hl/inc \
                    -Iservices/ble_stack/common/api \
                    -Iservices/ble_stack/ke/api \
                    $(BT_IF_INCLUDES) \
                    -Iservices/communication \
                    -Iservices/communication/comminication_knowles \
                    -Iservices/interconnection/umm_malloc \
                    -Iservices/multimedia/audio_dump/include \
                    -Iservices/multimedia/audio/codec/sbc/inc \
                    -Iservices/multimedia/audio/codec/sbc/src/inc \
                    -Iservices/multimedia/speech/inc \
                    -Iservices/nvrecord \
                    -Iservices/overlay \
                    -Iservices/resources \
                    -Iservices/voicepath/gsound/gsound_custom/inc \
                    -Ithirdparty/userapi \
                    -Iutils/cqueue \
                    -Iutils/heap \
                    -Iutils/kfifo \
                    -Iutils/lockcqueue \
                    -Iutils/retention_ram \

ifeq ($(AUDIO_RESAMPLE),1)
ANC_USB_CFG_FLAGS += -D__AUDIO_RESAMPLE__
endif
ifeq ($(SW_CAPTURE_RESAMPLE),1)
ANC_USB_CFG_FLAGS += -DSW_CAPTURE_RESAMPLE
endif

