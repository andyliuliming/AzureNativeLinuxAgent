#include <string>
#include "FileOperator.h"
#include "HostingEnvironmentConfig.h"
#include "Macros.h"
using namespace std;

HostingEnvironmentConfig::HostingEnvironmentConfig()
{
}

void HostingEnvironmentConfig::Parse(string &hostingEnvironmentConfigText)
{
    string hostingEnvironmentConfig = HOSTING_ENVIRONMENT_CONFIG;
    FileOperator::save_file(hostingEnvironmentConfigText, hostingEnvironmentConfig);
}

void HostingEnvironmentConfig::Process()
{
}

HostingEnvironmentConfig::~HostingEnvironmentConfig()
{
}
