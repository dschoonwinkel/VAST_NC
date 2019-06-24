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
        VASTStatLog(std::string filename);

        void nextStep();

        timestamp_t getTimestamp() const;
        timestamp_t getFirstTimestamp() const;
        Node getClientNode() const;
        id_t getSubID() const;
        Node getNeighborByID(Vast::id_t neighbor_id) const;
        int isRelay();
        bool isJoined() const;
        bool isJoinedAt(timestamp_t timestamp) const;

        bool in_view (const VASTStatLog &remote_log) const;
        bool knows (const VASTStatLog &remote_log) const;

        VASTStatLogEntry getEntry() const;

        size_t getNeighborsSize();
        std::vector<Node> getNeighbors();

        int getWorldConnSize();
        StatType getWorldSendStat();
        StatType getWorldRecvStat();

        bool getWorldIsGateway();
        bool getWorldIsMatcher();
        Area getMatcherAOI();

        size_t getRecordedSteps();
        size_t getCurrentStep();

        bool finished() const;

        std::string getFilename();

    private:
        std::vector<Vast::VASTStatLogEntry> _restoredLogs;
        size_t current_step = 0;
        size_t recorded_steps = 0;
        std::string filename = "";
    };

} //end namespace Vast

#endif // VASTSTATLOG_H
