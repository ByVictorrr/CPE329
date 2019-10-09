#include "msp.h"
#include "delay.h"

#define LED BIT0 // for port0
#define CCR0 1 // 960
#define CCRN 720
#define CCR0_PART B 480
//469
enum PARTS{PART_A, PART_B};

// Handler for CCR0
void TA0_0_IRQHandler(void){
    // step 0 - start time
    P4->OUT |= BIT1;
	// Step 1 - toggle led
	P4->OUT ^=LED;
	//step 3 - turn off capture/compare interrupt flag(to trigger again on rising edge)
	TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// Step 2 - add 960 cycles
	TIMER_A0 -> CCR[0] = CCR0; // add .
	P4->OUT &= ~BIT1;


}
// Handler for CCR[1-6]
void TA0_N_IRQHandler(void){


	// Step 1 - toggle led
	P4->OUT |= LED;
	//step 3 - turn off capture/compare interrupt flag
	TIMER_A0 -> CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
	// Step 2 - add 720 cycles
	TIMER_A0 -> CCR[1] += CCR0; // add .

}


void set_everything(int mode){
	// step 0 - set up the GPIO
	P4->DIR |= LED | BIT1;
	P4->SEL0 &= ~LED & ~BIT1;
	P4->SEL1 &= ~LED & ~BIT1;





	// Step 1 - set the inital cycles
    if (mode == PART_A)
        TIMER_A0->CCR[1] = CCRN; //cycles
	TIMER_A0->CCR[0] = CCR0; // cycles
	// XXX : review below
	//step 2 - control regs - enable interrupts , and compare mode
	if (mode == PART_A)
		TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;

	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

	//Step 3 - Select MCLK and select up mode
	TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // tassel - select clock src, mc - select up mode

	// Step 4 = enable NVIC
	NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0

	if (mode == PART_A)
		NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for other CCRs

	// Step 5 - enable globally
	__enable_irq();


}
/**
 * main.c
 */

void main(void)
{
    P4->DIR|=BIT3;
     P4->SEL1&=~BIT3;
     P4->SEL0|=BIT3;

    set_DCO(24.0); // 24Mhz
    set_clk("MCLK");
    //set_clk("MCLK");
    set_everything(PART_B);





/*
	// set SMCLK  out
	P7->DIR|=BIT0;
	P7->SEL0|=BIT0;
	P7->SEL1&=~BIT0;

*/
	while(1);


}


