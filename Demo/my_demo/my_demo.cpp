
/*
 *  demo_console    a console-based minimal VAST application 
 *  
 *  version:    2009/06/12      init
 *              2010/09/01      simplfied from test_console (removing all loging capability)
 */


#ifdef WIN32
// disable warning about "unsafe functions"
#pragma warning(disable: 4996)
#endif

#include "ace/ACE.h"    // for sleep functions
//#include "ace/OS.h"
#include "ace/Init_ACE.h"
#include "ace/OS_NS_unistd.h"       // ACE_OS::sleeps
#include "ace/OS_NS_sys_time.h"     // gettimeofday

#include <stdio.h>
#include <iostream>

#ifdef WIN32
#include <conio.h>      // for getting keyboard inputs
#endif

#include "Movement.h"

// use VAST for functions
#include "VASTVerse.h"
#include "VASTUtil.h"
#include "VASTCallback.h"       // for creating callback handler

#include "my_demo.h"

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
world_t     g_world_id = 0;     // world ID
bool        g_finished = false; // whether we're done for this program execution
char        g_lastcommand = 0;  // last keyboard character typed 

// VAST-specific variables
VASTVerse *     g_world = NULL;     // factory for creating a VAST node
VAST *          g_self  = NULL;     // pointer to VAST
NodeState       g_state = ABSENT;     //State of joining node
Vast::id_t      g_sub_id = 0;       // subscription # for my client (peer)  

//Variables for the second node, i.e. node 2
VAST *          g_self2  = NULL;     // pointer to VAST
NodeState       g_state2 = ABSENT;     //State of joining node
Vast::id_t      g_sub_id2 = 0;       // subscription # for my client (peer)

//Movement Model
MovementGenerator g_move_model;  
namespace MoveModel {
enum Move_Model {
    RANDOM  = 1 ,
    CLUSTER,
    GROUP,
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
    srand (now.usec ());
                      
    //
    // set default values
    //
    g_world_id     = VAST_DEFAULT_WORLD_ID;

    g_aoi.center.x = (coord_t)(rand () % 100);
    g_aoi.center.y = (coord_t)(rand () % 100);
    g_aoi.radius   = 200;

    // make backup of AOI, to detect position change so we can move the client
    g_prev_aoi = g_aoi;

    // set network parameters
    VASTPara_Net netpara (VAST_NET_ACE);
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

    // world id
    if (argc > 3)
        g_world_id = (world_t)atoi (argv[3]);

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


    //Randomize position before adding another node
    g_aoi.center.x = (coord_t)(rand () % 100);
    g_aoi.center.y = (coord_t)(rand () % 100);

    //I expect this to be false: can only use createVASTNode once... What should I do then?
    cout << "Creating another new node: success == " << g_world->createVASTNode(g_world_id, g_aoi, VAST_EVENT_LAYER) << endl;
    
    g_move_model.initModel(MoveModel::RANDOM, NULL, false, Position(0,0), Position(DIM_X, DIM_Y), 1, 10000, 1);

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
            // g_self->move(g_sub_id, g_aoi);
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
                std::cout << "Received message from " << from << " : " << chatmsg << std::endl;
            }

            char random_text[10];
            gen_random_str(random_text, 10);
            send_to_neighbours(g_self, string(random_text));

        }

        // NOTE the 2nd parameter is specified in microseconds (us) not milliseconds
        ACE_Time_Value duration (0, 100000);            
        ACE_OS::sleep (duration);
        g_world->tick(); 
    }
    
    //
    // depart & clean up
    //

    delete g_world;        

    return 0;
}
