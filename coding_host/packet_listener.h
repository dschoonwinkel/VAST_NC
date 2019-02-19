#ifndef PACKET_LISTENER_H
#define PACKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "rlnc_packet_factory.h"
#include <deque>

using namespace boost::asio;

class packet_listener
{
public:
    packet_listener (ip::udp::endpoint local_endpoint);
    ~packet_listener ();

    int open (io_service *io_service, void *msghandler);

    // close connection & unregister from io_service
    int close (void);

protected:

    //Start the receiving loop
    void start_receive ();

    // handling incoming message
    int handle_input (const boost::system::error_code& error,
                      std::size_t bytes_transferred);

    // if handle_input() returns -1, reactor would call handle_close()
    int handle_close ();

    size_t send (const void *buf, size_t n, ip::udp::endpoint remote_endpoint);

private:

    // secure connection stream
    bool                        _secure;

    ip::udp::socket             *_udp;
    ip::udp::endpoint           _local_endpoint;
    ip::udp::endpoint           _remote_endpoint_;

    // the same io_service as net_udp
    io_service                  *_io_service;
    boost::thread               *_iosthread;
    void                        *_msghandler;

    // generic buffer
    char _buf[BUFSIZ];

    //Message store
    std::deque<RLNCMessage> msgs;

};

#endif // PACKET_LISTENER_H
