#ifndef __INCLUDE_ALL_h
#define __INCLUDE_ALL_h

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "nrf_drv_wdt.h"

#include "ble_conn_params.h"
#include "nrf_gpio.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_uart.h"
#include "device_manager.h"
#include "pstorage.h"
#include "app_trace.h"
#include "app_scheduler.h"
#include "../components/drivers_nrf/hal/nrf_delay.h"
#include "../components/drivers_nrf/hal/nrf_adc.h"
#include "app_scheduler.h"
#include "../components/drivers_nrf/spi_master/spi_master.h"
#include "../components/drivers_nrf/twi_master/incubated/twi_master.h"
#include "../components/drivers_nrf/spi_master/spi_master.h"
#include "../components/ble/ble_db_discovery/ble_db_discovery.h"
#include "../components/ble/ble_db_discovery/ble_db_discovery.h"
#include "../components/ble/ble_services/ble_ancs_c/ble_ancs_c.h"
#include "../components/drivers_nrf/timer/nrf_drv_timer.h" 
#include "../components/drivers_nrf/ppi/nrf_drv_ppi.h"
#include "../components/drivers_nrf/gpiote/nrf_drv_gpiote.h" 

//////////////////////////////////////////DMWZ include///////////////////////////////////
#include "Timer_interrupt_fuction.h"
#include "uart_DMWZ.h"
#include "scheduler_DMWZ.h"
#include "ble_txrx_service.h"
#include "ble_dmwz.h"

#include "synaot.h"


#include "gpio_dmwz.h"
#include "timeclock.h"
#include "progam.h"
#include "closeoled.h"
#include "timedis.h"

#include "adc_dmwz.h"

#include "crc16_DMWZ.h"

#include "bluetoothkey_DMWZ.h"
#include "bluetooth_hwprotocol.h"
#include "bluetooth_appprotocol.h"

#include "flashmcu_DMWZ.h"
#include "appbond.h"
//////////////////////////////////////////////////////////////////////////////////////////
void TimeOut3Sec_program(void * p_event_data, uint16_t event_size);
#endif
