#include "include_all.h"

ble_nus_t      m_TXRX;       //creat service Structural variable



/************************************************
ble connect event manage function
p_nus user-defined Structural/  p_ble_evt ble recive data structural
***************************************************/
static void on_connect_TxRx(ble_nus_t * p_nus, ble_evt_t * p_ble_evt)
{
    p_nus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/************************************************
ble disconnect event manage function
 ***************************************************/
static void on_disconnect_TxRx(ble_nus_t * p_nus, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_nus->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/************************************************
ble recive data event manage function 
 ***************************************************/
static void on_write_TxRx(ble_nus_t * p_nus, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (
        (p_evt_write->handle == p_nus->rx_handles.cccd_handle)
        &&
        (p_evt_write->len == 2)
       )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
        {
            p_nus->is_notification_enabled = true;
        }
        else
        {
            p_nus->is_notification_enabled = false;
        }
    }
    else if (
             (p_evt_write->handle == p_nus->tx_handles.value_handle)
             &&
             (p_nus->data_handler != NULL)
            )
    {
        p_nus->data_handler(p_nus, p_evt_write->data, p_evt_write->len);
    }
    else
    {
        // Do Nothing. This event is not relevant for this service.
    }
}


/*****************************************************************
name:ble serive attribute add
p_nus          user-defined Structural
p_nus_init     this serive need to work function
******************************************************************/
static uint32_t rx_char_add(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init)
{
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_nus->uuid_type;
    ble_uuid.uuid = BLE_UUID_NUS_RX_CHARACTERISTIC;       

    memset(&attr_md, 0, sizeof(attr_md));

//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&attr_md.write_perm);
    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_NUS_MAX_RX_CHAR_LEN;

    return sd_ble_gatts_characteristic_add(p_nus->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_nus->rx_handles);
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
}


/**@brief Function for adding TX characteristic.
 *
 * @param[in] p_nus       Nordic UART Service structure.
 * @param[in] p_nus_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t tx_char_add(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write         = 1;
    char_md.char_props.write_wo_resp = 1;
    char_md.p_char_user_desc         = NULL;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    ble_uuid.type = p_nus->uuid_type;
    ble_uuid.uuid = BLE_UUID_NUS_TX_CHARACTERISTIC;

    memset(&attr_md, 0, sizeof(attr_md));

    //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
   	BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&attr_md.read_perm); //jyj
	BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&attr_md.write_perm); 

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 1;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_NUS_MAX_TX_CHAR_LEN;

    return sd_ble_gatts_characteristic_add(p_nus->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_nus->tx_handles);
}

/************************************************************************
name:ble transmission data serive for hardware
**************************************************************************/
void ble_TxRx_on_ble_evt(ble_nus_t * p_nus, ble_evt_t * p_ble_evt)
{
    if ((p_nus == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
                                    Ble_ConnnectFlag=BLE_connect;    
                                    on_connect_TxRx(p_nus, p_ble_evt);
         break;

        case BLE_GAP_EVT_DISCONNECTED:
                                    Ble_ConnnectFlag=BLE_Disconnect;
                                    on_disconnect_TxRx(p_nus, p_ble_evt);
        break;

        case BLE_GATTS_EVT_WRITE:
                                    on_write_TxRx(p_nus, p_ble_evt);
                                    __nop();
        break;

        default:
            // No implementation needed.
         break;
    }
}


/************************************************************************
ble dtat transmission init function
**************************************************************************/
uint32_t ble_TxRx_init(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init)
{
    uint32_t      err_code;
    ble_uuid_t    ble_uuid;
    ble_uuid128_t nus_base_uuid = NUS_BASE_UUID;

    if ((p_nus == NULL) || (p_nus_init == NULL))
    {
        return NRF_ERROR_NULL;
    }

    // Initialize the service structure.
    p_nus->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_nus->data_handler            = p_nus_init->data_handler;
    p_nus->is_notification_enabled = false;

    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&nus_base_uuid, &p_nus->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    ble_uuid.type = p_nus->uuid_type;
    ble_uuid.uuid = BLE_UUID_NUS_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_nus->service_handle);
    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the RX Characteristic.
    err_code = rx_char_add(p_nus, p_nus_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the TX Characteristic.
    err_code = tx_char_add(p_nus, p_nus_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}


/***********************************************************
name:ble send data funciton
************************************************************/
uint32_t ble_TXRX_string_send(ble_nus_t * p_nus, uint8_t * p_string, uint16_t length)
{
    ble_gatts_hvx_params_t hvx_params;

    if (p_nus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    if ((p_nus->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_nus->is_notification_enabled))
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (length > BLE_NUS_MAX_DATA_LEN)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_nus->rx_handles.value_handle;
    hvx_params.p_data = p_string;
    hvx_params.p_len  = &length;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;

    return sd_ble_gatts_hvx(p_nus->conn_handle, &hvx_params);
}

void ble_txrx_Send(uint8_t * p_string, uint16_t length)
{
	ble_TXRX_string_send(&m_TXRX, p_string, length);
}

extern void phone_to_ble_data_parse(u8 * buf, u16 buf_len);
extern u8 Apdufinsh;
/*******************************************************************
name:ble recive data event function
p_data data recive bufer,length recive length
*********************************************************************/
void BLE_TXRX_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{

		Recivedataflag=1;                       //have recive data,the flag been set
	  phone_to_ble_data_parse(p_data,length);   //analysis recive data

}
