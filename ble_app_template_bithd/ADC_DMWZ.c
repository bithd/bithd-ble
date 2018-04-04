#include "include_all.h"

unsigned char FullVoltage_flag=0;   //power is full flag
volatile int32_t adc_sample=0;      //storge batterylevel

/**************************
get states of charging
*************************/
void chargingBat(void)
{
	if(USB_connectFLag==USBconnect)
	{
		if(nrf_gpio_pin_read(CHG_Status_pin)==1)             //judge charing state
		{
			FullVoltage_flag=1;                              //power is full
		}
		else{}
	}
	else
	{
		FullVoltage_flag=0;                                   //clear charging flag
	}	
}

/*
get battery voltage
*/
void ReadAdc(void * p_event_data, uint16_t event_size)
{
 	 adc_sample=nrf_adc_convert_single(NRF_ADC_CONFIG_INPUT_5);  //read ADC
	adc_sample=ADC_RESULT_IN_MILLI_VOLTS(adc_sample);            //calculate voltage mv
	chargingBat();

	if(ParingDataDisFlag==1)
	{
		if(Ack_recive_enable==0)
		{	
			if((Main_status!=Main_status_download)&&(Main_status!=Main_status_blekey))
			{
				if(poweronkey_flag==0)
				{
					firmwaredownload_GPIO_H();                      //enter APP mode
					nrf_gpio_pin_clear(SlectPin);
					PowerOn_key();
				}
				memcpy(&g_apdu[stm32uartBUFstar],ParingDataBuf,sizeof(ParingDataBuf));
				CmdSendUart(BlueParingDatacmd,&g_apdu[stm32uartBUFstar],sizeof(ParingDataBuf));//paring datas display
				
				app_timer_stop(Timeout3Sec_id);    
				Timeout3Sec_StarFlag=TimeClose;   
				app_timer_stop(Timeout1Sec_f_id);   
				Timeout1Sec_f_StarFlag=TimeClose;
				Time_stuts=TimeDataDisPl;
				Main_status=Main_status_ParingDis;
			}
			ParingDataDisFlag=0;
	 	}
	}
}

/*
 * @brief ADC initialization.
 */
void adc_config(void)
{
    const nrf_adc_config_t nrf_adc_config = NRF_ADC_CONFIG_DEFAULT;
    // Initialize and configure ADC
    nrf_adc_configure( (nrf_adc_config_t *)&nrf_adc_config);

}

///////////////////////////////////low power manager/////////////////////

void Chargingandmanagerdisplay(void * p_event_data, uint16_t event_size)
{
	if(USB_connectFLag==USBconnect)
	{
	    if(Timeout1Sec_f_StarFlag==TimeClose)   //timedelay for display
		{
			app_timer_start(Timeout1Sec_f_id, ONE_SECOND_INTERVAL, NULL); 
			Timeout1Sec_f_StarFlag=TimeOpen;
		}
        else
		{
			if(Timeout1Sec_f_StarFlag==TimeOut)
			{
				app_timer_stop(Timeout1Sec_f_id);  
				Timeout1Sec_f_StarFlag=TimeClose;

				g_apdu[stm32uartBUFstar]=timerstm32;   //stm32 entern time display
				CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
        KEYwork_flag=0;
				Main_status=Main_status_timedisplay;   
			}
			else
			{}//1sec no timeout
		}				
	}
	else
	{
		if(USB_ChangingFLAG==USBChanged)           //USB Just remove ?
		{                         			//just remove usb
			KEYwork_flag=0;
			Main_status=Main_status_timedisplay;   
			nrf_gpio_pin_clear(SlectPin);
			firmwaredownload_GPIO_H();             //entern APP mode
			PowerOn_key();
			g_apdu[stm32uartBUFstar]=timerstm32;
		    CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
	  	}
		else
		{
      		if(Timeout1Sec_f_StarFlag==TimeClose)
			{
				app_timer_start(Timeout1Sec_f_id, ADC_NOpower, NULL); 
				Timeout1Sec_f_StarFlag=TimeOpen;
			}
      		else
			{
				if(Timeout1Sec_f_StarFlag==TimeOut)
				{
					app_timer_stop(Timeout1Sec_f_id);    
					Timeout1Sec_f_StarFlag=TimeClose;

					g_apdu[stm32uartBUFstar]=timerstm32;
					CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
          KEYwork_flag=0;
					Main_status=Main_status_timedisplay;
				}
				else
				{
					if(touch_key!=KEY_NO)
					{
						app_timer_stop(Timeout1Sec_f_id);   
						Timeout1Sec_f_StarFlag=TimeClose;

						g_apdu[stm32uartBUFstar]=timerstm32;
						CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
						Main_status=Main_status_timedisplay;   					
					  KEYwork_flag=0;
						touch_key=KEY_NO; 
						KeyWorkflag=1;						
					}
				}
			}			
	  	}
	}
}





















