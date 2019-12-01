#include "msp.h"
#include "Stepper.h"
#include "delay.h"
#include "UltraSonicSensor.h"
#include "bluetooth.h"
#include <math.h>

#define FOCAL_LENGTH 20.0 // cm
#define CIRCUMFERNCE .8 // 
#define RESET_DO 30.0 // cm
#define RESET_DI (FOCAL_LENGTH*RESET_DO)/(RESET_DO-FOCAL_LENGTH) //60cm

enum State {PREV, CURR};

float di[2]; // distance away from image

float _do(float di){
	float _do_var;
	if ((_do_var = (di *FOCAL_LENGTH)/(di-FOCAL_LENGTH)) <= 0)
		return 0;
	else
		return _do_var;
}
float get_revolutions(float *di){
	float diff = _do(di[CURR]) - _do(di[PREV]);
	return diff/CIRCUMFERNCE;
}



/*NOTE: Direction 
 * CCW - moves the phone back toward the motor
 * CW - moves the phone closer to the motor
 */
void main(void)
{
	float revolutions = 0;
	direction_t direction;
	init_UltraSonicSensor();
	init_Stepper(CW);
	init_BT();
	__enable_irq();

	di[PREV] = RESET_DI; // this is going to have to use the reset of do
	direction = CW;
	while(1){

		// STATE 1 - normal mode
	    if (bt == NORMAL_MODE){
			// step 1 - get di from get distance
 			di[CURR] = get_distance_cm_max();
			delay_us(1000);
			// step 2 - then use this to get revs  needed embeded is the direction
			revolutions = get_revolutions(di);
			delay_us(1000);
			direction = revolutions > 0 ? CCW : CW;
			// step 3 - then turn the motor needed	
			rotate(revolutions, direction);
			// step 4 - set prev = curr
			di[PREV] = di[CURR];
		// STATE 2 - reset do
		}else{
	        // restart to RESET_DI
			di[CURR] = RESET_DI;
			delay_us(1000);
			revolutions = get_revolutions(di);
			delay_us(1000);
			direction = revolutions > 0 ? CCW : CW;
			rotate(revolutions, direction);
			di[PREV] = di[CURR];
	    }
	    delay_us(10000);

	}
}
