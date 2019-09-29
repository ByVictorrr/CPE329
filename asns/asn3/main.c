#include "msp.h"
//#include "delay.h"


#define EN BIT7
#define RW BIT6
#define RS BIT5

#define DB7 BIT3
#define DB6 BIT2
#define DB5 BIT1
#define DB4 BIT0

#define N DB7
#define F DB6

#define B ~DB4
#define C DB5
#define D DB6

#define S DB4
#define I_D DB5



/*
DB7:P4.3
DB6:P4.2
DB5:P4.1
DB4:P4.0

E: P2.7
R/W:P2.6
RS:P2.5
*/

 ///////////////////////freq /////////

struct slopef{
    float freq;
    int b;
    float prop;
};

void set_DCO(float freq){

    if (freq >= 1.5 && freq<=48){ //assumed in Mhz
        //Step 1 - unlock
        CS->KEY = CS_KEY_VAL;
        CS->CTL0 = CS_CTL0_DCOTUNE_OFS;
        //range(1.5,2)
        if (freq >= 1.5 && freq < 2.0){
            CS->CTL0 = CS_CTL0_DCORSEL_0;
        //range(2,4)
        }else if (freq >= 2.0 && freq < 4.0){
            CS->CTL0 = CS_CTL0_DCORSEL_1;
        //range(4,8)
        }else if (freq >= 4.0 && freq < 8.0){
            CS->CTL0 = CS_CTL0_DCORSEL_2;
        //range(8,16)
        }else if (freq >= 8.0 && freq < 16){
            CS->CTL0 = CS_CTL0_DCORSEL_3;
        //range(16,32)
        }else if (freq >= 16.0 && freq < 32){
            CS->CTL0 = CS_CTL0_DCORSEL_4;
        //range(32,48)
        }else{
            /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
             PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            /* Configure Flash wait-state to 1 for both banks 0 & 1 */
            FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
             ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
            FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
             ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
            CS->CTL0 = 0; // Reset tuning parameters
            CS->CTL0 = CS_CTL0_DCORSEL_5; // Set DCO to 48MHz
            /* Select MCLK = DCO, no divider */
            CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) |CS_CTL1_SELM_3;
        }
        CS->KEY = ~CS_KEY_VAL;
    }
}

// is it will ret the frequency of the bits_extracted
struct slopef ret_freq_prop(uint32_t bits_extracted){

        //range(1.5,2)
        if (bits_extracted == CS_CTL0_DCORSEL_0){
            return (struct slopef){1.5,0, 4.8}; //{freq, b value, prop}
        }
        //range(2,4)/
        if (bits_extracted == CS_CTL0_DCORSEL_1)
            return (struct slopef){3.0,-200,1.214};
        //range(4,8)
        else if (bits_extracted == CS_CTL0_DCORSEL_2)
            return (struct slopef){6,0,.303};
        //range(8,16)
        else if (bits_extracted == CS_CTL0_DCORSEL_3 )
            return (struct slopef){12,-14, .0758};
        //range(16,32)+
        else if (bits_extracted == CS_CTL0_DCORSEL_4 )
            return (struct slopef){24,0,.01892};
        //range(32,48)
        else
            return (struct slopef){48,0,.00517};

}


// Create function delay_us() to cause a software delay of a specified time.
// usec =< 50ms*(10^3us/1ms) = 50000us
void delay_us(int usec){

    if (usec == 0)
        return;

    uint32_t bits_extracted = CS->CTL0;
     //BIT_PULL(16) & BIT_PULL(17) & BIT_PULL(18);

    struct slopef freq_prop = ret_freq_prop(bits_extracted);
    // bits extracted for - > time
    float time= 1/freq_prop.freq; // 10^-3 s


    int delay;

    //if (freq_prop.freq == 1.5 || freq_prop.freq == 3){
    delay = (time/freq_prop.prop)* usec + freq_prop.b;  // cycles



    int i;
    for (i = 0; i < delay; i++);

}

 //////////////////////////////////////////////////////
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
	P2->SEL0 &= ~BIT7 & ~BIT6 & ~BIT5;
	P2->SEL1 &= ~BIT7 & ~BIT6 & ~BIT5;
	// step 4 - turn direction on
	P2->DIR |= BIT7 | BIT6 | BIT5;


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
	set_LCD(37, arr[0]);
	set_LCD(37, arr[1]);
}

// Clear the display
void Clear_LCD(){
    uint16_t arr[2][4] = {{~DB4, ~DB5, ~DB6, ~DB7},{DB4,  ~DB5, ~DB6, ~DB7}};
	set_LCD(37, arr[0]);
	set_LCD(37, arr[1]);
}

void Entry_mode_set(){
    uint16_t arr[2][4] = {{~DB4, ~DB5, ~DB6, ~DB7},
                     {S, I_D, DB6, ~DB7}};
	set_LCD(152000,arr[0]);
	set_LCD(152000, arr[1]);
}
void Init_LCD(){
    uint16_t arr[5][4] = {
                     {DB4, DB5, ~DB6, ~DB7},
                     {~DB4, DB5, ~DB6, ~DB7},
                     {~DB4, ~DB5, F, N},
                     {~DB4, DB5, ~DB6, ~DB7},
                     {~DB4, ~DB5, F, N}
    };
	set_LCD(40000,arr[0]);
	//====== For function set ========
	set_LCD(37,arr[1]);
	set_LCD(0,arr[2]);
	//====== For function set ========
	set_LCD(37,arr[3]);
	set_LCD(0,arr[4]);
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
void Write_char_LCD(char c){
        delay_us(12);
        P2->OUT &= ~RW;
        P2->OUT |= RS;

        P2->OUT &= ~EN;
        P4->OUT = c;
        P2->OUT |= EN;
        delay_us(0);
        P2->OUT |= ~EN;

}
// write a string to a specified location on the lcd
void Write_string_LCD(){

}
void main(void)
{
    set_outputs();
	Init_LCD();

	Write_char_LCD('a');

}
