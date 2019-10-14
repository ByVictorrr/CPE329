#include "msp.h"

// Using UCB0 (port 1)
#define SCLK BIT5
#define SIMO BIT6
#define SOMI BIT7

// using port 3
#define _CS BIT2

#define _BIT(x) (BIT0 << x)

#define GAIN _BIT(13)
#define SHDN _BIT(12)



void init_SPI(){
	// Step 1 - put in rst state
	EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;
	// Step 2 - set as master of bus, synchronous mode and select smclk	
	EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_UCSSEL_2;
	EUSCI_B0 -> BRW = 0x01; // run full smclk speed
	// Step 3 - select simo, somi and sclk
	P1->SEL0 |= (SCLK | SIMO | SOMI);
	P1->SEL1 &= ~(SCLK | SIMO | SOMI);
	// Step 4 - clears software rst
	EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
	// Step 5 - enable interrupt (for transmitting)	
	//EUSCI_B0 -> CTLW0 ->IE = EUSCI_B_IE_RXIE; // enable interrupts
	//NVIC->ISER[0] = (1 << EUSCIB_IRQn); // enable on lookup table
	//__enable_irq(); // enable globals
}
/**
 * main.c
 */


void send_to_DAC(uint16_t out_voh){
	// Step 0 - sep bytes
	uint8_t loByte = out_voh & 0xFF, highByte = (out_voh >> 8) & 0xFF;
	// Step 1 - set up _CS
	P3->OUT &= ~_CS;
	P3->DIR |= _CS;
	// Step 2 - send the high byte first(put high byte in buffer)
	EUSCI_B0 ->TXBUF = highByte;
	// Step 3 - wait for Tx flag when tx buffer empty
	while(!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG));
	// Step 3.1 put more data in the buffer
	EUSCI_B0 ->TXBUF = loByte;
	// Step 4 - clear the rx flag before using
	EUSCI_B0 ->IFG &= ~EUSCI_B_IFG_RXIFG;
	// Step 5 - use  the recieve flag to indicate when we sent the first 8 bits
	while(!(EUSCI_B0 ->IFG & EUSCI_B_IFG_RXIFG));
	// Step 6 - after transmitting 
	P3->OUT |= _CS;

}
	

	
void main(void)
{

	// Step 1 - init SPI
	init_SPI();
	uint16_t data = 0;
	data |= GAIN | SHDN | 0xFFF;
	// Step 2 - send data
	send_to_DAC(data);

}
