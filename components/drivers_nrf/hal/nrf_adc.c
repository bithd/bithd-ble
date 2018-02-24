/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
 * @file
 * @brief ADC HAL implementation
 */

#include "nrf_adc.h"

/**
 * @brief Function for configuring ADC.
 *
 * This function power on ADC and configure it. ADC is in DISABLE state after configuration,
 * so it should be enabled before using it.
 *
 * @param[in] config is requested configuration
 */
void nrf_adc_configure(nrf_adc_config_t * config)
{
    uint32_t config_reg = 0;

    config_reg |= ((uint32_t)config->resolution << ADC_CONFIG_RES_Pos) & ADC_CONFIG_RES_Msk;
    config_reg |= ((uint32_t)config->scaling << ADC_CONFIG_INPSEL_Pos) & ADC_CONFIG_INPSEL_Msk;
    config_reg |= ((uint32_t)config->reference << ADC_CONFIG_REFSEL_Pos) & ADC_CONFIG_REFSEL_Msk;

    if (config->reference & ADC_CONFIG_EXTREFSEL_Msk)
    {
        config_reg |= config->reference & ADC_CONFIG_EXTREFSEL_Msk;
    }

    /* select input */
    nrf_adc_input_select(NRF_ADC_CONFIG_INPUT_DISABLED);

    /* set new configuration keeping selected input */
    NRF_ADC->CONFIG = config_reg | (NRF_ADC->CONFIG & ADC_CONFIG_PSEL_Msk);
}


/**
 * @brief Blocking function for executing single ADC conversion.
 *
 * This function selects desired input, starts single conversion,
 * waits for its finish  and returns result.
 * ADC is left in STOP state, given input is selected.
 * This function does not check if ADC is initialized and powered.
 *
 * @param[in] input is requested input to be selected
 *
 * @return conversion result
 */
int32_t nrf_adc_convert_single(nrf_adc_config_input_t input)
{
    int32_t val;

    nrf_adc_input_select(input);
    nrf_adc_start();
   
    while (!nrf_adc_conversion_finished())
    {
    }
    nrf_adc_conversion_event_clean();
    val = nrf_adc_result_get();
    nrf_adc_stop();
    return val;
		
		
		
		
//		   uint32_t adc_result = 0;

//    battery_start();
//    nrf_delay_ms(10);
//    while( !(NRF_ADC->EVENTS_END) ) {
//        nrf_delay_ms(10);
//    }

//    NRF_ADC->EVENTS_END     = 0;
//    adc_result              = NRF_ADC->RESULT;
//    NRF_ADC->TASKS_STOP     = 1;
//NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Disabled;
//    *batt_lvl_in_milli_volts = ((uint32_t)(ADC_RESULT_IN_MILLI_VOLTS(adc_result)) & 0xffff);
//    //when charging, battery voltage need adjust
//    if(charger_status()!= NoCharge) {
//        *batt_lvl_in_milli_volts -= BATTERY_VOLTAGE_ADJUSTMENT;
//    }
		
}

