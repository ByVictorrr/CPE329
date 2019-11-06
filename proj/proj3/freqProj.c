/*
#include "msp.h"
#include "delay.h"
#include "UART.h"
#include <math.h>


float f_x = 0;
int overflow_ta0 = 0;


uint32_t st_r = 0.0;
uint32_t en_r = 0.0;
uint32_t hi_lo = 1;

#define F_INPUT 1500000.0


void init_TA0(){
    //Configure Timer 0 for interrupt

        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, continuous mode
                TIMER_A_CTL_MC__CONTINUOUS|
                TIMER_A_CTL_IE; // enable TAxR overflow

        TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CM_1 | //Trigger on rising edge
                TIMER_A_CCTLN_CCIS_0 | //select CCIxA
                TIMER_A_CCTLN_CCIE | //enable interrupt
                TIMER_A_CCTLN_CAP | // capture mode
                TIMER_A_CCTLN_SCS; //synchronous

        TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | //SMCLK
                TIMER_A_CTL_MC__CONTINUOUS;

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

#define RANGE 1..6
uint32_t getRealFreq(float wavefreq){
    uint32_t real_f = 0;

    // real ranges [0,46]
    if (wavefreq > 0 && wavefreq < 9.37){
        real_f = 2.09*wavefreq - .107;
    }else if(wavefreq> 9.37 && wavefreq <= 16){
        real_f = 3.49*wavefreq-14.4;
    }else if(wavefreq >16 && wavefreq <=99 ){
        real_f = (int)wavefreq;
    }else if (wavefreq>99 && wavefreq <= 170 ){
        real_f = ceil(wavefreq);
    }else if (wavefreq>170 && wavefreq <= 300){
        real_f = ceil(wavefreq) + 1;
    }else if (wavefreq>300 && wavefreq <= 430){
        real_f = ceil(wavefreq) + 2;
    }else if (wavefreq>430 && wavefreq <= 590){
        real_f = ceil(wavefreq) + 3;
    }else if (wavefreq>590 && wavefreq <=700){
        real_f = ceil(wavefreq) + 4;
    }else if (wavefreq >700 && wavefreq <=830){
        real_f = ceil(wavefreq) + 5;
    }else if (wavefreq >830 && wavefreq <=1000){
        real_f = ceil(wavefreq) + 6;
    }

    return real_f;
}


// Description: checks if over overflow if so 
void TA0_0_IRQHandler(){

      if (hi_lo == 1){//not first point
          st_r = (0xffff)*overflow_ta0 + TIMER_A0->CCR[0];

          hi_lo =0;
      }else { //second point
          if (TIMER_A0->CCTL[0] & COV){
              TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_COV;
              overflow_ta0 = 0;
              TIMER_A0->CTL |= TIMER_A_CTL_CLR;
              hi_lo = 1;
          }else{
            en_r = (0xffff)*overflow_ta0 + TIMER_A0->CCR[0];
            f_x =   F_INPUT/((en_r - st_r));
            f_x = getRealFreq(f_x);
            hi_lo = 1;
            overflow_ta0 = 0;
            TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
            if (f_x > 1000){
                hi_lo =1;
            }

          }
      }

      TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
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
*/
