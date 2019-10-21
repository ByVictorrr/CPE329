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


typedef enum{FALSE, TRUE} bool;
int counter;

void init_SPI();

void send_to_DAC(uint16_t out_voh);

// returns a uint16 (12bits) - representing the voltage level
// returns -1 on error
uint16_t voltage_to_dacData(float volts);
// if you decrment the delta by half you have to increase the LEN by 2
void gen_arrays(float *voltages, int size, float delta, bool isSymetric, double (*fn)(double));

void gen_square(float *voltages, int size, int dutyCycle);
#endif
