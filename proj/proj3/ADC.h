/*
 * ADC.h
 *
 *  Created on: Nov 3, 2019
 *      Author: victor
 */

#ifndef ADC_H_
#define ADC_H_


#include "msp.h"


#define ADC14_MEM_LOC 22
// using port 4.6


int digital;


void ADC14_IRQHandler();
float calibrated_voltage(uint16_t N_ADC);

void init_ADC14();

void send_float_UART(float c_volt);


#endif /* ADC_H_ */
