#include "BlobRoutine.h"
#include "Logger.h"
#include "Macros.h"

BlobRoutine::BlobRoutine()
{
}

int BlobRoutine::Put(const char * url,const char* blobType, const char * data, HttpResponse & response)
{
    time_t     now = time(NULL);
    struct tm  tstruct;
    char       tstamp[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(tstamp, sizeof(tstamp), "%Y-%m-%dT%H.%M.%SZ", &tstruct);
    map<string, string> headers;
    headers["x-ms-date"] = tstamp;
    headers["x-ms-blob-type"] = blobType;
    headers["x-ms-version"] = STORAGE_VERSION;
    return HttpRoutine::Put(url, &headers, data, response);
}


BlobRoutine::~BlobRoutine()
{
}
