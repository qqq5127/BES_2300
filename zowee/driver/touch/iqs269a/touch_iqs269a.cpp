#include "touch_iqs269a.h"

#if __TOUCH_IQS269A__
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "tgt_hardware.h"
#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "app_thread.h"
#include "hwtimer_list.h"
#include "twi_hal_if.h"
#include "gesture_hal_if.h"
#include "ui_event_manage.h"
#include "ui_event_gesture.h"
#include "twi_iqs_master.h"
#include "ui_event_a2dp.h"
#include "ui_event_wear_status.h"
#include "ui_event_product_test.h"



static osTimerId _touch_iqs269a_timer=NULL;
extern uint32_t test_count_timer_op;

osTimerDef(_TOUCH_IQS269A_TIMER,  (void (*)(void const *))iqs269a_count_timehandler);


static uint8_t init_cout_flag = 0;  // IQS269 Initializes the sequence flag cout;
static iqs269a_state_t iqs269a_state;
extern "C" struct iqs_iic_state iqs_state;


void iqs269a_touch_start_timer(void)
{
	//APP_IQS269A_TRACE("!!!");

	osTimerStart((osTimerId)_touch_iqs269a_timer, 12);
}

/**************************touch handler*******************************/
static void iqs269a_touch_control_handler(void)
{
	uint8_t reg = IQS269_REG_I2C_SETTING;
	uint8_t recie_data[10];
	uint8_t work_mode[2]={0};
	uint8_t work_mode_flag=0;
	uint8_t transferBytes[1] ={0x40};  
	iqs_state.i2c_stop_bit=0;

	//APP_IQS269A_TRACE("app_touch_int_cfg.pin : %d", hal_gpio_pin_get_val((HAL_GPIO_PIN_T)app_touch_int_cfg.pin));
	if(hal_gpio_pin_get_val((HAL_GPIO_PIN_T)app_touch_int_cfg.pin) != 1)
	{
		reg = IQS269_REG_I2C_SETTING;
		transferBytes[0] =0x40;//rh_iq_set | 0x40; 
		twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],1);

		reg = IQS269_MM_SYSTEM_FLAGS;
		twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&recie_data[0],10);

		reg = IQS269_MM_SYSTEM_FLAGS;//读两遍寄存器
		twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&recie_data[0],10);

		reg = IQS269_MM_SYSTEM_FLAGS;//读三遍寄存器
		twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&recie_data[0],10);

		reg = IQS269_MM_PMU_SETTINGS;//准备写入工作模式
		twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&work_mode[0],2);

		if((recie_data[6]&CHANNEL_0)==CHANNEL_0)//正常模式
		{
			if(work_mode_flag !=1)
			{
				work_mode[0] &= 0xE7;
				work_mode_flag =1;
				twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &work_mode[0],2);
			}
		}
		else if((recie_data[6]&CHANNEL_0)==0)//超低功耗模式
		{
			if(work_mode_flag ==1)
			{
				work_mode[0] |= 0x10;
				work_mode_flag =0;
				twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &work_mode[0],2);
			}
		}
		

		reg = IQS269_REG_I2C_SETTING;	
		twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&transferBytes[0],1);
		transferBytes[0] |= 0x80;
		transferBytes[0] &= 0xbf;
		iqs_state.i2c_stop_bit=1;
		twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],1);

		APP_IQS269A_TRACE("recie_data[6] = %2x ", recie_data[6]);
		if(work_mode_flag)
			APP_IQS269A_TRACE("mode is normal");
		else
			APP_IQS269A_TRACE("mode is lowpower");

		if((recie_data[6]==0x04)||(recie_data[6]==0x05))
		{
			product_test_stauts.touch_status=1;
			iqs269a_state.touch_state=1;
		}
		else if((recie_data[6]==0x00)||(recie_data[6]==0x01))
		{
			product_test_stauts.touch_status=0;
			iqs269a_state.touch_state=0;
		}

		iqs269a_touch_recog_handler(recie_data,10);
	}
}

void iqs269a_touch_recog_handler(uint8_t *data, uint8_t len)
{
	//APP_IQS269A_TRACE("!!!");
	APP_IQS269A_TRACE("fetch_status = %d ",ui_device_status.local_status.fetch_status);
	if((data[0]&SHOW_RESET)==SHOW_RESET)
	{
		// flag set to 0, redo init function;
		init_cout_flag = 0;  		
		APP_IQS269A_TRACE("IQS269 sensor abnormal and need to redo init again ");
	}
    else if((data[0]&ENENT_MODE)==ENENT_MODE)
    {
		if((data[4]&0x01)==0x01)//有接近感应
		{
			iqs269a_state.prox_flag=1;
		}
		else
		{
			iqs269a_state.prox_flag=0;

		}
	
		if((data[1]&TOUCH_EVENT)==TOUCH_EVENT)   //TOUCH EVEVT;
		{	
			//APP_IQS269A_TRACE("(data[1]&TOUCH_EVENT)==TOUCH_EVENT !!!");
		
			APP_IQS269A_TRACE("DATA [6] =%d ,iqs269a_state.rh_prox_flag = %d", data[6], iqs269a_state.prox_flag);
			if(((data[6]&CHANNEL_0)==CHANNEL_0)&&(iqs269a_state.prox_flag==1))  //CH0 in ear detection;   
            {
            	if(ui_device_status.local_status.fetch_status == UI_EVENT_STATUS_FETCH_OUT)
            	{
					if(iqs269a_get_wear_state() == 0)  //just wear off can do;
					{
						app_ui_event_process(UI_EVENT_WEAR, 0, UI_EVENT_WEAR_ON, 0, 0);
						iqs269a_set_wear_state(true);
						iqs269a_state.first_wear_on=1;
						iqs269a_state.touch_state=0;
						iqs269a_state.timeout_count=0;	
						iqs269a_state.touch_count=0;
					}	
            	}
            }
			else if(((data[6]== 0x00)||(data[6]==0x04))&&(iqs269a_state.prox_flag==0))	//0x00 means wear off and touch up or down
			{
				APP_IQS269A_TRACE("WEAR OFF iqs269a_get_wear_state() = %d", iqs269a_get_wear_state());	
				if(ui_device_status.local_status.fetch_status == UI_EVENT_STATUS_FETCH_OUT)
            	{
					if(iqs269a_get_wear_state() == 1) //just wear on can do;
					{
						app_ui_event_process(UI_EVENT_WEAR, 0, UI_EVENT_WEAR_OFF, 0, 0);
						iqs269a_set_wear_state(false);
						iqs269a_state.touch_state=0;
						iqs269a_state.timeout_count=0;	
						iqs269a_state.touch_count=0;
					}
				}
			}
//			APP_IQS269A_TRACE("iqs269 if wear on %d", iqs269_get_wear_state());	
			if(iqs269a_get_wear_state() == 1)
			{
				if((data[6]&CHANNEL_2)==CHANNEL_2)	//CH2 for touch	 
				{	
					if(iqs269a_state.first_wear_on==0)
					{
						//app_ui_event_process(UI_EVENT_IQS_TOUCH_DOWN, 0, 0, 0, 0);	// 
					}
						
					iqs269a_state.touch_state=1;
					
					if(iqs269a_state.touch_count==0)
						iqs269a_state.touch_count++;					

					if(iqs269a_state.timeout_count>0)//&&(iqs269a_state.timeout_count<20))//表示连续按键并未超时
					{
						iqs269a_state.timeout_count=0;
						iqs269a_state.touch_count++;
					}
					else
					{
						//开启事件
						iqs269a_touch_start_timer();
						//app_ui_event_process(UI_EVENT_IQS_TOUCH_DOWN, 0, 0, 0, 0);
					}
				}
				
				else if(data[6]==0x01)	//0x01 means wear on and touch up
				{
					if(iqs269a_state.first_wear_on==1)
						iqs269a_state.first_wear_on=0;

					if(iqs269a_state.long_click==1)//长按松开
					{
						iqs269a_state.long_click=0;
						iqs269a_state.timeout_count=0;	

					}
					iqs269a_state.touch_state=0;
				}	
			}
			else
				APP_IQS269A_TRACE("do not wear on!!!");
		}
		else if((data[1]&PROX_EVENT)==PROX_EVENT)
		{
			//APP_IQS269A_TRACE("(data[1]&PROX_EVENT)==PROX_EVENT !!!");
		
			if(((data[6]&CHANNEL_0)==CHANNEL_0)&&(iqs269a_state.prox_flag==1))  //CH0 in ear detection;   
            {
                if(ui_device_status.local_status.fetch_status == UI_EVENT_STATUS_FETCH_OUT)
            	{
					if(iqs269a_get_wear_state() == 0)  //just wear off can do;
					{
						app_ui_event_process(UI_EVENT_WEAR, 0, UI_EVENT_WEAR_ON, 0, 0);
						iqs269a_set_wear_state(true);
						iqs269a_state.first_wear_on=1;
						iqs269a_state.touch_state=0;
						iqs269a_state.timeout_count=0;	
						iqs269a_state.touch_count=0;
					}	
                }
            }
			else if(((data[6]== 0x00)||(data[6]==0x04))&&(iqs269a_state.prox_flag==0))	//0x00 means wear off and touch up or down
			{
			    if(ui_device_status.local_status.fetch_status == UI_EVENT_STATUS_FETCH_OUT)
            	{
					if(iqs269a_get_wear_state() == 1) //just wear on can do;
					{
						app_ui_event_process(UI_EVENT_WEAR, 0, UI_EVENT_WEAR_OFF, 0, 0);
						iqs269a_set_wear_state(false);
						iqs269a_state.touch_state=0;
						iqs269a_state.timeout_count=0;	
						iqs269a_state.touch_count=0;
					}
			    }
			}
		}
	}
}


void iqs269a_touch_timer_init(void)
{
	//APP_IQS269A_TRACE("!!!");
	
	_touch_iqs269a_timer = osTimerCreate (osTimer(_TOUCH_IQS269A_TIMER), osTimerOnce, NULL);
	test_count_timer_op =(uint32_t )_touch_iqs269a_timer;
}


//用来计数按键次数
void iqs269a_count_timehandler(void const *param)
{
//	APP_IQS269A_TRACE("00000000000001");
	//APP_IQS269A_TRACE("!!!");

	iqs269a_state.timeout_count++;
	if(iqs269a_state.timeout_count>=42)//超时300+ms
	{
		if(iqs269a_state.touch_state==0)//
		{
			if(iqs269a_state.touch_count==2)//double click
			{
				if(iqs269a_get_wear_state()==1)//防止误触
				{
					APP_IQS269A_TRACE("Double Click !!!");
					app_ui_event_process(UI_EVENT_GESTURE, 0, UI_EVENT_GESTURE_DB_CLICK_PRESS, 0, 0);
				}
			
			}
			else if(iqs269a_state.touch_count==3)//trible click
			{
				if(iqs269a_get_wear_state()==1)
				{
					APP_IQS269A_TRACE("Three Click !!!");
					app_ui_event_process(UI_EVENT_GESTURE, 0, UI_EVENT_GESTURE_TB_CLICK_PRESS, 0, 0);
				}
			}
			else 
			{
				if(iqs269a_get_wear_state()==1)
				{
					APP_IQS269A_TRACE("touch_count = %2x click", iqs269a_state.touch_count);
				}
			}
			
			iqs269a_state.touch_count=0;
			iqs269a_state.timeout_count=0;
			osTimerStop((osTimerId)_touch_iqs269a_timer);
		}
		else
		{
			if(iqs269a_state.timeout_count>=240)//长按
			{
				iqs269a_state.touch_count=0;
				iqs269a_state.long_click=1;
				if(iqs269a_get_wear_state()==1)
				{
					APP_IQS269A_TRACE("long press !!!");
					app_ui_event_process(UI_EVENT_GESTURE, 0, UI_EVENT_GESTURE_LONG_PRESS, 0, 0);
				}
			}
			else
				osTimerStart((osTimerId)_touch_iqs269a_timer, 12);
		}


	}
	else		
		osTimerStart((osTimerId)_touch_iqs269a_timer, 12);
}


#if 0
void iqs269a_touch_control(uint8_t flag)
{
	APP_IQS269A_TRACE("!!!");

	if(flag)
	{
		APP_IQS269A_TRACE("TOUCH EVENT OPEN!!!");
		osTimerStart((osTimerId)_touch_iqs269a_timer, 12);
	}
	else
	{
		osTimerStop((osTimerId)_touch_iqs269a_timer);
	}

}
#endif


int iqs269a_get_wear_state(void)
{
	//APP_IQS269A_TRACE(": %d", iqs269a_state.iqs269a_wear_flag);

	return iqs269a_state.iqs269a_wear_flag;
}

void iqs269a_set_wear_state(uint8_t flag)
{
	iqs269a_state.iqs269a_wear_flag = flag;

	//APP_IQS269A_TRACE(": %d", iqs269a_state.iqs269a_wear_flag);
}



/**************************touch init*******************************/

static int iqs269a_gobal_init(void)
{
	uint8_t transferBytes[14];	
	uint8_t rh_transferBytes[1];
	uint8_t res = 1;
	uint8_t reg = IQS269_MM_PMU_SETTINGS;

	//APP_IQS269A_TRACE("!!!");

	iqs_state.i2c_stop_bit=0;

	reg = IQS269_REG_I2C_SETTING;
	transferBytes[0] =0x40;//rh_iq_set | 0x40;
	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],1);

	reg = IQS269_REG_CHIP_ID;  
	res &= twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&transferBytes[0],4);
	// Addresses 0x80 Byte0 to 0x85 Byte1.
	transferBytes[0] = PMU_GENERAL_SETTINGS; 
	transferBytes[1] = 0x01; //ack reset I2C_GENERAL_SETTINGS | modify by sky,20200729; 
	transferBytes[2] = SYS_CHB_ACTIVE;
	transferBytes[3] = ACF_LTA_FILTER_SETTINGS;
	transferBytes[4] = LTA_CHB_RESEED_ENABLED;
	transferBytes[5] = UIS_GLOBAL_EVENTS_MASK;
	transferBytes[6] = PMU_REPORT_RATE_NP;
	transferBytes[7] = PMU_REPORT_RATE_LP;
	transferBytes[8] = PMU_REPORT_RATE_ULP;
	transferBytes[9] = PMU_MODE_TIMOUT;
	transferBytes[10] = I2C_WINDOW_TIMEOUT;
	transferBytes[11] = LTA_HALT_TIMEOUT;
	// Now write the Device Settings settings to the device.
	reg = IQS269_MM_PMU_SETTINGS;
	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],12);

	// Save the next 12 bytes of the Global Settings to the transferBytes aray.

	reg = IQS269_REG_I2C_SETTING;	
	res &= twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&rh_transferBytes[0],1);

	iqs_state.i2c_stop_bit=1;
	rh_transferBytes[0] |= 0x80;
  	rh_transferBytes[0] &= 0xbf;
	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &rh_transferBytes[0],1);

	APP_IQS269A_TRACE("set_global_setting is ok !!!");

	return 0;
}


static int iqs269a_channal_init(void)
{
	uint8_t transferBytes[14];	
	uint8_t rh_transferBytes[2];
	uint8_t res = 1;
	uint8_t reg = IQS269_REG_I2C_SETTING;
	
	//APP_IQS269A_TRACE("!!!");

	iqs_state.i2c_stop_bit=0;
	transferBytes[0] =0x40;//rh_iq_set | 0x40;

	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],1);

	reg = IQS269_MM_PXS_GLBL_SETTINGS_0;
	transferBytes[0] = PXS_GENERAL_SETTINGS0;
	transferBytes[1] = PXS_GENERAL_SETTINGS1;
	transferBytes[2] = UIS_ABSOLUTE_CAPACITANCE;
	transferBytes[3] = UIS_DCF_GENERAL_SETTINGS;
	transferBytes[4] = GEM_CHB_BLOCK_NFOLLOW;
	transferBytes[5] = 0x00;
	transferBytes[6] = UIS_CHB_SLIDER0 ;
	transferBytes[7] = UIS_CHB_SLIDER1;
	transferBytes[8] = UIS_GESTURE_TAP_TIMEOUT;
	transferBytes[9] = UIS_GESTURE_SWIPE_TIMEOUT;
	transferBytes[10] = UIS_GESTURE_THRESHOLD;
	transferBytes[11] =  LTA_CHB_RESEED; // USE CHANNEL 2,6. ATI need to set 1 ;//adjust;LTA_CHB_RESEED ; //
	// Now write the Device Settings settings to the device.

	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],12);	
	reg = IQS269_MM_CH0_SELECT_CRX;
	// Addresses 0x8C Byte0 to 0x92 Byte1.
	transferBytes[0] = PXS_CRXSEL_CH0;
	transferBytes[1] = PXS_CTXSEL_CH0;
	transferBytes[2] = PXS_PROXCTRL_CH0;
	transferBytes[3] = PXS_PROXCFG0_CH0;
	transferBytes[4] = PXS_PROXCFG1_TESTREG0_CH0;
	transferBytes[5] = ATI_BASE_AND_TARGET_CH0;
	transferBytes[6] = ATI_MIRROR_CH0;
	transferBytes[7] = ATI_PCC_CH0;
	transferBytes[8] = PXS_PROX_THRESHOLD_CH0;
	transferBytes[9] = PXS_TOUCH_THRESHOLD_CH0;
	transferBytes[10] = PXS_DEEP_THRESHOLD_CH0;
	transferBytes[11] = PXS_HYSTERESIS_CH0 ;
	transferBytes[12] = DCF_CHB_ASSOCIATION_CH0;
	transferBytes[13] = DCF_WEIGHT_CH0;

	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],14);	
	reg = IQS269_MM_CH1_SELECT_CRX;

	transferBytes[0] = PXS_CRXSEL_CH1;
	transferBytes[1] = PXS_CTXSEL_CH1;
	transferBytes[2] = PXS_PROXCTRL_CH1;
	transferBytes[3] = PXS_PROXCFG0_CH1;
	transferBytes[4] = PXS_PROXCFG1_TESTREG0_CH1;
	transferBytes[5] = ATI_BASE_AND_TARGET_CH1;
	transferBytes[6] = ATI_MIRROR_CH1;
	transferBytes[7] = ATI_PCC_CH1;
	transferBytes[8] = PXS_PROX_THRESHOLD_CH1;
	transferBytes[9] = PXS_TOUCH_THRESHOLD_CH1;
	transferBytes[10] = PXS_DEEP_THRESHOLD_CH1;
	transferBytes[11] = PXS_HYSTERESIS_CH1 ;
	transferBytes[12] = DCF_CHB_ASSOCIATION_CH1;
	transferBytes[13] = DCF_WEIGHT_CH1;

	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],14);

	transferBytes[0] = PXS_CRXSEL_CH2;
	transferBytes[1] = PXS_CTXSEL_CH2;
	transferBytes[2] = PXS_PROXCTRL_CH2;
	transferBytes[3] = PXS_PROXCFG0_CH2;
	transferBytes[4] = PXS_PROXCFG1_TESTREG0_CH2;
	transferBytes[5] = ATI_BASE_AND_TARGET_CH2;
	transferBytes[6] = ATI_MIRROR_CH2;
	transferBytes[7] = ATI_PCC_CH2;
	transferBytes[8] = PXS_PROX_THRESHOLD_CH2;
	transferBytes[9] = PXS_TOUCH_THRESHOLD_CH2;
	transferBytes[10] = PXS_DEEP_THRESHOLD_CH2;
	transferBytes[11] = PXS_HYSTERESIS_CH2 ;
	transferBytes[12] = DCF_CHB_ASSOCIATION_CH2;
	transferBytes[13] = DCF_WEIGHT_CH2;

	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, IQS269_MM_CH2_SELECT_CRX, &transferBytes[0],14);

	reg = IQS269_MM_PMU_SETTINGS;
	rh_transferBytes[0] = 0x03;//0x03;
	rh_transferBytes[1] = 0x00 | 0x04; // redo ATI 
	twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &rh_transferBytes[0],2);

	reg = IQS269_REG_I2C_SETTING;	
	res &= twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&rh_transferBytes[0],1);

	iqs_state.i2c_stop_bit=1;
	//rh_transferBytes[0] = 0x80;//rh_iq_set | 0x80;
	rh_transferBytes[0] |= 0x80;
	rh_transferBytes[0] &= 0xbf;
	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &rh_transferBytes[0],1);


	APP_IQS269A_TRACE("set_channel 0&1 is ok !!!!!!");
	
	return 0;
}


int iqs269a_event_start(void)//clear reset reg and redo ATI
{
	uint8_t receive_data[2];
	uint8_t transferBytes[15];
	uint8_t rh_transferBytes[1];
	uint8_t res = 0;
	uint8_t reg = 0; //add by sky;
	iqs_state.i2c_stop_bit=0;

	//APP_IQS269A_TRACE("!!!");	 

	rh_transferBytes[0] = 0x40;//rh_iq_set | 0x80;
	reg = IQS269_REG_I2C_SETTING;
	//	 i2c_write(TOUCH_IQS_ADDR,(uint8_t *)&reg, 1,&rh_transferBytes[0], 1);	 
	twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &rh_transferBytes[0],1);

	reg = IQS269_MM_SYSTEM_FLAGS;
	//	 res &= i2c_read(TOUCH_IQS_ADDR, (uint8_t *)&reg, 1, &receive_data, 1);  
	res &= twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&receive_data[0],2);
	if((receive_data[0] & 0x04) == 0x04)
	{
		init_cout_flag--;
		APP_IQS269A_TRACE("ati_loop error turn -2 and IQS269_MM_SYSTEM_FLAGS =0x%x",receive_data[0]);		  
		return -2; 	 
	}

	reg = IQS269_MM_PMU_SETTINGS;
	res &= twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&transferBytes[0],2);
	transferBytes[0] = 0x03;
	transferBytes[1] = 0x00 | 0x20; //EVENT MODE

	twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &transferBytes[0],2);

	reg = IQS269_REG_I2C_SETTING;	 
	res &= twi_iqs_master_read(TOUCH_IQS_ADDR, reg,&rh_transferBytes[0],1);
	//rh_transferBytes[0] = 0x80;//rh_iq_set | 0x80;
	iqs_state.i2c_stop_bit=1;
	rh_transferBytes[0] |= 0x80;
	rh_transferBytes[0] &= 0xbf;
	res &= twi_iqs_master_write(TOUCH_IQS_ADDR, reg, &rh_transferBytes[0],1);


	APP_IQS269A_TRACE("transferBytes 0 = %x",transferBytes[0]);
	APP_IQS269A_TRACE("transferBytes 1 = %x",transferBytes[1]);
	APP_IQS269A_TRACE("init ati_loop   is ok !!!!!!");


	return 0;
}

static void iqs269a_int_handler_call_back(enum HAL_GPIO_PIN_T pin)
{
//	APP_IQS269A_TRACE("!!!");

	app_ui_event_process(UI_EVENT_GESTURE, 0, UI_EVENT_GESTURE_INT, 0, 0);
	//iqs269a_disable();
}

static void iqs269a_gpiote_enable(void)
{
	//APP_IQS269A_TRACE("!!!");
	
	struct HAL_GPIO_IRQ_CFG_T gpiocfg;

	//hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_touch_int_cfg,1);
	//hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )app_touch_int_cfg.pin, HAL_GPIO_DIR_IN, 0);

    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;//HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_handler = iqs269a_int_handler_call_back;	
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    
    hal_gpio_setup_irq((HAL_GPIO_PIN_T )app_touch_int_cfg.pin, &gpiocfg);

//	init_cout_flag = 0;  
}

static void iqs269a_gpiote_disable(void)
{
	//APP_IQS269A_TRACE("!!!");

    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

//	memset(&gpiocfg,0,sizeof(gpiocfg));
    
// 	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_touch_int_cfg,1);
//	hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )app_touch_int_cfg.pin, HAL_GPIO_DIR_IN, 0);

    gpiocfg.irq_enable = false;
    gpiocfg.irq_debounce = false;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;//HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_handler = NULL;	
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    
    hal_gpio_setup_irq((HAL_GPIO_PIN_T )app_touch_int_cfg.pin, &gpiocfg);
}

static void iqs269a_enable(void)
{
	//APP_IQS269A_TRACE("!!!");

	iqs269a_ldo_enable();
	
    iqs269a_gpiote_enable();
	
//	init_cout_flag = 0;  
}

static void iqs269a_disable(void)
{
	//APP_IQS269A_TRACE("!!!");

	iqs269a_ldo_disable();

	iqs269a_gpiote_disable();
}

void iqs269a_int_handler(void)
{	 
	//APP_IQS269A_TRACE("init_cout_flag: %d !!!", init_cout_flag);

	if(init_cout_flag >= 4)
	{
		iqs269a_touch_control_handler();
	}
	else
	{
		APP_IQS269A_TRACE("init_cout_flag: %d !!!", init_cout_flag);
		switch(init_cout_flag)
		{
			case 0:

				break;
			case 1: 
				iqs269a_gobal_init();
				break;
			case 2:
				iqs269a_channal_init();
				break;		
			case 3:
				iqs269a_event_start();
				break;

			default:

				break;
		}
		
		init_cout_flag++;
	}

	iqs269a_gpiote_enable();
}

static void iqs269a_gpiote_init(void)
{
	//APP_IQS269A_TRACE("!!!");

 	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_touch_int_cfg,1);
	hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )app_touch_int_cfg.pin, HAL_GPIO_DIR_IN, 0);
	
	iqs269a_gpiote_enable();

	APP_IQS269A_TRACE("success!!!\n");
}

void iqs269a_ldo_enable(void)
{
	//APP_IQS269A_TRACE("!!!");

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_ldo_en_cfg, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_ldo_en_cfg.pin, HAL_GPIO_DIR_OUT, 1);
	hal_sys_timer_delay_us(1000);
}

void iqs269a_ldo_disable(void)
{
	//APP_IQS269A_TRACE("!!!");

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_ldo_en_cfg.pin, HAL_GPIO_DIR_OUT, 0);
	hal_sys_timer_delay_us(1000);
}


int iqs269a_init(void)
{  	
	uint8_t rec_data;
	
	twi_hal_if_init();
	iqs269a_ldo_enable();

	APP_IQS269A_TRACE("!!!\n");
	
	//for BES2300 reset but iqs269 also in event mode,let rdy low for reset init
	twi_iqs_master_read(TOUCH_IQS_ADDR, 0xf2,&rec_data,1);
	APP_IQS269A_TRACE(": %d\n",rec_data);
	
	iqs269a_gpiote_init();
	
	iqs269a_touch_timer_init();

	ui_device_status.local_status.fetch_status=UI_EVENT_STATUS_PUT_IN;
	
	return 0;
}



extern "C" const hal_gesture_if_t gesture_ctrl =
{
    iqs269a_init,
    iqs269a_int_handler,
    iqs269a_disable,
    iqs269a_enable
};

extern "C" const hal_wear_if_t wear_ctrl =
{
	NULL,
	NULL,
	NULL,
	NULL,
};


#endif

