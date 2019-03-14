#ifndef PACKET_LISTENER_H
#define PACKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "rlnc_packet_factory.h"
#include "rlncmessage.h"
#include "rlncrecoder.h"
#include <deque>
#include <mutex>

using namespace boost::asio;

class packet_listener
{
public:
    packet_listener (ip::udp::endpoint local_endpoint);
    ~packet_listener ();

    virtual int open (io_service *io_service, void *msghandler);

    // close connection & unregister from io_service
    virtual int close (void);

    size_t getRecvMsgsCount();

protected:

    //Start the receiving loop
    virtual void start_receive ();

    // handling incoming message
    virtual int handle_input (const boost::system::error_code& error,
                      std::size_t bytes_transferred);

    // if handle_input() returns -1, reactor would call handle_close()
    virtual int handle_close ();

    virtual void start_send();
    virtual size_t send (const char *buf, size_t n, ip::udp::endpoint remote_endpoint);
    virtual void handle_send_to(const boost::system::error_code& error, std::size_t bytes_transferred);

    // secure connection stream
    bool                        _secure;

    ip::udp::socket             *_udp = NULL;
    ip::udp::endpoint           _local_endpoint;
    ip::udp::endpoint           _remote_endpoint_;

    // the same io_service as net_udp
    io_service                  *_io_service;
    boost::thread               *_iosthread_recv;
    boost::thread               *_iosthread_send;
    void                        *_msghandler;

    RLNCrecoder                 recoder;

    // generic buffer
    char _buf[BUFSIZ];
    char _sendbuf[BUFSIZ];

    //Message store
    std::deque<RLNCMessage> msgs;
    std::mutex msgs_mutex;

    size_t coded_msgs_sent = 0;
    size_t total_coded_msgs_sent = 0;
    size_t recv_msgs_count = 0;

};

#endif // PACKET_LISTENER_H
