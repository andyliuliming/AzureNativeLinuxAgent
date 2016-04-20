#pragma once
#include <string>
using namespace std;
class Certificates
{
private:
    string certificatesData;
    bool isKeyEndLine(string & line);
    bool isCertificateEndLine(string &line);
public:
    Certificates(); 
    void Parse(string &certificatesText);
    void Process();
    ~Certificates();
};

