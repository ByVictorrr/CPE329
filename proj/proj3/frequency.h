#ifndef FREQUENCY_H_
#define FREQUENCY_H_
#include "msp.h"
#include "UART.h"
#include "ADC.h"
#include "delay.h"
#include <math.h>
#include "UART.h"
#include <stdio.h>

#define F_INPUT 3000000

float f_wave;
int freq_flag;
int ta1_ifg_overflows;


uint32_t st_r;
uint32_t en_r;
uint32_t hi_lo;


// Description: TA1 timer init
void init_TA1();
// Description: for flag overflows
void TA1_N_IRQHandler();
// Description: for a given frequency calibrated lookup table
uint32_t getRealFreq(float wavefreq);
// Description: checks to see if first rising edge captured 
// if see check if cov is one is so repeat till not cov
void TA1_0_IRQHandler();
// Description: disable TA1 after enabling sampling module
void disable_TA1();

// Description: main function to get frequency stored in f_wave
void get_freq();

#endif
