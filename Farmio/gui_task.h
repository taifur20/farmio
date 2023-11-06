

/*******************************************************************************
 * Include guard
 ******************************************************************************/
#ifndef SOURCE_GUI_TASK_H_
#define SOURCE_GUI_TASK_H_


/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/*******************************************************************************
* Global constants
*******************************************************************************/
/* Allowed TCPWM compare value for maximum brightness */
//#define LED_MAX_BRIGHTNESS  (100u)

/* Allowed TCPWM compare value for minimum brightness*/
//#define LED_MIN_BRIGHTNESS  (2u)


/*******************************************************************************
 * Data structure and enumeration
 ******************************************************************************/
/* Available LED commands */
typedef enum
{
    BOT_START,
    BOT_STOP,
    BOT_SPEED_UPDATE,
} bot_command_t;

/* Structure used for storing LED data */
typedef struct
{
    bot_command_t command;
    uint32_t speed;
} bot_command_data_t;


/*******************************************************************************
 * Global variable
 ******************************************************************************/
extern QueueHandle_t bot_command_data_q;


/*******************************************************************************
 * Function prototype
 ******************************************************************************/
void task_gui(void* param);


#endif /* SOURCE_GUI_TASK_H_ */


/* [] END OF FILE  */
