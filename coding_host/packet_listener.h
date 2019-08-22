#ifndef PACKET_LISTENER_H
#define PACKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "rlnc_packet_factory.h"
#include "rlncmessage.h"
#include "rlncrecoder.h"
#include "abstract_sender.h"
#include "abstract_input_processor.h"
#include <deque>
#include <mutex>

#define PACKET_DEQUE_TOO_LONG 100

using namespace boost::asio;

class packet_listener : public abstract_sender
{
public:
    packet_listener (ip::udp::endpoint local_endpoint);
    virtual ~packet_listener ();

    virtual int open (io_service *io_service, abstract_sender *sender = NULL);

    virtual void process_input (const char *buf,
                           std::size_t bytes_transferred);

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
    virtual void handle_send_to(const boost::system::error_code& errcode, std::size_t bytes_transferred);

    // secure connection stream
    bool                        _secure;

    ip::udp::socket             *_recv_udp = NULL;
    ip::udp::socket             *_send_udp = NULL;
    ip::udp::endpoint           _local_endpoint;
    ip::udp::endpoint           _MC_address;
    ip::udp::endpoint           _remote_endpoint_;

    // the same io_service as net_udp
    io_service                  *_recv_io_service = NULL;
    io_service                  *_send_io_service = NULL;
    io_service::work            *_send_io_service_work = NULL;
    boost::thread               *_iosthread_recv = NULL;
    boost::thread               *_iosthread_send_ioservice = NULL;
    boost::thread               *_iosthread_send_startsend = NULL;

    abstract_sender             *_sender = NULL;


    bool                        running = true;

    RLNCrecoder                 recoder;

    // generic buffer
    char _buf[BUFSIZ];
    char _sendbuf[BUFSIZ];

    //Message store
    std::deque<RLNCMessage> msgs;
    std::mutex msgs_mutex;

    size_t total_coded_msgs_sent = 0;
    size_t total_resets = 0;
    size_t recv_msgs_count = 0;
    size_t process_msg_count = 0;
    size_t total_msgstoolong_cleared = 0;
    std::map<Vast::id_t, size_t> recvfrommap_count;

};

#endif // PACKET_LISTENER_H
