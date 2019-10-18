#include "msp.h"
#include "delay.h"
#include <math.h>
#include "DAC.h"

// Handler for CCR0
void TA0_0_IRQHandler(void){

    // Step 0 - for debuging
    P1->OUT=BIT0;
    // Step 1 - check if  counter over 21 if so reset it to zero else set it to itself + 1
    counter = counter >= LEN - 1 ? 0 : counter+1;
    //step 3 - turn off capture/compare interrupt flag(to trigger again on rising edge)
    TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    // Step 2 - add 960 cycles
    TIMER_A0 -> CCR[0] = CCR0; // up mode just set it equal to it self
    P1->OUT=~BIT0;
}

void init_TimerA(){
    // step 0 - set up the GPIO
    P4->DIR |= BIT0 | BIT1;
    P4->SEL0 &= ~BIT0 & BIT1;
    P4->SEL1 &= ~BIT0 & ~BIT1;
    P4->OUT&=~BIT0;

    // Step 1 - set the inital cycles
    TIMER_A0->CCR[0] = CCR0; // cycles
    // XXX : review below
    //step 2 - control regs - enable interrupts , and compare mode

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

    //Step 3 - Select MCLK and select up mode
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // tassel - select clock src, mc - select continuious mode

    // Step 4 = enable NVIC
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0

    // Step 5 - enable globally
    __enable_irq();


}


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
    uint16_t data;
    // data = slope * (volts) + b
    if (volts >= 0 && volts<=3.3)
        data = slope*volts+b;

    return abs(GAIN | SHDN | data);
}

// if you decrment the delta by half you have to increase the LEN by 2
void gen_arrays(float *voltages, int size, float delta, bool isSymetric, double (*fn)(double)){
    int i, mid = (LEN - 1)/2;
    voltages[0] = 0;
     for (i=1; i<size; i++){
         // for symetric waves mirror image
      if (isSymetric && i > mid){
            voltages[i] = fn == NULL? voltages[i-1] - delta: fn(2*voltages[i]) +1;
       }else{
           voltages[i] = fn == NULL? voltages[i-1] + delta: fn(2*voltages[i]) + 1;
        }
     }

}
