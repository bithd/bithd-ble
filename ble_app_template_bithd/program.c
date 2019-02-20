
#include "include_all.h"

unsigned char Main_status=Main_status_closeoled;   
unsigned char KEYwork_flag=0;                  
unsigned char pstorage_flag=0;                    
unsigned char switch_ble_flag=BLE_ON_STA;
unsigned int  close_ble_count = 0;
unsigned char system_status=ON_STATUS;

void Bluetooyparingdisplay(void * p_event_data, uint16_t event_size)
{
	if(Ble_ConnnectFlag==BLE_connect)
	{
		if(Timeout3Sec_StarFlag!=TimeClose)
		{
			if(Timeout3Sec_StarFlag==TimeOut)
			{
				app_timer_stop(Timeout3Sec_id); 
				Timeout3Sec_StarFlag=TimeClose;
				blueDisconnect();

				Main_status=Main_status_closeoled;
				PowerOff_key();
			}
			else{}
		}
		else
		{
			app_timer_start(Timeout3Sec_id,T_blue_TimeOut, NULL);
			Timeout3Sec_StarFlag=TimeOpen;                         
		}

		if(Ble_AuthFlag==1)
		{
			if(Ack_recive_enable==0)
			{
				app_timer_stop(Timeout3Sec_id);         
				Timeout3Sec_StarFlag=TimeClose;


				PowerOff_key();
				nrf_delay_ms(delaytime);
				firmwaredownload_GPIO_H();
				nrf_gpio_pin_clear(SlectPin);

				PowerOn_key();

				g_apdu[stm32uartBUFstar]=timerstm32;

				CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
				Main_status=Main_status_timedisplay;
        KEYwork_flag=0;
				Ble_AuthFlag=BLE_AuthFail;
			}
		}
	}
	else
	{
		if(Ack_recive_enable==0)
		{
			app_timer_stop(Timeout3Sec_id);
			Timeout3Sec_StarFlag=TimeClose;

			PowerOff_key();
			nrf_delay_ms(delaytime);
			firmwaredownload_GPIO_H();

			nrf_gpio_pin_clear(SlectPin);

			PowerOn_key();
			g_apdu[stm32uartBUFstar]=timerstm32;
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			Main_status=Main_status_timedisplay;

			KEYwork_flag=0;
		}
	}
	touch_key=KEY_NO;                 
	KeyWorkflag=1;                        
}


void firmwaresigned(void * p_event_data, uint16_t event_size)
{
	if(Ble_ConnnectFlag==BLE_connect)
	{
		if(Timeout3Sec_StarFlag!=TimeClose)
		{
			if(Timeout3Sec_StarFlag==TimeOut)
			{
				app_timer_stop(Timeout3Sec_id); 
				Timeout3Sec_StarFlag=TimeClose;
        KEYwork_flag=0;
				Main_status=Main_status_closeoled;
				PowerOff_key();
			}
			else{}
		}
		else
		{
			app_timer_start(Timeout3Sec_id,T_10SECOND_TimeOut, NULL);
			Timeout3Sec_StarFlag=TimeOpen;                         
		}
	}
	else
	{
		if(Ack_recive_enable==0)
		{
			app_timer_stop(Timeout3Sec_id);
			Timeout3Sec_StarFlag=TimeClose;
			PowerOff_key();
      KEYwork_flag=0;
			Main_status=Main_status_closeoled;
		}
	}
	touch_key=KEY_NO;                 
	KeyWorkflag=1;                        
}









