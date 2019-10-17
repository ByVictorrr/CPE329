#include "msp.h"
#include "delay.h"
#include <math.h>
// Using UCB0 (port 1)
#define SCLK BIT5
#define SIMO BIT6
#define _CS BIT7

// LOOK AT ME !!!! : I think the data isnt having enough time to send the data before the next interupt check that!

// working for 1-4

#define BUF (uint16_t)BIT(14)
#define GAIN (uint16_t)BIT(13)
#define SHDN (uint16_t)BIT(12)

#define CCR0 6000
#define CCRN 12000



typedef enum {ZERO ,ONE_UP, TWO, ONE_DOWN} state_t;
#define TWO_VOLTS 0x0FF3 | GAIN | SHDN
#define ONE_VOLTS 0x0FE9 | GAIN | SHDN
#define ZERO_VOLTS 0x0FDF | GAIN | SHDN


struct counter_field{
    uint8_t counter:2; // allocate 2 bits
    //will overflow and reset to 0
} two_bit_counter;


// Handler for CCR0
void TA0_0_IRQHandler(void){
    P1->OUT=BIT0;
    delay_us(100000);
	// Step 1 - send data representing ones(use a flag)
	two_bit_counter.counter += 1;
	//step 3 - turn off capture/compare interrupt flag(to trigger again on rising edge)
	TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
	// Step 2 - add 960 cycles
	TIMER_A0 -> CCR[0] += CCR0; // up mode just set it equal to it self
	P1->OUT=~BIT0;
}

// Handler for CCR[1-6]
void TA0_N_IRQHandler(void){


    // Step 1 - toggle led
    P4->OUT ^= BIT1;
    two_bit_counter.counter += 1;
    //step 3 - turn off capture/compare interrupt flag
    TIMER_A0 -> CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    // Step 2 - add ccrn
    TIMER_A0 -> CCR[1] += CCRN; // add .

}


void set_everything(){
    // step 0 - set up the GPIO
    P4->DIR |= BIT0 | BIT1;
    P4->SEL0 &= ~BIT0 & BIT1;
    P4->SEL1 &= ~BIT0 & ~BIT1;
    P4->OUT&=~BIT0;

    // Step 1 - set the inital cycles
    TIMER_A0->CCR[1] = CCRN; //cycles
    TIMER_A0->CCR[0] = CCR0; // cycles
    // XXX : review below
    //step 2 - control regs - enable interrupts , and compare mode
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

    //Step 3 - Select MCLK and select up mode
    TIMER_A0 -> CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_2; // tassel - select clock src, mc - select continuious mode

    // Step 4 = enable NVIC
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // for CCR0

    NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // for other CCRs

    // Step 5 - enable globally
    __enable_irq();


}



void init_SPI(){
    // Step 1 - put in rst state
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;
    // Step 2 - set as master of bus, synchronous mode and select smclk
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST
            |EUSCI_B_CTLW0_MSB |EUSCI_B_CTLW0_MST
            |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;
    EUSCI_B0 -> CTLW0 &=~(EUSCI_B_CTLW0_CKPL | EUSCI_B_CTLW0_CKPH);
    EUSCI_B0 -> BRW = 0x01; // run full smclk speed
    // Step 3 - select simo, somi and sclk
    P1->SEL0 |= (SCLK | SIMO);
    P1->SEL1 &= ~(SCLK | SIMO);
    // Step 4 - clears software rst
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;


    // Step 5 - enable interrupt (for transmitting)
    //EUSCI_B0 -> CTLW0 ->IE = EUSCI_B_IE_RXIE; // enable interruptsP3->DIR |= _CS;
    //NVIC->ISER[0] = (1 << EUSCIB_IRQn); // enable on lookup table
    //__enable_irq(); // enable globals
    // Step 6 - mode 0,0
   // EUSCI_B0 -> CTLW0 &= ~(EUSCI_B_CTLW0_CKPH | EUSCI_B_CTLW0_CKPL);
}
/**
 * main.c
 */





void send_to_DAC(uint16_t out_voh){
    // Step 0 - sep bytes
    uint8_t loByte = out_voh & 0xFF, highByte = ((out_voh >> 8) & 0xFF) ;
    // Step 1 - set up _CS
    P1->OUT &= ~_CS;

    // Step 2 - send the high byte first(put high byte in buffer)
    EUSCI_B0 ->TXBUF = loByte;
    // Step 3 - wait for Tx flag when tx buffer empty
    while(!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG));
    // Step 3.1 put more data in the buffer
    EUSCI_B0 ->TXBUF = highByte;
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
	
	float slope = 2774.4230;
	int b =-4783;
	uint16_t data;
	// data = slope * (volts) + b	
	if (volts >= 0 && volts<=3.3)
		data = slope*volts+b;

	return abs(GAIN | SHDN | data);
}



void main(void)
{
    P1->DIR |= _CS;
    set_clk("SMCLK");
    set_DCO(1.5);
    // Step 1 - init SPI
    init_SPI();
    uint16_t data = 0x0FE9 | GAIN | SHDN;
    //flag = LOW;



	set_everything();

        while(1){

                       // Triangle
                       switch(two_bit_counter.counter){
                       case ZERO:
                           data = ZERO_VOLTS;
                           break;
                       case ONE_UP:
                           data = ONE_VOLTS;
                           break;
                       case TWO:
                           data = TWO_VOLTS;
                           break;
                       case ONE_DOWN:
                           data = ONE_VOLTS;
                           break;

                       }

            send_to_DAC(data);
        }

}
