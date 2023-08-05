

/**
 * main.c
 */

extern "C" {
    #include <system_config.h> // Also includes f2837xD includes and driverlib
}

#include "led_blink.h"

int main(void) {
    ConfigSystem();
    led_blink_init();
    EnableInterrupts();

    while (1) {
        IDLE; // Sleep
    }
}
