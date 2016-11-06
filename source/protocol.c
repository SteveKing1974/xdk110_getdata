#include "protocol.h"

#include <stdio.h>
#include <string.h>

#ifdef INET_FOR_HTONL
#include <arpa/inet.h>
#else
#include <socket.h>
#endif

typedef union SensorEncode {
	SensorDataStruct rawData;
	char byteData[sizeof(SensorDataStruct)];
} SensorEncode;

Commands Protocol_Decode(const char*pData, uint16_t length, char* pCommandData, uint16_t* pDataLength)
{
	Commands retVal = CMD_INVALID;
	if (length>=3)
	{
		if (pData[0]== CMD_STX && pData[length-1]==CMD_ETX)
		{
			retVal = pData[1];

			*pDataLength = length - 3;

			if (*pDataLength>0)
			{
				memcpy(pCommandData, &pData[2], *pDataLength);
			}
		}
	}

	return retVal;
}

void Protocol_Encode(Commands cmdVal, const char* pCommandData, uint16_t commandDataLength, char* pCommand, uint16_t* pCommandLength)
{
	*pCommand++ = CMD_STX;
	*pCommand++ = cmdVal;

	if (commandDataLength>0)
	{
		memcpy(pCommand, pCommandData, commandDataLength);
		pCommand+=commandDataLength;
	}

	*pCommand = CMD_ETX;
	*pCommandLength = commandDataLength + 3;
}

void Protocol_EncodeSensorData(const SensorDataStruct* pSensor, char* pCommandData, uint16_t* pCommandLength)
{
	SensorEncode encodedData;
	encodedData.rawData.xAcc = htonl(pSensor->xAcc);
	encodedData.rawData.yAcc = htonl(pSensor->yAcc);
	encodedData.rawData.zAcc = htonl(pSensor->zAcc);
	encodedData.rawData.xGyro = htonl(pSensor->xGyro);
	encodedData.rawData.yGyro = htonl(pSensor->yGyro);
	encodedData.rawData.zGyro = htonl(pSensor->zGyro);

	memcpy(pCommandData, encodedData.byteData, sizeof(SensorDataStruct));
	*pCommandLength = sizeof(SensorDataStruct);
}

void Protocol_DecodeSensorData(const char* pCommandData, SensorDataStruct* pSensor)
{
	SensorEncode encodedData;

	memcpy(encodedData.byteData, pCommandData, sizeof(SensorDataStruct));

	pSensor->xAcc = ntohl(encodedData.rawData.xAcc);
	pSensor->yAcc = ntohl(encodedData.rawData.yAcc);
	pSensor->zAcc = ntohl(encodedData.rawData.zAcc);
	pSensor->xGyro = ntohl(encodedData.rawData.xGyro);
	pSensor->yGyro = ntohl(encodedData.rawData.yGyro);
	pSensor->zGyro = ntohl(encodedData.rawData.zGyro);
}
