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
#define CCW_PIN DIRECTION
#define CW_PIN ~DIRECTION
typedef enum{CCW, CW} direction_t;
// P5.0 - step
// P5.1 - DIRECTION
/**
 * main.c
 */
void init_stepper(direction_t direction);
void step(int steps);
void set_direction(direction_t direction);
void rotate(int revolutions, direction_t direction);

#endif /* STEPPER_H_ */
