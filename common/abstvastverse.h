#ifndef ABSTVASTVERSE_H
#define ABSTVASTVERSE_H

#include "VAST.h"           // provides spatial publish / subscribe (SPS)

namespace Vast
{
    // NOTE: a general rule: avoid STL objects passing across DLL boundaries
    class AbstVASTVerse
    {
    public:
        virtual ~AbstVASTVerse () {}
        // get current timestamp from host machine (millisecond since 1970)
        virtual timestamp_t getTimestamp () = 0;

        // obtain access to Voronoi class of the matcher (usually for drawing purpose)
        // returns NULL if matcher does not exist on this node
        virtual Voronoi *getMatcherVoronoi () = 0;

        // obtain the matcher's adjustable AOI radius, returns 0 if no matcher exists
        virtual Area *getMatcherAOI () = 0;

        // whether I am a matcher node
        virtual bool isMatcher () = 0;

        // whether I am a gateway node
        virtual bool isGateway () = 0;

        // obtain the number of active connections at this node
        virtual int getConnectionSize () = 0;

        // obtain the tranmission size by message type, default is to return all types
        virtual StatType &getSendStat (bool interval_only = false) = 0;
        virtual StatType &getReceiveStat (bool interval_only = false) = 0;

        // reset stat collection for a particular interval, however, accumulated stat will not be cleared
        virtual void clearStat () = 0;

        // record nodeID on the same host
        virtual void recordLocalTarget (id_t target) = 0;
    };

} // end namespace Vast

#endif // ABSTVASTVERSE_H
