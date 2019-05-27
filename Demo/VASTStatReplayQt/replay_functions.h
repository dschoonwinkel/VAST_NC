#ifndef REPLAY_FUNCTIONS_H
#define REPLAY_FUNCTIONS_H

#include "vaststatlog.h"
#include <string>

using namespace Vast;

extern std::map<std::string, Vast::VASTStatLog> allRestoredLogs;
extern std::vector<std::string> logIDs;
extern timestamp_t latest_timestamp;

extern size_t total_AN_actual, total_AN_visible, total_drift, max_drift, drift_nodes, total_active_nodes;


void initVariables();
bool readIniFile(const char * filename = "VASTreal.ini");
void calculateUpdate();
size_t calc_consistency (const VASTStatLog &restoredLog, size_t &total_AN_actual,
                       size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                       size_t &drift_nodes, timestamp_t latest_timestamp);
void outputResults();
void closeOutputFile();

#endif // REPLAY_FUNCTIONS_H
