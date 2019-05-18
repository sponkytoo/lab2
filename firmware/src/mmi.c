/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    mmi.c

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
#include <string.h>
#include <stdio.h>
#include "mmi.h"
#include "app.h"
#include "third_party/rtos/FreeRTOS/Source/Include/FreeRTOS.h"
#include "third_party/rtos/FreeRTOS/Source/Include/timers.h"
#include "../gfx_mono/gfx_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
extern struct font sysfont;
extern APP_DATA appData;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the MMI_Initialize function.

    Application strings and buffers are be defined outside this structure.
 */

MMI_DATA mmiData;

char VM_Items[][20] = {"1.Coca-Cola", "2.Diet-Coke", "3.Pepsi", "4.Dr Pepper", "5.Sprite", "6.Fanta", "7.Dasani"};
extern uint8_t VM_Count[7];
uint8_t Previous_Value[7] = {'9', '9', '9', '9', '9', '9', '9'};

#define NUM_TIMERS 1
TimerHandle_t xTimers[ NUM_TIMERS ];

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
bool DisplayIP = false;

void vTimerCallback(TimerHandle_t xTimer) {
    DisplayIP = true;
}

/*******************************************************************************
  Function:
    void MMI_Initialize ( void )

  Remarks:
    See prototype in mmi.h.
 */

void MMI_Initialize(void) {
    /* Place the App state machine in its initial state. */
    mmiData.state = MMI_STATE_INIT;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */

    xTimers[ 0 ] = xTimerCreate(/* Just a text name, not used by the RTOS kernel. */
            "Timer",
            /* The timer period in ticks, must be greater than 0. */
            300,
            /* The timers will auto-reload themselves when they
            expire. */
            pdTRUE,
            /* The ID is used to store a count of the number of
            times the timer has expired, which is initialized to 0. */
            (void *) 0,
            /* Each timer calls the same callback when it expires. */
            vTimerCallback);

}

/******************************************************************************
  Function:
    void MMI_Tasks ( void )

  Remarks:
    See prototype in mmi.h.
 */

void MMI_Tasks(void) {
    uint32_t xx1 = 15, yy1 = 15;
    static unsigned int CurrentIndex = 0;
    char Oled_Text[20];

    /* Check the application's current state. */
    switch (mmiData.state) {
            /* Application's initial state. */
        case MMI_STATE_INIT:
        {
            bool appInitialized = true;

            gfx_mono_ssd1306_init();

            if (appInitialized) {

                mmiData.state = MMI_STATE_SERVICE_TASKS;
            }
            break;
        }

        case MMI_STATE_SERVICE_TASKS:
        {

            bool serviceInit = true;

            for (unsigned int i = 0; i < 5; i++) {
                for (xx1 = 14; xx1 < (128 - 14); xx1 += 2) {
                    gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_SET, GFX_WHOLE);
                    vTaskDelay(pdMS_TO_TICKS(5));
                    gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_CLR, GFX_WHOLE);
                }

                for (xx1 = (128 - 15); xx1 > 14; xx1 -= 2) {
                    gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_SET, GFX_WHOLE);
                    vTaskDelay(pdMS_TO_TICKS(5));
                    gfx_mono_generic_draw_circle(xx1, yy1, 14, GFX_PIXEL_CLR, GFX_WHOLE);
                }
            }

            /**********************Text at the start*****************************/
            gfx_mono_draw_string("Rock and Roll\n"
                    "   ==> <==    ", 0, 0, &sysfont);
            vTaskDelay(pdMS_TO_TICKS(3000));

            gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
            gfx_mono_draw_string("  Welcome to \n"
                    "  23075 IoT6 ", 0, 0, &sysfont);
            vTaskDelay(pdMS_TO_TICKS(3000));

            gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
            gfx_mono_draw_string("Vending \n"
                    "      Machine", 0, 0, &sysfont);
            vTaskDelay(pdMS_TO_TICKS(3000));

            gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
            gfx_mono_draw_string("B1-Up,B2-Down \n"
                    "   B3-Select Item  ", 0, 0, &sysfont);
            vTaskDelay(pdMS_TO_TICKS(5000));


            /*************Display VM ITEMs***************************************************/

            gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
            gfx_mono_draw_string(VM_Items[CurrentIndex], 0, 0, &sysfont);
            sprintf(Oled_Text, "%c", VM_Count[CurrentIndex]);
            gfx_mono_draw_string(Oled_Text, 117, 3, &sysfont);
            gfx_mono_generic_draw_rect(112, 0, 16, 20, GFX_PIXEL_SET);
            if (serviceInit) {
                mmiData.state = MMI_APP_TASK;
                xTimerStart(xTimers[ 0 ], 0);
            }
            break;
        }

        case MMI_APP_TASK:

        {
            /***************B2-Scroll down for next item*******/
            if (BUTTON_2_Get() == 0) {
                CurrentIndex = CurrentIndex + 1;
                if (CurrentIndex > 6) {
                    CurrentIndex = 0;
                }
                gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
                gfx_mono_draw_string(VM_Items[CurrentIndex], 0, 0, &sysfont);
                sprintf(Oled_Text, "%c", VM_Count[CurrentIndex]);
                gfx_mono_draw_string(Oled_Text, 117, 0, &sysfont);
                gfx_mono_generic_draw_rect(112, 0, 16, 20, GFX_PIXEL_SET);
                LED_2_Toggle();
                vTaskDelay(pdMS_TO_TICKS(500));
                LED_2_Toggle();
            }/*****************B1-Scroll up for previous item******************/
            else if (BUTTON_1_Get() == 0) {
                CurrentIndex = CurrentIndex - 1;
                if (CurrentIndex < 0 || CurrentIndex > 6) {
                    CurrentIndex = 6;
                }
                gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
                gfx_mono_draw_string(VM_Items[CurrentIndex], 0, 0, &sysfont);
                sprintf(Oled_Text, "%c", VM_Count[CurrentIndex]);
                gfx_mono_draw_string(Oled_Text, 117, 0, &sysfont);
                gfx_mono_generic_draw_rect(112, 0, 16, 20, GFX_PIXEL_SET);
                LED_1_Toggle();
                vTaskDelay(pdMS_TO_TICKS(500));
                LED_1_Toggle();

            }/********************Select an Item Decrement by 1**************/
            else if (BUTTON_3_Get() == 0) {
                if (VM_Count[CurrentIndex] <= '0') {
                    VM_Count[CurrentIndex] = '0';
                } else {
                    VM_Count[CurrentIndex] = VM_Count[CurrentIndex] - 1;
                }

                sprintf(Oled_Text, "%c", VM_Count[CurrentIndex]);
                gfx_mono_draw_string(Oled_Text, 117, 0, &sysfont);
                gfx_mono_generic_draw_rect(112, 0, 16, 20, GFX_PIXEL_SET);
                LED_3_Toggle();
                vTaskDelay(pdMS_TO_TICKS(500));
                LED_3_Toggle();
            }
        }

            if (DisplayIP) {
                static char pc[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                static uint8_t count = 0;
                memcpy(pc, &appData.ip_address[count], 7);
                if (++count == 22)count = 0;
                DisplayIP = false;
                gfx_mono_draw_string(pc, 0, 16, &sysfont);
            }

            break;

        case MMI_STATE_IDLE:
            break;

            /* The default state should never be executed. */
        default:
        {
            if (Previous_Value[CurrentIndex] != VM_Count[CurrentIndex]) {
                sprintf(Oled_Text, "%c", VM_Count[CurrentIndex]);
                gfx_mono_draw_string(Oled_Text, 117, 0, &sysfont);
                gfx_mono_generic_draw_rect(112, 0, 16, 20, GFX_PIXEL_SET);
            }
            /* TODO: Handle error in application's state machine. */
            Previous_Value[CurrentIndex] = VM_Count[CurrentIndex];
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
