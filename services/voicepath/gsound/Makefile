cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_target/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_custom/src/*.c))
all_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_target/*.cpp))
all_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_custom/src/*.cpp))

filtout_cpp :=
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_target/*_test.cpp))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_custom/src/*_test.cpp))
ifneq ($(TWS_SYSTEM_ENABLED), 1)
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_target/*_tws.cpp))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_custom/src/*_tws.cpp))
endif

ifneq ($(GSOUND_HOTWORD_ENABLE), 1)
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_target/*hotword*.cpp))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_custom/src/*hotword*.cpp))
else
ifeq ($(GSOUND_HOTWORD_EXTERNAL), 1)
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_target/*hotword_internal.cpp))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_custom/src/*hotword_internal.cpp))
else
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_target/*hotword_external.cpp))
filtout_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gsound_custom/src/*hotword_external.cpp))
endif
endif

obj_cpp := $(filter-out $(filtout_cpp),$(all_cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

# include the libgsound
obj-y += libgsound.a

# include the dsp library if hotword enabled
ifeq ($(GSOUND_HOTWORD_ENABLE), 1)
obj-y += libcortexm4_google_hotword_dsp_multi_bank_api.a
endif

subdir-ccflags-y += \
    -Iapps/audioplayers/ \
    -Iapps/common/ \
    -Iapps/battery/ \
    -Iapps/key/ \
    -Iapps/main/ \
    -Iapps/factory \
    -Iplatform/hal \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/norflash \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/bt \
    -Iservices/nvrecord \
    -Iservices/nv_section/customparam_section/ \
    -Iservices/nv_section/log_section/ \
    -Iservices/voicepath \
    -Iservices/voicepath/gsound \
    -Iservices/voicepath/gsound/gsound_target/ \
    -Iservices/voicepath/gsound/gsound_target_api_read_only/ \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/cp_accel \
    -Iservices/resources \
    -Iservices/bluedroid/adapter/src \
    -Iservices/audio_process \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    -Iservices/overlay \
    -Iservices/audioflinger \
    -Iservices/bridge/ \
    $(BT_IF_INCLUDES) \
    -Iservices/bt_app \
    -Iservices/bt_app/a2dp_codecs/include \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/ble_app \
    -Iservices/ble_app/app_gfps \
    -Iservices/ble_app/app_voice/ \
    -Iservices/ble_app/app_amsc/ \
    -Iservices/ble_app/app_ancc/ \
    -Iservices/ble_app/app_voice/app_gsound/ \
    -Iservices/ble_app/app_main/ \
    -Iservices/ble_app/app_htp/ \
    -Iservices/ble_app/app_voice/app_smartvoice \
    -Iservices/ble_profiles/htp/htpt/api/ \
    -Iservices/ble_profiles/htp/htpt/src/ \
    -Iservices/ble_profiles/htp/ \
    -Iservices/ble_profiles/anc/ \
    -Iservices/ble_profiles/anc/ancc/ \
    -Iservices/ble_profiles/ams/ \
    -Iservices/ble_profiles/ams/amsc/ \
    -Iservices/ble_profiles/voicepath/gsound \
    -Iservices/ble_stack/ke/api \
    -Iservices/ble_stack/ke/src/ \
    -Iservices/ble_stack/ble_ip \
    -Iservices/ble_stack/common/api/ \
    -Iservices/ble_stack/hl/inc/ \
    -Iservices/ble_stack/hl/api/ \
    -Iservices/ble_stack/hl/src/gap/ \
    -Iservices/ble_stack/hl/src/gap/gapc/ \
    -Iservices/ble_stack/hl/src/gap/gapm/ \
    -Iservices/ble_stack/hl/src/gap/smpc/ \
    -Iservices/ble_stack/hl/src/gatt/attc/ \
    -Iservices/ble_stack/hl/src/gatt/attm/ \
    -Iservices/ble_stack/hl/src/gatt/atts/ \
    -Iservices/ble_stack/hl/src/gatt/gattc/ \
    -Iservices/ble_stack/hl/src/gatt/gattm/ \
    -Iservices/ble_stack/hl/src/gatt/ \
    -Iservices/ble_stack/hl/src/l2c/l2cc/ \
    -Iservices/ble_stack/hl/src/l2c/l2cm/ \
    -Iservices/ble_stack/hci/api/ \
    -Iservices/ble_stack/hci/src/ \
    -Iservices/ble_stack/app/api/ \
    -Iservices/ble_stack/app/src/ \
    -Iservices/multimedia/opus121/include \
    -Iservices/multimedia/speech/application/Pcm8kCvsd/include \
    -Iservices/multimedia/speech/codec/codecs/adpcm \
    -Iservices/multimedia/speech/process/lc_mmse_ns/include \
    -Iservices/multimedia/speech/process/ns_and_ec/include \
    -Iservices/multimedia/speech/process/speech_2mic_ns2_denoise/include \
    -Iservices/multimedia/speech/process/dual_mic_denoise/include \
    -Iservices/multimedia/speech/process/audio_drc2/include \
    -Iservices/multimedia/speech/process/far_field_speech_enhancement/include \
    -Iservices/multimedia/speech/process/plc/include \
    -Iservices/multimedia/speech/process/common/include \
    -Iservices/multimedia/speech/application/Pcm8kCvsd/include \
    -Iutils/cqueue \
    -Iutils/retention_ram \
    -Iutils/crc32 \
    -Iutils/lockcqueue \
    -Iutils/intersyshci \
    -Ithirdparty/userapi \
    -Iutils/heap \
    -Iservices/ble_profiles/voicepath/gsound \
    -Iservices/ai_voice/manager \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iservices/norflash_api \
    -Iservices/ota \
    -Iservices/app_tws/inc \
    -Iservices/ibrt_ui/inc \
    -Iservices/ibrt_core/inc \
    -Iservices/app_ai/inc \
    -Iservices/app_ibrt/inc \
    -Iservices/audio_dump/include \
    -Iapps/voice_detector

ifeq ($(FLASH_SUSPEND),1)
CFLAGS_gsound_custom_ota.o += -DFLASH_SUSPEND
endif

