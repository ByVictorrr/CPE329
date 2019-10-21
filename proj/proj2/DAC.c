#include "msp.h"
#include "delay.h"
#include <math.h>
#include "DAC.h"




void init_SPI(){
    // Step 1 - put in rst state
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;
    // Step 2 - set as master of bus, synchronous mode and select smclk
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST |
            EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_MSB
            |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2 | EUSCI_B_CTLW0_CKPL; // mode (0,1)
	//EUSCI_B0 -> CTLW0 &=~(EUSCI_B_CTLW0_CKPL | EUSCI_B_CTLW0_CKPH);
    EUSCI_B0 -> BRW = 0x01; // run full smclk speed
    // Step 3 - select simo, somi and sclk
    P1->SEL0 |= (SCLK | SIMO);
    P1->SEL1 &= ~(SCLK | SIMO);
    // Step 4 - clears software rst
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

}


void send_to_DAC(uint16_t out_voh){
    // Step 0 - sep bytes
    uint8_t loByte = out_voh & 0xFF, highByte = ((out_voh >> 8) & 0xFF) ;
    // Step 1 - set up _CS
    P1->OUT &= ~_CS;
    // Step 2 - send the high byte first(put high byte in buffer)
    EUSCI_B0 ->TXBUF = highByte;
    // Step 3 - wait for Tx flag when tx buffer empty
    while(!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG));
    // Step 3.1 put more data in the buffer
    EUSCI_B0 ->TXBUF = loByte;
    // Step 4 - clear the rx flag before using
    //EUSCI_B0 ->IFG &= ~EUSCI_B_IFG_RXIFG;
    // Step 5 - use  the recieve flag to indicate when we sent the first 8 bits
    while(!(EUSCI_B0 ->IFG & EUSCI_B_IFG_RXIFG));
    // Step 6 - after transmitting
    P1->OUT |= _CS;

}



// returns a uint16 (12bits) - representing the voltage level
// returns -1 on error
uint16_t voltage_to_dacData(float volts){

    float slope =1251;
    int b =-15.3;
    uint16_t data=0;
    // data = slope * (volts) + b
    if (volts > 0 && volts<=3.3)
        data = slope*volts+b;
    else
        return GAIN | SHDN | 0;

    if (data < 0)
        data = 0;

    return GAIN | SHDN | data;
}
// if you decrment the delta by half you have to increase the LEN by 2
void gen_arrays(float *voltages, int size, float delta, bool isSymetric, double (*fn)(double)){
    int i, mid = (LEN - 1)/2;
    float prev = 0, curr;
    voltages[0] = 0;

    if (fn != NULL){
        voltages[0] = 1.5*fn(2*voltages[0])+1.5;
    }

     for (i=1; i<size; i++){	// cond 1 - if the wave is symetic then plot its points sym }else if(isSymetric && i > mid){
		 if (i > mid && isSymetric){
            curr = prev - delta;
            voltages[i] = curr;
            if (fn != NULL){
               voltages[i] =  1.5*fn(2*curr) + 1.5;
            }
		// cond 2 - if symetric or just plotting half
       }else{
           curr = prev + delta;
            voltages[i] = curr;
            if (fn != NULL){
               voltages[i] =  1.5*fn(2*curr) + 1.5;
            }
        }
      prev = curr;
     }

}
void gen_square(float *voltages, int size, int dutyCycle){

	int i;
    for (i=0; i<size; i++){	
		// turn on the the voltage high
		if (i > (1 - dutyCycle/100.0)*size){
			voltages[i] = 3.2;
		}else{
			voltages[i] = 0;
		}	
}
}
