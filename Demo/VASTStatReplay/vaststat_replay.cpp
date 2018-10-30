#include <iostream>
#include <fstream>
#include "vaststatlog_entry.h"

int main() {

    std::string filename = "./logs/VASTStat_N9151315546691403777.txt";

//    Vast::VASTStatLog stat1;
//        stat1.restoreFirstFromLogFile(filename);
//        std::cout << stat1;
    std::vector<Vast::VASTStatLogEntry> restoredLogs = Vast::VASTStatLogEntry::restoreAllFromLogFile(filename);

    for (Vast::VASTStatLogEntry log : restoredLogs)
    {
        std::cout << log;
    }

//	std::cout << "Hello world!" << std::endl;
	return 0;
}
