#include "msp.h"
#include "delay.h"

void TA0_0_IRQHandler(void){

}

/**
 * main.c
 */

void main(void)
{

    setDCO(24); // 24Mhz


	// Step 1 - set the inital cycles
	TIMER_A0->CCR[1] = 720; //cycles
	TIMER_A0->CCR[0] = 960; // cycles
	// XXX : review below
	//step 2 - control regs - enable interrupts , and compare mode
	TIMER_A0->CTL[1] = TIMER_A_CCTLN_CCIE;
	TIMER_A0->CTL[0] = TIMER_A_CCTLN_CCIE;


	//Step 3 - Select Aclk and select contious mode
	TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_1 | TIMER_A_CTL_MC_2;


	// Step 4 = enable NVIC
	NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0
	NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for other CCRs

	__enable_irq();

	while(1);




	


}


