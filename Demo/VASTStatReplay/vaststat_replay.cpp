#include <iostream>
#include <fstream>
#include "replay_functions.h"

int main() {

    bool finished = false;
    int counter = 0;

    readIniFile ();
    initVariables();

    while (!finished)
    {
        finished = true;
        for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

            finished = finished && allRestoredLogs[logIDs[log_iter]].finished();
        }
        calculateUpdate();
        outputResults();

        if (counter % 200 == 0)
            std::cout << counter++ << std::endl;
        else
            counter++;
    }

    std::cout << "Experiment replay finished" << std::endl;

    closeOutputFile();

    std::cout << "Output file closed" << std::endl;


	return 0;
}
