#include "msp.h"
#include "delay.h"
#include <math.h>
#define TRIG BIT5
#define SPEED_OF_SOUND
#define F_INPUT 3000000
#define TIMER_A_MAX 0xffff


uint16_t rising_edge_counter
          ,falling_edge_counter
          ,overflows[2] = {0}
          ,overflow = 0;

/* P2.5 - output (trig)
 * P7.3 - input(TA0.CCI0A) (echo)
 * P2.4 - input(TA0.ccI1A) (echo)
 *
 *
 *
 */
void init_ECHO(){

   // step 1 - init CCI0A
    P7->SEL0 |= BIT3;
    P7->SEL1 &= ~BIT3;
    // step 2 - init CCI1A
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;

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


void send_trigger(){
    P2->OUT|= TRIG;
    delay_us(0);
    P2->OUT&=~ TRIG;
}


uint16_t get_time_high(){

    uint16_t diff = abs(rising_edge_counter - falling_edge_counter);

    return diff/(F_INPUT/pow(10,6));
}

double get_distance_cm()
{
  uint16_t time_high = get_time_high();
  float distance = time_high*(.034/2.0);

  return distance;

}




// Configure ccr0 and ccr1
void init_TA0(){

        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, continuous mode
                        TIMER_A_CTL_MC__CONTINUOUS|
                        TIMER_A_CTL_TASSEL_2 | //SMCLK
                        TIMER_A_CTL_IE; // enable TAxR overflow

        TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CM_1 | //Trigger on rising edge
                            TIMER_A_CCTLN_CCIS_0 | //select CCIxA
                            TIMER_A_CCTLN_CCIE | //enable interrupt
                            TIMER_A_CCTLN_CAP | // capture mode
                            TIMER_A_CCTLN_SCS; //synchronous

		TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CM_2 | //Trigger on rising falling edge
                            TIMER_A_CCTLN_CCIS_0 | //select CCIxA
                            TIMER_A_CCTLN_CCIE | //enable interrupt
                            TIMER_A_CCTLN_CAP | // capture mode
                            TIMER_A_CCTLN_SCS; //synchronous


        NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
        NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
        __enable_irq();
}

// Description: for ccr1 handler and r overflow
void TA0_N_IRQHandler(){
	// Step 1 - check to see the interrupt is from ccr
	if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG){
	    overflows[1] = overflow;
	    falling_edge_counter = TIMER_A0->CCR[1] + (overflows[1] - overflows[0])*TIMER_A_MAX;
		TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
		overflow = 0;
	// Step 2 - else its from overflow
	}else{
		overflow++;
		TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; // clear interrupt flag
	}
}
//description: for ccr0 handler and r overflow
void TA0_0_IRQHandler(){
    // Step 1 - check to see the interrupt is from ccr
        overflows[0] = overflow;
        rising_edge_counter = TIMER_A0->CCR[0];
        TIMER_A0->CCTL[0] &=  ~TIMER_A_CCTLN_CCIFG;
}


/**
 * main.c
 */
void main(void)
{
    init_TA0();
    set_DCO(F_INPUT/pow(10, 6)); // 3mhz
    set_clk("SMCLK");
    init_UtraSonicSensor();
    __enable_irq();

    float distance = 0;
        // Debugging
        P1->DIR|=BIT0;
	while(1){
	    send_trigger();
	    /*
	    delay_us(3000);
	    if((distance = get_distance_cm()) != 0){
	        P1->OUT|=BIT0;
	    }
	    */
	}
}
