#include "include_all.h"

unsigned char touch_key      =KEY_NO;                 
unsigned char touch_key_flag =0;                      
unsigned char KeyWorkflag=0;                          
static unsigned char KeepTouch_timer_flag=0;         


void Read_CapKey(void * p_event_data, uint16_t event_size)
{
	if((nrf_gpio_pin_read(CAPSENCE_INT_PIN)==0)||((nrf_gpio_pin_read(6)==0)))  
	{
		if(KeyWorkflag==0)
		{
			touch_key_flag=1;      

			if(KeepTouch_timer_flag!=1) 
			{                  
				KeepTouch_timer_flag=1; 
				Touch_timercount=0;
				app_timer_start(KeepTouch_timer_id, Touch_TIMER_TICKS, NULL);
			}
			else      
			{
				if(Touch_timercount<KeepTouchTime)
				{}
				else
				{
					app_timer_stop(KeepTouch_timer_id);
					KeepTouch_timer_flag=0;
					touch_key=KEY_LongTouch;
					touch_key_flag=0x00;  
				}
			}
		}         
	}
	else
	{
		if((Main_status_download==Main_status)||(Main_status_blekey==Main_status))
		{}
		else
		{
			if(touch_key_flag==1)
			{
				if(0==KeyWorkflag)
				{
					touch_key=KEY_1;						
				}
			}
			else
			{}				
		}
		app_timer_stop(KeepTouch_timer_id);
		KeepTouch_timer_flag=0;

		touch_key_flag=0x00;
		KeyWorkflag=0;
	}
}
