#ifndef _UART_DMWZ_H__
#define _UART_DMWZ_H__

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

#define Rx_pin 29
#define Tx_pin 28
#define Rts_pin 13
#define Cts_pin 15

#define uart_0xa5        1 //0xa5
#define uart_0x5a        2 //0x5a
#define uart_length_H    3 //length high 8 bits
#define uart_length_L    4 //length low 8 bits
#define uart_cmd         5 //CMD byte
#define uart_nouseful    6 //No useful byte
#define uart_databytes   7 //Data bytes

#define uart_first_0x5a  8
#define uart_secend_0xa5 9

#define MAX_resend 10 

#define NOACK    0
#define SuccsACK 1
#define FailACK  2

#define Apducmd_uart       0x01
#define Changestatuscmd_uart 0x02
#define EnterTimercmd_uart   0x03 
#define Balancecmd_uart   0x06 
#define BlueParingDatacmd   0x07 
#define BlueNameCmd              0x08
#define SHUTDOWN_SYSTEM			 0x0A

typedef struct
{
	unsigned short length;   
	unsigned char  cmd;       
	unsigned char  no_use; 
	unsigned char* data;
	unsigned short crc16;
}
uart_communication_dmwz;
extern unsigned char cmd_uart[5];
extern volatile unsigned char Keystatus;
extern volatile unsigned char uartnum;
extern uart_communication_dmwz uart_communicate_buf;
extern unsigned char uart_enable_flag;
extern unsigned char Ack_recive_enable;
extern unsigned char uart_send_flag;
extern unsigned char uart_waitack_flag;
unsigned short CRC16_Uart_send(void);
void uart_init(void);
void Uart_close(void);
void uart_send_Bty(void* buf,unsigned short len);
void UartDataSendrecive(void * p_event_data, uint16_t event_size);
void CmdSendUart(unsigned char cmd_uart,unsigned char* apdubuf,unsigned short apdulength);
#endif
