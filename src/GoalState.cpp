#include "AzureEnvironment.h"
#include "FileOperator.h"
#include "GoalState.h"
#include "HostingEnvironmentConfig.h"
#include "HttpRoutine.h"
#include "Logger.h"
#include "StringUtil.h"
#include "XmlRoutine.h"
#include <map>
using namespace std;

GoalState::GoalState()
{
    goalStageFilePrefix = "/var/lib/waagent/Native_GoalState.";
}

void GoalState::UpdateGoalState(AzureEnvironment &azureEnvironment)
{
    string goalStateEndpoint = string("http://") + azureEnvironment.wireServerAddress + "/machine/?comp=goalstate";

    //TODO: wrapper up a XML handling in our code
    /* set our custom set of headers */
    HttpResponse goalStateResponse;
    int getGoalStateResult = HttpRoutine::GetWithDefaultHeader(goalStateEndpoint.c_str(), goalStateResponse);

    if (getGoalStateResult == AGENT_SUCCESS)
    {
        xmlDocPtr goalStateDoc = xmlParseMemory(goalStateResponse.body.c_str(), goalStateResponse.body.size());
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Incarnation[1]/text()", NULL, this->incarnation);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/ContainerId/text()", NULL, this->containerId);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/RoleInstanceList/RoleInstance/InstanceId/text()", NULL, this->roleInstanceId);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/RoleInstanceList/RoleInstance/Configuration/HostingEnvironmentConfig/text()", NULL, this->hostingEnvironmentConfigUrl);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/RoleInstanceList/RoleInstance/Configuration/SharedConfig/text()", NULL, this->sharedConfigUrl);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/RoleInstanceList/RoleInstance/Configuration/ExtensionsConfig/text()", NULL, this->extensionsConfigUrl);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/RoleInstanceList/RoleInstance/Configuration/FullConfig/text()", NULL, this->fullConfigUrl);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/RoleInstanceList/RoleInstance/Configuration/ConfigName/text()", NULL, this->configName);
        XmlRoutine::getNodeText(goalStateDoc, "/GoalState/Container/RoleInstanceList/RoleInstance/Configuration/Certificates/text()", NULL, this->certificatesUrl);
        xmlFreeDoc(goalStateDoc);
        // saving the goal state file 
        string goalStageFileName = this->goalStageFilePrefix + incarnation + ".xml";
        FileOperator::save_file(goalStateResponse.body, goalStageFileName);
        // construct the instances
        HttpResponse hostingEnvironmentConfigText;
        int getHostingEnvironmentConfigResult = HttpRoutine::GetWithDefaultHeader(this->hostingEnvironmentConfigUrl.c_str(), hostingEnvironmentConfigText);
        if (getHostingEnvironmentConfigResult == AGENT_SUCCESS)
        {
            this->hostingEnvironmentConfig = new HostingEnvironmentConfig();
            this->hostingEnvironmentConfig->Parse(hostingEnvironmentConfigText.body);
        }
        else
        {
            Logger::getInstance().Error("failed to get the host environment config");
            //TODO error handling.
        }

        HttpResponse sharedConfigText;
        int getSharedConfigResult = HttpRoutine::GetWithDefaultHeader(this->sharedConfigUrl.c_str(), sharedConfigText);
        if (getSharedConfigResult == AGENT_SUCCESS)
        {
            this->sharedConfig = new SharedConfig();
            this->sharedConfig->Parse(sharedConfigText.body);
        }
        else
        {
            Logger::getInstance().Error("failed to get the shared config");
            //TODO error handling.
        }

        HttpResponse extentionsConfigText;
        int getExtensionsConfigResult = HttpRoutine::GetWithDefaultHeader(this->extensionsConfigUrl.c_str(), extentionsConfigText);
        if (getExtensionsConfigResult == AGENT_SUCCESS)
        {
            this->extensionsConfig = new ExtensionsConfig();
            this->extensionsConfig->Parse(extentionsConfigText.body);
        }
        else
        {
            Logger::getInstance().Error("failed to get the extensions config");
            //TODO error handling.
        }

        // get the certificates from the server.
        if (this->certificatesUrl.length() > 0)
        {
            Logger::getInstance().Error("the certificates Url length is not 0");
            this->certificates = new Certificates();
            string certificationFileContent;
            int getCertificationFileContentResult = FileOperator::get_content(TRANSPORT_CERT_PUB, certificationFileContent);
            if (getCertificationFileContentResult == AGENT_SUCCESS)
            {
                vector<string> splitResult;
                string spliter = "\n";
                StringUtil::string_split(certificationFileContent, spliter, &splitResult);
                string pureCertText;
                for (int i = 0; i < splitResult.size(); i++)
                {
                    if (splitResult[i].find("CERTIFICATE") == string::npos)
                    {
                        pureCertText += splitResult[i];
                    }
                }
                Logger::getInstance().Verbose("pureCertText is %s", pureCertText.c_str());
                //TODO get rid of the cert headers.
                map<string, string> headers;
                headers["x-ms-agent-name"] = WAAGENT_NAME;
                headers["x-ms-version"] = WAAGENT_VERSION;
                headers["x-ms-cipher-name"] = TRANSPORT_CERT_CIPHER_NAME;
                headers["x-ms-guest-agent-public-x509-cert"] = pureCertText;
                HttpResponse certificationsText;
                int getResult = HttpRoutine::Get(this->certificatesUrl.c_str(), &headers, certificationsText);
                // get certificates from the remote using the public cert.
                if (getResult == AGENT_SUCCESS)
                {
                    this->certificates->Parse(certificationsText.body);
                }
            }
            else
            {
                Logger::getInstance().Error("get the transport cert pub failed.");
                // TODO error handling
            }
        }
        else
        {
            Logger::getInstance().Warning("certificates url is null.");
        }
    }
    else
    {
        Logger::getInstance().Error("failed to get the goal state");
        //TODO Error handling.
    }
}

void GoalState::Process()
{
    this->hostingEnvironmentConfig->Process();
    this->sharedConfig->Process();
    this->extensionsConfig->Process();
    this->certificates->Process();
}


GoalState::~GoalState()
{
    if (this->hostingEnvironmentConfig != NULL)
    {
        delete hostingEnvironmentConfig;
        hostingEnvironmentConfig = NULL;
    }
    if (this->sharedConfig != NULL)
    {
        delete sharedConfig;
        sharedConfig = NULL;
    }
    if (this->extensionsConfig != NULL)
    {
        delete extensionsConfig;
        extensionsConfig = NULL;
    }
    if (this->certificates != NULL)
    {
        delete certificates;
        certificates = NULL;
    }
}
