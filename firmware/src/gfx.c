/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    gfx.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "gfx.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
extern struct font sysfont;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the GFX_Initialize function.

    Application strings and buffers are be defined outside this structure.
 */

GFX_DATA gfxData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
 */


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void GFX_Initialize ( void )

  Remarks:
    See prototype in gfx.h.
 */

void GFX_Initialize(void) {
    /* Place the App state machine in its initial state. */
    gfxData.state = GFX_STATE_INIT;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void GFX_Tasks ( void )

  Remarks:
    See prototype in gfx.h.
 */

void GFX_Tasks(void) {
    uint32_t xx1, yy1;

    /* Check the application's current state. */
    switch (gfxData.state) {
            /* Application's initial state. */
        case GFX_STATE_INIT:
        {
            bool appInitialized = true;
            gfx_mono_ssd1306_init();

            if (appInitialized) {

                gfxData.state = GFX_STATE_SERVICE_TASKS;
            }
            break;
        }

        case GFX_STATE_SERVICE_TASKS:
        {

            for (;;) {
                LED_3_Toggle();
                xx1 = 15;
                yy1 = 15;
                for (;;) {
                    for (xx1 = 14; xx1 < (128 - 14); xx1 += 1) {
                        gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_SET, GFX_WHOLE);
                        vTaskDelay(pdMS_TO_TICKS(5));
                        if (BUTTON_2_Get() == 0)break;
                        gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_CLR, GFX_WHOLE);
                    }
                    LED_2_Toggle();
                    if (BUTTON_2_Get() == 0)break;

                    for (xx1 = (128 - 15); xx1 > 14; xx1 -= 1) {
                        gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_SET, GFX_WHOLE);
                        vTaskDelay(pdMS_TO_TICKS(5));
                        if (BUTTON_2_Get() == 0)break;
                        gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_CLR, GFX_WHOLE);
                    }
                    LED_2_Toggle();
                    if (BUTTON_2_Get() == 0)break;
                }

                while (BUTTON_2_Get() == 0);
                gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
                LED_2_Set();

                for (;;) {
                    gfx_mono_draw_string("Rock and Roll\n"
                            "   ==> <==    ", 0, 0, &sysfont);

                    if (BUTTON_1_Get() == 0)break;
                    vTaskDelay(pdMS_TO_TICKS(300));
                    LED_1_Toggle();

                    gfx_mono_draw_string("  Welcome to \n"
                            "  23075 IoT6 ", 0, 0, &sysfont);

                    if (BUTTON_1_Get() == 0)break;
                    vTaskDelay(pdMS_TO_TICKS(300));
                    LED_1_Toggle();
                }

                gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

                LED_1_Set();

            }

            break;
        }

            /* TODO: implement your application state machine.*/


            /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
