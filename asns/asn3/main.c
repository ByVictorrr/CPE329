#include "msp.h"
#include "delay.h"
#include "LCD.h"




void main(void)
{
    set_outputs();
    Init_LCD();

    Write_string_LCD("OwO");
    delay_us(1000000);
    Clear_LCD();
    delay_us(1000000);
    Write_string_LCD("What's this UwU");
}
