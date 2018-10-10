#ifndef VASTSTATLOG_H
#define VASTSTATLOG_H
#include <map>
#include <VASTTypes.h>

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>

namespace Vast {

    struct NodeDetails {    //Initial details to store about node
        SimPara       _para;  //Experimental setup struct
        id_t          _node_no;        // Node number
    };

    class VAST; //Forward declaration
    class VASTVerse; //Forward declaration

    class VASTStatLog
    {
    public:
        VASTStatLog(VASTVerse *world, VAST *client);
        ~VASTStatLog();

        void recordStat ();
        void printStat ();

        //Implement the serialize functions
        size_t sizeOf ();
//        size_t serialize (char *buffer);
//        size_t deserialize (const char *buffer, size_t size);

        bool operator==(const VASTStatLog);


        //Boost Serialization functions

        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive& ar, const unsigned /*version*/)
        {
            ar & timestamp;
        }

        void openLogFile();
        void restoreFromLogFile(string filename);

        friend std::ostream& operator<<(std::ostream&, VASTStatLog const& log);


    private:
        //The properties marked with a # will be saved to logfile
        timestamp_t timestamp;                      // #
        Node clientNode;                            // #
        int clientIsRelay;                          // #
        size_t neighbors_size;                      // #
        std::vector<Node*> _neighbors;              // #

        int worldConnSize = -1;                     // #
        StatType worldSendStat;                     // #
        StatType worldRecvStat;                     // #
        bool worldIsGateway;                        // #
        bool worldIsMatcher;                        // #

        long _min_aoi, _total_aoi;
        int _max_CN, _total_CN;
        int _steps_recorded;

        //Used for sourcing data points. Not reconstructed in deserialise
        VAST* _client;                              //
        VASTVerse* _world;                          //

    };
}   //end namespace Vast

#endif // VASTSTATLOG_H
