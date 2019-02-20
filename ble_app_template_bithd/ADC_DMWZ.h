#ifndef _ADC_DMWZ_H_
#define _ADC_DMWZ_H_

#define CHG_EN_pin     24     //charging IC enable io,set output,L enable IC,H disable IC
#define CHG_Status_pin 18     //charging states read io,set input.when battry charging full power,the ic output H voltage

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200           /**< Reference voltage (in  milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION         1              /**< The ADC is configured to use VDD with no prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define ADC_HW_PRE_SCALING_COMPENSATION      4

#define BATTERY_ADC_RESOLUTION  (ADC_CONFIG_RES_10bit)
#define BATTERY_ADC_DIV         (1023)

#define Full_Voltage             (4100)                     //Full power 4.2V
#define Warning_Voltage          (3650)                     //warning    3.8V
#define Lowest_Voltage           (3500)                     //NO power
#define ADV_BLE_VOLTAGE			 (3450)
#define DIS_BLE_VOLTAGE			 (3400)

#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS)  * ADC_PRE_SCALING_COMPENSATION * ADC_HW_PRE_SCALING_COMPENSATION)/ BATTERY_ADC_DIV)
				
extern volatile int32_t adc_sample;
extern unsigned char FullVoltage_flag;

void battery_adc_read_once(uint16_t* batt_lvl_in_milli_volts);
void adc_config(void);
void ReadAdc(void * p_event_data, uint16_t event_size);

void Chargingandmanagerdisplay(void * p_event_data, uint16_t event_size);
#endif
