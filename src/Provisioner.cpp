#include "AgentConfig.h"
#include "CommandExecuter.h"
#include "DeviceRoutine.h"
#include "FileOperator.h"
#include "Logger.h"
#include "Macros.h"
#include "OvfEnv.h"
#include "Provisioner.h"
#include "XmlRoutine.h"

Provisioner::Provisioner()
{
}

bool Provisioner::isProvisioned()
{
    return FileOperator::file_exists(PROVISIONED_FILE_PATH);
}

int Provisioner::Prosess()
{
    //1. provision re-generate key
    AgentConfig::getInstance().LoadConfig();

    string type;
    int getKeyPairTypeResult = AgentConfig::getInstance().getConfig("Provisioning_SshHostKeyPairType", type);
    if (getKeyPairTypeResult != 0)
    {
        type = string("rsa");
    }
    string regenerateKeys;
    int getRegenerateKeysResult = AgentConfig::getInstance().getConfig("Provisioning_RegenerateSshHostKeyPair", regenerateKeys);
    if (getRegenerateKeysResult != 0 || regenerateKeys.find("y") == 0)
    {
        CommandResult removeCommandResult;
        CommandExecuter::RunGetOutput("rm -f /etc/ssh/ssh_host_*key*", removeCommandResult);
        CommandResult generateCommandResult;
        //TODO deallocate the c_str() here.
        CommandExecuter::RunGetOutput(("ssh-keygen -N '' -t " + type + " -f /etc/ssh/ssh_host_" + type + "_key").c_str(), generateCommandResult);
    }

    // 2. do the ovf-env
    string ovfFileTestHooking;
    int hookResult = AgentConfig::getInstance().getConfig("OvfFileTestHooking", ovfFileTestHooking);

    string ovfFileContent;
    int getOvfFileContentResult = AGENT_FAILED;
    if (hookResult != AGENT_SUCCESS)
    {
        string *romDevicePath = DeviceRoutine::findRomDevice();
        if (romDevicePath == NULL)
        {
            Logger::getInstance().Warning("could not find the rom device.");
            return AGENT_FAILED;
        }

        FileOperator::make_dir(SECURE_MOUNT_POINT);
#ifdef __FreeBSD__
        string mountCommand("mount -t udf /dev/" + *romDevicePath + " " + SECURE_MOUNT_POINT);
#else
        string mountCommand("mount " + *romDevicePath + " " + SECURE_MOUNT_POINT);
#endif
        delete romDevicePath;
        romDevicePath = NULL;
        CommandResult mountResult;
        CommandExecuter::RunGetOutput(mountCommand, mountResult);
        if (mountResult.exitCode == 0)
        {
            Logger::getInstance().Warning("the cd is mounted");
            string ovfEnvFullPath = OVF_ENV_FILE_FULL_PATH;
            getOvfFileContentResult = FileOperator::get_content(OVF_ENV_FILE_FULL_PATH, ovfFileContent);
            string umountCommand = string("umount ") + SECURE_MOUNT_POINT;
            CommandResult umountResult;
            CommandExecuter::RunGetOutput(umountCommand, umountResult);
            if (umountResult.exitCode != 0)
            {
                Logger::getInstance().Error("unmount result: %s", mountResult.output->c_str());
            }
        }
        else
        {
            Logger::getInstance().Error("mount result: %s", mountResult.output->c_str());
            return AGENT_FAILED;
        }
    }
    else
    {
        getOvfFileContentResult = FileOperator::get_content(ovfFileTestHooking.c_str(), ovfFileContent);
    }
    if (getOvfFileContentResult == AGENT_SUCCESS)
    {
        OvfEnv *ovfEnv = new OvfEnv();
        ovfEnv->Parse(ovfFileContent);
        int ovfResult = ovfEnv->Process();
        if (ovfResult == 0)
        {
            return AGENT_SUCCESS;
        }
        else
        {
            Logger::getInstance().Error("the ovf process failed.");
            return AGENT_FAILED;
        }
    }
    else
    {
        //TODO error handling.
        Logger::getInstance().Error("get OvfFile Content failed.");
        return AGENT_FAILED;
    }
    // 2. This is done here because regenerated SSH host key pairs may be potentially overwritten when processing the ovfxml

    /* fingerprint = RunGetOutput("ssh-keygen -lf /etc/ssh/ssh_host_" + type + "_key.pub")[1].rstrip().split()[1].replace(':', '')
    self.ReportRoleProperties(fingerprint)*/

}

void Provisioner::markProvisioned()
{
    FileOperator::save_file("", 0, PROVISIONED_FILE_PATH);
}

Provisioner::~Provisioner()
{
}
