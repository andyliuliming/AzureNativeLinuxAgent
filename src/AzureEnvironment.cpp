
#include <stdio.h>
#include "AbstractDistro.h"
#include "AzureEnvironment.h"
#include "CommandExecuter.h"
#include "Logger.h"
#include "Macros.h"
#include "NetworkRoutine.h"
#include "StringUtil.h"
#include "SocketWrapper.h"
#ifdef _WIN32
#include <WinSock2.h>
#include<ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else

#include <arpa/inet.h>
#include <curl/curl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h> // Needed for the socket functions
#include <string.h>
#endif
using namespace std;
AzureEnvironment::AzureEnvironment()
{
}

int AzureEnvironment::DoDhcpWork()
{
    //Open DHCP port if iptables is enabled.
    CommandResult commandResult;
    //TODO check the deallocation of commandResult
    CommandExecuter::RunGetOutput("iptables -D INPUT -p udp --dport 68 -j ACCEPT", commandResult);
    CommandExecuter::RunGetOutput("iptables -I INPUT -p udp --dport 68 -j ACCEPT", commandResult);

    PDHCPRequest dhcpRequest = NetworkRoutine::BuildDHCPRequest();

    // Configure the default routes.
    bool missingDefaultRoute = true;

    //TODO use the full path of the commands.
    #ifdef __FreeBSD__
    Interface *iface = NetworkRoutine::getFirstActiveNonLoopInterface();
    if (iface != NULL)
    {
        string command = "route add -net 255.255.255.255 -iface " + iface->name;
        CommandResult commandResult;
        CommandExecuter::RunGetOutput(command.c_str(), commandResult);
        delete iface;
        iface = NULL;
    }
    #else
    CommandExecuter::RunGetOutput("route -n", commandResult);
    vector<string> splitResult;
    string spliter = "\n";
    StringUtil::string_split((*(commandResult.output)), spliter, &splitResult);

    for (unsigned int i = 0; i < splitResult.size(); i++)
    {
        size_t lastZero = splitResult[i].find_last_of("0.0.0.0");
        size_t lastDefault = splitResult[i].find_last_of("default ");
        if (lastZero > 0 || lastDefault > 0)
        {
            missingDefaultRoute = false;
            break;
        }
    }

    if (missingDefaultRoute)
    {
        Logger::getInstance().Log("default route missing");
        Interface *iface = NetworkRoutine::getFirstActiveNonLoopInterface();
        if (iface != NULL)
        {
            string command = "route add 255.255.255.255 dev " + iface->name;
            CommandResult commandResult;
            CommandExecuter::RunGetOutput(command.c_str(), commandResult);
            delete iface;
            iface = NULL;
        }
    }
    #endif

    if (AbstractDistro::getInstance().isDhcpEnabled())
    {
        AbstractDistro::getInstance().stopDhcp();
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(DHCP_UDP_PORT);

    int sck = 0;
    /* Get a socket handle. */
    sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sck < 0)
    {
        Logger::getInstance().Error("sck < 0 ");
        return AGENT_FAILED;
    }
    else
    {
        shared_ptr<SocketWrapper> sw(new SocketWrapper(sck));
        int setsockopt_result = 0;
        int so_broadcast = 1;
        int so_reuseaddr = 1;
        setsockopt_result = setsockopt(sw.get()->s_, SOL_SOCKET, SO_BROADCAST, SOCKET_OPTION_P&so_broadcast, sizeof(so_broadcast));
        setsockopt_result = setsockopt(sw.get()->s_, SOL_SOCKET, SO_REUSEADDR, SOCKET_OPTION_P&so_reuseaddr, sizeof(so_reuseaddr));

        if (bind(sw.get()->s_, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            return AGENT_FAILED;
        }
        else
        {
            struct sockaddr_in addr_to;
            addr_to.sin_family = AF_INET;
            addr_to.sin_addr.s_addr = inet_addr("255.255.255.255");
            addr_to.sin_port = htons(DHCP_UDP_TO_PORT);
            long sendResult = sendto(sw.get()->s_, SEND_TO_OPTION dhcpRequest, sizeof(DHCPRequest), 0, 
                (struct sockaddr *)&addr_to, sizeof(addr_to));
            if (sendResult < 0)
            {
                Logger::getInstance().Error("send failed\n");
                return AGENT_FAILED;
            }
            else
            {
                Logger::getInstance().Verbose("send ok\n");
                struct timeval tv_out;
                tv_out.tv_sec = 6;
                tv_out.tv_usec = 0;
                setsockopt_result = setsockopt(sw.get()->s_, SOL_SOCKET, SO_RCVTIMEO, SOCKET_OPTION_P&tv_out, sizeof(tv_out));
                int bufferSize = 1024;
                PBYTE buffer = new BYTE[bufferSize];
                int addr_len = sizeof(struct sockaddr_in);
                int recvResult = recvfrom(sw.get()->s_, RECV_TO_OPTION buffer, bufferSize, 0, (struct sockaddr *)&addr, (socklen_t*)&addr_len);
                if (recvResult < 0)
                {
                    Logger::getInstance().Verbose("recv failed.");
                    return AGENT_FAILED;
                }
                else
                {
                    int i = 0xF0; //offset to first option
                    while (i < recvResult)
                    {
                        BYTE option = buffer[i];
                        long option_length = 0;
                        if ((i + 1) < recvResult)
                        {
                            option_length = buffer[i + 1];
                        }
                        if (option == 255)
                        {
                        }
                        if (option == 249)
                        {
                        }
                        if (option == 3)
                        {
                        }
                        if (option == 245)
                        {
                            char ip[INET6_ADDRSTRLEN];
                            sprintf(ip, "%d.%d.%d.%d", (int)(buffer[i + 2]), (int)(buffer[i + 3]), (int)(buffer[i + 4]), (int)(buffer[i + 5]));
                            wireServerAddress = ip;
                            break;
                        }
                        i += (option_length + 2);
                    }
                }
            }
        }
    }
    return AGENT_SUCCESS;
}

int AzureEnvironment::CheckVersion()
{
    return AGENT_SUCCESS;
}

AzureEnvironment::~AzureEnvironment()
{
}
