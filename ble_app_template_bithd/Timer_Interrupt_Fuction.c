#include "include_all.h"

app_timer_id_t                 m_sec_req_timer_id;
//app_timer_id_t                 m_app_timer_id;
app_timer_id_t                 KeepTouch_timer_id;              
unsigned char                  Touch_timercount=0;               
app_timer_id_t                 Scan_Touch_timer_id;            
app_timer_id_t                 wallClockID;     
app_timer_id_t                 ADCworkID;    

app_timer_id_t                 Timeout3Sec_id;                    
unsigned char                  Timeout3Sec_StarFlag=TimeClose;    

app_timer_id_t                 Timeout1Sec_f_id;                  
unsigned char                  Timeout1Sec_f_StarFlag=TimeClose;  

app_timer_id_t                 Timeout2ms_blekey_id;           

app_timer_id_t                 Motor_id;

app_timer_id_t                 balance_id;

app_timer_id_t                 Timeout1Sec_Uart_id;                  
unsigned char                  Timeout1Sec_Uart_StarFlag=TimeClose;  

app_timer_id_t                 chargestatus_time_id;
unsigned char                  chargstatustime_flag=0;
/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
void timers_init(void)
{

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers.
    uint32_t err_code;
//    err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
//    APP_ERROR_CHECK(err_code); 

		err_code = app_timer_create(&KeepTouch_timer_id, APP_TIMER_MODE_REPEATED, timer_touchkey_handler);  
    APP_ERROR_CHECK(err_code);
		err_code = app_timer_create(&Scan_Touch_timer_id, APP_TIMER_MODE_REPEATED, timer_Scantouchkey_handler);
    APP_ERROR_CHECK(err_code);

		err_code = app_timer_create(&balance_id,  APP_TIMER_MODE_REPEATED,Balance_handler); 
    APP_ERROR_CHECK(err_code);		

		err_code = app_timer_create(&wallClockID, APP_TIMER_MODE_REPEATED, TimerClock_handler);                
    APP_ERROR_CHECK(err_code);

		err_code = app_timer_create(&ADCworkID, APP_TIMER_MODE_REPEATED, ADCwork_handler);                  
    APP_ERROR_CHECK(err_code);		

		err_code = app_timer_create(&Timeout3Sec_id, APP_TIMER_MODE_SINGLE_SHOT, TimeOutMain_handler);        
    APP_ERROR_CHECK(err_code);
		err_code = app_timer_create(&Timeout1Sec_f_id, APP_TIMER_MODE_SINGLE_SHOT, TimeOut1sec_handler);        
    APP_ERROR_CHECK(err_code);
		
		err_code = app_timer_create(&Timeout2ms_blekey_id, APP_TIMER_MODE_REPEATED, Ble_timeout_handler);
    APP_ERROR_CHECK(err_code); 

		err_code = app_timer_create(&Motor_id, APP_TIMER_MODE_REPEATED, Motor_timeout_handler);
    APP_ERROR_CHECK(err_code);  

    err_code = app_timer_create(&m_sec_req_timer_id,APP_TIMER_MODE_SINGLE_SHOT,sec_req_timeout_handler); 
    APP_ERROR_CHECK(err_code); 

		err_code = app_timer_create(&Timeout1Sec_Uart_id, APP_TIMER_MODE_SINGLE_SHOT, TimeOutUart_handler);     
    APP_ERROR_CHECK(err_code);
		
		err_code = app_timer_create(&chargestatus_time_id, APP_TIMER_MODE_REPEATED, Timechargestatus_handler);     
    APP_ERROR_CHECK(err_code);
		
}
 



/**@brief Function for starting timers.
*/
void application_timers_start(void)
{
    // YOUR_JOB: Start your timers. below is an example of how to start a timer.
//		app_timer_start(m_app_timer_id, m_app_TIMER_T, NULL);
		app_timer_start(Scan_Touch_timer_id,Touch_TIMER_Scan, NULL);           
    app_timer_start(wallClockID, ONE_SECOND_INTERVAL_, NULL);           
		app_timer_start(ADCworkID, ADC_INTERVAL, NULL);                    
		app_timer_start(Timeout2ms_blekey_id, bletimeout_INTERVAL, NULL);     
//		app_timer_start(chargestatus_time_id, bletimeout_INTERVAL, NULL); 
}



//void debug_wangnan(void * p_event_data, uint16_t event_size)
//{
//	USBTEST_status();
//}


////debug
//void timer_timeout_handler(void * p_context)
//{
//	app_sched_event_put(NULL,0, debug_wangnan);
//}

void Motor_timeout_handler(void * p_context)
{
	
	if((flagtimerstm32==1)&&(Main_status==Main_status_timedisplay))
	{
		if(Ack_recive_enable==0){
		memcpy(&g_apdu[stm32uartBUFstar],stm32timer,sizeof(stm32timer));
  	CmdSendUart(EnterTimercmd_uart,&g_apdu[stm32uartBUFstar],sizeof(stm32timer));
	}
	}
	else
	{
		app_timer_stop(Motor_id);
		flagtimerstm32=0;
	}
}


void Balance_handler(void * p_context)
{
	if((flagbalancestm32==1)&&(Main_status==Main_status_timedisplay)&&(Time_stuts==BalanceDisplay))
	{
		if(Ack_recive_enable==0){
		memcpy(&g_apdu[stm32uartBUFstar],&coinbalance,balnace_usefsize);	
    CmdSendUart(Balancecmd_uart,&g_apdu[stm32uartBUFstar],balnace_usefsize);
	}
	}
	else
	{
		app_timer_stop(balance_id);
		flagbalancestm32=0;
	}

}


void Ble_timeout_handler(void * p_context)
{
	timer0_sec_count++; 
}

void timer_touchkey_handler(void * p_context)
{
	if(Touch_timercount<TouchTimerMax)
	{Touch_timercount++;}
}

void timer_Scantouchkey_handler(void * p_context)
{
	app_sched_event_put(NULL,0, Read_CapKey);
}



unsigned char clock=0;
void TimerClock_handler(void * p_context)
{
	clock++;
	if(clock>9)
	{
		app_sched_event_put(NULL,0, update_wall_clock);
   	clock=0;
	}
}

void ADCwork_handler(void * p_context)
{
	app_sched_event_put(NULL,0, ReadAdc); 
}

void TimeOutMain_handler(void * p_context)
{
	Timeout3Sec_StarFlag=TimeOut;
}

void TimeOut1sec_handler(void * p_context)
{
	Timeout1Sec_f_StarFlag=TimeOut;
}

void TimeOutUart_handler(void * p_context)
{
	Timeout1Sec_Uart_StarFlag=TimeOut;
}
#define NoCharg  0
#define Charging 1
#define Charged  2
#define _1200ms_ 1200
#define _3500ms_ 3500
unsigned char chargflag=NoCharg;
unsigned char H_count=0;
unsigned char L_count=0;
void Timechargestatus_handler(void * p_context)
{
//每隔100ms进入一次中断
//判断当前状态
  USB_ChangingFLAG=USBnoChanging;
  switch(chargflag){
		case (NoCharg):
			if(nrf_gpio_pin_read(CHG_Status_pin)==1)
			{
				USB_ChangingFLAG=USBChanged;
				H_count++;
				USB_connectFLag=USBconnect;
				chargflag=Charging;
			}
			else
			{
				app_timer_stop(chargestatus_time_id);
				chargstatustime_flag=0;
				H_count=0;
				L_count=0;
				USB_connectFLag=USBDisconnect;
			}
			break;
		case (Charging):
				if(nrf_gpio_pin_read(CHG_Status_pin)==1)
				{
					H_count++;
					L_count=0;
					USB_ChangingFLAG=USBnoChanging;
					if(H_count>(_1200ms_/100))
					{
						chargflag=Charged;
						H_count=0;
					}
				}
				else
				{
					H_count=0;
					L_count++;
					if(L_count>(_1200ms_/100))
					{
						chargflag=NoCharg;
						L_count=0;
						app_timer_stop(chargestatus_time_id);
						chargstatustime_flag=0;
						USB_connectFLag=USBDisconnect;
					}
				}
			break;
		case (Charged):
			if(nrf_gpio_pin_read(CHG_Status_pin)==0)
				{
					H_count=0;
					L_count++;
					if(L_count>(_3500ms_/100))
					{
            chargflag=NoCharg;
						L_count=0;
						app_timer_stop(chargestatus_time_id);
						chargstatustime_flag=0;
						USB_connectFLag=USBDisconnect;
					}
				}
				else
				{
					L_count=0;
					H_count=0;
				}
			break;
   }
}



