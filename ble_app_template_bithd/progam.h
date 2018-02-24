#ifndef __PROGAM_h
#define __PROGAM_h

#define delaytime 100

#define Main_status_closeoled         0 
#define Main_status_changbatlowpower  1 
#define Main_status_timedisplay       2 
#define Main_status_countstep         3 
#define Main_status_messengerQQ       4 
#define Main_status_usbkey            5 
#define Main_status_remind            6
#define Main_status_download          7
#define Main_status_displaylog        8
#define Main_status_blekey            9
#define Main_status_ParingDis            10 


#define bluekeystm32 0
#define timerstm32    1
#define lowpowerstm32 2
#define balancestm32  3
#define broadcastname  4  //tell stm32 work in broadcastname display


#define tureDMWZ            0
#define flaseDMWZ           1

#define Havedisturb   1
#define HaveNodisturb 0

#define YES_chose     1
#define No_chose      2
#define Wait_chose    0

#define stm32uartBUFstar 512

extern unsigned char Main_status;
extern unsigned char KEYwork_flag;
extern unsigned char pstorage_flag;
extern void Bluetooyparingdisplay(void * p_event_data, uint16_t event_size);
#endif

