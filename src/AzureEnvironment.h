#pragma once
#include <string>
#include "Macros.h"
#include "NetworkRoutine.h"
using namespace std;
class AzureEnvironment
{
public:
    string wireServerAddress;
    AzureEnvironment();
    int DoDhcpWork();
    int CheckVersion();
    ~AzureEnvironment();
};

