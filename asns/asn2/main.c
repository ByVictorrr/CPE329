#include "msp.h"


/**
 * main.cg
 */


#define BIT_PULL(x)                                 ((uint32_t)1 << (x))
#define FREQ_12_MHz 1.5
void set_DCO(float freq);
void delay_us(int usec);
double ret_freq(uint32_t bits_extracted);

void main(void)
{


	// Step 1 - set up DCO
	set_DCO(FREQ_12_MHz);
	// Step 2 - select right CTL1 values so that MCLK outputs the DCO
	// Step 2.1 - set SELM =
	// Step 2.2 - DIVM = 1 
	CS -> KEY = CS_KEY_VAL;
	CS -> CTL1 = CS_CTL1_SELM_3 |  CS_CTL1_DIVM__1;
	CS -> KEY = ~CS_KEY_VAL;

	// Step 3 - set gpio out  SEL = 01
	// Step 3.1 - first select what we want to output
	P4->SEL0 |= BIT3; //
	P4->SEL1 &= ~BIT3;//;
	//step 3.2 - connect MCLK to from module
	//Step 3.3 - set direction to output
	P4->DIR |= BIT3;

	delay_us(10000);

}


void set_DCO(float freq){

	if (freq >= 1.5 && freq<=48){ //assumed in Mhz
		//Step 1 - unlock 
		CS->KEY = CS_KEY_VAL;
		CS->CTL0 = CS_CTL0_DCOTUNE_OFS;
		//range(1.5,2)
		if (freq >= 1.5 && freq < 2.0){
			CS->CTL0 = CS_CTL0_DCORSEL_0;
		//range(2,4)
		}else if (freq >= 2.0 && freq < 4.0){
			CS->CTL0 = CS_CTL0_DCORSEL_1;
		//range(4,8)
		}else if (freq >= 4.0 && freq < 8.0){
			CS->CTL0 = CS_CTL0_DCORSEL_2;
		//range(8,16)
		}else if (freq >= 8.0 && freq < 16){
			CS->CTL0 = CS_CTL0_DCORSEL_3;
		//range(16,32)
		}else if (freq >= 16.0 && freq < 32){
			CS->CTL0 = CS_CTL0_DCORSEL_4;
		//range(32,48)
		}else{	
			CS->CTL0 = CS_CTL0_DCORSEL_5;
			// next step to assign DCOTUNE dec_ret in hex
		}
		CS->KEY = ~CS_KEY_VAL;
	}
}


// is it will ret the frequency of the bits_extracted
double ret_freq(uint32_t bits_extracted){
		//range(1.5,2)
		if (bits_extracted == CS_CTL0_DCORSEL_0) 
			return 1.5;
		//range(2,4)/
		if (bits_extracted == CS_CTL0_DCORSEL_1) 
			return 3;
		//range(4,8)
		else if (bits_extracted == CS_CTL0_DCORSEL_2)
			return 6;
		//range(8,16)
		else if (bits_extracted == CS_CTL0_DCORSEL_3 )
			return 12;
		//range(16,32)
		else if (bits_extracted == CS_CTL0_DCORSEL_4 )
			return 24;
		//range(32,48)
		else
			return 48;

}


// Create function delay_us() to cause a software delay of a specified time.
// usec =< 50ms*(10^3us/1ms) = 50000us
void delay_us(int usec){
	// Lets reverse engineer this bitch
	
	uint32_t bits_extracted = CS->CTL0 & BIT_PULL(16) & BIT_PULL(17) & BIT_PULL(18);
	
	// bits extracted for - > time
	double period = 1/ ret_freq(bits_extracted); // 10^-3 s
	
	int delay = period * usec;  // Period * delay
	int i; 
	for (i = delay; i > 0; i--);

}


