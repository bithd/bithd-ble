#include "include_all.h"

unsigned char USB_connectFLag=USBDisconnect;
unsigned char USB_ChangingFLAG=USBnoChanging;
unsigned char poweronkey_flag=0;

void gpio_init()
{
	nrf_gpio_cfg_input(CAPSENCE_INT_PIN,NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(6,NRF_GPIO_PIN_PULLUP);

  nrf_gpio_cfg_output(KEYpowercontrol_PIN);
	nrf_gpio_cfg_output(SlectPin);
  PowerOff_key();                        

	nrf_gpio_cfg_input(CHG_Status_pin,GPIO_PIN_CNF_PULL_Disabled); 
 
}

//void USBTEST_status(void)
//{
//	if(1)
//	{
//		if(USB_connectFLag!=USBconnect)
//		{
//			USB_ChangingFLAG=USBChanged;
//		}
//		else
//		{
//			USB_ChangingFLAG=USBnoChanging;
//		}
//    USB_connectFLag=USBconnect;
//   }
//	else
//	{
//		if(USB_connectFLag!=USBDisconnect)
//		{
//			USB_ChangingFLAG=USBChanged;
//		}
//		else
//		{
//			USB_ChangingFLAG=USBnoChanging;
//		}	

//    USB_connectFLag=USBDisconnect;

//  }
//}

void firmwaredownload_GPIO_H(void)
{
	nrf_gpio_cfg_output(29);  
	nrf_gpio_pin_set(29);      
}

void firmwaredownload_GPIO_L(void)
{
	nrf_gpio_cfg_output(29);   
	nrf_gpio_pin_clear(29);      
}

void PowerOn_key(void)
{
	nrf_gpio_pin_set(KEYpowercontrol_PIN);
  nrf_delay_ms(delaytime);
	if(uart_enable_flag==0)
	{
		uart_init();
	}	
	nrf_delay_ms(delaytime);
	poweronkey_flag=1;
}
extern unsigned short CRC_old;
void PowerOff_key(void)
{
	poweronkey_flag=0;
	nrf_gpio_pin_clear(KEYpowercontrol_PIN);
	nrf_gpio_pin_clear(SlectPin);
	if(uart_enable_flag==1)
	{
		Uart_close();
	}
	CRC_old=0;
}





