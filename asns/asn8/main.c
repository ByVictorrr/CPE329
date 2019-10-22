#include "msp.h"
#include "delay.h"
#include <stdio.h>

void EUSCIA0_IRQHandler(void){
	
}

void init_UART(){
        // Step 1 - put in rst state
        EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;
        // Step 2 - configure smclk for source
        EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK ;

		EUSCI_A0->BRW = 26;

		// baud rate = 115.2kbs
		// N = 26.041666
		// N > 16 -> oversample os16 = 1
		// UCBRx = int(N/16) = INT(26.041666/16) = 1
		// UCBRFx = int((N/16 - UCBRx)*16) = 10
		// UCBRSx[.04166] = 0x52;

		// step 3 - configure MCTLW
		EUSCI_A0->MCTLW |= EUSCI_A_MCTLW_OS16  // enable oversampling
		   	| ( 0x52 << 8)  // second modulation stage select (bits hold a pattern BITCLK)
			| (0xA << 4); // first modulation stage select, determine the modulation pattern for BITCLK16 

        // Step 4 - clears software rst
        EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
		// step 5 -interrupts
		EUSCI_A0->IE|=EUSCI_A_IE_RXIE; // endable rx interrupt
		 EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
		//enable global
		__enable_irq();
		// NVIC
		NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 0xff);



}
void sendCharUART(char c)
{
    EUSCI_A0-> TXBUF = c;
}

/**
 * main.c
 */
void main(void)
{
    set_clk("SMCLK");
	set_DCO(3);
	init_UART();
	while(1){
		sendCharUART('h');
	}
}
