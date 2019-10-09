#include "msp.h"
#include "delay.h"

#define LED BIT0 // for port0
#define CCR0 960
#define CCRN 720
enum PARTS{PART_A, PART_B};

// Handler for CCR0
void TA0_0_IRQHandler(void){
	// Step 1 - toggle led
	P5->OUT ^= ~LED;
	// Step 2 - add 960 cycles
	TIMER_A0 -> CCR[0] += CCR0; // add .
	//step 3 - turn off capture/compare interrupt flag
	TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; 
}
// Handler for CCR[1-6]
void TA0_N_IRQHandler(void){
	// Step 1 - toggle led
	P5->OUT ^= LED;
	// Step 2 - add 960 cycles
	TIMER_A0 -> CCR[1] += CCRN; // add .
	//step 3 - turn off capture/compare interrupt flag
	TIMER_A0 -> CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}


void set_everything(int mode){
	// step 0 - set up the GPIO
	P5->DIR |= LED;

    set_DCO(24); // 24Mhz

	// Step 1 - set the inital cycles
	TIMER_A0->CCR[1] = CCRN; //cycles
	TIMER_A0->CCR[0] = CCR0; // cycles
	// XXX : review below
	//step 2 - control regs - enable interrupts , and compare mode
	if (mode == PART_A){
		TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
	}
	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

	//Step 3 - Select Aclk and select up mode
	TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_1 | TIMER_A_CTL_MC_2; // tassel - select clock src, mc - select up mode

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

	set_everything(PART_A);
	while(1);

}


