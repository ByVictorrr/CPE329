#include "msp.h"
#include "Keypad.h"
#include "LCD.h"
#include "delay.h"
#include "DAC.h"
#include <string.h>
#include <math.h>
#include "function_generator.h"




void main(void)
{

	signed char key;
	int dutyC = 50;
	int freq = 100;
	char wave_type = SQUARE;

	// step 1 - init everything
	init_SPI();
	P1->DIR |= _CS;
	set_clk("SMCLK");
	set_DCO(12);
	Keypad_init();
	Init_LCD();

	// step 2 - set the data arrays for default
	gen_square(voltages, LEN, dutyC);
	
	// step 3 - by default and init timers
	wave_data = waveform(freq);
	init_TimerA(wave_data.ccr0);

	while(1){
	    send_to_DAC(voltage_to_dacData(wave_data.ptr[wave_data.i]));
		// doesnt go in the if statment if nothing is pressed
		if ((key=read_key()) != -1){
			// State 1 - select wave
			if (strchr(SELECT_WAVE, key) != NULL){
				if ((wave_type = key) == SAW){
					gen_arrays(voltages, LEN, .015, FALSE, NULL); // sine wave XXX (Configure step size and LEN)
				}else if (wave_type == SQUARE){
					gen_square(voltages, LEN, dutyC);
				}else if (wave_type == SINE){
					gen_arrays(voltages, LEN, .03 , FALSE, cos);
				}else if (wave_type == RANDOM){
					gen_arrays(voltages, LEN, .05, TRUE, atan);
				}
				wave_data = waveform(freq); // get new wave with new freq
			// state 2 - select freq
			}else if (strchr(SELECT_FREQ, key) != NULL){
				// Step 1 - get the freq
				freq = (key - '0')*100;
				// step 2 - relate the ccr0 value to freq
				wave_data.ccr0 = F_INPUT/((wave_data.max_index+1)*freq);
			// state 3 - select dc but also make sure we are using square voltages
			}else if (strchr(SELECT_DUTYCYCLE, key) != NULL && wave_type == SQUARE){
				// step 1 - decr/incr or rest duty cycle of square wave
				if (key == '*' && dutyC > 10){ // decreases by 10
					dutyC -= 10;
				}else if (key == '#' && dutyC < 90){
					dutyC += 10;
				}else if (key == '0'){
					dutyC = 50;
				}
				gen_square(voltages, LEN, dutyC);
				delay_us(200000);
			}
	}

}
}
