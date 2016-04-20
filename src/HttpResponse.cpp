#include "HttpResponse.h"
#include "StringUtil.h"
#include <string>
using namespace std;

HttpResponse::HttpResponse()
{
}

void HttpResponse::addHeader(const char * header_item)
{
    string headerLine = header_item;
    string spliter = ":";
    vector<string> splitted;
    StringUtil::string_split(headerLine, spliter, &splitted);
    if (splitted.size() > 1)
    {
        this->headers.insert(pair<string, string>(splitted[0], splitted[1]));
    }
}

HttpResponse::~HttpResponse()
{
}
