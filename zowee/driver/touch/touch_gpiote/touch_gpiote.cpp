#include "touch_gpiote.h"

#if __TOUCH_GPIOTE__
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
#include "ui_event_product_test.h"

static void touch_gpiote_timehandler(void const *param);

static touch_gpiote_struct_t touch_gpiote_struct;

osTimerDef(TOUCH_GPIOTE_TIMER, touch_gpiote_timehandler);
static osTimerId touch_gpiote_timer = NULL;


static void touch_gpiote_timehandler(void const *param)
{
	uint8_t touch_gpio_value;

	touch_gpio_value = hal_gpio_pin_get_val((HAL_GPIO_PIN_T )(touch_gpiote_int_cfg.pin));
	touch_gpiote_struct.touch_period++;

	if(touch_gpio_value == 1)
	{//release
		product_test_stauts.touch_status = 0;
		touch_gpiote_struct.release_cnt++;
		if(touch_gpiote_struct.release_cnt > TOUCH_TBLCLICK_THRESH_MS)
		{
			osTimerStop(touch_gpiote_timer);
			touch_gpiote_struct.timer_flag = 0;
			APP_GPIOTE_TRACE("release_cnt %d %d",touch_gpiote_struct.beat_cnt,touch_gpiote_struct.hold_cnt_back);
			if((touch_gpiote_struct.beat_cnt <= 1))
			{
				if(touch_gpiote_struct.hold_cnt >= TOUCH_CLICK_DEBOUNCE_MS || touch_gpiote_struct.hold_cnt_back >= TOUCH_CLICK_DEBOUNCE_MS)
				{ // click
					APP_GPIOTE_TRACE("TOCUH GPIO click");
					app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_CLICK,0,0);
				}
			}
			else if(touch_gpiote_struct.beat_cnt == 2)
			{
				APP_GPIOTE_TRACE("TOCUH GPIO DB click");
				app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_DB_CLICK_PRESS,0,0);
			}
			else if(touch_gpiote_struct.beat_cnt > 2)
			{
				APP_GPIOTE_TRACE("TOCUH GPIO TB click");
				app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_TB_CLICK_PRESS,0,0);
			}
			
			memset(&touch_gpiote_struct,0,sizeof(touch_gpiote_struct));
		}
		else
		{
			if(touch_gpiote_struct.hold_cnt != 0)
			{
				if(touch_gpiote_struct.hold_cnt >= TOUCH_LLPRESS_THRESH_MS)
				{// long long press
					APP_GPIOTE_TRACE("TOCUH GPIO long long press");
					app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_LONG_LONG_PRESS,0,0);
					touch_gpiote_struct.hold_cnt = 0;

				}
				else if(touch_gpiote_struct.hold_cnt >= TOUCH_LPRESS_THRESH_MS)
				{// long press
					APP_GPIOTE_TRACE("TOCUH GPIO long press");
					app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_LONG_PRESS,0,0);
					touch_gpiote_struct.hold_cnt = 0;
				}
				touch_gpiote_struct.hold_cnt_back = touch_gpiote_struct.hold_cnt;
				touch_gpiote_struct.hold_cnt = 0;
				touch_gpiote_struct.beat_cnt++;
			}
		}
		touch_gpiote_struct.status = TOUCH_GPIOTE_RELEASE;
	}
	else
	{//hold
		touch_gpiote_struct.hold_cnt++;
		touch_gpiote_struct.status = TOUCH_GPIOTE_HOLD;
		touch_gpiote_struct.release_cnt = 0;
		product_test_stauts.touch_status = 1;
	}

}

void touch_gpiote_handler(void)
{
	APP_GPIOTE_TRACE("%d",touch_gpiote_struct.timer_flag);
	if(touch_gpiote_struct.timer_flag == 0)
	{
		osTimerStart(touch_gpiote_timer, 5);	//5ms
		touch_gpiote_struct.timer_flag = 1;
		touch_gpiote_struct.touch_period = 0;
		touch_gpiote_struct.beat_cnt = 0;
		touch_gpiote_struct.hold_cnt = 0;
		touch_gpiote_struct.release_cnt = 0;
	}
}

static void touch_gpiote_int_handler_call_back(enum HAL_GPIO_PIN_T pin)
{
	app_ui_event_process(UI_EVENT_GESTURE,0,UI_EVENT_GESTURE_INT,0,0);
}

int32_t touch_gpiote_init(void)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

	memset(&touch_gpiote_struct,0,sizeof(touch_gpiote_struct));
    
 	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&touch_gpiote_int_cfg,1);
	hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )touch_gpiote_int_cfg.pin, HAL_GPIO_DIR_IN, 0);

    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;//HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_handler = touch_gpiote_int_handler_call_back;	
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    
    hal_gpio_setup_irq((HAL_GPIO_PIN_T )touch_gpiote_int_cfg.pin, &gpiocfg);

	if(touch_gpiote_timer == NULL)
	{
		touch_gpiote_timer = osTimerCreate(osTimer(TOUCH_GPIOTE_TIMER), osTimerPeriodic, NULL);
	}
	
	return 0;
}

static void touch_gpio_disable(void)
{
#if 1

    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

	memset(&touch_gpiote_struct,0,sizeof(touch_gpiote_struct));
    
 	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&touch_gpiote_int_cfg,1);
	hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )touch_gpiote_int_cfg.pin, HAL_GPIO_DIR_IN, 0);

    gpiocfg.irq_enable = false;
    gpiocfg.irq_debounce = 0;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;//HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_handler = NULL;	
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    
    hal_gpio_setup_irq((HAL_GPIO_PIN_T )touch_gpiote_int_cfg.pin, &gpiocfg);
#endif
}

static void touch_gpio_enable(void)
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

	memset(&touch_gpiote_struct,0,sizeof(touch_gpiote_struct));
    
 	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&touch_gpiote_int_cfg,1);
	hal_gpio_pin_set_dir((HAL_GPIO_PIN_T )touch_gpiote_int_cfg.pin, HAL_GPIO_DIR_IN, 0);

    gpiocfg.irq_enable = true;
    gpiocfg.irq_debounce = true;
    gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;//HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    gpiocfg.irq_handler = touch_gpiote_int_handler_call_back;	
    gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
    
    hal_gpio_setup_irq((HAL_GPIO_PIN_T )touch_gpiote_int_cfg.pin, &gpiocfg);

	if(touch_gpiote_timer == NULL)
	{
		touch_gpiote_timer = osTimerCreate(osTimer(TOUCH_GPIOTE_TIMER), osTimerPeriodic, NULL);
	}
	hal_gpio_pin_get_val((HAL_GPIO_PIN_T)touch_gpiote_int_cfg.pin);
}

extern "C" const hal_gesture_if_t gesture_ctrl =
{
    touch_gpiote_init,
    touch_gpiote_handler,
    touch_gpio_disable,
    touch_gpio_enable
};


#endif
