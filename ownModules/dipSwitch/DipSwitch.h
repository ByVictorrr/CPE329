#ifndef DIPSWITCH_H_
#define DIPSWITCH_H_
#include "msp.h"

/*  P4.1 - dip switch 1 */

#define DIP1 BIT1
typedef enum{NORMAL_MODE, RESTART_MODE} dip_state;
dip_state dip_switch;
// Description: turn the dip_switch on/off
void PORT4_IRQHandler();
// Description; 4.1 input switch
void init_DipSwitch();
#endif

