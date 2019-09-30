/*
 * delay.h
 *
 *  Created on: Sep 29, 2019
 *      Author: victor
 */

#ifndef DELAY_H_
#define DELAY_H_

struct slopef{
    float freq;
    int b;
    float prop;
};


void set_DCO(float freq);
struct slopef ret_freq_prop(uint32_t bits_extracted);
void delay_us(int usec);

#endif /* DELAY_H_ */
