#include "msp.h"
#include "delay.h"


#define EN BIT7
#define RW BIT6
#define RS BIT5

#define DB7 BIT3
#define DB6 BIT2
#define DB5 BIT1
#define DB4 BIT0

#define N 0
#define F 0
#define B 0
#define C 0
#define D 0
#define S 0
#define I_D 1



/*
 *
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

	// Step 1 - select the P4->out
	P4->SEL0 &= ~BIT0 & ~BIT1 & ~BIT2 & ~BIT3;
	P4->SEL1 &= ~BIT0 & ~BIT1 & ~BIT2 & ~BIT3;
	// Step 2 - turn direction on
	P4->DIR |= BIT0 | BIT1 | BIT2 | BIT3;
	// Step 3 - selectthe P2->out
	P2->SEL0 &= ~BIT7 & BIT6 & BIT5;
	P2->SEL1 &= ~BIT7 & BIT6 & BIT5;
	// step 4 - turn direction on
	P2->DIR |= BIT7 | BIT6 | BIT5;


}


// arr = [DB4, DB5, DB6, DB7]

// helper function for init_lcd
void set_LCD(int wait_time, int arr[4]){
		delay_us(wait_time);
		P2->OUT &= ~(EN | RS | RW); // set EN, RS, RW, low
		int i;
		for (i = 0; i < 4; i++){
			if (arr[i] == 0){
				P4->OUT &= arr[i];
			}else{
				P4->OUT |= arr[i];
			}
		}
		P2->OUT |= EN; // pulse EN
		delay_us(0); // delay >= 480ns
		P2->OUT &= ~EN; //turn off enable
}

//helper function for init lcd
void display_ON_OFF_CTRL(){
	set_LCD(37, [~DB4, ~DB5, ~DB6, ~DB7]);
	set_LCD(0, [B, C, D, DB7]);
}

// Clear the display
void Clear_LCD(){
	set_LCD(37, [~DB4, ~DB5, ~DB6, ~DB7]);
	set_LCD(37, [DB4, ~DB5, ~DB6, ~DB7]);
}

void Entry_mode_set(){
	set_LCD(152000, [~DB4, ~DB5, ~DB6, ~DB7]);
	set_LCD(0, [S, I_D, DB6, ~DB7]);
}
void Init_LCD(){
	set_LCD(40000,[DB4, DB5, ~DB6, ~DB7]);
	//====== For function set ========
	set_LCD(37,[~DB4, DB5, ~DB6, ~DB7]);
	set_LCD(0,[0, 0, F, N]);
	//====== For function set ========
	set_LCD(37,[~DB4, DB5, ~DB6, ~DB7]);
	set_LCD(0,[0, 0, F, N]);
	//====== For Display  ON/OFF control===
	display_ON_OFF_CTRL();
	//====== Display clear=============
	Clear_LCD();
	//=======Entry mode set===========
	Entry_mode_set();
}
// move the cursor to the top left of the LCD
void Home_LCD(){

}
// Write a char to the lcd
void Write_char_LCD(){

}
// write a string to a speicified location on the lcd 
void Write_string_LCD(){

}
void main(void)
{
	Init_LCD();

}
