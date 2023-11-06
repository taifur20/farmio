/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software  has been licensed to  Cypress Semiconductor Corporation,
whose registered  office is situated  at 198 Champion Ct. San Jose, CA
95134 USA  solely for the  purposes of creating  libraries for Cypress
PSoC3 and  PSoC5 processor-based devices,  sublicensed and distributed
under  the  terms  and  conditions  of  the  Cypress  End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              Cypress Semiconductor Corp, 198 Champion Ct., San Jose, CA 95134, USA
Licensed SEGGER software: emWin
License number:           GUI-00319
License model:            Services and License Agreement, signed June 10th, 2009
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2009-06-12 - 2022-07-27
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Generic  configuration file for GUIDRV_FlexColor
---------------------------END-OF-HEADER------------------------------
*/

#include "emwin.h"

#if defined(EMWIN_ENABLED)

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "LCDConf.h"
#include "mtb_hx8347.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS MTB_DISPLAY_SIZE_X
#define YSIZE_PHYS MTB_DISPLAY_SIZE_Y

//
// Color conversion
//   The color conversion functions should be selected according to
//   the color mode of the target display. Details can be found in
//   the chapter "Colors" in the emWin user manual.
//
//#define COLOR_CONVERSION GUICC_M565
#define COLOR_CONVERSION GUICC_565

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
    #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
    #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
    #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
    #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
    #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
    #error No display driver defined!
#endif


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void)
{
    GUI_DEVICE * pDevice;
    CONFIG_FLEXCOLOR Config = {0};
    GUI_PORT_API PortAPI = {0};
    //
    // Set the display driver and color conversion
    //
    pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
    //
    // Display size configuration
    //
    LCD_SetSizeEx (0, XSIZE_PHYS,  YSIZE_PHYS);
    LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
    //
    // Orientation
    //
    Config.Orientation   = GUI_MIRROR_Y | GUI_SWAP_XY;
    GUIDRV_FlexColor_Config(pDevice, &Config);
    //
    // Set controller and operation mode
    //
    PortAPI.pfWrite8_A0  = mtb_hx8347_write_command;
    PortAPI.pfWrite8_A1  = mtb_hx8347_write_data;
    PortAPI.pfWriteM8_A1 = mtb_hx8347_write_data_stream;
    PortAPI.pfRead8_A1   = mtb_hx8347_read_data;
    PortAPI.pfReadM8_A1  = mtb_hx8347_read_data_stream;

    //GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B8);
    GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66712, GUIDRV_FLEXCOLOR_M16C0B8);
}


int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
{
    int r;

    GUI_USE_PARA(LayerIndex);
    GUI_USE_PARA(pData);

    switch (Cmd)
    {
        case LCD_X_INITCONTROLLER:
        	mtb_hx8347_init();
            r = 0;
            break;

        default:
            r = -1;
            break;
    }

    return r;
}

#if defined(__cplusplus)
}
#endif

#endif /* defined(EMWIN_ENABLED) */


/*************************** End of file ****************************/
