#pragma once
#include <string>
#include <vector>
#include "ExtensionConfig.h"
#include "XmlRoutine.h"
using namespace std;
class ExtensionsConfig
{
private:
    vector<ExtensionConfig*> extensionConfigs;
    string statusUploadBlobUri;
    string statusBlobType;
    string incarnationStr;
    int GenerateAggStatus(ExtensionConfig &extensionConfig,string &aggStatus);
public:
    ExtensionsConfig();
    void ReportExtensionsStatus();
    void Parse(string &extensionsConfigText);
    void Process();
    ~ExtensionsConfig();
};

