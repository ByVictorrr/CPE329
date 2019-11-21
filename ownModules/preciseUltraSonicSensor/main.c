#include "msp.h"
#include "delay.h"
#include "UltraSonicSensor.h"

void main(void)
{

    float distance = 0;
    init_UltraSonicSensor();
    __enable_irq();
    while(1){

	    distance = get_distance_cm();
	    delay_us(10000);

    }

}
