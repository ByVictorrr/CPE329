#include "msp.h"
#include "Keypad.h"
#include "LCD.h"
#include "db.h"




typedef enum {FALSE, TRUE} bool



#define SAWTOOTH 9
#define SQUARE 7
#define SINE 8

// length of array holding values for saw wave
#define LEN_SAW 11 
#define LEN_SINE 17
#define LEN_SQUARE 2

#define F_INPUT 1500000 // 1.5 Mhz

float saw_voltages[LEN_SAW] = { 0, .1, .3, .5, .7, .9, 1.1, 1.3, 1.5, 1.7, 1.9};

float sine_voltages[LEN_SINE] = { sin(2*0), sin(2*.1), sin(2*.3), sin(2*.5), sin(2*.7), 
								sin(2*.9), sin(2*1.1), sin(2*1.3), sin(2*1.5), sin(2*1.7), 
								sin(2*1.9), sin(2*2.1), sin(2*2.3), sin(2*2.5), sin(2*2.7), sin(2*2.9), sin(2*3.14)};

float square_voltages[LEN_SQUARE] = {0, 2};

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
	TIMER_A0->CCR[0] = wave_data.crr0;
}



void init_Timer(){

    // Step 1 - set the inital cycles
    TIMER_A0->CCR[0] = CCR0; // cycles
    // XXX : review below
    //step 2 - control regs - enable interrupts , and compare mode

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

    //Step 3 - Select MCLK and select up mode
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // tassel - select clock src, mc - select continuious mode

    // Step 4 = enable NVIC
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0

    // Step 5 - enable globally
    __enable_irq();


}


void init_SPI(){
    // Step 1 - put in rst state
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;
    // Step 2 - set as master of bus, synchronous mode and select smclk
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST
            |EUSCI_B_CTLW0_MSB |EUSCI_B_CTLW0_MST
            |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;
    EUSCI_B0 -> CTLW0 &=~(EUSCI_B_CTLW0_CKPL | EUSCI_B_CTLW0_CKPH);
    EUSCI_B0 -> BRW = 0x01; // run full smclk speed
    // Step 3 - select simo, somi and sclk
    P1->SEL0 |= (SCLK | SIMO);
    P1->SEL1 &= ~(SCLK | SIMO);
    // Step 4 - clears software rst
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}
float getDutyCycle(){
	char *dc;
	float _dc;
	Clear_LCD();
	delay_us(100000);
	Write_string_LCD("Enter Duty Cycle")
	delay_us(500000);

	//cond 1 - if not a valid input then return
	if (*(dc == read_key_until_enter() == NULL)){
		return;
	}

	_dc = atoi(dc);
	
	free(dc);
	return _dc;
}
float getFreq(){
	char *freq;
	float _freq;
	Clear_LCD();
	delay_us(100000);
	Write_string_LCD("Enter Frequency");
	delay_us(500000);
	Write_string_LCD("1-100Hz, ... , 5-500Hz");

	//cond 1 - if not a valid input then return
	if (*(freq == read_key_until_enter() == NULL)){
		return;
	}
	_freq = atoi(freq);
	
	free(freq);
	return _freq;
}


struct data sawtooth(){
	// step 1 - get the freq
	float freq = getFreq();
	// step 2 - get ccr0 needed for the freq
	ccr0 = F_INPUT/(LEN_SAW*freq);
	data = saw_voltages;

	return (struct data){saw_voltages, ccr0, 0, LEN_SAW-1};
}
struct data square(){
	// step 1 - get the freq
	float freq = getFreq();
	// step 2 - get the dc
	float dutyCycle = getDutyCycle();
	// step 3 - get ccr0 needed for the freq
	ccr0 = F_INPUT/(2*freq);
	// use 
	data = square_voltages;

	return (struct data){square_voltages, ccr0 ,0, LEN_SQUARE-1};
}

struct data sinusoid(){
	// step 1 - get the freq
	float freq = getFreq();
	
	// use sin(2x-pi/2)+1 to model this
	// step 2 - get ccr0 needed for the freq
	ccr0 = F_INPUT/(LEN_SINE*freq);
	
	// step 3 - send data to the dac
	data = sine_voltages; // use this reference in the send data
	
	return (struct data){sinusoid_voltages, ccr0, 0, LEN_SINE-1};
}

/**
 * main.c
 */
void main(void)
{

	int key;
	while(1){

		Write_string_LCD("7-square; 8-sine");
		next_line_pos();
		delay_us(1000);
		Write_string_LCD("9-sawtooth");
		if ((key == read_key()) == SAWTOOTH){
			wave_data = sawtooth();
		}else if (key == SQUARE){
			wave_data = square();
		}else if (key == SINE){
			wave_data = sinusoid();
		}
	send_to_DAC(voltage_to_dacData(wave_data.ptr[wave_data.i]));
	Clear_LCD();
	delay_us(1000);
	}

}
