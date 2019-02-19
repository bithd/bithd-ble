#include "include_all.h"
extern app_timer_id_t                   m_sec_req_timer_id;
unsigned char                           ParingDataBuf[6];                                //save paring datas bufer
unsigned char                           ParingDataDisFlag=0;                             //paring data display flag/ 1 need display/ 0 not need display
unsigned char                           Ble_ConnnectFlag=BLE_Disconnect;                 //ble connect flag
unsigned char                           Ble_AuthFlag=0;                                  //ble authentication flag / 1 hasbeen authentication/ 0 not authentication

static dm_application_instance_t        m_app_handle;                                    /**< Application identifier allocated by device manager */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;         /**< Handle of the current connection. */
static app_bond_table_t                 m_app_bond_table;  
static dm_handle_t                      m_peer_handle;                                   /**< Identifies the peer that is currently connected. */

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init_DMWZ(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    //set device name
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (uint8_t const *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
    APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;  //min connext interval
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;  //max connect interval
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;   //timeout

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing services that will be used by the application.
 */
void services_init_DMWZ(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;
    
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = BLE_TXRX_handler;   //add APP service handl
    
    err_code = ble_TxRx_init(&m_TXRX, &nus_init);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
 void conn_params_init_DMWZ(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;  
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
        break;
        case BLE_ADV_EVT_IDLE:
        break;
        default:
        break;
    }
}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{   
    switch (p_ble_evt->header.evt_id)
            {
        case BLE_GAP_EVT_CONNECTED:
                        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;	           
                        app_timer_start(m_sec_req_timer_id, SECURITY_REQUEST_DELAY,NULL);
        break;

        case BLE_GAP_EVT_DISCONNECTED:
                        m_conn_handle = BLE_CONN_HANDLE_INVALID;
                        Ble_AuthFlag=BLE_AuthFail;
        break;
					
        case BLE_GAP_EVT_PASSKEY_DISPLAY:
                        ParingDataDisFlag=1;
                        for ( int i = 0; i < 6; i++ )
                        {
                            ParingDataBuf[i]=p_ble_evt->evt.gap_evt.params.passkey_display.passkey[i];
                        }
        break;

		case BLE_GAP_EVT_AUTH_STATUS:                                                                                                                                    //???????  ????????  
                        if(p_ble_evt->evt.gap_evt.params.auth_status.auth_status != BLE_GAP_SEC_STATUS_SUCCESS)   
                        {
                            Ble_AuthFlag=BLE_AuthFail;							
                            sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                        }
                        else
                        {
                            Ble_AuthFlag=BLE_AuthSuccess;				//authentication success
                        }
		break;  

        default:
            // No implementation needed.
        break;
    }
}

void blueDisconnect(void)
{
	Ble_AuthFlag=BLE_AuthFail;
	sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}
/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    dm_ble_evt_handler(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
		
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    /*YOUR_JOB add calls to _on_ble_evt functions from each service your application is using*/
    ble_TxRx_on_ble_evt(&m_TXRX, p_ble_evt); //Add ble data recive&send service event
}

/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init_DMWZ(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION, NULL);
	
    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT; 
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the Device Manager events.
 *
 * @param[in] p_evt  Data associated to the device manager event.
 */
 static uint32_t device_manager_evt_handler(dm_handle_t const * p_handle,
                                           dm_event_t const  * p_event,
                                           ret_code_t        event_result)
{
    uint32_t err_code;

    // Recovery in the event of DM_DEVICE_CONTEXT_FULL
    if(event_result == DM_DEVICE_CONTEXT_FULL)
    {
        /* Clear all devices from the bond table*/ 
        err_code = dm_device_delete_all(&m_app_handle);
        APP_ERROR_CHECK(err_code);
             
    }
    else
    {}

	#if 0
    if (p_event->event_id == DM_EVT_DEVICE_CONTEXT_STORED)
    {
        table_index_t table_index;
        
        //Find first and last bond created from m_bond_index_table 
        app_bond_find(&m_app_bond_table,&table_index);
        
		//Increment counter if a new bond was created
        if(!(table_index.mr_cnt_val >= m_app_bond_table.app_bond_cnt[p_handle->device_id]))
        {
           table_index.mr_cnt_val++;
           m_app_bond_table.app_bond_cnt[p_handle->device_id] = table_index.mr_cnt_val;
        }
                
        //Delete first created bond if bond table is full 
        if(((table_index.mr_cnt_val-table_index.lr_cnt_val)== DEVICE_MANAGER_MAX_BONDS-1)
             && (table_index.lr_cnt_val != NO_APP_CONTEXT))
          {
                uint32_t err_code;
                dm_handle_t device;
                
                device.appl_id = 0;
                    
                m_app_bond_table.app_bond_cnt[table_index.lr_index]=NO_APP_CONTEXT; 
                device.device_id = m_app_bond_table.device_id[table_index.lr_index];
                    
                err_code = dm_device_delete(&device); 
                APP_ERROR_CHECK(err_code);
                    
          }   
                
        //Update the app context for new device
        app_bond_update_context(&m_app_bond_table,p_handle); 
        
    }
    else if (p_event->event_id ==DM_EVT_DEVICE_CONTEXT_DELETED)
    {
        
         /* Wait for all devices to be cleared before perfoming a sys reset */ 
         if(device_delete_all_started && (p_handle->device_id == DEVICE_MANAGER_MAX_BONDS -1))
         {
             err_code = sd_nvic_SystemReset();
             APP_ERROR_CHECK(err_code);
         }
    }
	#endif
	
    return NRF_SUCCESS;
}

void device_manager_init_DMWZ(bool erase_bonds)
{
    uint32_t               err_code;
    dm_init_param_t        init_param = {.clear_persistent_data = erase_bonds};
    dm_application_param_t register_param;

    // Initialize persistent storage module.
    err_code = pstorage_init();
    APP_ERROR_CHECK(err_code);

    err_code = dm_init(&init_param);
    APP_ERROR_CHECK(err_code);

    memset(&register_param.sec_param, 0, sizeof(ble_gap_sec_params_t));

    register_param.sec_param.bond         = SEC_PARAM_BOND;
    register_param.sec_param.mitm         = SEC_PARAM_MITM;
    register_param.sec_param.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    register_param.sec_param.oob          = SEC_PARAM_OOB;
    register_param.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    register_param.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    register_param.evt_handler            = device_manager_evt_handler;
    register_param.service_type           = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

    err_code = dm_register(&m_app_handle, &register_param);
    APP_ERROR_CHECK(err_code);
		
		app_bond_init(&m_app_bond_table);
}

/**@brief Function for initializing the Advertising functionality.
 */
static ble_uuid_t     m_txrx_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN}};  /*scan notify service*/
void advertising_init_DMWZ(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;  //scan variable
    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;           //broadcast interval
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS; //broadcast timeout seting,0 means ceaseless broadcast

	memset(&scanrsp, 0, sizeof(scanrsp));                       //seting scan variable,for give service attribute
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_txrx_adv_uuids) / sizeof(m_txrx_adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = m_txrx_adv_uuids;
    err_code = ble_advertising_init(&advdata,&scanrsp, &options, on_adv_evt, NULL);//
    APP_ERROR_CHECK(err_code);
		
}

unsigned char mac_ascii[24];
unsigned char mac[6]={0x42,0x13,0xc7,0x98,0x95,0x1a}; //Device MAC address
char DEVICE_NAME[20]="BITHD";//"bithd001122334455";

/***************************************************************
Seting Mac address
****************************************************************/
void Mac_address_set(void)
{
    ble_gap_addr_t  Mac_address;
    Mac_address.addr_type=BLE_GAP_ADDR_CYCLE_MODE_NONE;
    memcpy(Mac_address.addr,mac,6);
    sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE,&Mac_address);
}

/***************************************************************
fuction:get local bluetooth address
****************************************************************/
void Mac_address_get(void)
{
	  ble_gap_addr_t  Mac_address;
	  unsigned char i,j=0;
		sd_ble_gap_address_get(&Mac_address);
		memcpy(mac,Mac_address.addr,6);
		for(i=0;i<6;i++)
		{
			if((mac[i]>>4)<0x0a)
				{
		      mac_ascii[j]=0x30+(mac[i]>>4);
					j++;
		    }
			else
				{
				mac_ascii[j]=0x31;
				j++;
				mac_ascii[j]=0x30+(mac[i]>>4)%0x0a;
				j++;
		    }
		
		  if((mac[i]&0x0f)<0x0a)
		    {
					mac_ascii[j]=0x30+(mac[i]&0x0f);
					j++;
		    }
			else
				{
		      mac_ascii[j]=0x31;
					j++;
				  mac_ascii[j]=0x30+(mac[i]&0x0f)%0x0a;
				  j++;
		    }
		}		
		
		memcpy(&DEVICE_NAME[5],mac_ascii,12);
}


/**@brief Function for initializing the database discovery module.
 */
void db_discovery_init(void)
{
    uint32_t err_code = ble_db_discovery_init();
    APP_ERROR_CHECK(err_code);
}


void sec_req_timeout_handler(void * p_context)
{
    uint32_t             err_code;
    dm_security_status_t status;

    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        err_code = dm_security_status_req(&m_peer_handle, &status);
        APP_ERROR_CHECK(err_code);

        // If the link is still not secured by the peer, initiate security procedure.
        if (status == NOT_ENCRYPTED)
        {
            err_code = dm_security_setup_req(&m_peer_handle);
            APP_ERROR_CHECK(err_code);
        }
    }
}



