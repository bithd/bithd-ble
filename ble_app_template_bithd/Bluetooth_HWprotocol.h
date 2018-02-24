#ifndef _BLUETOOTHHWPROTOCOL_H_
#define _BLUETOOTHHWPROTOCOL_H_

#define bluetoothNOdata     0 
#define bluetoothRecivedata 1 
#define bluetoothSenddata   2

#define PackRecive 20                          
#define WaitRertyTims 3              
#define TimeOver      1              

#define DATAFIELD_MAX_LEN		1024
#define APDU_HEAD_LEN			8
#define APDU_MAX_LEN			(APDU_HEAD_LEN+DATAFIELD_MAX_LEN+8)


typedef unsigned char * pu8;
typedef unsigned char	uint8;
typedef struct TimerProgram		
{
	u32 T_0;   
	u32 T_1;   
} TimProg;

//hardware device 
extern u32 volatile timer0_sec_count;
extern unsigned char DataP[PackRecive];       
extern unsigned char Len_DATA;                
extern unsigned char Recivedataflag;           

//app
extern u8 g_apdu[APDU_MAX_LEN];
extern u16 g_apdu_length;
extern unsigned char blueRecivSendflag;
extern void Bluetooth_ReciveANDSend(void * p_event_data, uint16_t event_size);
#endif
