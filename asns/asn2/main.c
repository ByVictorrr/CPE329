#include "msp.h"
#include "delay.h"


/**
 * main.c
 */


void main(void)
{


    // Step 1 - set up DCO
    set_DCO(FREQ_1_5_MHZ);
    // Step 2 - select right CTL1 values so that MCLK outputs the DCO
    // Step 2.1 - set SELM =
    // Step 2.2 - DIVM = 1
    CS -> KEY = CS_KEY_VAL;
    CS -> CTL1 = CS_CTL1_SELM_3 |  CS_CTL1_DIVM__1;
    CS -> KEY = ~CS_KEY_VAL;

    // Step 3 - set gpio out  SEL = 01
    // Step 3.1 - first select what we want to output
    P4->SEL0 |= BIT3; //
    P4->SEL1 &= ~BIT3;//;
    //step 3.2 - connect MCLK to from module
    //Step 3.3 - set direction to output
    P4->DIR |= BIT3;

    P4->SEL0 &= ~BIT1;
    P4->SEL1 &= ~BIT1;
    P4->DIR |= BIT1;


    // Step 4 - set the led high and low
    P2->SEL0 &= ~BIT0;
    P2->SEL1 &= ~BIT0;
    P2->DIR |= BIT0 | BIT1 | BIT2;

    while (1){
        //P4->OUT |= BIT1;
        P2->OUT ^= BIT0 | BIT1 | BIT2;
        delay_us(arr[5]);
        //P4->OUT &= ~BIT1;
        //delay_us(0);
    }

}



