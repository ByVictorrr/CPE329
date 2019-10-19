#include "msp.h"
#include "delay.h"
#include <math.h>
#include "DAC.h"


// Handler for CCR0
void TA0_0_IRQHandler(void){

    // Step 0 - for debuging
    P1->OUT|=BIT0;
    // Step 1 - check if  counter over 21 if so reset it to zero else set it to itself + 1
    counter = counter >= LEN - 1 ? 0 : counter+1;
    //step 3 - turn off capture/compare interrupt flag(to trigger again on rising edge)
    TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    // Step 2 - add 960 cycles
    TIMER_A0 -> CCR[0] = CCR0; // up mode just set it equal to it self
    P1->OUT&=~BIT0;
}

void main(void)
{
    P1->DIR |= _CS | BIT0;
    P1->OUT &= ~BIT0;
    set_clk("SMCLK");
    set_DCO(F_INPUT);
    // Step 1 - init SPI
    init_SPI();
    init_TimerA();
    // For triangle wave
    //float voltages[LEN] = {0, .1, .3, .5, .7, .9, 1.1, 1.3, 1.5, 1.7, 1.9, 1.7, 1.5, 1.3, 1.1, .9, .7, .5,.3};

    float voltages[LEN];

    float sine_voltages[LEN];
    //memset(sine_voltages, 0, LEN);
    gen_arrays(sine_voltages, LEN, .025, FALSE, cos); // generates a good sine wave

    //

    while(1){
        send_to_DAC(voltage_to_dacData(sine_voltages[counter]));
        //send_to_DAC(0xFFF | SHDN | GAIN);
    }

}
