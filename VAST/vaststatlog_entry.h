#ifndef VASTSTATLOG_ENTRY_H
#define VASTSTATLOG_ENTRY_H
#include <map>
#include <VASTTypes.h>

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <string>

namespace Vast {

//    struct NodeDetails {    //Initial details to store about node
//        SimPara       _para;  //Experimental setup struct
//        id_t          _node_no;        // Node number
//    };

    class VAST; //Forward declaration
    class VASTVerse; //Forward declaration

    class VASTStatLogEntry
    {
    public:
        VASTStatLogEntry(VASTVerse *world, VAST *client);
        VASTStatLogEntry() {}
        ~VASTStatLogEntry();

        void recordStat ();
        void printStat ();

        //Implement the serialize functions
        size_t sizeOf ();

        bool operator==(const VASTStatLogEntry);


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
            ar & worldIsGateway;

            ar & _steps_recorded;
        }

        void saveToLogFile(int step, std::string filename);
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


    private:
        long _min_aoi, _total_aoi;
        int _max_CN, _total_CN;
        int _steps_recorded;

        //Used for sourcing data points. Not reconstructed in deserialise
        VAST* _client;                              //
        VASTVerse* _world;                          //

        std::string _logfilename = "./logs/VASTStat";
        std::ofstream *ofs = NULL;
        boost::archive::text_oarchive *ar = NULL;

    };
}   //end namespace Vast

#endif // VASTSTATLOG_ENTRY_H
