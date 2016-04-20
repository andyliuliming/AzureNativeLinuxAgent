#include "AbstractDistro.h"
#include "FileOperator.h"
#include "Logger.h"
#include "Macros.h"
#include "OvfEnv.h"
#include "StringUtil.h"
#include "UserManager.h"
#include "XmlRoutine.h"
#include <algorithm>
#include <map>
using namespace std;

OvfEnv::OvfEnv()
{
}

int OvfEnv::Parse(string &sharedConfigText)
{
    //TODO: remove the password before saving to disk
    FileOperator::save_file(sharedConfigText, string("/var/lib/waagent/Native_ovf-env.xml"));

    xmlDocPtr doc = xmlParseMemory(sharedConfigText.c_str(), sharedConfigText.size());

    map<string, string> namespaces;
    namespaces["oe"] = "http://schemas.dmtf.org/ovf/environment/1";
    namespaces["wa"] = "http://schemas.microsoft.com/windowsazure";
    string version;

    XmlRoutine::getNodeText(doc, "/oe:Environment/wa:ProvisioningSection/wa:Version/text()", &namespaces, version);
    vector<string> splitResult;
    string spliter = ".";
    StringUtil::string_split(version, spliter, &splitResult);
    bool newer = false;
    int major = atoi(splitResult[0].c_str());
    int minor = atoi(splitResult[1].c_str());
    if (major != this->majorVersion)
    {
        return OVF_VERSION_NOT_MATCH;
    }
    else
    {
        if (minor > this->minorVersion)
        {
            Logger::getInstance().Warning("Newer provisioning configuration detected. Please consider updating waagent.");
        }
    }
    //TODO: find the section with the specified version.(if have multiple ProvisioningSections).
    XmlRoutine::getNodeText(doc, "/oe:Environment/wa:ProvisioningSection/wa:LinuxProvisioningConfigurationSet/wa:HostName/text()", &namespaces, this->hostName);
    XmlRoutine::getNodeText(doc, "/oe:Environment/wa:ProvisioningSection/wa:LinuxProvisioningConfigurationSet/wa:UserName/text()", &namespaces, this->userName);
    XmlRoutine::getNodeText(doc, "/oe:Environment/wa:ProvisioningSection/wa:LinuxProvisioningConfigurationSet/wa:UserPassword/text()", &namespaces, this->passWord);
    string disableSshPasswordAuthenticationStr;
    XmlRoutine::getNodeText(doc, "/oe:Environment/wa:ProvisioningSection/wa:LinuxProvisioningConfigurationSet/wa:DisableSshPasswordAuthentication/text()", &namespaces, disableSshPasswordAuthenticationStr);

    std::transform(disableSshPasswordAuthenticationStr.begin(), disableSshPasswordAuthenticationStr.end(), disableSshPasswordAuthenticationStr.begin(), ::tolower);
    if (disableSshPasswordAuthenticationStr.compare("yes"))
    {
        this->disableSshPasswordAuthentication = true;
    }
    else
    {
        this->disableSshPasswordAuthentication = false;
    }

    XmlRoutine::getNodeText(doc, "/oe:Environment/wa:ProvisioningSection/wa:LinuxProvisioningConfigurationSet/wa:CustomData/text()", &namespaces, this->customData);

    xmlXPathObjectPtr publicKeys =
        XmlRoutine::getNodes(doc, "/oe:Environment/wa:ProvisioningSection/wa:LinuxProvisioningConfigurationSet/wa:SSH/wa:PublicKeys", &namespaces);
    
    if (publicKeys != NULL)
    {
        if (publicKeys->nodesetval != NULL)
        {
            for (int j = 0; j < publicKeys->nodesetval->nodeNr; j++)
            {
                xmlNodePtr publicKey = publicKeys->nodesetval->nodeTab[j];
                Logger::getInstance().Error("#################X1");
                xmlXPathObjectPtr fingerPrintObject = XmlRoutine::findNodeByRelativeXpath(doc, publicKey, "./wa:PublicKey/wa:Fingerprint/text()", &namespaces);
                Logger::getInstance().Error("#################X2");
                xmlXPathObjectPtr pathObject = XmlRoutine::findNodeByRelativeXpath(doc, publicKey, "./wa:PublicKey/wa:Path/text()", &namespaces);
                Logger::getInstance().Error("#################X3");
                string fingerPrint = (char *)fingerPrintObject->nodesetval->nodeTab[0]->content;
                Logger::getInstance().Error("#################X4");
                string path = (char*)pathObject->nodesetval->nodeTab[0]->content;
                Logger::getInstance().Error("#################X5");
                this->SshPublicKeys[fingerPrint] = path;
                Logger::getInstance().Error("#################X6");
            }
        }
        xmlXPathFreeObject(publicKeys);
    }
    else
    {
        Logger::getInstance().Warning("no public keys found.");
    }

    xmlXPathObjectPtr keyPairs =
        XmlRoutine::getNodes(doc, "/oe:Environment/wa:ProvisioningSection/wa:LinuxProvisioningConfigurationSet/wa:SSH/wa:KeyPairs", &namespaces);
    if (keyPairs != NULL)
    {
        if (keyPairs->nodesetval != NULL)
        {
            for (int i = 0; i < keyPairs->nodesetval->nodeNr; i++)
            {
                xmlNodePtr keyPair = publicKeys->nodesetval->nodeTab[i];
                xmlXPathObjectPtr fingerPrintObject = XmlRoutine::findNodeByRelativeXpath(doc, keyPair, "./wa:KeyPair/wa:Fingerprint/text()", &namespaces);
                xmlXPathObjectPtr pathObject = XmlRoutine::findNodeByRelativeXpath(doc, keyPair, "./wa:KeyPair/wa:Path/text()", &namespaces);
                string fingerPrint = (char *)fingerPrintObject->nodesetval->nodeTab[0]->content;
                string path = (char*)pathObject->nodesetval->nodeTab[0]->content;
                this->SshKeyPairs[fingerPrint] = path;
            }
        }
        xmlXPathFreeObject(keyPairs);
    }
    else
    {
        Logger::getInstance().Warning("no key pairs found.");
    }

    xmlFreeDoc(doc);
    return AGENT_SUCCESS;
}

int OvfEnv::Process()
{
    AbstractDistro::getInstance().setHostName(this->hostName);
    if (this->disableSshPasswordAuthentication)
    {
        AbstractDistro::getInstance().disableSshPasswordAuthentication();
    }
    //TODO handle if the passWord is empty.
    int createUserResult = UserManager::CreateUser(this->userName, this->passWord);

    string home = AbstractDistro::getInstance().getHome();

    for (map<string, string>::iterator it = this->SshPublicKeys.begin(); it != this->SshPublicKeys.end(); ++it)
    {
        string keyFilePath = it->first + ".crt";
        if (FileOperator::file_exists(keyFilePath.c_str()))
        {
            FileOperator::prepare_dir(it->second.c_str());
            CommandResult commandResult;
            string exportPubKey = "openssl x509 -in " + it->first + ".crt -noout -pubkey >" + it->first + ".pub";
            CommandExecuter::RunGetOutput(exportPubKey.c_str(), commandResult);
        }
    }
    for (map<string, string>::iterator it = this->SshKeyPairs.begin(); it != this->SshKeyPairs.end(); ++it)
    {
        string keyFilePath = it->first + ".prv";
        if (FileOperator::file_exists(keyFilePath.c_str()))
        {
            FileOperator::prepare_dir(it->second.c_str());
            CommandResult commandResult;
            //TODO normalize the path.
            string exportPubKey = "ssh-keygen -y -f " + it->first + ".prv > " + it->second + ".pub";
            CommandExecuter::RunGetOutput(exportPubKey.c_str(), commandResult);
        }
    }
    AbstractDistro::getInstance().restartSshService();
    return createUserResult;
}

OvfEnv::~OvfEnv()
{
}
