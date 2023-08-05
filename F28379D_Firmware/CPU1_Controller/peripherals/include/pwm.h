/*
 * pwm.h
 *
 *  Created on: 20 Jan 2023
 *      Author: Charley Shi
 *
 *  Code for PWM generation via the ePWM modules
 */

#ifndef PERIPHERALS_INCLUDE_PWM_H_
#define PERIPHERALS_INCLUDE_PWM_H_

#include "system_config.h"

void ConfigPwm(); // Initializes all PWM modules
void ConfigHalfBridgePWM(EPWM_Module module);

#define SYMMETRICAL_PWM 1 // 0 = Fast PWM (up count or down count mode); 1 = symmetrical PWM (up-down count mode)

// Parameters of the PWM common to all three phases
#define SWITCHING_FREQ 1000 // PWM frequency (Hz)
#define EPWMCLK_PRESCALER 2 // HSPCLKDIV * CLKDIV
#define TBCLK_FREQ (PLLSYSCLK/EPWMCLK_PRESCALER) // Time base clock frequency (Hz)
#if SYMMETRICAL_PWM == 1
#define EPWM_COUNTER_TOP (TBCLK_FREQ/(2*SWITCHING_FREQ)) // Assuming up-down count
#else
#define EPWM_COUNTER_TOP (TBCLK_FREQ/SWITCHING_FREQ - 1) // Assuming up count
#endif

// DBRED holds the number of TBCLK cycles for the rising edge delay (RED). Similar for falling edge delay (FED)
#define HALFCYCLE_DB_CLOCKING_ENABLE 1 // 0 = Full Cycle clocking for dead band counters; 1 = Half Cycle clocking for dead band counters
#define DEAD_BAND_RED_US 0.5 // Rising edge delay for dead band (us)
#define DEAD_BAND_FED_US 0.5 // Falling edge delay for dead band (us)


#endif /* PERIPHERALS_INCLUDE_PWM_H_ */
