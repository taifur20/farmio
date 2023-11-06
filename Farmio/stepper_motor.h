/*
 * stepper_motor.h
 *
 *  Created on: Nov 5, 2023
 *      Author: khair
 */

#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#define ENABLE P0_2
#define STEP P1_3
#define DIR P0_3

void init_stepper(void);
void drive_stepper_motor_ccw(void);
void drive_stepper_motor_cw(void);

#endif /* STEPPER_MOTOR_H_ */
