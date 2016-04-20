#pragma once

using namespace std;

class ZipRoutine
{
public:
    ZipRoutine();
    static int UnZipToDirectory(const string& archive, const string& zipExtractDirectory);
    ~ZipRoutine();
};

