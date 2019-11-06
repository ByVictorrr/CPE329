#include "msp.h"
#include "UART.h"
#include "ADC.h"
#include "delay.h"
#include <math.h>
#include "UART.h"
#include <stdio.h>
#include "sample.h"



float total_sqr = 0.0;

int low, high; // for high and low values
int sample_count = 0;


// Description: get the sampled value
void ADC14_IRQHandler(){
    float voltage;
	// step 1 - get voltage and check if its higher than high
    if (high < (digital = ADC14->MEM[22])){
        high = digital;
    }
	// step 2 - get voltage and check if its lower than low
    if (low > digital){
        low = digital;
    }	
	// step 3 - this is for the rms
    voltage = calibrated_voltage(digital);
    total_sqr = total_sqr + voltage*voltage;
}

// Description: triggers the adc14 to start new sample every 3000 cycles
void TA0_0_IRQHandler(){
    // step 1 - clear flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    //step 2 - set the adc run
    ADC14-> CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;
    // step 3 - ccr0 
    TIMER_A0->CCR[0] += 3000;
    // step 4 - increment counter
    if(sample_count < MAX_SAMPLE){
        sample_count++;
    }else{
		// disable cctl0
		disable_TA0();
    }
}

// Description: disables Ta0ccr0 from going to handler
void disable_TA0(){
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE; // enable interupts
}
// Description: enables Ta0ccr0 from going to handler
void enable_TA0(){
	TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interupts
}

// Description: init of Ta0
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
	TIMER_A0->CCR[0] = 3000;

    // step 3 - enable interrupts
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // ccr1 interupts
    TIMER_A0->R = 0;
}



// Description: gets vrms and vpp by pass by ref
void get_vpp_vrms(float *Vrms, float *Vpp){

    float high_v, low_v;
	// step 1 - enable ta0 for adc14
    enable_TA0();
	// step 2 - reset low and high and sample count
    low = LOW_RST;
    high = HIGH_RST;
    sample_count = 0;
    while(1){
		// Step 3 - wait for max_samples to complete
        if(sample_count >= MAX_SAMPLES){
            TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
            high_v = calibrated_voltage(high);
            low_v = calibrated_voltage(low);
            if (low_v <= 0){ // dont output if low_v is neg
                high_v = 0;
                low_v = 0;
            }else if ((high_v - low_v) <0.49){ // dont output if diff is less than .5V
                high_v = low_v = 0;
            }else if ((high_v + low_v)/2 >= 2.75){ // dont output if dc offset is over 2.75
                high_v = 0;
                low_v = 0;
            }
			// Step 4 - get the vrms and vpp
            *Vpp = high_v - low_v;
            *Vrms = sqrt(total_sqr/1000);
			// Step 5 - rst all vars
            total_sqr = 0;
            sample_count = 0;
            low = LOW_RST;
            high = 0;
            TIMER_A0 -> R = 0;
			// step 6 - disable ta0
			disable_TA0();
            break;
        }
    }

}


