/*
 * pwm.cpp
 *
 *  Created on: 20 Jan 2023
 *      Author: Charley Shi
 *
 *  Phase A PWM is on EPWM1 (GPIO0 and GPIO1)
 *  Phase B PWM is on EPWM2 (GPIO2 and GPIO3)
 *  Phase C PWM is on EPWM3 (GPIO4 and GPIO5)
 *
 *  Using C++ 'new' operator: https://www.geeksforgeeks.org/new-vs-operator-new-in-cpp/
 */

#include "pwm.h"
#include <math.h>

// Global PWM modules for each phase. Making these global allows other files to update their duty cycles.
HalfBridgePWM *phaseA;
HalfBridgePWM *phaseB;
HalfBridgePWM *phaseC;

// PWM parameters which will be shared between the modules
const uint32_t PWM_frequency_Hz = 100000;
const PWMCountMode count_mode = SYMMETRICAL_PWM;
const float dead_time_ns = 100.0;

/* Configures all EPWM modules and enables the time base clocks */
void ConfigPwm() {
    phaseA = new HalfBridgePWM(EPWM1, PWM_frequency_Hz, count_mode, dead_time_ns);
    phaseB = new HalfBridgePWM(EPWM2, PWM_frequency_Hz, count_mode, dead_time_ns);
    phaseC = new HalfBridgePWM(EPWM3, PWM_frequency_Hz, count_mode, dead_time_ns);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC); // Enable time base clocks for all ePWM modules
}

/* Configures the PWM module with active high complementary PWM for driving half bridges.
 *
 * \param module is the EPWM module (EPWM1, EPWM2,...,EPWM12) to use.
 * \param frequency_Hz is the frequency of the PWM
 * \param countMode determines whether the PWM is up counting, down counting or symmetrical
 * \param dead_time_ns sets the dead time in ns
 *
 * A clock prescaler of /2 is used for TBCLK
 *
 * */
HalfBridgePWM::HalfBridgePWM(EPWM_Module module, uint32_t frequency_Hz, PWMCountMode count_mode, float dead_time_ns) {
    // Initialise fields
    base = EPWM1_BASE + module * 0x00000100U; // Base address of registers. See <inc/hw.memmap.h> in driverlib
    tbclk_Hz = (uint32_t)(PLLSYSCLK/2); // Prescaler of 2
    countMode = count_mode;

    EPWM_enableModule(module, EPWM_OUTPUT_A_B); // Enable the ePWM module and its outputs (warning: GPIO settings won't work for outside GPIO0-23)

    // Configure the module
    configClock(frequency_Hz);
    configActionQualifiers();
    configDeadBand(dead_time_ns);
}

/* Updates the duty cycle of the PWM */
void HalfBridgePWM::setDutyCycle(float D) {
    Uint16 compare_value = (Uint16)(D*timer_top);
    HWREGH(base + EPWM_O_CMPA + 1) = compare_value; // Write to the CMPA register (no EALLOW protection). '+1' since it's the high word of a 32-bit register
}

/* Configures the ePWM clock with a prescaler of 2 */
void HalfBridgePWM::configClock(uint32_t frequency_Hz) {
    EPWM_setClockPrescaler(base, EPWM_CLOCK_DIVIDER_2, EPWM_HSCLOCK_DIVIDER_1); // Net prescaler of /2

    // Calculate the timer top value
    if (countMode == SYMMETRICAL_PWM) {
        timer_top = tbclk_Hz/(2*frequency_Hz);
    }
    else {
        timer_top = tbclk_Hz/frequency_Hz - 1;
    }

    EPWM_setTimeBasePeriod(base, (Uint16)timer_top);
}

/* Configures the action qualifiers for output A based on the count mode.
 * Note: Configuring the dead band submodule will automatically configure output B based on output A. */
void HalfBridgePWM::configActionQualifiers() {
    switch (countMode) {
    case SYMMETRICAL_PWM:
        // Up-down count mode, non-inverting
        EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP_DOWN);

        /* Configure action qualifiers */
        // Set on bottom (zero), clear on compare match when up counting, clear on TOP, set on compare match when down counting
        EPWM_setActionQualifierActionComplete(base, EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_OUTPUT_HIGH_ZERO | EPWM_AQ_OUTPUT_LOW_UP_CMPA
                                              | EPWM_AQ_OUTPUT_LOW_PERIOD | EPWM_AQ_OUTPUT_HIGH_DOWN_CMPA);
        break;

    case UP_COUNT_PWM:
        EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP);

        /* Configure action qualifiers */
        // Set on bottom (zero), clear on compare match when up counting
        EPWM_setActionQualifierActionComplete(base, EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_OUTPUT_HIGH_ZERO | EPWM_AQ_OUTPUT_LOW_UP_CMPA);
        break;

    case DOWN_COUNT_PWM:
        EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_DOWN);

        /* Configure action qualifiers */
        // Clear on bottom (zero), set on compare match when down counting
        EPWM_setActionQualifierActionComplete(base, EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_OUTPUT_LOW_ZERO | EPWM_AQ_OUTPUT_HIGH_DOWN_CMPA);
        break;
    }
}

/* Configures the dead band submodule. Can use half cycle for more resolution. Assumes that
 * the rising and falling edge delays (FED and RED) are the same. */
void HalfBridgePWM::configDeadBand(uint32_t dead_time_ns) {
    Uint16 FED_count, RED_count;
    if (HALFCYCLE_DB_CLOCKING_ENABLE) {
        EPWM_setDeadBandCounterClock(base, EPWM_DB_COUNTER_CLOCK_HALF_CYCLE);
        FED_count = (Uint16)(2 * dead_time_ns * powf(10, -9) * tbclk_Hz); // Value to load DBFED register with
        RED_count = (Uint16)(2 * dead_time_ns * powf(10, -9) * tbclk_Hz); // Value to load DBRED register with
    }
    else {
        EPWM_setDeadBandCounterClock(base, EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);
        FED_count = (Uint16)(dead_time_ns * powf(10, -9) * tbclk_Hz); // Value to load DBFED register with
        RED_count = (Uint16)(dead_time_ns * powf(10, -9) * tbclk_Hz); // Value to load DBRED register with
    }

    // See <inc/hw_epwm.h> for the register address shifts and bit shifts. Note that there is no EALLOW protection for this register
    HWREGH(base + EPWM_O_DBCTL) |= (0b10 << EPWM_DBCTL_POLSEL_S); // Active high complementary mode (EPWMxB is EPWMxA inverted)
    HWREGH(base + EPWM_O_DBCTL) |= (0b11 << EPWM_DBCTL_OUT_MODE_S); // Fully enable DB submodule

    // Assign RED and FED count values
    EPWM_setRisingEdgeDelayCount(base, FED_count); // Loads FED_COUNT into DBFED register
    EPWM_setFallingEdgeDelayCount(base, RED_count); // Loads RED_COUNT into DBRED register
}
