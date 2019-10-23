#include "msp.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include "DAC.h"
#include <math.h>

#define NUMBER "0123456789"
#define MAX_CHARS 10
#define NULL 0

#define RETURN '\n'

char inValue[MAX_CHARS];
int i = 0;
int flag = 0;

void EUSCIA0_IRQHandler(void){

	char num = EUSCI_A0-> RXBUF;

	if (strchr(NUMBER,(num = EUSCI_A0-> RXBUF)) != NULL){
		inValue[i] = num;
		EUSCI_A0 ->TXBUF = inValue[i++];
	}else if (num == RETURN){
		flag = 1;
		EUSCI_A0 ->TXBUF = 'n';
	}
}

void init_UART(){
        // step 0 - setup gpio for tx/rx
        P1->SEL1 &= ~BIT3 & BIT2;
        P1->SEL0 |= BIT3 | BIT2;
        // Step 1 - put in rst state
        EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;
        // Step 2 - configure smclk for source
        EUSCI_A0 -> CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK ;

        //EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SYNC;

		EUSCI_A0->BRW = 1;

		// baud rate = 115.2kbs
		// N = 26.041666
		// N > 16 -> over sample os16 = 1
		// UCBRx = Int(N/16) = INT(26.041666/16) = 1
		// UCBRFx = Int((N/16 - UCBRx)*16) = 10
		// UCBRSx[.04166] = 0x52;

		// step 3 - configure MCTLW3
		EUSCI_A0->MCTLW |= EUSCI_A_MCTLW_OS16  // enable over sampling
		   	| ( 0x52 << 8)  // second modulation stage select (bits hold a pattern BITCLK)
			| (0xA << 4); // first modulation stage select, determine the modulation pattern for BITCLK16

        // Step 4 - clears software rst
        EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
		// step 5 -interrupts
		EUSCI_A0->IE|=EUSCI_A_IE_RXIE; // enable rx interrupt
		EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG & ~EUSCI_A_IFG_TXIFG;    // Clear eUSCI RX interrupt flag
		//enable global
		__enable_irq();
		// NVIC
		NVIC->ISER[0] = 1 << (EUSCIA0_IRQn & 031);

}



void recieveCharUART(){

    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG));
    printf("%u", EUSCI_A0-> RXBUF);
    fflush(stdout);
}
void sendCharUART(char c)
{
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0-> TXBUF = c;
}

uint16_t charToInt(char *str){
	int i, decPlace = pow(10,(strlen(str)-1));
	uint16_t val;
	for(i = 0; i < strlen(str); i++){
		val += (str[i] - '0') * decPlace;
		decPlace *= 10;
	}
	return val;
}
/**
 * main.c
 */
void main(void)
{
	int val = 0;
    set_clk("SMCLK");
	set_DCO(3);
	init_UART();
	init_SPI();
	P1->DIR |= _CS;
	memset(inValue, '\0', MAX_CHARS);
	while(1){
		if (flag){
			if ((val = charToInt(inValue)) <= 4095 && val >= 0){
				// Reset everything
				flag = 0;
				memset(inValue, '\0', MAX_CHARS);
				i = 0;
			}
		}
		 send_to_DAC(GAIN | SHDN | val);
	}
}
