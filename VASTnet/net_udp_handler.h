#ifndef NET_UDP_HANDLER_H
#define NET_UDP_HANDLER_H

#include "VASTTypes.h"
#include "VASTBuffer.h"
#include "net_manager.h"
#include "dest_unr_listener.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

namespace Vast {

    class net_udp_handler
    {
    friend class net_udp;
    friend class dest_unreachable_listener;

    public:
        net_udp_handler (ip::udp::endpoint local_endpoint);

        int open (io_service *io_service, void *msghandler);

        // close connection & unregister from io_service
        int close (void);

        // obtain address of remote host
        IPaddr *getRemoteAddress (id_t host_id);

        // obtain id_t of remote host IP
        id_t getRemoteIDByIP (IPaddr ip);

        //Store address of remote host by ID
        void storeRemoteAddress (id_t host_id, IPaddr addr);

        // swtich remote ID to a new one
        bool switchRemoteID (id_t oldID, id_t newID);

        void handle_disconnect (IPaddr ip_addr);

    protected:

        //Start the receiving loop
        void start_receive ();

        // handling incoming message
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        // if handle_input() returns -1, reactor would call handle_close()
        int handle_close ();

        virtual size_t send (const char *msg, size_t n, ip::udp::endpoint remote_endpoint);

        uint16_t getPort ();

        ~net_udp_handler ();

        ip::udp::socket             *_udp;

    private:



        // info the remote nodes using this socket
        std::vector<id_t>           _remote_ids;
        std::map<id_t, IPaddr>      _remote_addrs;
        std::map<IPaddr, id_t>      _remote_ids_IPs;

        // secure connection stream
        bool                        _secure;

        ip::udp::endpoint           _local_endpoint;
        ip::udp::endpoint           _remote_endpoint_;

        // the same io_service as net_udp
        io_service                  *_io_service;
        boost::thread               *_iosthread;
        void                        *_msghandler;
        dest_unreachable_listener   *_disconn_listener;

        // generic buffer
//        VASTBuffer       _buf;
        char _buf[VAST_BUFSIZ];
    };

} // end namespace Vast

#endif // NET_UDP_HANDLER_H
