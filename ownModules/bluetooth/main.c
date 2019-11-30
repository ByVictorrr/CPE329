#include "msp.h"
#include "delay.h"
#include "bluetooth.h"
void main(void)
 {

    set_clk("SMCLK");
    set_DCO(3);
    init_BT();
    __enable_irq();
    while(1);

}
