#pragma once
#include <string>
using namespace std;

class AbstractDistro
{
public:
    static AbstractDistro& getInstance()
    {
        static AbstractDistro instance;
        return instance;
    }
    void setHostName(string &hostName);
    void disableSshPasswordAuthentication();
    void restartSshService();
    void stopDhcp();
    bool isDhcpEnabled();
    void sshDeployPublicKey(string &pubKeyFilePath, string &sshFilePath);
    
    string getHome();
private:
    string service_cmd;
    string ssh_service_restart_option;
    string ssh_service_name;
    AbstractDistro()
    {
        this->service_cmd = "/usr/sbin/service";
        this->ssh_service_restart_option = "restart";
        this->ssh_service_name = "ssh";
    }
    AbstractDistro(AbstractDistro const&);              // Don't Implement.
    ~AbstractDistro() {
    };
    void operator=(AbstractDistro const&); // Don't implement
};

