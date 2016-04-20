#include <string>
#include "AgentConfig.h"
#include "FileOperator.h"
#include "Logger.h"
#include "StringUtil.h"
#include "UserManager.h"
#ifdef _WIN32
#else
#include <cstdlib>
#include <unistd.h>
#endif
using namespace std;

UserManager::UserManager()
{
}

int UserManager::CreateUser(const string& userName, const string& passWord)
{
    Logger::getInstance().Warning("creating the user");
    struct passwd * existed = getpwnam(userName.c_str());
    bool usePassword = true;
    if (existed != NULL)
    {
        Logger::getInstance().Error("user existed already");
        return AGENT_FAILED;
    }
    else
    {
#ifdef __FreeBSD__
        string command = string("pw useradd ") + userName + " -m";
        CommandResult addUserResult;
        CommandExecuter::RunGetOutput(command, addUserResult);
        //TODO deallocate the c_str();
        Logger::getInstance().Warning("user add result: %s", addUserResult.output->c_str());
        AgentConfig::getInstance().LoadConfig();

        string changePasswordCmd = string("echo -n ") + passWord + string(" | pw usermod ") + userName + " -h0";
        CommandResult commandResult;
        CommandExecuter::RunGetOutput(changePasswordCmd, commandResult);
        if (commandResult.exitCode == 0)
        {
            Logger::getInstance().Warning("user change password result succeeded :%d, %s", commandResult.exitCode, commandResult.output->c_str());
            string bsdSudoerDirBase = string(BSD_SUDOERS_DIR_BASE);
            string modernSusoerDir = bsdSudoerDirBase + "sudoers.d/";
            if (!FileOperator::file_exists(modernSusoerDir.c_str()))
            {
                FileOperator::make_dir(modernSusoerDir.c_str());
                string sudoersFileContent;
                string sudoersFilePath = modernSusoerDir + "sudoers";
                FileOperator::get_content(sudoersFilePath.c_str(), sudoersFileContent);
                sudoersFileContent += sudoersFileContent + "\n#includedir " + bsdSudoerDirBase + "sudoers.d\n";
            }
            if (usePassword)
            {
                string waagentSudoerFileContent = userName + " ALL = (ALL) NOPASSWD: ALL\n";
                string waagentSudoerFilePath = bsdSudoerDirBase + "sudoers.d/waagent";
                FileOperator::save_file(waagentSudoerFileContent, waagentSudoerFilePath);
                int chmodResult =  chmod(waagentSudoerFilePath.c_str(), (mode_t)0440);
                if (chmodResult != 0)
                {
                    Logger::getInstance().Error("chmod failed");
                }
            }

        }
        else
        {
            Logger::getInstance().Error("user change password result failed:%d %s", commandResult.exitCode, commandResult.output->c_str());
        }
        return AGENT_SUCCESS;
#else
        string command = string("useradd -m ") + userName;
        CommandResult addUserResult;
        CommandExecuter::RunGetOutput(command,addUserResult);
        //TODO deallocate the c_str();
        Logger::getInstance().Log("user add result: %s", addUserResult.output->c_str());
        AgentConfig::getInstance().LoadConfig();

        string crypt_id;
        int getCryptIdResult = AgentConfig::getInstance().getConfig("Provisioning_PasswordCryptId", crypt_id);
        if (getCryptIdResult != 0)
        {
            Logger::getInstance().Warning("Provisioning_PasswordCryptId not found");
            crypt_id = "6";
        }

        string salt_len;
        int getSaltLenResult = AgentConfig::getInstance().getConfig("Provisioning_PasswordCryptSaltLength", salt_len);

        int salt_len_val = 10;
        if (getSaltLenResult == 0)
        {
            Logger::getInstance().Warning("Provisioning_PasswordCryptSaltLength found");
            salt_len_val = atoi((salt_len).c_str());
        }

        if (salt_len_val == 0)
        {
            Logger::getInstance().Warning("salt_len_val is zero");
            return AGENT_FAILED;
        }

        char * salt = new char[salt_len_val];
        StringUtil::gen_random(salt, salt_len_val);
        string realSalt = crypt_id + salt;
        //TODO check whether the c_str is deallocated.
        char * passWordToSet = crypt(passWord.c_str(), realSalt.c_str());
        delete salt;
        salt = NULL;

        string changePasswordCmd = string("usermod -p '") + passWordToSet + "' " + userName;
        delete passWordToSet;
        passWordToSet = NULL;
        CommandResult commandResult;
        CommandExecuter::RunGetOutput(changePasswordCmd, commandResult);
        if (commandResult.exitCode == 0)
        {
            Logger::getInstance().Warning("user add result:%d, %s", commandResult.exitCode, commandResult.output->c_str());
        }
        else
        {
            Logger::getInstance().Warning("user add result:%d %s", commandResult.exitCode, commandResult.output->c_str());
        }
        return AGENT_SUCCESS;
#endif
    }
}

UserManager::~UserManager()
{
}
