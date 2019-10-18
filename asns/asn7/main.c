#include "msp.h"
#include "delay.h"
#include <math.h>
#include "DAC.h"

void main(void)
{
    P1->DIR |= _CS;
    set_clk("SMCLK");
    set_DCO(F_INPUT);
    // Step 1 - init SPI
    init_SPI();
    init_TimerA();
    // For triangle wave
    //float voltages[LEN] = {0, .1, .3, .5, .7, .9, 1.1, 1.3, 1.5, 1.7, 1.9, 1.7, 1.5, 1.3, 1.1, .9, .7, .5,.3};

    float voltages[LEN];

    float sine_voltages[LEN];
    gen_arrays(sine_voltages, LEN, .025, FALSE, cos); // generates a good sine wave

    //

    while(1){
        send_to_DAC(voltage_to_dacData(sine_voltages[counter]));
        //send_to_DAC(0xFFF | SHDN | GAIN);
    }

}
