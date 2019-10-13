#include "msp.h"
#include "delay.h"
#include "Keypad.h"
#include "LCD.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CLK_FREQ 24
void swap(char *str1, char *str2){
    char str_temp[strlen(str1)];
    memcpy(str_temp, str1, strlen(str1)); //str_temp = str1
    memcpy(str1, str2, strlen(str2)); //str_temp = str1
    memcpy(str2, str_temp, strlen(str_temp));
}

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap((str+start), (str+end));
        start++;
        end--;
    }
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}




int over_fl = 0;

// 6 and 5 is used for keypad
// 2 and 4 are used for lcd 
// use 1 and 3 for comparison 


//steP4: user A press a button, trigger that Port Interupt,
//interupt would start timer_a0 timer.


int toggle = 0;
int timer_int;


void PORT6_IRQHandler(void){
    if (toggle == 0){
        //startTimer(&timer_int);
		/*start timer_a0 here*/
		TIMER_A0 -> CTL |= TIMER_A_CTL_IE;
		TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_2; // tassel - select clock src, mc - select continuous mode
		// do a TA CLR here 
		TIMER_A0 -> CTL |= TIMER_A_CTL_CLR;	

        toggle = 1;
		TIMER_A0 -> R = 0;
		TIMER_A0 ->CTL&=~TIMER_A_CTL_IFG;
	    P6-> IFG &= ~ROW1;


	}else{
         //stopTimer(&timer_int);
	    /*stop timer here */
		int final_cycle_count =0;
		int final_time = 0;
		char buffer[30];
		//(*((volatile uint16_t *)(0x40000010))))
		final_cycle_count= TIMER_A0->R + (over_fl * 65535);
		// clock = 24mhz -> 1s = 24 000 000 cycle
		final_time = final_cycle_count/ (CLK_FREQ*10^6);
		

		itoa(final_time,buffer,10);
		Clear_LCD();
		delay_us(1000000);
		//Write_string_LCD("hi");

		delay_us(2000000);
		
		//reset the IFG flag for the next time the user play ( buttons pressed);
		over_fl = 0;
		P6 -> IFG &= ~(ROW1); // similar to TIMER_A0 -> CTL&= ~TIMER_A_CTL_IFG;
		//turn off timer A0 here;

         toggle =0;

		 // show time
		 //
	}
}
//step 2: while waiting until USER b press button, maybe timer_a0 overflow
//this handler will trigger ;

void TA0_N_IRQHandler(void){
    TIMER_A0 -> CTL&= ~TIMER_A_CTL_IFG;
    over_fl++;
}

//step 3:  USER B Press button, print out time and clear everything
//for next run



void set_everything(){
    
  //keypad init here 
 
    Init_LCD();
	// Step 0 - enable row 1
    P5->DIR |= (COL1);
    P5->OUT = COL1;

	P6->IES &= ~(ROW1);
	P6-> IFG &= ~(ROW1);
	P6-> IE |= (ROW1);

    // Step 2 - inti the inputs
    P6->DIR &= ~(ROW1); // row inputs
    P6->REN |= (ROW1); // enable resistor
    P6->OUT |= (ROW1);	// pull down

	// Step 4 = enable NVIC
    NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for TAIFG
    NVIC->ISER[1] = (1 << (PORT6_IRQn & 0x1F)); // for port 1

	// Step 5 - enable globally
	__enable_irq();


}

void main(void)
{
    Clear_LCD();
    delay_us(100000);
    Home_LCD();
    set_DCO(24.0); // 24Mhz
    set_clk("MCLK");
    set_everything();

    //do your button thing here;
	while(1);


}

