#include "msp.h"
#include "LCD.h"
#include "Keypad.h"
#include "delay.h"

void main(void)
{

    Init_LCD();
    Keypad_init();
    while (1){
        display_key_press_LCD();
        delay_us(400000);
    }
}

