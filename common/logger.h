#ifndef BASICLOGGER_H
#define BASICLOGGER_H

#include "Config.h"
#include <chrono>
#include <thread>
#include <string>
#include <mutex>
#include <map>

//#define SAVE_PACKETS

class Logger
{
public:
    Logger();

    static void debug(std::string logmessage, bool timestamp = false);
    static void debugPeriodic(std::string logmessage, std::chrono::milliseconds perDuration = std::chrono::milliseconds(1), size_t repetition = 1000, bool timestamp = false);
    static void debugPeriodic(std::string logmessage, size_t millis, size_t repetition = 1000, bool timestamp = false);
    void _debugPeriodic(std::string logmessage, std::chrono::milliseconds perDuration, size_t repetition, bool timestamp = false);

    static void debugThread(std::string logmessage, bool timestamp = false);
    static void registerThreadName(std::thread::id id, std::string name);

    static void info(std::string logmessage, bool timestamp = false);

    //Printing array helper function
    static std::string printArray(const char array[], size_t len);

    ///Save binary array to a file
    static void saveBinaryArray(std::string name, const char array[], size_t len);
    static void saveBinaryArray(std::string name, const unsigned char array[], size_t len);

    static Logger* getInstance();

    static Logger *instance;
    static std::map<std::thread::id, std::string> thread_names;
    static std::mutex thread_names_mux;


    std::string prev_message;
    std::chrono::high_resolution_clock::time_point prev_time;
    size_t message_duplicatecount = 0;



};

#endif // BASICLOGGER_H
