#include "msp.h"
#include "Keypad.h"
#include "LCD.h"
#include "delay.h"
#include "DAC.h"
#include <string.h>
#include <math.h>

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
#define LEN 250

static float voltages[LEN];
// global pointer 
struct data wave_data;


struct data{
	float *ptr; // ptr to the arr
	uint16_t ccr0; // for the handler (now we can pass to handler)
	int i; // index the arr
	int max_index; // max value the index of the array
};


// Handler for CCR0
void TA0_0_IRQHandler(void){
	// step 1 - check if the counter has went over max
	wave_data.i = wave_data.i >= wave_data.max_index? 0 : wave_data.i+1;
	// step 2 - turn off the timers flag
	TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// step 3 - set ccr0
	TIMER_A0->CCR[0] = wave_data.ccr0 ;
	//P1->OUT &= ~BIT0;
}


struct data waveform(int freq){

    uint16_t ccr0;
	struct data ret;

	// Structure of global var
	ccr0 = F_INPUT/(LEN*freq);
	ret.ptr = voltages;
	ret.i=0;
	ret.max_index = LEN-1;
	ret.ccr0 = ccr0;

	return ret;
}



void init_TimerA(uint16_t ccr0){

    // Step 1 - set the inital cycles
    TIMER_A0->CCR[0] = ccr0; // cycles
    //step 2 - control regs - enable interrupts , and compare mode

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

    //Step 3 - Select MCLK and select up mode
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // tassel - select clock src, mc - select continuious mode

    // Step 4 = enable NVIC
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0
    // Step 5 - enable globally
    __enable_irq();

}




void main(void)
{
	signed char key;
	int dutyC = 50;
	int freq = 100;
	char wave_type = SQUARE;

	// step 1 - init everything
	init_SPI();
	P1->DIR |= _CS;
	set_clk("SMCLK");
	set_DCO(12);
	Keypad_init();
	Init_LCD();

	// step 2 - set the data arrays for default
	gen_square(voltages, LEN, dutyC);
	
	// step 3 - by default and init timers
	wave_data = waveform(freq);
	init_TimerA(wave_data.ccr0);

	while(1){
	    send_to_DAC(voltage_to_dacData(wave_data.ptr[wave_data.i]));
		// doesnt go in the if statment if nothing is pressed
		if ((key=read_key()) != -1){
			// State 1 - select wave
			if (strchr(SELECT_WAVE, key) != NULL){
				if ((wave_type = key) == SAW){
					gen_arrays(voltages, LEN, .012, FALSE, NULL); // sine wave XXX (Configure step size and LEN)
				}else if (wave_type == SQUARE){
					gen_square(voltages, LEN, dutyC);
				}else if (wave_type == SINE){
					gen_arrays(voltages, LEN, .01256, FALSE, cos);
				}else if (wave_type == RANDOM){
					gen_arrays(voltages, LEN, .01256, FALSE, tan);
				}
				wave_data = waveform(freq); // get new wave with new freq
			// state 2 - select freq
			}else if (strchr(SELECT_FREQ, key) != NULL){
				// Step 1 - get the freq
				freq = (key - '0')*100;
				// step 2 - relate the ccr0 value to freq
				wave_data.ccr0 = F_INPUT/((wave_data.max_index+1)*freq);
			// state 3 - select dc but also make sure we are using square voltages
			}else if (strchr(SELECT_DUTYCYCLE, key) != NULL && wave_type == SQUARE){
				// step 1 - decr/incr or rest duty cycle of square wave
				if (key == '*' && dutyC > 10){ // decreases by 10
					dutyC -= 10;
				}else if (key == '#' && dutyC < 90){
					dutyC += 10;
				}else if (key == '0'){
					dutyC = 50;
				}
				gen_square(voltages, LEN, dutyC);
				delay_us(200000);
			}
	}

}
}
