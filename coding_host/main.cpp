#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>

#include <boost/asio.hpp>
#include <boost/chrono.hpp>

#include <kodo_rlnc/coders.hpp>
#include "packet_listener.h"

using namespace boost::asio;

bool running = true;

void SIGINT_handler (int)
{
    std::cout << "Interrupt received" << std::endl;
    running = false;

}

int main(int argc, char* argv[])
{
    signal(SIGINT, SIGINT_handler);

    std::string bind_address = "10.0.0.254";

    if (argc > 1)
    {
        bind_address.assign(argv[1]);
    }

    boost::asio::io_service io_service;
    auto local_endpoint = boost::asio::ip::udp::endpoint(ip::address::from_string(bind_address), 1037);

    std::cout << "Starting packet listener for coding host" << std::endl;
    packet_listener listener(local_endpoint);

    listener.open(&io_service);

    while(running);

}
