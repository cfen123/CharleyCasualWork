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
#include <stdint.h>

void ConfigPwm(); // Initializes all PWM modules

typedef enum PWM_Count_Mode {
    SYMMETRICAL_PWM, // Up-down count
    DOWN_COUNT_PWM, // Down count
    UP_COUNT_PWM // Up count
} PWMCountMode;

#define HALFCYCLE_DB_CLOCKING_ENABLE 0 // 0 = Full Cycle clocking for dead band counters; 1 = Half Cycle clocking for dead band counters

class HalfBridgePWM {
    private:
        uint32_t base;
        uint32_t timer_top; // For setting duty cycle
        PWMCountMode countMode;
        uint32_t tbclk_Hz;

        void configClock(uint32_t frequency_Hz);
        void configActionQualifiers();
        void configDeadBand(uint32_t dead_time_ns);

    public:
        HalfBridgePWM(EPWM_Module module, uint32_t frequency_Hz, PWMCountMode count_mode, float dead_time_ns);
        void setDutyCycle(float D);
};

// Global PWM modules for each phase. Making these global allows other files to update their duty cycles.
extern HalfBridgePWM *phaseA;
extern HalfBridgePWM *phaseB;
extern HalfBridgePWM *phaseC;

#endif /* PERIPHERALS_INCLUDE_PWM_H_ */
