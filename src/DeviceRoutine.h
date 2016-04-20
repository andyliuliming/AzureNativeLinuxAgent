#pragma once
#include "CommandExecuter.h"
#ifndef _WIN32
#include <sys/param.h>
#endif

#ifdef _WIN32
#elif defined __FreeBSD__
#else
#include <libudev.h>
#endif
class DeviceRoutine
{
public:
    DeviceRoutine();
    static void setIsciTimeOut();
    static string* findRomDevice();
    static void setBlockDeviceTimeOut(string &timeOutFile, string &timeOut);
    ~DeviceRoutine();
};


