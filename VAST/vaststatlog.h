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
        Node getClientNode() const;
        id_t getSubID() const;
        Node getNeighborByID(Vast::id_t neighbor_id) const;
        int isRelay();
        bool isJoined() const;

        bool in_view (const VASTStatLogEntry &remote_log) const;
        bool knows (const VASTStatLogEntry &remote_log) const;

        VASTStatLogEntry getEntry() const;

        size_t getNeighborsSize();
        std::vector<Node> getNeighbors();

        int getWorldConnSize();
        StatType getWorldSendStat();
        StatType getWorldRecvStat();

        bool getWorldIsGateway();
        bool getWorldIsMatcher();

        size_t getRecordedSteps();

        bool finished() const;

    private:
        std::vector<Vast::VASTStatLogEntry> _restoredLogs;
        size_t current_step = 0;
        size_t recorded_steps = 0;
    };

} //end namespace Vast

#endif // VASTSTATLOG_H
