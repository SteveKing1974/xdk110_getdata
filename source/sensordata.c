#include "SensorData.h"

#include <stdio.h>

#include "BCDS_Accelerometer.h"
#include "BCDS_Gyroscope.h"
#include "BCDS_SensorErrorType.h"
#include "XdkSensorHandle.h"

bool SensorData_Init(void)
{
	Retcode_T retVal = Accelerometer_init(xdkAccelerometers_BMI160_Handle);

	if (retVal != RETCODE_OK)
	{
		BCDS_SensorErrorType_T userCode = BCDS_getSensorErrorCode(retVal);
		printf("Failed to initialise acceleration %x %i\r\n", (uint32_t)retVal, userCode);
	}

	retVal = Gyroscope_init(xdkGyroscope_BMG160_Handle);

	if (retVal != RETCODE_OK)
	{
		BCDS_SensorErrorType_T userCode = BCDS_getSensorErrorCode(retVal);
		printf("Failed to initialise gyro %x %i\r\n", (uint32_t)retVal, userCode);
	}

	return (retVal == RETCODE_OK);
}

void SensorData_Read(SensorDataStruct* pData)
{
	Accelerometer_XyzData_T accData = {0};
	Gyroscope_XyzData_T gyroData = {0};

	const Retcode_T accRetVal = Accelerometer_readXyzGValue(xdkAccelerometers_BMI160_Handle, &accData);
	const Retcode_T gyroRetVal = Gyroscope_readXyzDegreeValue(xdkGyroscope_BMG160_Handle, &gyroData);

	if ((accRetVal == RETCODE_OK) && (gyroRetVal == RETCODE_OK))
	{
		pData->xAcc = accData.xAxisData;
		pData->yAcc = accData.yAxisData;
		pData->zAcc = accData.zAxisData;

		pData->xGyro = gyroData.xAxisData;
		pData->yGyro = gyroData.yAxisData;
		pData->zGyro = gyroData.zAxisData;

		printf("Acc: %ld %ld %ld %ld %ld %ld\r\n", accData.xAxisData, accData.yAxisData, accData.zAxisData, gyroData.xAxisData, gyroData.yAxisData, gyroData.zAxisData);
	}
	else
	{
		printf("Failed to read acc or gyro\r\n");
	}
}
