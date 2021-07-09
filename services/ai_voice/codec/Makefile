cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)compression/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(OPUS_CODEC),1)
src_obj += opus121/src/
endif

CODEC_LIB_NAME := lib_codec

$(CODEC_LIB_NAME)-y := $(src_obj)

obj-y += $(CODEC_LIB_NAME).a

subdir-ccflags-y +=	-Iservices/audioflinger \
					-Iapps/audioplayers \
					-Iservices/app_ai/voice_sbc \
					-Iservices/ai_voice/codec/opus121/include \
					-Iservices/multimedia/speech/inc \
					-Iservices/bt_app \
					$(BT_IF_INCLUDES) \
					-Iutils/cqueue \
					-Iutils/heap \
					-Iutils/lockcqueue \
	                -Iservices/multimedia/audio/codec/sbc/inc \
	                -Iservices/multimedia/audio/codec/sbc/src/inc \
					-Iutils/retention_ram \





