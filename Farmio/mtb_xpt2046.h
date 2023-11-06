/*
 * mtb_xpt2046.h
 *
 *  Created on: Sep 22, 2023
 *      Author: khair
 */

#ifndef MTB_XPT2046_H_
#define MTB_XPT2046_H_

#include <stdint.h>
#include <stdint.h>
#include "cyhal.h"
#include "cybsp.h"

#define TP_CS_PIN CYBSP_D4
#define TP_IRQ_PIN CYBSP_D3
//be careful about this pin, it is default CTS of debug uart and will not work with debug enabled

#define __TP_CS_OUT()       cyhal_gpio_init(TP_CS_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false)
#define __TP_IRQ_IN()       cyhal_gpio_init(TP_IRQ_PIN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, true)

#define __TP_CS_CLR()      cyhal_gpio_write(TP_CS_PIN, false)
#define __TP_CS_SET()      cyhal_gpio_write(TP_CS_PIN, true)
#define __TP_IRQ_READ()    cyhal_gpio_read(TP_IRQ_PIN)

void xpt2046_init(void);
uint16_t xpt2046_read_ad_value(uint8_t chCmd);
uint16_t xpt2046_read_average(uint8_t chCmd);
void xpt2046_read_xy(uint16_t *phwXpos, uint16_t *phwYpos);
bool xpt2046_twice_read_xy(uint16_t *phwXpos, uint16_t *phwYpos);

#endif /* MTB_XPT2046_H_ */


/*
 cyhal_gpio_init(TP_IRQ_PIN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);
 cyhal_gpio_register_callback(TP_IRQ_PIN, gpio_interrupt_handler, (void *)&global_count);
 cyhal_gpio_register_callback(TP_IRQ_PIN, gpio_interrupt_handler, NULL);
 cyhal_gpio_enable_event(TP_IRQ_PIN, CYHAL_GPIO_IRQ_FALL, 3, true); //interrupt priority set to 3
 *
 *
 *
 */
