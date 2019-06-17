#include "logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <stdio.h>
#include <sstream>
#include <fstream>

Logger::Logger()
{

}

Logger* Logger::instance = NULL;
std::map<std::thread::id, std::string> Logger::thread_names;
std::mutex Logger::thread_names_mux;

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

void Logger::debug (std::string logmessage, bool timestamp)
{
    //Add timestamp if needed
    if (timestamp)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
        auto epoch = now_us.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::microseconds>(epoch);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%T");
        ss << "." << std::setw(6) << std::to_string(value.count() % 1000000) << ": ";
        logmessage = ss.str() + logmessage;
    }

    CPPDEBUG(logmessage << std::endl);
}

void Logger::info (std::string logmessage, bool timestamp)
{
    std::cout << logmessage << std::endl;
}

void Logger::debugPeriodic (std::string logmessage, size_t millis, size_t repetition, bool timestamp)
{
    debugPeriodic (logmessage, std::chrono::milliseconds(millis), repetition, timestamp);
}

void Logger::debugPeriodic (std::string logmessage, std::chrono::milliseconds perDuration, size_t repetition, bool timestamp)
{
    getInstance ()->_debugPeriodic (logmessage, perDuration, repetition, timestamp);
}

void Logger::_debugPeriodic (std::string logmessage, std::chrono::milliseconds perDuration, size_t repetition, bool timestamp)
{
    auto now = std::chrono::high_resolution_clock::now();
    if (logmessage != prev_message)
    {
        debug(logmessage, timestamp);
        message_duplicatecount = 0;
    }
    else if (logmessage == prev_message && prev_time + perDuration < now)
    {
        debug(logmessage, timestamp);
        message_duplicatecount = 0;
    }
    else if (logmessage == prev_message && prev_time + perDuration > now)
    {
//        CPPDEBUG("Same message and timeout has not yet passed" << std::endl);
        message_duplicatecount++;
        if (message_duplicatecount % repetition == 0)
        {
            debug(logmessage + " x (" + std::to_string(message_duplicatecount) + ")", timestamp);
        }
    }
    prev_time = std::chrono::high_resolution_clock::now();
    prev_message = logmessage;
}

void Logger::debugThread(std::string logmessage, bool timestamp)
{
    std::thread::id threadid = std::this_thread::get_id();
    if (thread_names.find(threadid) != thread_names.end())
    {
        debug("thread=" + thread_names[threadid] + ":" + logmessage, timestamp);
    }
    else
    {
        std::stringstream ss;
        ss << threadid;
        std::string thread_id_str = ss.str();
        debug(thread_id_str + ":" + logmessage, timestamp);
    }
}

void Logger::registerThreadName(std::thread::id id, std::string name)
{
    std::lock_guard<std::mutex> guard(thread_names_mux);
    thread_names[id] = name;
}

std::string Logger::printArray(const char array[], size_t len)
{
    char buffer[3];
    std::string result = "";

    for (size_t i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            result += "\n";
        }
        sprintf(buffer, "%02x", (array[i] & 0xFF));
        result += buffer;

        if ((i) % 2 == 1)
        {
            result += " ";
        }

    }

    return result;
}

void Logger::saveBinaryArray(std::string name, const char array[], size_t len)
{
    saveBinaryArray(name, reinterpret_cast<const unsigned char*>(array), len);
}

void Logger::saveBinaryArray(std::string name, const unsigned char array[], size_t len)
{
    std::ofstream ofstr("save/"+name);
    for (size_t i = 0; i < len; i++)
    {
        ofstr << array[i];
    }
    ofstr.flush();
    ofstr.close();
}

