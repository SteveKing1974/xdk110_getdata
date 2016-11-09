#include "leddrv.h"

#include <led.h>

#include <stddef.h>

#include <XdkBoardHandle.h>

/* own header files */


static LED_handle_tp redLedHandle = (LED_handle_tp) NULL; /**< variable to store red led handle */
static LED_handle_tp yellowLedHandle = (LED_handle_tp) NULL; /**< variable to store yellow led handle */
static LED_handle_tp orangeLedHandle = (LED_handle_tp) NULL; /**< variable to store orange led handle */


bool LedDrv_Init(void)
{
    bool retVal = false;

    redLedHandle = LED_create(gpioRedLed_Handle, GPIO_STATE_OFF);
    if (redLedHandle != NULL)
    {
        yellowLedHandle = LED_create(gpioYellowLed_Handle, GPIO_STATE_OFF);
    }
    if (yellowLedHandle != NULL)
    {
        orangeLedHandle = LED_create(gpioOrangeLed_Handle, GPIO_STATE_OFF);
    }
    if (orangeLedHandle != NULL)
    {
        retVal = true;
    }
    return retVal;
}

void LedDrv_On(LED_ID id)
{
    switch (id)
    {
    case RED_LED:
        LED_setState(redLedHandle, LED_SET_ON);
        break;
    case ORANGE_LED:
        LED_setState(orangeLedHandle, LED_SET_ON);
        break;
    case YELLOW_LED:
        LED_setState(yellowLedHandle, LED_SET_ON);
        break;
    }
}

void LedDrv_Off(LED_ID id)
{
    switch (id)
    {
    case RED_LED:
        LED_setState(redLedHandle, LED_SET_OFF);
        break;
    case ORANGE_LED:
        LED_setState(orangeLedHandle, LED_SET_OFF);
        break;
    case YELLOW_LED:
        LED_setState(yellowLedHandle, LED_SET_OFF);
        break;
    }
}
