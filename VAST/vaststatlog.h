#ifndef VASTSTATLOG_H
#define VASTSTATLOG_H
#include <map>
#include <VASTTypes.h>

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

        void recordStat ();
        void printStat ();

        //Implement the serialize functions
        size_t sizeOf ();
        size_t serialize (char *buffer);
        size_t deserialize (const char *buffer, size_t size);

        bool operator==(const VASTStatLog);


    private:
        //The properties marked with a # will be saved to logfile
        timestamp_t timestamp;                      // #
        Node * clientNode;                          // #
        bool clientIsRelay;                         // #
        std::map<Vast::id_t, Node*> _neighbors;     // #

        int worldConnSize = -1;                     // #
        StatType worldSendStat;                     // #
        StatType worldRecvStat;                     // #
        bool worldIsGateway;                        // #


        long _min_aoi, _total_aoi;
        int _max_CN, _total_CN;
        int _steps_recorded;

        //Used for sourcing data points. Not reconstructed in deserialise
        VAST* _client;                              //
        VASTVerse* _world;                          //

    };
}   //end namespace Vast

#endif // VASTSTATLOG_H
