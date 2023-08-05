/*
 * adc.h
 *
 *  Created on: 13/12/2022
 *      Author: Charley Shi
 *
 *      ADC implementation
 */

#ifndef CONFIG_ADC_H_
#define CONFIG_ADC_H_

#include "system_config.h"
#include <stdint.h>

#define ACQUISITION_WINDOW 200 // Acquisition time in SYSCLK cycles
#define ADC_VREF 3.3
#define EXT_SIG_SAMP_FREQ 1.3 // Sampling frequency of the external signal
#define TEMP_SENSE_SAMP_FREQ 1 // Sampling frequency of the temperature sensor

void ConfigAdcs(void);

#endif /* CONFIG_ADC_H_ */
