/*
 * stepper_motor.h
 *
 *  Created on: Nov 5, 2023
 *      Author: khair
 */

#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#define ENABLE P0_2 //IO_0
#define STEP P1_3   //IO_2
#define DIR P0_3    //IO_1
#define LIMIT_SWITCH CYBSP_D8    //D8

void init_stepper(void);
void drive_stepper_motor_ccw(void);
void drive_stepper_motor_cw(void);
void drive_stepper_motor_down(void);
void drive_stepper_motor_up(void);

#endif /* STEPPER_MOTOR_H_ */
