#ifndef TIMER_INTERRUPT_FU_H__
#define TIMER_INTERRUPT_FU_H__

#define TimeOpen  1  
#define TimeClose 0 
#define TimeOut   2

#define   TouchTimerMax 200                                              
#define   Touch_TIMER_TICKS       APP_TIMER_TICKS(100, APP_TIMER_PRESCALER)
#define   Touch_TIMER_Scan        APP_TIMER_TICKS(200, APP_TIMER_PRESCALER) 

#define   NoTouch_Timeout_max     APP_TIMER_TICKS(10000, APP_TIMER_PRESCALER)
#define   LowPowTouch_Timeout_etu APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER) 
#define   NoWorkTimes             1
#define   WorkTimes               1
#define   TotleTimes              (WorkTimes+NoWorkTimes)

#define   Read3D_TIMER_Scan      APP_TIMER_TICKS(Frq_slecet, APP_TIMER_PRESCALER) 

#define   ReadStatus_TIMER_Scan  APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)
#define   ONE_SECOND_INTERVAL      APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)
#define   ONE_SECOND_INTERVAL_     APP_TIMER_TICKS(100, APP_TIMER_PRESCALER)

#define   ADC_INTERVAL            APP_TIMER_TICKS(10000, APP_TIMER_PRESCALER) 
#define   ADC_NOpower             APP_TIMER_TICKS(3000, APP_TIMER_PRESCALER)

#define   bletimeout_INTERVAL            APP_TIMER_TICKS(200, APP_TIMER_PRESCALER)   //200ms

#define   _20ms_INTERVAL            APP_TIMER_TICKS(100, APP_TIMER_PRESCALER)   //10ms

#define   motortimeout_INTERVAL          APP_TIMER_TICKS(motortime, APP_TIMER_PRESCALER)   //500ms
#define   motortimeout_LOOP              APP_TIMER_TICKS(100, APP_TIMER_PRESCALER)

#define   T_3SECOND_TimeOut        APP_TIMER_TICKS(3000, APP_TIMER_PRESCALER)
#define   T_blue_TimeOut        APP_TIMER_TICKS(60000, APP_TIMER_PRESCALER)
#define   T_update_TimeOut        APP_TIMER_TICKS(1200000, APP_TIMER_PRESCALER)
#define   KeepTouchTime            5                          
#define   T_1SECOND_TimeOut        APP_TIMER_TICKS(3000, APP_TIMER_PRESCALER)
#define   T_10SECOND_TimeOut       APP_TIMER_TICKS(10000, APP_TIMER_PRESCALER)
#define   T_3SECOND_TimeOutremind  APP_TIMER_TICKS(3000, APP_TIMER_PRESCALER)
#define   T_5SECOND_TimeOutremind  APP_TIMER_TICKS(7000, APP_TIMER_PRESCALER)
#define   T_60SECOND_TimeOutremind  APP_TIMER_TICKS(60000, APP_TIMER_PRESCALER)
#define   T_5Mini_TimeOutremind  APP_TIMER_TICKS(300000, APP_TIMER_PRESCALER)

#define SECURITY_REQUEST_DELAY          APP_TIMER_TICKS(1500, APP_TIMER_PRESCALER)  /**< Delay after connection until security request is sent, if necessary (ticks). */


#define m_app_TIMER_T APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)  //debug ms


extern app_timer_id_t            CheckKEY_id;
//extern app_timer_id_t                 m_app_timer_id;
extern app_timer_id_t                 KeepTouch_timer_id;
extern unsigned char                  Touch_timercount;
extern app_timer_id_t                 Scan_Touch_timer_id; 
extern app_timer_id_t                 Scan_StepCount_timer_id;
extern app_timer_id_t                 Scan_StepStatus_timer_id;
extern app_timer_id_t                 NoTouch_timeout_id;
extern unsigned char                  NoTouch_timeout;
extern app_timer_id_t                 LowPowTouch_timeout_id; 
extern unsigned char                  LowPowTouch_times;
extern app_timer_id_t                 Timeout3Sec_id; 
extern unsigned char                  Timeout3Sec_StarFlag;
extern app_timer_id_t                 Timeout1Sec_f_id;    
extern unsigned char                  Timeout1Sec_f_StarFlag;
extern app_timer_id_t                  Motor_id;
extern unsigned char                   motor_timeout;
extern app_timer_id_t                  Motorloop_id;
extern app_timer_id_t                  Timeout2ms_blekey_id;
extern app_timer_id_t                  Timeout1Sec_Uart_id; 
extern app_timer_id_t                  balance_id; 
extern app_timer_id_t                 chargestatus_time_id;

extern unsigned char                   Timeout1Sec_Uart_StarFlag;
extern unsigned char                  chargstatustime_flag;
void timers_init(void);
void application_timers_start(void);
void ADCwork_handler(void * p_context);
//void timer_timeout_handler(void * p_context);
void timer_touchkey_handler(void* p_context);
void timer_Scantouchkey_handler(void * p_context);
void ScanStepRead3D_handler(void * p_context);
void ScanStepStatus_handler(void * p_context);
void NoTouch_timeout_handler(void * p_context);
void LowPowTouch_timeout_handler(void * p_context);
void TimerClock_handler(void * p_context);
void TimeOutMain_handler(void * p_context);
void TimeOut1sec_handler(void * p_context);
void Ble_timeout_handler(void * p_context);
void Motor_timeout_handler(void * p_context);
void Motor_LOOP_handler(void * p_context);
void Checkkey_timeout_handler(void * p_context);
void TimeOutUart_handler(void * p_context);
void Balance_handler(void * p_context);
void Timechargestatus_handler(void * p_context);

#endif
