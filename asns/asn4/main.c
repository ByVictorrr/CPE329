#include "msp.h"

/*P5 = ROWS*/


#define COL1 BIT0
#define COL2 BIT1
#define COL3 BIT2

/* P4 = COLS*/
#define ROW1 BIT0
#define ROW2 BIT1
#define ROW3 BIT2
#define ROW4 BIT3


void Keypad_init(){
	// Use P4 for rows and P5 for cols
	// Step 1 - init P5 as output port
	P5->DIR |= (COL1 | COL2 | COL3);
	P5->OUT &= ~(COL1 | COL2 | COL3); // set columns low

	// Step 2 - inti the inputs 
	P4->DIR &= ~(ROW1 | ROW2 | ROW3 | ROW4); // row inputs
	P4->REN |= (ROW1 | ROW2 | ROW3 | ROW4); // enable resistor
	P4->OUT &= ~(ROW1 | ROW2 | ROW3 | ROW4); // pull down 
}

/* returns : {'0'-'9', #, *} if normal operation
 *		   : {-1 if no btn pressed}
 *		   : {'\0' if multiple}
 
 */
char lookup[4][3] = {
			{'1','2','3'},
			{'4','5','6'},
			{'7','8', '9'},
			{'*', '0', '#'}
};
char read_key(){
	// Step 1 - for each column setting high and reading rows
	int col, rows;
	for (col=0; col < 3; col++){
		P5->OUT &= ~(COL1 | COL2 | COL3); // set columns low
		P5->OUT |= (COL1 << col);
		__delay_cycles(25);
		// Cond 1 - if a btn is pressed
		if((rows=P4->IN & 0x0F) > 0)
			break;
	}
	if (col == 3 )
		return -1;
	
	// cond 2 - if two btns pressed 
	if (count_ones(rows, 4) > 1)
		return '\0';
	
	// Step 2 - find what btn is pressed	
	return lookup[get_row(rows, 4)][col];
}

int get_row(int row, int size){
	int i;
	for (i = 0; i < size; i++)
		if (row >> i == 1)
		   return i

	return -1;
}
int count_ones(int rows, int size){

	int i, count=0;
	for (i=0; i < size; i++)
		if (rows & (ROW1 << i) == 1)
			count++;

	return count;
}

void display_key_press_LCD(){

	char key;
	if ((key = read_key()) == '\0')
		Write_string_LCD("Two btns pressed");
	else if (key == -1)
		Write_string_LCD("Btn not pressed");
	else
		Write_char_LCD(key);

	delay(100000);
}

/**
 * main.c
 */
void main(void)
{

	while (1){
		display_key_press_LCD();
	}
}
