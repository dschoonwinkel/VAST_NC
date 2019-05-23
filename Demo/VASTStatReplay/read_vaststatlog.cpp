#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include "vaststatlog_entry.h"
#include "vaststatlog.h"

using namespace Vast;
using namespace std;

void readLogFile(string filename)
{
    std::vector<Vast::VASTStatLogEntry> restoredLogEntries =
            Vast::VASTStatLogEntry::restoreAllFromLogFile(filename);

    //Cut off .txt
    std::string id_string = filename.substr(0, filename.find(".txt"));
    std::cout << "restoredLogID id_string: " << id_string << std::endl;
    //Extract id_t
    id_string = id_string.substr(id_string.find("N") + 1);
    std::cout << "restoredLogID id_string: " << id_string << std::endl;
    id_string = id_string.substr(0, id_string.find("_"));

    Vast::id_t restoredLogID = stoll(id_string);
    std::cout << "restoredLogID id_string: " << id_string << std::endl;
    std::cout << "restoredLogID: " << restoredLogID << std::endl;

    restoredLogEntries[0].printStat();

    VASTStatLog restoredLog(restoredLogEntries);

    std::cout << "restoredLog.finished(): " << restoredLog.finished() << std::endl;
    std::cout << "restoredLog.getRecordedSteps(): " << restoredLog.getRecordedSteps() << std::endl;
    std::cout << "restoredLog.getCurrentStep(): " << restoredLog.getCurrentStep() << std::endl;

    while (restoredLog.getCurrentStep() + 1 != restoredLog.getRecordedSteps())
    {
        restoredLog.nextStep();
//        std::cout << "restoredLog.finished(): " << restoredLog.finished() << std::endl;
//        std::cout << "restoredLog.getRecordedSteps(): " << restoredLog.getRecordedSteps() << std::endl;
//        std::cout << "restoredLog.getCurrentStep(): " << restoredLog.getCurrentStep() << std::endl;
    }

    std::cout << "restoredLog.finished(): " << restoredLog.finished() << std::endl;
    std::cout << "restoredLog.getRecordedSteps(): " << restoredLog.getRecordedSteps() << std::endl;
    std::cout << "restoredLog.getCurrentStep(): " << restoredLog.getCurrentStep() << std::endl;
    std::cout << "restoredLog.isJoined: " << restoredLog.isJoined() << std::endl;
}

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "No commandline argument supplied, reading all logs" << std::endl;

        path dir_path("./logs");
        directory_iterator end_itr;

        for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

            //Skip subfolders
            if (is_directory(itr->path())) continue;

            CPPDEBUG(itr->path() << std::endl);
            std::string filename = itr->path().string();
            //If this is not a TXT file, it is probably not a VASTStatLog file
            if (filename.find(".txt") == string::npos)
            {
                CPPDEBUG("Skipping " << filename << std::endl);
                continue;
            }

            readLogFile(filename);
        }

    }

    else
    {
        string filename(argv[1]);
        readLogFile(filename);

    }

    return 0;
}
