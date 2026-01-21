#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

/*This library is a copy/fork of what I usually use Just made windows compilable*/

class Logger
{
    private:
    bool logEnabled = false; //Use to enable or disable logging might not be necessary for this project
    public:
    void enable() {logEnabled = true;}
    void disable() {logEnabled = false;}

    void print(const char* msg)
    {
        if(!logEnabled) return;
        ::printf("%s", msg);
    }

    void println(const char* msg)
    {
        if(!logEnabled) return;
        ::printf("%s\n", msg);
    }

    void printf(const char *fmt, ...)
    {
        if(!logEnabled) return;
        char buf[128];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        ::printf("%s\n", buf);
    }

    void debug(const char* fmt, ...)
    {
        if(!logEnabled) return;

        time_t now = time(NULL);
        struct tm* t = localtime(&now);

        char buf[128];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        ::printf("[%02d:%02d:%02d] [DEBUG] %s\n", t->tm_hour, t->tm_min, t->tm_sec, buf);
    }

    void info(const char* fmt, ...)
    {
        if(!logEnabled) return;

        time_t now = time(NULL);
        struct tm* t = localtime(&now);

        char buf[128];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        ::printf("[%02d:%02d:%02d] [INFO] %s\n", t->tm_hour, t->tm_min, t->tm_sec, buf);
    }

    void error(const char* fmt, ...)
    {
        if(!logEnabled) return;

        time_t now = time(NULL);
        struct tm* t = localtime(&now);

        char buf[128];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        ::printf("[%02d:%02d:%02d] [ERROR] %s\n", t->tm_hour, t->tm_min, t->tm_sec, buf);
    }


};

extern Logger Log;