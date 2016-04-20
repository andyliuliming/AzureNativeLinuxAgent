#include "FileOperator.h"
#include "Macros.h"
#include "SharedConfig.h"

SharedConfig::SharedConfig()
{
}

void SharedConfig::Parse(string &sharedConfigText)
{
    string sharedConfigFilePath = SHARED_CONFIG_FILE_NAME;
    FileOperator::save_file(sharedConfigText, sharedConfigFilePath);
}
void SharedConfig::Process()
{
}

SharedConfig::~SharedConfig()
{
}
