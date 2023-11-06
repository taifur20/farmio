

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* TFT GUI header */
#include "mtb_hx8347.h"
#include "GUI.h"


/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <inttypes.h>

/* Motor Control header */
#include "cytron_motor_driver.h"


/* Capsense control */
#include "capsense_task.h"
#include "gui_task.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* SPI baud rate in Hz */
#define SPI_FREQ_HZ                (10000000UL)
/* Delay of 1000ms between commands */
#define CMD_TO_CMD_DELAY           (1000UL)
/* SPI transfer bits per frame */
#define BITS_PER_FRAME             (8)





/* Priorities of user tasks in this project. configMAX_PRIORITIES is defined in
 * the FreeRTOSConfig.h and higher priority numbers denote high priority tasks.
 */
#define TASK_CAPSENSE_PRIORITY (configMAX_PRIORITIES - 1)
#define TASK_GUI_PRIORITY (configMAX_PRIORITIES - 2)

/* Stack sizes of user tasks in this project */
#define TASK_CAPSENSE_STACK_SIZE (256u)
#define TASK_GUI_STACK_SIZE (256u)
//#define TASK_GUI_STACK_SIZE (configMINIMAL_STACK_SIZE)

/* Queue lengths of message queues used in this project */
#define SINGLE_ELEMENT_QUEUE (1u)

/*******************************************************************************
* Global Variables
*******************************************************************************/
cyhal_spi_t mSPI;
cy_rslt_t rslt;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/


/*******************************************************************************
* Function Definitions
*******************************************************************************/
void handle_error(cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        /* Halt the CPU while debugging */
        CY_ASSERT(0);
    }
}
/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CPU. It...
*    1.
*    2.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    handle_error(result);

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize the retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                     CY_RETARGET_IO_BAUDRATE);
    handle_error(result);

    /* Initialize the SPI to use the TFT */
    result = cyhal_spi_init(&mSPI, CYBSP_SPI_MOSI, CYBSP_SPI_MISO, CYBSP_SPI_CLK,
        	                                    NC, NULL, BITS_PER_FRAME,
        	                                    CYHAL_SPI_MODE_11_MSB, false);
    handle_error(result);

    /* Set SPI frequency */
    result = cyhal_spi_set_frequency(&mSPI, SPI_FREQ_HZ);
    handle_error(result);

    /*Initialize motor driver */
    cytron_motor_driver_init_pwm(PWM_INDEPENDENT, CYBSP_D2, CYBSP_D8, CYBSP_D5, CYBSP_D6); //digita, digital, pwm, pwm

    /* drive the motor */
    cytron_motor_driver_control(150, 150); //left_speed, right_speed

    printf("Starting the program\n");
    /* Create the queues. See the respective data-types for details of queue
     * contents
     */
    bot_command_data_q = xQueueCreate(SINGLE_ELEMENT_QUEUE,
                                          sizeof(bot_command_data_t));
    capsense_command_q = xQueueCreate(SINGLE_ELEMENT_QUEUE,
                                          sizeof(capsense_command_t));

    /* Create the user tasks. See the respective task definition for more
     * details of these tasks.
     */
    xTaskCreate(task_capsense, "CapSense Task", TASK_CAPSENSE_STACK_SIZE,
                    NULL, TASK_CAPSENSE_PRIORITY, NULL);
    xTaskCreate(task_gui, "Gui Task", TASK_GUI_STACK_SIZE,
                    NULL, TASK_GUI_PRIORITY, NULL);

    /* Test the TFT
    GUI_Init();
    GUI_SetFont(GUI_FONT_32B_1);
    GUI_SetTextAlign(GUI_TA_CENTER);
    GUI_DispStringAt("Hello RTOS.", GUI_GetScreenSizeX()/2,GUI_GetScreenSizeY()/2 - GUI_GetFontSizeY()/2);
    */
    /*
    GUI_Init();

    GUI_SetColor(GUI_RED);
    GUI_SetFont(GUI_FONT_20B_1);
    GUI_SetTextAlign(GUI_TA_CENTER);
    GUI_DispStringAt("1 Square = 2 Feet", GUI_GetScreenSizeX()/2,GUI_GetScreenSizeY()/2 - GUI_GetFontSizeY()/2);
    //cyhal_system_delay_ms(2000); //this delay is not allow with rtos

    GUI_SetColor(GUI_WHITE);

    for(int i = 0; i<=240; i+=10)
        GUI_DrawHLine(i, 0, 320);

    for(int i = 0; i<=320; i+=10)
        GUI_DrawVLine(i, 0, 240);
    */

    /* Start the RTOS scheduler. This function should never return */
    vTaskStartScheduler();

    /********************** Should never get here ***************************/
    /* RTOS scheduler exited */
    /* Halt the CPU if scheduler exits */
    CY_ASSERT(0);


    for (;;)
    {
    }
}

/* [] END OF FILE */
