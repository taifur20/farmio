/*
 * mtb_hx8347.c
 *
 *  Created on: Apr 18, 2023
 *  Author: Md. Khairul Alam
 */

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "mtb_hx8347.h"
#include "GUI.h"



extern cyhal_spi_t mSPI;

void lcd_write_byte(uint8_t chByte, uint8_t chCmd)
{
    if (chCmd) {
        __LCD_DC_SET();
    } else {
        __LCD_DC_CLR();
    }
    __LCD_CS_CLR();
        cyhal_spi_send(&mSPI, chByte);
    __LCD_CS_SET();
}


void lcd_write_word(uint16_t hwData)
{
	uint8_t hval = hwData >> 8;
	uint8_t lval = hwData & 0xFF;
    __LCD_DC_SET();
    __LCD_CS_CLR();
        cyhal_spi_send(&mSPI, hval);
        cyhal_spi_send(&mSPI, lval);
    __LCD_CS_SET();
}


void lcd_write_register(uint8_t chRegister, uint8_t chValue)
{
	lcd_write_byte(chRegister, LCD_CMD);
	lcd_write_byte(chValue, LCD_DATA);
}


void lcd_set_cursor(uint16_t hwXpos, uint16_t hwYpos)
{

	if (hwXpos >= LCD_WIDTH || hwYpos >= LCD_HEIGHT) {
				return;
    }

	lcd_write_register(0x02, hwXpos >> 8);
	lcd_write_register(0x03, hwXpos & 0xFF); //Column Start
	lcd_write_register(0x06, hwYpos >> 8);
	lcd_write_register(0x07, hwYpos & 0xFF); //Row Start

}

//clear the lcd with the specified color.
void lcd_clear_screen(uint16_t hwColor)
{
	uint32_t i, wCount = LCD_WIDTH;
	uint8_t hval = hwColor >> 8;
	uint8_t lval = hwColor & 0xFF;
	wCount *= LCD_HEIGHT;


    lcd_set_cursor(0, 0);
    lcd_write_byte(0x22, LCD_CMD);

    __LCD_DC_SET();
    __LCD_CS_CLR();
    for (i = 0; i < wCount; i ++) {
        cyhal_spi_send(&mSPI, hval);
        cyhal_spi_send(&mSPI, lval);
    }
    __LCD_CS_SET();

}


void mtb_hx8347_init(void)
{
    __LCD_DC_OUT();
    __LCD_DC_SET();

    __LCD_CS_OUT();
    __LCD_CS_SET();

    __LCD_BL_OUT();
    __LCD_BL_OFF();

    __LCD_DC_CLR();
    __LCD_CS_CLR();


    lcd_write_register(0xEA,0x00); //PTBA[15:8]
    lcd_write_register(0xEB,0x20); //PTBA[7:0]
    lcd_write_register(0xEC,0x0C); //STBA[15:8]
    lcd_write_register(0xED,0xC4); //STBA[7:0]
    lcd_write_register(0xE8,0x38); //OPON[7:0]
    lcd_write_register(0xE9,0x10); //OPON1[7:0]
    lcd_write_register(0xF1,0x01); //OTPS1B
    lcd_write_register(0xF2,0x10); //GEN
    //Gamma 2.2 Setting
    lcd_write_register(0x40,0x01); //
    lcd_write_register(0x41,0x00); //
    lcd_write_register(0x42,0x00); //
    lcd_write_register(0x43,0x10); //
    lcd_write_register(0x44,0x0E); //
    lcd_write_register(0x45,0x24); //
    lcd_write_register(0x46,0x04); //
    lcd_write_register(0x47,0x50); //
    lcd_write_register(0x48,0x02); //
    lcd_write_register(0x49,0x13); //
    lcd_write_register(0x4A,0x19); //
    lcd_write_register(0x4B,0x19); //
    lcd_write_register(0x4C,0x16); //
    lcd_write_register(0x50,0x1B); //
    lcd_write_register(0x51,0x31); //
    lcd_write_register(0x52,0x2F); //
    lcd_write_register(0x53,0x3F); //
    lcd_write_register(0x54,0x3F); //
    lcd_write_register(0x55,0x3E); //
    lcd_write_register(0x56,0x2F); //
    lcd_write_register(0x57,0x7B); //
    lcd_write_register(0x58,0x09); //
    lcd_write_register(0x59,0x06); //
    lcd_write_register(0x5A,0x06); //
    lcd_write_register(0x5B,0x0C); //
    lcd_write_register(0x5C,0x1D); //
    lcd_write_register(0x5D,0xCC); //
    //Power Voltage Setting
    lcd_write_register(0x1B,0x1B); //VRH=4.65V
    lcd_write_register(0x1A,0x01); //BT (VGH~15V,VGL~-10V,DDVDH~5V)
    lcd_write_register(0x24,0x2F); //VMH(VCOM High voltage ~3.2V)
    lcd_write_register(0x25,0x57); //VML(VCOM Low voltage -1.2V)
    //VCOM offset
    lcd_write_register(0x23,0x88); //for Flicker adjust //can reload from OTP
    //Power on Setting
    lcd_write_register(0x18,0x34); //I/P_RADJ,N/P_RADJ, Normal mode 60Hz
    lcd_write_register(0x19,0x01); //OSC_EN='1', start Osc
    lcd_write_register(0x01,0x00); //DP_STB='0', out deep sleep
    lcd_write_register(0x1F,0x88);// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
    lcd_write_register(0x1F,0x80);// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
    lcd_write_register(0x1F,0x90);// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
    lcd_write_register(0x1F,0xD0);// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    //262k/65k color selection
    lcd_write_register(0x17,0x05); //default 0x06 262k color // 0x05 65k color
    //SET PANEL
    lcd_write_register(0x36,0x00); //SS_P, GS_P,REV_P,BGR_P
    //Display ON Setting
    lcd_write_register(0x28,0x38); //GON=1, DTE=1, D=1000
    lcd_write_register(0x28,0x3F); //GON=1, DTE=1, D=1100
    lcd_write_register(0x16,0x18);
    //Set GRAM Area
    lcd_write_register(0x02,0x00);
    lcd_write_register(0x03,0x00); //Column Start
    lcd_write_register(0x04,0x00);
    lcd_write_register(0x05,0xEF); //Column End
    lcd_write_register(0x06,0x00);
    lcd_write_register(0x07,0x00); //Row Start
    lcd_write_register(0x08,0x01);
    lcd_write_register(0x09,0x3F); //Row End

    //lcd_clear_screen(WHITE);
    //lcd_clear_screen(GUI_WHITE);

    __LCD_BL_ON();

}


/*******************************************************************************
 * Writes one byte of data to the software i8080 interface with the LCD_DC pin
 *******************************************************************************/
void mtb_hx8347_write_command(uint8_t data)
{

	__LCD_DC_CLR();

	__LCD_CS_CLR();
	cyhal_spi_send(&mSPI, data);
	__LCD_CS_SET();
}


/*******************************************************************************
 * Writes one byte of data to the software i8080 interface with the LCD_DC pin
 *******************************************************************************/
void mtb_hx8347_write_data(uint8_t data)
{

	__LCD_DC_SET();

    __LCD_CS_CLR();
    cyhal_spi_send(&mSPI, data);
    __LCD_CS_SET();
}


/*******************************************************************************
 * Writes multiple command bytes to the software i8080 interface with the LCD_DC
 * pin set to 0.
 *******************************************************************************/
void mtb_hx8347_write_command_stream(uint8_t *data, int num)
{
    int i;

    __LCD_DC_CLR();

    __LCD_CS_CLR();
    for (i = 0; i < num; i++)
    {
    	cyhal_spi_send(&mSPI, data[i]);
    }
    __LCD_CS_SET();
}


/*******************************************************************************
 * Writes multiple bytes of data to the software i8080 interface with the LCD_DC
 * pin set to 1.
 *******************************************************************************/
void mtb_hx8347_write_data_stream(uint8_t *data, int num)
{
	int i;

	__LCD_DC_SET();

	__LCD_CS_CLR();
	for (i = 0; i < num; i++)
	{
	    cyhal_spi_send(&mSPI, data[i]);
	    //cyhal_spi_transfer(&mSPI, (const uint8_t*)data, num, NULL, 0, 0);
	}
	__LCD_CS_SET();
}


/*******************************************************************************
 * Reads one byte of data from the software i8080 interface with the LCD_DC pin
 * set to 1.
 *******************************************************************************/
uint8_t mtb_hx8347_read_data(void)
{

	CY_ASSERT(0);
	return 0;

}


/*******************************************************************************
 * Reads multiple bytes of data from the software i8080 interface with the LCD_DC
 * pin set to 1.
 *******************************************************************************/
void mtb_hx8347_read_data_stream(uint8_t *data, int num)
{

	//cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    //cyhal_gpio_write(CYBSP_USER_LED, 0);
	CY_ASSERT(0);

}


/*******************************************************************************
 * Free all resources used for the software i8080 interface.
 *******************************************************************************/
void mtb_hx8347_free(void)
{
    cyhal_gpio_free(LCD_DC_PIN);
    cyhal_gpio_free(LCD_CS_PIN);
    cyhal_gpio_free(LCD_BL_PIN);
    cyhal_spi_free(&mSPI);

}

/*
cy_rslt_t MTB_E2271CS021_WriteSPIBuffer(uint8_t* data, uint16_t dataLength)
{
    return cyhal_spi_transfer(spi_ptr, (const uint8_t*)data, dataLength, NULL, 0, 0);
}
*/
