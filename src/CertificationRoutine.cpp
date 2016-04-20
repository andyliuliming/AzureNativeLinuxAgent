#include "CertificationRoutine.h"
#include "CommandExecuter.h"
#include "FileOperator.h"
#include "Logger.h"
#include "Macros.h"
#include <string>
using namespace std;

CertificationRoutine::CertificationRoutine()
{
}

int CertificationRoutine::GenerateTransportCertification()
{
    //TODO: generate the openssl TRANSPORT_CERT_PRIV
    string commandToExecute =
        string("openssl req -x509 -nodes -subj /CN=LinuxTransport -days 32768 -newkey rsa:2048 -keyout ") + TRANSPORT_CERT_PRIV + " -out " + TRANSPORT_CERT_PUB;
    CommandResult result;
    CommandExecuter::RunGetOutput(commandToExecute, result);
    if (result.exitCode != AGENT_SUCCESS)
    {
        Logger::getInstance().Error(result.output->c_str());
    }
    return result.exitCode;
}

CertificationRoutine::~CertificationRoutine()
{
}
