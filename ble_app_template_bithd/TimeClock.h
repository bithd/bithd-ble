#ifndef _Time_CLOCK_H_
#define _Time_CLOCK_H_

#define IsLeapYear(yr) (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define YearLength(yr) (IsLeapYear(yr) ? 366 : 365)
#define BEGYEAR         2000     // UTC started at 00:00:00 January 1, 2000 
#define DAY             86400UL  // 24 hours * 60 minutes * 60 seconds
#define LENGTH_OF_WEEK      (7)
#define alarmMAX 8
#define SECOND_CLOSE_BLE		3600

//2000-01-01 is sat
#define SYSTEM_ORIGIN_DAY_OF_WEEK (Sat)
// To be used with
typedef uint32_t UTCTime; /* used to store the second counts for RTC */
typedef struct
{
    uint16_t year;    // 2000+
    uint8_t month;    // 0-11
    uint8_t day;      // 0-30
    uint8_t hour;     // 0-23
    uint8_t minutes;  // 0-59
    uint8_t seconds;  // 0-59
}
UTCTimeStruct;
typedef enum {
    MOn  = 0,
    Tues  = 1,
    Wed  = 2,
    Thur = 3,
    Fri  = 4,
    Sat  = 5,
    Sun  = 6
}DAY_OF_WEEK;

/* time bit field */
typedef struct
{
uint32_t seconds  :
    6;
uint32_t minute  :
    6;
uint32_t hours  :
    5;
uint32_t day   :
    5;
uint32_t month  :
    4;
uint32_t year   :
    6;
}
time_bit_field_type_t;

typedef union
{
    uint32_t data;
    time_bit_field_type_t time;
} time_union_t;
/* time bit field */


/* alarm clock bit field */
typedef struct
{
uint64_t day_repeat_flag    :
    7;
uint64_t status   :
    4;
uint64_t id     :
    3;
uint64_t minute     :
    6;
uint64_t hour       :
    5;
uint64_t day            :
    5;
uint64_t month      :
    4;
uint64_t year           :
    6;
}
alarm_clock_field_type;

typedef union { 
    uint64_t data;
    alarm_clock_field_type alarm;
} alarm_union_t;


extern UTCTime SecondCountRTC; 
extern alarm_clock_field_type Clock_timebuf[8];
extern DAY_OF_WEEK TimerWeek; 
extern UTCTimeStruct Global_Time;
void set_system_clock(time_union_t time);
UTCTimeStruct * get_wall_clock_time(void);
void update_wall_clock(void * p_event_data, uint16_t event_size);
#endif

