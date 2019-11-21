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

void rotate(int revolutions, direction_t direction){
    int i;
    set_direction(direction);
    for (i = 0; i < abs(revolutions); i++)
        step(STEP_PER_REV);
}

