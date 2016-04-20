#pragma once
#include <memory>
#include <string>
#include <vector>
#include "CommandResult.h"
using namespace std;

class CommandExecuter
{
public:
    CommandExecuter();
    static void PosixSpawn(string& cmd, string &cwd);
    static void RunGetOutput(string &cmd, CommandResult &result);
    static void RunGetOutput(const char* cmd, CommandResult &result);
    ~CommandExecuter();
};

