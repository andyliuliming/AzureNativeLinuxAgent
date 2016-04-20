#pragma once

#include <sys/types.h>
#include "CommandExecuter.h"
#ifdef _WIN32
#else
#include <pwd.h>
#endif
using namespace std;

class UserManager
{
public:
    UserManager();
    static int CreateUser(const string&userName, const string&passWord);
    ~UserManager();
};

