#include "Stepper.h"
#include "delay.h"
#include <math.h>

void set_direction(direction_t direction){
    if (direction == CW){
        P5->OUT&=CW_PIN;
    }else{
        P5->OUT|=CCW_PIN;
    }
}

void init_Stepper(direction_t direction){
    P5->SEL0 &= ~(STEP | DIRECTION);
    P5->SEL1 &= ~(STEP | DIRECTION);
    P5->DIR|= (STEP | DIRECTION);
    set_direction(direction);

}
void step(int steps){
    int i;
    for(i=0; i< steps; i++){
        P5->OUT|=STEP;
        delay_us(500); // delay 1us
        P5->OUT&=~STEP;
        delay_us(500);
    }
}

void rotate(float revolutions, direction_t direction){
    int i, integral_revolutions = (int)revolutions;
    float decimal_revolutions = revolutions - integral_revolutions;
    set_direction(direction);
    // Case 1 - for just the integral part
    for (i = 0; i < abs(integral_revolutions); i++)
        step(STEP_PER_REV);
    // Case 2 - for the decimal part of revolutions
    /* Had to get rid of because sensors not accurate engough
    if (decimal_revolutions)
        step(abs(STEP_PER_REV*decimal_revolutions));
        */
}

