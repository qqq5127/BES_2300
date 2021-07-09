cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c :=
obj_cpp :=
obj_s :=

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gma_ota/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gma_manager/*.cpp))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gma_manager/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gma_crypto/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gma_ble/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gma_gatt/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gma_bt/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

DMA_LIB_NAME := lib_gma

$(DMA_LIB_NAME)-y := $(src_obj)

obj-y := $(DMA_LIB_NAME).a

ccflags-y +=  -Iapps/key \
			  -Iapps/main \
			  -Iapps/common   \
			  -Iapps/audioplayers \
			  -Iapps/app_anker/device_drivers/bes_hal \
			  -Iapps/battery \
			  -Iplatform/drivers/bt \
			  -Iplatform/hal \
			  -Iplatform/drivers/ana \
			  -Iplatform/drivers/norflash \
			  -Iservices/ai_voice/protocol/gma/gma_ble \
			  -Iservices/ai_voice/protocol/gma/gma_bt \
			  -Iservices/ai_voice/protocol/gma/gma_gatt \
              -Iservices/ai_voice/protocol/gma/gma_ota \
			  -Iservices/ai_voice/protocol/gma/gma_crypto \
			  -Iservices/ai_voice/protocol/gma/gma_manager \
			  -Iservices/ai_voice/algorithm \
			  -Iservices/ai_voice/audio \
			  -Iservices/ai_voice/codec/compression \
              -Iservices/ai_voice/codec/opus121/include \
			  -Iservices/ai_voice/transport \
			  -Iservices/ai_voice/manager \
              -Iservices/app_ai/voice_sbc \
              -Iservices/app_ai/inc \
			  -Iservices/bt_app \
			  -Iservices/bt_app/a2dp_codecs/include \
			  -Iservices/multimedia/speech/process/common/include \
			  -Iservices/ble_app/app_main \
			  -Iservices/ble_stack/ble_ip/  \
			  -Iservices/ble_stack/common/api  \
			  -Iservices/ble_stack/hl/api  \
			  -Iservices/ble_stack/hl/inc  \
			  -Iservices/ble_stack/ke/api  \
			  -Iservices/nvrecord \
	          -Iservices/resources \
              -Iservices/ibrt_core/inc \
              -Iservices/norflash_api   \
			  -Iservices/apps/common    \
			  -Iservices/app_ibrt/inc \
			  -Itests/rom_cust \
			  -Iutils/cqueue \
			  -Iutils/heap \
			  -Iutils/kfifo \
	          -Iservices/multimedia/audio/codec/sbc/inc \
	          -Iservices/multimedia/audio/codec/sbc/src/inc \
			  $(BT_IF_INCLUDES) \
			  -Iservices/nv_section/factory_section \
			  -Iutils/encrypt \
			  -Iutils/crc32 \
			  -Iservices/ibrt_ui/inc
			  