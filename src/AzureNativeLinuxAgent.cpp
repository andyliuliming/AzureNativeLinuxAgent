#include <string>
#include "AgentConfig.h"
#include "AzureEnvironment.h"
#include "CertificationRoutine.h"
#include "CommandExecuter.h"
#include "DeviceRoutine.h"
#include "ExtensionsConfig.h"
#include "FileOperator.h"
#include "GoalState.h"
#include "Logger.h"
#include "Macros.h"
#include "Provisioner.h"
#include "StatusReporter.h"
#include "StringUtil.h"
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <regex>
using namespace std;

int main(void)
{
    int chdirResult = chdir(WORKING_DIR);

    int currentPid = getpid();
    char *pidBuff = new char[32];
    sprintf(pidBuff, "%d", currentPid);
    FileOperator::save_file(pidBuff, strlen(pidBuff), WAAGENT_PID_FILE);
    delete pidBuff;
    pidBuff = NULL;

    // load the config at the beginning.
    AgentConfig::getInstance().LoadConfig();

    // 2. do dhcp 
    Logger::getInstance().Log("DoDhcpWork");
    AzureEnvironment azureEnvironment;
    int dhcpWorkResult = 1;

    while (dhcpWorkResult != AGENT_SUCCESS)
    {
        dhcpWorkResult = azureEnvironment.DoDhcpWork();
        if (dhcpWorkResult != AGENT_SUCCESS)
        {
            Logger::getInstance().Error("no azure environment found.");
            SLEEP(120 * 1000);
        }
    }
    Logger::getInstance().Log("azure environment found.");

    // 3. check version
    int checkResult = azureEnvironment.CheckVersion();
    if (checkResult != AGENT_SUCCESS)
    {
        Logger::getInstance().Error("check version failed, so exit.");
        exit(AGENT_FAILED);
    }

    // 4. Set SCSI timeout on SCSI disks
    DeviceRoutine::setIsciTimeOut();

    // 5. GenerateTransportCert
    int transportCertGenerationResult = CertificationRoutine::GenerateTransportCertification();
    if (transportCertGenerationResult != AGENT_SUCCESS)
    {
        Logger::getInstance().Error("generate the certification failed");
    }

    // 6. where true daemon
    Provisioner *provisioner = new Provisioner();
    bool provisioned = provisioner->isProvisioned();
    string incarnationReturned;
    bool pass0 = true;
    GoalState goalState;

    StatusReporter *statusReporter = new StatusReporter();

    while (true)
    {
        // a. UpdateGoalState
        // b. process goal state
        if (pass0
            || goalState.incarnation.compare(incarnationReturned) != AGENT_SUCCESS)
        {
            Logger::getInstance().Error("incarnation in goal state is %s, incarnaitonReturned is %s", goalState.incarnation.c_str(), incarnationReturned.c_str());
            pass0 = false;
            Logger::getInstance().Error("start goal state");
            goalState.UpdateGoalState(azureEnvironment);
            Logger::getInstance().Error("end update goal state");
            if (!provisioned)
            {
                Logger::getInstance().Warning("report not ready");
                string status = "Provisioning";
                string desc = "Starting";
                int reportNotReadyResult = statusReporter->ReportNotReady(azureEnvironment, goalState, status, desc);
                if (reportNotReadyResult != AGENT_SUCCESS)
                {
                    Logger::getInstance().Error("report not ready failed with %d", reportNotReadyResult);
                }
                Logger::getInstance().Warning("doing provision.");
                int provisionResult = provisioner->Prosess();
                //TODO execute CustomData if it has.
                if (provisionResult == AGENT_SUCCESS)
                {
                    provisioner->markProvisioned();
                    provisioned = true;
                }
                else
                {
                    Logger::getInstance().Error("provision failed");
                }
            }

            string type;
            int result = AgentConfig::getInstance().getConfig("Provisioning_SshHostKeyPairType", type);
            if (result != AGENT_SUCCESS)
            {
                type = "rsa";
            }
            Logger::getInstance().Warning("start process.");
            goalState.Process();
            Logger::getInstance().Warning("end process");

            // Report SSH key finger print
            string host_key_path = string("/etc/ssh/ssh_host_") + type + "_key.pub";

            string commandToGetFingerPrint = string("ssh-keygen -lf ") + host_key_path;
            CommandResult fingerPrintResult;
            //TODO check the result.
            CommandExecuter::RunGetOutput(commandToGetFingerPrint, fingerPrintResult);
            vector<string> splitResult;
            string spliter = " ";
            StringUtil::string_split(*(fingerPrintResult.output), spliter, &splitResult);
            int reportRolePropertiesResult = statusReporter->ReportRoleProperties(azureEnvironment, goalState, splitResult[1].c_str());
            if (reportRolePropertiesResult != AGENT_SUCCESS)
            {
                Logger::getInstance().Error("report ReportRoleProperties failed with %d", reportRolePropertiesResult);
            }
        }
        else
        {
            Logger::getInstance().Error("the incarnationReturned is empty, or the incarnation is same");
        }

        // TODO handle the StateConsumer program = Config.get("Role.StateConsumer")
        int sleepToReduceAccessDenied = 3;
        SLEEP(sleepToReduceAccessDenied * 1000);
        if (provisioned)
        {
            Logger::getInstance().Warning("start do report ready");
            int reportReadyResult = statusReporter->ReportReady(azureEnvironment, goalState, incarnationReturned);
            if (reportReadyResult != AGENT_SUCCESS)
            {
                Logger::getInstance().Error("ReportReady failed with %d", reportReadyResult);
            }
        }
        else
        {
            string status = "ProvisioningFailed";
            //TODO make the description more clear.
            string desc = "ProvisioningFailed";
            int reportNotReadyResult = statusReporter->ReportNotReady(azureEnvironment, goalState, status, desc);
            if (reportNotReadyResult != AGENT_SUCCESS)
            {
                Logger::getInstance().Error("ReportNotReady failed with %d", reportNotReadyResult);
            }
        }

        goalState.extensionsConfig->ReportExtensionsStatus();

        SLEEP(25 * 1000);
    }
}
