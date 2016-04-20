#pragma once
#include <string>
using namespace std;
class HostingEnvironmentConfig
{
public:
    HostingEnvironmentConfig();
    void Parse(string & hostingEnvironmentConfigText);
    void Process();

    ~HostingEnvironmentConfig();
};

