
#include "Logger.h"
#include "XmlRoutine.h"
using namespace std;

XmlRoutine::XmlRoutine()
{
}

xmlXPathObjectPtr XmlRoutine::findNodeByRelativeXpath(xmlDocPtr doc, xmlNodePtr rootnode, const char * xpathExpr, map<string,string> *namespaces)
{
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    // Important part
    xpathCtx->node = rootnode;
    if (namespaces != NULL)
    {
        for (std::map<string, string>::iterator it = namespaces->begin(); it != namespaces->end(); ++it)
        {
            if (xmlXPathRegisterNs(xpathCtx, BAD_CAST it->first.c_str(), BAD_CAST it->second.c_str()) != 0)
            {
                Logger::getInstance().Error("Error: unable to register NS with prefix");
            }
        }
    }
    xmlChar * xpathXmlCharExpr = xmlCharStrdup(xpathExpr);
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathXmlCharExpr, xpathCtx);
    delete xpathXmlCharExpr;
    xpathXmlCharExpr = NULL;
    xmlXPathFreeContext(xpathCtx);
    return xpathObj;
}

xmlXPathObjectPtr XmlRoutine::getNodes(xmlDocPtr doc, const char* xpathExpr, map<string, string> * namespaces)
{
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL)
    {
        Logger::getInstance().Error("Error: unable to create new XPath context");
        return NULL;
    }
    if (namespaces != NULL)
    {
        for (std::map<string, string>::iterator it = namespaces->begin(); it != namespaces->end(); ++it)
        {
            if (xmlXPathRegisterNs(xpathCtx, BAD_CAST it->first.c_str(), BAD_CAST it->second.c_str()) != 0)
            {
                Logger::getInstance().Error("Error: unable to register NS with prefix");
            }
        }
    }
    xmlChar * xpathXmlCharExpr = xmlCharStrdup(xpathExpr);

    xpathObj = xmlXPathEvalExpression(xpathXmlCharExpr, xpathCtx);
    delete xpathXmlCharExpr;
    xpathXmlCharExpr = NULL;
    if (xpathObj == NULL)
    {
        Logger::getInstance().Error("Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        return NULL;
    }
    xmlXPathFreeContext(xpathCtx);
    return xpathObj;
}

void XmlRoutine::getNodeContent(xmlNodePtr node, string &value)
{
    const char* runtimeSettingsText = (const char*)xmlNodeGetContent(node);
    value = (runtimeSettingsText);
    delete runtimeSettingsText;
    runtimeSettingsText = NULL;
}

void XmlRoutine::getNodeProperty(xmlNodePtr node, const char* propertyName, string&value)
{
    xmlChar * propertyDup = xmlCharStrdup(propertyName);
    xmlChar * incarnation = xmlGetProp(node, propertyDup);
    //TODO error handling when the incarnation is not there.
    value = (const char*)incarnation;
    delete incarnation;
    incarnation = NULL;
    delete propertyDup;
    propertyDup = NULL;
}

int XmlRoutine::getNodeText(xmlDocPtr doc, const char * xpathExpr, map<string, string> * namespaces, string &text)
{
    /* Evaluate xpath expression */
    //
    const xmlChar* xpathXmlCharExpr = xmlCharStrdup(xpathExpr);
    xmlXPathObjectPtr xpathObj = getNodes(doc, xpathExpr, namespaces);
    delete xpathXmlCharExpr;
    xpathXmlCharExpr = NULL;
    if (xpathObj == NULL)
    {
        Logger::getInstance().Error("Error: unable to evaluate xpath expression \"%s\"", xpathExpr);
        return ELEMENT_NOT_FOUNT;
    }
    else
    {
        int result = AGENT_FAILED;
        if (xpathObj->nodesetval == NULL)
        {
            Logger::getInstance().Error("Error: node set is null for \"%s\"", xpathExpr);
            result = ELEMENT_NOT_FOUNT;
        }
        else
        {
            if (xpathObj->nodesetval->nodeNr > 0)
            {
                xmlNodeSetPtr nodes = xpathObj->nodesetval;
                text = ((const char*)nodes->nodeTab[0]->content);
                result = AGENT_SUCCESS;
            }
            else
            {
                Logger::getInstance().Error("node not found for \"%s\"", xpathExpr);
                result = ELEMENT_NOT_FOUNT;
            }
        }
        xmlXPathFreeObject(xpathObj);
        return result;
    }
}

XmlRoutine::~XmlRoutine()
{
}