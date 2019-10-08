#ifndef VASTLATENCYSTATLOG_H
#define VASTLATENCYSTATLOG_H

#include "vastlatencystatlog_entry.h"
#include <vector>

namespace Vast {

    class VASTLatencyStatLog
    {
    public:
        VASTLatencyStatLog() {}
        VASTLatencyStatLog(std::vector<VASTLatencyStatLogEntry> entries);
        VASTLatencyStatLog(std::string filename);

        void nextStep();

        timestamp_t getTimestamp() const;
        timestamp_t getFirstTimestamp() const;
        bool isJoinedAt(timestamp_t timestamp) const;
        VASTLatencyStatLogEntry getEntry() const;

        size_t getRecordedSteps();
        size_t getCurrentStep();

        StatType getLatencyStat();

        bool finished() const;

        std::string getFilename();

    private:
        std::vector<Vast::VASTLatencyStatLogEntry> _restoredLogs;
        size_t current_step = 0;
        size_t recorded_steps = 0;
        std::string filename = "";
    };

} //end namespace Vast

#endif // VASTLATENCYSTATLOG_H
