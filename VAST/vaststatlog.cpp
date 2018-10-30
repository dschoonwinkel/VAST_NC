#include "vaststatlog.h"

namespace Vast {

    VASTStatLog::VASTStatLog(std::vector<VASTStatLogEntry> entries)
    {
        _restoredLogs = entries;
    }

    void VASTStatLog::nextStep()
    {
        if (current_step +1 < recorded_steps)
        {
            current_step++;
        }
    }

    // distance to a point
    bool
    VASTStatLog::in_view (VASTStatLogEntry &remote_log)
    {
        return (getEntry().clientNode.aoi.center.distance (remote_log.clientNode.aoi.center) < (double)getEntry().clientNode.aoi.radius);
    }

    // returns true if known
    bool
    VASTStatLog::knows (VASTStatLogEntry &remote_log)
    {
        // see if 'node' is a known neighbor of me
        for (size_t i = 0; i < getEntry()._neighbors.size(); i++)
        {
            if (getEntry()._neighbors[i].id == remote_log.clientNode.id)
                return true;
        }
        return false;
    }

    VASTStatLogEntry VASTStatLog::getEntry()
    {
        return _restoredLogs[current_step];
    }


    //Getters
    timestamp_t VASTStatLog::getTimestamp()
    {
        return getEntry().timestamp;
    }

    int VASTStatLog::isRelay()
    {
        return getEntry().clientIsRelay;
    }

    bool VASTStatLog::isJoined()
    {
        return getEntry().clientIsJoined;
    }


    Node VASTStatLog::getClientNode()
    {
        return getEntry().clientNode;
    }

    size_t VASTStatLog::getNeighborsSize()
    {
        return getEntry().neighbors_size;
    }
    std::vector<Node> VASTStatLog::getNeighbors()
    {
        return getEntry()._neighbors;
    }

    int VASTStatLog::getWorldConnSize()
    {
        return getEntry().worldConnSize;
    }

    StatType VASTStatLog::getWorldSendStat()
    {
        return getEntry().worldSendStat;
    }
    StatType VASTStatLog::getWorldRecvStat()
    {
        return getEntry().worldRecvStat;
    }

    bool VASTStatLog::getWorldIsGateway()
    {
        return getEntry().worldIsGateway;
    }
    bool VASTStatLog::getWorldIsMatcher()
    {
        return getEntry().worldIsMatcher;
    }

}


