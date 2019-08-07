#ifndef VASTSTATLOG_ENTRY_H
#define VASTSTATLOG_ENTRY_H
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

    class VASTNetStatLogEntry
    {
    public:
        VASTNetStatLogEntry(Vast::id_t nodeID);
        VASTNetStatLogEntry() {}
        //Need copy constructor for vector operations, but must override for unique_ptr's sake
        VASTNetStatLogEntry(const VASTNetStatLogEntry &other);

        ~VASTNetStatLogEntry();

        //This is called every time a MC packet is received
        void recordMCStat (timestamp_t timestamp, StatType raw, StatType used);
        void printStat ();

        bool operator==(const VASTNetStatLogEntry);
        VASTNetStatLogEntry& operator=(const VASTNetStatLogEntry& other);


        //Boost Serialization functions

        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive& ar, const unsigned /*version*/)
        {
            ar & timestamp;
            ar & raw_MCRecvStat;
            ar & used_MCRecvStat;
            ar & _steps_recorded;
        }

        void saveToLogFile();
        void restoreFirstFromLogFile(std::string filename);
        static std::vector<VASTNetStatLogEntry> restoreAllFromLogFile(std::string filename);

        friend std::ostream& operator<<(std::ostream&, VASTNetStatLogEntry const& log);

        friend class VASTNetStatLog;

        //Getters
        timestamp_t getTimestamp() const;
        StatType getRawMCRecvStat() const;
        StatType getUsedMCRecvStat() const;



    protected:
        //The properties marked with a # will be saved to logfile
        timestamp_t timestamp;                      // #
        Vast::id_t _nodeID;                         // #
        StatType raw_MCRecvStat;                        // #
        StatType used_MCRecvStat;                        // #



    private:
        int _steps_recorded;

        std::string _logfilename_base = "./logs/VASTNetStat";
        std::string _logfilename = "./logs/VASTNetStat";
        std::unique_ptr<std::ofstream> pOfs = nullptr;
        std::unique_ptr<boost::archive::text_oarchive> pAr = nullptr;

    };
}   //end namespace Vast

#endif // VASTSTATLOG_ENTRY_H
