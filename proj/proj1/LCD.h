#ifndef LCD_H_
#define LCD_H_

#include "msp.h"



/*
DB7:P4.3
DB6:P4.2
DB5:P4.1
DB4:P4.0
E: P2.7
R/W:P2.6
RS:P2.5
*/


#define EN BIT7
#define RW BIT6
#define RS BIT5

#define DB7 BIT3
#define DB6 BIT2
#define DB5 BIT1
#define DB4 BIT0

#define N ~DB7
#define F ~DB6

#define B DB4
#define C DB5
#define D DB6

#define S ~DB4
#define I_D DB5

#define L_R DB6
#define S_C DB7

/*
DB7:P4.3
DB6:P4.2
DB5:P4.1
DB4:P4.0

E: P2.7
R/W:P2.6
RS:P2.5
*/


void set_outputs();

// arr = [DB4, DB5, DB6, DB7]

// helper function for init_lcd
void set_LCD(int wait_time, uint16_t *arr);
void display_ON_OFF_CTRL();

void Clear_LCD();

void Entry_mode_set();
void Init_LCD();
void Home_LCD();
void Write_char_LCD(char c);
void Write_string_LCD(char *str);
void next_line_pos();


#endif
