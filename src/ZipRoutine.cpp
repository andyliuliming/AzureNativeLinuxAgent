#include "FileOperator.h"
#include "ZipRoutine.h"
#include "Logger.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zip.h>
using namespace std;

ZipRoutine::ZipRoutine()
{
    //
}

/*
the zipExtractDirectory format should be like /var/lib/waagent/xxx_version/
*/
int ZipRoutine::UnZipToDirectory(const string& archive, const string& zipExtractDirectory)
{
    

    struct zip *za;
    struct zip_file *zf;
    struct zip_stat sb;
    char buf[100];
    int err;
    int i, len;
    int fd;
    long long sum;

    //TODO check whether the c_str is deallocated
    if ((za = zip_open(archive.c_str(), 0, &err)) == NULL)
    {
        zip_error_to_str(buf, sizeof(buf), err, errno);
        Logger::getInstance().Error("can't open zip archive: %s", buf);
        return AGENT_FAILED;
    }

    int entries_num = zip_get_num_entries(za, 0);
    Logger::getInstance().Log("entries_num is: %d", entries_num);
    for (i = 0; i < entries_num; i++)
    {
        if (zip_stat_index(za, i, 0, &sb) == 0)
        {
            len = strlen(sb.name);
            // create the sub dir
            string relative_path = sb.name;
            int last_index_slash = relative_path.find_last_of('/');
            if (last_index_slash != string::npos)
            {
                //not find so it's 
                string folder_part = relative_path.substr(0, last_index_slash);
                string newFolderPath = zipExtractDirectory + folder_part;
                int make_dir_result = FileOperator::make_dir(newFolderPath.c_str());
                if (make_dir_result != 0)
                {
                    Logger::getInstance().Error(" make dir result: %d", make_dir_result);
                }
            }

            if (last_index_slash != (relative_path.length() - 1))
            {
                zf = zip_fopen_index(za, i, 0);
                if (!zf)
                {
                    Logger::getInstance().Error("failed to open file with index");
                    break;
                }
                string newFolderPath = zipExtractDirectory + sb.name;
                fd = open(newFolderPath.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0644);
                if (fd < 0)
                {
                    Logger::getInstance().Error("failed to open %s, %d", newFolderPath.c_str(), fd);
                    break;
                }

                sum = 0;
                while (sum != sb.size)
                {
                    len = zip_fread(zf, buf, 100);
                    if (len < 0)
                    {
                        Logger::getInstance().Error("read length is negative");
                        break;
                    }
                    int written_bytes = write(fd, buf, len);
                    if (len != written_bytes)
                    {
                        Logger::getInstance().Error(" written bytes is not equals to the read");
                    }
                    sum += len;
                }
                close(fd);
                zip_fclose(zf);
            }
        }
        else
        {
            
        }
    }

    if (zip_close(za) == -1)
    {
        Logger::getInstance().Error("can't close zip archive: %s",archive.c_str());
        return AGENT_FAILED;
    }
    
    return AGENT_SUCCESS;
}


ZipRoutine::~ZipRoutine()
{
}
