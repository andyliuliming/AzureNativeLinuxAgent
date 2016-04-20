#include "HttpRoutine.h"
#include "Logger.h"
#include "StatusReporter.h"
#include "StringUtil.h"
StatusReporter::StatusReporter()
{
}

int StatusReporter::ReportReady(AzureEnvironment &environment, GoalState & goalState, string &incarnationValue)
{
    string healthReport = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Health xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><GoalStateIncarnation>"
        + goalState.incarnation
        + "</GoalStateIncarnation><Container><ContainerId>"
        + goalState.containerId
        + "</ContainerId><RoleInstanceList><Role><InstanceId>"
        + goalState.roleInstanceId
        + "</InstanceId><Health><State>Ready</State></Health></Role></RoleInstanceList></Container></Health>";
    map<string, string> headers;
    headers["x-ms-agent-name"] = WAAGENT_NAME;
    headers["Content-Type"] = "text/xml; charset=utf-8";
    headers["x-ms-version"] = WAAGENT_VERSION;
    string apiAddress = "http://" + environment.wireServerAddress + "/machine?comp=health";
    HttpResponse response;
    int postResult = HttpRoutine::Post(apiAddress.c_str(), &headers, healthReport.c_str(), response);
    Logger::getInstance().Error("report ready status result %d", response.status_code);
    incarnationValue = response.headers.find("x-ms-latest-goal-state-incarnation-number")->second;
    StringUtil::trim(incarnationValue);
    return postResult;
}

int StatusReporter::ReportNotReady(AzureEnvironment &environment, GoalState & goalState, string &status, string&desc)
{
    string healthReport = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Health xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><GoalStateIncarnation>"
        + goalState.incarnation
        + "</GoalStateIncarnation><Container><ContainerId>"
        + goalState.containerId
        + "</ContainerId><RoleInstanceList><Role><InstanceId>"
        + goalState.roleInstanceId
        + "</InstanceId><Health><State>NotReady</State>"
        + "<Details><SubStatus>" + status + "</SubStatus><Description>" + desc + "</Description></Details>"
        + "</Health></Role></RoleInstanceList></Container></Health>";
    map<string, string> headers;
    headers["x-ms-agent-name"] = WAAGENT_NAME;
    headers["Content-Type"] = "text/xml; charset=utf-8";
    headers["x-ms-version"] = WAAGENT_VERSION;
    string apiAddress = "http://" + environment.wireServerAddress + "/machine?comp=health";
    HttpResponse response;
    long postResult = HttpRoutine::Post(apiAddress.c_str(), &headers, healthReport.c_str(), response);
    return postResult;
}

int StatusReporter::ReportRoleProperties(AzureEnvironment & environment, GoalState &goalState, const char * thumbprint)
{
    string healthReport = ("<?xml version=\"1.0\" encoding=\"utf-8\"?><RoleProperties><Container><ContainerId>"
        + goalState.containerId + "</ContainerId>"
        + "<RoleInstances><RoleInstance>"
        + "<Id>" + goalState.roleInstanceId + "</Id>"
        + "<Properties><Property name=\"CertificateThumbprint\" value=\"" + thumbprint + "\" /></Properties>"
        + "</RoleInstance></RoleInstances></Container></RoleProperties>");
    map<string, string> headers;
    headers["x-ms-agent-name"] = WAAGENT_NAME;
    headers["Content-Type"] = "text/xml; charset=utf-8";
    headers["x-ms-version"] = WAAGENT_VERSION;
    string apiAddress = "http://" + environment.wireServerAddress + "/machine?comp=roleProperties";
    HttpResponse response;
    long postResult = HttpRoutine::Post(apiAddress.c_str(), &headers, healthReport.c_str(), response);
    return postResult;
}

StatusReporter::~StatusReporter()
{
}
