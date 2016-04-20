#pragma once
#include <string>
using namespace std;

class HandlerManifest
{
public:
    string installCommand;
    string enableCommand;
    string uninstallCommand;
    string disableCommand;
    HandlerManifest();
    ~HandlerManifest();
};

