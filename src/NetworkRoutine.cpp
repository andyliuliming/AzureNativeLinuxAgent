#include <stdio.h>
#include <stdlib.h>
#include "Logger.h"
#include "NetworkRoutine.h"
#include "SocketWrapper.h"
#ifndef _WIN32
#include <sys/param.h>
#endif
//#ifdef _WIN32
//#include <WinSock2.h>
#ifdef __FreeBSD__
#include <net/if_dl.h>
#else
#include <arpa/inet.h>
#include <curl/curl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h> // Needed for the socket functions
#include <sys/types.h>
#endif

//http://curl.haxx.se/libcurl/cplusplus/

NetworkRoutine::NetworkRoutine()
{
}

Interface * NetworkRoutine::getFirstActiveNonLoopInterface()
{
    Interface * iface = NULL;
#ifdef __FreeBSD__
    struct ifaddrs *ifap, *ifaptr;
    unsigned char *ptr;
    int getIfAddrsResult = getifaddrs(&ifap);
    if (getIfAddrsResult == 0)
    {
        for (ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next)
        {
            if (((ifaptr)->ifa_addr)->sa_family == AF_LINK)
            {
                ptr = (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
                //TODO if the ifa_name is null?
                if (!(ifaptr->ifa_name[0] == 'l' && ifaptr->ifa_name[1 == 'o']))
                {
                    iface = new Interface();
                    iface->name = ifaptr->ifa_name;
                    iface->mac_address[0] = *ptr;
                    iface->mac_address[1] = *(ptr + 1);
                    iface->mac_address[2] = *(ptr + 2);
                    iface->mac_address[3] = *(ptr + 3);
                    iface->mac_address[4] = *(ptr + 4);
                    iface->mac_address[5] = *(ptr + 5);
                    break;
                }
            }
        }
        freeifaddrs(ifap);
    }
    else
    {
        Logger::getInstance().Error("get if addrs failed with error: %d", getIfAddrsResult);
    }
#else
    char buf[8192] = { 0 };
    struct ifconf ifc = { 0 };
    struct ifreq *ifr = NULL;
    int sck = 0;
    int nInterfaces = 0;
    int i = 0;
    char ip[INET6_ADDRSTRLEN] = { 0 };
    struct ifreq *item;
    struct sockaddr *addr;

    /* Get a socket handle. */
    sck = socket(PF_INET, SOCK_DGRAM, 0);
    if (sck < 0)
    {
        Logger::getInstance().Error("sck < 0 ");
    }
    else
    {
        shared_ptr<SocketWrapper> sw(new SocketWrapper(sck));
        /* Query available interfaces. */
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        int ioctl_result = 0;
        ioctl_result = ioctl(sw.get()->s_, SIOCGIFCONF, &ifc);
        if (ioctl_result < 0)
        {
            Logger::getInstance().Error("ioctl(sw.get()->s_, SIOCGIFCONF, &ifc) is: %d", ioctl_result);
        }

        /* Iterate through the list of interfaces. */
        ifr = ifc.ifc_req;
        nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
        if (nInterfaces == 0)
        {
            Logger::getInstance().Error("network interfaces number is zero");
        }
        for (i = 0; i < nInterfaces; i++)
        {
            item = &ifr[i];
            addr = &(item->ifr_addr);
            /* Get the IP address*/
            ioctl_result = ioctl(sw.get()->s_, SIOCGIFADDR, item);
            if (ioctl_result < 0)
            {
                Logger::getInstance().Error("ioctl(OSIOCGIFADDR) error: %d", ioctl_result);
            }

            if (inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr), ip, sizeof ip) == NULL)
            {
                Logger::getInstance().Error("inet_ntop error.");
                continue;
            }

            /* Get the MAC address */
            ioctl_result = ioctl(sw.get()->s_, SIOCGIFHWADDR, item);
            if (ioctl_result < 0)
            {
                Logger::getInstance().Error("ioctl(SIOCGIFHWADDR) %d", ioctl_result);
            }

            if (!(item->ifr_name[0] == 'l'
                &&item->ifr_name[1] == 'o'))
            {
                iface = new Interface();
                iface->name = item->ifr_name;
                iface->mac_address[0] = item->ifr_hwaddr.sa_data[0];
                iface->mac_address[1] = item->ifr_hwaddr.sa_data[1];
                iface->mac_address[2] = item->ifr_hwaddr.sa_data[2];
                iface->mac_address[3] = item->ifr_hwaddr.sa_data[3];
                iface->mac_address[4] = item->ifr_hwaddr.sa_data[4];
                iface->mac_address[5] = item->ifr_hwaddr.sa_data[5];
                break;
            }
        }
    }
#endif
    return iface;
}

PDHCPRequest NetworkRoutine::BuildDHCPRequest()
{
    PDHCPRequest dhcpRequest = new DHCPRequest();
    memset(dhcpRequest, 0, sizeof(DHCPRequest));
    dhcpRequest->Opcode = 1;
    dhcpRequest->HardwareAddressType = 1;
    dhcpRequest->HardwareAddressLength = 6;
    dhcpRequest->Hops = 0;

    int lvalue = rand();
    memcpy(dhcpRequest->TransactionID, &lvalue, 4);

    memset(dhcpRequest->Seconds, 0, 2);
    Interface * iface = NetworkRoutine::getFirstActiveNonLoopInterface();
    PUINT8 macAddress = iface->mac_address;
    memcpy(dhcpRequest->ClientHardwareAddress, macAddress, 6);
    delete macAddress;

    //99, 130, 83, 99, 53, 1, 1, 255
    PUINT8 magicCookie = new UINT8[4];
    magicCookie[0] = 99;
    magicCookie[1] = 130;
    magicCookie[2] = 83;
    magicCookie[3] = 99;
    memcpy(dhcpRequest->MagicCookie, magicCookie, 4);
    delete magicCookie;
    dhcpRequest->MessageTypeCode = 53;
    dhcpRequest->MessageTypeLength = 1;
    dhcpRequest->MessageType = DHCPDISCOVER;
    dhcpRequest->End = 255;
    return dhcpRequest;
}

NetworkRoutine::~NetworkRoutine()
{
}
