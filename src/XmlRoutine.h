#pragma once
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <map>
#include <stdio.h>
#include <string>
using namespace std;

class XmlRoutine
{
public:
    XmlRoutine();
    static xmlXPathObjectPtr findNodeByRelativeXpath(xmlDocPtr doc,xmlNodePtr rootnode, const char * xpathExpr, map<string,string> *namespaces);
    static xmlXPathObjectPtr getNodes(xmlDocPtr doc, const char* xpathExpr, map<string, string> * namespaces);
    static int getNodeText(xmlDocPtr doc, const char* xpathExpr, map<string, string> * namespaces, string &text);
    static void getNodeProperty(xmlNodePtr node, const char* propName, string&value);
    static void getNodeContent(xmlNodePtr node, string &value);
    ~XmlRoutine();
};