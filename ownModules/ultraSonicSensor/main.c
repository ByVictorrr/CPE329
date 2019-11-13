#include "msp.h"
#include "delay.h"
#include <math.h>
#define TRIG BIT4
#define ECHO BIT5
#define SPEED_OF_SOUND
#define F_INPUT 3000000


double ms;
double time_high;
double distance_cm;


// Handler for CCR0
void TA0_0_IRQHandler(void){
    ms++;
    TIMER_A0->CCTL[0]&= ~TIMER_A_CCTLN_CCIFG;
}


void init_TimerA0(uint16_t ccr0){
    // Step 1 - set the inital cycles
    TIMER_A0->CCR[0] = ccr0; // cycles
    //step 2 - control regs - enable interrupts , and compare mode
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    //Step 3 - Select MCLK and select up mode
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1;
    // Step 4 = enable NVIC
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0

}


void PORT1_IRQHandler(){
	// step 1 - check to see if there is an interrupt pending
	if (P1->IFG & ECHO){
		// Step 2 - is this the rising edge
		if (!(P1->IES & ECHO)){
			// clear timer a
		    TIMER_A0 -> CTL|= TIMER_A_CTL_CLR;
			// set milis to zero
			ms = 0;
			// trigger an interrupt on the falling edge
			P1->IES |=  ECHO;
		// Step 3 - because this must mean its a falling edge
		}else{
		    int R = TIMER_A0->R;
			// calculate echo length
			time_high = ms*1000 + R;
		    //time_high = ms+(TIMER_A0->R*1000)/F_INPUT;
		    //distance_cm = 17*time_high;
		}
	}
	P1->IFG &= ~ECHO;
	//TIMER_A0->CCTL[0]&= ~TIMER_A_CCTLN_CCIE;
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
double get_distance_cm()
{
    // TIMER_A0->CCTL[0]|= TIMER_A_CCTLN_CCIE;
     send_trigger();
     P1->IES &= ~ECHO; // detect on rising edge
     P1->IFG &= 0; // clear ifg flage
     P1->IE |= ECHO;
     __delay_cycles(30000);
     if ((distance_cm = time_high/58)  < 20 && distance_cm !=0){
         P1->OUT|=BIT0;
     }else{
         P1->OUT&=~BIT0;
     }
}



/**
 * main.c
 */
void main(void)
{
	set_DCO(F_INPUT/pow(10, 6)); // 3mhz
	set_clk("SMCLK");
	init_UtraSonicSensor();
	init_TimerA0(F_INPUT * .001);
	__enable_irq();
	
	// Debugging
	P1->DIR|=BIT0;

	while(1){
	    /*
	   // TIMER_A0->CCTL[0]|= TIMER_A_CCTLN_CCIE;
	    send_trigger();
	    P1->IES &= ~ECHO; // detect on rising edge
	    P1->IFG &= 0; // clear ifg flage
	    P1->IE |= ECHO;
		__delay_cycles(30000);
		if ((distance_cm = time_high/58)  < 20 && distance_cm !=0){
		    P1->OUT|=BIT0;
		}else{
		    P1->OUT&=~BIT0;
		}
		*/



	}



}
