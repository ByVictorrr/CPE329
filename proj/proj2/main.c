#include "msp.h"
#include "Keypad.h"
#include "LCD.h"
#include "delay.h"
#include "DAC.h"
#include <string.h>



/*
#define SAWTOOTH 9
#define SQUARE 7
#define SINE 8
*/

#define SQUARE '7'
#define SAW '9'
#define SINE '8'

#define SELECT_WAVE "789" // range
#define SELECT_FREQ "12345" // range
#define SELECT_DUTYCYCLE "#0*"

// length of array holding values for saw wave
#define LEN_SAW 11 
#define LEN_SINE 17
#define LEN_SQUARE 2

//#define F_INPUT 1500000 // 1.5 Mhz

float saw_voltages[LEN_SAW];
float sine_voltages[LEN_SINE];
float square_voltages[LEN_SQUARE];
struct data wave_data;

// global for all functions
uint16_t ccr0 = 0;

struct data{
	float *ptr; // ptr to the arr
	uint16_t ccr0; // for the handler (now we can pass to handler)
	int i; // index the arr
	int max_index; // max value the index of the array
};



// Handler for CCR0
void TA0_0_IRQHandler(void){
	// step 1 - check if the counter has went over max
	wave_data.i = wave_data.i > wave_data.max_index? 0 : wave_data.i+1;
	// step 2 - turn off the timers flag
	TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// step 3 - set ccr0
	TIMER_A0->CCR[0] = wave_data.ccr0;
}



struct data sawtooth(int freq){

	ccr0 = F_INPUT/(LEN_SAW*freq);

	return (struct data){saw_voltages, ccr0, 0, LEN_SAW-1};
}
struct data square(int freq){

	// step 3 - get ccr0 needed for the freq
	ccr0 = F_INPUT/(2*freq);
	// use 

	return (struct data){square_voltages, ccr0 ,0, LEN_SQUARE-1};
}

struct data sinusoid(int freq){
	
	// use sin(2x-pi/2)+1 to model this
	// step 2 - get ccr0 needed for the freq
	ccr0 = F_INPUT/(LEN_SINE*freq);
	
	// step 3 - send data to the dac
	

	return (struct data){sine_voltages, ccr0, 0, LEN_SINE-1};
}

/**
 * main.c
 */



#define 100HZ '1'
#define 200HZ '2'
#define 300HZ '3'
#define 400HZ '4'
#define 500HZ '5'

void main(void)
{

	int key;
	int freq =  100, dutyC = 50;
	char wave_type = SQUARE;

	// step 1 - init everything
	init_TimerA();
	init_SPI();

	// step 2 - set the data arrays for pts
	gen_arrays(square_voltages, LEN_SQUARE, 1 , FALSE, NULL); // square wave
	gen_arrays(sine_voltages, LEN_SQUARE, .1 , TRUE, cos); // sine wave XXX (Configure step size and LEN)
	gen_arrays(saw_voltages, LEN_SQUARE, .1 , TRUE, cos); // sine wave XXX (Configure step size and LEN)



	while(1){
		Write_string_LCD("(1-5): change freq, (7-9): change wave");
		next_line_pos();
		delay_us(1000);
		Write_string_LCD("(*,0,#): change duty cycle");

		// State 1 - select wave
		if (strstr(SELECT_WAVE, key=read_key()) != NULL){
	        if ((wave_type = key) == SAW){
	            wave_data = sawtooth(freq);
	        }else if (wave_type == SQUARE){
	            wave_data = square(freq);
	        }else if (wave_type == SINE){
	            wave_data = sinusoid(freq);
	        }
	    // state 2 - select freq
		}else if (strstr(SELECT_FREQ, key) != NULL){
			freq = (key + '0')*100;
		// state 3 - select dc
		}else if (strstr(SELECT_DUTYCYCLE, key) != NULL){
			// XXX todo 
		}


	send_to_DAC(voltage_to_dacData(wave_data.ptr[wave_data.i]));
	Clear_LCD();
	delay_us(1000);
	}

}
