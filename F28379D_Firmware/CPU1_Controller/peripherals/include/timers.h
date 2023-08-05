/*
 * cputimers.h
 *
 *  Created on: 20 Jan 2023
 *      Author: Charley Shi
 *
 *  Contents:
 *  - Configuring the CPU Timers 0, 1 and 2
 *  - ISRs for each timer
 *
 *  Driver code for timers.
 */

#ifndef PERIPHERALS_INCLUDE_TIMERS_H_
#define PERIPHERALS_INCLUDE_TIMERS_H_

#include "system_config.h"
#include <stdint.h>

typedef enum {
    TIMER0 = CPUTIMER0_BASE,
    TIMER1 = CPUTIMER1_BASE,
    TIMER2 = CPUTIMER2_BASE
} TimerNumber;

class Timer {
private:
    uint32_t base;

public:
    Timer(TimerNumber index, uint32_t periodCount, uint16_t clkDivFactor);
    void configInterrupt(void (*handler)(void));
};

/* Interrupt Service Routines
 * (Template code, does not need to be used)
 * */
interrupt void Timer0ISR(void);
interrupt void Timer1ISR(void);
interrupt void Timer2ISR(void);

#endif /* PERIPHERALS_INCLUDE_TIMERS_H_ */
