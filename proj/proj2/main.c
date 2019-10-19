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

#define F_INPUT 1500000
#define DURATION_WHILE_LOOP 0.182192
#define CCR0_WHILE_LOOP DURATION_WHILE_LOOP *F_INPUT

#define SQUARE '7'
#define SAW '9'
#define SINE '8'

#define SELECT_WAVE "789" // range
#define SELECT_FREQ "12345" // range
#define SELECT_DUTYCYCLE "#0*"

// length of array holding values for saw wave
#define LEN_SAW 80 // keep this
#define LEN_SINE 124 // keep this
#define LEN_SQUARE 2

//#define F_INPUT 1500000 // 1.5 Mhz

float saw_voltages[LEN_SAW];
float sine_voltages[LEN_SINE];
float square_voltages[LEN_SQUARE];
struct data wave_data;

// global for all functions

struct data{
	float *ptr; // ptr to the arr
	uint16_t ccr[2]; // for the handler (now we can pass to handler)
	int i; // index the arr
	int max_index; // max value the index of the array
};



// Handler for CCR0
void TA0_0_IRQHandler(void){
    //P1->OUT |= BIT0;
   // delay_us(1000000);
	// step 1 - check if the counter has went over max
	wave_data.i = wave_data.i >= wave_data.max_index? 0 : wave_data.i+1;
	// step 2 - turn off the timers flag
	TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// step 3 - set ccr0
	TIMER_A0->CCR[0] = wave_data.ccr[0] ;
	//P1->OUT &= ~BIT0;
}

void TA0_N_IRQHandler(void){
	// Step 1 - toggle BIT0
	P1->OUT |= BIT0;
	wave_data.i = wave_data.i >= wave_data.max_index? 0 : wave_data.i+1;
	//step 3 - turn off capture/compare interrupt flag
	TIMER_A0 -> CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
	// Step 2 - add 720 cycles
	TIMER_A0 -> CCR[1] = wave_data.ccr[1]; // add .

}


//XXX : NEED TO WRITE A GENERAL FUNCTION TO RETURN THE SHAPE
struct data sawtooth(int freq){
    uint16_t ccr0;
	ccr0 = F_INPUT/(LEN_SAW*freq) + ceil(CCR0_WHILE_LOOP);

	struct data ret;
	    ret.ptr = saw_voltages;
	    ret.i=0;
	    ret.max_index = LEN_SAW-1;
	    ret.ccr[0] = ccr0;
	    ret.ccr[1] = 0;

	    return ret;
}
struct data square(int freq){

	// step 3 - get ccr0 needed for the freq
    uint16_t ccr0;
	ccr0 = F_INPUT/(2*freq) ; //ceil(CCR0_WHILE_LOOP);

	// use 
	struct data ret;
	ret.ptr = square_voltages ;
	ret.i=0;
	ret.max_index = LEN_SQUARE-1;
	ret.ccr[0] = ccr0;
	ret.ccr[1] = 0;

	return ret;
}

struct data sinusoid(int freq){
    uint16_t ccr0;
	// use sin(2x-pi/2)+1 to model this
	// step 2 - get ccr0 needed for the freq
	ccr0 = F_INPUT/(LEN_SINE*freq) + ceil(CCR0_WHILE_LOOP);
	// step 3 - send data to the dac
	struct data ret;
	    ret.ptr = sine_voltages;
	    ret.i=0;
	    ret.max_index = LEN_SINE-1;
	    ret.ccr[0] = ccr0;
	    ret.ccr[1] = 0;

	    return ret;
}

// XXX : problem changing the freq changes the ccr0 value and then that changes the freq 
//
// denote that the duty cycle is 50 % if ccr[1]  == 0
void set_DutyCycle(struct data *wave_data, int dutyC, int freq){
	// cond 1 - if the wave_data is a square  then change it to 
	if (wave_data -> ptr == square_voltages){
		TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
	// cond2 - if sine or saw
	}else{
		TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIE;
	// step 1 - set ccr[1] to the duty cycle
	}
	float real_DC = dutyC/100.0;
	//------------------------XXX-------------------------
	wave_data->ccr[1] = floor((1 - real_DC)*wave_data->ccr[0]); // XXX relate dutyC to ccr1
	TIMER_A0 -> CCR[1] = wave_data->ccr[1]; // inital
	//------------------------XXX-------------------------
}








void init_TimerA(uint16_t ccr0){
    // step 0 - set up the GPIO
    P4->DIR |= BIT0 | BIT1;
    P4->SEL0 &= ~BIT0 & BIT1;
    P4->SEL1 &= ~BIT0 & ~BIT1;
    P4->OUT&=~BIT0;

    // Step 1 - set the inital cycles
    TIMER_A0->CCR[0] = ccr0; // cycles
    // XXX : review below
    //step 2 - control regs - enable interrupts , and compare mode

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

    //Step 3 - Select MCLK and select up mode
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // tassel - select clock src, mc - select continuious mode

    // Step 4 = enable NVIC
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0
    NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for CCR0
    // Step 5 - enable globally
    __enable_irq();


}

/**
 * main.c
 */



void main(void)
{
    P1->DIR|=BIT0;
    P1->OUT&=~BIT0;
	char key;
	int freq = 100, dutyC = 10;
	char wave_type = SQUARE;

	// step 1 - init everything
	init_SPI();
	P1->DIR |= _CS;
	set_clk("SMCLK");
	set_DCO(1.5);
	Keypad_init();
	Init_LCD();

	// step 2 - set the data arrays for pts

	gen_arrays(square_voltages, LEN_SQUARE, 2 , FALSE, NULL);
	gen_arrays(sine_voltages, LEN_SINE, .025 , FALSE, sin); // sine wave XXX (Configure step size and LEN)
	gen_arrays(saw_voltages, LEN_SAW, .025 , FALSE, NULL); // sine wave XXX (Configure step size and LEN)


	// by defualt
	wave_data = square(freq);
	set_DutyCycle(&wave_data,dutyC, freq);
	init_TimerA(wave_data.ccr[0]);

	while(1){
	    //P1->OUT ^= BIT0;
	   send_to_DAC(voltage_to_dacData(wave_data.ptr[wave_data.i]));
	   /*
		Write_string_LCD("(1-5): change freq, (7-9): change wave");
		next_line_pos();
		//delay_us(10000000);
		Write_string_LCD("(*,0,#): change duty cycle");
		key=read_key();
		// by default wave_data is square
		// State 1 - select wave
		if (strchr(SELECT_WAVE, key) != NULL){
	        if ((wave_type = key) == SAW){
	            wave_data = sawtooth(freq);
				set_DutyCycle(&wave_data,50, freq);
	        }else if (wave_type == SQUARE){
	            wave_data = square(freq);
				set_DutyCycle(&wave_data,50,freq);
	        }else if (wave_type == SINE){
	            wave_data = sinusoid(freq);
				set_DutyCycle(&wave_data,50,freq);
	        }

	    // state 2 - select freq
		}else if (strchr(SELECT_FREQ, key) != NULL){
			freq = (key + '0')*100;
			// selecting freq effects the duty cycle which means we have to compensate of moving ccr[1]
			set_DutyCycle(&wave_data, dutyC,freq); // setting it in here makes it orderide
		// state 3 - select dc
		}else if (strchr(SELECT_DUTYCYCLE, key) != NULL){
			// XXX todo 
			// step 1 -  enable ccrn
			// step 2 - check the ccr0 value so that t0-t1/ T = duty cycle
			if (key == '*' && dutyC > 10){
				dutyC = dutyC-10;
				set_DutyCycle(&wave_data,dutyC,freq);
			}else if (key == '#' && dutyC < 90){			
				dutyC = dutyC-10;
				set_DutyCycle(&wave_data,dutyC,freq);
			}else if (key == '0'){
				dutyC = 50;
				set_DutyCycle(&wave_data,dutyC,freq);
			}
		}
	 delay_us(1000000);
	 Clear_LCD();
	 Home_LCD();
	 */

	}

}
