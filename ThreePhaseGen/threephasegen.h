/* This file contains the timer and PWM code
 *
 *  EPWM4A = Phase A
 *  EPWM5A = Phase B
 *  EPWM6A = Phase C
 *
 *  The duty cycles will be updated via timer interrupts which occur at the sampling frequency.
 */

/** Macros **/
#ifndef THREEPHASEGEN_H
#define THREEPHASEGEN_H

#include <f28002x_device.h>
#include <driverlib.h>
#include <system_config.h>

#define PWM_FREQUENCY 100000

#define SINUSOID_FREQUENCY 50 // Frequency of the sinusoids to generate
#define SAMPLING_FREQUENCY 50000 // How frequently the duty cycle in the PWM is updated.
#define N_SAMPLES (SAMPLING_FREQUENCY/SINUSOID_FREQUENCY) // Number of samples per period of the sinusoid

#define PHASEA_PHASE_DEGREES 0 // Phase A is 0 degrees phase
#define PHASEB_PHASE_DEGREES 120 // Phase B is 120 degrees phase
#define PHASEC_PHASE_DEGREES 240 // Phase C is 240 degrees phase

/** Functions **/
void ConfigThreePhaseGen(); // Configures everything using the other functions
void ConfigTimer();
void ConfigEpwmPhase(EPWM_Module module); // Configures output A on the EPWM module given by module.
interrupt void updateDutyCycles(); // This is the timer interrupt
Uint16 incrementIndex(Uint16 num); // Helper function for ISR

/* Functions for updating duty cycle */
static inline void updatePhaseA_Duty(float D) {
    EPwm4Regs.CMPA.bit.CMPA = (Uint16)(D * EPwm4Regs.TBPRD);
}

static inline void updatePhaseB_Duty(float D) {
    EPwm5Regs.CMPA.bit.CMPA = (Uint16)(D * EPwm5Regs.TBPRD);
}

static inline void updatePhaseC_Duty(float D) {
    EPwm6Regs.CMPA.bit.CMPA = (Uint16)(D * EPwm6Regs.TBPRD);
}

#endif
