/*
 * clocks.h
 *
 *  Created on: 18/11/2022
 *      Author: Charley Shi
 *
 *  Functions and variables used to initialize and configure the system clock and interrupts.
 *
 *
 */

#ifndef INIT_H_
#define INIT_H_

#include <f28002x_device.h>

#define USE_PLL 1 // 0 = Not using PLL clock; 1 = Using PLL clock

/* MACROS */
#define DELAY_SIXTY_CYCLES asm(" RPT #60 || NOP")

// Clock scaling factors
#if USE_PLL == 1
#define PLL_IMULT 60 // Integer multiplier of 60
#define PLL_REFDIV 2 // Reference clock divider of 2 + 1 = 3
#define PLL_ODIV 3 // Output clock divider of 3 + 1 = 4
#endif
#define SYSCLKDIV 2 // System clock divider of /2
#define LSPCLKDIVIDER 2

/* Clock frequencies: With the internal 10MHz oscillator and the above clock scaling factors:
 * OSCCLK = 10MHz
 * PLLRAWCLK = 10MHz * 60 / ((2 + 1) * (3+1)) = 10MHz * 5 = 50MHz
 * PLLSYSCLK = 50MHz / 2 = 25MHz
 * LSPCLK = 25MHz / 2 = 12.5MHz
 *
 * Note that PLLRAWCLK is restricted to be below 120MHz as per the datasheet (pg 88)
 */
#define OSCCLK_FREQ_HZ 10000000UL // Oscillator frequency (Hz) for OSCCLK source chosen
#if USE_PLL == 1
#define PLLRAWCLK ((OSCCLK_FREQ_HZ * PLL_IMULT) / ((PLL_REFDIV + 1) * (PLL_ODIV + 1))) // Raw PLL clock frequency (Hz)
#define PLLSYSCLK (PLLRAWCLK / SYSCLKDIV) // SYSCLK (PLLSYSCLK) frequency (Hz)
#else
#define PLLSYSCLK (OSCCLK_FREQ_HZ / SYSCLKDIV) // SYSCLK (PLLSYSCLK) frequency (Hz)
#endif
#define LSPCLK (PLLSYSCLK / LSPCLKDIVIDER)

/* Functions */
void ConfigSystem();
void ConfigPllSysClock(); // Configures the PLL clock for SYSCLK = 60MHz operation
void InitRam(); // Copies functions from FLASH to RAM
void EnableInterrupts(); // Initializes the interrupts and PIE
void ConfigSleepMode();

/*** External symbols created by the linker command file. The linker places these single-word variables at the addresses they represent.
 * The addresses they represent can be extracted through '&'.
   Use to relocate code from one LOAD location in Flash to a different RUN location in internal RAM.

   To copy functions from flash to RAM, given that the functions are allocated to the memory section ".TI.ramfunc",
   use memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, &RamfuncsLoadSize)

   ***/
/* For general functions to run in RAM */
extern Uint16 RamfuncsLoadStart; // Start address of RAM functions in load location (flash)
extern Uint16 RamfuncsLoadEnd; // End address of RAM functions in flash
extern Uint16 RamfuncsLoadSize; // Size of memory allocated to RAM functions in flash
extern Uint16 RamfuncsRunStart; // Start address of RAM functions in RAM
extern Uint16 RamfuncsRunEnd; // Start address of RAM functions in run location (RAM)
extern Uint16 RamfuncsRunSize; // Size of memory allocated to RAM functions in RAM

#endif /* INIT_H_ */
