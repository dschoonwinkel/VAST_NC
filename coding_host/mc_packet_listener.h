#ifndef MC_PACKET_LISTENER_H
#define MC_PACKET_LISTENER_H

#include "packet_listener.h"

class MC_packet_listener : packet_listener
{
public:
    MC_packet_listener();

    int open (io_service *ios);
    void start_receive();
    int handle_input(const boost::system::error_code &error, std::size_t bytes_transferred);

    ip::udp::endpoint MC_address;
};

#endif // MC_PACKET_LISTENER_H
