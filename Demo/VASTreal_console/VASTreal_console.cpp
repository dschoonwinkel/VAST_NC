
/*
 *	console application for VAST      ver 0.0             22/11/2018/
 */

                                  


#include <stdlib.h>         // atoi, srand
#include "ace/ACE.h"
#include "ace/Init_ACE.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_sys_time.h"

#include "VASTreal.h"
#include "vaststatlog_entry.h"

using namespace std;
using namespace Vast;

int main (int argc, char *argv[])
{    
    SimPara simpara;                                    // simulation parameters
    VASTPara_Net netpara (VAST_NET_ACE);           // network parameter

    ReadPara (simpara);

    // read parameters and initialize simulations
    InitPara (VAST_NET_ACE, netpara, simpara, argc, argv);
    
    InitSim (simpara, netpara);

    // # of nodes currently created
    int nodes_created = 0;

    ACE::init();
    ACE_Time_Value now = ACE_OS::gettimeofday();
    //Random seed is applied in  InitSim function
//    srand(2)
//    srand(now.usec());

    // Set up variables required for operation
    int steps = 0;
    bool running = true;

    std::cout << "VASTreal_console::main:: Is gateway: " << netpara.is_entry << std::endl << std::endl;
    while (running)
    {     
        steps++; 

        // Create node is not yet joined
        if (nodes_created == 0)
        {
            CreateNode ();
            nodes_created++;
        }

                 
        if (NextStep () < 0)
            break;

        if (steps % 100 == 0)
            printf ("step %d\n", steps);

        ACE_Time_Value duration (0, 10000);
        ACE_OS::sleep(duration);

    }

    Shutdown();
    
    return 0;
}

