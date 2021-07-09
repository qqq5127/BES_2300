export PROJ_NAME ?= zt203


export NO_LPU_26M ?= 1

KBUILD_CPPFLAGS += -D__PROJ_NAME_ZT203__

#public
KBUILD_CPPFLAGS += -D__APP_ZOWEE_UI__

#software version information for test
KBUILD_CPPFLAGS += -D__APP_ZOWEE_SOFTWARE_VERSION__

#bus
KBUILD_CPPFLAGS += -D__BUS_TWI_SW_TWI__
#KBUILD_CPPFLAGS += -D__BUS_TWI_HW_TWI__


#Gesture Manage Micro Control
#gsenor or touch support
KBUILD_CPPFLAGS += -D__APP_GESTURE_ENABLE__
	KBUILD_CPPFLAGS += -D__TOUCH_GPIOTE__

#Wear Manage Micro Control
KBUILD_CPPFLAGS += -D__APP_WEAR_ENABLE__
	KBUILD_CPPFLAGS += -D__IR_W2001__
	KBUILD_CPPFLAGS += -D__WEAR_CONTROL_ANC__
	
#product test
KBUILD_CPPFLAGS += -D__SCPI_TEST__

#VBUS Manage Micro Control
KBUILD_CPPFLAGS += -D__APP_VBUS_ENABLE__
	KBUILD_CPPFLAGS += -D__VBUS_USE_UART__

#BATTERY USER FUNCTION
KBUILD_CPPFLAGS += -D__APP_BATTERY_USER__
	#KBUILD_CPPFLAGS += -D__BATTERY_LOWPOWER_PROMPT_FIXED__
	KBUILD_CPPFLAGS += -D__BATTERY_ZT203__

#anc hearthrough
KBUILD_CPPFLAGS += -D__APP_ANC_HEAR_THROUGH__

#bt user_config
KBUILD_CPPFLAGS += -D__APP_HOLD_TWS_WHILE_PAIRING_MODE__

#ring ring
KBUILD_CPPFLAGS += -D__BT_WARNING_TONE_MERGE_INTO_STREAM_SBC__

ifeq ($(DEBUG),1)
#user gpio set earside
KBUILD_CPPFLAGS += -D__ZOWEE_ANC_TEST_VIA_SPP__
endif

