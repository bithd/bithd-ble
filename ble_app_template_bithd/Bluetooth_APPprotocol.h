#ifndef BLUETOOTH_APPPROTOCOL_H_
#define BLUETOOTH_APPPROTOCOL_H_

#define version_0 0x02
#define version_1 0x00
#define version_2 0x02

#define recivestatus_cmdid 9  

#define setup_time            0x01  
#define setup_balance         0x02 
#define setup_timeout         0x03
#define setup_getpower        0x04
#define setup_getversion      0x05
#define setup_turnoff         0x06


typedef struct BluetoothDataStruct		
{
	u8 *label;      
	u8 *length;    
	u8 *cmd_id;    
	u8 *data;       
	u8 *crc16;      
} BluetoothData;




extern BluetoothData communicationBluetooth;
void Send_bluetoothdata(unsigned short len);
void Bluetooth_CmdProcess(void * p_event_data, uint16_t event_size);
void remind_cmdid_F(void);
void BluetoothWork(void);
#endif
