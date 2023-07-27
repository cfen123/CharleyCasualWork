# DriverLib library 
Source: `C2000Ware_4_02_00_00/driverlib/f28002x/driverlib` 

Note that `driverlib.h` is from `C2000Ware_4_02_00_00/device_support/f28002x/common/include`

I have added additional functions to the GPIO, ePWM and CPU timer files. The rest of this file will detail those functions. For these functions, detailed comments about the usage have been added in the code. 

## GPIO 
Functions for setting and clearing GPIO output pins. 
Relevant files: `gpio.h` and `gpio.c`. 

### Clear GPIO pin 
```c
static inline void GPIO_clearPin(uint32_t pin)
```
Clears a GPIO output pin

### Set GPIO pin 
```c
static inline void GPIO_setPin(uint32_t pin)
```
Sets a GPIO output pin

## CPU Timers
Similar to for the CLA, two functions were added to make configuring CPU timers 
and their associated interrupts easier. Relevant files: `cputimer.h` and `cputimer.c`. 

### Configure timer 
```c
void CPUTimer_setConfig(uint32_t base, uint32_t periodCount, uint16_t clkDivFactor)
```
Set CPU timer period and clock division factor.

### Configure timer's CPU interrupt 
```c
void CPUTimer_configInterrupt(uint32_t base, void (*handler)(void))
```
Configures CPU timer interrupt with ISR.

## ePWM 
A function which enables the ePWM peripheral clock and the ePWM outputs on the GPIO pins was added. The clock and outputs are enabled for a specific ePWM module 
(ePWM1 through ePWM8). Relevant files: `epwm.h` and `epwm.c`. 
```c
void EPWM_enableModule(EPWM_Module module, EPWM_Outputs outputs)
```
The types of the function parameters were defined as follows. 
```c
typedef enum {
    EPWM1,
    EPWM2,
    EPWM3,
    EPWM4,
    EPWM5,
    EPWM6,
    EPWM7,
    EPWM8
} EPWM_Module;

typedef enum {
    EPWM_OUTPUT_NONE, // Neither output enabled. Use if the ePWM module is just used as a timer.
    EPWM_OUTPUT_A, // EPWM output on ePWMxA, not on ePWMxB
    EPWM_OUTPUT_B, // EPWM output on ePWMxB, not on ePWMxA
    EPWM_OUTPUT_A_B // EPWM output on both ePWMxA and ePWMxB
} EPWM_Outputs;
```