#include <boost/asio.hpp>
#include <boost/chrono.hpp>

#include "mc_packet_listener.h"

using namespace boost::asio;

int main()
{
    io_service ios;

    MC_packet_listener mc_listener;
    mc_listener.open(&ios);

    //Wait while reading incoming messages
    while(true);

}
