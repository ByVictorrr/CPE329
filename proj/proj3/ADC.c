#include "msp.h"
#include "ADC.h"
#include "UART.h"
#include <math.h>

// using port 4.6




float calibrated_voltage(uint16_t N_ADC){
	// (16741, 3.3V) - highest value N_ADC can be
	float m = 2.012*pow(10, -4);
	float b = -7.25*pow(10, -3);
	if (N_ADC <= 16741){
		return m*N_ADC+b;
	}
	return -1;
}


void init_ADC14(){

    P4->SEL0 |= BIT6;
    P4->SEL1 |= BIT6;

	ADC14->CTL0 &= ~ADC14_CTL0_ENC; // set ENC to 0 for config

	ADC14->CTL0 = ADC14_CTL0_SHP // use internal sample timer
	| ADC14_CTL0_SSEL__SMCLK // use smclk
	| ADC14_CTL0_SHT1_2// sample 16 clks samples
	| ADC14_CTL0_ON; // turn on ADC14 (saves power)

	ADC14->CTL1 = ADC14_MEM_LOC << ADC14_CTL1_CSTARTADD_OFS
		| ADC14_CTL1_RES_3; // 14 bit mode

	//Look at mem config 
	ADC14->MCTL[ADC14_MEM_LOC]  = ADC14_MCTLN_INCH_7;
	// enable interupts
	ADC14->IER0 = ADC14_IER0_IE22; // enable interupts on mem loc 22

	//ADC14->CTL0 |= ADC14_CTL0_CONSEQ_3; // enable sequence of channels

	ADC14->CTL0 |= ADC14_CTL0_ENC;


	// step 2 - enable interrupts 
	NVIC->ISER[0] = (1 << (ADC14_IRQn & 0x1f));
	__enable_irq();
}

void send_float_UART(float c_volt){
		//step 2.1 - multiply float by 100 so we get two decimal places
		int whole_cvolt = c_volt *100, val = 0;
		char send_char;
		//step 2.2 - send the 100 place char 
		send_char = intToChar(whole_cvolt/100);
		sendCharUART(send_char);
		//step 2.3 - send '.'
		sendCharUART('.');
		//step 2.4 - send 10 place
		whole_cvolt %= 100;
		send_char = intToChar(whole_cvolt/10);
		sendCharUART(send_char);
		//step 2.5 - send 1's place
		send_char = intToChar(whole_cvolt % 10);
		sendCharUART(send_char);
		// step 2.5 - send '\n'

}
