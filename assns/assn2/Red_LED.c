#include "msp.h"


// make p1.0 an output
void LED_Init(){
	P1->SEL0 &= ~0x01;
	P1->SEL1 &= ~0x01;
	P1->DIR |= 0x01;
}
void LED_ON(){
	LED_Init();
	P1->OUT = 0x01;
}
/**
 * main.c
 */
void main(void)
{
	LED_ON();
}
