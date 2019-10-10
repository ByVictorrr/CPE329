#include "msp.h"
#include "Keypad.h"

int prev;
int now;

void PORT6_IRQHandler(){
	//Step 1 - toggle a timer
	TIMER_A0 -> CCR

}
/**
 * main.c
 */
void main(void)
{
	// Step 0 - enable NVIC for port 6[0-4]
	NVIC->ISER[1] = (1 << PORT6_IRQn & 0x1F);

	// step 1 - enable global interupts
	__enable_irq();

	while (1){
		// step 0 - press btn
		if (read_key() != '\0'){
			// that means a btn is pressed
			// Step 1 - configure a gpio in to internuptp

		}
	}
	
}
