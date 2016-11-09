#ifndef __LedDrv
#define __LedDrv

#include <stdint.h>
#include <stdbool.h>

typedef enum LED_ID {
    RED_LED,
    ORANGE_LED,
    YELLOW_LED
} LED_ID;


bool LedDrv_Init(void);

void LedDrv_On(LED_ID id);
void LedDrv_Off(LED_ID id);


#endif
