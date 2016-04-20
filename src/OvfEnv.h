#pragma once
#include <string>
#include <map>
using namespace std;

class OvfEnv
{
private:
    string hostName;
    string userName;
    string passWord;
    bool disableSshPasswordAuthentication;
    string customData;
    string fingerPrint;
    map<string, string> SshPublicKeys;
    map<string, string> SshKeyPairs;
    int majorVersion = 1;
    int minorVersion = 0;
public:
    OvfEnv(); 
    int Parse(string &sharedConfigText);
    int Process();
    ~OvfEnv();
};

