#include "msp.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>


/*
DB7:P4.3
DB6:P4.2
DB5:P4.1
DB4:P4.0

E: P2.7
R/W:P2.6
RS:P2.5
*/



/**
 * main.c
 */

void set_outputs(){


    // Step 2 - turn direction on
    P4->DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
   // P4->OUT &= (BIT0 | BIT1 | BIT2 | BIT3);
    // Step 3 - selectthe P2->out

   // P2->OUT &= ~(BIT7 | BIT6 |BIT5);
    // step 4 - turn direction on
    P2->DIR |= (BIT7 | BIT6 | BIT5);
}


// arr = [DB4, DB5, DB6, DB7]

// helper function for init_lcd
void set_LCD(int wait_time, uint16_t *arr){
        delay_us(wait_time);
        P2->OUT &= ~(EN | RS | RW); // set EN, RS, RW, low
        int i;
        for (i = 0; i < 4; i++){
            if (arr[i] == BIT0 | arr[i] == BIT1 | arr[i] == BIT2 | arr[i] == BIT3){
                P4->OUT |= arr[i];
            }else{
                P4->OUT &= arr[i];
            }
        }
        P2->OUT |= EN; // pulse EN
        delay_us(0); // delay >= 480ns
        P2->OUT &= ~EN; //turn off enable
}

//helper function for init lcd
void display_ON_OFF_CTRL(){
    uint16_t arr[2][4] = {{~DB4, ~DB5, ~DB6, ~DB7},{B, C, D, DB7}};
    set_LCD(1, arr[0]);
    set_LCD(0, arr[1]);
}

// Clear the display
void Clear_LCD(){
    uint16_t arr[2][4] = {{~DB4, ~DB5, ~DB6, ~DB7},{DB4,  ~DB5, ~DB6, ~DB7}};
    set_LCD(1, arr[0]);
    set_LCD(0, arr[1]);
}

void Entry_mode_set(){
    uint16_t arr[2][4] = {{~DB4, ~DB5, ~DB6, ~DB7},
                     {S, I_D, DB6, ~DB7}};
    set_LCD(1520,arr[0]);
    set_LCD(0, arr[1]);
}
void Init_LCD(){
    uint16_t arr[5][4] = {
                     {DB4, DB5, ~DB6, ~DB7},
                     {~DB4, DB5, ~DB6, ~DB7},
                     {~DB4, ~DB5, F, N},
                     {~DB4, DB5, ~DB6, ~DB7},
                     {~DB4, ~DB5, F, N}
    };
    set_LCD(40003,arr[0]);
    //====== For function set ========
    set_LCD(1,arr[1]); // 1 actually 53 microsecond
    set_LCD(0,arr[2]);
    //====== For function set ========
    set_LCD(1,arr[3]);
    set_LCD(0,arr[4]);
//    //====== For Display  ON/OFF control===
    display_ON_OFF_CTRL();
//    //====== Display clear=============
    Clear_LCD();
//    //=======Entry mode set===========
    Entry_mode_set();
}
// move the cursor to the top left of the LCD
void Home_LCD(){
	uint16_t arr[][4] = {{~DB4,~DB5,~DB6,~DB7},{DB4, I_D, DB6, ~DB7}};
	set_LCD(1520, arr[0]);
	set_LCD(1520, arr[1]);

}
// Write a char to the lcd
void Write_char_LCD(char c){
    delay_us(0);
    P4->OUT = c >> 4;
    P2->OUT &= ~(EN | RS | RW); // set EN, RS, RW, low
    P2->OUT |= RS; // NEON

    P2->OUT |= EN; // pulse EN
    delay_us(0); // delay >= 480ns
    P2->OUT &= ~EN; //turn off enable

    P4->OUT = c;
    P2->OUT &= ~(EN | RS | RW); // set EN, RS, RW, low
    P2->OUT |= RS; // NEON

    P2->OUT |= EN; // pulse EN
    delay_us(0); // delay >= 480ns
    P2->OUT &= ~EN; //turn off enable

}

// write a string to a specified location on the lcd
void Write_string_LCD(char *str){
    int i = 0;
    for (i=0 ; i < strlen(str); i++){
        Write_char_LCD(str[i]);
    }
}
