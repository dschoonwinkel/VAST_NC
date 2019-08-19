#include "autodestructortimer.h"

AutoDestructorTimer::AutoDestructorTimer(std::string name, std::chrono::microseconds *totalTime)
    : name(name)
{
//    std::cout << "Starting AutoDestructorTimer:" << name << std::endl;
    t1 = std::chrono::high_resolution_clock::now();
    this->totalTime = totalTime;
}

AutoDestructorTimer::~AutoDestructorTimer()
{
    auto t2 = std::chrono::high_resolution_clock::now();
    internalTimer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);

    if (totalTime != NULL)
    {
        (*totalTime) += internalTimer;
    }

//    std::cout << "~AutoDestructorTimer:" << name << " time spent: " << internalTimer.count() << " microseconds " << std::endl;
}
