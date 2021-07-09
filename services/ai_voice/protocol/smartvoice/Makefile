cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)smartvoice_manager/*.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)smartvoice_bt/*.cpp))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)smartvoice_ble/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)smartvoice_gatt/*.c))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

SMARTVOICE_LIB_NAME := lib_smartvoice

$(SMARTVOICE_LIB_NAME)-y := $(src_obj)

obj-y :=  $(SMARTVOICE_LIB_NAME).a

ccflags-y +=  -Iapps/main \
              -Iapps/app_anker/device_drivers/bes_hal \
              -Iapps/audioplayers \
              -Iapps/key \
              -Iplatform/hal \
              -Iplatform/drivers/bt \
              -Iservices/ai_voice/algorithm \
              -Iservices/ai_voice/protocol/smartvoice/smartvoice_ble \
              -Iservices/ai_voice/protocol/smartvoice/smartvoice_bt \
              -Iservices/ai_voice/protocol/smartvoice/smartvoice_gatt \
              -Iservices/ai_voice/protocol/smartvoice/smartvoice_manager \
              -Iservices/ai_voice/audio \
              -Iservices/ai_voice/codec/compression \
              -Iservices/ai_voice/codec/opus121/include \
              -Iservices/ai_voice/transport \
              -Iservices/ai_voice/manager \
              -Iservices/app_ai/voice_sbc \
              -Iservices/app_ai/inc \
              -Iservices/bt_app \
              -Iservices/bt_app/a2dp_codecs/include \
              -Iservices/resources \
              -Iservices/nvrecord \
              -Iservices/nv_section/factory_section \
              -Ithirdparty/userapi \
              -Iutils/cqueue \
              -Iutils/crc32 \
              -Iutils/heap \
