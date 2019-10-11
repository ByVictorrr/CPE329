#include "msp.h"
#include "delay.h"


int over_fl = 0;

// 6 and 5 is used for keypad
// 2 and 4 are used for lcd 
// use 1 and 3 for comparison 


//step1: user A press a button, trigger that Port Interupt,
//interupt would start timer_a0 timer.

void PORT1_IRQHandler(void){
    
    /*start timer_a0 here*/
    TIMER_A0 -> CTL |= TIMER_A_CTL_IE;
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_2; // tassel - select clock src, mc - select up mode
    // do a TA CLR here 
    
    
}

//step 2: while waiting until USER b press button, maybe timer_a0 overflow
//this handler will trigger ;

void TA0_N_IRQHandler(void){
    TIMER_A0 -> CTL&= ~TIMER_A_CTL_IFG;
    over_fl++;
}

//step 3:  USER B Press button, print out time and clear everything
//for next run

void PORT3_IRQHandler(void){

    /*stop timer here */
    int final_cycle_count =0;
    int final_time = 0;
    
    final_cycle_count = TIMER_A0-> TA0R + (over_fl * 65535);
    // clock = 24mhz -> 1s = 24 000 000 cycle
    final_time = final_cycle_count/ clockfrequency;
    
    Write_String_LCD(itoa(final_time));

    delay_us(2000000);
    
    //reset the IFG flag for the next time the user play ( buttons pressed);
    over_fl = 0;
    P1 -> IFG &= ~BIT0; // similar to TIMER_A0 -> CTL&= ~TIMER_A_CTL_IFG;
    P3 -> IFG &= ~BIT0;
    //turn off timer A0 here;
}





void set_everything(){
    
  //keypad init here 
  //lcd init here 
    
	// step 0 - set up the GPIO
  P1->DIR |= BIT0; // setup p4 dir to be output
	P1->SEL0 &= ~BIT0;
	P1->SEL1 &= ~BIT0;
  P1-> IE |= BIT 0;
  P1-> IFG &= ~BIT0;

	// step 0 - set up the GPIO
	P3->DIR |= BIT0; // setup p4 dir to be output
	P3->SEL0 &= ~BIT0;
	P3->SEL1 &= ~BIT0;
    P3-> IE |= BIT 0;
    P3-> IFG &= ~BIT0;    
    
    //now everytime you change P3->OUT manually, it actually sets P3 ->IFG high
    // 12.2.7 TRM page 680 said so 
    // do this in main i guess
    //if button 1 is pressed p1-> bit 0 high which trigger interupt  
    //if button 3 is pressed p3-> bit 0 out high 
    

	// Step 4 = enable NVIC

    NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for TAIFG
    NVIC->ISER[0] = (1 << (PORT1_IRQn & 0x1F)); // for port 1
    NVIC->ISER[0] = (1 << (PORT3_IRQn & 0x1F)); // for port 3

	// Step 5 - enable globally
	__enable_irq();


}

void main(void)
{


    set_DCO(24.0); // 24Mhz
    set_clk("MCLK");
    set_everything();

    //do your button thing here;


	while(1);


}

