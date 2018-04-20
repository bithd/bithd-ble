#include "include_all.h"

BluetoothData communicationBluetooth={&g_apdu[0],&g_apdu[1],&g_apdu[3],&g_apdu[4]};//save recive data pointer
unsigned char CMD09_oldlabel=0;      
unsigned char CMD09_SW[2]={0,0};       


void Recive_bluetoothdata_point(void)
{
	communicationBluetooth.crc16=&g_apdu[g_apdu_length-2];
	blueRecivSendflag=bluetoothNOdata;
}

/*************************************
len the length of data is KEY+value
**************************************/
void Send_bluetoothdata(unsigned short len)
{
	unsigned short l=len;
	unsigned short crc16;
	//Organize the data according to the protocol
	l=l+3;
	communicationBluetooth.length[0]=l>>8;
	communicationBluetooth.length[1]=0x00ff&l;

	g_apdu_length=l+3;
	
	communicationBluetooth.crc16=&g_apdu[g_apdu_length-2];  //according send data updata crc data
    crc16=bd_crc16(0,g_apdu,g_apdu_length-2);              
	communicationBluetooth.crc16[0]=crc16>>8;
	communicationBluetooth.crc16[1]=crc16&0x00ff;

	blueRecivSendflag=bluetoothSenddata;                    //send enable	
}

/****************************
check crc success or fail
*****************************/
unsigned char bluetoothjudge_crc16(void)
{
	unsigned short crc0,crc1;
	crc0=bd_crc16(0,g_apdu,g_apdu_length-2);
	crc1=communicationBluetooth.crc16[0];
	crc1=(crc1<<8)|communicationBluetooth.crc16[1];
	
	if(crc1!=crc0)
	{
		return 1;   //fail
	}
	return 0;       //success
}


void recivestatus_F(void)
{	
	communicationBluetooth.data[0]=CMD09_oldlabel;
	communicationBluetooth.data[1]=CMD09_SW[0];	
	communicationBluetooth.data[2]=CMD09_SW[1];	

	Send_bluetoothdata(3);

}


void setup_time_f(unsigned char* value)
{
	time_union_t time;
	time.data = 0;
	time.data |= value[3];
	time.data |= value[2] << 8;
	time.data |= value[1] << 16;
	time.data |= value[0] << 24;
	set_system_clock(time); 
  Send_bluetoothdata(1);
	if(	poweronkey_flag==0)
	{
		firmwaredownload_GPIO_H();      //enter APP mode
		nrf_gpio_pin_clear(SlectPin);
		PowerOn_key();                  //stm32 poweron
	}

	if(Main_status!=Main_status_timedisplay)
	{
		if(Ack_recive_enable==0)
		{
			  KEYwork_flag=0;
			Main_status=Main_status_timedisplay;    
			g_apdu[stm32uartBUFstar]=timerstm32;//send cmd ,enter timer display mode
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
		}
	}

}

/***********************
balance seting
*************************/
void setup_balance_f(unsigned char* value)
{
	unsigned char i;
	unsigned char buf[16];
	unsigned char* p;

    //update balance
    memcpy(&coinbalance,value,balnace_usefsize);
	p=(unsigned char*)&coinbalance;
	//flash storage
	//Save balance data to flash
	for(i=0;i<4;i++)
	{
		pstorage_block_identifier_get(&base_handle,balance_storgeblock+i, &USEblock_handle);			
		pstorage_wait_flag=1;
		pstorage_clear(&USEblock_handle, 16);
		while(pstorage_wait_flag==1);//wait clear flash
		pstorage_wait_flag=1;
		pstorage_store(&USEblock_handle,p+i*16,16,0);
		while(pstorage_wait_flag==1);			
	}

	//Save coinkind and flag
	for(i=1;i<9;i++)
	{
		buf[i+7]=i;
	}
    pstorage_block_identifier_get(&base_handle,65, &USEblock_handle);			
	pstorage_wait_flag=1;
	pstorage_clear(&USEblock_handle, 16);
	while(pstorage_wait_flag==1);//wait clear flash
	pstorage_wait_flag=1;
	pstorage_store(&USEblock_handle,buf,16,0);
	while(pstorage_wait_flag==1);	
	
	Send_bluetoothdata(1);
}

void setup_timeout_f(unsigned char* value)
{
	unsigned int i=value[0]*10000;
	unsigned int timeout=APP_TIMER_TICKS(i, APP_TIMER_PRESCALER);

  if(i!=0)
	{
	app_timer_stop(Timeout3Sec_id); 
	app_timer_start(Timeout3Sec_id,timeout, NULL);
	Timeout3Sec_StarFlag=TimeOpen; 
	Send_bluetoothdata(1);		
	}

}

void setup_getpower_f(unsigned char* value)
{
	uint8_t batlvl=0;
	if(adc_sample<Lowest_Voltage){batlvl=0;}
	else
	{
		if(adc_sample<Full_Voltage){batlvl=(adc_sample-Lowest_Voltage)*100/(Full_Voltage-Lowest_Voltage);}//get battery level
		else{batlvl=100;}
	}
	communicationBluetooth.data[1]=batlvl;	
	Send_bluetoothdata(2);

}

void setup_getversion_f(unsigned char* value)
{

	communicationBluetooth.data[1]=version_0;	
	communicationBluetooth.data[2]=version_1;
	communicationBluetooth.data[3]=version_2;
	Send_bluetoothdata(4);

}

void setup_turnoff_f(void)
{
	app_timer_stop(Timeout3Sec_id);                                 //stop timer
	Timeout3Sec_StarFlag=TimeClose;                                 //clear flag
	Time_stuts=TimeDataDisPl;

	Main_status=Main_status_closeoled;                              //states chang to shutdown mode
	PowerOff_key();                                                 //stm32 power off
	Send_bluetoothdata(1);
}


/**************************************************
seting cmd manage function
***************************************************/
void Setup_F(void)
{	
	   //according different key
		switch(communicationBluetooth.data[0])
		{
			case setup_time:           setup_time_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_balance:        setup_balance_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_timeout:        setup_timeout_f(&communicationBluetooth.data[1]);
			break;

			case setup_getpower:       setup_getpower_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_getversion:     setup_getversion_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_turnoff:        setup_turnoff_f();
      break;
		}			
}


void download_cmdid_F(void)
{

  	if(communicationBluetooth.data[0]==0x01)
	{
		app_timer_stop(Timeout3Sec_id);          //turn off timer
		Timeout3Sec_StarFlag=TimeClose;
		if(Main_status==Main_status_closeoled)   //oled is shutdown,you can update firmware
		{
			firmwaredownload_GPIO_L();           //entern update mode
		PowerOn_key();
		
			Main_status=Main_status_download;
		KEYwork_flag=1;
			communicationBluetooth.data[0]=0x02;
			communicationBluetooth.data[1]=0x00;
			communicationBluetooth.data[2]=0x00;
		}
		else
		{
			communicationBluetooth.data[0]=0x02;
			communicationBluetooth.data[1]=0x01;
			communicationBluetooth.data[2]=0x00;				
		}
		Send_bluetoothdata(3); 	            
	}

	if(communicationBluetooth.data[0]==0x03&& Main_status==Main_status_download)
	{
         //apdu cmd
		 APDU_blekey=&communicationBluetooth.data[3];                                       //datas bufer
		 Apdu_len_blekey=(communicationBluetooth.data[1]<<8)|communicationBluetooth.data[2];//data length	
	}
	
	if(communicationBluetooth.data[0]==0x04&& Main_status==Main_status_download)
	{
		//exit update mode
		app_timer_stop(Timeout3Sec_id);        
		Timeout3Sec_StarFlag=TimeClose;

		PowerOff_key();
		nrf_delay_ms(delaytime);
		firmwaredownload_GPIO_H();//enter APP mode

		nrf_gpio_pin_clear(SlectPin);

		PowerOn_key();

		g_apdu[stm32uartBUFstar]=timerstm32;
		CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
		Main_status=Main_status_timedisplay;   

		KEYwork_flag=0;

		communicationBluetooth.data[0]=0x02;
		communicationBluetooth.data[1]=0x00;
		communicationBluetooth.data[2]=0x00;

		Send_bluetoothdata(3); 		
	}
}


void test_cmdid_F(void)
{
//	  printf("test_cmdid_F\r\n");
}


void blueKEY_cmdid_F(void)
{
	if(Main_status==Main_status_download)
	{return;}

	if(communicationBluetooth.data[0]==0x02&&KEYwork_flag==1)
	{//apduָ manage     
		APDU_blekey=&communicationBluetooth.data[3];
		Apdu_len_blekey=(communicationBluetooth.data[1]<<8)|communicationBluetooth.data[2];
	}
	else
	{
		if(communicationBluetooth.data[0]!=0x01)
		{return;}
		app_timer_stop(Timeout3Sec_id);      
		Timeout3Sec_StarFlag=TimeClose;

		if(communicationBluetooth.data[1]==0x01)//&&KEYwork_flag!=1)
		{
			PowerOff_key();
			nrf_delay_ms(delaytime);
			firmwaredownload_GPIO_H();//APP mode

//			nrf_gpio_pin_set(SlectPin);
      nrf_gpio_pin_clear(SlectPin);
			PowerOn_key();

			g_apdu[stm32uartBUFstar]=bluekeystm32;
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			KEYwork_flag=1;
			Main_status=Main_status_blekey;
		}
		if(communicationBluetooth.data[1]==0x00)//&&(KEYwork_flag!=0))
		{
			PowerOff_key();
			nrf_delay_ms(delaytime);
			firmwaredownload_GPIO_H();//APP mode
			nrf_gpio_pin_clear(SlectPin);
			PowerOn_key();
			g_apdu[stm32uartBUFstar]=timerstm32;
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			Main_status=Main_status_timedisplay;     
			Time_stuts=TimeDataDisPl;
			KEYwork_flag=0;
			Send_bluetoothdata(1); 	            
		}
	}
}


///////////////////////////////////////FLASH read or write//////////////////////////
void blue_writeflash(unsigned char* buf)
{
	unsigned char i;
	unsigned char j=buf[0];
	unsigned char bufdata[64];
	memcpy(bufdata,&buf[1],64);
	//Save time data to flash
	for(i=0;i<4;i++)
	{
		pstorage_block_identifier_get(&base_handle,67+j*4+i, &USEblock_handle);			
		pstorage_wait_flag=1;
		pstorage_clear(&USEblock_handle,16);
		while(pstorage_wait_flag==1);//??flash????	
		pstorage_wait_flag=1;

		pstorage_store(&USEblock_handle,&bufdata[i*16],16,0);
		while(pstorage_wait_flag==1);			
	}

	Send_bluetoothdata(1); 	         

}

void blue_readflash(unsigned char address)
{
	unsigned char i;
	unsigned char buf[64];
	
	for(i=0;i<4;i++)
	{
		pstorage_block_identifier_get(&base_handle,67+address*4+i, &USEblock_handle);			
		pstorage_wait_flag=1;
		pstorage_load(&buf[16*i], &USEblock_handle,16, 0);
		while(pstorage_wait_flag==1);		
	}

	memcpy(&communicationBluetooth.data[1],buf,64);	
	Send_bluetoothdata(65);
	
}

void flashwriteread_F(void)
{

	switch(communicationBluetooth.data[0])
	{
		case 0x01://write flash
			blue_writeflash(&communicationBluetooth.data[1]);
			break;
		case 0x02://read flash
			blue_readflash(communicationBluetooth.data[1]);
			break;
	}
}


void firmware_signed_F(void)
{
			nrf_gpio_pin_set(SlectPin);
			PowerOn_key();
			Main_status=Main_status_firmware;
}

void bluetoothupdate_F(void)
{
	unsigned char i;

	Send_bluetoothdata(1);
	app_sched_event_put(NULL,NULL, Bluetooth_ReciveANDSend);
	
	for(i=0;i<20;i++){
	app_sched_execute(); 
  sd_app_evt_wait();  
	}
	sd_power_gpregret_set(0xb1);
	NVIC_SystemReset();
}
///////////////////////////////////////FLASH read or write END//////////////////////////
/*******************************
according different cmd to work
**********************************/
void BluetoothWork(void)
{
	void (*buf[])(void)={   &download_cmdid_F,\
							&Setup_F,\
							&flashwriteread_F,\
							&bluetoothupdate_F,\
							&firmware_signed_F,\
							NULL,\
							&blueKEY_cmdid_F,\
							NULL
						};

	void (*f)(void);                              
	app_timer_stop(Timeout3Sec_id);         
	Timeout3Sec_StarFlag=TimeClose;
	f=buf[communicationBluetooth.cmd_id[0]-1];	  
	f();                                          
}

void Bluetooth_CmdProcess(void * p_event_data, uint16_t event_size)
{ 
  u16 L=(communicationBluetooth.length[0]<<8)|communicationBluetooth.length[1];
	
	//Any data need to analyze ?
	if(blueRecivSendflag==bluetoothRecivedata)
	{   //recived data
		if((L+3)==g_apdu_length)
		{
			Recive_bluetoothdata_point();                            //analyze data
																				
			if(recivestatus_cmdid!=communicationBluetooth.cmd_id[0])
			{                                                      
				CMD09_oldlabel=communicationBluetooth.label[0];        
				if(!bluetoothjudge_crc16())                           
				{                                                   
					CMD09_SW[0]=0x90;                                  
					BluetoothWork(); //work according cmd
				}
				else
				{   
					CMD09_SW[0]=0x67;                           
				}
			}
			else
			{
				recivestatus_F(); 
			}			
		
		}
		
	}	
}












