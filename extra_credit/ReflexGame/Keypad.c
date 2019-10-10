#include "delay.h"
#include "LCD.h"
#include "Keypad.h"
/*P5 = cols*/





void Keypad_init(){
    // Use P6 for rows and P5 for cols
    // Step 1 - init P5 as output port
    P5->DIR |= (COL1 | COL2 | COL3);
    P5->OUT &= ~(COL1 | COL2 | COL3); // set columns low

    // Step 2 - inti the inputs
    P6->DIR &= ~(ROW1 | ROW2 | ROW3 | ROW4); // row inputs
    P6->REN |= (ROW1 | ROW2 | ROW3 | ROW4); // enable resistor
    P6->OUT &= ~(ROW1 | ROW2 | ROW3 | ROW4); // pull down

    // Step 3 - enable interupts
    P6->IE |= (ROW1 | ROW2 | ROW3 | ROW4); // pull down
    // Step 4 - interrupt on rising edge
    P6->IES |=(ROW1 | ROW2 | ROW3 | ROW4);

}



int get_row(int row, int size){
    int i;
    for (i = 0; i < size; i++)
        if (row >> i == 1)
           return i;

    return -1;
}
int count_ones(int rows, int size){

    int i, count=0;
    for (i=0; i < size; i++)
        if (rows & (ROW1 << i) == 1)
            count++;

    return count;
}

/* returns : {'0'-'9', #, *} if normal operation
 *         : {-1 if no btn pressed}
 *         : {'\0' if multiple}

 */
signed char read_key(){
    // Step 1 - for each column setting high and reading rows
    int col, rows;
    for (col=0; col < 3; col++){
        P5->OUT &= ~(COL1 | COL2 | COL3); // set columns low
        P5->OUT |= (COL1 << col);
        __delay_cycles(25);
        // Cond 1 - if a btn is pressed
        if((rows=P6->IN & 0x0F) > 0)
            break;
    }
    if (col == 3 )
        return -1;

    /*
    // cond 2 - if two btns pressed
    if (count_ones(rows, 4) > 1)
        return 0;

    */
    // Step 2 - find what btn is pressed
    return lookup[get_row(rows, 4)][col];
}

void display_key_press_LCD(){

    signed char key;
   if ((key = read_key())== 0){
        Write_string_LCD("Mult Press");
    }else if (key == -1){
        //Write_string_LCD("Nothing");
    }else
        Write_char_LCD(read_key());
}

/**
 * main.c
 */

