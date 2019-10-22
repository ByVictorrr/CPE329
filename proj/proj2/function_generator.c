#include "function_generator.h"
#include "msp.h"

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
    NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for CCR0
    // Step 5 - enable globally
    __enable_irq();

}



