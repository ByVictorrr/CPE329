#include "msp.h"
#include "UART.h"
#include "ADC.h"
#include "delay.h"
#include <math.h>
#include <stdio.h>

#define F_INPUT 3000000
#define SAMPLE_RATE 4
#define SIZE 10000
#define MAX_SAMPLE 1000

float arr[SIZE];

typedef enum{FALSE, TRUE} bool;

int low, high; // for high and low values

bool isFirstSample = TRUE;
int sample_count = 0;

int f_wave = 1000;
int ta0_ifg_overflows = 0;

int haha = 0;


void barGraph(float voltage){
    int bars = voltage/.1; // volts/100mv
	int i;
	for (i=0; i < bars; i++)
		sendCharUART('|');
}

void printAll(int dc, int RMS, int AVG, int PP){

    sendStrUART("============Multimeter================");
    sendVT100UART("[3;2H");
    //====== DC voltage=================

    sendStrUART("DC Voltage (V) : ");
	sendVT100UART("[4;2H");
    //====== AC voltage==============
    sendStrUART("Vrms (V) : ");
    sendVT100UART("[5;2H");
    sendStrUART("Vpp (V) : ");
    sendVT100UART("[6;2H");
    sendStrUART("Freq (Hz) : ");
    sendVT100UART("[7;2H");
    sendStrUART("DC Graph (50mV/Div): ");
    barGraph(dc);
    sendVT100UART("[8;2H");
    sendStrUART("RMS Graph (50mV/Div): ");
    barGraph(RMS);
}
// Description: gets sample time for a given input frequency
int get_sample_count(int f_wave, int F_input, int SAMPLERATE){
    return F_input/(SAMPLERATE*f_wave);
}

void ADC14_IRQHandler(){
    if (high < ADC14->MEM[22]){
        high = ADC14->MEM[22];
    }
    if (low > ADC14->MEM[22]){
        low = ADC14->MEM[22];
    }
}

// For flag overflows
void TA0_N_IRQHandler(){
    ta0_ifg_overflows++;
}


// to get a new TAxR value on rising edge of wave clock
void TA0_0_IRQHandler(){
	static int TA0_R_i = 0; // inital TAXR
	int TA0_R_f;
    uint64_t delta_TA0;
	// step 1 - clear flag
	TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	//step 2 - set the adc run
	ADC14-> CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;
	int var;
	// step 3 - ccr0 + 1/
	var = 50+ 1.0/(f_wave) *( F_INPUT * 0.001);
	TIMER_A0->CCR[0] +=var;

	// step 4 - increade counter
	if(sample_count < MAX_SAMPLE){
	    sample_count++;
	}else{
	    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
	    TIMER_A0->CTL &= ~TIMER_A_CTL_IE;
	}
	/*
	if (ta0_ifg_overflows != 0){
	    // Step 1 - get difference in cycles with overflow
	    delta_TA0= ((TA0_R_f = TIMER_A0->R)+(0xffff)*ta0_ifg_overflows - TA0_R_i);
	    ta0_ifg_overflows = 0;
	}else{
	    // Step q - get difference in cycles no overflow
	    delta_TA0= ((TA0_R_f = TIMER_A0->R) - TA0_R_i);
	}
	// Step 2 - get freq
	f_wave = F_INPUT/delta_TA0;
	//Step 3 - get initial rx
	TA0_R_i = TA0_R_f;
	*/

}


void init_TA0(){
	// step 1 - configure ctl reg
	TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 // select smclk
					| TIMER_A_CTL_MC_2 // up mode
					| TIMER_A_CTL_IE; // enable interputs for TAxR overflowing
	TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; // clear interrupt flag
	// step 2 - config cctl[0]
	TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CM_1 // capture on rising edge
											 // use CCI0A
						| TIMER_A_CCTLN_SCS // TODO: not sure if enable synchrounous
						| TIMER_A_CCTLN_CCIE; // enable interupts
	TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

	// step 4 - initalize ccr0
	TIMER_A0->CCR[0] = 50 + (1.0/(f_wave)* 3000000.0 * 0.001);

	// step 3 - enable interrupts
	NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // ifg flag
	NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // ccr1 interupts
	TIMER_A0->R = 0;
}


void swap(float *xp, float *yp)
{
    float temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(float arr[], int n)
{
    int i, j;
    for (i = 0; i < n-1; i++)

    // Last i elements are already in place
    for (j = 0; j < n-i-1; j++)
        if (arr[j] > arr[j+1])
            swap(&arr[j], &arr[j+1]);
}

/**
 * main.c
 */
void main(void){

	// step -1 - enable wave port
	P7->SEL0|=BIT3;
	P7->SEL1&=~BIT3;
	// step 0 - init timer a0 flag
	init_UART();
	init_ADC14();
    set_clk("SMCLK");
    set_DCO(F_INPUT/1000000);

	init_TA0();
	// step 1 - configure ctl reg
	__enable_irq();

	//f_wave = 200;
   // printAll(3,2,2, 2);
    float high_v, low_v;
    float Vpp;
    low = 30000; // Low equavilent of 3V
    high = 0; // High equivalent o 0v
    sample_count = 0;
	while(1){

	    if (sample_count >= MAX_SAMPLE){

	        TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
	        high_v = calibrated_voltage(high);
            low_v = calibrated_voltage(low);
	        send_float_UART(high_v);
	        sendCharUART('\n');
	        send_float_UART(low_v);
	        sendCharUART('\n');
	        Vpp = high_v - low_v;
	        sample_count = 0;
	        low = 30000;
	        high = 0;
	        TIMER_A0 -> R = 0;
	        TIMER_A0 -> CCTL[0] |= TIMER_A_CCTLN_CCIE;

	    }
	}



}
