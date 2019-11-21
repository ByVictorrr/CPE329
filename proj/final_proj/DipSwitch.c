#include "DipSwitch.h"


// Dip switch
void PORT4_IRQHandler(){
	// step 1 - check if dip one
	if (P4->IFG & DIP1){
		// switch iES to other to trigger an turn led off
		// STATE 1 : turning on (low to high)
		if (!(P4->IES & DIP1)){
			dip_switch = RESTART_MODE;
			P4->IES|=DIP1;
		// STATE 2 : turning off (high to low)
		}else{
		    P4->IES &= ~DIP1;
			dip_switch = NORMAL_MODE;
		}
		P4->IFG &=~DIP1;
	}

}

/*  P4.1 - dip switch 1  *
 * 
 */
void init_DipSwitch(){
	// step 1 - enable gpio
	P4->DIR&= ~DIP1;
	P4->SEL0&=~DIP1;
	P4->SEL1&=~DIP1;
	// step 2 - enable interrupts
	P4->IES&=~DIP1; // high to low
	P4->IE|= DIP1;
	//P4->IFG&=~DIP1;
	dip_switch = NORMAL_MODE;
	NVIC->ISER[1] = (1 << (PORT4_IRQn & 0x1f));
}

