#include <stdio.h>
#include <stdarg.h>
#ifdef _WIN32
#else
#include <syslog.h>
#include <unistd.h>
#endif
#include "Logger.h"
using namespace std;
void Logger::Verbose(const char * msg, ...)
{
    va_list arguments;                     // A place to store the list of arguments
    va_start(arguments, msg);           // Initializing arguments to store all values after num
#ifdef _WIN32
#else
    vsyslog(LOG_DEBUG, msg, arguments);
#endif
    va_end(arguments);                  // Cleans up the list
}

void Logger::Log(const char * msg, ...)
{
    va_list arguments;                     // A place to store the list of arguments
    va_start(arguments, msg);           // Initializing arguments to store all values after num
#ifdef _WIN32
#else
    vsyslog(LOG_INFO, msg, arguments);
#endif
    va_end(arguments);                  // Cleans up the list
}

void Logger::Warning(const char * msg, ...)
{
    va_list arguments;                     // A place to store the list of arguments
    va_start(arguments, msg);           // Initializing arguments to store all values after num
#ifdef _WIN32
#else
    vsyslog(LOG_WARNING, msg, arguments);
#endif
    va_end(arguments);                  // Cleans up the list
}

void Logger::Error(const char * msg, ...)
{
    va_list arguments;                     // A place to store the list of arguments
    va_start(arguments, msg);           // Initializing arguments to store all values after num
#ifdef _WIN32
#else
    vsyslog(LOG_ERR, msg, arguments);
#endif
    va_end(arguments);                  // Cleans up the list
}
