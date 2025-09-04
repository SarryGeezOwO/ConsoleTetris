#ifndef LOG_HPP
#define LOG_HPP

#include <unordered_map>

enum LogType
{
    DEBUG,
    OK,
    WARNING,
    FATAL,
};

void Log(const char* msg) {
    std::cout << msg << std::endl;
}

void Log(LogType type, const char* msg) {
    static std::unordered_map<LogType, std::string> color_str = {
        {DEBUG,     "\x1B[37m"},
        {OK,        "\x1B[32m"},
        {WARNING,   "\x1B[33m"},
        {FATAL,     "\x1B[31m"},
    };

    std::cout << color_str[type].c_str() << msg << "\033[0m" << std::endl;
}

#endif