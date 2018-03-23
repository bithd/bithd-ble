#include "include_all.h"
unsigned char *APDU_blekey;
unsigned short Apdu_len_blekey=0;

void BlueUPfirmware(void * p_event_data, uint16_t event_size)
{
	if(Ble_ConnnectFlag==BLE_connect)
	{
		if(Main_status!=Main_status_download)
		{
			app_timer_stop(Timeout3Sec_id);   
			Timeout3Sec_StarFlag=TimeClose;
		}
		else
		{
		
			if(Timeout3Sec_StarFlag!=TimeClose)
			{}
			else
			{
				app_timer_start(Timeout3Sec_id,T_update_TimeOut, NULL);
				Timeout3Sec_StarFlag=TimeOpen;                         
			}

			 
			 if(Apdu_len_blekey!=0)
			{
				CmdSendUart(Apducmd_uart,APDU_blekey,Apdu_len_blekey);
				Apdu_len_blekey=0;
			}
			 
			if(touch_key==KEY_LongTouch)
			{
				app_timer_stop(Timeout3Sec_id);       
				Timeout3Sec_StarFlag=TimeClose;
				KEYwork_flag=0;
				Main_status=Main_status_closeoled;
				PowerOff_key();
				touch_key=KEY_NO;
				KeyWorkflag=1;
			}
			 else
			 {
				 if(Timeout3Sec_StarFlag==TimeOut)
					 {
						app_timer_stop(Timeout3Sec_id);         
						Timeout3Sec_StarFlag=TimeClose;
						KEYwork_flag=0;
						 Main_status=Main_status_closeoled;
				 		PowerOff_key();
					 }
					 else
					 {}			 
			 }
		}	 
	} 
	else
	{
		app_timer_stop(Timeout3Sec_id);      
		Timeout3Sec_StarFlag=TimeClose;
		g_apdu[stm32uartBUFstar]=timerstm32;
		CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
		Main_status=Main_status_timedisplay;     
	}
	
}


void Bluetooyhkeydisplay(void * p_event_data, uint16_t event_size)
{	
	if(Ble_ConnnectFlag==BLE_connect)
	{
		if(Main_status!=Main_status_blekey)
		{
			app_timer_stop(Timeout3Sec_id);       
			Timeout3Sec_StarFlag=TimeClose;
		}
		else
		{
		
		if(Timeout3Sec_StarFlag!=TimeClose)
			{}
			else
			{
				app_timer_start(Timeout3Sec_id,T_blue_TimeOut,NULL);
				Timeout3Sec_StarFlag=TimeOpen;  
			}
			if(Apdu_len_blekey!=0)
			{
				CmdSendUart(Apducmd_uart,APDU_blekey,Apdu_len_blekey);
			    Apdu_len_blekey=0;
			}
			 
			 if(touch_key==KEY_LongTouch)
			 {
				app_timer_stop(Timeout3Sec_id);   
				Timeout3Sec_StarFlag=TimeClose;
				KEYwork_flag=0;
				Main_status=Main_status_closeoled;
				PowerOff_key();
				touch_key=KEY_NO;
				KeyWorkflag=1;
			 }
			 else
			 {
				 if(Timeout3Sec_StarFlag==TimeOut)
					 {
						app_timer_stop(Timeout3Sec_id);    
						Timeout3Sec_StarFlag=TimeClose;
						KEYwork_flag=0;
						 Main_status=Main_status_closeoled;
				 		PowerOff_key();
					 }
					 else
					 {}			 
			 }
		}	 
	} 
	else
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
