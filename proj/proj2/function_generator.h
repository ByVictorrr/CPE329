#ifndef FUNCTION_GENERATOR_H_
#define FUNCTION_GENERATOR_H_
#include "msp.h"


#define FREQ_100HZ '1'
#define FREQ_200HZ '2'
#define FREQ_300HZ '3'
#define FREQ_400HZ '4'
#define FREQ_500HZ '5'

#define F_INPUT 12000000

#define RANDOM '6'
#define SQUARE '7'
#define SAW '9'
#define SINE '8'

#define SELECT_WAVE "6789" 
#define SELECT_FREQ "12345"
#define SELECT_DUTYCYCLE "#0*"

// length of array holding values for each wave
#define LEN 100

static float voltages[LEN];
// global pointer 
static struct data wave_data;


struct data{
	float *ptr; // ptr to the arr
	uint16_t ccr0; // for the handler (now we can pass to handler)
	int i; // index the arr
	int max_index; // max value the index of the array
};

// Handler for CCR0
void TA0_0_IRQHandler(void);
// returns associated vars needed for a given freq
struct data waveform(int freq);
// initalize timerA
void init_TimerA(uint16_t ccr0);



#endif


