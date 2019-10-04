#include "msp.h"
#include "delay.h"


/**
 * main.c
 */

void main(void)
{

    setDCO(24); // 24Mhz
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
}


