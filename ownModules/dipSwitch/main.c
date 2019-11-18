#include "msp.h"

#define DIP1 BIT1
typedef enum{NORMAL_MODE, RESTART_MODE} dip_state;
dip_state dip_switch = NORMAL_MODE;

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
void turn_onLED(){
	P1->OUT|=BIT0;
}
void turn_offLED(){
	P1->OUT&=~BIT0;
}

/*  P4.1 - dip switch 1 
 *
 *
 *
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
	NVIC->ISER[1] = (1 << (PORT4_IRQn & 0x1f));
}

/**
 * main.c
 */
void main(void)
{
    init_DipSwitch();
	// ==debugging===
	P1->DIR|=BIT0;
	//=============
	__enable_irq();

	while(1){
		if (dip_switch == RESTART_MODE){
			turn_onLED();
		}else{
			turn_offLED();
		}
	}
}
