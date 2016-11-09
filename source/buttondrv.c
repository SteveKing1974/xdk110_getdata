
#include "buttondrv.h"

#include <button.h>

static BUTTON_handle_tp Button1Handle = (BUTTON_handle_tp) NULL;
static BUTTON_handle_tp Button2Handle = (BUTTON_handle_tp) NULL;

static ButtonFunction g_Button1_Func = NULL;
static ButtonFunction g_Button2_Func = NULL;

static void callback(void *handle, uint32_t userParameter)
{
    ButtonState state = BUTTON_PRESSED;
    ButtonID id = BUTTON_1;
    ButtonFunction pFunc = NULL;

    if (handle == Button1Handle)
    {
        pFunc = g_Button1_Func;
        id = BUTTON_1;
    }
    else if (handle == Button2Handle)
    {
        pFunc = g_Button2_Func;
        id = BUTTON_2;
    }

    if (pFunc!=NULL)
    {
        if (BUTTON_isPressed(handle))
        {
            state = BUTTON_PRESSED;
        }
        if (BUTTON_isReleased(handle))
        {
            state = BUTTON_RELEASED;
        }

        pFunc(userParameter, state);
    }
}

static bool createButton(BUTTON_handle_tp* pHandle, int gpioHandle)
{
    BUTTON_errorTypes_t buttonReturn = BUTTON_ERROR_INVALID_PARAMETER;

    *pHandle = BUTTON_create(gpioHandle, GPIO_STATE_OFF);

    if (*pHandle != NULL)
    {
        buttonReturn = BUTTON_enable(*pHandle);

        if (buttonReturn == BUTTON_ERROR_OK)
        {
            buttonReturn = BUTTON_setCallback(Button1Handle, callback, NULL);
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

void ButtonDrv_InstallCallback(ButtonID id, ButtonFunction func)
{
    if (id == BUTTON_1)
    {
        g_Button1_Func = func;
    }
    else if (id == BUTTON_2)
    {
        g_Button2_Func = func;
    }
}
