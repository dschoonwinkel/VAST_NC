#include "logger.h"
#include <iostream>

Logger::Logger()
{

}

Logger* Logger::instance = NULL;

Logger *Logger::getInstance()
{
    if (!instance)
    {
        instance = new Logger();
        instance->prev_time = std::chrono::high_resolution_clock::now();
        instance->prev_message = "";
    }

    return instance;
}

void Logger::debug (std::string logmessage)
{
    CPPDEBUG(logmessage << std::endl);
}

void Logger::info (std::string logmessage)
{
    std::cout << logmessage << std::endl;
}

void Logger::debugPeriodic (std::string logmessage, size_t millis, size_t repetition)
{
    debugPeriodic (logmessage, std::chrono::milliseconds(millis), repetition);
}

void Logger::debugPeriodic (std::string logmessage, std::chrono::milliseconds perDuration, size_t repetition)
{
    getInstance ()->_debugPeriodic (logmessage, perDuration, repetition);
}

void Logger::_debugPeriodic (std::string logmessage, std::chrono::milliseconds perDuration, size_t repetition)
{
    auto now = std::chrono::high_resolution_clock::now();
    if (logmessage != prev_message)
    {
        CPPDEBUG(logmessage << std::endl);
        message_duplicatecount = 0;
    }
    else if (logmessage == prev_message && prev_time + perDuration < now)
    {
        CPPDEBUG(logmessage << std::endl);
        message_duplicatecount = 0;
    }
    else if (logmessage == prev_message && prev_time + perDuration > now)
    {
//        CPPDEBUG("Same message and timeout has not yet passed" << std::endl);
        message_duplicatecount++;
        if (message_duplicatecount % repetition == 0)
        {
            CPPDEBUG(logmessage + " x (" + std::to_string(message_duplicatecount) + ")" << std::endl);
        }
    }
    prev_time = std::chrono::high_resolution_clock::now();
    prev_message = logmessage;
}

