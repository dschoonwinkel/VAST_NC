#ifndef VASTLATENCYSTATLOG_ENTRY_H
#define VASTLATENCYSTATLOG_ENTRY_H
#include <map>
#include <memory>
#include <VASTTypes.h>
#include "VoronoiSF.h"

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <string>

namespace Vast {

    class VASTLatencyStatLogEntry
    {
    public:
        VASTLatencyStatLogEntry(Vast::id_t nodeID);
        VASTLatencyStatLogEntry() {}
        //Need copy constructor for vector operations, but must override for unique_ptr's sake
        VASTLatencyStatLogEntry(const VASTLatencyStatLogEntry &other);

        ~VASTLatencyStatLogEntry();

        //This is called every time a MC packet is received
        void recordLatencyStat (timestamp_t timestamp, StatType *latency);
        void printStat ();

        bool operator==(const VASTLatencyStatLogEntry);
        VASTLatencyStatLogEntry& operator=(const VASTLatencyStatLogEntry& other);


        //Boost Serialization functions

        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive& ar, const unsigned /*version*/)
        {
            ar & timestamp;
            ar & latencyStat;
            ar & _steps_recorded;
        }

        void saveToLogFile();
        void restoreFirstFromLogFile(std::string filename);
        static std::vector<VASTLatencyStatLogEntry> restoreAllFromLogFile(std::string filename);

        friend std::ostream& operator<<(std::ostream&, VASTLatencyStatLogEntry const& log);

        friend class VASTLatencyStatLog;

        //Getters
        timestamp_t getTimestamp() const;
        StatType getLatencyStat() const;



    protected:
        //The properties marked with a # will be saved to logfile
        timestamp_t timestamp;                      // #
        Vast::id_t _nodeID;                         // #
        StatType latencyStat;                        // #



    private:
        int _steps_recorded;

        std::string _logfilename_base = "./logs/VASTLatencyStat";
        std::string _logfilename = "./logs/VASTLatencyStat";
        std::unique_ptr<std::ofstream> pOfs = nullptr;
        std::unique_ptr<boost::archive::text_oarchive> pAr = nullptr;

    };
}   //end namespace Vast

#endif // VASTLATENCYSTATLOG_ENTRY_H
