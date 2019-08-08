#include "vastnetstatlog.h"

namespace Vast {

    VASTNetStatLog::VASTNetStatLog(std::vector<VASTNetStatLogEntry> entries)
    {
        _restoredLogs = entries;
        recorded_steps = _restoredLogs.size();
    }

    VASTNetStatLog::VASTNetStatLog(std::string filename)
    {
        _restoredLogs = VASTNetStatLogEntry::restoreAllFromLogFile(filename);
        recorded_steps = _restoredLogs.size();
        this->filename = filename;
    }

    void VASTNetStatLog::nextStep()
    {
        if (current_step +1 < recorded_steps)
        {
            current_step++;
        }
    }

    VASTNetStatLogEntry VASTNetStatLog::getEntry() const
    {
        return _restoredLogs[current_step];
    }


    //Getters
    timestamp_t VASTNetStatLog::getTimestamp() const
    {
        return getEntry().getTimestamp();
    }

    timestamp_t VASTNetStatLog::getFirstTimestamp() const
    {
        return _restoredLogs[0].getTimestamp();
    }

    bool VASTNetStatLog::isJoinedAt(timestamp_t timestamp) const
    {
        return (getFirstTimestamp() <= timestamp);
    }

    size_t VASTNetStatLog::getRecordedSteps()
    {
        return recorded_steps;
    }

    size_t VASTNetStatLog::getCurrentStep()
    {
        return current_step;
    }

    StatType VASTNetStatLog::getRawMCRecvStat()
    {
        return getEntry().getRawMCRecvStat();
    }

    StatType VASTNetStatLog::getUsedMCRecvStat()
    {
        return getEntry().getUsedMCRecvStat();
    }

    bool VASTNetStatLog::finished() const
    {
//        CPPDEBUG(getClientNode().id << "current_step: " << current_step
//                  << " /" << recorded_steps << std::endl);

        //Check if this is the final step (index off by one)
        return (current_step+1) == recorded_steps;
    }

    std::string VASTNetStatLog::getFilename()
    {
        return filename;
    }

}


