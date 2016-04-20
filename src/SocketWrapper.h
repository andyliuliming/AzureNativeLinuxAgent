#pragma once
#ifdef _WIN32
#include <WinSock2.h>
#include<ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/socket.h> // Needed for the socket functions
#include <curl/curl.h>
#include <string.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#include "Macros.h"
class SocketWrapper
{
    SocketWrapper(SocketWrapper const &);
    SocketWrapper & operator=(SocketWrapper const &);
public:
    int const s_;
    explicit SocketWrapper(int s) : s_(s) { }
    ~SocketWrapper()
    {
        (void)shutdown(s_, SHUT_RDWR);
        (void)CLOSESOCKET(s_);
    }
};

