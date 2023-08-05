

/**
 * main.c
 */

extern "C" {
    #include <system_config.h> // Also includes f2837xD includes and driverlib
}

#include "pwm.h"
#include "led_blink.h"

int main(void) {
    ConfigSystem();
    led_blink_init();
    ConfigPwm();
    EnableInterrupts();

    phaseA->setDutyCycle(0.2);
    phaseB->setDutyCycle(0.5);
    phaseC->setDutyCycle(0.8);

    while (1) {
        IDLE; // Sleep
    }
}
