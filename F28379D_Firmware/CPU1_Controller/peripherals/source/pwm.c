/*
 * pwm.c
 *
 *  Created on: 20 Jan 2023
 *      Author: Charley Shi
 */

#include "pwm.h"
#include <math.h>

/* Configures all EPWM modules and enables the time base clocks */
void ConfigPwm() {
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC); // Enable time base clocks for all ePWM modules
}

/* Configures an ePWM module for driving a half-bridge using active-high complementary (AHC) PWM with dead time
 *
 * Output A: High side of half-bridge controlled by compare match A
 * Output B: Low side of half-bridge complementary to output A
 *
 * \param module is the module to configure (format: EPWMx for ePWMx)
 *
 * Either up-down (phase correct) PWM or up count (fast PWM) are used depending on the configuration in pwm.h.
 *
 * This function:
 * - Enables the clock to the EPWMx peripheral
 * - Sets the time base clock prescaler (edit this function to match the PRESCALER macro in pwm.h)
 * - Sets the time base counter top value calculated using parameters in pwm.h
 * - Configures the action qualifiers for either up-down count or up-count (non-inverting mode)
 * - Configures the dead band (DB) submodule with AHC and dead times as defined in pwm.h
 *
 * Note that module = 0 for ePWM1, 1 for ePWM2, etc.
 */
void ConfigHalfBridgePWM(EPWM_Module module) {
    EPWM_enableModule(module, EPWM_OUTPUT_A_B); // Call the function I defined in the ePWM DriverLib file
    uint32_t base = EPWM1_BASE + module * 0x00000100U; // Base address of registers. See <inc/hw.memmap.h> in driverlib

    /* Configure time base clock */
    EPWM_setClockPrescaler(base, EPWM_CLOCK_DIVIDER_2, EPWM_HSCLOCK_DIVIDER_1); // Net prescaler of /2
    EPWM_setTimeBasePeriod(base, (Uint16)EPWM_COUNTER_TOP);

    if (SYMMETRICAL_PWM) { // Up-down count mode, non-inverting
        EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP_DOWN); // Up-down count mode for phase-correct PWM

        /* Configure action qualifiers */
        // Set on bottom (zero), clear on compare match when up counting, clear on TOP, set on compare match when down counting
        EPWM_setActionQualifierActionComplete(base, EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_OUTPUT_HIGH_ZERO | EPWM_AQ_OUTPUT_LOW_UP_CMPA
                                              | EPWM_AQ_OUTPUT_LOW_PERIOD | EPWM_AQ_OUTPUT_HIGH_DOWN_CMPA);
    }
    else { // Single ended (up count) mode, non-inverting
        EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP_DOWN); // Up-down count mode for phase-correct PWM

        /* Configure action qualifiers */
        // Set on bottom (zero), clear on compare match when up counting
        EPWM_setActionQualifierActionComplete(base, EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_OUTPUT_HIGH_ZERO | EPWM_AQ_OUTPUT_LOW_UP_CMPA);
    }

    /* Configure dead band (DB) submodule */
    Uint16 FED_count, RED_count;
    if (HALFCYCLE_DB_CLOCKING_ENABLE) {
        EPWM_setDeadBandCounterClock(base, EPWM_DB_COUNTER_CLOCK_HALF_CYCLE);
        FED_count = (Uint16)(2 * DEAD_BAND_FED_US * powf(10, -6) * TBCLK_FREQ); // Value to load DBFED register with
        RED_count = (Uint16)(2 * DEAD_BAND_RED_US * powf(10, -6) * TBCLK_FREQ); // Value to load DBRED register with
    }
    else {
        EPWM_setDeadBandCounterClock(base, EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);
        FED_count = (Uint16)(DEAD_BAND_FED_US * powf(10, -6) * TBCLK_FREQ); // Value to load DBFED register with
        RED_count = (Uint16)(DEAD_BAND_RED_US * powf(10, -6) * TBCLK_FREQ); // Value to load DBRED register with
    }

    // See <inc/hw_epwm.h> for the register address shifts and bit shifts
    HWREGH(base + EPWM_O_DBCTL) |= (0b10 << EPWM_DBCTL_POLSEL_S); // Active high complementary mode (EPWMxB is EPWMxA inverted)
    HWREGH(base + EPWM_O_DBCTL) |= (0b11 << EPWM_DBCTL_OUT_MODE_S); // Fully enable DB submodule
    // Assign RED and FED count values
    EPWM_setRisingEdgeDelayCount(base, FED_count); // Loads FED_COUNT into DBFED register
    EPWM_setFallingEdgeDelayCount(base, RED_count); // Loads RED_COUNT into DBRED register
}
