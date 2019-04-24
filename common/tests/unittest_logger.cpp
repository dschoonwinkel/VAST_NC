#include <iostream>
#include "logger.h"
#include <chrono>
#include <thread>

int main()
{
    Logger::debug ("Simple debug");

    Logger::debugPeriodic ("Hello!", std::chrono::milliseconds(100));
    Logger::debugPeriodic ("Hello!", std::chrono::milliseconds(100));
    Logger::debug ("Sleeping for 100 milliseconds");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Logger::debugPeriodic ("Hello!", std::chrono::milliseconds(100));
    //Testing default period
    Logger::debugPeriodic ("Hello!");
    Logger::debugPeriodic ("Hello! but different message", std::chrono::milliseconds(100));
    Logger::info ("Info message: this should always be displayed, even if CPPDEBUG is not defined");

    for(size_t i = 0; i < 10000; i++)
    {
        Logger::debugPeriodic ("Hello! repeated");
    }

}
