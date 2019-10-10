#include "msp.h"
#include "delay.h"

#define CCR0 480

enum PARTS{PART_A, PART_B};

// Handler for CCR0
void TA0_0_IRQHandler(void){
    // step 0 - start time
    P4->OUT |= BIT1;
	// Step 1 - toggle BIT0
	P4->OUT ^=BIT0;
	//step 3 - turn off capture/compare interrupt flag(to trigger again on rising edge)
	TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// Step 2 - add 960 cycles
	TIMER_A0 -> CCR[0] = CCR0; // add .
	// step 3 - stop isr time
	P4->OUT &= ~BIT1;

}


void set_everything(){
	// step 0 - set up the GPIO
	P4->DIR |= BIT0;
	P4->SEL0 &= ~BIT0;
	P4->SEL1 &= ~BIT0;
	// Step 1 - init the CCR0
	TIMER_A0->CCR[0] = CCR0; // cycles
	// XXX : review below
	//step 2 - control regs - enable interrupts , and compare mode
	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

	//Step 3 - Select MCLK and select up mode
	TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // tassel - select clock src, mc - select up mode

	// Step 4 = enable NVIC
	NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0

	// Step 5 - enable globally
	__enable_irq();


}

void main(void)
{
    // Getting MCLK out
    P4->DIR|=BIT3;
    P4->SEL1&=~BIT3;
    P4->SEL0|=BIT3;

    set_DCO(24.0); // 24Mhz
    set_clk("MCLK");
    set_everything();


	while(1);


}

