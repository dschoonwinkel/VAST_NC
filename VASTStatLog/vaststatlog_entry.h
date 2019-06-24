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
#include "abstvastverse.h"

namespace Vast {

//    struct NodeDetails {    //Initial details to store about node
//        SimPara       _para;  //Experimental setup struct
//        id_t          _node_no;        // Node number
//    };

    class VAST; //Forward declaration

    class VASTStatLogEntry
    {
    public:
        VASTStatLogEntry(Vast::AbstVASTVerse *world, VAST *client);
        VASTStatLogEntry() {}
        //Need copy constructor for vector operations, but must override for unique_ptr's sake
        VASTStatLogEntry(const VASTStatLogEntry &other);

        ~VASTStatLogEntry();

        void recordStat ();
        void printStat ();

//        //Implement the serialize functions
//        size_t sizeOf ();

        bool operator==(const VASTStatLogEntry);
        VASTStatLogEntry& operator=(const VASTStatLogEntry& other);


        //Boost Serialization functions

        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive& ar, const unsigned /*version*/)
        {
            ar & timestamp;

            ar & clientNode;
            ar & clientIsRelay;
            ar & subID;
            ar & clientIsJoined;

            ar & neighbors_size;
            ar & _neighbors;

            ar & worldConnSize;

            ar & worldSendStat;
            ar & worldRecvStat;

            ar & worldIsMatcher;

            if (worldIsMatcher)
            {
                ar &  matcherAOI;
            }

            ar & worldIsGateway;

            ar & _steps_recorded;
        }

        void saveToLogFile();
        void restoreFirstFromLogFile(std::string filename);
        static std::vector<VASTStatLogEntry> restoreAllFromLogFile(std::string filename);

        friend std::ostream& operator<<(std::ostream&, VASTStatLogEntry const& log);

        friend class VASTStatLog;

        //Getters
        timestamp_t getTimestamp() const;
        Node getClientNode();
        Node getNeighborByID(Vast::id_t neighbor_id) const;
        id_t getSubID() const;
        int isRelay() const;
        bool isJoined() const;

        bool in_view (const VASTStatLogEntry &remote_log);
        bool knows (const VASTStatLogEntry &remote_log);

        size_t getNeighborsSize();
        std::vector<Node> getNeighbors();

        int getWorldConnSize();
        StatType getWorldSendStat();
        StatType getWorldRecvStat();

        bool getWorldIsGateway();
        bool getWorldIsMatcher();

        Area getMatcherAOI();



    protected:
        //The properties marked with a # will be saved to logfile
        timestamp_t timestamp;                      // #
        Node clientNode;                            // #
        Vast::id_t subID;                           // #
        int clientIsRelay;                          // #
        size_t neighbors_size;                      // #
        std::vector<Node> _neighbors;              // #
        bool clientIsJoined;                        // #

        int worldConnSize = -1;                     // #
        StatType worldSendStat;                     // #
        StatType worldRecvStat;                     // #
        bool worldIsGateway;                        // #
        bool worldIsMatcher;                        // #
        Area matcherAOI;                            // #
        //Voronoi matcherVoronoi;                   // #


    private:
        long _min_aoi, _total_aoi;
        int _max_CN, _total_CN;
        int _steps_recorded;

        //Used for sourcing data points. Not reconstructed in deserialise
        VAST* _client = NULL;                       //
        AbstVASTVerse* _world = NULL;                          //

        std::string _logfilename_base = "./logs/VASTStat";
        std::string _logfilename = "./logs/VASTStat";
        std::unique_ptr<std::ofstream> pOfs = nullptr;
        std::unique_ptr<boost::archive::text_oarchive> pAr = nullptr;

    };
}   //end namespace Vast

#endif // VASTSTATLOG_ENTRY_H
