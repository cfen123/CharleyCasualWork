/*
 * led_blink.h
 *
 *  Testing the microcontroller by blinking the on-board LED on LAUNCHXL-F28379D
 *
 *  Created on: 5 Aug 2023
 *      Author: Charley Shi
 */

#ifndef PERIPHERALS_INCLUDE_LED_BLINK_H_
#define PERIPHERALS_INCLUDE_LED_BLINK_H_

#define RED_LED 34 // Red LED is on GPIO34
#define BLUE_LED 31 // Blue LED is on GPIO31

#define LED_TOGGLE_FREQUENCY 1

void led_blink_init();
interrupt void blink_led();

#endif /* PERIPHERALS_INCLUDE_LED_BLINK_H_ */
