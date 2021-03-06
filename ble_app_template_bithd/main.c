
////////////////////////////////////////////////////////////////////////////////////////////
#include "include_all.h"

///////////////////////////////////////DMWZ//////////////////////////////////////////////////////////////

///////////////////////////////////////DMWZ_END/////////////////////////////////////////////////////////////



/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

#define adress PSTORAGE_DATA_START_ADDR+0x400

static void INIT(void)
{    
  unsigned char buf123[16];
	unsigned char buf456[]={1,2,3,4,5,6,7,8};
	unsigned char* p;

    // Initialize.
		p=(unsigned char*)(adress+0x00000400);//0x38400
		memcpy(buf123,p,16);
		if(0==memcmp(buf456,&buf123[8],8))
		{
			pstorage_flag=1;
			memcpy((unsigned char*)(&SecondCountRTC),buf123,4);
		}
		else
		{
			pstorage_flag=0;
		}
		
	  p=(unsigned char*)((adress+0x400)+16);
		memcpy(buf123,p,16);
		if(0==memcmp(buf456,&buf123[8],8))
		{
			pstorage_flag=2;
			p=(unsigned char*)(adress);
			memcpy((unsigned char*)(&coinbalance),p,64);
		}

		
    timers_init();        
		gpio_init();           
    adc_config();       
		scheduler_init();       
		//BLE Initialize.
    ble_stack_init_DMWZ();     
		device_manager_init_DMWZ(0);
		
		db_discovery_init(); 
		Mac_address_get();
    gap_params_init_DMWZ();      
    services_init_DMWZ();

		advertising_init_DMWZ();

    conn_params_init_DMWZ();  

    mcuflash_init();      
    touch_key=KEY_LongTouch;

}                     

void main_loop(void)
{
 
	app_sched_event_put(NULL,NULL, Bluetooth_ReciveANDSend); 
	app_sched_event_put(NULL,NULL, Bluetooth_CmdProcess);   
	app_sched_event_put(NULL,NULL, UartDataSendrecive);

	switch(Main_status)
	{
		case Main_status_closeoled:        app_sched_event_put(NULL,0, progam_closeoled);       
		break;
		case Main_status_changbatlowpower: app_sched_event_put(NULL,0, Chargingandmanagerdisplay);
		break;		 
		case Main_status_timedisplay:      app_sched_event_put(NULL,0, timedisplay);
		break;		 	
		case Main_status_download:         app_sched_event_put(NULL,0, BlueUPfirmware);
		break;		 
		case Main_status_blekey:           app_sched_event_put(NULL,0, Bluetooyhkeydisplay);
		break;		
		case Main_status_ParingDis:        app_sched_event_put(NULL,0, Bluetooyparingdisplay);
		break;		
    case Main_status_firmware:         app_sched_event_put(NULL,0, firmwaresigned);
		break;
	}
}

/**@brief Function for application main entry.
 */

int main(void)
{
    uint32_t err_code;
		
		INIT();
    // Start execution.
    application_timers_start(); 
    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);

    wdtinit();
    // Enter main loop.
    for (;;)
    {
			  main_loop();
			  app_sched_execute();   
        power_manage();
    }
}

/**
 * @}
 */
