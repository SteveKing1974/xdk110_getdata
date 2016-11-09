#ifndef __SensorData
#define __SensorData

#include <stdint.h>
#include <stdbool.h>

#include "protocol.h"

bool SensorData_Init(void);

void SensorData_Read(SensorDataStruct* pData);



#endif
