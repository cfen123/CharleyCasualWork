/*
 * system_config.c
 *
 *  Created on: 18/11/2022
 *      Author: Charley Shi
 *
 *  For some reason, I get the unresolved symbol error if it's a .c file but it works if it's .cpp.
 */

#include <system_config.h>

/* Initializes the system
 *
 * - Disables the watchdog timer
 * - Configures the system clocks
 * - Initializes the RAM
 * - Configures the sleep mode
 * */
void ConfigSystem() {
    // Disable watchdog timer
    EALLOW;
    WdRegs.WDCR.bit.WDCHK = 0b101; // Write 0b101 to WDCHK (5:3) (manual p.501)
    WdRegs.WDCR.bit.WDDIS = 1; // Disable watchdog timer by setting WDDIS. (manual p.501)
    EDIS;

    ConfigPllSysClock();
    SysCtl_setLowSpeedClock(SYSCTL_LSPCLK_PRESCALE_2);
    InitRam();
    ConfigSleepMode();
}

/* Configures the PLL system clock according to Section 3.7.10 "System Clock Setup" of the TRM. */
void ConfigPllSysClock() {
    EALLOW;

    /* Step 1: Bypass the PLL by clearing SYSPLLCTL1[PLLCLKEN].
     * Allow at least 60 NOP instructions (60 clock cycles) for this to take effect. */
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
    DELAY_SIXTY_CYCLES;

    /* Step 2: Power down the PLL by writing to SYSPLLCTL1.PLLEN=0 and allow at least 60 NOP
     * instructions for this to take effect. */
    ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 0;
    DELAY_SIXTY_CYCLES;

    /* Step 3: Select the reference clock source (OSCCLK) by writing to CLKSRCCTL1.OSCCLKSRCSEL.
     * Allow at least 300 NOP instructions for this to take effect. */
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0b00; // Use INTOSC2 as clock source
    DELAY_SIXTY_CYCLES;
    DELAY_SIXTY_CYCLES;
    DELAY_SIXTY_CYCLES;
    DELAY_SIXTY_CYCLES;
    DELAY_SIXTY_CYCLES;

    /* Step 4: Set the system clock divider using SYSCLKDIVSEL[PLLSYSCLKDIV] */
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = SYSCLKDIV >> 1;

#if USE_PLL == 1
    /* Step 5: Set the IMULT and FMULT simultaneously by writing 32-bit value in SYSPLLMULT at once.
     * This will automatically enable the PLL. Be sure the settings for the multiplier and dividers
     * do not violate the frequency specifications defined in the datasheet. */
    ClkCfgRegs.SYSPLLMULT.all = (Uint32)PLL_IMULT + ((Uint32)PLL_FMULT << 8);


    /* Step 6: Wait for PLL to lock by polling for lock status bit to go high, that is, SYSPLLSTS.LOCKS=1 */
    while(!ClkCfgRegs.SYSPLLSTS.bit.LOCKS);

    /* Step 7: Configure DCC with reference clock as OSCCLK and clock under measurement as PLLRAWCLK,
     * and verify the frequency of the PLL. If the frequency is out of range, do not enable PLLRAWCLK
       SYSCLK, stop here and troubleshoot. Refer to DCC chapter for more information on its configuration
       and usage.*/

    /* Step 8: Switch to the PLL as the system clock by setting SYSPLLCTL1[PLLCLKEN]. */
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;
    ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 1; // Not necessary?
#endif
    EDIS;
}

void InitRam() {
    memcpy((uint32_t *)&RamfuncsRunStart, (uint32_t *)&RamfuncsLoadStart, (uint32_t)&RamfuncsLoadSize);
}

/* Enables the PIE and global interrupts */
void EnableInterrupts() {
    Interrupt_enablePIE(); // Enables the PIE block
    Interrupt_enableMaster(); // Enables global interrupts
}

/* Configures the sleep mode entered when the CPU goes to sleep.
 * 0 = IDLE
 * 1 = STANDBY
 * 1x = HALT
 */
void ConfigSleepMode() {
    EALLOW;
    CpuSysRegs.LPMCR.bit.LPM = 0; // IDLE mode
    EDIS;
}
