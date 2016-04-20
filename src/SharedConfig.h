#pragma once
#include <string>
using namespace std;
class SharedConfig
{
public:
    SharedConfig();
    void Parse(string &sharedConfigText);
    void Process();
    ~SharedConfig();
};

