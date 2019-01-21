/*
 * VAST, a scalable peer-to-peer network for virtual environments
 * Copyright (C) 2005-2011 Shun-Yun Hu (syhu@ieee.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 *	simulation library for VAST      ver 0.1         2005/04/11 
 *                                   ver 0.2         2006/04/04
 */

#include "VASTreal.h"
#include "VASTVerse.h"
#include "RealNode.h"
#include <stdlib.h>         // strtok
#include "vaststatlog_entry.h"

#define RECORD_LATENCY      // to record transmission latency for MOVEMENT messages
//#include "Statistics.h"

using namespace std;        // vector
using namespace Vast;       // vast

SimPara             g_para;
VASTPara_Net        g_vastnetpara (VAST_NET_ACE);
//statistics          g_stat;

//vector<RealNode *>   g_nodes;            // pointer to all simulation nodes
size_t              g_node_number = 0;
RealNode*           g_node = NULL;
world_t             g_world_id = 0;
NodeState           g_state = ABSENT;
Vast::id_t          g_sub_id = 0;


vector<bool>        g_as_relay;
vector<bool>        g_as_matcher;
MovementGenerator   g_move_model;
char                g_GWstr[80];          // address to gateway node
SectionedFile      *g_pos_record = NULL;

//map<int, VAST *>    g_peermap;          // map from node index to the peer's relay id
//map<int, Vast::id_t> g_peerid;           // map from node index to peer id

//int                 g_last_seed;       // storing random seed

int                 g_steps     = 0;
bool                g_joining   = true;


// Initilize parameters, including setting default values or read from INI file
int InitPara (VAST_NetModel model, VASTPara_Net &netpara, SimPara &simpara, int argc, char *argv[], bool *p_is_gateway, world_t *p_world_id, Area *p_aoi, char *p_GWstr, int *p_interval)
{
    netpara.model = model;

    // parameters to be filled
    bool is_gateway;
    g_world_id = VAST_DEFAULT_WORLD_ID;
    Area aoi;


    // set default values
    aoi.center.x = (coord_t)(rand () % DIM_X);
    aoi.center.y = (coord_t)(rand () % DIM_Y);
    aoi.radius   = (length_t)DEFAULT_AOI;

    netpara.port  = GATEWAY_DEFAULT_PORT;

    netpara.relay_limit     = 0;
    netpara.client_limit    = 0;
    netpara.overload_limit  = 0;

    // by default the node can be relay & matcher
    netpara.is_relay = true;
    netpara.is_matcher = true;

    // dynamic load balancing is enabled by default
    netpara.is_static = false;

    simpara.NODE_SIZE = 10;
    simpara.STEPS_PERSEC = STEPS_PER_SECOND;

    // which node to simulate, (0) means manual
    int node_no = 0; 

    // interval in seconds to pause before joining
    int interval = 0;
   
    char GWstr[80];
    GWstr[0] = 0;

    // process command line parameters, if available
    char *p;

    if (argc <= 2)
    {
        std::cout << "Usage: ./VASTreal_console <nodeid> <randomseed> <port> <GW_IPaddr> <world_id> <pause_interval> <x-coord> <y-coord> <radius> <is_relay> <is_matcher>" << std::endl;
//        exit(0);
    }
    
    if (argc >= 2)
    {
        p = argv[0];
    
        for (int i = 1; i < argc; i++)
        {
            //Increment to the next cmdline arg string
            p = argv[i];

            printf("Parsing %s for arg %d\n", p, i);
            switch (i - 1)
            {

            // first parameter: node to simulate
            case 0:
                node_no = atoi (p);
                netpara.node_number = node_no;
                g_node_number = node_no;
                break;

            case 1:
                netpara.random_seed = atoi (p);
                break;

            // port
            case 2:
                netpara.port = (unsigned short)atoi (p);
                break;
    
            // Gateway IP
            case 3:
                // (zero indicates gateway)
                if (p[0] != '0')
                    sprintf (GWstr, "%s:%d", p, netpara.port);
                break; 

            // world id
            case 4:
                g_world_id = (world_t)atoi (p);
                break;

            // interval for pausing in joining
            case 5:
                interval = atoi (p);
                break; 

            // X-coord
            case 6:
                aoi.center.x = (coord_t)atoi (p);
                break;
    
            // Y-coord
            case 7:
                aoi.center.y = (coord_t)atoi (p);
                break;

            // AOI-radius
            case 8:
                aoi.radius = (length_t)atoi (p);
                break;

            // is relay
            case 9:
                netpara.is_relay = (atoi (p) == 1);
                break;

            // is matcher
            case 10:
                netpara.is_matcher = (atoi (p) == 1);
                break;
            }
        }
    }

//    std::cout << "VASTreal::InitPara commandline args:" << std::endl;
//    for (int i = 0; i < argc; i++)
//    {
//        std::cout << argv[i] << std::endl;
//    }

//    std::cout << "Parsed values from cmdline args:" << std::endl;
//    std::cout << GWstr << std::endl;
//    std::cout << netpara.port << std::endl;

//    std::cout << std::endl;
	 
    printf("ReadPara from InitPara\n");   
    // see if simulation behavior file exists for simulated behavior            
    if (ReadPara (simpara, "VASTreal.ini") == true)
    {
        // override defaults         
        netpara.overload_limit = simpara.OVERLOAD_LIMIT;

        // command line radius takes precedence than INI radius
        if (aoi.radius == (length_t)DEFAULT_AOI)
            aoi.radius = (length_t)simpara.AOI_RADIUS;        
    }
    else  
    {
        // INI file not found, cannot perform simulation
        printf ("warning: INI file is not found at working directory, it's required for simulation\n");
        return (-1);
    }
    
    is_gateway = false;

    // default gateway set to localhost
    if (node_no == 0 && GWstr[0] == 0)
    {
        is_gateway = true;
        sprintf (GWstr, "127.0.0.1:%d", netpara.port);
    }

    if (node_no == 0 && is_gateway == false)
    {
        is_gateway = true;
    }

    netpara.is_entry = is_gateway;
        
    // if physical coordinate is not supplied, VAST will need to obtain it itself
    //g_vastnetpara.phys_coord = g_aoi.center;    

    // translate gateway string to Addr object
    strcpy (g_GWstr, GWstr);

    // create VAST node factory (with default physical coordinate)          

    // return values, if needed
    if (p_is_gateway != NULL)
        *p_is_gateway = is_gateway;

    if (p_aoi != NULL)
        *p_aoi = aoi;

    if (p_world_id != NULL)
        *p_world_id = g_world_id;

    if (p_GWstr != NULL)
        strcpy (p_GWstr, g_GWstr);

    if (p_interval != NULL)
        *p_interval = interval;

    return node_no;
}

// read parameters from input file
bool ReadPara (SimPara &para, const char * filename /* = "VASTreal.ini*/)
{
    printf("ReadPara called\n");
    FILE *fp;
    if ((fp = fopen (filename, "rt")) == NULL)
        return false;

    int *p[] = {
        &para.VAST_MODEL,
        &para.NET_MODEL,
        &para.MOVE_MODEL,
        &para.WORLD_WIDTH,
        &para.WORLD_HEIGHT,
        &para.NODE_SIZE,
        &para.RELAY_SIZE,
        &para.MATCHER_SIZE,
        &para.TIME_STEPS,
        &para.STEPS_PERSEC,
        &para.AOI_RADIUS,
        &para.AOI_BUFFER,
        &para.CONNECT_LIMIT,
        &para.VELOCITY,
        &para.STABLE_SIZE,
        &para.JOIN_RATE,
        &para.LOSS_RATE,
        &para.FAIL_RATE, 
        &para.UPLOAD_LIMIT,
        &para.DOWNLOAD_LIMIT,
        &para.PEER_LIMIT,
        &para.RELAY_LIMIT,
        &para.OVERLOAD_LIMIT,
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


int InitSim (SimPara &para, VASTPara_Net &netpara)
{
    g_para = para;
    g_vastnetpara = netpara;

    // note there's no need to assign the gateway ID as it'll be found automatically
    g_vastnetpara.model        = (VAST_NetModel)para.NET_MODEL;
    g_vastnetpara.port         = GATEWAY_DEFAULT_PORT;    
    g_vastnetpara.client_limit = para.PEER_LIMIT;
    g_vastnetpara.relay_limit  = para.RELAY_LIMIT;        
    g_vastnetpara.conn_limit   = para.CONNECT_LIMIT;
    g_vastnetpara.recv_quota   = para.DOWNLOAD_LIMIT;
    g_vastnetpara.send_quota   = para.UPLOAD_LIMIT;

    // create / open position log file
    char filename[80];
    sprintf (filename, VAST_POSFILE_FORMAT, para.NODE_SIZE, para.WORLD_WIDTH, para.WORLD_HEIGHT, para.TIME_STEPS);

    FileClassFactory fcf;
    g_pos_record = fcf.CreateFileClass (0);
    bool replay = true;
    if (g_pos_record->open (filename, SFMode_Read) == false)
    {
        replay = false;
        g_pos_record->open (filename, SFMode_Write);
    }

    srand(netpara.random_seed);

    // create behavior model
    std::cout << std::endl << "VASTreal::InitSim::g_para.NODE_SIZE " << g_para.NODE_SIZE << std::endl;
    g_move_model.initModel (g_para.MOVE_MODEL, g_pos_record, replay, 
                            Position (0,0), Position ((coord_t)g_para.WORLD_WIDTH, (coord_t)g_para.WORLD_HEIGHT),
                            g_para.NODE_SIZE, g_para.TIME_STEPS, (double)g_para.VELOCITY);    

    // close position log file
    fcf.DestroyFileClass (g_pos_record);

    // initialize random number generator
    //srand ((unsigned int)time (NULL));
//    srand (37);
//    g_last_seed = rand ();

//    // randomly choose which nodes will be the relays
    int num_relays = 1;
    int i;
    for (i=0; i < para.NODE_SIZE; i++)
    {
        g_as_relay.push_back (false);
        g_as_matcher.push_back (false);
    }

    // determine which nodes will be relays & matchers
    g_as_relay[0] = true;
    g_as_matcher[0] = true;

    i = 1;
    while (num_relays < para.RELAY_SIZE)
    {
        //if (rand () % 100 <= ((float)para.RELAY_SIZE / (float)para.NODE_SIZE * 100))  //This would help with random assignment
        //{    
            g_as_relay[i] = true;
            num_relays++;
        //}

        // Restart counting from first node
        if (++i == para.NODE_SIZE)
            i = 0;
    }

    i = 1;
    while (i < para.MATCHER_SIZE)
    {
        g_as_matcher[i++] = true;       
    }

    // starts stat collections
//    g_stat.init_timer (g_para);
    return 0;
}

bool CreateNode (bool wait_till_ready)
{

    std::cout << "VASTreal::CreateNode::g_vastnetpara.NET_model: " << g_vastnetpara.model << std::endl << std::endl;

    g_vastnetpara.is_relay      = g_as_relay[g_node_number];
    g_vastnetpara.is_matcher    = g_as_matcher[g_node_number];
    g_vastnetpara.is_static     = STATIC_PARTITIONING;

    g_node = new RealNode(g_node_number, &g_move_model, g_GWstr, g_para, g_vastnetpara);

    // NOTE: it's important to advance the logical time here, because nodes would 
    //       not be able to receive messages sent during the same time-steps

    if (wait_till_ready)
    {
        // make sure all nodes have joined before moving    
        do 
        {
                g_node->processMessage ();
        }
        // make sure the new node has joined before moving on
        while (g_node->isJoined () == false);
    }
    else
    {
            g_node->processMessage ();
    }
    
    return true;
}

// return # of inconsistent nodes during this step if stat_collect is enabled
// returns (-1) if simulation has ended  
//
// NOTE: during failure simulation, there's a JOIN-stablize-FAIL-stablize cycle
//       stat is collected only during the FAIL-stablize period 
//
int NextStep ()
{   
    g_steps++;

        if (g_node->isJoined ())
        {
            g_node->move ();
        }    

//    // fail a node if time has come, but only within a certain time interval
//    if (g_para.FAIL_RATE > 0   &&
//        num_active > g_para.STABLE_SIZE &&
//        g_steps > (g_para.TIME_STEPS / 3) && g_steps <= (2 * g_para.TIME_STEPS / 3) &&
//        //num_active < (g_para.STABLE_SIZE * STABLE_SIZE_MULTIPLIER) &&
//        g_steps % g_para.FAIL_RATE == 0)
//    {
//        //FailMethod method = RELAY_ONLY;
//        //FailMethod method = MATCHER_ONLY;
//        //FailMethod method = CLIENT_ONLY;
//        FailMethod method = RANDOM;
        
//        // index of the node to fail
//        int i = n;

//        switch (method)
//        {
//        // random fail
//        case RANDOM:
//            {
//                if (num_active > 1)
//                {
//                    // NOTE: we assume there's definitely some node to fail, otherwise
//                    //       will enter infinite loop
//                    bool failed = false;
//                    int tries = 0;
//                    while (!failed)
//                    {
//                        // do not fail gateway
//                        i = (rand () % (n-1))+1;
                                        
//                        if (g_node->isJoined () || tries > n)
//                            break;
                
//                        tries++;
//                    }
//                }
//            }
//            break;

//        case RELAY_ONLY:
//            {
//                // fail each active relay (except gateway) until all are failed
//                for (i=1; i<n; i++)
//                {
//                    if (g_as_relay[i] == true)
//                    {
//                        g_as_relay[i] = false;
//                        break;
//                    }
//                }
//            }
//            break;

//        case MATCHER_ONLY:
//            {
//                // fail each active relay (except gateway) until all are failed
//                for (i=1; i<n; i++)
//                {
//                    if (g_as_matcher[i] == true)
//                    {
//                        g_as_matcher[i] = false;
//                        break;
//                    }
//                }
//            }
//            break;

//        case CLIENT_ONLY:
//            {
//                // fail each active non-relay
//                for (i=1; i<n; i++)
//                {
//                    if ((g_as_relay[i] == false && g_as_matcher[i] == false) &&
//                        g_node->isJoined () == true)
//                    {
//                        break;
//                    }
//                }
//            }
//            break;
//        }

//        if (i > 0 && i < n)
//        {
//            printf ("failing [%lu]..\n", g_node->getPeerID ());
//            g_node->fail ();
//        }
//    }
    
    // each node processes messages received so far
    g_node->processMessage ();

    // each node calculates stats
    g_node->recordStat ();

    if (g_steps >= g_para.TIME_STEPS)
        return (-1);
    else
        return 1;
}

Node *GetNode ()
{
    // check for VAST node not yet joined, or failed node
    if (g_node->vnode == NULL ||
        g_node->isFailed ())
        return NULL;

    return g_node->getSelf ();
}

std::vector<Node *>* GetNeighbors ()
{   
    //if (g_peermap.find (index) == g_peermap.end ())
    //    return NULL;

    // neighbors as stored on the relay's VONPeers
    //return g_peermap[index]->getPeerNeighbors (g_peerid[index]);

    // check if the node referred has not yet joined successfully
    if (g_node->vnode == NULL)
        return NULL;

    return &g_node->vnode->list ();
}

std::vector<Vast::id_t> *GetEnclosingNeighbors (int level)
{
    return NULL;

    /*
    if (g_peermap.find (index) == g_peermap.end ())
        return NULL;

    Voronoi *v = g_peermap[index]->getVoronoi (g_peerid[index]);   

    if (v == NULL)
        return NULL;

    return &v->get_en (g_node->getID (), level);
    */
}

std::vector<line2d> *GetEdges ()
{
    //return NULL;

    if (g_node->vnode == NULL)
        return NULL;

    Voronoi *v = g_node->getVoronoi ();

    if (v == NULL)
        return NULL;

    return &v->getedges ();
}

bool isNodeMatcher() {
    return g_node->getVerse()->isMatcher();
}


bool GetBoundingBox (point2d& min, point2d& max)
{
    return false;
}

Area * GetNodeMatcherAOI() {
    return g_node->getVerse()->getMatcherAOI();
}

int Shutdown ()
{    

    g_as_relay.clear ();

    delete (RealNode *)g_node;
    g_node = NULL;

    return 0;
}





















