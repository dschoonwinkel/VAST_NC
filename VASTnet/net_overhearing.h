/*
 * Extra networking implementation by Daniel Schoonwinkel (2018)
 *
 */

/*
 * net_overhearing.h -- network implementation using wireless overhearing
 *                  
 *
 *      init:   2010-09-30
 */

#ifndef VAST_NET_OVERHEARING_H
#define VAST_NET_OVERHEARING_H

#include "VASTnet.h"
    // Wireless overhearing class
#include "net_overhearing_handler.h"

#include <boost/asio.hpp>
#include <mutex>

namespace Vast {
    class net_overhearing : public Vast::net_manager
    {
    friend class net_overhearing_handler;

    public:

        net_overhearing (uint16_t port);
        virtual ~net_overhearing();
     
        //
        // basic services 
        //
        
        // starting and stopping the network service
        void start ();
        void stop ();

        // get current physical timestamp (milliseconds since start)
        timestamp_t getTimestamp ();

        // get IP address from host name
        const char *getIPFromHost (const char *host = NULL);

        // obtain the IP / port of a remotely connected host
        bool getRemoteAddress (id_t host_id, IPaddr &addr);

        // connect or disconnect a remote node (should check for redundency)
        bool connect (id_t target, unsigned int host, unsigned short port, bool is_secure = false);
        bool disconnect (id_t target);

        // send an outgoing message to a remote host, if 'addr' is provided, the message is sent as UDP
        // return the number of bytes sent
        size_t send (id_t target, char const *msg, size_t size, const Addr *addr = NULL);

        // receive an incoming message
        // return pointer to valid NetSocketMsg structure or NULL for no messages
        NetSocketMsg *receive ();

        // change the ID for a remote host
        bool switchID (id_t prevID, id_t newID);

        // perform a tick of the logical clock 
        void tickLogicalClock ();

        //
        // callback services called by specific network binding (e.g. ACE or emulation)
        //

        // store a message into priority queue
        // returns success or not
        bool msg_received (id_t fromhost, const char *message, size_t size, timestamp_t recvtime = 0, bool in_front = false);

        bool socket_connected (id_t id, void *stream, bool is_secure);
        bool socket_disconnected (id_t id);

    private:
        // bind port for this node
        uint16_t                    _port_self;

        // hostname & IP of current host
        char                        _hostname[255];
        char                        _IPaddr[17];

        //Handler for receiving UDP packets
        net_overhearing_handler     *_udphandler;

        //Event loop for handling async IO.
        boost::asio::io_service     *_io_service;

        std::mutex                  _msg_mutex;
        std::mutex                  _conn_mutex;

        const double                UDP_TIMEOUT = 5000;

    };
} // end namespace Vast

#endif // VAST_NET_OVERHEARING_H
