/**
 * @file tms_adc.h
 * @brief header file for tms_adc.c
 */

#ifndef TMS_ADC_H
#define TMS_ADC_H

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "sys_common.h"
#include "adc.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define ADC_MAX_CHANNELS 24

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    ADC_SUCCESS = 0,
    ADC_MUTEX_ERROR = 1,
    ADC_NO_CHANNEL_ID = 2,
    ADC_CONVERSION_TIMEOUT = 3,
} adc_err_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

adc_err_t tms_adc_channel_read(adcBASE_t *adc_module, uint32_t channel, uint16_t *channel_val_raw);
adc_err_t tms_adc_read_millivolts(adcBASE_t *adc_module, uint32_t channel, uint16_t *channel_val_raw, uint16_t *converted_voltage);
void tms_adc_init_hw(void);
void tms_adc_pre_init(void);

#endif /*TMS_ADC_H*/
