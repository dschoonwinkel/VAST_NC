#include "vastlatencystatlog.h"

namespace Vast {

    VASTLatencyStatLog::VASTLatencyStatLog(std::vector<VASTLatencyStatLogEntry> entries)
    {
        _restoredLogs = entries;
        recorded_steps = _restoredLogs.size();
    }

    VASTLatencyStatLog::VASTLatencyStatLog(std::string filename)
    {
        _restoredLogs = VASTLatencyStatLogEntry::restoreAllFromLogFile(filename);
        recorded_steps = _restoredLogs.size();
        this->filename = filename;
    }

    void VASTLatencyStatLog::nextStep()
    {
        if (current_step +1 < recorded_steps)
        {
            current_step++;
        }
    }

    VASTLatencyStatLogEntry VASTLatencyStatLog::getEntry() const
    {
        return _restoredLogs[current_step];
    }


    //Getters
    timestamp_t VASTLatencyStatLog::getTimestamp() const
    {
        return getEntry().getTimestamp();
    }

    timestamp_t VASTLatencyStatLog::getFirstTimestamp() const
    {
        return _restoredLogs[0].getTimestamp();
    }

    bool VASTLatencyStatLog::isJoinedAt(timestamp_t timestamp) const
    {
        return (getFirstTimestamp() <= timestamp);
    }

    size_t VASTLatencyStatLog::getRecordedSteps()
    {
        return recorded_steps;
    }

    size_t VASTLatencyStatLog::getCurrentStep()
    {
        return current_step;
    }

    StatType VASTLatencyStatLog::getLatencyStat()
    {
        return getEntry().getLatencyStat();
    }

    bool VASTLatencyStatLog::finished() const
    {
//        CPPDEBUG(getClientNode().id << "current_step: " << current_step
//                  << " /" << recorded_steps << std::endl);

        //Check if this is the final step (index off by one)
        return (current_step+1) == recorded_steps;
    }

    std::string VASTLatencyStatLog::getFilename()
    {
        return filename;
    }

}


