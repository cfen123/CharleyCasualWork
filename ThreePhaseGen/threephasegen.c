/*
 * threephasegen.c
 *
 *  Created on: 27/07/2023
 *      Author: Charley Shi
 *
 *  EPWM4A = Phase A
 *  EPWM5A = Phase B
 *  EPWM6A = Phase C
 *
 *  The duty cycles will be updated via timer interrupts which occur at the sampling frequency.
 */

#include "threephasegen.h"
#include <math.h>

static float sinusoidDutyCycles[(Uint16)N_SAMPLES]; // Array stores all the duty cycles for each sample. Assumes zero phase.

// Initialise these to include phase information.
Uint16 PhaseA_Index = (Uint16)((float)PHASEA_PHASE_DEGREES/360.0 * N_SAMPLES); // Index of sinusoidDutyCycles for duty cycle of phase A
Uint16 PhaseB_Index = (Uint16)((float)PHASEB_PHASE_DEGREES/360.0 * N_SAMPLES); // Index of sinusoidDutyCycles for duty cycle of phase B
Uint16 PhaseC_Index = (Uint16)((float)PHASEC_PHASE_DEGREES/360.0 * N_SAMPLES); // Index of sinusoidDutyCycles for duty cycle of phase C

void ConfigThreePhaseGen() { // Configures everything using the other functions
    // Initialise the sinusoidDutyCycles array
    int i;
    for (i = 0; i < N_SAMPLES; i++) {
        sinusoidDutyCycles[i] = 0.5 + 0.5*sin(i*2*M_PI/N_SAMPLES); // 0.5*sin(2pi*i/N) + 0.5
    }

    ConfigTimer();

    // Configure PWMs
    ConfigEpwmPhase(EPWM4); // Phase A
    ConfigEpwmPhase(EPWM5); // Phase B
    ConfigEpwmPhase(EPWM6); // Phase C
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC); // Enable time base clocks for all ePWM modules
}

void ConfigTimer() {
    Uint32 base = CPUTIMER1_BASE; // Use Timer 1
    Uint32 timer_top = PLLSYSCLK/SAMPLING_FREQUENCY - 1; // Top value of timer with /1 prescaler
    CPUTimer_setConfig(base, timer_top, 1); // /1 prescaler
    CPUTimer_startTimer(base); // Start timer. Not actually necessary since timer starts automatically by default.
    CPUTimer_configInterrupt(base, updateDutyCycles); // Assign the ISR as updateDutyCycles()
}

void ConfigEpwmPhase(EPWM_Module module) {
    // Do not enable the output here. Enable it when the mode is changed to 'PWM'
    EPWM_enableModule(module, EPWM_OUTPUT_A); // Call the function I defined in the ePWM DriverLib file.
    uint32_t base = EPWM1_BASE + module * 0x00000100U; // Base address of registers. See <inc/hw.memmap.h> in driverlib

    /* Configure time base clock */
    EPWM_setClockPrescaler(base, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1); // Net prescaler of /1
    uint32_t EPWM_clock = PLLSYSCLK; // Prescaled clock
    uint32_t timer_top = EPWM_clock/PWM_FREQUENCY - 1;
    EPWM_setTimeBasePeriod(base, (Uint16)timer_top);

    EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP);

    /* Configure action qualifiers */
    // Set on bottom (zero), clear on compare match when up counting
    EPWM_setActionQualifierActionComplete(base, EPWM_AQ_OUTPUT_A,
                                          EPWM_AQ_OUTPUT_HIGH_ZERO | EPWM_AQ_OUTPUT_LOW_UP_CMPA);

}

interrupt void updateDutyCycles() { // This is the timer interrupt
    // Update indices
    PhaseA_Index = incrementIndex(PhaseA_Index);
    PhaseB_Index = incrementIndex(PhaseB_Index);
    PhaseC_Index = incrementIndex(PhaseC_Index);

    // Update duty cycles
    updatePhaseA_Duty(sinusoidDutyCycles[PhaseA_Index]);
    updatePhaseB_Duty(sinusoidDutyCycles[PhaseB_Index]);
    updatePhaseC_Duty(sinusoidDutyCycles[PhaseC_Index]);
}

Uint16 incrementIndex(Uint16 num) {
    if (num < N_SAMPLES - 1) {
        return num + 1;
    }
    else {
        return 0;
    }
}
