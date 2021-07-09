#include "ir_w2001.h"

#if __IR_W2001__
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
#include "wear_hal_if.h"
#include "ui_event_manage.h"
#include "ui_event_wear_status.h"
#include "zowee_section.h"
#include "norflash_api.h"


static ir_w2001_struct_t ir_w2001_data;

static void ir_w2001_self_calibration_timehandler(void const *param);

osTimerDef(IR_W2001_SELF_CALI_TIMER, ir_w2001_self_calibration_timehandler);
static osTimerId ir_w2001_self_cali_timer = NULL;

#if IR_W2001_TEST
static void ir_w2001_test_timehandler(void const *param);

osTimerDef(IR_W2001_TEST_TIMER, ir_w2001_test_timehandler);
static osTimerId ir_w2001_test_timer = NULL;

uint16_t STK_Read_PS(void);
static uint8_t STK_ReadReg(uint8_t addr);

static void ir_w2001_test_timehandler(void const *param)
{
	IR_W2001_TRACE("ps data %d", STK_Read_PS());
	
}
#endif


static bool ir_w2001_need_refresh_flash(void)
{
	uint16_t offset_flash,offset_actually;

	if(ir_w2001_data.ps_actually_max < ir_w2001_data.ps_actually_min)
	{
		IR_W2001_TRACE("data don't collected\n");
		return false;
	}
	offset_flash = ir_w2001_data.ps_data_max - ir_w2001_data.ps_data_min;
	offset_actually = ir_w2001_data.ps_actually_max - ir_w2001_data.ps_actually_min;
	IR_W2001_TRACE("%d %d %d %d \n",offset_flash,offset_actually,ir_w2001_data.ps_data_max,ir_w2001_data.ps_actually_min);
	if(offset_actually < IR_W2001_NEED_CALIBRATION_OFFSET)
	{
			IR_W2001_TRACE("offset not reach not need refresh");
			return false;
	}

	if((ir_w2001_data.ps_data_max) < ir_w2001_data.ps_actually_min)
	{
		IR_W2001_TRACE("MM5 value in flash bigger then actually CT value,need refresh");
		app_ui_event_process(UI_EVENT_WEAR,0,UI_EVENT_WEAR_REFRESH_CALI_DATA,0,0);
		return true;
	}
	return false;

}

static void ir_w2001_self_calibration_timehandler(void const *param)
{
	uint16_t ps_data;

	ps_data = STK_Read_PS();
	if((ps_data > ir_w2001_data.ps_actually_max) && (ps_data != 0xffff))
	{
		ir_w2001_data.ps_actually_max = ps_data;
	}
	if((ps_data < ir_w2001_data.ps_actually_min) && (ps_data != 0))
	{
		ir_w2001_data.ps_actually_min = ps_data;
	}
	IR_W2001_TRACE("%d %d %d %d %d %d",ir_w2001_data.read_ps_data_times,ps_data,ir_w2001_data.ps_actually_max,ir_w2001_data.ps_actually_min,ir_w2001_data.ps_data_max,ir_w2001_data.ps_data_min);
	ir_w2001_data.read_ps_data_times++;
	if(ir_w2001_data.read_ps_data_times < IR_W2001_READ_PS_DATA_TIME)
	{
		osTimerStart(ir_w2001_self_cali_timer,100); // 100ms
	}
}

static uint8_t STK_ReadReg(uint8_t addr)
{
	uint8_t transferBytes_buff[2];
	
	twi_hal_if_read(IR_W2001_ADDR, addr,(uint8_t *)&transferBytes_buff[0],1);
	//IR_W2001_TRACE("STK_ReadReg: %d  %d \r\n",transferBytes_buff[0],transferBytes_buff[1]);

	return transferBytes_buff[0];
}

static uint8_t STK_WriteReg(uint8_t addr,uint8_t value)
{
	uint8_t transferBytes_buff[2],res;

	transferBytes_buff[0] = addr;
	transferBytes_buff[1] = value;
	res = twi_hal_if_write(IR_W2001_ADDR, &transferBytes_buff[0],1,1);

	return res;
}

#if 0
void STK_Show_Allreg(void)
{
	uint8_t reg[13] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x10, 0x3E, 0x3F, 0xA0, 0xA1, 0xDB, 0xF4};
	int i = 0;
	uint8_t val = 0;
	for(i=0;i<13;i++)
	{
		val = STK_ReadReg(reg[i]);		
		IR_W2001_TRACE("STK_Show_Allreg STK_ReadReg(%x) = %x\n", reg[i], val);
	}
}
#endif
void STK_PowerOn(void)
{
	uint8_t ID;
	
	STK_sw_reset();
	hal_sys_timer_delay_us(50000);
	
	ID = STK_ReadReg(0x3E);
	IR_W2001_TRACE("STK_PowerOn ID = 0x%x, 0x%x\r\n",ID, STK_ReadReg(0x3F));

	STK_Init();
	//STK_Show_Allreg();
}

 void STK_sw_reset(void)
 {
	STK_WriteReg(0x80,0x99);//W any data to reset the chip

 }
void STK_Init(void)
{
	uint16_t ps_thd_h, ps_thd_l;
   uint32_t scpi_ir_flash_ct_value,scpi_ir_flash_5mm_value;

   STK_WriteReg(0x00, 0x00);/*0x05 only enable ps;0x02 only enable als; 
   						    0x00 disable all &go into a low power standby mode//STATE */
    STK_WriteReg(0x01, 0x70);
    STK_WriteReg(0x02, 0x31);
    STK_WriteReg(0x03, 0x40); 
	//STK_WriteReg(0xF4, 0x00);

#ifdef PS_INTERRUPT_MODE
    STK_WriteReg(0x04, 0x01);
#else
    STK_WriteReg(0x04, 0x00);		
#endif

	//WAIT TIME
	STK_WriteReg(0x05, 0x1F);    

    STK_WriteReg(0x0A, 0xFF);//THDH1_ALS
    STK_WriteReg(0x0B, 0xFF);//THDH2_ALS
    STK_WriteReg(0x0C, 0x00);//THDL1_ALS
    STK_WriteReg(0x0D, 0x00);//THDL2_ALS

	STK_WriteReg(0xA0, 0x10);//ENABLE BGIR	
	STK_WriteReg(0xA1, 0x06);//PD1
	STK_WriteReg(0xDB, 0x15);//AGainx1

	STK_WriteReg(0x00, 0x05);
	hal_sys_timer_delay_us(10000);
	
	zowee_section_get_ir_calib_value(PT_IR_CT,&scpi_ir_flash_ct_value);
	zowee_section_get_ir_calib_value(PT_IR_5MM,&scpi_ir_flash_5mm_value);
	IR_W2001_TRACE("flash value = %d %d\r\n",scpi_ir_flash_5mm_value,scpi_ir_flash_ct_value);
	{
		uint16_t ir_diff;
		
		ir_diff = (uint32_t )(scpi_ir_flash_5mm_value - scpi_ir_flash_ct_value);
		ps_thd_h = (uint32_t )(scpi_ir_flash_ct_value + ir_diff*0.9); 
		ps_thd_l = (uint32_t )(scpi_ir_flash_ct_value + ir_diff*0.5);
	}
	
    STK_WriteReg(0x06, (ps_thd_h & 0xFF00) >> 8);//THDH1_PS  
    STK_WriteReg(0x07, (ps_thd_h & 0x00FF));//THDH2_PS  
    STK_WriteReg(0x08, (ps_thd_l & 0xFF00) >> 8);//THDL1_PS
    STK_WriteReg(0x09, (ps_thd_l & 0x00FF));//THDL2_PS	
	IR_W2001_TRACE("high thd = %d\r\n",ps_thd_h);
	IR_W2001_TRACE("low thd = %d\r\n",ps_thd_l);
}

void stk_als_set_new_thd(uint16_t alscode)
{
    uint32_t high_thd,low_thd;
	uint16_t high_thd_u16, low_thd_u16;
	
    high_thd = alscode + ALS_INTERRUPT_THD_DIFF;
    low_thd = alscode - ALS_INTERRUPT_THD_DIFF;
    if (high_thd >= (1<<16))
    {
    	high_thd = (1<<16) -1;
    }
    if (low_thd <0)
    {
    	low_thd = 0;
    }
	high_thd_u16 = (uint16_t)high_thd;
	low_thd_u16 = (uint16_t)low_thd;
	
    STK_WriteReg(0x0A, (uint8_t)((high_thd_u16&0xFF00) >> 8));//THDH1_ALS
    STK_WriteReg(0x0B, (uint8_t)(high_thd_u16&0x00FF));//THDH2_ALS
    STK_WriteReg(0x0C, (uint8_t)((low_thd_u16&0xFF00) >> 8));//THDL1_ALS
    STK_WriteReg(0x0D, (uint8_t)(low_thd_u16&0x00FF));//0xf0//THDL2_ALS		
	return;
}

uint16_t STK_Read_ALS(void)
{
	uint16_t als_data;

    als_data = ((STK_ReadReg(0x13)<<8)|STK_ReadReg(0x14));//ALS_DATA

	return als_data;    

}

uint16_t STK_Read_PS(void)
{
	uint16_t ps_data;

    ps_data = ((STK_ReadReg(0x11)<<8)|STK_ReadReg(0x12));//PS_DATA

	return ps_data;    


}

#if 0//defined(PS_INTERRUPT_MODE) || defined(ALS_INTERRUPT_MODE)
void eint_work_func(void)
{	
	uint8_t flag = 0;
	uint8_t nf_data = 0;
	uint16_t ps_data = 0, als_data = 0;
	uint8_t flag_psint = 0, flag_alsint = 0;
   	//uint32_t savedMask;		// TODO
   	
	//savedMask = SaveAndSetIRQMask(); //disable interrupt	TODO
	
	flag = STK_ReadReg(0x10);
	IR_W2001_TRACE("eint_work_func: flag = 0x%x\r\n",flag);
	
	flag_psint = flag & 0x10;
	flag_alsint = flag & 0x20;
	
	if(flag_psint) //IF PS Interrupt
	{
		ps_data = STK_Read_PS();
		IR_W2001_TRACE("eint_work_func: ps = %d\r\n",ps_data);

		if(flag & 0x01)  // Far
		{
		    nf_data = 0;
			//to do something
		}
		else  // Near
		{
		    nf_data = 1;
			//to do something
		}
		
		//report_ps_value(ps_data, nf_data);  //Report ps_data and ps event(Far or Near) to system TODO
		IR_W2001_TRACE("report_ps_value: nf_data = %d\r\n",nf_data);
		STK_WriteReg(0x10,flag & (~0x10)); //clear FLG_PSINT ( Bit[4] )

	}

	if(flag_alsint) //IF ALS Interrupt
	{
		als_data = STK_Read_ALS();
		IR_W2001_TRACE("eint_work_func:als = %d\r\n",als_data);
		stk_als_set_new_thd(als_data);
		//report_als_value(als_data);  //Report als_data to system
		STK_WriteReg(0x10,flag & (~0x20));//clear FLG_ALSINT ( Bit[5] )
	}
	
	//STK_Show_Allreg();	//for debug

	//RestoreIRQMask(savedMask); // turn on interrupt TODO
}
#endif

#ifndef PS_INTERRUPT_MODE
void get_ps_data(void)
{	
	uint8_t flag = 0;
	uint8_t nf_data = 0;

	flag = STK_ReadReg(0x10);
	IR_W2001_TRACE("get_ps_data: flag = 0x%x\r\n",flag);
	
	ps_data = STK_Read_PS();
	IR_W2001_TRACE("get_ps_data: ps = %d\r\n",ps_data);

	if(flag & 0x01)  // Far
	{
		nf_data = 0;
		//to do something
	}
	else  // Near
	{
		nf_data = 1;
		//to do something
	}
	
	report_ps_value(ps_data, nf_data);  //Report ps_data and ps event(Far or Near) to system

	//STK_Show_Allreg();	//for debug
}
#endif 

#if 0//ndef ALS_INTERRUPT_MODE 
void read_als_data(void)
{
	uint8_t flag = 0;
	uint16_t als_data = 0;

	flag = STK_ReadReg(0x10);
	IR_W2001_TRACE("read_als_data flag = 0x%x\r\n",flag);

	als_data = STK_Read_ALS();
	IR_W2001_TRACE("read_als_data  = %d\r\n",als_data);
}
#endif

/**********************************************************************
int STK_enable_PS()
int STK_enable_ALS()
**********************************************************************
Description: enable ps/als 

Parameters:
        None
Return Value:
        PS_en/ALS_en
            
**********************************************************************/
uint32_t STK_enable_PS(void)
{
	uint8_t PS_en;
	uint8_t state = 0;
	uint8_t flag = 0;
	
	state = STK_ReadReg(0x00);	
    PS_en = STK_WriteReg(0x00,0x05 | state);//enable ps and wait time
	IR_W2001_TRACE("STK_enable_PS,  0x00 = 0x%x\r\n",0x05 | state);
	flag = STK_ReadReg(0x10);
	STK_WriteReg(0x10,flag & (~0x10)); //clear FLG_PSINT ( Bit[4] )
	
	return PS_en;
}

uint32_t STK_DisEnable_PS(void)
{
	uint8_t PS_en;
	uint8_t state = 0;
	
	//IR_W2001_TRACE("STK_DisEnable_PS \n");
	state = STK_ReadReg(0x00);	
	
    PS_en = STK_WriteReg(0x00,(~0x05) & state);//disenable ps and wait time
	
	//IR_W2001_TRACE("STK_DisEnable_PS,  Reg[0x00] = 0x%x\r\n", ((~0x05) & state));

	return PS_en;
}

uint32_t STK_enable_ALS(void)
{
	uint32_t ALS_en;
	uint8_t state = 0;
	
	state = STK_ReadReg(0x00);		
    ALS_en = STK_WriteReg(0x00,0x02 | state);//only enable als,no wait time

	//IR_W2001_TRACE("STK_enable_ALS,  0x00 = 0x%x\r\n",0x02 | state);

	return ALS_en;
}

uint32_t STK_DisEnable_ALS(void)
{
	uint32_t ALS_en;
	uint8_t state = 0;
	
	IR_W2001_TRACE("STK_DisEnable_ALS \n");
	state = STK_ReadReg(0x00);		
    ALS_en = STK_WriteReg(0x00,(~0x02) & state);//only disenable als,no wait time

	return ALS_en;
}


static void ir_w2001_ldo_enable(void)
{
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&ldo_en_cfg, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)ldo_en_cfg.pin, HAL_GPIO_DIR_OUT, 1);
	hal_sys_timer_delay_us(1000);
}

static void w2001_int_handler_call_back(enum HAL_GPIO_PIN_T pin)
{
	IR_W2001_TRACE("\n");	

	app_ui_event_process(UI_EVENT_WEAR,0,UI_EVENT_WEAR_INT,0,0);
}

static void ir_w2001_gpiote_enable(void)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = 0;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;//HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_handler = w2001_int_handler_call_back;	
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    
    hal_gpio_setup_irq((HAL_GPIO_PIN_T )ir_w2001_int_cfg.pin, &gpiocfg);
	hal_gpio_pin_get_val((HAL_GPIO_PIN_T)ir_w2001_int_cfg.pin);

}

static void ir_w2001_gpiote_disable(void)
{
#if 1
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

	memset(&gpiocfg,0,sizeof(gpiocfg));
    
 	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&ir_w2001_int_cfg,1);
	hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )ir_w2001_int_cfg.pin, HAL_GPIO_DIR_IN, 0);

    gpiocfg.irq_enable = false;
    gpiocfg.irq_debounce = 0;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;//HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_handler = NULL;	
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    
    hal_gpio_setup_irq((HAL_GPIO_PIN_T )ir_w2001_int_cfg.pin, &gpiocfg);


#endif
}


static void ir_w2001_int_init(void)
{
 	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&ir_w2001_int_cfg,1);
	hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )ir_w2001_int_cfg.pin, HAL_GPIO_DIR_IN, 0);

	ir_w2001_gpiote_enable();
	//STK_ReadReg(0x3E);
	//IR_W2001_TRACE("0x%x\r\n", STK_ReadReg(0x3F));
}

void ir_w2001_int_handler(void)
{
	uint8_t flag = 0;
	uint8_t nf_data = 0;
	uint16_t ps_data = 0;
	uint8_t flag_psint = 0, flag_alsint = 0;
   	//uint32_t savedMask;		// TODO
   	
	//ir_w2001_gpiote_disable();//disable interrupt
	
	flag = STK_ReadReg(0x10);
	
	flag_psint = flag & 0x10;
	flag_alsint = flag & 0x20;
	IR_W2001_TRACE("eint_work_func: %d flag_psint %d flag_alsint %d \r\n",flag,flag_psint,flag_alsint);
	
#if PS_INTERRUPT_MODE
	if(flag_psint) //IF PS Interrupt
	{
		ps_data = STK_Read_PS();
		IR_W2001_TRACE("eint_work_func: ps = %d\r\n",ps_data);

		if(flag & 0x01)  // Far
		{
		    nf_data = 0;
			//to do something
			app_ui_event_process(UI_EVENT_WEAR,0,UI_EVENT_WEAR_OFF,0,0);
			ir_w2001_data.refresh_flash_data_flag = 0;
			ir_w2001_data.wear_off_flag = 1;
		}
		else  // Near
		{
		    nf_data = 1;
			//to do something
			app_ui_event_process(UI_EVENT_WEAR,0,UI_EVENT_WEAR_ON,0,0);
			ir_w2001_data.refresh_flash_data_flag = 1;
		}
		
		//report_ps_value(ps_data, nf_data);  //Report ps_data and ps event(Far or Near) to system TODO
		IR_W2001_TRACE("report_ps_value: nf_data = %d\r\n",nf_data);
		STK_WriteReg(0x10,flag & (~0x10)); //clear FLG_PSINT ( Bit[4] )

	}
#endif
	
#if ALS_INTERRUPT_MODE
	if(flag_alsint) //IF ALS Interrupt
	{
		uint16_t als_data = 0;
		
		als_data = STK_Read_ALS();
		IR_W2001_TRACE("eint_work_func:als = %d\r\n",als_data);
		stk_als_set_new_thd(als_data);
		//report_als_value(als_data);  //Report als_data to system
		STK_WriteReg(0x10,flag & (~0x20));//clear FLG_ALSINT ( Bit[5] )
	}
#endif	
	//STK_Show_Allreg();	//for debug
	//ir_w2001_int_init();//enable interrupt

}

int ir_w2001_init(void)
{
	twi_hal_if_init();
	ir_w2001_ldo_enable();
	
	ir_w2001_data.refresh_flash_data_flag = 0;
	ir_w2001_data.ps_actually_min = 0xffff;
	ir_w2001_data.ps_actually_max = 0;
	ir_w2001_data.wear_off_flag = 0;
	//zowee_section_set_ir_calib_value(PT_IR_CT,(uint32_t )900);
	//zowee_section_set_ir_calib_value(PT_IR_5MM,(uint32_t )2300);
	//norflash_api_flush_all();//cuixu test
	
	zowee_section_get_ir_calib_value(PT_IR_CT,&ir_w2001_data.ps_data_min);
	zowee_section_get_ir_calib_value(PT_IR_5MM,&ir_w2001_data.ps_data_max);
	
	IR_W2001_TRACE("\n  ir_w2001_int_init %d %d,\n",ir_w2001_data.ps_data_min,ir_w2001_data.ps_data_max);	

	ir_w2001_self_cali_timer = osTimerCreate(osTimer(IR_W2001_SELF_CALI_TIMER), osTimerOnce, NULL);;

#if IR_W2001_TEST
	ir_w2001_test_timer = osTimerCreate(osTimer(IR_W2001_TEST_TIMER), osTimerPeriodic, NULL);;
	osTimerStart(ir_w2001_test_timer, 1000);	//1000ms
#endif
	return 0;
}

void ir_w2001_disable(void)
{
	IR_W2001_TRACE("%d %d\n",ir_w2001_data.refresh_flash_data_flag,ir_w2001_data.wear_off_flag);
	if(ir_w2001_data.refresh_flash_data_flag && !ir_w2001_data.wear_off_flag)
	{
		ir_w2001_need_refresh_flash();
		ir_w2001_data.refresh_flash_data_flag = 0;
	}
	STK_WriteReg(0x00, 0x00);//entry standby mode
	ir_w2001_gpiote_disable();
	osTimerStop(ir_w2001_self_cali_timer);	//
}

void ir_w2001_enable(void)
{
	STK_PowerOn();
	STK_enable_PS();
	ir_w2001_int_init();
	osTimerStart(ir_w2001_self_cali_timer,100);	// 100ms
	ir_w2001_data.read_ps_data_times = 0;
}

void ir_w2001_refresh_data(void)
{
	uint16_t offset_data;
	uint16_t refresh_ct_value;
	uint16_t refresh_mm5_value;

	offset_data = ir_w2001_data.ps_data_max - ir_w2001_data.ps_data_min;
	refresh_ct_value = ir_w2001_data.ps_actually_min;
	if(offset_data == 0)
	{
		refresh_mm5_value = refresh_ct_value + 2000;
	}
	else
	{
		refresh_mm5_value = refresh_ct_value + offset_data;
	}
	
	ir_w2001_data.ps_data_min = refresh_ct_value;
	ir_w2001_data.ps_data_max = refresh_mm5_value;
	
	zowee_section_set_ir_calib_value(PT_IR_CT,(uint32_t )refresh_ct_value);
	zowee_section_set_ir_calib_value(PT_IR_5MM,(uint32_t )refresh_mm5_value);
	norflash_api_flush_all();

}

extern "C" const hal_wear_if_t wear_ctrl =
{
    ir_w2001_init,
    ir_w2001_int_handler,
    ir_w2001_disable,
    ir_w2001_enable,
    ir_w2001_refresh_data,
};


/**********************************************************************

**********************************************************************/

#endif
