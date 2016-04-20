#include "AgentConfig.h"
#include "Logger.h"
using namespace std;

void AgentConfig::LoadConfig()
{
    //linux code goes here
    if (this->config != NULL)
    {
        delete this->config;
        this->config = NULL;
    }
    this->config = new Config();
    if (this->task_config != NULL)
    {
        delete this->task_config;
        this->task_config = NULL;
    }
    this->task_config = new Config();
    // Read the file. If there is an error, report it and exit.
    try
    {
        this->config->readFile("/etc/waagent_native.conf");
        this->task_config->readFile("/etc/wala_task.conf");
    }
    catch (const FileIOException &fioex)
    {
        Logger::getInstance().Error("I/O error while reading file.");
    }
}

int AgentConfig::getTaskConfig(const char *propertyName, string &value)
{
    try
    {
        Setting &propertyValue = this->task_config->lookup(propertyName);
        value = propertyValue.c_str();
        return AGENT_SUCCESS;
    }
    catch (SettingNotFoundException e)
    {
        Logger::getInstance().Error("SettingNotFoundException");
        return AGENT_FAILED;
    }
}

int AgentConfig::getConfig(const char * propertyName, string &value)
{
    try
    {
        Setting &propertyValue = this->config->lookup(propertyName);
        value = propertyValue.c_str();
        return AGENT_SUCCESS;
    }
    catch (SettingNotFoundException e)
    {
        Logger::getInstance().Error("SettingNotFoundException");
        return AGENT_FAILED;
    }
}
