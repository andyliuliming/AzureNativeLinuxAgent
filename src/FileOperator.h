#pragma once
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/stat.h>
using namespace std;

class FileOperator
{
public:
    FileOperator();
    static bool file_exists(const char* name);
    //TODO make_dir need to have the owner and the mod.
    static int make_dir(const char* dir_path);
    static int prepare_dir(const char* file_path);
    //TODO change owner.
    static bool move_file(const char * from, const char *to);
    static void save_file(const char * content, long size, const char * fileName);
    static void save_file(const string& content, const string& fileName);
    static int get_content(const char *fileName, string&content);
    static int append_content(const char * filePath, string&content);
    ~FileOperator();
};

