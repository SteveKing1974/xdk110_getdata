
#include "buttondrv.h"

#include <button.h>
#include <XdkBoardHandle.h>
#include <stddef.h>
#include <BCDS_Basics.h>

static BUTTON_handle_tp Button1Handle = (BUTTON_handle_tp) NULL;
static BUTTON_handle_tp Button2Handle = (BUTTON_handle_tp) NULL;

static ButtonFunction g_Button_Func = NULL;

static void callback(void *handle, uint32_t userParameter)
{
	BCDS_UNUSED(userParameter);

    if ((handle == Button1Handle) || (handle == Button2Handle))
    {
        ButtonState state = BUTTON_PRESSED;

        if (BUTTON_isPressed(handle))
        {
            state = BUTTON_PRESSED;
        }
        if (BUTTON_isReleased(handle))
        {
            state = BUTTON_RELEASED;
        }

        g_Button_Func((handle == Button1Handle) ? BUTTON_1 : BUTTON_2, state);
    }
}

static bool createButton(BUTTON_handle_tp* pHandle, GPIO_handle_tp gpioHandle)
{
    BUTTON_errorTypes_t buttonReturn = BUTTON_ERROR_INVALID_PARAMETER;

    *pHandle = BUTTON_create(gpioHandle, GPIO_STATE_OFF);

    if (*pHandle != NULL)
    {
        buttonReturn = BUTTON_enable(*pHandle);

        if (buttonReturn == BUTTON_ERROR_OK)
        {
            buttonReturn = BUTTON_setCallback(*pHandle, callback, 0);
        }
    }

    return (buttonReturn == BUTTON_ERROR_OK);
}

bool ButtonDrv_Init()
{
    bool initOk = createButton(&Button1Handle, gpioButton1_Handle);
    initOk = initOk && createButton(&Button2Handle, gpioButton2_Handle);

    return initOk;
}

void ButtonDrv_InstallCallback(ButtonFunction func)
{
    g_Button_Func = func;
}
