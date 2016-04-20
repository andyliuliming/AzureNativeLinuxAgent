#pragma once
#include <string>
#include "XmlRoutine.h"
using namespace std;
class ExtensionConfig
{
private:
    int DownloadExtractExtensions(xmlDocPtr manifestXmlDoc, const char* pluginXpathManifestExpr);
    int get_extension_path(string& pluginName, string& pluginVersion, string&extensionPath);
public:
    string autoUpgrade;
    string failoverLocation;
    string location;
    string name;
    string runAsStartupTask;
    string isJson;
    string state;
    string version;
    string RuntimeSettings;

    int PrepareExtensionPackage(string &incarnationStr);
    int Process();
    int Uninstall();
    int Disable();
    int Update(string &previous_version);
    int SaveHandlerEnvironemnt(string &seqNo, string &extensionPathOut);
    void SavePluginSettings(string &seqNo,string &settings);
    ExtensionConfig();
    ~ExtensionConfig();
};

