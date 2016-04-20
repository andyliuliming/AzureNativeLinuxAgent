#pragma once
#include <map>
#include <memory>
#include <string>
using namespace std;

class HttpResponse
{
public:
    int status_code;
    map<string, string> headers;
    string body;
    HttpResponse();
    void addHeader(const char * header_item);
    ~HttpResponse();
};

