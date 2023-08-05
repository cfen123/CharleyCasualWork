//#############################################################################
//
// FILE:   cputimer.c
//
// TITLE:   C28x CPU timer Driver
//
//#############################################################################
// $Copyright:
// Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

#include "cputimer.h"
#include "interrupt.h" // Added by user for CPUTimer_configInterrupt()

//*****************************************************************************
//
// CPUTimer_setEmulationMode
//
//*****************************************************************************
void CPUTimer_setEmulationMode(uint32_t base, CPUTimer_EmulationMode mode)
{
    ASSERT(CPUTimer_isBaseValid(base));
    //
    // Write to FREE_SOFT bits of register TCR
    //
      HWREGH(base + CPUTIMER_O_TCR) =
            (HWREGH(base + CPUTIMER_O_TCR) &
            ~(CPUTIMER_TCR_FREE | CPUTIMER_TCR_SOFT)) |
            (uint16_t)mode;
}

//*****************************************************************************
//
//! Set CPU timer period and clock division factor.
//!
//! \param base is the base address of the timer module.
//! \param periodCount is the top value of the timer.
//! \param clkDivFactor is the CPU timer clock division factor. Not named 'prescaler' since
//! the parameter 'prescaler' works differently in CPUTimer_setPreScaler().
//!
//! This function sets the clock division factor for the CPU timer. For every value
//! of <clkDivFactor>, the CPU timer counter decrements by 1.
//!
//! Timer clock frequency (Hz) = SYSCLK/<clkDivFactor> where SYSCLK is system clock frequency (Hz)
//! Timer period (seconds) = (<periodCount> + 1)/<Timer clock frequency>
//!
//! \return None.
//! Function author: Charley Shi
//! Date added: 19/01/2023
//
//*****************************************************************************
void CPUTimer_setConfig(uint32_t base, uint32_t periodCount, uint16_t clkDivFactor) {
    ASSERT(CPUTimer_isBaseValid(base));
    CPUTimer_setPeriod(base, periodCount);
    CPUTimer_setPreScaler(base, clkDivFactor - 1); // -1 to account for difference between clkDivFactor and 'prescaler'
}

//*****************************************************************************
//
//! Configures CPU timer interrupt with ISR.
//!
//! \param base is the base address of the timer module.
//! \param handler is the address of the ISR.
//!
//! Enables CPU timer interrupt in PIE and CPU Timer and assigns an ISR.
//!
//! \return None.
//! Function author: Charley Shi
//! Date added: 19/01/2023
//
//*****
void CPUTimer_configInterrupt(uint32_t base, void (*handler)(void)) {
    ASSERT(CPUTimer_isBaseValid(base));
    uint32_t interruptNumber; // Interrupt number as in inc/hw_ints.h
    switch (base) { // Can't use a formula to calculate interruptNumber since there's no pattern.
        case CPUTIMER0_BASE:
            interruptNumber = INT_TIMER0;
            break;

        case CPUTIMER1_BASE:
            interruptNumber = INT_TIMER1;
            break;

        case CPUTIMER2_BASE:
            interruptNumber = INT_TIMER2;
            break;
    }

    CPUTimer_enableInterrupt(base);
    Interrupt_register(interruptNumber, handler);
    Interrupt_enable(interruptNumber);
}


