#include <cstdio>
#include <memory>
#include <spawn.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "CommandExecuter.h"
#include "CommandResult.h"
#include "Logger.h"
#include "Macros.h"
using namespace std;

CommandExecuter::CommandExecuter()
{
}

void CommandExecuter::PosixSpawn(string& cmd, string &cwd)
{
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        // TODO check the cwd_str is deallocated.
        const char * cwd_str = cwd.c_str();
        int chDirResult = chdir(cwd_str);

        CommandResult installResult;
        const char * cmd_str = cmd.c_str();
        CommandExecuter::RunGetOutput(cmd_str, installResult);

        if (installResult.exitCode != AGENT_SUCCESS)
        {
            Logger::getInstance().Error("subprocess exit with code:%d, output:%s", installResult.exitCode, installResult.output->c_str());
        }
        exit(installResult.exitCode);
    }
    else
    {
        if (waitpid(pid, &status, 0) != -1)
        {
            Logger::getInstance().Log("Child exited with status %i", status);
        }
        else
        {
            Logger::getInstance().Error("waitpid failed");
        }

    }
}

void CommandExecuter::RunGetOutput(string &cmd, CommandResult &commandResult)
{
    const char * cmd_str = cmd.c_str();
    RunGetOutput(cmd_str, commandResult);
    /*delete cmd_str;
    cmd_str = NULL;*/
}

void CommandExecuter::RunGetOutput(const char* cmd, CommandResult &commandResult) {

    FILE* pipe = POPEN(cmd, "r");
    if (!pipe)
    {
        commandResult.exitCode = AGENT_FAILED;
    }
    else
    {
        char buffer[128];
        string *result = new string();

        while (!feof(pipe))
        {
            if (fgets(buffer, 128, pipe) != NULL)
            {
                //TODO error use this?
                result->append(buffer);
            }
        }
        int returnCode = PCLOSE(pipe);
        commandResult.exitCode = returnCode;
        commandResult.output = result;
    }
}

CommandExecuter::~CommandExecuter()
{
}
