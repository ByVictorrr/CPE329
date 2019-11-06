#ifndef SAMPLE_H_
#define SAMPLE_H_
#include "msp.h"
#include "UART.h"
#include "ADC.h"
#include "delay.h"
#include <math.h>
#include "UART.h"
#include <stdio.h>


#define MAX_SAMPLE 1000
#define LOW_RST 30000
#define HIGH_RST 0


float total_sqr;

int low, high; // for high and low values
int sample_count;


// Description: get the sampled value
void ADC14_IRQHandler();

// Description: triggers the adc14 to start new sample every 3000 cycles
void TA0_0_IRQHandler();
// Description: disables Ta0ccr0 from going to handler
void disable_TA0();
// Description: enables Ta0ccr0 from going to handler
void enable_TA0();
// Description: init of Ta0
void init_TA0();
// Description: gets vrms and vpp by pass by ref
void get_vpp_vrms(float *Vrms, float *Vpp);


#endif
