/*
 * cytron_motor_driver.h
 *
 *  Created on: Sep 15, 2023
 *  Author: Md. Khairul Alam
 */

#ifndef CYTRON_MOTOR_DRIVER_H_
#define CYTRON_MOTOR_DRIVER_H_

enum {
  RC_MCU,
  PWM_INDEPENDENT,
  PWM_MIXED,
  SERIAL_SIMPLIFIED,
  SERIAL_PACKETIZED
};

void cytron_motor_driver_init_pwm(int mode, int in1Pin, int in2Pin, int an1Pin, int an2Pin);
void cytron_motor_driver_init_uart(int mode, int txPin, int rxPin, uint32_t baudrate);
void cytron_motor_driver_initbyte(uint8_t dummyByte);
void cytron_motor_driver_control(signed int motorLSpeed, signed int motorRSpeed);
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif /* CYTRON_MOTOR_DRIVER_H_ */
