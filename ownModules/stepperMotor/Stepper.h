/*
 * Stepper.h
 *
 *  Created on: Nov 16, 2019
 *      Author: victor
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#define STEP BIT0
#define DIRECTION BIT1
#define STEP_PER_REV 200
#define CCW DIRECTION
#define CW ~DIRECTION
// P5.0 - step
// P5.1 - DIRECTION
/**
 * main.c
 */
void init_stepper(int direction);
void step(int steps);
void set_direction(int direction);
void rotate(int revolutions, int direction);

#endif /* STEPPER_H_ */
