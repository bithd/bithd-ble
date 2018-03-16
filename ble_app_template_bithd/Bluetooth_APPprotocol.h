#ifndef BLUETOOTH_APPPROTOCOL_H_
#define BLUETOOTH_APPPROTOCOL_H_



#define recivestatus_cmdid 9  

#define setup_time            0x01  
#define setup_balance         0x02 
#define setup_timeout         0x03

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
