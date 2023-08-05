/*
 * cputimers.c
 *
 *  Created on: 20 Jan 2023
 *      Author: Charley Shi
 */

#include <timers.h>

Timer::Timer(TimerNumber index, uint32_t periodCount, uint16_t clkDivFactor) {
    base = index;
    CPUTimer_setConfig(base, periodCount, clkDivFactor);
    CPUTimer_startTimer(base); // Start timer. Not actually necessary since timer starts automatically by default.
}

void Timer::configInterrupt(void (*handler)(void)) {
    CPUTimer_configInterrupt(base, handler);
}

/*** Interrupt Service Routines
 * (Template code, not necessarily used)
 * ***/

interrupt void Timer0ISR(void) {
    // Enter code here
    PieCtrlRegs.PIEACK.bit.ACK1 = 1; // Acknowledge interrupt on group 1. Clear PIEACK bit for INT1.7 (interrupt group 1)
}

interrupt void Timer1ISR(void) {
    // Enter code here

    // Don't need to acknowledge since Timer1 interrupt doesn't go through PIE
}

interrupt void Timer2ISR(void) {
    // Enter code here

    // Don't need to acknowledge since Timer2 interrupt doesn't go through PIE
}
