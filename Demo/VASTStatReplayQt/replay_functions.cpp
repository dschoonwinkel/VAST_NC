#include <map>
#include <fstream>

#include "VASTVerse.h"
#include "vaststatlog.h"

#include <climits>

#include "boost/filesystem.hpp"
using namespace boost::filesystem;

using namespace Vast;

SimPara simpara;
VASTPara_Net netpara(VAST_NET_EMULATED);
map <int, vector<Node *> *> nodes;

std::map<Vast::id_t, VASTStatLog> allRestoredLogs;
std::vector<Vast::id_t> logIDs;
timestamp_t latest_timestamp;

std::string results_file = "./logs/results/results1.txt";
std::ofstream ofs(results_file);


//Variables needed for calc_consistency
size_t total_AN_actual =0, total_AN_visible =0, total_drift =0, max_drift =0, drift_nodes =0, total_active_nodes =0;
long worldSendStat =0, worldRecvStat = 0;
long prevWorldSendStat = 0, prevWorldRecvStat = 0;

#define UPDATE_PERIOD 10

void initVariables();
void calculateUpdate();
void calc_consistency (const VASTStatLog &restoredLog, size_t &total_AN_actual,
                       size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                       size_t &drift_nodes, timestamp_t latest_timestamp);
void outputResults();
void closeOutputFile();





void initVariables()
{
    path dir_path("./logs");
    directory_iterator end_itr;

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
        std::vector<Vast::VASTStatLogEntry> restoredLogs = Vast::VASTStatLogEntry::restoreAllFromLogFile(filename);
        VASTStatLog restoredLog(restoredLogs);

        //Cut off .txt
        std::string id_string = filename.substr(0, filename.find(".txt"));
        //Extract id_t
        id_string = id_string.substr(id_string.find("N") + 1);

        Vast::id_t restoredLogID = stoll(id_string);
        allRestoredLogs[restoredLogID] = restoredLog;
        logIDs.push_back(restoredLogID);

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

    ofs << "timestamp," << "active_nodes," << "AN_actual," << "AN_visible,"
        << "Total drift," << "Max drift," << "drift nodes," << "worldSendStat," << "worldRecvStat," << std::endl;
}

void calculateUpdate()
{

    //Drift distance and topology consistency
    total_AN_actual =0, total_AN_visible =0, total_drift =0, max_drift =0, drift_nodes =0;
    worldSendStat = 0, worldRecvStat = 0;

    size_t tempWorldSendStat = 0, tempWorldRecvStat = 0;

    total_active_nodes =0;


    //VASTStatLog approach - instead of working with vectors of entries
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        VASTStatLog &restoredLog = allRestoredLogs[logIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
            continue;

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
            calc_consistency(restoredLog, total_AN_actual, total_AN_visible, total_drift, max_drift, drift_nodes, latest_timestamp);
//            worldSendStat += restoredLog.getWorldSendStat().average;
            tempWorldSendStat += restoredLog.getWorldSendStat().total;
            tempWorldRecvStat += restoredLog.getWorldRecvStat().total;

            //Count the number of active nodes at the moment
            total_active_nodes++;
        }



    }

    if (tempWorldSendStat > prevWorldSendStat)
    {
        worldSendStat = tempWorldSendStat - prevWorldSendStat;
        worldRecvStat = tempWorldRecvStat - prevWorldRecvStat;
    }
//    else //Handle the cases where a node leaves / stats are reset / other calculation errors occur
//    {
//        //Throw away anomolous statistic
//        worldSendStat = 0;
//        worldRecvStat = 0;
//    }

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

    latest_timestamp += UPDATE_PERIOD;

}

void calc_consistency (const Vast::VASTStatLog &restoredLog, size_t &total_AN_actual,
                                   size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                                   size_t &drift_nodes, timestamp_t latest_timestamp)
{
//    AN_actual = AN_visible = total_drift = max_drift = drift_nodes = 0;

    // loop through all nodes
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

            if (restoredLog.knows (otherLog))
            {
                total_AN_visible++;

                // calculate drift distance (except self)
                // NOTE: drift distance is calculated for all known AOI neighbors
                drift_nodes++;

                size_t drift = (size_t)restoredLog.getNeighborByID(otherLog.getSubID()).aoi.center.distance (otherLog.getClientNode().aoi.center);
                total_drift += drift;

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


}

void outputResults() {

    ofs << latest_timestamp << "," << total_active_nodes << "," << total_AN_actual <<
           "," << total_AN_visible << "," << total_drift << "," << max_drift << ","
        << drift_nodes << "," << worldSendStat << "," << worldRecvStat << std::endl;
}

void closeOutputFile()
{
    ofs.flush();
    ofs.close();
}
