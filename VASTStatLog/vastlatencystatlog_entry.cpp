#include "vastlatencystatlog_entry.h"
#include <climits>
#include <stdio.h>

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <stdexcept>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

namespace Vast {

    VASTLatencyStatLogEntry::VASTLatencyStatLogEntry(id_t nodeID)

    {
        _nodeID = nodeID;

        _steps_recorded = 0;

        size_t logfile_count = 0;
       do
        {
           _logfilename = _logfilename_base + "_N" + std::to_string(_nodeID) +
                   "_"+ std::to_string(logfile_count) + ".txt";
           CPPDEBUG("VASTLatencyStatLogEntry: _logfilename: " << _logfilename << std::endl);
           logfile_count++;
        } while (boost::filesystem::exists(_logfilename));

        std::cout << "VASTLatencyStat Logfilename: " << _logfilename << std::endl;

            pOfs = std::make_unique<std::ofstream>(_logfilename);
       if (!pOfs->is_open())
       {
            std::cerr << "VASTLatencyStatLogEntry::constructor file open : " << (pOfs->is_open() ? "true":"false") << std::endl << "EXITING" <<std::endl;
            exit(EXIT_FAILURE);
       }

        pAr = std::make_unique<boost::archive::text_oarchive>(*pOfs);

        latencyStat.reset();

    }

    //Copy constructor
    VASTLatencyStatLogEntry::VASTLatencyStatLogEntry(const VASTLatencyStatLogEntry &other)
    {
        this->timestamp = other.timestamp;
        this->_nodeID = other._nodeID;
        this->latencyStat = other.latencyStat;
    }

    void VASTLatencyStatLogEntry::recordLatencyStat (timestamp_t timestamp, StatType *latency)
    {
        this->timestamp = timestamp;

        if (latency != NULL)
        {
//            CPPDEBUG("VASTLatencyStatLogEntry::recordLatencyStat: latency.total: "
//                     << latency->total << std::endl);
            latencyStat = *latency;
            latencyStat.calculateAverage();
            latency->reset();
            _steps_recorded++;
        }
        else {
            CPPDEBUG("VASTLatencyStatLogEntry::recordLatencyStat: latency was NULL"
                     << std::endl);
            latencyStat.reset();
        }

        saveToLogFile();
    }

    void VASTLatencyStatLogEntry::printStat ()
    {
        printf("VASTLatencyStatLogEntry::printStat: ******************************\n");
        printf("Timestamp %lu\n", timestamp);
        printf("LatencyStat.total %lu\n", latencyStat.total);


        printf("steps_recorded: %d \n", _steps_recorded);
        printf("******************************************************\n");
    }

    void VASTLatencyStatLogEntry::saveToLogFile() {

        this->serialize(*pAr, 0);
        pOfs->flush();

    }

    void VASTLatencyStatLogEntry::restoreFirstFromLogFile(std::string filename) {
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        this->serialize(ar, 0);
    }

    std::vector<VASTLatencyStatLogEntry> VASTLatencyStatLogEntry::restoreAllFromLogFile(std::string filename) {

        std::vector<VASTLatencyStatLogEntry> restoredLogs;
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        while(!ifs.eof()) {
            VASTLatencyStatLogEntry log;
            log.serialize(ar,0);
            restoredLogs.push_back(log);
        }

        return restoredLogs;
    }

    bool VASTLatencyStatLogEntry::operator==(const VASTLatencyStatLogEntry other) {

        bool equals = true;

        equals = equals && this->timestamp == other.timestamp;
        equals = equals && this->getLatencyStat().total == other.getLatencyStat().total;

        return equals;
    }

    VASTLatencyStatLogEntry& VASTLatencyStatLogEntry::operator=(const VASTLatencyStatLogEntry &other)
    {
        this->timestamp = other.timestamp;

        return *this;
    }

    std::ostream& operator<<(std::ostream& output, Vast::VASTLatencyStatLogEntry const& stat )
    {

            output << "VASTLatencyStatLogEntry::stream >> output: ******************************\n";
            output << "Timestamp " <<  stat.timestamp << std::endl;

            output << "steps_recorded: " <<  stat._steps_recorded << std::endl;
            output << "******************************************************" << std::endl;


            return output;
    }


    //Getters
    timestamp_t VASTLatencyStatLogEntry::getTimestamp() const
    {
        return timestamp;
    }

    StatType VASTLatencyStatLogEntry::getLatencyStat() const
    {
        return latencyStat;
    }

    VASTLatencyStatLogEntry::~VASTLatencyStatLogEntry()
    {
        //Do memory free here....
        if (pOfs != NULL)
        {
            pOfs->flush();
            pOfs->close();
        }
    }

}



