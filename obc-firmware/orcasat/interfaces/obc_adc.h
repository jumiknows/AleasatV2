/*
 * obc_adc.h
 *
 *  Created on: Jun 4, 2018
 *      Author: Richard
 */

#ifndef OBC_ADC_H_
#define OBC_ADC_H_
#include "sys_common.h"

void adc_init(void);                            /* Initialize ADC and create mutex */
bool adc_update(void);                          /* Trigger an ADC update sequence (reads new values) */
uint16_t adc_get_channel_val(uint8_t adc_chan); /* Grab a particular ADC channel's value */

#define ADC_MAX_CHANNELS 24

#endif /* OBC_ADC_H_ */
