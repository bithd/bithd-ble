
#include "include_all.h"

void progam_closeoled(void * p_event_data, uint16_t event_size)
{

	if(USB_ChangingFLAG==USBChanged)
	{
		firmwaredownload_GPIO_H();
		nrf_gpio_pin_clear(SlectPin);
		PowerOn_key();
		
		if(USB_connectFLag!=USBDisconnect)   
		{
			  KEYwork_flag=0;
     		Main_status=Main_status_timedisplay;
		 	g_apdu[stm32uartBUFstar]=timerstm32;
     		CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
	  	}
		else
		{
			  KEYwork_flag=0;
			Main_status=Main_status_timedisplay;  
			g_apdu[stm32uartBUFstar]=timerstm32;
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
	  	}
	}
	else
	{
		if(touch_key!=KEY_NO)
		{
			 firmwaredownload_GPIO_H();
			 nrf_gpio_pin_clear(SlectPin);
			 PowerOn_key();

			if((adc_sample<Warning_Voltage)&&(USB_connectFLag==USBDisconnect))
			{
				Main_status=Main_status_changbatlowpower;
				g_apdu[stm32uartBUFstar]=lowpowerstm32;
				CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			}
			else
			{
				  KEYwork_flag=0;
				Main_status=Main_status_timedisplay; 
				g_apdu[stm32uartBUFstar]=timerstm32;
				CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			}
			touch_key=KEY_NO; 
			KeyWorkflag=1;
		}
		else
		{}	
	}
}
