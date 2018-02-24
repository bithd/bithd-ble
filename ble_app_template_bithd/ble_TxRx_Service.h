#ifndef BLE_TXRX_SEVERICE_H__
#define BLE_TXRX_SEVERICE_H__

#define BLE_UUID_NUS_SERVICE 0x0001                                //transmission service UUID
#define BLE_NUS_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3)           //transmission max data size

#define BLE_UUID_NUS_TX_CHARACTERISTIC 0x0002                      /**< send character UUID*/
#define BLE_UUID_NUS_RX_CHARACTERISTIC 0x0003                      /**< recive character UUID*/

#define BLE_NUS_MAX_RX_CHAR_LEN        BLE_NUS_MAX_DATA_LEN        /**< Maximum length of the RX Characteristic (in bytes). */
#define BLE_NUS_MAX_TX_CHAR_LEN        BLE_NUS_MAX_DATA_LEN        /**< Maximum length of the TX Characteristic (in bytes). */

#define NUS_BASE_UUID                  {{0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */


typedef struct ble_nus_s ble_nus_t;

/*define service callback funciton*/
typedef void (*ble_nus_data_handler_t) (ble_nus_t * p_nus, uint8_t * p_data, uint16_t length);

/**@brief Nordic UART Service initialization structure.
 *
 * @details This structure contains the initialization information for the service. The application
 * must fill this structure and pass it to the service using the @ref ble_nus_init
 *          function.
 */
typedef struct
{
    ble_nus_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_nus_init_t;

/**@brief Nordic UART Service structure.
 *
 * @details This structure contains status information related to the service.
 */
struct ble_nus_s
{
    uint8_t                  uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the S110 SoftDevice). */
    ble_gatts_char_handles_t tx_handles;              /**< Handles related to the TX characteristic (as provided by the S110 SoftDevice). */
    ble_gatts_char_handles_t rx_handles;              /**< Handles related to the RX characteristic (as provided by the S110 SoftDevice). */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the S110 SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    bool                     is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    ble_nus_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
};


extern ble_nus_t      m_TXRX;       
extern void ble_TxRx_on_ble_evt(ble_nus_t * p_nus, ble_evt_t * p_ble_evt);
extern uint32_t ble_TxRx_init(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init);
extern void BLE_TXRX_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length);
void ble_txrx_Send(uint8_t * p_string, uint16_t length);
#endif
