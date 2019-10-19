#ifndef KEYPAD_H_
#define KEYPAD_H_
#include "msp.h"
/*P5 = cols*/

static const signed char lookup[4][3] = {
            {'1','2','3'},
            {'4','5','6'},
            {'7','8', '9'},
            {'*', '0', '#'}
};

#define COL1 BIT0
#define COL2 BIT1
#define COL3 BIT2

/* P6 = rows*/
#define ROW1 BIT0 //6.1
#define ROW2 BIT1
#define ROW3 BIT2
#define ROW4 BIT3



void Keypad_init();

int get_row(int row, int size);
int count_ones(int rows, int size);
/* returns : {'0'-'9', #, *} if normal operation
 *         : {-1 if no btn pressed}
 *         : {'\0' if multiple}

 */
signed char read_key();
void display_key_press_LCD();
#endif 


