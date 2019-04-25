#ifndef BASICLOGGER_H
#define BASICLOGGER_H

#include "Config.h"
#include <chrono>

class Logger
{
public:
    Logger();

    static void debug(std::string logmessage);
    static void debugPeriodic(std::string logmessage, std::chrono::milliseconds perDuration = std::chrono::milliseconds(1), size_t repetition = 1000);
    static void debugPeriodic(std::string logmessage, size_t millis, size_t repetition = 1000);
    void _debugPeriodic(std::string logmessage, std::chrono::milliseconds perDuration, size_t repetition);
    static void info(std::string logmessage);

    static Logger* getInstance();

    static Logger *instance;
    std::string prev_message;
    std::chrono::high_resolution_clock::time_point prev_time;
    size_t message_duplicatecount = 0;

};

#endif // BASICLOGGER_H
