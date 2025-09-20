/**
 * @file tms_adc.c
 * @brief source file for the tms570 adc driver.
 */



/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include "tms_adc.h"
#include "obc_hardwaredefs.h"
#include "obc_utils.h"
#include "rtos.h"
#include "logger.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define MUTEX_TIMEOUT_MS 500
#define CONVERSION_TIMEOUT_TICKS 5U
#define TMS_ADC_MAX_RAW_VALUE 4096
#define TMS_UPPER_REF_VOLTAGE_MV 3300


/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static adcData_t adc_data[ADC_MAX_CHANNELS];
static SemaphoreHandle_t adc_Mutex;


/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the mutex.
 *
 */
void tms_adc_pre_init(void) {
    static StaticSemaphore_t adc_buffer;
    adc_Mutex = xSemaphoreCreateMutexStatic(&adc_buffer);
}

/**
 * @brief Initializes the mutex. This function also initializes the adc with adcInit()
 *
 */
void tms_adc_init_hw(void) {
    adcInit();
}

/**
 * @brief updates the channel values by starting a conversion and then returns a specific channel's voltage
 *
 * @param[in] adc_module a pointer to an adc group i.e. adcREG1, adcREG2
 * @param[in] channel a channel. Check the launchpad documentation for valid channel numbers
 * @param[in] channel_val_raw an output pointer that stores the raw value (12 bit resolution) of the voltage at the channel. Only valid
 *                        if ADC_SUCCESS was returned
 *
 * @return a status from the adc_err enum. ADC_SUCCESS if the conversion succeeded, ADC_MUTEX_ERROR if there was a mutex
 *          error and ADC_NO_CHANNEL_ID if the channel is incorrect
 */
adc_err_t tms_adc_channel_read(adcBASE_t *adc_module, uint32_t channel, uint16_t *channel_val_raw) {

    bool adc_timeout = false;

    //First update the channel values
    if (xSemaphoreTake(adc_Mutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS)) == pdTRUE) {

        adcStartConversion(adc_module, adcGROUP1);
        uint32_t start = xTaskGetTickCount();

        while (!adcIsConversionComplete(adc_module, adcGROUP1)) {
            if ((uint32_t)(xTaskGetTickCount() - start) > CONVERSION_TIMEOUT_TICKS) { // ADC Timeout
                LOG_ADC__CONVERSION_TIMEOUT();
                adc_timeout = true;
            }
        }

        //update data
        adcGetData(adc_module, adcGROUP1, &adc_data[0]);

    } else {
        LOG_ADC__MUTEX_ERROR();
        return ADC_MUTEX_ERROR;
    }

    xSemaphoreGive(adc_Mutex);

    if (adc_timeout) {
        return ADC_CONVERSION_TIMEOUT;
    }

    //Find the channel value
    for (int i = 0; i < ADC_MAX_CHANNELS; i++) {
        if (adc_data[i].id == channel) {
            *channel_val_raw = adc_data[i].value; //assign output argument pointer
            return ADC_SUCCESS;
        }
    }

    LOG_ADC__NO_ID();
    return ADC_NO_CHANNEL_ID;

}

/**
 * @brief Calls tms_adc_channel_read and converts the raw, 12 bit resolution voltage value to millivolts.
 *
 * @param[in] adc_module a pointer to an adc group i.e. adcREG1, adcREG2
 * @param[in] channel a channel. Check the launchpad documentation for valid channel numbers
 * @param[in] channel_val_raw an output pointer that stores the raw value of the voltage at the channel. Only valid if ADC_SUCCESS was returned
 * @param[in] converted_voltage an output pointer that stores voltage at the channel in mV. Only valid if ADC_SUCCESS was returned
 *
 *
 * @return a status from the adc_err enum. ADC_SUCCESS if the conversion succeeded, ADC_MUTEX_ERROR if there was a mutex
 *          error and ADC_NO_CHANNEL_ID if the channel is incorrect
 */
adc_err_t tms_adc_read_millivolts(adcBASE_t *adc_module, uint32_t channel, uint16_t *channel_val_raw, uint16_t *converted_voltage) {

    adc_err_t err = tms_adc_channel_read(adc_module, channel, channel_val_raw);
    *converted_voltage = (TMS_UPPER_REF_VOLTAGE_MV * (*channel_val_raw + 0.5)) / TMS_ADC_MAX_RAW_VALUE; //Formula from TMS570 reference manual

    return err;

}
