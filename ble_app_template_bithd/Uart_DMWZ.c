
#include "include_all.h"

unsigned char cmd_uart[5];
uint8_t data_array[1];


uart_communication_dmwz uart_communicate_buf={0,0,0,&g_apdu[4],0};

unsigned char uart_waitack_flag=0; 

volatile unsigned char Keystatus=0;                         
volatile unsigned char uartnum=0;                           
unsigned char uart_enable_flag=0;                 
unsigned char uart_send_flag=0;                    
unsigned char uart_recive_All=0;                   
unsigned char olddata=0;                          
unsigned char ACK_recive_satus=0;                 
unsigned char Ack_recive_enable=0;                 
unsigned char ACC_resend=0;                       

static unsigned short uart_recive_l=0;           

void uart_send_Bty(unsigned char* buf,unsigned short len)
{
	unsigned short i;
	for (i = 0; i < len; i++)
    {
        while(app_uart_put(buf[i]) != NRF_SUCCESS);
    }
}

void uart_event_handle(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
	{
		case  APP_UART_DATA_READY:
		case	APP_UART_DATA:					    
		UNUSED_VARIABLE(app_uart_get(&data_array[0]));

		switch(olddata)
		{
			case uart_0xa5: 
			if(data_array[0]==0x5a){olddata=uart_0x5a;}
			else{olddata=0;}
			break;

			case uart_0x5a: 
			uart_communicate_buf.length=data_array[0];
			uart_communicate_buf.length=uart_communicate_buf.length<<8;
			olddata=uart_length_H;
			break;

			case uart_length_H:
			uart_communicate_buf.length=(uart_communicate_buf.length)|data_array[0];
			olddata=uart_length_L;
			break;

			case uart_length_L:
			uart_communicate_buf.cmd = data_array[0];
			olddata=uart_cmd;

			break;

			case uart_cmd: 
			uart_communicate_buf.no_use = data_array[0];
			uart_recive_l=0;
			olddata=uart_nouseful;
			break;

			case uart_nouseful:
			uart_communicate_buf.data[uart_recive_l]=data_array[0];
			uart_recive_l++;

			if(uart_recive_l<uart_communicate_buf.length-2){}
			else
			{
				uart_recive_All=1;
				olddata=uart_databytes;
				uart_communicate_buf.crc16=uart_communicate_buf.data[uart_recive_l-2];
				uart_communicate_buf.crc16=(uart_communicate_buf.crc16)<<8;
				uart_communicate_buf.crc16=uart_communicate_buf.crc16|uart_communicate_buf.data[uart_recive_l-1];
			}

			break;

			case uart_databytes: 
			break;

			case uart_first_0x5a: 
			if(data_array[0]==0xa5){olddata=uart_secend_0xa5;}
			else{olddata=0;}
			break;

			case uart_secend_0xa5:
			if(data_array[0]==0x00)
			{
				ACK_recive_satus=SuccsACK;
			}
			else{ACK_recive_satus=FailACK;}
			break;

			default:
			if(data_array[0]==0xa5&&olddata==0)
			{olddata=uart_0xa5;}
			if(Ack_recive_enable==1)
			{
				if(data_array[0]==0x5a&&olddata==0){olddata=uart_first_0x5a;}
			}						            
			break;
		}

		break;

		default:
		break;
	}
		
}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        Rx_pin,
        Tx_pin,
        Rts_pin,
        Cts_pin,
        APP_UART_FLOW_CONTROL_ENABLED,//APP_UART_FLOW_CONTROL_ENABLED,//APP_UART_FLOW_CONTROL_DISABLED,//
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);
    APP_ERROR_CHECK(err_code);
	uart_enable_flag=1;
}
/**@snippet [UART Initialization] */

void Uart_close(void)
{
	app_uart_close(0);
	uart_enable_flag=0;
	
	uart_send_flag=0; 
	uartnum=0;        
	uart_recive_All=0;
	ACC_resend=0; 
	olddata=0;         
	ACK_recive_satus=0;
	Ack_recive_enable=0;
	uart_recive_l=0;
	
	app_timer_stop(Timeout1Sec_Uart_id);   
	Timeout1Sec_Uart_StarFlag=TimeClose;
}      

unsigned char uartsendhead[6]={0xa5,0x5a,0x00,0x00,0,0};
void uart_send(void)
{
  	unsigned char i;
	uartsendhead[2]=0x00ff&((uart_communicate_buf.length)>>8);
	uartsendhead[3]=0x00ff&(uart_communicate_buf.length);
	uartsendhead[4]=uart_communicate_buf.cmd;
	uartsendhead[5]=uart_communicate_buf.no_use;


	app_sched_event_put(uartsendhead,6, uart_send_Bty);
    for(i=0;i<((uart_communicate_buf.length)-2)/SCHED_MAX_EVENT_DATA_SIZE;i++)
	{
		app_sched_event_put(&(uart_communicate_buf.data)[SCHED_MAX_EVENT_DATA_SIZE*i],SCHED_MAX_EVENT_DATA_SIZE, uart_send_Bty);
	}
		
	app_sched_event_put(&(uart_communicate_buf.data)[SCHED_MAX_EVENT_DATA_SIZE*i],((uart_communicate_buf.length)-2)%SCHED_MAX_EVENT_DATA_SIZE, uart_send_Bty);
		
}

unsigned char CRC16_check(void)
{
	unsigned short crc16=0;
	unsigned char data[4];
	
	data[0]=0x00ff&((uart_communicate_buf.length)>>8);
	data[1]=0x00ff&((uart_communicate_buf.length));	
	data[2]=uart_communicate_buf.cmd;
	data[3]=uart_communicate_buf.no_use;
	
    crc16=bd_crc16(0,data,4);             
	crc16=bd_crc16(crc16,uart_communicate_buf.data,uart_communicate_buf.length-4); 
	if(crc16==uart_communicate_buf.crc16)	{return 0;}
    else{}
    return 1;
	
}

unsigned short CRC16_Uart_send(void)
{
    unsigned short crc16=0;
	unsigned char data[4];
	
	data[0]=0x00ff&((uart_communicate_buf.length)>>8);
	data[1]=0x00ff&((uart_communicate_buf.length));	
	data[2]=uart_communicate_buf.cmd;
	data[3]=uart_communicate_buf.no_use;
	
    crc16=bd_crc16(0,data,4);            
	crc16=bd_crc16(crc16,uart_communicate_buf.data,uart_communicate_buf.length-4); 	
	
	return crc16;
}

void CmdSendUart(unsigned char cmd_uart,unsigned char* apdubuf,unsigned short apdulength)
{
	uart_communicate_buf.length=1+1+apdulength+2;
	uart_communicate_buf.cmd=cmd_uart;              
	uartnum++;
	uart_communicate_buf.no_use=uartnum;        
	uart_communicate_buf.data=apdubuf;          
	
	uart_communicate_buf.crc16=CRC16_Uart_send();

	uart_communicate_buf.data[apdulength]=0x00ff&(uart_communicate_buf.crc16>>8);
	uart_communicate_buf.data[apdulength+1]=0x00ff&(uart_communicate_buf.crc16);

	uart_send_flag=1;		
}

void Uart_cmdprogam(void)
{
	switch(uart_communicate_buf.cmd)
	{
		case 0x01:
				communicationBluetooth.data[1]=0x00ff&((uart_communicate_buf.length-4)>>8);
				communicationBluetooth.data[2]=0x00ff&(uart_communicate_buf.length-4);
				memcpy(&communicationBluetooth.data[3],uart_communicate_buf.data,(uart_communicate_buf.length-4));
				Send_bluetoothdata(uart_communicate_buf.length-1); 	
				uart_waitack_flag=1;
		break;		
	}
}

void successAck(void)
{
	unsigned char ack_succs[3]={0x5a,0xa5,0x00};
    uart_send_Bty(ack_succs,3);
}

extern unsigned char SumPackNumber;
unsigned short CRC_old;
void UartDataSendrecive(void * p_event_data, uint16_t event_size)
{
	unsigned char ack_erro[3]={0x5a,0xa5,0x01};
	unsigned char ack_succs[3]={0x5a,0xa5,0x00};

	if(uart_enable_flag==1)
	{		
	  	if(uart_send_flag==1)
		{
			if(Main_status==Main_status_blekey||Main_status==Main_status_download)
			{
				app_timer_stop(Timeout3Sec_id);  
				Timeout3Sec_StarFlag=TimeClose;
			}
			uart_send();      
			uart_send_flag=0; 
			CRC_old=0;
			Ack_recive_enable=1;
		}

		if(uart_waitack_flag==2)
		{
			uart_send_Bty(ack_succs,3);
			uart_waitack_flag=0;
		}

		if(uart_recive_All==1)
		{
			app_timer_stop(Timeout1Sec_Uart_id);    
			Timeout1Sec_Uart_StarFlag=TimeClose;
				
			ACC_resend=0;     

			if(CRC16_check()==0)
			{
				if(uart_communicate_buf.no_use==uartnum)
				{
					if(uart_communicate_buf.crc16!=CRC_old)
					{
						CRC_old=uart_communicate_buf.crc16;
						Ack_recive_enable=0;
						Uart_cmdprogam();
								
					}					
				}
				else
				{
					Ack_recive_enable=2;
					uart_send_Bty(ack_erro,3);	
				}
			}
			else
			{
				Ack_recive_enable=2;
				uart_send_Bty(ack_erro,3);
			}			
			ACK_recive_satus=0;
			uart_recive_All=0;
			olddata=0;				
		}

		if(Ack_recive_enable==1)
		{
			if(ACK_recive_satus!=NOACK)
			{
				
				app_timer_stop(Timeout1Sec_Uart_id);   
				Timeout1Sec_Uart_StarFlag=TimeClose;
				
				if(ACK_recive_satus==FailACK)
				{
					ACC_resend++;
					if(ACC_resend<MAX_resend)
					{
						uart_send_flag=1;
					}
					else
					{
						Ack_recive_enable=0;
						ACC_resend=0;       
					}
				}
				else
				{
						Ack_recive_enable=0;
						ACC_resend=0;      
						if((Main_status==Main_status_blekey)||(Main_status_download==Main_status))
						{//Recive Success ACK,Bluetooth ACK empty data.
							Send_bluetoothdata(1);
							uart_waitack_flag=1;
						}
				}
				
				
				ACK_recive_satus=0;
				uart_recive_All=0;
				olddata=0;			
			}
			else
			{ 
				if(Timeout1Sec_Uart_StarFlag==TimeClose)
				{
				app_timer_start(Timeout1Sec_Uart_id, ONE_SECOND_INTERVAL, NULL);
					Timeout1Sec_Uart_StarFlag=TimeOpen;
				}
				else
				{	
					if(Timeout1Sec_Uart_StarFlag==TimeOut)
					{
						app_timer_stop(Timeout1Sec_Uart_id);  
						Timeout1Sec_Uart_StarFlag=TimeClose;
		
						ACC_resend++;
						if(ACC_resend<MAX_resend)
						{
							uart_send_flag=1;
						}
						else
						{
						Ack_recive_enable=0;
							ACC_resend=0;     
						}
					}
					else
					{}
				}
			}
		}

		if(Ack_recive_enable==2)
		{
			
			if(Timeout1Sec_Uart_StarFlag==TimeClose)
			{
				app_timer_start(Timeout1Sec_Uart_id, ONE_SECOND_INTERVAL, NULL);
				Timeout1Sec_Uart_StarFlag=TimeOpen;
			}
			else
			{	
				if(Timeout1Sec_Uart_StarFlag==TimeOut)
				{	
					app_timer_stop(Timeout1Sec_Uart_id);   
					Timeout1Sec_Uart_StarFlag=TimeClose;
					
					ACC_resend++;
					if(ACC_resend<MAX_resend)
					{
						uart_send_Bty(ack_erro,3);
					}
					else
					{
						Ack_recive_enable=0;
						ACC_resend=0;      
					}
				}
				else
				{
					if(uart_recive_All==1)
					{
						app_timer_stop(Timeout1Sec_Uart_id);   
						Timeout1Sec_Uart_StarFlag=TimeClose;
						
						if(CRC16_check()==0)
						{
							if(uart_communicate_buf.no_use==uartnum)
							{
								ACK_recive_satus=0;
								ACC_resend=0;
								Ack_recive_enable=0;
								uart_send_Bty(ack_succs,3);
								Uart_cmdprogam();
							}
							else
							{
								ACC_resend++;
								if(ACC_resend<MAX_resend)
								{
									uart_send_Bty(ack_erro,3);
								}
								else
								{
									Ack_recive_enable=0;
									ACC_resend=0;     
									ACK_recive_satus=0;
								}										
							}
						}
						else
						{
							ACC_resend++;
							if(ACC_resend<MAX_resend)
							{
							uart_send_Bty(ack_erro,3);
							}
							else
							{
								Ack_recive_enable=0;
								ACC_resend=0;      
								ACK_recive_satus=0;
							}								
						}
						
						olddata=0;
						uart_recive_All=0;
					}
				}
			}		 
		}
	}		
}

















