
/**
 * main.c
 *
 * This generates three sinusoids using PWM. The PWM will be filtered using a low pass
 * filter to give the sinusoids.
 */

#include "threephasegen.h"

int main(void) {
    ConfigSystem();
    ConfigThreePhaseGen();
    EnableInterrupts(); // Enables global interrupts

    while (1) {
        IDLE; // Execute the IDLE instruction to make the CPU go into sleep mode.
    }
}
