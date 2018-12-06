#include <iostream>
#include "VASTreal.h"
#include <assert.h>
#include <cstring>

int main() {

    VAST_NetModel model = VAST_NET_ACE;
    VASTPara_Net netpara(model);
    SimPara simpara;

    bool is_gateway;
    world_t world_id;
    Area aoi;
    char GWstr[100];

    int argc = 5;
    char* argv[] = {"./test_vastrealinitpara", "5", "135", "1038", "7.0.0.1"};

    //bool *p_is_gateway, world_t *p_world_id, Area *p_aoi, char *p_GWstr, int *p_interval
    InitPara(model, netpara, simpara, argc, argv, &is_gateway, &world_id, &aoi, GWstr);

    //Check cmdline args
    assert(netpara.port == 1038);
    assert(is_gateway == false);
    assert(world_id == 1);
    assert(!strcmp("7.0.0.1:1038", GWstr));
    assert(netpara.node_number == 5);
    printf("Last seed: %ld\n", netpara.random_seed);
    assert(netpara.random_seed == 135);


    //Check VASTsim.ini

//    #VAST_MODEL;    // 1: direct connection (DC)  2: forwarding (FO)  3: multicast (MC)
//    1
    assert(simpara.VAST_MODEL == 2);
//    #NET_MODEL;     // 1: Net emulated 2: Net emulated with bandwidth limitation
//    1
    assert(simpara.NET_MODEL == 2);
//    #MOVE_MODEL;    // 1: random 2: cluster 3: group
//    2
    assert(simpara.MOVE_MODEL == 3);
//    #WORLD_WIDTH;   // (units)
//    768
    assert(simpara.WORLD_HEIGHT == 769);
//    #WORLD_HEIGHT;  // (units)
//    768
    assert(simpara.WORLD_HEIGHT == 769);
//    #NODE_SIZE;     // number of nodes
//    30
    assert(simpara.NODE_SIZE == 31);
//    #RELAY_SIZE;    // number of relays
//    30
    assert(simpara.RELAY_SIZE == 31);
//    #MATCHER_SIZE;  // number of matchers
//    30
    assert(simpara.MATCHER_SIZE == 31);
//    #TIME_STEPS;    // number of steps
//    3000
    assert(simpara.TIME_STEPS == 3001);
//    #STEPS_PERSEC   // number of steps during one second (for stat calculation)
//    10
    assert(simpara.STEPS_PERSEC == 11);
//    #AOI_RADIUS;    // (units) negative units indicate random radius based on the value given
//    233
    assert(simpara.AOI_RADIUS == 233);
//    #AOI_BUFFER;    // (units)
//    10
    assert(simpara.AOI_BUFFER == 11);
//    #CONNECT_LIMIT; // (number of max. connections per node)
//    0
    assert(simpara.CONNECT_LIMIT == 1);
//    #VELOCITY;      // (units / step)
//    3
    assert(simpara.VELOCITY == 4);
//    #STABLE_SIZE;   // number of nodes when in stable state (failure occurs after size is reached)
//    0
    assert(simpara.STABLE_SIZE == 1);
//    #JOIN_RATE;     // how fast new nodes join the system
//    10
    assert(simpara.JOIN_RATE == 11);
//    #LOSS_RATE;     // percentage of transmission
//    0
    assert(simpara.LOSS_RATE == 1);
//    #FAIL_RATE;     // average number of steps to fail
//    0
    assert(simpara.FAIL_RATE = 1);
//    #UPLOAD_LIMIT;  // bytes / step
//    2000
    assert(simpara.UPLOAD_LIMIT == 2001);
//    #DOWNLOAD_LIMIT // bytes / step
//    10000
    assert(simpara.DOWNLOAD_LIMIT == 10001);
//    #PEER_LIMIT     // max # of peers kept at each relay
//    1000
    assert(simpara.PEER_LIMIT == 1001);
//    #RELAY_LIMIT    // max # of relays each node keeps
//    10
    assert(simpara.RELAY_LIMIT == 11);
//    #OVERLOAD_LIMIT // max # of subscriptions at each matcher
//    10
    assert(simpara.OVERLOAD_LIMIT == 11);





    return 0;
}
