#ifndef __Protocol
#define __Protocol

#include <stdint.h>
#include <stdbool.h>

#define DATA_PORT	7777

#if __cplusplus
extern "C" {
#endif

typedef enum Commands {
	CMD_INVALID = 0x0,

	CMD_STX = 0x10,
	CMD_ETX,
	CMD_START_SENDING,
	CMD_STOP_SENDING,
	CMD_SENSOR_DATA,

} Commands;

typedef struct {
	int32_t xAcc;
	int32_t yAcc;
	int32_t zAcc;

	int32_t xGyro;
	int32_t yGyro;
	int32_t zGyro;
} SensorDataStruct;

Commands Protocol_Decode(const char*pData, uint16_t length, char* commandData, uint16_t* pDataLength);
void Protocol_Encode(Commands cmdVal, const char* pCommandData, uint16_t commandDataLength, char* pCommand, uint16_t* pCommandLength);
void Protocol_EncodeSensorData(const SensorDataStruct* pSensor, char* pCommandData, uint16_t* pCommandLength);
void Protocol_DecodeSensorData(const char* pCommandData, SensorDataStruct* pSensor);

#if __cplusplus
}
#endif

#endif
