#include "Stepper.h"
#include "delay.h"

void set_direction(int direction){
    if (direction == CW){
        P5->OUT&=CW;
    }else{
        P5->OUT|=CCW;
    }
}

void init_Stepper(int direction){
    P5->SEL0 &= ~(STEP | DIRECTION);
    P5->SEL1 &= ~(STEP | DIRECTION);
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

void rotate(int revolutions, int direction){
    int i;
    set_direction(direction);
    for (i = 0; i < revolutions; i++)
        step(STEP_PER_REV);
}

