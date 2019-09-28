#include "msp.h"


/**
 * main.cg
 */

#define BIT_PULL(x)                                 ((uint32_t)1 << (x))
#define FREQ_1_5_MHZ 1.5
#define FREQ_3_MHZ 3
#define FREQ_6_MHZ 6
#define FREQ_12_MHZ 12
#define FREQ_24_MHZ 24
#define FREQ_48_MHZ 48

void set_DCO(float freq);
void delay_us(int usec);
float ret_freq(uint32_t bits_extracted);


struct slopef{
    float freq;
    int b;
    float prop;
};

int arr[6] = {10, 500, 1000, 2500, 5000, 40000};

void main(void)
{


    // Step 1 - set up DCO
    set_DCO(FREQ_48_MHZ);
    // Step 2 - select right CTL1 values so that MCLK outputs the DCO
    // Step 2.1 - set SELM =
    // Step 2.2 - DIVM = 1
    CS -> KEY = CS_KEY_VAL;
    CS -> CTL1 = CS_CTL1_SELM_3 |  CS_CTL1_DIVM__1;
    CS -> KEY = ~CS_KEY_VAL;

    // Step 3 - set gpio out  SEL = 01
    // Step 3.1 - first select what we want to output
    P4->SEL0 |= BIT3; //
    P4->SEL1 &= ~BIT3;//;
    //step 3.2 - connect MCLK to from module
    //Step 3.3 - set direction to output
    P4->DIR |= BIT3;

    P4->SEL0 &= ~BIT1;
    P4->SEL1 &= ~BIT1;
    P4->DIR |= BIT1;

    while (1){
        P4->OUT |= BIT1;
        delay_us(arr[0]);
        P4->OUT &= ~BIT1;
        delay_us(arr[0]);
    }

}


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

float freq;
   int b;
   float prop;
// is it will ret the frequency of the bits_extracted
struct slopef ret_freq_prop(uint32_t bits_extracted){
        //range(1.5,2)
        if (bits_extracted == CS_CTL0_DCORSEL_0)
            return (struct slopef){1.5,0, 4.8}; //{freq, b value, prop}
        //range(2,4)/
        if (bits_extracted == CS_CTL0_DCORSEL_1)
            return (struct slopef){3.0,0,1.214};
        //range(4,8)
        else if (bits_extracted == CS_CTL0_DCORSEL_2)
            return (struct slopef){6,0,.303};
        //range(8,16)
        else if (bits_extracted == CS_CTL0_DCORSEL_3 )
            return (struct slopef){12,0, .0758};
        //range(16,32)
        else if (bits_extracted == CS_CTL0_DCORSEL_4 )
            return (struct slopef){24,0,.01892};
        //range(32,48)
        else
            return (struct slopef){48,0,1};
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


