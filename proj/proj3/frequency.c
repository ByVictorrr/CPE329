#include "msp.h"
#include "UART.h"
#include "ADC.h"
#include "delay.h"
#include <math.h>
#include "UART.h"
#include <stdio.h>


//===============================frequence stuff (freq stuff) ==================================\\

float f_wave = 0;
int freq_flag = 0;
int ta1_ifg_overflows = 0;

uint32_t st_r = 0.0;
uint32_t en_r = 0.0;
uint32_t hi_lo = 1;


// Description: TA1 timer init
void init_TA1(){
	//Configure Timer 0 for interrupt
	TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, continuous mode
					TIMER_A_CTL_MC__CONTINUOUS|
					TIMER_A_CTL_TASSEL_2 | //SMCLK
					TIMER_A_CTL_IE; // enable TAxR overflow

	TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CM_1 | //Trigger on rising edge
						TIMER_A_CCTLN_CCIS_0 | //select CCIxA
						TIMER_A_CCTLN_CCIE | //enable interrupt
						TIMER_A_CCTLN_CAP | // capture mode
						TIMER_A_CCTLN_SCS; //synchronous

	NVIC->ISER[0] = 1 << ((TA1_N_IRQn) & 31);
	NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31);
	__enable_irq();
}
// Description: for flag overflows
void TA1_N_IRQHandler(){
	ta1_ifg_overflow++;
    TIMER_A1->CTL &= ~TIMER_A_CTL_IFG; // clear interrupt flag
}
// Description: for a given frequency calibrated lookup table
uint32_t getRealFreq(float wavefreq){
    uint32_t real_f = 0;
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
// Description: checks to see if first rising edge captured 
// if see check if cov is one is so repeat till not cov
void TA1_0_IRQHandler(){
      if (hi_lo == 1){//not first point
          st_r = (0xffff)*overflow_ta0 + TIMER_A1->CCR[0];
          hi_lo =0;
      }else { //second point
          if (TIMER_A1->CCTL[0] & COV){
              TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_COV;
              overflow_ta0 = 0;
              TIMER_A1->CTL |= TIMER_A_CTL_CLR;
              hi_lo = 1;
          }else{
            en_r = (0xffff)*overflow_ta0 + TIMER_A1->CCR[0];
            f_wave =   F_INPUT/((en_r - st_r));
            f_wave = getRealFreq(f_wave);
            hi_lo = 1;
            overflow_ta0 = 0;
            TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
            freq_flag = 1;
            if (f_wave > 1000){
                hi_lo =1;
            }

          }
      }

      TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}
// Description: disable TA1 after enabling sampling module
void disable_TA1(){
    TIMER_A1->CTL &= ~TIMER_A_CTL_IE; // disable interputs for TAxR overflowing
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIE; // disable interupts
}

// Description: main function to get frequency stored in f_wave
void get_freq(){
    init_TA1();
    while(freq_flag == 0);
    disable_TA1();
}

