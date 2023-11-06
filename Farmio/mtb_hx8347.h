/*
 * mtb_hx8347.h
 *
 *  Created on: Apr 18, 2023
 *  Author: Md. Khairul Alam
 */

#pragma once

#include <stdint.h>
#include "cy_result.h"
#include "cyhal.h"
#include "cybsp.h"


#if defined(__cplusplus)
extern "C"
{
#endif


#define LCD_CMD					    0
#define LCD_DATA				    1

#define WHITE						0xFFFF
#define BLACK						0x0000
#define BLUE                        0x001F

#define LCD_WIDTH    240
#define LCD_HEIGHT   320

#define LCD_BL_PIN CYBSP_D9
#define LCD_DC_PIN CYBSP_D7
#define LCD_CS_PIN CYBSP_D10


#define __LCD_CS_OUT()      cyhal_gpio_init(LCD_CS_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false)
#define __LCD_DC_OUT()      cyhal_gpio_init(LCD_DC_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false)
#define __LCD_BL_OUT()      cyhal_gpio_init(LCD_BL_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false)

#define __LCD_CS_CLR()      cyhal_gpio_write(LCD_CS_PIN, false)
#define __LCD_CS_SET()      cyhal_gpio_write(LCD_CS_PIN, true)
#define __LCD_DC_CLR()      cyhal_gpio_write(LCD_DC_PIN, false)
#define __LCD_DC_SET()      cyhal_gpio_write(LCD_DC_PIN, true)
#define __LCD_BL_OFF()		cyhal_gpio_write(LCD_BL_PIN, false)
#define __LCD_BL_ON()		cyhal_gpio_write(LCD_BL_PIN, true)

/**
* \addtogroup group_board_libs TFT Display
* \{
* APIs for controlling the TFT display on the board.
*/


/**
 * Initializes GPIOs for the software i8080 8-bit interface.
 * @param[in] pins Structure providing the pins to use for the display
 * @return CY_RSLT_SUCCESS if successfully initialized, else an error about
 * what went wrong
 */
void mtb_hx8347_init(void);

/**
 * Sets value of the display Reset pin.
 * @param[in] value The value to set on the pin
 */
//void mtb_hx8347_write_reset_pin(bool value);

/**
 * Writes one byte of data to the software i8080 interface with the LCD_DC pin
 * set to 0. Followed by a low pulse on the NWR line to complete the write.
 * @param[in] command The command to issue to the display
 */
void mtb_hx8347_write_command(uint8_t command);

/**
 * Writes one byte of data to the software i8080 interface with the LCD_DC pin
 * set to 1. Followed by a low pulse on the NWR line to complete the write.
 * @param[in] data The value to issue to the display
 */
void mtb_hx8347_write_data(uint8_t data);

/**
 * Writes multiple command bytes to the software i8080 interface with the LCD_DC
 * pin set to 0.
 * @param[in] data Pointer to the commands to send to the display
 * @param[in] num  The number of commands in the data array to send to the display
 */
void mtb_hx8347_write_command_stream(uint8_t *data, int num);

/**
 * Writes multiple bytes of data to the software i8080 interface with the LCD_DC
 * pin set to 1.
 * @param[in] data Pointer to the data to send to the display
 * @param[in] num  The number of bytes in the data array to send to the display
 */
void mtb_hx8347_write_data_stream(uint8_t *data, int num);

/**
 * Reads one byte of data from the software i8080 interface with the LCD_DC pin
 * set to 1.
 * @return The byte read from the display
 */
uint8_t mtb_hx8347_read_data(void);

/**
 * Reads multiple bytes of data from the software i8080 interface with the LCD_DC
 * pin set to 1.
 * @param[in,out]   data Pointer to where to store the bytes read from the display
 * @param[in]       num  The number of bytes to read from the display
 */
void mtb_hx8347_read_data_stream(uint8_t *data, int num);

/**
 * Free all resources used for the software i8080 interface.
 */
void mtb_hx8347_free(void);

/** \} group_board_libs */

void lcd_write_byte(uint8_t chByte, uint8_t chCmd);
void lcd_write_word(uint16_t hwData);
void lcd_write_register(uint8_t chRegister, uint8_t chValue);
void lcd_set_cursor(uint16_t hwXpos, uint16_t hwYpos);
void lcd_clear_screen(uint16_t hwColor);


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
