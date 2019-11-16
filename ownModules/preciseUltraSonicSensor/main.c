#include "msp.h"
#include "delay.h"
#include "UltraSonicSensor.h"

void main(void)
{

    float distance = 0;

    while(1){
	    distance = get_distance_cm();
    }

}
