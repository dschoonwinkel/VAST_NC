
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
#include <chrono>
#include "timeouts.h"
#include <unistd.h>
#include <signal.h>

using namespace std;
using namespace Vast;

bool running = true;

//Sleep duration secs, microseconds
#define SLEEP_DURATION 0, MS_PER_STEP*1000

void SIGINT_handler (int)
{
    std::cout << "Interrupt received" << std::endl;
    running = false;

}

int main (int argc, char *argv[])
{
    signal(SIGINT, SIGINT_handler);

    std::cout << "VASTreal_console: PID: " << getpid() << std::endl;
    SimPara simpara;                                    // simulation parameters
    VASTPara_Net netpara (VAST_NET_UDP);           // network parameter

    ReadPara (simpara);

    // read parameters and initialize simulations
    InitPara (VAST_NET_UDP, netpara, simpara, argc, argv);
    

    std::cout << "netpara.model: " << netpara.model << std::endl;

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

    std::chrono::microseconds process_duration = std::chrono::microseconds::zero();
    std::chrono::microseconds total_duration = std::chrono::microseconds::zero();

    std::cout << "VASTreal_console::main:: Is gateway: " << netpara.is_entry << std::endl << std::endl;

    CPPDEBUG("Steps per printout update: " << VASTREAL_STEP_DISPLAY_DIVISOR/g_MS_PER_TIMESTEP << std::endl);
    while (running)
    {     
        steps++; 

        auto t1 = std::chrono::high_resolution_clock::now();

        // Create node is not yet joined
        if (nodes_created == 0)
        {
            CreateNode ();
            nodes_created++;
        }

                 
        if (NextStep () < 0)
            break;

        if (steps % (VASTREAL_STEP_DISPLAY_DIVISOR/g_MS_PER_TIMESTEP) == 0)
        {
            printf ("step %d\n", steps);
            std::cout << "Average process duration: " << process_duration.count() / steps << " microsec" << std::endl;
            std::cout << "Average total duration: " << total_duration.count() / steps << " microsec" << std::endl;
        }

        auto t2 = std::chrono::high_resolution_clock::now();
        process_duration += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);



        ACE_Time_Value duration (SLEEP_DURATION - std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
        ACE_OS::sleep(duration);

        auto t3 = std::chrono::high_resolution_clock::now();
        total_duration += std::chrono::duration_cast<std::chrono::microseconds>(t3-t1);

    }

    Shutdown();
    
    return 0;
}

