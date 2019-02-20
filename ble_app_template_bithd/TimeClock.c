#include "include_all.h"
UTCTimeStruct Global_Time;     
DAY_OF_WEEK TimerWeek;

alarm_clock_field_type Clock_timebuf[alarmMAX];

UTCTime SecondCountRTC;

void system_clock_init(void)
{
    /* set a default value */
    Global_Time.year = BEGYEAR;
    Global_Time.month = 0;
    Global_Time.day = 0;
    Global_Time.hour =0;
    Global_Time.minutes = 0;
    Global_Time.seconds = 0;

    /* Init ticks from RTC */
    SecondCountRTC = 0;       /* This should read from flash */

}

/* calculate day of week*/
void get_day_of_week(UTCTime secTime)
{
    uint32_t day = secTime / DAY;

    TimerWeek= (DAY_OF_WEEK)(((day %LENGTH_OF_WEEK) + SYSTEM_ORIGIN_DAY_OF_WEEK) %LENGTH_OF_WEEK); 

}
/*********************************************************************
 * Get month length
 *********************************************************************/
uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
    uint8_t days = 31;

    if ( mon == 1 ) // feb
    {
        days = ( 28 + lpyr );
    } else {
        if ( mon > 6 ) // aug-dec
        {
            mon--;
        }

        if ( mon & 1 ) {
            days = 30;
        }
    }

    return ( days );
}
/**************************************************************************
UTCTime
***************************************************************************/
void ConvertToUTCTime( UTCTimeStruct *tm, UTCTime secTime )
{
    // calculate the time less than a day - hours, minutes, seconds
    {
        uint32_t day = secTime % DAY; 
        tm->seconds = day % 60UL;
        tm->minutes = (day % 3600UL) / 60UL;
        tm->hour = day / 3600UL;
    }

    // Fill in the calendar - day, month, year
    {
        uint16_t numDays = secTime / DAY;
        tm->year = BEGYEAR;
        while ( numDays >= YearLength( tm->year ) ) 
        {
            numDays -= YearLength( tm->year );
            tm->year++;
        }

        tm->month = 0;
        while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ))
        {
            numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
            tm->month++;
        }

        tm->day = numDays;
    }
}

UTCTimeStruct * get_wall_clock_time(void)
{
    ConvertToUTCTime(&Global_Time,SecondCountRTC);
    Global_Time.month += 1; //calibration
    Global_Time.day += 1; //calibration
    return &Global_Time;
}


extern void feed(void);
unsigned char pstorage_times=0;
void update_wall_clock(void * p_event_data, uint16_t event_size)
{
    unsigned char buf[16]={0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8};	
    SecondCountRTC++;                           

	if(switch_ble_flag == BLE_ON_STA)
	{
		close_ble_count++;
	}	
	if(close_ble_count>SECOND_CLOSE_BLE)
	{
		close_ble_count = 0;
		switch_ble_flag = BLE_OFF_STA;
	}

	  UTCTimeStruct * tm = get_wall_clock_time(); 
		get_day_of_week(SecondCountRTC);

    feed();
		 
    if(Main_status==Main_status_closeoled)
		{
			  pstorage_times++;
			  
			  //low power do not save time datas
			  if(adc_sample < DIS_BLE_VOLTAGE)
			  {
				return;
			  }
				if(pstorage_times<60)
				{}
				else
				{
					pstorage_times=0;
					//Save time data to flash
					pstorage_block_identifier_get(&base_handle,64, &USEblock_handle);			
					pstorage_wait_flag=1;
					pstorage_clear(&USEblock_handle, 16);
					while(pstorage_wait_flag==1);
					pstorage_wait_flag=1;
					memcpy(buf,(unsigned char*)(&SecondCountRTC),4);
					pstorage_store(&USEblock_handle,buf,16,0);
					while(pstorage_wait_flag==1);
				}
		}
}
extern app_timer_id_t    wallClockID;
void set_system_clock(time_union_t time)
{
    uint32_t i = 0;
    UTCTime offset = 0;

    //day time
    offset += time.time.seconds;
    offset += time.time.minute * 60;
    offset += time.time.hours * 60 * 60;

    uint8_t leapYear = IsLeapYear(time.time.year + 2000);

    offset += DAY * (time.time.day - 1);

    for(i = 0; i < time.time.month - 1; ++i) { //month start from 1
        offset += monthLength(leapYear,i) * DAY;
    }

    for(i = 0; i< time.time.year ;++i) {
        if(IsLeapYear(i + 2000)) {
            offset += DAY * 366;
        } else {
            offset += DAY * 365;
        }
    }

    SecondCountRTC = offset;
    /************************************************************
    * restart timer
    ************************************************************/
    app_timer_stop(wallClockID);
    app_timer_start(wallClockID, ONE_SECOND_INTERVAL_, NULL);

}
