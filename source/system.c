/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */
//lint -esym(956,*) /* Suppressing "Non const, non volatile static or external variable" lint warning*/

/* module includes ********************************************************** */

/* system header files */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* additional interface header files */
#include "BCDS_Assert.h"
#include "FreeRTOS.h"
#include "timers.h"

/* own header files */
#include "comms.h"
#include "sensordata.h"
#include "leddrv.h"
#include "buttondrv.h"

#include "protocol.h"

#define TIMERBLOCKTIME UINT32_MAX             /**< Macro used to define block time of a timer*/
#define ZERO    UINT8_C(0)  			            /**< Macro to define value zero*/
#define TIMER_AUTORELOAD_ON             UINT32_C(1)             /**< Auto reload of timer is enabled*/
#define TIMER_AUTORELOAD_OFF            UINT32_C(0)             /**< Auto reload of timer is disabled*/


xTimerHandle systemInitHandle = NULL;
xTimerHandle systemTickHandle = NULL;


static void systemInit(xTimerHandle xTimer);
static void systemTick(xTimerHandle xTimer);

static bool systemEnabled = false;

void buttonCallback(ButtonID ID, ButtonState state)
{
	if (ID == BUTTON_1 && state==BUTTON_PRESSED)
	{
		systemEnabled = true;
	}
	if (ID == BUTTON_2 && state==BUTTON_PRESSED)
	{
		systemEnabled = false;
	}
}

/**
 *  @brief
 *      Function to initialize the wifi network send application. Create timer task
 *      to start WiFi Connect and get IP function after one second. After that another timer
 *      to send data periodically.
 */
void appInitSystem(xTimerHandle xTimer)
{
    BCDS_UNUSED(xTimer);
    uint32_t Ticks = UINT32_C(50);

    if (Ticks != UINT32_MAX) /* Validated for portMAX_DELAY to assist the task to wait Infinitely (without timing out) */
    {
        Ticks /= portTICK_RATE_MS;
    }
    if (UINT32_C(0) == Ticks) /* ticks cannot be 0 in FreeRTOS timer. So ticks is assigned to 1 */
    {
        Ticks = UINT32_C(1);
    }
    /* create timer task*/
    systemInitHandle = xTimerCreate((char * const ) "systemInit", Ticks, TIMER_AUTORELOAD_OFF, NULL, systemInit);
    systemTickHandle = xTimerCreate((char * const ) "systemTick", Ticks, TIMER_AUTORELOAD_ON, NULL, systemTick);

    if ((systemInitHandle != NULL) && (systemTickHandle != NULL))
    {
        /*start the init timer*/
        if ( xTimerStart(systemInitHandle, TIMERBLOCKTIME) != pdTRUE)
        {
            assert(false);
        }
    }
    else
    {
        /* Assertion Reason: "Failed to create timer task during initialization"   */
        assert(false);
    }


}

void systemInit(xTimerHandle xTimer)
{
	BCDS_UNUSED(xTimer);

	bool initComplete = Comms_Init();
	initComplete = initComplete && SensorData_Init();
	initComplete = initComplete && ButtonDrv_Init();
	initComplete = initComplete && LedDrv_Init();

	//
	if (initComplete)
	{
		ButtonDrv_InstallCallback(buttonCallback);
		if (xTimerStart(systemTickHandle, TIMERBLOCKTIME) != pdTRUE)
		{
			assert(false);
		}
	}

}

void systemTick(xTimerHandle xTimer)
{
	BCDS_UNUSED(xTimer);

	static uint32_t remoteIP = 0;
	uint32_t tempIP = 0;
	char rawData[64] = {0};
	char commandData[64] = {0};
	uint16_t dataLength;

	uint16_t commandLength = Comms_RecvData(rawData, sizeof(rawData), &tempIP);
	const Commands cmd = Protocol_Decode(rawData, commandLength, commandData, &dataLength);

	if (cmd == CMD_START_SENDING)
	{
		remoteIP = tempIP;
	}
	else if (cmd==CMD_STOP_SENDING)
	{
		remoteIP = 0;
	}

	if (systemEnabled)
	{
		LedDrv_On(YELLOW_LED);
		LedDrv_Off(RED_LED);

		if (remoteIP != 0)
		{
			SensorDataStruct sensorDat;
			SensorData_Read(&sensorDat);

			Protocol_EncodeSensorData(&sensorDat, commandData, &dataLength);
			Protocol_Encode(CMD_SENSOR_DATA, commandData, dataLength, rawData, &commandLength);

			Comms_SendData(rawData, commandLength, remoteIP);
		}
	}
	else
	{
		LedDrv_Off(YELLOW_LED);
		LedDrv_On(RED_LED);
	}

}


/**
 *  @brief
 *      Function to periodically send data over WiFi as UDP packets. This is run as an Auto-reloading timer.
 *
 *  @param [in ] xTimer - necessary parameter for timer prototype
 */
//static void wifiSend(xTimerHandle xTimer)
//{
//    BCDS_UNUSED(xTimer);
//    if (STATUS_OK != bsdUdpClient(SERVER_PORT))
//    {
//        /* assertion Reason:  "Failed to  send udp packet" */
//        assert(false);
//    }
//}

/**
 *  @brief
 *      Function to connect to wifi network and obtain IP address
 *
 *  @param [in ] xTimer
 */
//static void wifiConnectGetIP(xTimerHandle xTimer)
//{
//    BCDS_UNUSED(xTimer);
//
//    NetworkConfig_IpSettings_T myIpSettings;
//    memset(&myIpSettings, (uint32_t) 0, sizeof(myIpSettings));
//    char ipAddress[PAL_IP_ADDRESS_SIZE] = { 0 };
//    Ip_Address_T* IpaddressHex = Ip_getMyIpAddr();
//    WlanConnect_SSID_T connectSSID;
//    WlanConnect_PassPhrase_T connectPassPhrase;
//    Retcode_T ReturnValue = (Retcode_T)RETCODE_FAILURE;
//    int32_t Result = INT32_C(-1);
//
//    SlSockAddrIn_t  LocalAddr;
//    _i16 AddrSize = sizeof(SlSockAddrIn_t);
//    SlSockNonblocking_t enableOption;
//
//
//    if (RETCODE_OK != WlanConnect_Init())
//    {
//        printf("Error occurred initializing WLAN \r\n ");
//        return;
//    }
//
//    printf("Connecting to %s \r\n ", WLAN_CONNECT_WPA_SSID);
//
//    connectSSID = (WlanConnect_SSID_T) WLAN_CONNECT_WPA_SSID;
//    connectPassPhrase = (WlanConnect_PassPhrase_T) WLAN_CONNECT_WPA_PASS;
//    ReturnValue = NetworkConfig_SetIpDhcp(NULL);
//    if (ReturnValue)
//    {
//        printf("Error in setting IP to DHCP\n\r");
//        return;
//    }
//
//    if (RETCODE_OK == WlanConnect_WPA(connectSSID, connectPassPhrase, NULL))
//    {
//        ReturnValue = NetworkConfig_GetIpSettings(&myIpSettings);
//        if (RETCODE_OK == ReturnValue)
//        {
//            *IpaddressHex = Basics_htonl(myIpSettings.ipV4);
//            Result = Ip_convertAddrToString(IpaddressHex, ipAddress);
//            if (Result < 0)
//            {
//                printf("Couldn't convert the IP address to string format \r\n ");
//                return;
//            }
//            printf("Connected to WPA network successfully. \r\n ");
//            printf(" Ip address of the device: %s \r\n ", ipAddress);
//        }
//        else
//        {
//            printf("Error in getting IP settings\n\r");
//            return;
//        }
//    }
//    else
//    {
//        printf("Error occurred connecting %s \r\n ", WLAN_CONNECT_WPA_SSID);
//        return;
//    }
//
//    g_RecvSocket = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, (uint32_t) ZERO);
//
//    LocalAddr.sin_family = SL_AF_INET;
//    LocalAddr.sin_port = sl_Htons(7777);
//    LocalAddr.sin_addr.s_addr = 0;
//
//    printf("Bind %i\r\n", sl_Bind(g_RecvSocket, (SlSockAddr_t *)&LocalAddr, AddrSize));
//
//    enableOption.NonblockingEnabled = 1;
//    sl_SetSockOpt(g_RecvSocket,SL_SOL_SOCKET,SL_SO_NONBLOCKING, (_u8 *)&enableOption,sizeof(enableOption)); // Enable/disable nonblocking mode
//
//
//    /* After connection start the wifi sending timer*/
//    if (xTimerStart(wifiSendTimerHandle, TIMERBLOCKTIME) != pdTRUE)
//    {
//        assert(false);
//    }
//}

/* global functions ********************************************************* */

/** ************************************************************************* */
