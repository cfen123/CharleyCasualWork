# C2000 Peripherals 

- ADCs: ADC-A and ADC-C 
  - ADC-A samples external signal VTEST on A1 
  - ADC-C samples internal temperature sensor 
  - ADC-A is enabled when `mode = ADC` and disabled otherwise. This ensures that UART doesn't transmit garbage ADC data when not testing ADC. 
- Comparators using CMPSS1 
  - Positive input is A1 (positive input 4) 
  - Negative input is A11 (negative input 1)
  - Output given by output XBAR on GPIO2 when `mode = COMPARATOR`
- PWM using EPWM2A (GPIO2) when `mode = PWM`
- UART using SCI-A 
  - GPIO29 is TX
  - GPIO28 is RX