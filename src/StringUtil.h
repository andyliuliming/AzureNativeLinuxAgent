#pragma once
#include <string>
#include <vector>
using namespace std;

class StringUtil
{
public:
    StringUtil();
    static void string_split(std::string& s, std::string& delim, std::vector< std::string >* ret);
    static void gen_random(char *s, const int len);
    // trim from both ends
    static string trim(const string &s, const string &ws);
    // trim from start
    static string &ltrim(string &s);

    // trim from end
    static string &rtrim(string &s);

    // trim from both ends
    static string &trim(string &s);
    ~StringUtil();
};