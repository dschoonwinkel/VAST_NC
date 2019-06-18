#include <iostream>
#include <thread>
#include "autodestructortimer.h"

int main()
{
    AutoDestructorTimer timer("unittest");
    std::this_thread::sleep_for (std::chrono::milliseconds(200));
}
