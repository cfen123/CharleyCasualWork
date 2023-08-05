/*
 * led_blink.cpp
 *
 *  Created on: 5 Aug 2023
 *      Author: Charley Shi
 */

#include "led_blink.h"
#include "system_config.h"
#include "timers.h"

void led_blink_init() {
    // Set LED pins as outputs
    GPIO_setDirectionMode(RED_LED, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(BLUE_LED, GPIO_DIR_MODE_OUT);
    GPIO_setPin(RED_LED); // Also make the red one inverted from the blue one

    // Initialise Timer 1 with the correct period and prescaler of 1.
    uint32_t timer_top = (uint32_t)((float)PLLSYSCLK/LED_TOGGLE_FREQUENCY) - 1;
    Timer timer1(TIMER1, timer_top, 1);
    timer1.configInterrupt(blink_led);
}

interrupt void blink_led() {
    GPIO_togglePin(RED_LED);
    GPIO_togglePin(BLUE_LED);
}
