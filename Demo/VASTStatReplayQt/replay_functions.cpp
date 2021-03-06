#include <map>
#include <fstream>

#include "VASTVerse.h"
#include "vaststatlog.h"
#include "vastnetstatlog.h"
#include "vastlatencystatlog.h"

#include <climits>

#include "boost/filesystem.hpp"
using namespace boost::filesystem;

using namespace Vast;

SimPara simpara;

std::map<std::string, VASTStatLog> allRestoredLogs;
std::map<std::string, VASTNetStatLog> allRestoredNetStatLogs;
std::map<std::string, VASTLatencyStatLog> allRestoredLatencyStatLogs;
std::vector<std::string> logIDs;
std::vector<std::string> NetStatLogIDs;
std::vector<std::string> LatencyStatLogIDs;
timestamp_t latest_timestamp;

std::string results_file = "./logs/results/results1.txt";
std::ofstream ofs(results_file);

std::string individual_drift = "./logs/results/individual_drift.txt";
std::ofstream drift_distances_file(individual_drift);

std::string latency_results = "./logs/results/results1_latency.txt";
std::ofstream latency_results_file(latency_results);


//Variables needed for calc_consistency
size_t total_AN_actual =0, total_AN_visible =0;
double maxTopoConsistency=0, minTopoConsistency = 100.0;
size_t total_drift =0, max_drift =0, drift_nodes =0, total_active_nodes =0;
size_t total_active_matchers =0;
long worldSendStat =0, worldRecvStat = 0;
long prevWorldSendStat = 0, prevWorldRecvStat = 0;


//NetStat variables
bool hasNetStat = false;
long RawMCRecvBytes = 0, UsedMCRecvBytes = 0;
long prevRawMCRecvBytes = 0, prevUsedMCRecvBytes = 0;


//LatencyStat variables
size_t total_latency_nodes = 0;
size_t total_latency_records = 0;
bool hasLatencyStat = false;
long LatencyTotal = 0;
long minLatency = 0;
long maxLatency = 0;
long prevLatencyTotal = 0;


map <int, size_t> drift_distances;
int g_MS_PER_TIMESTEP;

void initVariables();
void readIniFile();
void calculateUpdate();
void calculateLatencyUpdate();
size_t calc_consistency (const VASTStatLog &restoredLog, size_t &total_AN_actual,
                       size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                       size_t &drift_nodes, timestamp_t latest_timestamp);
void outputResults();
void closeOutputFile();


bool readIniFile(const char* filename /*VASTreal.ini*/)
{
    printf("readIniFile called\n");
    FILE *fp;
    if ((fp = fopen (filename, "rt")) == NULL)
        return false;

    int *p[] = {
        &simpara.VAST_MODEL,
        &simpara.NET_MODEL,
        &simpara.MOVE_MODEL,
        &simpara.WORLD_WIDTH,
        &simpara.WORLD_HEIGHT,
        &simpara.NODE_SIZE,
        &simpara.RELAY_SIZE,
        &simpara.MATCHER_SIZE,
        &simpara.TIME_STEPS,
        &simpara.STEPS_PERSEC,
        &simpara.AOI_RADIUS,
        &simpara.AOI_BUFFER,
        &simpara.CONNECT_LIMIT,
        &simpara.VELOCITY,
        &simpara.STABLE_SIZE,
        &simpara.JOIN_RATE,
        &simpara.LOSS_RATE,
        &simpara.FAIL_RATE,
        &simpara.UPLOAD_LIMIT,
        &simpara.DOWNLOAD_LIMIT,
        &simpara.PEER_LIMIT,
        &simpara.RELAY_LIMIT,
        &simpara.OVERLOAD_LIMIT,
        &simpara.TIMESTEP_DURATION,
        0
    };

    char buff[255];
    int n = 0;
    while (fgets (buff, 255, fp) != NULL)
    {
        // skip any comments or empty lines
        if (buff[0] == '#' || buff[0] == ' ' || buff[0] == '\n')
            continue;

        // read the next valid parameter
        if (sscanf (buff, "%d ", p[n]) != 1)
            return false;
        n++;

        //Null Pointer, i.e. ending of the struct?
        if (p[n] == 0)
            return true;
    }

    return false;

}



void initVariables()
{
    path dir_path("./logs");
    directory_iterator end_itr;

    //VASTStatLog_id.txt
    CPPDEBUG("Processing normal VASTStatLog" << std::endl);
    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

        //Skip subfolders
        if (is_directory(itr->path())) continue;

        CPPDEBUG(itr->path() << std::endl);
        std::string filename = itr->path().string();
        //If this is not a TXT file, it is probably not a VASTStatLog file
        if (filename.find(".txt") == string::npos)
        {
            CPPDEBUG("Skipping " << filename << std::endl);
            continue;
        }

        else if (filename.find("VASTStat") == string::npos)
        {
            CPPDEBUG("NetStat or LatencyStat file, skipping " << filename << std::endl);
            continue;
        }

        VASTStatLog restoredLog(filename);

        allRestoredLogs[filename] = restoredLog;
        logIDs.push_back(filename);

        latest_timestamp = ULONG_LONG_MAX;
        //Initiate latest_timestamp to the earliest timestamp
        for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++)
        {
          CPPDEBUG("Starting timestamp: " << allRestoredLogs[logIDs[log_iter]].getTimestamp() << std::endl);
          if (allRestoredLogs[logIDs[log_iter]].getTimestamp() < latest_timestamp)
          {
              latest_timestamp = allRestoredLogs[logIDs[log_iter]].getTimestamp();
          }


        }

    }

    std::cout << std::endl;

    //VASTNetStatLog_id.txt
    CPPDEBUG("Processing VASTNetStatLog" << std::endl);
    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

        //Skip subfolders
        if (is_directory(itr->path())) continue;

        CPPDEBUG(itr->path() << std::endl);
        std::string filename = itr->path().string();
        //If this is not a TXT file, it is probably not a VASTNetStatLog file
        if (filename.find(".txt") == string::npos)
        {
            CPPDEBUG("Skipping " << filename << std::endl);
            continue;
        }

        else if (filename.find("Net") == string::npos)
        {
            CPPDEBUG("normal Stat file, skipping " << filename << std::endl);
            continue;
        }

        VASTNetStatLog restoredLog(filename);

        allRestoredNetStatLogs[filename] = restoredLog;
        NetStatLogIDs.push_back(filename);

    }

    ofs << "timestamp," << "active_nodes," << "active_matchers," << "AN_actual," << "AN_visible,"
        << "Total drift," << "Max drift," << "drift nodes," << "worldSendStat," << "worldRecvStat,"
        << "rawMCRecvBytes," << "usedMCRecvBytes," << "maxTopoConsistency," << "minTopoConsistency" << std::endl;




    //VASTLatencyStatLog_id.txt
    CPPDEBUG("Processing VASTLatencyStatLog" << std::endl);
    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

        //Skip subfolders
        if (is_directory(itr->path())) continue;

        CPPDEBUG(itr->path() << std::endl);
        std::string filename = itr->path().string();
        //If this is not a TXT file, it is probably not a VASTLatencyStatLog file
        if (filename.find(".txt") == string::npos)
        {
            CPPDEBUG("Skipping " << filename << std::endl);
            continue;
        }

        else if (filename.find("Latency") == string::npos)
        {
            CPPDEBUG("normal Stat file, skipping " << filename << std::endl);
            continue;
        }

        VASTLatencyStatLog restoredLog(filename);

        allRestoredLatencyStatLogs[filename] = restoredLog;
        LatencyStatLogIDs.push_back(filename);

    }

    latency_results_file << "timestamp," << "total_records," << "MOVElatency," << "maxLatency," << "minLatency,"<< std::endl;



    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++)
    {
        drift_distances_file << "Node " << logIDs[log_iter] << ",";
    }
    drift_distances_file << std::endl;
    g_MS_PER_TIMESTEP = simpara.TIMESTEP_DURATION;
}

void calculateUpdate()
{

    //Drift distance and topology consistency
    total_AN_actual =0, total_AN_visible =0, total_drift =0, max_drift =0, drift_nodes =0;
    worldSendStat = 0, worldRecvStat = 0;

    long tempWorldSendStat = 0, tempWorldRecvStat = 0;
    long tempRawMCRecvBytes = 0, tempUsedMCRecvBytes = 0;

    total_active_nodes =0;
    total_active_matchers=0;

    maxTopoConsistency = 0;
    minTopoConsistency = 100;


    //VASTStatLog approach - instead of working with vectors of entries
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        VASTStatLog &restoredLog = allRestoredLogs[logIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
        {
            continue;
        }

        //Get client node state
        Node node = restoredLog.getClientNode();

//        CPPDEBUG("MainWindow::calcUpdate:: timestamp: " << restoredLog.getTimestamp() << std::endl);
//        CPPDEBUG("MainWindow::calcUpdate:: latest timestamp: " << latest_timestamp << std::endl);

        //Allow each log to catch up to the current timestamp
        while (restoredLog.getTimestamp() < latest_timestamp && !restoredLog.finished())
        {
            restoredLog.nextStep();
            node = restoredLog.getClientNode();
        }

        if (restoredLog.isJoinedAt(latest_timestamp))
        {
            drift_distances[log_iter] = calc_consistency(restoredLog, total_AN_actual, total_AN_visible, total_drift, max_drift, drift_nodes, latest_timestamp);
//            worldSendStat += restoredLog.getWorldSendStat().average;
            tempWorldSendStat += restoredLog.getWorldSendStat().total;
            tempWorldRecvStat += restoredLog.getWorldRecvStat().total;

            //Count the number of active nodes at the moment
            total_active_nodes++;
            if (restoredLog.getWorldIsMatcher())
            {
                total_active_matchers++;
            }
        }

    }

    if (tempWorldSendStat > prevWorldSendStat)
    {
        worldSendStat = tempWorldSendStat - prevWorldSendStat;
        worldRecvStat = tempWorldRecvStat - prevWorldRecvStat;
    }

    if (worldSendStat > 100000 )
    {
        std::cout << "World send stat is very large" << std::endl;
        std::cout << "worldSendStat" << worldSendStat << std::endl;
        std::cout << "tempWorldSendStat" << tempWorldSendStat << std::endl;
        std::cout << "prevWorldSendStat" << prevWorldSendStat << std::endl;
    }

    if (worldSendStat < 0)
    {
        std::cout << "World send stat is negative" << std::endl;
        std::cout << "worldSendStat" << worldSendStat << std::endl;
        std::cout << "tempWorldSendStat" << tempWorldSendStat << std::endl;
        std::cout << "prevWorldSendStat" << prevWorldSendStat << std::endl;

    }
    prevWorldSendStat = tempWorldSendStat;
    prevWorldRecvStat = tempWorldRecvStat;



    for (size_t log_iter = 0; log_iter < NetStatLogIDs.size(); log_iter++) {

        VASTNetStatLog &restoredLog = allRestoredNetStatLogs[NetStatLogIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
        {
            continue;
        }

        //Allow each log to catch up to the current timestamp
        while (restoredLog.getTimestamp() < latest_timestamp && !restoredLog.finished())
        {
            restoredLog.nextStep();
        }

        if (restoredLog.isJoinedAt(latest_timestamp))
        {
            tempRawMCRecvBytes += restoredLog.getRawMCRecvStat().total;
            tempUsedMCRecvBytes += restoredLog.getUsedMCRecvStat().total;
        }
    }

    if (tempRawMCRecvBytes > prevRawMCRecvBytes)
    {
        RawMCRecvBytes = tempRawMCRecvBytes - prevRawMCRecvBytes;
        UsedMCRecvBytes = tempUsedMCRecvBytes - prevUsedMCRecvBytes;
    }

    if (RawMCRecvBytes > 1000000 )
    {
        std::cout << "\nRawMCRecvBytes stat is very large" << std::endl;
        std::cout << "RawMCRecvBytes " << RawMCRecvBytes << std::endl;
        std::cout << "tempRawMCRecvBytes " << tempRawMCRecvBytes << std::endl;
        std::cout << "prevRawMCRecvBytes " << prevRawMCRecvBytes << std::endl;
    }

    if (UsedMCRecvBytes > 100000)
    {
        std::cout << "\nUsedMCRecvBytes stat is very large" << std::endl;
        std::cout << "UsedMCRecvBytes " << UsedMCRecvBytes << std::endl;
        std::cout << "tempUsedMCRecvBytes " << tempUsedMCRecvBytes << std::endl;
        std::cout << "prevUsedMCRecvBytes " << prevUsedMCRecvBytes << std::endl;

    }
    prevRawMCRecvBytes = tempRawMCRecvBytes;
    prevUsedMCRecvBytes = tempUsedMCRecvBytes;

    calculateLatencyUpdate();

    if (total_AN_actual == 0)
    {
        maxTopoConsistency = 100;
    }

    latest_timestamp += simpara.TIMESTEP_DURATION;

}

void calculateLatencyUpdate()
{
//    long tempLatency = 0;
    LatencyTotal = 0;
    total_latency_records = 0;
    total_latency_nodes = 0;
    maxLatency = 0;
    minLatency = 100000;


    //LatencyStat
    for (size_t log_iter = 0; log_iter < LatencyStatLogIDs.size(); log_iter++) {

        VASTLatencyStatLog &restoredLog = allRestoredLatencyStatLogs[LatencyStatLogIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
        {
            continue;
        }

        //Allow each log to catch up to the current timestamp
        while (restoredLog.getTimestamp() < latest_timestamp && !restoredLog.finished())
        {
            restoredLog.nextStep();
        }

        if (restoredLog.isJoinedAt(latest_timestamp))
        {
            if (restoredLog.getLatencyStat().average > 10000)
            {
                CPPDEBUG("Latency Stat very large: " << restoredLog.getLatencyStat().average << std::endl);
            }
            else if (restoredLog.getLatencyStat().num_records == 0)
            {
                CPPDEBUG("Latency Stat Did not contain any records" << std::endl);
            }
            else {
                LatencyTotal += restoredLog.getLatencyStat().total;
                total_latency_records += restoredLog.getLatencyStat().num_records;
                total_latency_nodes++;
                if (restoredLog.getLatencyStat().maximum > maxLatency)
                {
                    maxLatency = restoredLog.getLatencyStat().maximum;
                }
                if (restoredLog.getLatencyStat().minimum < minLatency)
                {
                    minLatency = restoredLog.getLatencyStat().minimum;
                }
            }


        }
    }

//    std::cout << "timestamp: " << latest_timestamp << "Temp Latency: " << tempLatency << std::endl;


//    if (tempLatency > prevLatencyTotal)
//    {
//        LatencyTotal = tempLatency - prevLatencyTotal;
//    }

    if (LatencyTotal > 100000 )
    {
        std::cout << "\nLatency stat is very large" << std::endl;
        std::cout << "Latency " << LatencyTotal << std::endl;
//        std::cout << "tempLatency " << tempLatency << std::endl;
//        std::cout << "prevLatencyTotal " << prevLatencyTotal << std::endl;
    }
    if (total_active_nodes > 0)
    {
//        CPPDEBUG("Normalized latency: " << LatencyTotal / total_active_nodes << " active nodes " << total_active_nodes << std::endl);
    }

//    prevLatencyTotal = tempLatency;

}

size_t calc_consistency (const Vast::VASTStatLog &restoredLog, size_t &total_AN_actual,
                                   size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                                   size_t &drift_nodes, timestamp_t latest_timestamp)
{
//    AN_actual = AN_visible = total_drift = max_drift = drift_nodes = 0;
    size_t node_drift = 0;
    // loop through all nodes
    size_t local_AN_actual = 0, local_AN_visible  = 0;

    for (size_t j=0; j< allRestoredLogs.size (); ++j)
    {
        const VASTStatLog &otherLog = allRestoredLogs[logIDs[j]];

        // skip self-check or failed / not yet joined node
        if (restoredLog.getClientNode().id == otherLog.getClientNode().id || otherLog.isJoinedAt(latest_timestamp) == false)
        {
            continue;
        }

        // visible neighbors
        if (restoredLog.in_view (otherLog))
        {
            total_AN_actual++;
            local_AN_actual++;

            if (restoredLog.knows (otherLog))
            {
                total_AN_visible++;
                local_AN_visible++;

                // calculate drift distance (except self)
                // NOTE: drift distance is calculated for all known AOI neighbors
                drift_nodes++;

                size_t drift = (size_t)restoredLog.getNeighborByID(otherLog.getSubID()).aoi.center.distance (otherLog.getClientNode().aoi.center);
                total_drift += drift;

                //What is the drift of this individual node?
                node_drift += drift;

                if (max_drift < drift)
                {
                    max_drift = drift;
#ifdef DEBUG_DETAIL
                    printf ("%4d - max drift updated: [%d] info on [%d] drift: %d\n", _steps+1, (int)_simnodes[i]->getID (), (int)neighbor->id, (int)drift);
#endif
                }
            }
        }
    } // end looping through all other nodes

    if (local_AN_actual > 0)
    {
        double localTopoConsistency = 100.0 * local_AN_visible / local_AN_actual;
        CPPDEBUG("localTopoConsistency: AN visible (" << local_AN_visible <<
                 ") / AN actual (" << local_AN_actual << ") = "
                 << localTopoConsistency << std::endl);
        if (localTopoConsistency >= maxTopoConsistency)
        {
            maxTopoConsistency = localTopoConsistency;
        }
        if (localTopoConsistency <= minTopoConsistency)
        {
            minTopoConsistency = localTopoConsistency;
        }
    }

    return node_drift;

}

void outputResults() {

    ofs << latest_timestamp << "," << total_active_nodes << "," << total_active_matchers << "," << total_AN_actual <<
           "," << total_AN_visible << "," << total_drift << "," << max_drift << ","
        << drift_nodes << "," << worldSendStat << "," << worldRecvStat << ","
        << RawMCRecvBytes << "," << UsedMCRecvBytes << "," << maxTopoConsistency << "," << minTopoConsistency << std::endl;

    //Save the latency results in a different file
    latency_results_file << latest_timestamp << "," << total_latency_records << "," << LatencyTotal << "," << maxLatency << "," << minLatency << std::endl;


    //Save the individual drift distances seperately
    drift_distances_file << latest_timestamp << ",";
    for (size_t log_iter = 0; log_iter < allRestoredLogs.size(); log_iter++)
    {
        drift_distances_file << drift_distances[log_iter];
        if (log_iter < allRestoredLogs.size() - 1)
            drift_distances_file << ",";
    }
    drift_distances_file << std::endl;

}

void closeOutputFile()
{
    ofs.flush();
    ofs.close();

    latency_results_file.flush();
    latency_results_file.close();

    drift_distances_file.flush();
    drift_distances_file.close();
}
