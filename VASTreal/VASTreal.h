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
 *  VASTreal.h -- header for simulation DLL
 *
 *  ver 0.1 (2005/04/12)
 *   
 */

#ifndef VASTREAL_H
#define VASTREAL_H

#ifdef WIN32
// disable warning about "identifier exceeds'255' characters"
#pragma warning(disable: 4786)
#endif

#define ACE_DISABLED_                        // we disable using ACE in emulation mode
#define RECORD_INCONSISTENT_NODES_           // enable this will greatly reduce run speed

#define PREASSIGNED_MATCHERS_   9                // whether matchers join at beginning 
const bool STATIC_PARTITIONING = false;          // whether dynamic load balancing is off

#include "VASTTypes.h"
#include "Voronoi.h"
#include "VASTVerse.h"

#include <stdlib.h>         // atoi, srand
#include <vector>

using namespace Vast;
using namespace std;
    
#define DEFAULT_AOI         195     // the AOI-radius size for all nodes
#define DIM_X               768     // size of the world & its default values
#define DIM_Y               768
#define STEPS_PER_SECOND    10      // default steps per second

#define STABLE_SIZE_MULTIPLIER  (1.50) // define upper bound for stable size

typedef enum  
{
    IDLE,
    WAITING,
    NORMAL,
    FAILING,
    FAILED

} RealNodeState;

// initialize parameters, returns which node index to simulate, -1 for error, 0 for manual control
EXPORT bool                 ReadPara (SimPara &para, const char * filename = "VASTreal.ini");
EXPORT int                  InitPara (VAST_NetModel model, VASTPara_Net &netpara, SimPara &simpara, int argc = 0, char *argv[] = NULL, bool *is_gateway = NULL, world_t *world_id = 0, Area *aoi = NULL, char *GWstr = NULL, int *interval = NULL);
EXPORT int                  InitSim (SimPara &para, VASTPara_Net &netpara);
EXPORT bool                 CreateNode (bool wait_till_ready = true);
EXPORT int                  NextStep ();
EXPORT Node *               GetNode ();
vector<Vast::Node *>*GetNeighbors ();
EXPORT vector<Vast::id_t> * GetEnclosingNeighbors (int level = 1);
EXPORT int                  Shutdown ();
EXPORT vector<Vast::line2d>*GetEdges ();
EXPORT bool					isNodeMatcher();
EXPORT Area                 *GetNodeMatcherAOI();
EXPORT bool                 GetBoundingBox (point2d& min, point2d& max);




#endif // VASTREAL_H

