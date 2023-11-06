/*
 * touch.c
 *
 *  Created on: Sep 23, 2023
 *      Author: khair
 */

#include "touch.h"
#include "mtb_xpt2046.h"
#include <stdlib.h>
#include <math.h>
#include "mtb_hx8347.h"
#include "GUI.h"
//#include "cy_retarget_io.h"
#include <stdio.h>

tp_dev_t s_tTouch;

void tp_draw_touch_point(uint16_t hwXpos, uint16_t hwYpos)
{
	GUI_SetFont(GUI_FONT_16B_1);
	GUI_DrawLine(hwXpos - 12, hwYpos, hwXpos + 13, hwYpos);
	GUI_DrawLine(hwXpos, hwYpos - 12, hwXpos, hwYpos + 13);
	GUI_DrawPoint(hwXpos + 1, hwYpos + 1);
	GUI_DrawPoint(hwXpos - 1, hwYpos + 1);
	GUI_DrawPoint(hwXpos + 1, hwYpos - 1);
	GUI_DrawPoint(hwXpos - 1, hwYpos - 1);
	GUI_DrawCircle(hwXpos, hwYpos, 6);
}


void tp_draw_big_point(uint16_t hwXpos, uint16_t hwYpos)
{
	GUI_DrawPoint(hwXpos, hwYpos);
	GUI_DrawPoint(hwXpos + 1, hwYpos);
	GUI_DrawPoint(hwXpos, hwYpos + 1);
	GUI_DrawPoint(hwXpos + 1, hwYpos + 1);
}


void tp_show_info(uint16_t hwXpos0, uint16_t hwYpos0,
                     uint16_t hwXpos1, uint16_t hwYpos1,
                     uint16_t hwXpos2, uint16_t hwYpos2,
                     uint16_t hwXpos3, uint16_t hwYpos3, uint16_t hwFac)
{

	GUI_SetColor(GUI_RED); //font 16
	GUI_DispStringAt("x1", 40, 160);
	GUI_DispStringAt("y1", 40 + 80, 160);

	GUI_DispStringAt("x2", 40, 180);
	GUI_DispStringAt("y2",40 + 80, 180);

	GUI_DispStringAt("x3", 40, 200);
	GUI_DispStringAt("y3", 40 + 80, 200);

	GUI_DispStringAt("x4", 40, 220);
	GUI_DispStringAt("y4",40 + 80, 220);

	GUI_DispStringAt("fac is:", 40, 240);

	GUI_DispDecAt(hwXpos0, 40 + 24, 160, 4);   //xpos, ypos, num, len, size, color
	GUI_DispDecAt(hwYpos0, 40 + 24 + 80, 160, 4);

	GUI_DispDecAt(hwXpos1, 40 + 24, 180, 4);
	GUI_DispDecAt(hwYpos1, 40 + 24 + 80, 180, 4);

	GUI_DispDecAt(hwXpos2, 40 + 24, 200, 4);
	GUI_DispDecAt(hwYpos2, 40 + 24 + 80, 200, 4);

	GUI_DispDecAt(hwXpos3, 40 + 24, 220, 4);
	GUI_DispDecAt(hwYpos3, 40 + 24 + 80, 220, 4);

	GUI_DispDecAt(hwFac, 40 + 56, 240, 3);
}


uint8_t tp_scan(uint8_t chCoordType)
{
	if (!(__TP_IRQ_READ())) {
		if (chCoordType) {
			xpt2046_twice_read_xy(&s_tTouch.hwXpos, &s_tTouch.hwYpos);
		} else if (xpt2046_twice_read_xy(&s_tTouch.hwXpos, &s_tTouch.hwYpos)) {
			s_tTouch.hwXpos = s_tTouch.fXfac * s_tTouch.hwXpos + s_tTouch.iXoff;
			s_tTouch.hwYpos = s_tTouch.fYfac * s_tTouch.hwYpos + s_tTouch.iYoff;
		}
		if (0 == (s_tTouch.chStatus & TP_PRESS_DOWN)) {
			s_tTouch.chStatus = TP_PRESS_DOWN | TP_PRESSED;
			s_tTouch.hwXpos0 = s_tTouch.hwXpos;
			s_tTouch.hwYpos0 = s_tTouch.hwYpos;
		}

	} else {
		if (s_tTouch.chStatus & TP_PRESS_DOWN) {
			s_tTouch.chStatus &= ~(1 << 7);
		} else {
			s_tTouch.hwXpos0 = 0;
			s_tTouch.hwYpos0 = 0;
			s_tTouch.hwXpos = 0xffff;
			s_tTouch.hwYpos = 0xffff;
		}
	}
    //printf("chStatus = %d\t%d\n", s_tTouch.hwXpos, s_tTouch.hwYpos);
	//printf("chStatus = %x\n", s_tTouch.chStatus);
	return (s_tTouch.chStatus & TP_PRESS_DOWN);
}


void tp_adjust(void)
{
	uint8_t  cnt = 0;
	uint16_t hwTimeout = 0, d1, d2, pos_temp[4][2];
	uint32_t tem1, tem2;
	float fac;

	GUI_Clear();
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_RED);
	GUI_SetFont(GUI_FONT_16_1);
	GUI_Clear();
	GUI_SetBkColor(GUI_WHITE);
	GUI_DispStringAt("Please use the stylus click the cross on the screen.", 5, 40);
	GUI_DispStringAt("Cross will move until screen adjustment is completed.", 5, 60);
	GUI_SetColor(GUI_RED);
	tp_draw_touch_point(20, 20);
	s_tTouch.chStatus = 0;
	s_tTouch.fXfac = 0;
	while (1) {
		tp_scan(1);
		if((s_tTouch.chStatus & 0xC0) == TP_PRESSED) {
			//printf("tp pressed\n");
			hwTimeout = 0;
			s_tTouch.chStatus &= ~(1 << 6);

			pos_temp[cnt][0] = s_tTouch.hwXpos;
			pos_temp[cnt][1] = s_tTouch.hwYpos;
			cnt ++;
			switch(cnt) {
				case 1:
					GUI_SetColor(GUI_WHITE);
					tp_draw_touch_point(20, 20);
					GUI_SetColor(GUI_RED);
					tp_draw_touch_point(LCD_WIDTH - 20, 20);
					break;
				case 2:
					GUI_SetColor(GUI_WHITE);
					tp_draw_touch_point(LCD_WIDTH - 20, 20);
					GUI_SetColor(GUI_RED);
					tp_draw_touch_point(20, LCD_HEIGHT - 20);
					break;
				case 3:
					GUI_SetColor(GUI_WHITE);
					tp_draw_touch_point(20, LCD_HEIGHT - 20);
					GUI_SetColor(GUI_RED);
					tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20);
					break;
				case 4:
					tem1=abs((int16_t)(pos_temp[0][0]-pos_temp[1][0]));//x1-x2
					tem2=abs((int16_t)(pos_temp[0][1]-pos_temp[1][1]));//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d1=sqrt(tem1);

					tem1=abs((int16_t)(pos_temp[2][0]-pos_temp[3][0]));//x3-x4
					tem2=abs((int16_t)(pos_temp[2][1]-pos_temp[3][1]));//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d2=sqrt(tem1);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0) {
						cnt=0;
 						tp_show_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
 						GUI_SetColor(GUI_WHITE);
 						GUI_FillRect(96, 240, 24, 16);
						tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20);
						GUI_SetColor(GUI_RED);
						tp_draw_touch_point(20, 20);
						continue;
					}

					tem1=abs((int16_t)(pos_temp[0][0]-pos_temp[2][0]));//x1-x3
					tem2=abs((int16_t)(pos_temp[0][1]-pos_temp[2][1]));//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d1=sqrt(tem1);//

					tem1=abs((int16_t)(pos_temp[1][0]-pos_temp[3][0]));//x2-x4
					tem2=abs((int16_t)(pos_temp[1][1]-pos_temp[3][1]));//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d2=sqrt(tem1);//
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05) {
						cnt=0;
 						tp_show_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//??¡§o?¡§oy?Y
 						GUI_SetColor(GUI_WHITE);
 						GUI_FillRect(96, 240, 24, 16);
						tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20);
						GUI_SetColor(GUI_RED);
						tp_draw_touch_point(20, 20);
						continue;
					}//

					tem1=abs((int16_t)(pos_temp[1][0]-pos_temp[2][0]));//x1-x3
					tem2=abs((int16_t)(pos_temp[1][1]-pos_temp[2][1]));//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d1=sqrt(tem1);//

					tem1=abs((int16_t)(pos_temp[0][0]-pos_temp[3][0]));//x2-x4
					tem2=abs((int16_t)(pos_temp[0][1]-pos_temp[3][1]));//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d2=sqrt(tem1);//
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05) {
						cnt=0;
 						tp_show_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//??¡§o?¡§oy?Y
 						GUI_SetColor(GUI_WHITE);
 						GUI_FillRect(96, 240, 24, 16);
						tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20);
						GUI_SetColor(GUI_RED);
						tp_draw_touch_point(20, 20);
						continue;
					}

					s_tTouch.fXfac = (float)(LCD_WIDTH - 40) / (int16_t)(pos_temp[1][0] - pos_temp[0][0]);
					s_tTouch.iXoff = (LCD_WIDTH - s_tTouch.fXfac * (pos_temp[1][0] + pos_temp[0][0])) / 2;

					s_tTouch.fYfac = (float)(LCD_HEIGHT - 40) / (int16_t)(pos_temp[2][1] - pos_temp[0][1]);
					s_tTouch.iYoff = (LCD_HEIGHT - s_tTouch.fYfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;


					if(abs(s_tTouch.fXfac) > 2 || abs(s_tTouch.fYfac) > 2) {
						cnt=0;
						GUI_SetColor(GUI_WHITE);
 				    	tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20);
 				    	GUI_SetColor(GUI_RED);
						tp_draw_touch_point(20, 20);
						GUI_DispStringAt("TP Need readjust!", 40, 26);
						continue;
					}
					GUI_Clear();
					GUI_SetBkColor(GUI_WHITE);
					GUI_DispStringAt("Touch Screen Adjust OK!", 35, 110);
					cyhal_system_delay_ms(1000);
					GUI_Clear();
					GUI_SetBkColor(GUI_WHITE);
					return;
			}
		}
		cyhal_system_delay_ms(100);
		if (++ hwTimeout >= 1000) {
			break;
		}
 	}
}


void tp_dialog(void)
{
	GUI_Clear();
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLUE);
	GUI_SetFont(GUI_FONT_16B_1);
	GUI_DispStringAt("CLEAR", LCD_WIDTH - 40, 0);
}

void tp_draw_board(void)
{
	tp_scan(0);
	if (s_tTouch.chStatus & TP_PRESS_DOWN) {
		if (s_tTouch.hwXpos < LCD_WIDTH && s_tTouch.hwYpos < LCD_HEIGHT) {
			if (s_tTouch.hwXpos > (LCD_WIDTH - 40) && s_tTouch.hwYpos < 16) {
				tp_dialog();
			} else {
				GUI_SetColor(GUI_RED);
				GUI_DrawPoint(s_tTouch.hwXpos, s_tTouch.hwYpos);
			}
		}
	}
}
