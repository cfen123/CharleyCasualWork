/*
 * config_adc.c
 *
 *  Created on: 16 Jan 2023
 *      Author: Charley Shi
 *
 *
 *  Note: Based on driverlib adc.h, the temperature sensor is only supported by external reference. We want
 *  to use the external reference anyway due to its improved accuracy.
 */

#include "adcs.h"
#include "timers.h"


void ConfigAdcs(void) {
    // Add code here later
    asm(" RPT #125 || NOP"); // Delay for 500us (125 clock cycles at SYSCLK = 25MHz) recommended by TRM
}

