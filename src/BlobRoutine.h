#pragma once
#include <string>
#include <map>
#include "HttpRoutine.h"
#include "HttpResponse.h"
#include <cstring>
using namespace std;

class BlobRoutine
{
public:
    BlobRoutine();
    static int Put(const char * url, const char*blobType, const char * data, HttpResponse &response);
    ~BlobRoutine();
};

