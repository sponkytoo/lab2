/*******************************************************************************
 * Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define APP_AT24MAC_DEVICE_MACADDR          (0x5f9A)
#define MAC_ADDR_LENGTH (6)
#define TCP_CLIENT_CONNECTION_TIMEOUT_PERIOD_ms 15000

typedef enum {
    MAC_ADDR_READ_STATE_READ,
    MAC_ADDR_READ_STATE_WAIT,
    MAC_ADDR_READ_STATE_SUCCESS,
    MAC_ADDR_READ_STATE_ERROR,
} AT24_MAC_ADDR_READ_STATE;

char macAddr[6];
char macAddrString[18];
extern TCPIP_NETWORK_CONFIG __attribute__((unused)) TCPIP_HOSTS_CONFIGURATION[];
static void AT24_MacAddr_Read(void);
SYS_MODULE_OBJ TCPIP_STACK_Init();

uint32_t MessageCounter = 1;

extern const char VM_Items[][20];
extern uint8_t VM_Count[7];

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
 */

APP_DATA appData;

APP_LED_STATE LEDstate = APP_LED_STATE_OFF;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary local functions.
 */

void AT24_MacAddr_Read_Callback(uintptr_t context) {
    AT24_MAC_ADDR_READ_STATE* transferState = (AT24_MAC_ADDR_READ_STATE*) context;

    if (TWIHS0_ErrorGet() == TWIHS_ERROR_NONE) {
        if (transferState) {
            *transferState = MAC_ADDR_READ_STATE_SUCCESS;
        }
    } else {
        if (transferState) {
            *transferState = MAC_ADDR_READ_STATE_ERROR;
        }
    }
}

static bool NETWORKCOMS_IsTickPeriodElapsed(uint32_t tickValue_ms, uint32_t periodToCheck_ms) {
    if ((SYS_TMR_TickCountGet() - tickValue_ms) >= periodToCheck_ms) {
        return true;
    } else {
        return false;
    }
}

static void AT24_MacAddr_Read(void) {
    static AT24_MAC_ADDR_READ_STATE state = MAC_ADDR_READ_STATE_READ;
    switch (state) {
        case MAC_ADDR_READ_STATE_READ:
            /* Register the TWIHS Callback with transfer status as context */
            TWIHS0_CallbackRegister(AT24_MacAddr_Read_Callback, (uintptr_t) & state);
            //Initiate Read AT24 MAC Address
            TWIHS0_Read(APP_AT24MAC_DEVICE_MACADDR, (uint8_t *) (macAddr), MAC_ADDR_LENGTH);
            state = MAC_ADDR_READ_STATE_WAIT;
            break;

        case MAC_ADDR_READ_STATE_WAIT:
            break;

        case MAC_ADDR_READ_STATE_SUCCESS:
            //convert MAC address to string format
            TCPIP_Helper_MACAddressToString((const TCPIP_MAC_ADDR*) macAddr, macAddrString, 18);
            //update host configuration with new MAC address
            (TCPIP_HOSTS_CONFIGURATION[0].macAddr) = (char*) macAddrString;
            SYS_CONSOLE_PRINT("MAC TCPIP_HOSTS_CONFIGURATION[0].macAddr: %s\n\r", TCPIP_HOSTS_CONFIGURATION[0].macAddr);
            appData.state = APP_TCPIP_INIT_TCPIP_STACK;
            break;

        case MAC_ADDR_READ_STATE_ERROR:
            // error; use default MAC address
            appData.state = APP_TCPIP_INIT_TCPIP_STACK;
            break;
    }

}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_TCPIP_WAIT_INIT;

    /* Place the application state machine in its initial state. */
    appData.state = APP_START_CASE;

    appData.new_ip = false;
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_Tasks(void) {
    SYS_STATUS tcpipStat;
    TCPIP_NET_HANDLE netH;
    int nNets;
    static IPV4_ADDR dwLastIP[2] = {
        {-1},
        {-1}
    };
    IPV4_ADDR ipAddr;
    int i;
   // const char *netName, *netBiosName;
    static uint32_t startTick = 0;
    static uint32_t blink = 2;
    static unsigned int count = 0;
    static unsigned int bay_index = 0;

    SYS_CMD_READY_TO_READ();

    switch (appData.state) {
        case APP_START_CASE:
            SYS_CONSOLE_PRINT("\n\r==========================================================\r\n");
            SYS_CONSOLE_PRINT("web_net_server_nvm_mpfs_freertos_lab1 %s %s\r\n", __DATE__, __TIME__);
            appData.state = APP_MOUNT_DISK;
            break;

        case APP_MOUNT_DISK:
            if (SYS_FS_Mount(SYS_FS_NVM_VOL, LOCAL_WEBSITE_PATH_FS, MPFS2, 0, NULL) == 0) {
                SYS_CONSOLE_PRINT("SYS_Initialize: The %s File System is mounted\r\n", SYS_FS_MPFS_STRING);
                appData.state = APP_TCPIP_INIT_MAC;
            }
            break;

        case APP_TCPIP_INIT_MAC:
            // Read MAC address 
            AT24_MacAddr_Read();
            break;
        case APP_TCPIP_INIT_TCPIP_STACK:
            // TCPIP Stack Initialization
            sysObj.tcpip = TCPIP_STACK_Init();
            SYS_ASSERT(sysObj.tcpip != SYS_MODULE_OBJ_INVALID, "TCPIP_STACK_Init Failed");
            appData.state = APP_TCPIP_WAIT_INIT;
            break;

        case APP_TCPIP_WAIT_INIT:
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);
            if (tcpipStat < 0) { // some error occurred
                SYS_CONSOLE_MESSAGE("APP: TCP/IP stack initialization failed!\r\n");
                appData.state = APP_TCPIP_ERROR;
            } else if (tcpipStat == SYS_STATUS_READY) {
                // now that the stack is ready we can check the
                // available interfaces and register
                // a Bonjour service

                nNets = TCPIP_STACK_NumberOfNetworksGet();

                for (i = 0; i < nNets; i++) {
                    netH = TCPIP_STACK_IndexToNet(i);
                    appData.netName = TCPIP_STACK_NetNameGet(netH);
                    appData.netBiosName = TCPIP_STACK_NetBIOSName(netH);

#if defined(TCPIP_STACK_USE_NBNS)
                    SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS enabled\r\n",appData.netName,appData.netBiosName);
#else
                    SYS_CONSOLE_PRINT("    Interface %s on host %s - NBNS disabled\r\n", netName, netBiosName);
#endif // defined(TCPIP_STACK_USE_NBNS)

#if defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
                    // base name of the service Must not exceed 16 bytes long
                    // the last digit will be incremented by interface
                    char mDNSServiceName[] = "MyWebServiceNameX ";

                    mDNSServiceName[sizeof (mDNSServiceName) - 2] = '1' + i;
                    TCPIP_MDNS_ServiceRegister(netH
                            , mDNSServiceName // name of the service
                            , "_http._tcp.local" // type of the service
                            , 80 // TCP or UDP port, at which this service is available
                            , ((const uint8_t *) "path=/index.htm") // TXT info
                            , 1 // auto rename the service when if needed
                            , NULL // no callback function
                            , NULL); // no application context
#endif // defined(TCPIP_STACK_USE_ZEROCONF_MDNS_SD)
                }

#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)
                // register the application HTTP processing
                HTTP_APP_Initialize();
#endif // defined(TCPIP_STACK_USE_HTTP_NET_SERVER)

                SYS_CONSOLE_PRINT("\r\n%s -sends message to the server when a Bay is empty\r\n\r\n",appData.netBiosName);
                appData.state = APP_TCPIP_TRANSACT;
            }

            break;

        case APP_TCPIP_TRANSACT:
            if (SYS_TMR_TickCountGet() - startTick >= SYS_TMR_TickCounterFrequencyGet() >> blink) {
                startTick = SYS_TMR_TickCountGet();
                LEDstate ^= APP_LED_STATE_ON;
                if (LEDstate == 1) {
                    APP_LED_1StateSet();
                } else if (LEDstate == 0) {
                    APP_LED_1StateClear();
                    count++;
                }
            }

            // if the IP address of an interface has changed
            // display the new value on the system console
            nNets = TCPIP_STACK_NumberOfNetworksGet();

            for (i = 0; i < nNets; i++) {
                netH = TCPIP_STACK_IndexToNet(i);
                ipAddr.Val = TCPIP_STACK_NetAddress(netH);
                if (dwLastIP[i].Val != ipAddr.Val) {
                    dwLastIP[i].Val = ipAddr.Val;
                    SYS_CONSOLE_PRINT("%s IP Address: %d.%d.%d.%d \r\n",
                            TCPIP_STACK_NetNameGet(netH),
                            ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                    if (ipAddr.v[0])blink = 0;

                    sprintf(appData.ip_address, "        %d.%d.%d.%d           ", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                    appData.new_ip = true;
                }
            }

            if (count > 0) {
                count = 0;
                if (bay_index >= 7) {
                    bay_index = 0;
                }
                if (VM_Count[bay_index] == '0') {
                    SYS_CONSOLE_PRINT("%s-Sending VM Status\r\n",appData.netBiosName);
                    appData.state = APP_SETUP_TCP_CLIENT;
                }
                bay_index++;

            }
            break;

        case APP_SETUP_TCP_CLIENT:
        {
            IPV4_ADDR addr;

            addr.v[0] = 10;
            addr.v[1] = 13;
            addr.v[2] = 33;
            addr.v[3] = 91;
            appData.port = 80;
            appData.socket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                    appData.port,
                    (IP_MULTI_ADDRESS*) & addr);

            if (appData.socket == INVALID_SOCKET) {
                SYS_CONSOLE_MESSAGE("Could not start connection\r\n");
                appData.state = APP_SETUP_TCP_CLIENT;
            }
            SYS_CONSOLE_MESSAGE("Starting connection\r\n");
            appData.timeoutTick = SYS_TMR_TickCountGet();
            appData.state = APP_WAIT_FOR_TCP_CONNECTION;
            break;
        }


        case APP_WAIT_FOR_TCP_CONNECTION:
            if (!TCPIP_TCP_IsConnected(appData.socket)) {
                if (NETWORKCOMS_IsTickPeriodElapsed(appData.timeoutTick, TCP_CLIENT_CONNECTION_TIMEOUT_PERIOD_ms)) {
                    SYS_CONSOLE_MESSAGE("Timeout: TCP Socket could not be established\r\n");
                    appData.state = APP_CLOSE_TCP_CLIENT;
                }
                break;
            }
            if (TCPIP_TCP_PutIsReady(appData.socket) == 0) {
                break;
            }

            SYS_CONSOLE_MESSAGE("TCP Socket Connected\r\n");
            appData.state = APP_MANAGE_TCP_CLIENT;
            break;


        case APP_MANAGE_TCP_CLIENT:
        {
            char buffer[256];
            buffer[0] = 0;
            //SYS_CONSOLE_PRINT("Sending message \"%s is empty\" \r\n", VM_Items[j - 1]);
            sprintf(buffer, "MSG:%d from %s : %s is empty", (int) MessageCounter++, (char *) TCPIP_HOSTS_CONFIGURATION[0].macAddr, (char *) VM_Items[bay_index - 1]);
            SYS_CONSOLE_PRINT("Sending message: %s\r\n", buffer);
            TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*) buffer, strlen(buffer));
            appData.state = APP_CLOSE_TCP_CLIENT;
            break;
        }

        case APP_CLOSE_TCP_CLIENT:
            /* TODO:[6] Close the TCP Socket
             *        
             */
            TCPIP_TCP_Close(appData.socket);
            appData.socket = INVALID_SOCKET;
            SYS_CONSOLE_MESSAGE("TCP Client Closed\r\n\r\n\r\n");
            appData.state = APP_TCPIP_TRANSACT;
            break;


        default:
            break;
    }
}

/*******************************************************************************
 End of File
 */
