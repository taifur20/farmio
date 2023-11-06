/*
 * mtb_xpt2046.c
 *
 *  Created on: Sep 22, 2023
 *      Author: khair
 */

#include "cyhal.h"
#include "cybsp.h"
#include "cy_result.h"
#include "mtb_xpt2046.h"
//#include "cy_retarget_io.h"
#include <stdio.h>

extern cyhal_spi_t mSPI;

void xpt2046_init(void)
{
	uint16_t hwXpos, hwYpos;
	//__TP_CS_OUT();
	//__TP_CS_SET();
	//__TP_IRQ_IN();
	cyhal_gpio_init(CYBSP_D4, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);
	cyhal_gpio_write(CYBSP_D4, true);
	cyhal_gpio_init(CYBSP_D3, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, true);
	cyhal_gpio_read(TP_IRQ_PIN);

	xpt2046_read_xy(&hwXpos, &hwYpos);
}


uint16_t xpt2046_read_ad_value(uint8_t chCmd)
{
    uint16_t hwData = 0;
    uint8_t transmit_data = chCmd;
    uint8_t receive_ldata = 0;
    uint8_t receive_hdata = 0;

    //__TP_CS_CLR();
    cyhal_gpio_write(CYBSP_D4, false);
    cyhal_spi_send(&mSPI, transmit_data);
    cyhal_spi_recv(&mSPI, &receive_hdata);
    cyhal_spi_recv(&mSPI, &receive_ldata);
    hwData = receive_hdata << 8 | receive_ldata;
    hwData >>= 4;
    cyhal_gpio_write(CYBSP_D4, true);
    //__TP_CS_SET();

    //printf("data = %d\n", hwData);
    //cyhal_system_delay_ms(500);
    return hwData;
}


#define READ_TIMES  5
#define LOST_NUM    1
uint16_t xpt2046_read_average(uint8_t chCmd)
{
    int i, j, count = 0;
    uint16_t hwbuffer[5] = {0,0,0,0,0}, hwSum = 0, hwTemp;

    hwbuffer[0] = xpt2046_read_ad_value(chCmd);//cyhal_system_delay_ms(1);
    hwbuffer[1] = xpt2046_read_ad_value(chCmd);//cyhal_system_delay_ms(1);
    hwbuffer[2] = xpt2046_read_ad_value(chCmd);//cyhal_system_delay_ms(1);
    hwbuffer[3] = xpt2046_read_ad_value(chCmd);//cyhal_system_delay_ms(1);
    hwbuffer[4] = xpt2046_read_ad_value(chCmd);//cyhal_system_delay_ms(1);

    //for (i = 0; i < 5; i++) {
    //    hwbuffer[i] = xpt2046_read_ad_value(chCmd);
    //    printf("hard average %d = %d\n", i , hwbuffer[i]);
    //}

    for (i = 0; i < READ_TIMES - 1; i ++) {
        for (j = i + 1; j < READ_TIMES; j ++) {
            if (hwbuffer[i] > hwbuffer[j]) {
                hwTemp = hwbuffer[i];
                hwbuffer[i] = hwbuffer[j];
                hwbuffer[j] = hwTemp;
            }
        }
    }
    for (i = LOST_NUM; i < READ_TIMES - LOST_NUM; i ++) {
        hwSum += hwbuffer[i];
    }
    hwTemp = hwSum / (READ_TIMES - 2 * LOST_NUM);
    return hwTemp;
}

void xpt2046_read_xy(uint16_t *phwXpos, uint16_t *phwYpos)
{
	*phwXpos = xpt2046_read_average(0xD0);
	*phwYpos = xpt2046_read_average(0x90);
    //printf("hw Xpos = %d\thw Ypos = %d\n", *phwXpos, *phwYpos);
}


#define ERR_RANGE 50
bool xpt2046_twice_read_xy(uint16_t *phwXpos, uint16_t *phwYpos)
{
	uint16_t hwXpos1, hwYpos1, hwXpos2, hwYpos2;

	xpt2046_read_xy(&hwXpos1, &hwYpos1);
	xpt2046_read_xy(&hwXpos2, &hwYpos2);
	//printf("hw Xpos1 = %d\thw Ypos1 = %d\n", hwXpos1, hwYpos1);
	//printf("hw Xpos2 = %d\thw Ypos2 = %d\n", hwXpos2, hwYpos2);

	if (((hwXpos2 <= hwXpos1 && hwXpos1 < hwXpos2 + ERR_RANGE) || (hwXpos1 <= hwXpos2 && hwXpos2 < hwXpos1 + ERR_RANGE))
	&& ((hwYpos2 <= hwYpos1 && hwYpos1 < hwYpos2 + ERR_RANGE) || (hwYpos1 <= hwYpos2 && hwYpos2 < hwYpos1 + ERR_RANGE))) {
		*phwXpos = (hwXpos1 + hwXpos2) / 2;
		*phwYpos = (hwYpos1 + hwYpos2) / 2;
		//printf("hw Xpos = %d\thw Ypos = %d\n", *phwXpos, *phwYpos);
		return true;
	}

	return false;
}
