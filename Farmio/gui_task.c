
/*******************************************************************************
 * Header file includes
 ******************************************************************************/
#include "mtb_hx8347.h"
#include "GUI.h"
#include "gui_task.h"

#include "cybsp.h"
#include "cyhal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cycfg.h"


/*******************************************************************************
* Global constants
*******************************************************************************/


/*******************************************************************************
 * Global variable
 ******************************************************************************/
/* Queue handle used for LED data */
QueueHandle_t bot_command_data_q;
int line_gap = 0;

/*******************************************************************************
* Function Name: task_led
********************************************************************************
* Summary:
*  Task that controls the LED.
*
* Parameters:
*  void *param : Task parameter defined during task creation (unused)
*
*******************************************************************************/
void update_line_graph(int gap);

void task_gui(void* param)
{
    BaseType_t rtos_api_result;
    bot_command_data_t bot_cmd_data;

    /* Suppress warning for unused parameter */
    (void)param;


    GUI_Init();

    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_20B_1);
    GUI_SetTextAlign(GUI_TA_CENTER);
    GUI_DispStringAt("1 Square = 2 Feet", GUI_GetScreenSizeX()/2,GUI_GetScreenSizeY()/2 - GUI_GetFontSizeY()/2);
    //cyhal_system_delay_ms(2000); //this delay is not allow with rtos

    GUI_SetColor(GUI_WHITE);

    for(int i = 0; i<=240; i+=10)
        GUI_DrawHLine(i, 0, 320);

    for(int i = 0; i<=320; i+=10)
        GUI_DrawVLine(i, 0, 240);

    GUI_SetPenSize(4);
    GUI_SetColor(GUI_RED);
    vTaskDelay(3000);
    GUI_DrawLine(70, 100, 160, 100);
    vTaskDelay(3000);
    GUI_DrawLine(160, 100, 160, 160);
    vTaskDelay(3000);
    GUI_DrawLine(160, 160, 220, 160);

    /* Repeatedly running part of the task */
    for(;;)
    {
        /* Block until a command has been received over queue */
        rtos_api_result = xQueueReceive(bot_command_data_q, &bot_cmd_data,
                            portMAX_DELAY);

        /* Command has been received from queue */
        if(rtos_api_result == pdTRUE)
        {
            switch(bot_cmd_data.command)
            {
                /* Turn on the LED. */
                case BOT_START:
                {
                	GUI_SetPenSize(15);
                    line_gap+=3; if(line_gap>30) line_gap = 30;
                    update_line_graph(10+line_gap);
                    break;
                }
                /* Turn off LED */
                case BOT_STOP:
                {
                	line_gap-=3; if(line_gap<0) line_gap = 0;
                	update_line_graph(10+line_gap);
                    break;
                }
                /* Update LED brightness */
                case BOT_SPEED_UPDATE:
                {
                    if ((bot_cmd_data.speed>0))
                    {
                    	GUI_DispDecAt(bot_cmd_data.speed, 200, 0, 2);
                    	line_gap = bot_cmd_data.speed;
                    	update_line_graph(10+line_gap/3);
                    }
                    break;
                }
                /* Invalid command */
                default:
                {
                    /* Handle invalid command here */
                    break;
                }
            }
        }

        /* Task has timed out and received no data during an interval of
         * portMAXDELAY ticks.
         */
        else
        {
            /* Handle timeout here */
        }
    }
}

void update_line_graph(int gap){
	GUI_Clear();
	int height = 240/gap;
	int width = 360/gap;
	int area = 4*height*width;
	GUI_DispStringAt("Gap = 2 Feet", 10, 50);
	GUI_DispStringAt("Total Area =          SF", 10, 70);
	GUI_DispDecAt(area, 120, 70, 4);
	for(int i = 0; i<=240; i+=gap)
		GUI_DrawHLine(i, 0, 320);

	for(int i = 0; i<=320; i+=gap)
	    GUI_DrawVLine(i, 0, 240);
}


/* END OF FILE [] */
