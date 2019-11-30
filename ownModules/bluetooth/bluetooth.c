#include "msp.h"
#include "bluetooth.h"


void EUSCIA0_IRQHandler(void){

    char num;
    if ((num = EUSCI_A0-> RXBUF) == NORMAL){
        bt = NORMAL_MODE;
    }else{
        bt = RESTART_MODE;
    }
}
// Description: setup for a baud rate of 9600
void init_BT(){
        // step 0 - setup gpio for tx/rx
        P1->SEL1 &= ~BIT3 & BIT2;
        P1->SEL0 |= BIT3 | BIT2;
        // Step 1 - put in rst state
        EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;
        // Step 2 - configure smclk for source
        EUSCI_A0 -> CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK ;
        EUSCI_A0->BRW = 19;
        // step 3 - configure MCTLW3
        EUSCI_A0->MCTLW |= EUSCI_A_MCTLW_OS16  // enable over sampling
            | ( 0x55 << 8)  // second modulation stage select (bits hold a pattern BITCLK)
            | (8 << 4); // first modulation stage select, determine the modulation pattern for BITCLK16
        // Step 4 - clears software rst
        EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
        // step 5 -interrupts
        EUSCI_A0->IE|=EUSCI_A_IE_RXIE; // enable rx interrupt
        EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG & ~EUSCI_A_IFG_TXIFG;    // Clear eUSCI RX interrupt flag
        // NVIC
        NVIC->ISER[0] = 1 << (EUSCIA0_IRQn & 031);
		// Step 6 - setup bt state to nomal
		bt=NORMAL_MODE;

}
