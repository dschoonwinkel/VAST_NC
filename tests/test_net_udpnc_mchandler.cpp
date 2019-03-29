#include <iostream>

#include "net_udpnc_mchandler.h"

using namespace boost::asio;

int main()
{
    //std::string remote_ip = "127.0.0.1";
    //uint16_t remote_port = 1037;
    //ip::udp::endpoint local_endpoint = ip::udp::endpoint(
    //            ip::address::from_string(remote_ip), remote_port);

    //Vast::net_udpNC_MChandler mchandler(local_endpoint);
    Vast::net_udpNC_MChandler mchandler();
    mchandler.open(NULL);

    while(true);
}
