#include "vastnetstatlog_entry.h"
#include <climits>
#include <stdio.h>

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <stdexcept>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

namespace Vast {

    VASTNetStatLogEntry::VASTNetStatLogEntry(id_t nodeID)

    {
        _nodeID = nodeID;

        _steps_recorded = 0;

        size_t logfile_count = 0;
       do
        {
           _logfilename = _logfilename_base + "_N" + std::to_string(_nodeID) +
                   "_"+ std::to_string(logfile_count) + ".txt";
           CPPDEBUG("VASTNetStatLogEntry: _logfilename: " << _logfilename << std::endl);
           logfile_count++;
        } while (boost::filesystem::exists(_logfilename));

        std::cout << "VASTNetStat Logfilename: " << _logfilename << std::endl;

            pOfs = std::make_unique<std::ofstream>(_logfilename);
       if (!pOfs->is_open())
       {
            std::cerr << "VASTNetStatLogEntry::constructor file open : " << (pOfs->is_open() ? "true":"false") << std::endl << "EXITING" <<std::endl;
            exit(EXIT_FAILURE);
       }

        pAr = std::make_unique<boost::archive::text_oarchive>(*pOfs);

    }

    //Copy constructor
    VASTNetStatLogEntry::VASTNetStatLogEntry(const VASTNetStatLogEntry &other)
    {
        this->timestamp = other.timestamp;
        this->_nodeID = other._nodeID;
        this->raw_MCRecvStat = other.raw_MCRecvStat;
        this->used_MCRecvStat = other.used_MCRecvStat;
    }

    void VASTNetStatLogEntry::recordMCStat (timestamp_t timestamp, StatType raw, StatType used)
    {
        this->timestamp = timestamp;
        raw_MCRecvStat = raw;
        used_MCRecvStat = used;
        _steps_recorded++;

        saveToLogFile();
    }

    void VASTNetStatLogEntry::printStat ()
    {
        printf("VASTStatLog::printStat: ******************************\n");
        printf("Timestamp %lu\n", timestamp);


        printf("steps_recorded: %d \n", _steps_recorded);
        printf("******************************************************\n");
    }

    void VASTNetStatLogEntry::saveToLogFile() {

        this->serialize(*pAr, 0);
        pOfs->flush();

    }

    void VASTNetStatLogEntry::restoreFirstFromLogFile(std::string filename) {
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        this->serialize(ar, 0);
    }

    std::vector<VASTNetStatLogEntry> VASTNetStatLogEntry::restoreAllFromLogFile(std::string filename) {

        std::vector<VASTNetStatLogEntry> restoredLogs;
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        while(!ifs.eof()) {
            VASTNetStatLogEntry log;
            log.serialize(ar,0);
            restoredLogs.push_back(log);
        }

        return restoredLogs;
    }

    bool VASTNetStatLogEntry::operator==(const VASTNetStatLogEntry other) {

        bool equals = true;

        equals = equals && this->timestamp == other.timestamp;

        return equals;
    }

    VASTNetStatLogEntry& VASTNetStatLogEntry::operator=(const VASTNetStatLogEntry &other)
    {
        this->timestamp = other.timestamp;

        return *this;
    }

    std::ostream& operator<<(std::ostream& output, Vast::VASTNetStatLogEntry const& stat )
    {

            output << "VASTStatLog::stream >> output: ******************************\n";
            output << "Timestamp " <<  stat.timestamp << std::endl;

//            output << "GetSendStat: " <<  stat.worldSendStat.average << std::endl;
//            output << "GetRecvStat: " <<  stat.worldRecvStat.average << std::endl;

            output << "steps_recorded: " <<  stat._steps_recorded << std::endl;
            output << "******************************************************" << std::endl;


            return output;
    }


    //Getters
    timestamp_t VASTNetStatLogEntry::getTimestamp() const
    {
        return timestamp;
    }

    StatType VASTNetStatLogEntry::getRawMCRecvStat() const
    {
        return raw_MCRecvStat;
    }

    StatType VASTNetStatLogEntry::getUsedMCRecvStat() const
    {
        return used_MCRecvStat;
    }

    VASTNetStatLogEntry::~VASTNetStatLogEntry()
    {
        //Do memory free here....
        if (pOfs != NULL)
        {
            pOfs->flush();
            pOfs->close();
        }
    }

}



