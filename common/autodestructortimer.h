#ifndef AUTODESTRUCTORTIMER_H
#define AUTODESTRUCTORTIMER_H

#include <chrono>
#include <iostream>
#include <string>
#include "Config.h"

class AutoDestructorTimer
{
public:
    AutoDestructorTimer(std::string name, std::chrono::microseconds *totalTime = NULL);
    ~AutoDestructorTimer();

private:
    std::chrono::microseconds internalTimer = std::chrono::microseconds::zero();
    std::chrono::high_resolution_clock::time_point t1;
    std::string name;

    std::chrono::microseconds *totalTime = NULL;
};

#endif //AUTODESTRUCTORTIMER_H
