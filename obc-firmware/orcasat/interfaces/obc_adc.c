/*
 * obc_adc.c
 *
 *  Created on: Jun 4, 2018
 *      Author: Richard
 *
 *
 *  HALCoGEN Setup:
 * 		TMS570LS0714PGE > Driver Enable > Enable ADC Drivers (checked)
 * 		VIM Channel 0-31 >  15: ADC1 Group 1
 * 		ADC1 > ADC1 Group 1 Channel Selection > Enable Pin x
 */

#include "obc_adc.h"
#include "obc_hardwaredefs.h"
#include "obc_utils.h"
#include "tms_uart.h"
#include "sys_common.h"
#include "adc.h"
#include "rtos.h"
#include "logger.h"

adcData_t adc_data[ADC_MAX_CHANNELS];
SemaphoreHandle_t xADCMutex;

/**
 * @brief HAL initialize ADC, create mutex for protection since we poll for completion
 */
void adc_init(void) {
    static StaticSemaphore_t xADCMutexBuffer;

    adcInit();
    xADCMutex = xSemaphoreCreateMutexStatic(&xADCMutexBuffer);
}

/**
 * @brief Get ADC reading. Search through the data array until we hit the correct ID (channel number = 0 based)
 * @note The results from the ADC aren't necessarily in order or at the same position as the channel #
 * @pre adc_update() has been called
 *
 * @return	raw value from desired ADC channel, else 0 (if ADC channel is not found).
 */
uint16_t adc_get_channel_val(uint8_t adc_chan) {
    uint8_t i;
    for (i = 0; i < ADC_MAX_CHANNELS; i++) {
        if (adc_data[i].id == adc_chan) {
            return adc_data[i].value;
        }
    }
    log_str(ERROR, LOG_ADC, true, "No ADC ID");
    return 0;
}

/**
 * @brief Updates the global ADC result struct with the most recent values
 * @note Upon calling this, you can access the values from the channels you desire
 *
 * @return True if updated correctly, False otherwise
 */
bool adc_update(void) {
    bool adc_ok = true;
    if (xSemaphoreTake(xADCMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
        uint32_t start = xTaskGetTickCount();
        adcStartConversion(adcREG1, adcGROUP1); // sample all channels on ADC1
        while (!adcIsConversionComplete(adcREG1, adcGROUP1)) {
            if ((uint32_t)(xTaskGetTickCount() - start) > 5U) { // ADC Timeout
                log_str(ERROR, LOG_ADC, true, "ADC Conversion Timeout");
                adc_ok = false;
                break;
            }
        }
        adcGetData(adcREG1, adcGROUP1, &adc_data[0]);
    } else {
        log_str(ERROR, LOG_ADC, false, "ADC Mutex Error");
        adc_ok = false;
    }
    xSemaphoreGive(xADCMutex);
    return adc_ok;
}
