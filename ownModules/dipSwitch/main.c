#include "msp.h"
#include "DipSwitch.h"

void main(void)
{
    init_DipSwitch();
	// ==debugging===
	P1->DIR|=BIT0;
	//=============
	__enable_irq();

	while(1){
		if (dip_switch == RESTART_MODE){
			turn_onLED();
		}else{
			turn_offLED();
		}
	}
}
