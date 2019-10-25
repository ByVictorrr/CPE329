#include "msp.h"
#include "delay.h"

#define CCR0 960 // 960
#define CCRN 720


// Handler for CCR0
void TA0_0_IRQHandler(void){

	// Step 1 - toggle BIT0
	P4->OUT = ~BIT0;
	//step 3 - turn off capture/compare interrupt flag(to trigger again on rising edge)
	TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// Step 2 - add 960 cycles
	TIMER_A0 -> CCR[0] += CCR0; // add .
	P4->OUT &= ~BIT1;


}
// Handler for CCR[1-6]
void TA0_N_IRQHandler(void){


	// Step 1 - toggle BIT0
	P4->OUT = BIT0;
	//step 3 - turn off capture/compare interrupt flag
	TIMER_A0 -> CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
	// Step 2 - add 720 cycles
	TIMER_A0 -> CCR[1] += CCR0; // add .

}


void set_everything(){
	// step 0 - set up the GPIO
	P4->DIR |= BIT0;
	P4->SEL0 &= ~BIT0;
	P4->SEL1 &= ~BIT0;



	// Step 1 - set the inital cycles
	TIMER_A0->CCR[1] = CCRN; //cycles
	TIMER_A0->CCR[0] = CCR0; // cycles
	//step 2 - control regs - enable interrupts , and compare mode
	TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

	//Step 3 - Select MCLK and select up mode
	TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_2; // tassel - select clock src, mc - select up mode

	// Step 4 = enable NVIC
	NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0
	NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for other CCRs

	// Step 5 - enable globally
	__enable_irq();


}

void main(void)
{


    set_DCO(3); // 24Mhz
    set_clk("MCLK");
    //set_clk("SMCLK");
    set_everything();

	while(1);


}

