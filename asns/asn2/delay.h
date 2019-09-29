#ifdef DELAY_H_
#define DELAY_H_
#include "msp.h"

/**
 * main.cg
 */

#define BIT_PULL(x)                                 ((uint32_t)1 << (x))
#define FREQ_1_5_MHZ 1.5
#define FREQ_3_MHZ 3
#define FREQ_6_MHZ 6
#define FREQ_12_MHZ 12
#define FREQ_24_MHZ 24
#define FREQ_48_MHZ 48

void set_DCO(float freq);
void delay_us(int usec);
struct slopef ret_freq_prop(uint32_t bits_extracted);


struct slopef{
    float freq;
    int b;
    float prop;
};

int arr[6] = {10, 500, 1000, 2500, 5000, 40000};


#endif 

