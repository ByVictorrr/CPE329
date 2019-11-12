#include "msp.h"
#include "delay.h"
#define TRIG BIT4
#define ECHO BIT5




void PORT1_IRQHandler(){
	static int countSonicBursts = 0;
	// Cond 1 - where we are going to read the length of the echo pulse
	/*
	if (countSonicBursts  >= 8){
		//Step 1 - start Timer 
		// Step 2 - get echo pulse high time
		countSonicBursts = 0;
	}else{
	    countSonicBursts++;
	}
	*/
	 countSonicBursts++;
	// Last step clear echo flag
	P1->IFG &= ~ECHO;
}

void send_trigger(){
	P2->OUT|= TRIG;
	delay_us(0);
	P2->OUT&=~ TRIG;
}


/* P2.4 - output (trig)
 * P1.5 - input (echo)
 *
 *
 *
 * 
 */
void init_ECHO(){
	// Step 1 - init gpio
	P1->SEL0 &= ~ECHO;
	P1->SEL1 &= ~ECHO;
	P1->DIR &= ~ECHO;
	// Pull down resistor
	P1->OUT &= ~ECHO; // Select Vss
	P1->REN |= ECHO;
	// Step 2 - setup interupts on rising edge (for P1.5)
	P1->IES &= ~ECHO; // set IFG when 
	// step 3 - setup interrupts
	P1->IE |= ECHO;
	P1->IFG &= ~ECHO;
	NVIC->ISER[1] = (1 << (PORT1_IRQn & 0x1F));

}
void init_TRIGGER(){
	// Step 1 - init gpio
	P2->SEL0 &= ~TRIG;
	P2->SEL1 &= ~TRIG;
	P2->DIR |= TRIG;
	P2->OUT &= TRIG; // init low
}

void init_UtraSonicSensor(){
	// Step 1 - setup gpio 
	init_ECHO();
	init_TRIGGER();
	// step 2 -  


}

/**
 * main.c
 */
void main(void)
{
	init_UtraSonicSensor();
	__enable_irq();
	
	send_trigger();
	while(1);


}
