#pragma once
#include <string>
using namespace std;
class CommandResult
{
public:
    int exitCode;
    // TODO make this an reference
    string *output;
    CommandResult();
    ~CommandResult();
};