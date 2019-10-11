#include "msp.h"

// Using UCB0 (port 1)
#define SCLK BIT5
#define SIM0 BIT6
#define SOMI BIT7



void init_SPI(){
	// Step 1 - put in rst state
	EUSCI_B0 -> CTL0 |= EUSCI_B_CTLW0_SWRST;
	// Step 2 - set as master of bus, synchronous mode and select smclk	
	EUSCI_B0 -> CTL0 |= EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_UCSSEL_2; 
	EUSCI_B0 -> BRW = 0x01; // run full smclk speed
	// Step 3 - select simo, somi and sclk
	P1->SEL0 |= (SCLK | SIMO | SOMI);
	P1->SEL1 &= ~(SCLK | SIMO | SOMI);
	// Step 4 - clears software rst
	EUSCI_B0 -> CTL0 &= ~EUSCI_B_CTLW0_SWRST;
	// Step 5 - enable interrupt (for transmitting)	
	EUSCI_B0 -> CTL0 ->IE = EUSCI_B_IE_RXIE; // enable interrupts
	NVIC->ISER[0] = (1 << EUSCIB_IRQn); // enable on lookup table
	__enable_irq(); // enable globals
}
/**
 * main.c
 */
void main(void)
{

	init_SPI();

}
