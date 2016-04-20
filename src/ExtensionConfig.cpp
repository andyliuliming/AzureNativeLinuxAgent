#include "CommandExecuter.h"
#include "ExtensionConfig.h"
#include "FileOperator.h"
#include "HandlerManifest.h"
#include "HttpRoutine.h"
#include "JsonRoutine.h"
#include "Logger.h"
#include "XmlRoutine.h"
#include "ZipRoutine.h"
#include <cstring>
using namespace std;

int ExtensionConfig::DownloadExtractExtensions(xmlDocPtr manifestXmlDoc, const char* pluginXpathManifestExpr)
{
    xmlXPathObjectPtr xpathManifestObj = XmlRoutine::getNodes(manifestXmlDoc, pluginXpathManifestExpr, NULL);

    xmlNodeSetPtr pluginManifestNodes = xpathManifestObj->nodesetval;
    for (int j = 0; j < pluginManifestNodes->nodeNr; j++)
    {
        xmlXPathObjectPtr versionObjects = XmlRoutine::findNodeByRelativeXpath(manifestXmlDoc, pluginManifestNodes->nodeTab[j], "./Version/text()", NULL);
        string currentVersion = (const char*)versionObjects->nodesetval->nodeTab[0]->content;
        xmlXPathFreeObject(versionObjects);
        if (currentVersion == this->version)
        {
            // downloading the bundles
            xmlXPathObjectPtr uriObjects = XmlRoutine::findNodeByRelativeXpath(manifestXmlDoc, pluginManifestNodes->nodeTab[j], "./Uris/Uri/text()", NULL);
            string bundleFilePath = string(WAAGENT_LIB_BASE_DIR) + "Native_" + this->name + "_" + this->version + ".zip";
            HttpRoutine::GetToFile((const char*)(uriObjects->nodesetval->nodeTab[0]->content), NULL, bundleFilePath.c_str());
            string extensionPath;
            this->get_extension_path(this->name, this->version, extensionPath);
            FileOperator::make_dir(extensionPath.c_str());
            ZipRoutine::UnZipToDirectory(bundleFilePath.c_str(), extensionPath.c_str());

            string changePermissionCommand = string("find ") + extensionPath + " -type f | xargs chmod  u+x ";
            CommandResult changePermissionResult;
            CommandExecuter::RunGetOutput(changePermissionCommand.c_str(), changePermissionResult);
            break;
        }
    }
    xmlXPathFreeObject(xpathManifestObj);
    
    return AGENT_SUCCESS;
}

int ExtensionConfig::PrepareExtensionPackage(string &incarnationStr)
{
    
    if (this->state.compare("uninstall") == 0
        || this->state.compare("disabled") == 0)
    {
        Logger::getInstance().Verbose("the extension is disabled/uninstalled");
    }

    Logger::getInstance().Verbose("download/extract extension %s", this->name.c_str());
    // get the manifest, get the bundle zip file location, download it, extract it.
    HttpResponse response;
    int getResult = HttpRoutine::Get(this->location.c_str(), NULL, response);
    if (getResult != AGENT_SUCCESS)
    {
        Logger::getInstance().Verbose("failed to get the manifest file from location, download from failover location.");
        getResult = HttpRoutine::Get(this->failoverLocation.c_str(), NULL, response);
    }
    if (getResult == AGENT_SUCCESS)
    {
        // download the extension bundle zip.
        string filepath = string(WAAGENT_LIB_BASE_DIR) + "Native_" + this->name + "." + incarnationStr + ".manifest";
        FileOperator::save_file(response.body, filepath);
        xmlDocPtr manifestXmlDoc = xmlParseMemory(response.body.c_str(), response.body.size());
        this->DownloadExtractExtensions(manifestXmlDoc, "/PluginVersionManifest/Plugins/Plugin");
        this->DownloadExtractExtensions(manifestXmlDoc, "/PluginVersionManifest/InternalPlugins/Plugin");

        xmlFreeDoc(manifestXmlDoc);
    }
    else
    {
        //TODO: error handling.
        Logger::getInstance().Error("failed to get the extensions manifest");
    }
    
    return AGENT_SUCCESS;
}
int ExtensionConfig::get_extension_path(string& pluginName, string& pluginVersion, string&extensionPath)
{
    extensionPath = string(WAAGENT_LIB_BASE_DIR) + "Native_" + pluginName + "_" + pluginVersion + "/";
    return AGENT_SUCCESS;
}

void ExtensionConfig::SavePluginSettings(string &seqNo, string &settings)
{
    string extensionPathOut;
    this->get_extension_path(this->name, this->version, extensionPathOut);

    string configFolderPath = extensionPathOut + "config/";
    FileOperator::make_dir(configFolderPath.c_str());
    string settingFilePath = configFolderPath + seqNo + ".settings";
    FileOperator::save_file(settings, settingFilePath);

    SaveHandlerEnvironemnt(seqNo, extensionPathOut);
}

int ExtensionConfig::Uninstall()
{
    //TODO implement this.
    return AGENT_FAILED;
}

int ExtensionConfig::Disable()
{
    //TODO implement this.
    return AGENT_FAILED;
}

int ExtensionConfig::Update(string &previous_version)
{
    //TODO implement this.
    return AGENT_FAILED;
}

int ExtensionConfig::SaveHandlerEnvironemnt(string &seqNo, string &extensionPathOut)
{
    //TODO implement this.
    string logDir = string(EXTENSION_LOG_BASE_DIR) + this->name + "/" + this->version;
    string configFolder = extensionPathOut + "config";
    string statusFolder = extensionPathOut + "status";
    string heartBeatFile = extensionPathOut + "heartbeat.log";
    // use the real json api to handle it.
    json_object * handleEnvArray = json_object_new_array();

    json_object * handleEnvObj = json_object_new_object();
    json_object_array_add(handleEnvArray, handleEnvObj);
    json_object_object_add(handleEnvObj, "name", json_object_new_string(this->name.c_str()));
    json_object_object_add(handleEnvObj, "seqNo", json_object_new_string(seqNo.c_str()));
    json_object_object_add(handleEnvObj, "version", json_object_new_double(1.0));
    json_object * handlerEnvironmentObj = json_object_new_object();
    json_object_object_add(handlerEnvironmentObj, "logFolder", json_object_new_string(logDir.c_str()));
    json_object_object_add(handlerEnvironmentObj, "configFolder", json_object_new_string(configFolder.c_str()));
    json_object_object_add(handlerEnvironmentObj, "heartbeatFile", json_object_new_string(heartBeatFile.c_str()));

    const char * handleEnvText = json_object_to_json_string(handleEnvArray);

    string handlelEnvironmentFilePath = extensionPathOut + "HandlerEnvironment.json";

    //TODO make sure the strings are all deallocated including the c_str() ones.
    FileOperator::save_file(handleEnvText, strlen(handleEnvText), handlelEnvironmentFilePath.c_str());
    //TODO make sure the handleEnvFormat is deallocated.
    return AGENT_SUCCESS;
}

int ExtensionConfig::Process()
{
    //handle it 
    Logger::getInstance().Verbose("start handling extension %s", this->name.c_str());
    string  extensionPath;
    this->get_extension_path(this->name, this->version, extensionPath);
    string manifestFilePath = extensionPath + "/HandlerManifest.json";

    HandlerManifest handlerManifest;
    int parseHandlerManifest = JsonRoutine::ParseHandlerManifest(manifestFilePath, handlerManifest);
    if (parseHandlerManifest == AGENT_SUCCESS)
    {
        CommandExecuter::PosixSpawn(handlerManifest.installCommand, extensionPath);
        CommandExecuter::PosixSpawn(handlerManifest.enableCommand, extensionPath);
    }
    else
    {
        //TODO error handling.
    }
    Logger::getInstance().Verbose("end handling extension");
    return AGENT_SUCCESS;
}


ExtensionConfig::ExtensionConfig()
{
}

ExtensionConfig::~ExtensionConfig()
{
}
