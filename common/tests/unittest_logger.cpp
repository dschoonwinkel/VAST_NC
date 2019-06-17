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


    for(size_t i = 0; i < 100000; i++)
    {
        if (i % 10000 == 0)
            Logger::debug("Debug message with timestamp", true);
    }

    unsigned char array[] = {0x01, 0x02, 0x03, 0x04, 0x05,
                             0x06, 0x07, 0x08, 0x09, 0x0A,
                             0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                             0x10, 0x11, 0x12, 0x01, 0x02,
                             0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C,
                             0x0D, 0x0E, 0x0F, 0x10, 0x11,
                             0x12};

    std::cout << Logger::printArray(reinterpret_cast<char*>(array), 36) << std::endl;
    Logger::saveBinaryArray("123.txt", array, 36);
}
