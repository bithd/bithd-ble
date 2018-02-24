#ifndef	_SYNAOT_H  
#define	_SYNAOT_H

#define LongKeepDownkey_maxtime 20 

#define CAPSENCE_INT_PIN 12

#define KEY_NO    0
#define KEY_1     2
#define KEY_2     1
#define KEY_3     4
#define KEY_4     8
#define KEY_Left  5 
#define KEY_Right 6 
#define KEY_LongTouch     7


/* Error_codes */
#define		OT_SUCCESS				0x00
#define		OT_FAILURE				0x01

/* True/ False Codes */
#define OT_TRUE		1
#define	OT_FALSE	0

/* OneTouch Register Definitions */

#define 	OT_CONFIG_REG_START_ADDR		0x0000


#define		OT_DATA_REG_START_ADDR			0x0106
#define		OT_NUM_DATA_REG_BYTES			4		/* (0x108 - 0x106)*2 */

#define		OT_DATA_REG_START_ADDR_HIGH		0x01
#define		OT_DATA_REG_START_ADDR_LOW		0x06

#define		OT_BUTTON_OFFSET				1

/* OneTouch Register Definitions */

#define OT_CONFIG_REG_START_ADDR        0x0000
#define OT_CONFIG_REG_END_ADDR          0x0025
#define OT_NUM_CONFIG_REG_BYTES         ((OT_CONFIG_REG_END_ADDR - OT_CONFIG_REG_START_ADDR + 1)*2)
                                        /* Total number of Configuration Bytes */
#define OT_NUM_CONFIG_BYTES             (OT_NUM_CONFIG_REG_BYTES + 2)


/* API Function Prototypes in OTLib */
void            Read_CapKey(void * p_event_data, uint16_t event_size);

//void Read_key_son(void);
extern unsigned char KeyWorkflag;
extern unsigned char touch_key_flag;           
extern unsigned char touch_key;                  

#endif
