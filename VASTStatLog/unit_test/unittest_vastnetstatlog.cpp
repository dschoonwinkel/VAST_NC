#include <iostream>
#include "vastnetstatlog.h"


int main()
{
    std::string filename = "./logs/VASTNetStat_N720575944742207489_0.txt";

    ifstream f(filename);
    if (f.good())
    {
        std::cout << filename << " found" << std::endl;
    }
    else
    {
        std::cout << filename << " not found" << std::endl;
        exit(1);
    }

    Vast::VASTNetStatLog restoredLog(filename);
    std::cout << restoredLog.getCurrentStep() << std::endl;
    std::cout << restoredLog.getRecordedSteps() << std::endl;


    bool finished = false;
    int counter = 0;

    size_t prevRAWMCRecvBytes = 0, prevUsedMCRecvBytes = 0;

    while (!finished)
    {
        finished = restoredLog.finished();
        restoredLog.nextStep();

//        if (counter % 200 == 0)
//            std::cout << counter++ << std::endl;
//        else

        std::cout << "Step " << counter << std::endl;
        std::cout << "RAWMCRecvStat at step: numrecords " << restoredLog.getRawMCRecvStat().num_records << std::endl;
        std::cout << "RAWMCRecvStat at step: min " << restoredLog.getRawMCRecvStat().minimum << std::endl;
        std::cout << "RAWMCRecvStat at step: max " << restoredLog.getRawMCRecvStat().maximum << std::endl;
        std::cout << "RAWMCRecvStat at step: total " << restoredLog.getRawMCRecvStat().total << std::endl;
        std::cout << "RAWMCRecvStat in this step " << restoredLog.getRawMCRecvStat().total - prevRAWMCRecvBytes << std::endl;
        prevRAWMCRecvBytes = restoredLog.getRawMCRecvStat().total;

        std::cout << std::endl;

        std::cout << "UsedMCRecvStat at step: numrecords " << restoredLog.getUsedMCRecvStat().num_records << std::endl;
        std::cout << "UsedMCRecvStat at step: min " << restoredLog.getUsedMCRecvStat().minimum << std::endl;
        std::cout << "UsedMCRecvStat at step: max " << restoredLog.getUsedMCRecvStat().maximum << std::endl;
        std::cout << "UsedMCRecvStat at step: total " << restoredLog.getUsedMCRecvStat().total << std::endl;
        std::cout << "UsedMCRecvStat in this step " << restoredLog.getUsedMCRecvStat().total - prevUsedMCRecvBytes << std::endl;
        prevUsedMCRecvBytes = restoredLog.getUsedMCRecvStat().total;

        std::cout << std::endl;

        counter++;
    }


}
