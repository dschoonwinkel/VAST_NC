#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

// use VAST for functions
#include "VASTVerse.h"
#include "VASTUtil.h"
#include "VASTCallback.h"       // for creating callback handler
#include "vaststatlog_entry.h"

#define VAST_EVENT_LAYER    1                   // layer ID for sending events
#define VAST_UPDATE_LAYER   2                   // layer ID for sending updates

using namespace Vast;
using namespace std;

// global
int         g_steps = 0;
Area        g_aoi;              // my AOI (with center as current position)
world_t     g_world_id = 1;     // world ID
bool        g_finished = false; // whether we're done for this program execution

// VAST-specific variables
VASTVerse *     g_world = NULL;     // factory for creating a VAST node
VAST *          g_self  = NULL;     // pointer to VAST
NodeState       g_state = ABSENT;     //State of joining node
Vast::id_t      g_sub_id = 0;       // subscription # for my client (peer)

//VAST statistics
VASTStatLogEntry*    g_statlog = NULL;    //Logger for statistics


int main (int argc, char *argv[])
{
    g_world_id     = VAST_DEFAULT_WORLD_ID;

    //These are the join coordinate of the demo_chatva_Qt program (I do not know why it is this, and always the same...)
    g_aoi.center.x = 103;
    g_aoi.center.y = 454;
    g_aoi.radius   = 200;

    // set network parameters
    VASTPara_Net netpara (VAST_NET_EMULATED);
    netpara.port = GATEWAY_DEFAULT_PORT;

    char GWstr[80];     // gateway string
    GWstr[0] = 0;

    bool is_gateway = false;

    if (GWstr[0] == 0)
    {
        is_gateway = true;
        netpara.is_entry = true;
        sprintf (GWstr, "127.0.0.1:%d", netpara.port);
    }

    // force all clients to default join at world 2, unless specified other than 2
    if (!is_gateway && g_world_id == VAST_DEFAULT_WORLD_ID)
    {
        g_world_id = VAST_DEFAULT_WORLD_ID + 1;
    }

    // ESSENTIAL: create VAST node factory and pass in callback
    g_world = new VASTVerse (is_gateway, string (GWstr), &netpara, NULL, NULL, 20);
    g_world->createVASTNode(g_world_id, g_aoi, VAST_EVENT_LAYER);


    while (g_state != JOINED)
    {
        g_steps++;
        //Wait for node to join and get subID
        if (g_state != JOINED) {
            if ((g_self = g_world->getVASTNode()) != NULL) {
                g_sub_id = g_self->getSubscriptionID();
                g_state = JOINED;
                g_statlog = new VASTStatLogEntry(g_world, g_self);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        g_world->tick();
        g_world->tickLogicalClock();
        if (g_statlog != NULL)
        {
            g_statlog->recordStat();
            g_statlog->printStat();

        }
    }

    string _logfilename = "./logs/VASTStat";
    _logfilename = _logfilename + "_N" + std::to_string(g_self->getSelf()->id) + ".txt";
    std::cout << "Reading file: " << _logfilename << std::endl;


    std::vector<Vast::VASTStatLogEntry> restoredLogs = Vast::VASTStatLogEntry::restoreAllFromLogFile(_logfilename);

    if (restoredLogs.size() < 1)
    {
        std::cerr << "Could not restore any logs from filename specified: " << _logfilename << std::endl;
        exit(123);
    }

    restoredLogs[0].printStat();


//    Node node1 = *g_self->getSelf();
//    Node node2 = node1;
//    Node node3;

//    std::cout << "Nodes are equal: " << (node1 == node2) << std::endl;
//    std::cout << "Nodes are equal: " << (node1 == node3) << std::endl;


    g_world->destroyVASTNode(g_self);
    delete g_world;

    return 0;
}
