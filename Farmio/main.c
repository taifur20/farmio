/**********Pin Maping**************************************
 *
 * A0, A1, A2 -> used by graphics display
 * P6_0, P6_1 -> used by AN pin of the motor driver
 * D3, D4, D7, D9, D10, D11, D12, D13 -> used by TFT LCD
 * D5, D6 -> used by motor driver IN pin
 * IO5, IO5, IO4, IO3 -> used by soil sensor
 * IO2, IO1, IO0 -> used by stepper motor controller
 * D8 -> used by stepper limit switch
 * D2 -> used by pump control
 */

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* TFT GUI header */
#include "mtb_hx8347.h"
#include "GUI.h"
#include "mtb_xpt2046.h"
#include "touch.h"


/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <inttypes.h>

/* handle file system */
#include "FS.h"
#include <string.h>

/* DC Motor Control header for running the bot*/
#include "cytron_motor_driver.h"

/* Grapics display control */
#include "mtb_st7920_serial.h"


/* Capsense control */
#include "capsense_task.h"
#include "gui_task.h"

/* Soil sensor & Stepper motor driving */
#include "stepper_motor.h"
#include "soil_sensor.h"

/* Stnadard C lib */
#include <stdlib.h>

/*******************************************************************************
* Macros
*******************************************************************************/
/* SPI baud rate in Hz */
#define SPI_FREQ_HZ                (10000000UL)
/* Delay of 1000ms between commands */
#define CMD_TO_CMD_DELAY           (1000UL)
/* SPI transfer bits per frame */
#define BITS_PER_FRAME             (8)

#define NUM_BYTES_TO_READ_FROM_FILE         (256U) //define your size
#define FILE_NAME                           "Soil_data.txt"




/* Priorities of user tasks in this project. configMAX_PRIORITIES is defined in
 * the FreeRTOSConfig.h and higher priority numbers denote high priority tasks.
 */
#define TASK_CAPSENSE_PRIORITY (configMAX_PRIORITIES - 1)
#define TASK_GUI_PRIORITY (configMAX_PRIORITIES - 2)
#define TASK_RUN_BOT_PRIORITY (configMAX_PRIORITIES - 3)
#define TASK_GRAPHICS_PRIORITY (configMAX_PRIORITIES - 4)
#define TASK_EMFILE_PRIORITY (configMAX_PRIORITIES - 5)


/* Stack sizes of user tasks in this project */
#define TASK_CAPSENSE_STACK_SIZE (256u)
#define TASK_GUI_STACK_SIZE (256u)
#define TASK_GRAPHICS_STACK_SIZE (256u)
#define TASK_RUN_BOT_STACK_SIZE (256u)
#define TASK_EMFILE_STACK_SIZE  (512U)
//#define TASK_GUI_STACK_SIZE (configMINIMAL_STACK_SIZE)

/* Queue lengths of message queues used in this project */
#define SINGLE_ELEMENT_QUEUE (1u)

#define SPRAY_PUMP CYBSP_D2
#define LEFT_MOTOR_DIR_PIN CYBSP_D5
#define RIGHT_MOTOR_DIR_PIN CYBSP_D6
#define LEFT_MOTOR_SPEED_PIN P6_0
#define RIGHT_MOTOR_SPEED_PIN P6_1
/*******************************************************************************
* Global Variables
*******************************************************************************/
cyhal_spi_t mSPI;
cy_rslt_t rslt;

static TaskHandle_t emfile_task_handle;
static char file_data[NUM_BYTES_TO_READ_FROM_FILE];

float soil_ph, soil_temperature, soil_moisture;
int soil_conductivity, soil_nitrogen, soil_phosphorous, soil_potassium;
int soil_data_available = 0;
int point = 0;
int n_threshold, p_threshold, k_threshold, threshold;
int point_x, point_y;
int soil_data_read_happen = 0;
int file_read_flag = 0;
/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void read_soil_data(void);
void spray_fertilizer(int amount);


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
* Task Definitions
*******************************************************************************/
static void task_graphics(void* arg)
{

	ST7920_Init();

    ST7920_SendString(0,0, "I AM FARMIO!");
    ST7920_SendString(1,0, "I");
	ST7920_SendString(2,0, "ASSIST FARMERS!");
	//ST7920_SendString(3,0, "FARMERS!");
	vTaskDelay(5000);
	ST7920_Clear();
	char value[20];

	if(soil_data_available){
		sprintf(value, "%.2f", soil_ph); //convert float value to string
		ST7920_SendString(0,0, value);

		sprintf(value, "%.2f", soil_temperature); //convert float value to string
		ST7920_SendString(0,10, value);

		sprintf(value, "%.2f", soil_moisture); //convert float value to string
	    ST7920_SendString(1,0, value);

	    sprintf(value, "%d", soil_conductivity); //convert float value to string
	    ST7920_SendString(1,10, value);

	    sprintf(value, "%d", soil_nitrogen); //convert float value to string
	    ST7920_SendString(2,0, value);

	    sprintf(value, "%d", soil_phosphorous); //convert float value to string
	    ST7920_SendString(2,10, value);

	    sprintf(value, "%d", soil_potassium); //convert float value to string
	    ST7920_SendString(3,0, value);
	}
}


static void task_run_bot(void* arg)
{
	//initialize touch
	xpt2046_init();
	//calibrate the touch
	tp_adjust();
	tp_dialog();
	//initialized stepper motor
	init_stepper();
	//initialized soil sensor
	init_soil_sensor();
	//initialize the motor driver
	cytron_motor_driver_init_pwm(PWM_INDEPENDENT, RIGHT_MOTOR_DIR_PIN, LEFT_MOTOR_DIR_PIN,
			                             RIGHT_MOTOR_SPEED_PIN, LEFT_MOTOR_SPEED_PIN);

	while(1){
		//calculate_track_point(); //read the map and point to measure
		if(point == 1)
			cytron_motor_driver_control(40, 40); //speed 40 percent & forward
		else if(point == 2)
			cytron_motor_driver_control(-10, 40); //speed 40 percent & left
		else if(point == 3)
			cytron_motor_driver_control(40, -10); //speed 40 percent & right
		else if(point == 4)
			cytron_motor_driver_control(-40, -40); //speed 40 percent & backward
		else if(point == 3)
			cytron_motor_driver_control(0, 0); //motor stop

		//read the soil
		read_soil_data();

		//spray fertilizer based on the soil data
		if(soil_nitrogen <n_threshold || soil_phosphorous<p_threshold || soil_potassium <k_threshold){
			int amount = threshold - (soil_nitrogen + soil_phosphorous + soil_potassium)/3;
			spray_fertilizer(amount);
		}
	}

}

/*******************************************************************************
* Function Name: emfile_task
********************************************************************************
* Summary:
*   Formats the storage device, reads the content from a file and prints the
*   content to the UART terminal, writes a message to the same file, and waits
*   for the user button press. When the button is pressed, deletes the file and
*   returns.
*
* Parameters:
*  arg - Unused.
*
*******************************************************************************/
static void task_emfile(void* arg)
{
    U32    volume_size;
    U32    num_bytes_to_read;
    int         error;
    FS_FILE    *file_ptr;
    const char *volume_name = "";

    //printf("Using SD card as storage device\n");

    /* Initialize the file system. */
    FS_Init();

    /* Check if volume needs to be high-level formatted. */
    error = FS_IsHLFormatted(volume_name);
    //check_error("Error in checking if volume is high-level formatted", error);

    /* Return value of 0 indicates that high-level format is required. */
    if (error == 0)
    {
       // printf("Perform high-level format\n");
        error = FS_Format(volume_name, NULL);
       // check_error("Error in high-level formatting", error);
    }

    volume_size = FS_GetVolumeSizeKB(volume_name);
    //printf("Volume size: %"PRIu32" KB\n\n", volume_size);

    if(0U == volume_size)
    {
        //printf("Error in checking the volume size\n");
        CY_ASSERT(0U);
    }

    //printf("Opening the file for reading...\n");

    /* Open the file for reading. */
    if(file_read_flag == 1){
    	file_ptr = FS_FOpen(FILE_NAME, "r");

    	if (file_ptr != NULL)
    	{
        	/* Last byte is for storing the NULL character. */
        	num_bytes_to_read = sizeof(file_data) - 1U;
        	volume_size = FS_GetFileSize(file_ptr);

        	if(volume_size < num_bytes_to_read)
        	{
            	num_bytes_to_read = volume_size;
        	}

        	printf("Reading %"PRIu32" bytes from the file. ", num_bytes_to_read);
        	volume_size = FS_Read(file_ptr, file_data, num_bytes_to_read);

        	if(volume_size != num_bytes_to_read)
        	{
            	error = FS_FError(file_ptr);
            	//check_error("Error in reading from the file", error);
        	}

        	/* Terminate the string using NULL. */
        	file_data[num_bytes_to_read] = '\0';

        	/* Display the file content. */
        	printf("File Content:\n\"%s\"\n", file_data);

        	error = FS_FClose(file_ptr);
        	//check_error("Error in closing the file", error);

        	//printf("\nOpening the file for overwriting...\n");
    	}
    	else
    	{
    		printf("Unable to read. File not found.\n");
    		//printf("\nOpening the file for writing...\n");
    	}
    }

    /* Mode 'w' truncates the file size to zero if the file exists otherwise
     * creates a new file.
     */
    //write the sensor reading
    if(soil_data_read_happen == 1){
    	char string_to_write[150];
    	sprintf(string_to_write, "%d,%d,%.2f,%.2f,%.2f,%d,%d,%d,%d", point_x, point_y, soil_ph,
    			soil_temperature, soil_moisture, soil_conductivity, soil_nitrogen, soil_phosphorous, soil_potassium);

    	file_ptr = FS_FOpen(FILE_NAME, "w");

    	if(file_ptr != NULL)
    	{
    		volume_size = FS_Write(file_ptr, string_to_write, strlen(string_to_write));

    		if(volume_size != strlen(string_to_write))
    		{
    			error = FS_FError(file_ptr);
    			//check_error("Error in writing to the file", error);
    		}

    		printf("File is written with the following message:\n");
    		printf("\"%s\"\n\n", string_to_write);

    		printf("You can now view the file content in your PC. File name is \"%s\"\n", FILE_NAME);

    		error = FS_FClose(file_ptr);
    		//check_error("Error in closing the file", error);

    		FS_Unmount(volume_name);

    		printf("Filesystem operations completed successfully!\n");
    		soil_data_read_happen = 0;
    	}
    	else
    	{
    		printf("Unable to open the file for writing! Exiting...\n");
    	}
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

    /*Initialize spray pump */
    result = cyhal_gpio_init(SPRAY_PUMP, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
    handle_error(result);
    cyhal_gpio_write(SPRAY_PUMP, false); //disable the pump

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
    xTaskCreate(task_run_bot, "Runbot Task", TASK_RUN_BOT_STACK_SIZE,
                        NULL, TASK_RUN_BOT_PRIORITY, NULL);
    xTaskCreate(task_graphics, "Graphics Task", TASK_GRAPHICS_STACK_SIZE,
                        NULL, TASK_GRAPHICS_PRIORITY, NULL);
    xTaskCreate(task_emfile, "emFile Task", TASK_EMFILE_STACK_SIZE,
                    NULL, TASK_EMFILE_PRIORITY, &emfile_task_handle);

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


void read_soil_data(void)
{
	drive_stepper_motor_down();
	soil_ph = read_ph();
	soil_temperature = read_temperature();
	soil_moisture = read_moisture();
	soil_conductivity = read_conductivity();
	soil_nitrogen = read_nitrogen();
	soil_phosphorous = read_phosphorous();
	soil_potassium = read_potassium();
	drive_stepper_motor_up();
	soil_data_available = 1;
	soil_data_read_happen = 1;
}

void spray_fertilizer(int amount)
{
	cyhal_gpio_write(SPRAY_PUMP, true);
	vTaskDelay(amount*300);
	cyhal_gpio_write(SPRAY_PUMP, false);
}

/* [] END OF FILE */
