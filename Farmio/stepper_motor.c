/*
 * stepper_motor.c
 *
 *  Created on: Nov 5, 2023
 *  Author: Md. Khairul Alam
 */


#include "cyhal.h"
#include "cybsp.h"
#include "stepper_motor.h"


void init_stepper(void)
{

	cy_rslt_t stepper_result;

	stepper_result = cyhal_gpio_init(ENABLE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);

    /* GPIO init failed. Stop program execution */
	if (stepper_result != CY_RSLT_SUCCESS)
    {
	    CY_ASSERT(0);
	}

	stepper_result = cyhal_gpio_init(STEP, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);

	/* GPIO init failed. Stop program execution */
	if (stepper_result != CY_RSLT_SUCCESS)
	{
	    CY_ASSERT(0);
	}

	stepper_result = cyhal_gpio_init(DIR, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);

	/* GPIO init failed. Stop program execution */
	if (stepper_result != CY_RSLT_SUCCESS)
	{
	     CY_ASSERT(0);
	}
}


void drive_stepper_motor_ccw(void)
{
	cyhal_gpio_write(ENABLE, false); //enable the stepper motor control
	cyhal_gpio_write(DIR, false);    //turn is a direction
	for(int x= 0; x<10000; x++)  //Loop the stepping enough times for motion to be visible
	  {
		cyhal_gpio_write(STEP, true); //Trigger one step
		cyhal_system_delay_ms(1);
	    cyhal_gpio_write(STEP, false); //Pull step pin low so it can be triggered again
	    cyhal_system_delay_ms(1);
	  }
}


void drive_stepper_motor_cw(void)
{
	cyhal_gpio_write(ENABLE, false); //enable the stepper motor control
	cyhal_gpio_write(DIR, true);    //turn is a direction
	for(int x= 0; x<10000; x++)  //Loop the stepping enough times for motion to be visible
	  {
		cyhal_gpio_write(STEP, true); //Trigger one step
		cyhal_system_delay_ms(1);
	    cyhal_gpio_write(STEP, false); //Pull step pin low so it can be triggered again
	    cyhal_system_delay_ms(1);
	  }
}
