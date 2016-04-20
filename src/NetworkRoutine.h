#pragma once
#include <string>
#include <string.h>
#include "Macros.h"

using namespace std;
typedef struct _DHCPRequest {
    UINT8   Opcode;                     /* op:     BOOTREQUEST or BOOTREPLY */
    UINT8   HardwareAddressType;        /* htype:  ethernet */
    UINT8   HardwareAddressLength;      /* hlen:   6 (48 bit mac address) */
    UINT8   Hops;                       /* hops:   0 */
    UINT8   TransactionID[4];           /* xid:    random */
    UINT8   Seconds[2];                 /* secs:   0 */
    UINT8   Flags[2];                   /* flags:  0 or 0x8000 for broadcast */
    UINT8   ClientIpAddress[4];         /* ciaddr: 0 */
    UINT8   YourIpAddress[4];           /* yiaddr: 0 */
    UINT8   ServerIpAddress[4];         /* siaddr: 0 */
    UINT8   RelayAgentIpAddress[4];     /* giaddr: 0 */
    UINT8   ClientHardwareAddress[16];  /* chaddr: 6 byte ethernet MAC address */
    UINT8   ServerName[64];             /* sname:  0 */
    UINT8   BootFileName[128];          /* file:   0  */
    UINT8   MagicCookie[4];             /*   99  130   83   99 */
                                    /* 0x63 0x82 0x53 0x63 */
/* options -- hard code ours */

    UINT8 MessageTypeCode;              /* 53 */
    UINT8 MessageTypeLength;            /* 1 */
    UINT8 MessageType;                  /* 1 for DISCOVER */
    UINT8 End;                          /* 255 */
} DHCPRequest, *PDHCPRequest;

class Interface
{
public:
    Interface()
    {
        this->mac_address = new UINT8[6];
        memset(this->mac_address, 0, 6);
    }
    string name;
    PUINT8 mac_address;
    ~Interface()
    {
        if (mac_address != NULL)
        {
            delete mac_address;
            mac_address = NULL;
        }
    }
};

class NetworkRoutine
{
public:
    NetworkRoutine();
    static Interface* getFirstActiveNonLoopInterface();
    static PDHCPRequest BuildDHCPRequest();
    ~NetworkRoutine();
};

