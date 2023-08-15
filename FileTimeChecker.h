#ifndef FILE_TIME_CHECKER_H
#define FILE_TIME_CHECKER_H
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#define stat _stat
#endif

#include<string>
#include<ctime>
static std::string get_modification_file_time(const std::string& file)
{
    struct stat result;
    if (stat(file.c_str(), &result) == 0)
    {
        time_t mod_time = result.st_mtime;
        char mbstr[100];
        if (std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&mod_time)))
            return std::string(mbstr);
    }
    return "";
}

#endif