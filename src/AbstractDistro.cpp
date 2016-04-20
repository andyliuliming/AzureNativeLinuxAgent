#include "AbstractDistro.h"
#include "CommandExecuter.h"
#include "FileOperator.h"
#include "Logger.h"
#include "Macros.h"

void AbstractDistro::setHostName(string & hostName)
{
    //TODO implement this.
}

void AbstractDistro::disableSshPasswordAuthentication()
{
    //TODO implement this.
}

void AbstractDistro::restartSshService()
{
    string sshRestartCmd = this->service_cmd + " " + this->ssh_service_name + " " + this->ssh_service_restart_option;
    CommandResult commandResult;
    CommandExecuter::RunGetOutput(sshRestartCmd.c_str(), commandResult);
    if (commandResult.exitCode != PROCESS_EXIT_SUCCESS)
    {
        Logger::getInstance().Error("restart the ssh service failed.");
    }
}

void AbstractDistro::stopDhcp()
{
    //TODO implement this.
}

bool AbstractDistro::isDhcpEnabled()
{
    //TODO implement this.
    return false;
}

void AbstractDistro::sshDeployPublicKey(string & pubKeyFilePath, string & sshFilePath)
{
    //TODO implement this.
    string commandToConvertToRsa = "ssh-keygen -i -m PKCS8 -f "+ pubKeyFilePath;
    CommandResult commandResult;
    CommandExecuter::RunGetOutput(commandToConvertToRsa.c_str(), commandResult);

    string sshPubKey = *commandResult.output;
    FileOperator::append_content(sshFilePath.c_str(), sshPubKey);
}

string AbstractDistro::getHome()
{
    return "/home";
}
