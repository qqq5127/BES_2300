#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "tgt_hardware.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "app_thread.h"
#include "hwtimer_list.h"
#include "app_thread.h"
#include "hal_bootmode.h"

#include "ui_event_manage.h"
#include "zowee_config.h"
#include "ui_event_scpi.h"
#include "app_scpi_test.h"
#include "apps.h"
#include "app_factory_bt.h"
#include "app_factory.h"
#include "wear_hal_if.h"
#include "gesture_hal_if.h"
#include "battery_hal_if.h"
#include "vbus_hal_if.h"
#include "ui_event_product_test.h"
#include "norflash_api.h"
#include "nvrecord_bt.h"
#include "app_ibrt_nvrecord.h"
#include "app_spp_tota.h"
#include "nvrecord_env.h"


#ifdef __SCPI_TEST__

extern int app_nvrecord_rebuild(void);


void ui_manage_scpi_event(UI_MANAGE_MESSAGE_BLOCK *msg_body)
{
	switch(msg_body->id)
	{
		case UI_EVENT_SCPI_SPP_PARSE:
		{
			uint8_t *buff;
			uint8_t buff_length;

			product_test_stauts.spp_vbus_flag = 0;
			buff_length = (*((uint8_t *)(msg_body->param1)));
			buff = (uint8_t *)(msg_body->param0);
			SCPI_Input_Fun((const char *)buff,(int )buff_length);
		}
		break;
		
		case UI_EVENT_SCPI_VBUS_PARSE:
		{
			uint8_t *buff;
			uint8_t buff_length;

			product_test_stauts.spp_vbus_flag = 1;
			buff_length = (*((uint8_t *)(msg_body->param1)));
			buff = (uint8_t *)(msg_body->param0);
			SCPI_Input_Fun((const char *)buff,(int )buff_length);
		}
		break;

		case UI_EVENT_SCPI_DUT_MODE:
		{
			app_scpi_entry_dut();
		}
		break;

		case UI_EVENT_SCPI_SPP_FACTORY_MODE:
		{
			uint8_t resume_factory_test_string[] = {"zowee resume factory 1"};
			struct nvrecord_env_t *nvrecord_env;

			nv_record_env_get(&nvrecord_env);
			nvrecord_env->anc_state = UI_ANC_OFF;	
			nv_record_env_set(nvrecord_env);	

			app_ibrt_nvrecord_delete_all_mobile_record();
			nv_record_flash_flush();			
			osDelay(1000);
			app_tota_send_cmd_via_spp(resume_factory_test_string,22);
			osDelay(1000);
			UI_SCPI_TRACE("reboot!");
			hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
			hal_cmu_sys_reboot();
		}
		break;
		default:

		break;
	}
}

#endif

