CHIP        ?= best2300p

DEBUG       ?= 1

FPGA        ?= 0

MBED        ?= 0

RTOS        ?= 1

SOFTWARE_VERSION	?=	1.00.01


#KERNEL      ?= FREERTOS

LIBC_ROM    ?= 1

export USER_SECURE_BOOT	?= 0
# enable:1
# disable:0

WATCHER_DOG ?= 1

DEBUG_PORT  ?= 1
# 0: usb
# 1: uart0
# 2: uart1

FLASH_CHIP	?= ALL
# GD25Q80C
# GD25Q32C
# ALL

export NO_TRACE_TIME_STAMP ?=1

export FORCE_SIGNALINGMODE ?= 0

export FORCE_NOSIGNALINGMODE ?= 0

export FORCE_SCO_MAX_RETX ?= 0

export FA_RX_GAIN_CTRL ?= 0

export BT_FA_ECC ?= 0
export CONTROLLER_DUMP_ENABLE ?= 0
export CONTROLLER_MEM_LOG_ENABLE ?= 0

export INTERSYS_DEBUG ?= 0

export PROFILE_DEBUG ?= 0

export BTDUMP_ENABLE ?= 0

export BT_DEBUG_TPORTS ?= 0

TPORTS_KEY_COEXIST ?= 0

export SNIFF_MODE_CHECK ?= 0

AUDIO_OUTPUT_MONO ?= 0

AUDIO_OUTPUT_DIFF ?= 0

#### ANC DEFINE START ######
export ANC_APP		    ?= 1
export ANC_FF_ENABLED	?= 1
export ANC_FB_ENABLED	?= 1
export ANC_WNR_ENABLED	?= 0
export AUDIO_ANC_FB_MC ?= 0
export AUDIO_SECTION_SUPPT ?= 1
export AUD_SECTION_STRUCT_VERSION ?= 2
export AUDIO_ANC_FB_MC_HW ?=1

##### ANC DEFINE END ######

export DEBUG_ANC_BY_PHONE ?= 0

APP_ANC_TEST ?= 1

TEST_OVER_THE_AIR ?= 1

HW_FIR_EQ_PROCESS ?= 0

SW_IIR_EQ_PROCESS ?= 1

HW_DAC_IIR_EQ_PROCESS ?= 0

HW_IIR_EQ_PROCESS ?= 0

HW_DC_FILTER_WITH_IIR ?= 0

AUDIO_DRC ?= 0

AUDIO_DRC2 ?= 0

PC_CMD_UART ?= 0

AUDIO_SECTION_ENABLE ?= 0

# if open SNDP_TX_3MIC_ENABLE,need close audio resample
# default should be 1
AUDIO_RESAMPLE ?= 1

RESAMPLE_ANY_SAMPLE_RATE ?= 1

OSC_26M_X4_AUD2BB ?= 1

AUDIO_OUTPUT_VOLUME_DEFAULT ?= 17
# range:1~16

AUDIO_INPUT_CAPLESSMODE ?= 0

AUDIO_INPUT_LARGEGAIN ?= 0

AUDIO_CODEC_ASYNC_CLOSE ?= 0

AUDIO_SCO_BTPCM_CHANNEL ?= 1

export A2DP_CP_ACCEL ?= 1

export SCO_CP_ACCEL ?= 1

export SCO_TRACE_CP_ACCEL ?= 0

# For TWS SCO DMA snapshot and low delay
export PCM_FAST_MODE ?= 1

export CVSD_BYPASS ?= 1

export LOW_DELAY_SCO ?= 0

#SOUNDPLUS add by kyle 20210202
#should close other tx algorithm and audio resamle when open it
export SNDP_TX_3MIC_ENABLE ?= 0

SPEECH_TX_DC_FILTER ?= 0#

SPEECH_TX_AEC2FLOAT ?= 1#

SPEECH_TX_NS3 ?= 0

SPEECH_TX_2MIC_NS2 ?= 0

SPEECH_TX_COMPEXP ?= 0#

SPEECH_TX_EQ ?= 0

SPEECH_TX_POST_GAIN ?= 0

SPEECH_RX_NS2FLOAT ?= 0

SPEECH_RX_EQ ?= 0

SPEECH_RX_POST_GAIN ?= 0

LARGE_RAM ?= 1

HSP_ENABLE ?= 0

HFP_1_6_ENABLE ?= 1

MSBC_PLC_ENABLE ?= 1

MSBC_PLC_ENCODER ?= 1

MSBC_16K_SAMPLE_RATE ?= 1

SBC_FUNC_IN_ROM ?= 0

ROM_UTILS_ON ?= 0

APP_LINEIN_A2DP_SOURCE ?= 0

APP_I2S_A2DP_SOURCE ?= 0

VOICE_PROMPT ?= 1

#### Google related feature ####
# the overall google service switch
# currently, google service includes BISTO and GFPS
export GOOGLE_SERVICE_ENABLE ?= 0

# BISTO is a GVA service on Bluetooth audio device
# BISTO is an isolated service relative to GFPS
export BISTO_ENABLE ?= 0

# GSOUND_HOTWORD is a hotword library running on Bluetooth audio device
# GSOUND_HOTWORD is a subset of BISTO
export GSOUND_HOTWORD_ENABLE ?= 0

# GFPS is google fastpair service
# GFPS is an isolated service relative to BISTO
export GFPS_ENABLE ?= 0
#### Google related feature ####

BLE ?= 0

TOTA ?= 1

OTA_ENABLE ?= 1

TILE_DATAPATH_ENABLED ?= 0

CUSTOM_INFORMATION_TILE_ENABLE ?= 0

INTERCONNECTION ?= 0

INTERACTION ?= 0

INTERACTION_FASTPAIR ?= 0

BT_ONE_BRING_TWO ?= 0

DSD_SUPPORT ?= 0

A2DP_EQ_24BIT ?= 1

A2DP_AAC_ON ?= 1

A2DP_SCALABLE_ON ?= 0

A2DP_LHDC_ON ?= 0
ifeq ($(A2DP_LHDC_ON),1)
A2DP_LHDC_V3 ?= 0
endif

A2DP_LDAC_ON ?= 0

export TX_RX_PCM_MASK ?= 0

A2DP_SCALABLE_ON ?= 0

FACTORY_MODE ?= 1

ENGINEER_MODE ?= 1

ULTRA_LOW_POWER	?= 1

DAC_CLASSG_ENABLE ?= 1

NO_SLEEP ?= 0

CORE_DUMP ?= 1

CORE_DUMP_TO_FLASH ?= 0

ENHANCED_STACK ?= 1

CALIB_SLOW_TIMER ?= 1


export SYNC_BT_CTLR_PROFILE ?= 0

export A2DP_DECODER_VER := 2

export IBRT ?= 1

export IBRT_SEARCH_UI ?= 0

export BES_AUD ?= 1

export POWER_MODE   ?= DIG_DCDC

export BT_RF_PREFER ?= 2M

export SPEECH_CODEC ?= 1

#audio promot merge
export TWS_PROMPT_SYNC ?= 1
export MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED ?= 1

export FLASH_SIZE ?= 0x400000
export FLASH_SUSPEND ?= 1

ifeq ($(DSD_SUPPORT),1)
export BTUSB_AUDIO_MODE     ?= 1
export AUDIO_INPUT_MONO     ?= 1
export USB_ISO              ?= 1
export USB_AUDIO_DYN_CFG    ?= 1
export DELAY_STREAM_OPEN    ?= 0
export KEEP_SAME_LATENCY    ?= 1
export HW_FIR_DSD_PROCESS   ?= 1
ifeq ($(HW_FIR_DSD_PROCESS),1)
ifeq ($(CHIP),best2300)
export HW_FIR_DSD_BUF_MID_ADDR  ?= 0x200A0000
export DATA_BUF_START           ?= 0x20040000
endif
endif
export USB_AUDIO_UAC2 ?= 1
export USB_HIGH_SPEED ?= 1
KBUILD_CPPFLAGS += \
    -DHW_FIR_DSD_BUF_MID_ADDR=$(HW_FIR_DSD_BUF_MID_ADDR) \
    -DDATA_BUF_START=$(DATA_BUF_START)
endif

USE_THIRDPARTY ?= 0
export USE_KNOWLES ?= 0
export USE_CYBERON ?= 0

ifeq ($(CURRENT_TEST),1)
export VCODEC_VOLT ?= 1.6V
export VANA_VOLT ?= 1.35V
else
endif

export LAURENT_ALGORITHM ?= 0

export TX_IQ_CAL ?= 0

export BT_XTAL_SYNC ?= 1

export BTADDR_FOR_DEBUG ?= 1

export POWERKEY_I2C_SWITCH ?=0

AUTO_TEST ?= 0

export DUMP_NORMAL_LOG ?= 0

SUPPORT_BATTERY_REPORT ?= 1

SUPPORT_HF_INDICATORS ?= 0

SUPPORT_SIRI ?= 1

BES_AUDIO_DEV_Main_Board_9v0 ?= 0

APP_USE_LED_INDICATE_IBRT_STATUS ?= 0

export BT_EXT_LNA_PA ?=0
export BT_EXT_LNA ?=0
export BT_EXT_PA ?=0

ifeq ($(A2DP_LHDC_ON),1)
AUDIO_BUFFER_SIZE := 140*1024
else
AUDIO_BUFFER_SIZE := 100*1024
endif

export TRACE_BUF_SIZE := 16*1024
export TRACE_BAUD_RATE ?= 10*115200

init-y      :=
core-y      := platform/ services/ apps/ utils/cqueue/ utils/list/ services/multimedia/ utils/intersyshci/ zowee/

KBUILD_CPPFLAGS +=  -Iplatform/cmsis/inc \
                    -Iservices/audioflinger \
                    -Iplatform/hal \
                    -Iservices/fs/ \
                    -Iservices/fs/sd \
                    -Iservices/fs/fat \
                    -Iservices/fs/fat/ChaN

KBUILD_CPPFLAGS += \
    -DAPP_AUDIO_BUFFER_SIZE=$(AUDIO_BUFFER_SIZE) \
    -DCHARGER_PLUGINOUT_RESET=0 \
    -D__APP_KEY_FN_STYLE_A__ \
    -D__BT_ANC_KEY__

ifeq ($(BES_AUDIO_DEV_Main_Board_9v0),1)
KBUILD_CPPFLAGS += -DBES_AUDIO_DEV_Main_Board_9v0
endif

ifeq ($(TPORTS_KEY_COEXIST),1)
KBUILD_CPPFLAGS += -DTPORTS_KEY_COEXIST
endif

#-DIBRT_LINK_LOWLAYER_MONITOR

#-D_AUTO_SWITCH_POWER_MODE__
#-D__APP_KEY_FN_STYLE_A__
#-D__APP_KEY_FN_STYLE_B__
#-D__EARPHONE_STAY_BOTH_SCAN__
#-D__POWERKEY_CTRL_ONOFF_ONLY__
#-DAUDIO_LINEIN

ifeq ($(CURRENT_TEST),1)
INTSRAM_RUN ?= 1
endif
ifeq ($(INTSRAM_RUN),1)
LDS_FILE    := best1000_intsram.lds
else
LDS_FILE := zt102.lds
#LDS_FILE    := best1000.lds
endif

ifeq ($(TOTA),1)
ifeq ($(BLE),1)
KBUILD_CPPFLAGS += -DBLE_TOTA_ENABLED
endif
KBUILD_CPPFLAGS += -DSHOW_RSSI
KBUILD_CPPFLAGS += -DTEST_OVER_THE_AIR_ENANBLED
export TEST_OVER_THE_AIR ?= 1
endif

ifneq ($(A2DP_DECODER_VER), )
KBUILD_CPPFLAGS += -DA2DP_DECODER_VER=$(A2DP_DECODER_VER)
endif

ifeq ($(DEBUG_ANC_BY_PHONE),1)
KBUILD_CPPFLAGS += -DDEBUG_ANC_BY_PHONE
endif

ifeq ($(ANC_APP),1)
KBUILD_CPPFLAGS += -DANC_APP
endif

ifeq ($(ANC_WNR_ENABLED),1)
KBUILD_CPPFLAGS += \
    -DANC_WNR_ENABLED
endif

ifeq ($(USE_CYBERON),1)

export THIRDPARTY_LIB ?= cyberon
KBUILD_CPPFLAGS += -D__CYBERON

export KWS_IN_RAM := 1
ifeq ($(KWS_IN_RAM),1)
CPPFLAGS_${LDS_FILE} += -DKWS_IN_RAM
endif #KWS_IN_RAM

endif #USE_CYBERON

KBUILD_CFLAGS +=

LIB_LDFLAGS += -lstdc++ -lsupc++

#CFLAGS_IMAGE += -u _printf_float -u _scanf_float

#LDFLAGS_IMAGE += --wrap main

include $(KBUILD_ROOT)/zowee/zowee_config.mk
