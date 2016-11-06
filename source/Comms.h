#ifndef __Comms
#define __Comms

#include <stdint.h>
#include <stdbool.h>

bool Comms_Init(void);

uint16_t Comms_RecvData(char* pData, uint16_t maxLength, uint32_t* pFrom);

bool Comms_SendData(const char* pData, uint16_t length, uint32_t sendTo);


#endif
