/*
 *  cytron_motor_driver.c
 *
 *  Created on: Sep 15, 2023
 *      Author: Md. Khairul Alam
 */

#include "cybsp.h"
#include "cyhal.h"
#include "cytron_motor_driver.h"


extern cy_rslt_t rslt;
cyhal_uart_t cytron_uart_obj;
cyhal_pwm_t pwm_obj_an1, pwm_obj_an2;

uint32_t actualbaud;
uint8_t _mode;
uint8_t _an1Pin, _an2Pin, _in1Pin, _in2Pin;
uint8_t commandByte;
int _motorLSpeed, _motorRSpeed;


void cytron_motor_driver_init_pwm(int mode, int in1Pin, int in2Pin, int an1Pin, int an2Pin)
{
  _mode = mode;
  _in1Pin = in1Pin;
  _in2Pin = in2Pin;
  _an1Pin = an1Pin;
  _an2Pin = an2Pin;

  /* Initialize pin P0_0 GPIO as an output with strong drive mode and initial value = false (low) */
  rslt = cyhal_gpio_init(in1Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
  rslt = cyhal_gpio_init(in2Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
  /* Initialize PWM on the supplied pin and assign a new clock */
  rslt = cyhal_pwm_init(&pwm_obj_an1, an1Pin, NULL);
  rslt = cyhal_pwm_init(&pwm_obj_an2, an2Pin, NULL);

}


void cytron_motor_driver_init_uart(int mode, int txPin, int rxPin, uint32_t baudrate)
{
  _mode = mode;

  const cyhal_uart_cfg_t cytron_uart_config =
  {
      .data_bits = 8,
      .stop_bits = 1,
      .parity = CYHAL_UART_PARITY_NONE,
      .rx_buffer = NULL,
      .rx_buffer_size = 0
  };

  /* Initialize the UART Block */
  rslt = cyhal_uart_init(&cytron_uart_obj, txPin, rxPin, NC, NC, NULL, &cytron_uart_config);
  /* Set the baud rate */
  rslt = cyhal_uart_set_baud(&cytron_uart_obj, baudrate, NULL);

}

void cytron_motor_driver_initbyte(uint8_t dummyByte)
{
  uint8_t tx_buf[1] = {dummyByte};
  size_t tx_length = 1;
  /* Begin Tx Transfer */
  cyhal_uart_write(&cytron_uart_obj, (void*)tx_buf, &tx_length);
  //cyhal_system_delay_ms(5);
}


void cytron_motor_driver_control(signed int motorLSpeed, signed int motorRSpeed)
{
  switch (_mode) {

    case PWM_INDEPENDENT:
      //Left Motor Speed
      if (motorLSpeed >= 0) {
        if (motorLSpeed > 100) motorLSpeed = 100;
        /* Write the value to the output pin */
        cyhal_gpio_write(_in1Pin, false);
        /* Set a duty cycle of speed and frequency of 1kHz */
        rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_an1, motorLSpeed, 1000);
        /* Start the PWM output */
        rslt = cyhal_pwm_start(&pwm_obj_an1);
        //cyhal_system_delay_ms(5);
        /* Stop the PWM output */
        //rslt = cyhal_pwm_stop(&pwm_obj);
      }
      else if (motorLSpeed < 0) {
        if (motorLSpeed < -100) motorLSpeed = -100;
        motorLSpeed = motorLSpeed * -1;
        cyhal_gpio_write(_in1Pin, true);
        rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_an1, motorLSpeed, 1000);
        rslt = cyhal_pwm_start(&pwm_obj_an1);
      }
      //Right Motor Speed
      if (motorRSpeed >= 0) {
        if (motorRSpeed > 100) motorRSpeed = 100;
        rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_an2, motorLSpeed, 1000);
        rslt = cyhal_pwm_start(&pwm_obj_an2);
        cyhal_gpio_write(_in2Pin, true);
      }
      else if (motorRSpeed < 0) {
        if (motorRSpeed < -100) motorRSpeed = -100;
        motorRSpeed = motorRSpeed * -1;
        rslt = cyhal_pwm_set_duty_cycle(&pwm_obj_an2, motorLSpeed, 1000);
        rslt = cyhal_pwm_start(&pwm_obj_an2);
        cyhal_gpio_write(_in2Pin, false);
      }
      break;

    case SERIAL_SIMPLIFIED:
      uint8_t tx_buf[1];
      size_t tx_length;
      if (motorLSpeed >= 0) {
        commandByte = 0;
        _motorLSpeed = map(motorLSpeed, 0, 100, 0, 63);
      }
      else if (motorLSpeed < 0) {
        commandByte = 0x40;
        _motorLSpeed = map(motorLSpeed, 0, -100, 0, 63);
      }
      commandByte = commandByte | _motorLSpeed;
      tx_buf[1] = commandByte;
      tx_length = 1;
      cyhal_uart_write(&cytron_uart_obj, (void*)tx_buf, &tx_length);

      if (motorRSpeed >= 0) {
        commandByte = 0xC0;
        _motorRSpeed = map(motorRSpeed, 0, 100, 0, 63);
      }
      else if (motorRSpeed < 0) {
        commandByte = 0x80;
        _motorRSpeed = map(motorRSpeed, 0, -100, 0, 63);
      }
      commandByte = commandByte | _motorRSpeed;
      tx_buf[1] = commandByte;
      tx_length = 1;
      cyhal_uart_write(&cytron_uart_obj, (void*)tx_buf, &tx_length);
      break;

    default:
      break;
  }
}


long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
