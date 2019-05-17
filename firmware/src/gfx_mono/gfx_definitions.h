/* ************************************************************************** */
/** Descriptive File Name

    Microchip
    global definitions for gfx mono library
 *  
 */
/* ************************************************************************** */

#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "FreeRTOS.h"
#include "task.h"

#define GFX_DELAY_FUNCTION(x)           vTaskDelay( pdMS_TO_TICKS( x ) )
#define GFX_SPI_WRITE_FUNCTION(x,y)     SPI0_Write(x, y)
#define GFX_SPI_IS_BUSY()               SPI0_IsBusy()
#define GFX_DISPLAY_RESET_CLEAR()       DISPLAY_RESET_Clear()
#define GFX_DISPLAY_RESET_SET()         DISPLAY_RESET_Set()
#define GFX_DISPLAY_SS_N_CLEAR()        DISPLAY_SS_N_Clear()
#define GFX_DISPLAY_SS_N_SET()          DISPLAY_SS_N_Set()
#define GFX_DATA_CMD_SEL_CLEAR()        DATA_CMD_SEL_Clear()
#define GFX_DATA_CMD_SEL_SET()          DATA_CMD_SEL_Set()

#include "../config/sam_e70_xult_freertos/peripheral/pio/plib_pio.h"
#include "../config/sam_e70_xult_freertos/peripheral/spi/plib_spi0.h"

#define GFX_MONO_UG_2832HSWEG04
#include "../../gfx_mono/gfx_mono_text.h"


#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
