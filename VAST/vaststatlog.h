#ifndef VASTSTATLOG_H
#define VASTSTATLOG_H

#include "vaststatlog_entry.h"
#include <vector>

namespace Vast {

    class VASTStatLog
    {
    public:
        VASTStatLog() {}
        VASTStatLog(std::vector<VASTStatLogEntry> entries);

        void nextStep();

        timestamp_t getTimestamp();
        timestamp_t getFirstTimestamp();
        Node getClientNode();
        int isRelay();
        bool isJoined();

        bool in_view (VASTStatLogEntry &remote_log);
        bool knows (VASTStatLogEntry &remote_log);

        VASTStatLogEntry getEntry();

        size_t getNeighborsSize();
        std::vector<Node> getNeighbors();

        int getWorldConnSize();
        StatType getWorldSendStat();
        StatType getWorldRecvStat();

        bool getWorldIsGateway();
        bool getWorldIsMatcher();

        size_t getRecordedSteps();

        bool finished();

    private:
        std::vector<Vast::VASTStatLogEntry> _restoredLogs;
        size_t current_step = 0;
        size_t recorded_steps = 0;
    };

} //end namespace Vast

#endif // VASTSTATLOG_H
