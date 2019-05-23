#include "vaststatlog.h"

namespace Vast {

    VASTStatLog::VASTStatLog(std::vector<VASTStatLogEntry> entries)
    {
        _restoredLogs = entries;
        recorded_steps = _restoredLogs.size();
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
    VASTStatLog::in_view (const VASTStatLog &remote_log) const
    {
        return (getEntry().in_view(remote_log.getEntry()) && remote_log.getFirstTimestamp() <= getTimestamp());
    }

    // returns true if known
    bool
    VASTStatLog::knows (const VASTStatLog &remote_log) const
    {
        return (getEntry().knows(remote_log.getEntry()) && remote_log.getFirstTimestamp() <= getTimestamp());
    }

    VASTStatLogEntry VASTStatLog::getEntry() const
    {
        return _restoredLogs[current_step];
    }


    //Getters
    timestamp_t VASTStatLog::getTimestamp() const
    {
        return getEntry().getTimestamp();
    }

    timestamp_t VASTStatLog::getFirstTimestamp() const
    {
        return _restoredLogs[0].getTimestamp();
    }

    int VASTStatLog::isRelay()
    {
        return getEntry().isRelay();
    }

    bool VASTStatLog::isJoined() const
    {
        return getEntry().isJoined() && current_step >0 && !finished();
    }

    bool VASTStatLog::isJoinedAt(timestamp_t timestamp) const
    {
        return (isJoined() && getFirstTimestamp() <= timestamp);
    }


    Node VASTStatLog::getClientNode() const
    {
        return getEntry().getClientNode();
    }

    id_t VASTStatLog::getSubID() const
    {
        return getEntry().getSubID();
    }

    Node VASTStatLog::getNeighborByID(Vast::id_t neighbor_id) const
    {
        return getEntry().getNeighborByID(neighbor_id);
    }

    size_t VASTStatLog::getNeighborsSize()
    {
        return getEntry().getNeighborsSize();
    }
    std::vector<Node> VASTStatLog::getNeighbors()
    {
        return getEntry().getNeighbors();
    }

    int VASTStatLog::getWorldConnSize()
    {
        return getEntry().getWorldConnSize();
    }

    StatType VASTStatLog::getWorldSendStat()
    {
        return getEntry().getWorldSendStat();
    }
    StatType VASTStatLog::getWorldRecvStat()
    {
        return getEntry().getWorldRecvStat();
    }

    bool VASTStatLog::getWorldIsGateway()
    {
        return getEntry().getWorldIsGateway();
    }
    bool VASTStatLog::getWorldIsMatcher()
    {
        return getEntry().getWorldIsMatcher();
    }

    Area* VASTStatLog::getMatcherAOI()
    {
        return getEntry().getMatcherAOI();
    }

    size_t VASTStatLog::getRecordedSteps()
    {
        return recorded_steps;
    }

    size_t VASTStatLog::getCurrentStep()
    {
        return current_step;
    }

    bool VASTStatLog::finished() const
    {
//        CPPDEBUG(getClientNode().id << "current_step: " << current_step
//                  << " /" << recorded_steps << std::endl);

        //Check if this is the final step (index off by one)
        return (current_step+1) == recorded_steps;
    }

}


