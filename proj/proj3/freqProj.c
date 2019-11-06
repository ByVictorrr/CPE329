#include "msp.h"
#include "delay.h"
#include "UART.h"


uint16_t f_x = 0;
int overflow_ta0 = 0;

#define F_INPUT 3000000


void init_TA0(){
    //Configure Timer 0 for interrupt

        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, continuous mode
                TIMER_A_CTL_MC__CONTINUOUS;

        TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CM_1 | //trigger on rising edge
                TIMER_A_CCTLN_CCIS_0 | //select CCIxA
                TIMER_A_CCTLN_CCIE | //enable interrupt
                TIMER_A_CCTLN_CAP |
                TIMER_A_CCTLN_SCS; //synchronous

        TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | //SMCLK
                TIMER_A_CTL_MC__CONTINUOUS;
                //TIMER_A_CTL_CLR |
                //TIMER_A_CTL_IE;

        NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
        NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

}

// Description: for flag overflows
void TA0_N_IRQHandler(){
    overflow_ta0++;
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; // clear interrupt flag
}


uint64_t TA0_R_i = 0;
uint64_t TA0_R_f = 0;

// Description: checks if over overflow if so 
void TA0_0_IRQHandler(){

    TA0_R_f = TIMER_A0->CCR[0];
    // Step 1 - get difference in cycles with overflow
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    // Step 3 - get freq
    f_x = F_INPUT/( TA0_R_f + (0xffff)*overflow_ta0 - TA0_R_i);
    overflow_ta0 = 0;
    //Step 4 - get initial rx
    TA0_R_i = TA0_R_f;
}

uint32_t getFreq(){
    int m = 1, b = 1;
    return f_x*m+b;
}

void main(){


    // step -1 - enable wave port
    P7->SEL0|=BIT3;
    P7->SEL1&=~BIT3;
    P7->DIR&=~BIT3;
    // step 0 - init timer a0 flag
    set_clk("SMCLK");
    set_DCO(F_INPUT/1000000);
    init_TA0();
    while(1);



}
