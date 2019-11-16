#include "msp.h"
#include "delay.h"
#include "Stepper.h"


void main(void)
{
    P5->SEL0 &= ~(STEP | DIRECTION);
    P5->SEL1 &= ~(STEP | DIRECTION);

    P5->DIR|= (STEP | DIRECTION);
    // Set the DIRECTIONection
    P5->OUT|=DIRECTION;
    while(1){
        rotate(20, CW);
        delay_us(10000);
    }
}
