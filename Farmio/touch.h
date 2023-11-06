/*
 * touch.h
 *
 *  Created on: Sep 23, 2023
 *      Author: khair
 */

#ifndef TOUCH_H_
#define TOUCH_H_


#include <stdint.h>

#define TP_PRESS_DOWN           0x80
#define TP_PRESSED              0x40 //0xC0//

typedef struct {
	uint16_t hwXpos0;
	uint16_t hwYpos0;
	uint16_t hwXpos;
	uint16_t hwYpos;
	uint8_t chStatus;
	uint8_t chType;
	short iXoff;
	short iYoff;
	float fXfac;
	float fYfac;
} tp_dev_t;

void tp_init(void);
void tp_adjust(void);
void tp_dialog(void);
void tp_draw_board(void);


#endif /* TOUCH_H_ */
