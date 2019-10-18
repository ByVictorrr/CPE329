#ifndef DAC_H_
#define DAC_H_
#include "msp.h"
// Using UCB0 (port 1)
#define SCLK BIT5
#define SIMO BIT6
#define _CS BIT7


// working for 1-4

#define BUF (uint16_t)BIT(14)
#define GAIN (uint16_t)BIT(13)
#define SHDN (uint16_t)BIT(12)

#define NULL 0


#define LEN 128
#define F_INPUT 1.5
#define T_OUTPUT 20
#define K 1000 //  rep kila
#define CCR0  (uint16_t)(F_INPUT * T_OUTPUT * K)/LEN

typedef enum{FALSE, TRUE} bool;
int counter;

// Handler for CCR0
void TA0_0_IRQHandler(void);

// inits timerA
void init_TimerA();


void init_SPI();

void send_to_DAC(uint16_t out_voh);

// returns a uint16 (12bits) - representing the voltage level
// returns -1 on error
uint16_t voltage_to_dacData(float volts);
// if you decrment the delta by half you have to increase the LEN by 2
void gen_arrays(float *voltages, int size, float delta, bool isSymetric, double (*fn)(double));
#endif
