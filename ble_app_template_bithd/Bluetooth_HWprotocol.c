/******************communication protocol***************/

//Instructions for use
//1.Bluetooth_ReciveANDSend work in main.realize the data recive and send
//2.when flag blueRecivSendflag=1   you can recive data from g_apdu and g_apdu_length
//3.when you need to send data, yo can put the data in  g_apdu and g_apdu_length,then let blueRecivSendflag=2
//
#include "include_all.h"

//////////////////////software timer///////////////////////////////////
#define HWtimerFreq 5                
u32 volatile timer0_sec_count= 0;    

unsigned char DataP[PackRecive];   
unsigned char Len_DATA=0;                
unsigned char Recivedataflag=0;        
u8 g_apdu[APDU_MAX_LEN];
u16 g_apdu_length;
unsigned char blueRecivSendflag=bluetoothNOdata;

TimProg T_Connect;                   
u8 ucReTry;                           
unsigned char SumPackNumber;        
u8 ucaPackMsg[20]; 
u8 ucaRevPackN[15];
u8 ucExpPackN;
u8 ucTotalPack;
u8 ucResend;
u16 lr;                               
u8 Apdufinsh=0;                      
unsigned short Sendlenth=0;          


void DELAY_10MS(void)
{
	nrf_delay_ms(10);
}

bool sram_memcmp(void * dst, void * src, u16 len)
{
	u16 i;
	bool ret = 0;

	for(i = 0; i < len; i++)
	{
		if(*((pu8)dst + i) != *((pu8)src + i))
		{
			ret = 1;
		}
	}
	
	return ret;
}

void sram_memset(void * dst, u8 val, u16 len)
{
	u16 i;

	for(i = 0; i < len; i++)
	{
		*((pu8)dst + i) = val;
	}
}

void sram_memcpy(void * dst, void * src, u16 len)
{
	u16 i;
	
	if (dst == src || NULL == dst || NULL == src || 0 == len)
	{
        return;
	}
	
    if (src < dst)
    {
        for(i = len-1; i != 0; i--)
        {
            *((pu8)dst+i) = *((pu8)src+i);
        }
		
        *(pu8)dst = *(pu8)src;        
    }	
	else
	{
		for(i = 0; i < len; i++)
    	{
        	*((pu8)dst+i) = *((pu8)src+i);
    	}
	}
}

void sram_reverse(void * dst, u16 len)
{
    u8 tmp_data;
    pu8 tmp_pointer;
    u16 i;
	u16 data_len = len>>1;
	
    for (i = 0; i < data_len; i++)
    {
        tmp_pointer = (pu8)dst+len-i-1;
        tmp_data = *tmp_pointer;
        *tmp_pointer = *((pu8)dst + i);
        *((pu8)dst + i) = tmp_data;
    }
}

void MarkPackNum(uint8 curPackN, uint8 totalNum, uint8 *RevPackN, uint8 *MissedPackN)
{
    uint8 i = 0, n, m, tmp, len;   
    n = totalNum/8;
    m = totalNum%8;
    if(m != 0) 
    {
        n += 1; 
    }
    len = n+1;
    RevPackN[curPackN/8] |= 0x80 >> (curPackN%8);  
    
    for(i=0;i<n;i++)
    {
        MissedPackN[2+i] = ~RevPackN[i];
    }
    
    if(m != 0)
    {
        tmp = MissedPackN[2+n-1] >> (8-m);
        MissedPackN[2+n-1] = tmp << (8-m);
    }

    for(i=2+n-1;i>1;i--)
    {
        if(MissedPackN[i] == 0x00)
        {
            len--;
        }
        else
        {
            break;
        }
    }
    MissedPackN[0] = len;
    MissedPackN[1] = (len==1 ? 0x00 : 0x01);
}


uint8 CheckExist(uint8 PackN, uint8 *Msg)
{
    uint8 tmp;
    tmp = Msg[PackN/8]<<(PackN%8);
    if(tmp&0x80)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void ClrMissedPack(uint8 PackN, uint8 *Msg)
{    
    uint8 i = 0,len;
    len = Msg[0];
    Msg[2+PackN/8] &= ~(0x80 >> (PackN%8));  
    for(i=Msg[0];i>1;i--)
    {
        if(Msg[i] == 0x00)
        {
            len--;
        }
        else
        {
            break;
        }
    }
    Msg[0] = len;
    Msg[1] = (len==1 ? 0x00 : 0x01);
}


uint8 NextExpPack(uint8 curPackN, const uint8 *Msg)
{
    uint8 tmp;

        tmp = Msg[2+curPackN/8]<<(curPackN%8);
        if(tmp&0x80)
        {
            return (curPackN+1);
        }

    return 0;
}


void TP_START(TimProg* T,u32 Delaytime) 
{
	T->T_0=timer0_sec_count;
	T->T_1=timer0_sec_count+(Delaytime*HWtimerFreq);  
}


void TP_CLOSE(TimProg* T)
{
	T->T_0=0;
	T->T_1=0;
}


unsigned char CheckTP(TimProg* T)
{
    if((timer0_sec_count)<(T->T_1))
    	{
          return 1;
    	}
	return 0;
}

void ble_transfer_init(void)
{
	sram_memset(ucaRevPackN, 0, 15);
	sram_memset(ucaPackMsg, 0, 20);

	ucExpPackN = 0;
	ucReTry = 0;
	ucTotalPack = 0;
	ucResend = 0;
}


void tell_phone_missed_packets(void)
{
	
	TP_START(&T_Connect,TimeOver);
	ucaPackMsg[0] = 0xA5;
	ucaPackMsg[1] = 0xA5;
	ble_txrx_Send(ucaPackMsg, ucaPackMsg[2]+3);

}

void phone_to_ble_data_parse(u8 * buf, u16 buf_len)
{
	
	TP_CLOSE(&T_Connect);
	ucReTry = 0;
	if((buf[0] == 0xA5) && (buf[1] == 0xA5))
	{   if(SumPackNumber!=0) 
		  {
			sram_memcpy(ucaPackMsg, buf, buf_len); 

			if(0x01 == ucaPackMsg[3])
			{
				ucResend = 1;
			}
			else
			{  
				sram_memset(ucaPackMsg, 0, 20);
	      		sram_memset(ucaRevPackN, 0, 15);
				SumPackNumber=0;
				Sendlenth=0;
				lr = 0;
				if(uart_waitack_flag==1)
				{
					uart_waitack_flag=2;
				}
			}

		}

	}
	else
	{
        
        if(SumPackNumber!=0) 
        	{
				return;
        	}
		if(buf[0]!=0&&buf_len<3)
			{
              return;
			}
		if(buf[0]==0&&buf_len<4)
			{
              return;
			}

		TP_START(&T_Connect,TimeOver);
		ucTotalPack = buf[1]; 
		
		if(CheckExist(buf[0],ucaRevPackN))
        {    
            return;
        }

		sram_memcpy(g_apdu+buf[0]*18, buf+2, buf_len-2);
		MarkPackNum(buf[0],buf[1],ucaRevPackN, ucaPackMsg+2);  
		ucExpPackN = NextExpPack(buf[0], ucaPackMsg+2);

		if(Apdufinsh==0)
			{
			 Apdufinsh=2; 
			 g_apdu_length=buf_len-2;
			}
		else
			{
			  g_apdu_length += buf_len-2;
			}

		if((buf[0] == (buf[1]-1)) || (ucaPackMsg[3] == 0))
		{
			TP_CLOSE(&T_Connect);
			if(ucaPackMsg[3] == 0)
			{
                Apdufinsh=1;
			}
			else
			{
			    tell_phone_missed_packets();
			    ucReTry = 1;				
			}
		}
	}
}


void ble_to_phone_msg(void)
{
   unsigned char buf[PackRecive];

    if((lr<17)&&(lr>0))	
	{
		SumPackNumber=1;
		buf[0]=0;
		buf[1]=SumPackNumber;
		buf[2]=0;
		buf[3]=lr;
		sram_memcpy(&buf[4],g_apdu,lr);
		ble_txrx_Send(buf,lr+4);

		Sendlenth=lr;
		lr = 0;

		TP_START(&T_Connect,TimeOver);//wait ack

	}
	if(lr>16)
	{
		SumPackNumber=(lr-(PackRecive-4))/(PackRecive-2)+1;
		if(lr>((PackRecive-4)+(SumPackNumber-1)*18))
		{
			SumPackNumber=SumPackNumber+1;
		}

		if(Sendlenth==0)
		{
					
			buf[0]=0;
			buf[1]=SumPackNumber;
			buf[2]=(lr>>8)&0x00ff;
			buf[3]=lr&0x00ff;
			sram_memcpy(&buf[4],g_apdu,PackRecive-4);
			ble_txrx_Send(buf,PackRecive);

			Sendlenth=PackRecive-4;
		}
		else
		{
			buf[0]=(Sendlenth-(PackRecive-4))/(PackRecive-2)+1;
			buf[1]=SumPackNumber;
			if((SumPackNumber-buf[0])!=1)
				{
					sram_memcpy(&buf[2],&g_apdu[Sendlenth],PackRecive-2);
					ble_txrx_Send(buf,PackRecive);
					Sendlenth=Sendlenth+PackRecive-2;	
				}
			else
				{
					sram_memcpy(&buf[2],&g_apdu[Sendlenth],(lr-(PackRecive-4)-(Sendlenth-(PackRecive-4))));
					ble_txrx_Send(buf,(lr-(PackRecive-4)-(Sendlenth-(PackRecive-4)))+2);
					Sendlenth=lr;
					lr = 0;
					TP_START(&T_Connect,TimeOver);
				}

		}
		DELAY_10MS();
		DELAY_10MS();

	}



}

unsigned char SendPackNumber(unsigned char Number)
{
	unsigned char buf[PackRecive];
	buf[0]=Number;
	buf[1]=SumPackNumber;
	if(Number==0)
	{
		buf[2]=(Sendlenth>>8)&0x00ff;
		buf[3]=Sendlenth&0x00ff;
		sram_memcpy(&buf[4],g_apdu,PackRecive-4);
		ble_txrx_Send(buf,PackRecive);
		return 0;
	}
	else
	{
		if((SumPackNumber-buf[0])!=1)
		{
			sram_memcpy(&buf[2],&g_apdu[(Number-1)*(PackRecive-2)+(PackRecive-4)],PackRecive-2);
			ble_txrx_Send(buf,PackRecive);
			return 0;
		}
		else
		{
			sram_memcpy(&buf[2],&g_apdu[(Number-1)*(PackRecive-2)+(PackRecive-4)],(Sendlenth-((Number-1)*(PackRecive-2)+(PackRecive-4))));
			ble_txrx_Send(buf,(Sendlenth-((Number-1)*(PackRecive-2)+(PackRecive-4)))+2);
			return 0;
		}
	}
}

u8 remberPackn=0; 
void ReSendNotification(u8 *PackMsg)
{
    u8 ucExpPackN_ = 0;
	u8 flag=0;

	if(remberPackn<(PackMsg[0]-1)*8)
	{
		ucExpPackN_ = NextExpPack(remberPackn,PackMsg);
	}
	
	
    if(ucExpPackN_!=0)
	{
		if(SendPackNumber(ucExpPackN_-1)==0)
		{}
		else
		{
			flag=1;
		}
		DELAY_10MS();
		DELAY_10MS();
	}

		
    if((ucExpPackN_==SumPackNumber&&flag==0)||(((PackMsg[0]-1)*8)==remberPackn))
	{
		ucResend=0;     
		remberPackn=0;
		TP_START(&T_Connect,TimeOver);
	}
	else
	{
		if(flag==0)
		{remberPackn++;}
	}
}

void Bluetooth_ReciveANDSend(void * p_event_data, uint16_t event_size)
{
	if(Ble_ConnnectFlag==BLE_Disconnect)
	{
		Apdufinsh=0;
		g_apdu_length = 0;
		lr = 0;
		SumPackNumber=0;  
		Sendlenth=0;
		ble_transfer_init();
		blueRecivSendflag=bluetoothNOdata;
		return;
	}
		
	
	if(blueRecivSendflag==bluetoothSenddata)
	{
		lr=g_apdu_length;
		blueRecivSendflag=bluetoothNOdata;
		if(Main_status==Main_status_blekey||Main_status==Main_status_download) 
		{
			app_timer_stop(Timeout3Sec_id);        
			Timeout3Sec_StarFlag=TimeClose;				
		}

	}

	if(Recivedataflag==1)
	{
		Recivedataflag=0;                         
		Len_DATA=0;                              
		if(Apdufinsh==1)                          
		{
			/*********APDUָ****************/
			g_apdu_length=g_apdu_length-2;
			sram_memcpy(g_apdu,&g_apdu[2],g_apdu_length);
			blueRecivSendflag=bluetoothRecivedata;
			ble_transfer_init();
			Apdufinsh=0;
		}
	}		

	ble_to_phone_msg();   

	if(ucResend==1)
	{
		ReSendNotification(&ucaPackMsg[2]);
	}

	if(!CheckTP(&T_Connect))
	{
		
		if(Apdufinsh==2)
		{
			if(ucReTry<(WaitRertyTims))	
			{
				tell_phone_missed_packets();
				ucReTry++;
			}
			else
			{
				TP_CLOSE(&T_Connect);
				ble_transfer_init();
				Apdufinsh=0;
				g_apdu_length = 0;
				lr = 0;
			}
		}
		if(SumPackNumber!=0)
		{
			if(ucReTry<(WaitRertyTims))	
			{
				TP_START(&T_Connect,TimeOver);
				ucReTry++;
			}
				else
			{
				TP_CLOSE(&T_Connect);
				ble_transfer_init();
				SumPackNumber=0;
				Sendlenth=0;
				lr = 0;
			}
		}
	}
}
