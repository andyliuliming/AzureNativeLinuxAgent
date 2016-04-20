#pragma once
#include <string>
using namespace std;
#include <libconfig.h++>
using namespace libconfig;
class AgentConfig
{
public:
    static AgentConfig& getInstance()
    {
        static AgentConfig    instance;
        return instance;
    }
    void LoadConfig();
    int getConfig(const char * propertyName, string &value);
    int getTaskConfig(const char * propertyName, string &value);
private:
    AgentConfig() {
    }
#ifdef _WIN32
#else
    Config *config;
    Config *task_config;
#endif
    AgentConfig(AgentConfig const&);              // Don't Implement.
    ~AgentConfig() {
    };
    void operator=(AgentConfig const&); // Don't implement
};

