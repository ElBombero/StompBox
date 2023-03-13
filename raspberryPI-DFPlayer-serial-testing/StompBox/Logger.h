#pragma once

#include <string>
//#include <cstdio>
#include <iostream>


class Logger
{
public:
    enum class ELogLevel
    {
        Log_Debug,
        Log_Warning,
        Log_Error
    };

    static void Log(ELogLevel level, std::string source, std::string message)
    {
        std::string levelStr = "";
        switch (level)
        {
        case ELogLevel::Log_Debug:
            levelStr = "D";
            break;
        case ELogLevel::Log_Warning:
            levelStr = "W";
            break;
        case ELogLevel::Log_Error:
            levelStr = "E";
            break;
        default:
            levelStr = "U";
        }
        std::cout << "<" << levelStr << "> " << source << " - " << "\'" << message << "\'" << std::endl;
    }
};

