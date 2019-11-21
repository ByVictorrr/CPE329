#include "msp.h"
#include "Debug.h"
void turn_onLED(){
    P1->OUT|=BIT0;
}
void turn_offLED(){
    P1->OUT&=~BIT0;
}
