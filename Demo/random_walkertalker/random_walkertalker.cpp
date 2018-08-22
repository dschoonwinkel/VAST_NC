
/*
 *  demo_console    a console-based minimal VAST application 
 *  
 *  version:    2009/06/12      init
 *              2010/09/01      simplfied from test_console (removing all loging capability)
 */


#include "ace/ACE.h"    // for sleep functions
//#include "ace/OS.h"
#include "ace/Init_ACE.h"
#include "ace/OS_NS_unistd.h"       // ACE_OS::sleeps
#include "ace/OS_NS_sys_time.h"     // gettimeofday

#include <stdio.h>
#include <iostream>

#include "Movement.h"

// use VAST for functions
#include "VASTVerse.h"
#include "VASTUtil.h"
#include "VASTCallback.h"       // for creating callback handler

#include "random_walkertalker.h"

#define VAST_EVENT_LAYER    1                   // layer ID for sending events 
#define VAST_UPDATE_LAYER   2                   // layer ID for sending updates

// user-defined message types
#define DEMO_MSGTYPE_BANDWIDTH_REPORT   1
#define DEMO_MSGTYPE_KEYPRESS           2

#define INPUT_SIZE          200                 // size for chat message

#define DIM_X 768
#define DIM_Y 768

using namespace Vast;
using namespace std;

#ifdef ACE_DISABLED
#error "ACE needs to be enabled to build demo_console, please modify /common/Config.h"
#endif


// number of seconds elasped before a bandwidth usage is reported to gateway
const int REPORT_INTERVAL        = 10;     

// global
int         g_steps = 0;
Area        g_aoi;              // my AOI (with center as current position)
Area        g_prev_aoi;         // previous AOI (to detect if AOI has changed)
world_t     g_world_id = 1;     // world ID
bool        g_finished = false; // whether we're done for this program execution
char        g_lastcommand = 0;  // last keyboard character typed 

// VAST-specific variables
VASTVerse *     g_world = NULL;     // factory for creating a VAST node
VAST *          g_self  = NULL;     // pointer to VAST
NodeState       g_state = ABSENT;     //State of joining node
Vast::id_t      g_sub_id = 0;       // subscription # for my client (peer)  


//Movement Model
MovementGenerator g_move_model;  
namespace MoveModel {
    enum Move_Model {
        RANDOM  = 1 ,
        CLUSTER,
        GROUP
    };
}


// socket-specific variables
Vast::id_t      g_socket_id = NET_ID_UNASSIGNED;    // socket ID


// print out current list of observed neighbors
void printNeighbors (unsigned long long curr_msec, Vast::id_t selfID, bool screen_only = false)
{
    // if we havn't joined successfully
    if (g_self == NULL)
        return;
    
    vector<Node *>& neighbors = g_self->list ();
    
    printf ("Neighbors:");
    for (size_t i = 0; i < neighbors.size (); i++)
    {
        if (i % 2 == 0)
            printf ("\n");

        printf ("[%lu] (%d, %d) ", (neighbors[i]->id), (int)neighbors[i]->aoi.center.x, (int)neighbors[i]->aoi.center.y);
    }
    printf ("\n");  
}

int main (int argc, char *argv[])
{   
    // 
    // Initialization
    //
    
    // for using ACE (get current time in platform-independent way)
    ACE::init ();

    // initialize random seed
    // NOTE: do not use time () as nodes at different sites may have very close time () values
    ACE_Time_Value now = ACE_OS::gettimeofday ();
    printf ("Setting random seed as: %d\n", (int)now.usec ());

    //Keep randominess constant
    srand (2);
//    Or changing, if required later
//    srand (now.usec ());
                      
    //
    // set default values
    //
    g_world_id     = VAST_DEFAULT_WORLD_ID;

//    g_aoi.center.x = (coord_t)(rand () % 100);
//    g_aoi.center.y = (coord_t)(rand () % 100);

    //These are the join coordinate of the demo_chatva_Qt program (I do not know why it is this, and always the same...)
    g_aoi.center.x = 103;
    g_aoi.center.y = 454;



    g_aoi.radius   = 200;

    // make backup of AOI, to detect position change so we can move the client
    g_prev_aoi = g_aoi;

    // set network parameters
    VASTPara_Net netpara (VAST_NET_OVERHEARING);
    netpara.port = GATEWAY_DEFAULT_PORT;

    //
    // load command line parameters, if any
    //
    char GWstr[80];     // gateway string
    GWstr[0] = 0;

    // port
    if (argc > 1)        
        netpara.port = (unsigned short)atoi (argv[1]);
    
    // gateway's IP 
    if (argc > 2)        
        sprintf (GWstr, "%s:%d", argv[2], netpara.port);

//    // world id
//    if (argc > 3)
//        g_world_id = (world_t)atoi (argv[3]);

    //
    // setup gateway
    //

    bool is_gateway = false;

    // default gateway set to localhost
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


//    Randomize position before adding another node
//    g_aoi.center.x = (coord_t)(rand () % 100);
//    g_aoi.center.y = (coord_t)(rand () % 100);
    
    g_move_model.initModel(MoveModel::RANDOM, NULL, false, Position(0,0), Position(DIM_X, DIM_Y), 1, 100000, 1);

    //
    // main loop
    //
    while (!g_finished)
    {
        g_steps++;
        //Wait for node to join and get subID
        if (g_state != JOINED) {
            if ((g_self = g_world->getVASTNode()) != NULL) {
                g_sub_id = g_self->getSubscriptionID();
                g_state = JOINED;
            }
        }
        //Move AOI subscription somewhere
        else {
            // cout << "Moving... apparently" << endl;
            // g_aoi.center.x = (coord_t)(rand () % 100);
            // g_aoi.center.y = (coord_t)(rand () % 100);
            Position *pos = g_move_model.getPos(0, g_steps%10000);
            g_aoi.center = *pos;
             g_self->move(g_sub_id, g_aoi);
        }

        if (g_state == JOINED) {
            char recv_buf[INPUT_SIZE];
            size_t size = 0;
            Vast::id_t from;

            Message *msg = NULL;
            if ((msg = g_self->receive ()) != NULL)
            {
                size = msg->extract (from);
                size = msg->extract (recv_buf, 0);
                recv_buf[size]=0;

                string chatmsg (recv_buf, size);
//                std::cout << "Received message from " << from << " : " << chatmsg << std::endl;
                debug_print("Received message from %lu %s\n", from, chatmsg.c_str());
            }

            if (g_steps % 50 == 0) {
                char random_text[50];
                gen_random_sentence(random_text, 50);
                send_to_neighbours(g_self, string(random_text));
//                g_finished = true;
            }

        }

//        printf("random_walkertalker: main loop: timestamp: %lu\n", g_world->getTimestamp());

        // NOTE the 2nd parameter is specified in microseconds (us) not milliseconds
        ACE_Time_Value duration (0, 100000);            
        ACE_OS::sleep (duration);
        g_world->tick();
        g_world->tickLogicalClock();
    }
    
    //
    // depart & clean up
    //

    delete g_world;        

    return 0;
}
