#pragma once
#ifdef _WIN32
#else
#include <syslog.h>
#endif

#include "Macros.h"
class Logger
{
public:
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }
    void Verbose(const char *msg, ...);
    void Log(const char *msg, ...);
    void Warning(const char * msg, ...);
    void Error(const char * mgs, ...);
private:
    Logger() {
#ifdef _WIN32
#else
        setlogmask(LOG_UPTO(LOG_NOTICE));
        openlog(WAAGENT_SYSLOG_NAME, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
#endif
    }
    Logger(Logger const&);              // Don't Implement.
    ~Logger() {
#ifdef _WIN32
#else
        closelog();
#endif
    }
    void operator=(Logger const&); // Don't implement
};
