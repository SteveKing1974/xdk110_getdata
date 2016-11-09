#ifndef __ButtonDrv
#define __ButtonDrv

#include <stdint.h>
#include <stdbool.h>

typedef enum ButtonID {
    BUTTON_1,
    BUTTON_2
} ButtonID;

typedef enum ButtonState {
    BUTTON_PRESSED,
    BUTTON_RELEASED
} ButtonState;

typedef void (*ButtonFunction)(ButtonID ID, ButtonState state);

bool ButtonDrv_Init(void);

void ButtonDrv_InstallCallback(ButtonID id, ButtonFunction func);


#endif
