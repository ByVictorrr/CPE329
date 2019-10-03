#include "msp.h"
#include "delay.h"
#include "LCD.h"


void main(void)
{
    set_outputs();
    Init_LCD();
    Write_char_LCD('8');
    delay_us(10000);
}
