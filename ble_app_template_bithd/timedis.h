
#ifndef TIMEDIS_H__
#define TIMEDIS_H__

#define TimeDataDisPl       0
#define BalanceDisplay      1
#define BrodcastnameDisplay 2

#define balance_storgeblock 0
#define balnace_usefsize 59
#define numcoin8 8

typedef struct
{
	unsigned char coin_name[32];
	unsigned char balance[10];
	unsigned char balance_after_di[10];
	unsigned char year[2];
	unsigned char  month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char Nousebuf[5];
}coin_Attr;
extern unsigned char	Time_stuts;
extern coin_Attr coinbalance;
extern unsigned char stm32timer[9];
extern unsigned char timedisp_status;
extern unsigned char flagtimerstm32;
extern unsigned char flagbalancestm32;
//extern unsigned char flagnamestm32;

void timedisplay(void * p_event_data, uint16_t event_size);


#endif
