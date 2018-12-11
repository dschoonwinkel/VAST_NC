#ifndef REPLAY_FUNCTIONS_H
#define REPLAY_FUNCTIONS_H

#include "vaststatlog.h"

using namespace Vast;

extern std::map<Vast::id_t, Vast::VASTStatLog> allRestoredLogs;
extern std::vector<Vast::id_t> logIDs;
extern timestamp_t latest_timestamp;

extern size_t total_AN_actual, total_AN_visible, total_drift, max_drift, drift_nodes, total_active_nodes;


void initVariables();
void calculateUpdate();
void calc_consistency (const VASTStatLog &restoredLog, size_t &total_AN_actual,
                       size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                       size_t &drift_nodes, timestamp_t latest_timestamp);
void outputResults();
void closeOutputFile();

#endif // REPLAY_FUNCTIONS_H
