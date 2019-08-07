#ifndef VASTSTATLOG_H
#define VASTSTATLOG_H

#include "vastnetstatlog_entry.h"
#include <vector>

namespace Vast {

    class VASTNetStatLog
    {
    public:
        VASTNetStatLog() {}
        VASTNetStatLog(std::vector<VASTNetStatLogEntry> entries);
        VASTNetStatLog(std::string filename);

        void nextStep();

        timestamp_t getTimestamp() const;
        timestamp_t getFirstTimestamp() const;
        VASTNetStatLogEntry getEntry() const;

        size_t getRecordedSteps();
        size_t getCurrentStep();

        StatType getRawMCRecvStat();
        StatType getUsedMCRecvStat();

        bool finished() const;

        std::string getFilename();

    private:
        std::vector<Vast::VASTNetStatLogEntry> _restoredLogs;
        size_t current_step = 0;
        size_t recorded_steps = 0;
        std::string filename = "";
    };

} //end namespace Vast

#endif // VASTSTATLOG_H
