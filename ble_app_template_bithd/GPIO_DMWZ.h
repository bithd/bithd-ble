#ifndef _GPIO_DMWZ_H_
#define _GPIO_DMWZ_H_

#define USBconnect    1  
#define USBDisconnect 0 
#define USBnoChanging 0 
#define USBChanged    1 


#define USB_test_PIN        16  
#define KEYpowercontrol_PIN 30  
#define SlectPin 25


extern unsigned char poweronkey_flag;
extern unsigned char USB_connectFLag;
extern unsigned char USB_ChangingFLAG;

void gpio_init(void);
void USBTEST_status(void);
void PowerOn_key(void);    
void PowerOff_key(void);
void firmwaredownload_GPIO_H(void);
void firmwaredownload_GPIO_L(void);
#endif
