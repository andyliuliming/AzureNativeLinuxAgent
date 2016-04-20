#pragma once
#include "HttpResponse.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <curl/curl.h>
using namespace std;

struct MemoryStruct
{
    const char *memory;
    size_t size;
};
class HttpRoutine
{
private:
    //
    //  libcurl write callback function
    //
    static size_t writer(const char *data, size_t size, size_t nmemb, string *writerData);
    static size_t writerToFile(const char *data, size_t size, size_t nmemb, FILE *file);
    static size_t header_callback(const char *buffer, size_t size, size_t nitems, HttpResponse *writerData);
    static size_t ReadMemoryCallback(void *ptr, size_t size, size_t nmemb, void *pMem);
    static char errorBuffer[CURL_ERROR_SIZE];
    static bool init_common(CURL *&conn, const char *url);

public:
    HttpRoutine();
    static int GetWithDefaultHeader(const char *url, HttpResponse &response);
    static int Get(const char * url, map<string, string> * headers, HttpResponse &response);
    static int GetToFile(const char * url, map<string, string> * headers, const char * filePath);
    static int Post(const char * url, map<string, string> * headers, const char * data, HttpResponse&response);
    static int Put(const char * url, map<string, string> * headers, const char * data, HttpResponse &response);
    ~HttpRoutine();
};

