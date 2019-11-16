/*
 * UltraSonicSensor.h
 *
 *  Created on: Nov 16, 2019
 *      Author: victor
 */

#ifndef ULTRASONICSENSOR_H_
#define ULTRASONICSENSOR_H_


#include "msp.h"
#define TRIG BIT5
#define F_INPUT 3000000
#define TIMER_A_MAX 0xffff



/* P2.5 - output (trig)
 * P7.3 - input(TA0.CCI0A) (echo)
 * P2.4 - input(TA0.ccI1A) (echo)
 *
 *
 *
 */
void init_ECHO();
void init_TRIGGER();
void send_trigger();
uint16_t get_time_high();
double get_distance_cm();
void init_TA0();
void TA0_N_IRQHandler();
void TA0_0_IRQHandler();
void reset_TimerA();
void init_UltraSonicSensor();
void main(void)


#endif /* ULTRASONICSENSOR_H_ */
