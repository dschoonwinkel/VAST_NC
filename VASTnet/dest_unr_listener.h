#ifndef DEST_UNR_LISTENER_H
#define DEST_UNR_LISTENER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <istream>
#include <iostream>
#include <ostream>

#include "icmp_header.hpp"
#include "ipv4_header.hpp"
#include "udp_header.hpp"

namespace Vast{
    class net_overhearing_handler;   //Forward declaration
}

using boost::asio::ip::icmp;

class dest_unreachable_listener {

public:
    dest_unreachable_listener(boost::asio::io_service& io_service, const char* destination, Vast::net_overhearing_handler* disconn_handler);
    void start_receive();
    void handle_receive(std::size_t length);

private:
    icmp::resolver resolver_;
    icmp::endpoint destination_;
    icmp::socket socket_;
    boost::asio::streambuf reply_buffer_;
    Vast::net_overhearing_handler* _disconn_handler;

};

#endif // DEST_UNR_LISTENER_H
