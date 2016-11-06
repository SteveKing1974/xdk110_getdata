#include "Comms.h"

/* system header files */
#include <stdio.h>
#include <stdint.h>

#include <socket.h>

/* additional interface header files */
//#include "simplelink.h"
#include "BCDS_Basics.h"
//#include "BCDS_Assert.h"
//#include "FreeRTOS.h"
//#include "timers.h"

#include "BCDS_WlanConnect.h"
#include "BCDS_NetworkConfig.h"
#include <Serval_Types.h>
#include <Serval_Basics.h>
#include <Serval_Ip.h>

/* own header files */
#include "protocol.h"

static int16_t g_RecvSocket = (int16_t) 0;

bool Comms_Init(void)
{
	const char* k_SSID = "GCHQ-Monitoring";
	const char* k_Password = "gizzygirl";

	Retcode_T ReturnValue;
    NetworkConfig_IpSettings_T myIpSettings;
    memset(&myIpSettings, (uint32_t) 0, sizeof(myIpSettings));
    char ipAddress[PAL_IP_ADDRESS_SIZE] = { 0 };

    Ip_Address_T* IpaddressHex = Ip_getMyIpAddr();
    WlanConnect_SSID_T connectSSID;
    WlanConnect_PassPhrase_T connectPassPhrase;

    int32_t Result = INT32_C(-1);

    SlSockAddrIn_t  LocalAddr;
    _i16 AddrSize = sizeof(SlSockAddrIn_t);
    SlSockNonblocking_t enableOption;


    if (RETCODE_OK != WlanConnect_Init())
    {
        printf("Error occurred initializing WLAN \r\n ");
        return false;
    }

    printf("Connecting to %s \r\n ", k_SSID);

    connectSSID = (WlanConnect_SSID_T) k_SSID;
    connectPassPhrase = (WlanConnect_PassPhrase_T) k_Password;
    ReturnValue = NetworkConfig_SetIpDhcp(NULL);
    if (ReturnValue)
    {
        printf("Error in setting IP to DHCP\n\r");
        return false;
    }

    if (RETCODE_OK == WlanConnect_WPA(connectSSID, connectPassPhrase, NULL))
    {
        ReturnValue = NetworkConfig_GetIpSettings(&myIpSettings);
        if (RETCODE_OK == ReturnValue)
        {
            *IpaddressHex = Basics_htonl(myIpSettings.ipV4);
            Result = Ip_convertAddrToString(IpaddressHex, ipAddress);
            if (Result < 0)
            {
                printf("Couldn't convert the IP address to string format \r\n ");
                return false;
            }
            printf("Connected to WPA network successfully. \r\n ");
            printf(" Ip address of the device: %s \r\n ", ipAddress);
        }
        else
        {
            printf("Error in getting IP settings\n\r");
            return false;
        }
    }
    else
    {
        printf("Error occurred connecting %s \r\n ", k_SSID);
        return false;
    }

    g_RecvSocket = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, (uint32_t) 0);

    LocalAddr.sin_family = SL_AF_INET;
    LocalAddr.sin_port = sl_Htons(DATA_PORT);
    LocalAddr.sin_addr.s_addr = 0;

    printf("Bind %i\r\n", sl_Bind(g_RecvSocket, (SlSockAddr_t *)&LocalAddr, AddrSize));

    enableOption.NonblockingEnabled = 1;
    sl_SetSockOpt(g_RecvSocket,SL_SOL_SOCKET,SL_SO_NONBLOCKING, (_u8 *)&enableOption,sizeof(enableOption)); // Enable/disable nonblocking mode

    return true;
}

uint16_t Comms_RecvData(char* pData, uint16_t maxLength, uint32_t* pFrom)
{
    SlSockAddrIn_t Addr;
    uint16_t AddrSize = (uint16_t) 0;
    int16_t Status = (int16_t) 0;

    AddrSize = sizeof(SlSockAddrIn_t);

	Status = sl_RecvFrom(g_RecvSocket, pData, maxLength, 0, (SlSockAddr_t *)&Addr, (SlSocklen_t*)&AddrSize);

    if (Status>0)
    {
        *pFrom = sl_Ntohl(Addr.sin_addr.s_addr);
        return (uint16_t)Status;
    }

    return 0;
}

bool Comms_SendData(const char *pData, uint16_t length, uint32_t sendTo)
{
    SlSockAddrIn_t Addr;
    uint16_t AddrSize = (uint16_t) 0;
    int16_t SockID = (int16_t) 0;
    int16_t Status = (int16_t) 0;

    AddrSize = sizeof(SlSockAddrIn_t);
    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons(DATA_PORT);
    Addr.sin_addr.s_addr = sl_Htonl(sendTo);

    SockID = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, (uint32_t) 0); /**<The return value is a positive number if successful; other wise negative*/
    if (SockID < (int16_t) 0)
    {
        /* error case*/
        return false;
    }

    Status = sl_SendTo(SockID, pData, length, (uint32_t) 0, (SlSockAddr_t *) &Addr, AddrSize);/**<The return value is a number of characters sent;negative if not successful*/
    /*Check if 0 transmitted bytes sent or error condition*/

    if (Status <= (int16_t) 0)
    {
        Status = sl_Close(SockID);
        return false;
    }
    Status = sl_Close(SockID);
    if (Status < 0)
    {
        return false;
    }

    return true;
}
