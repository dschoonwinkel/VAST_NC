#ifndef NET_OVERHEARING_HANDLER_H
#define NET_OVERHEARING_HANDLER_H

#include "VASTTypes.h"
#include "VASTBuffer.h"
#include "net_manager.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

namespace Vast {

    class net_overhearing_handler
    {
    friend class net_overhearing;

    public:
        net_overhearing_handler (char IPaddr[], uint16_t port);

        int open (io_service *io_service, void *msghandler);

        // close connection & unregister from io_service
        int close (void);

        // obtain address of remote host
        IPaddr *getRemoteAddress(id_t host_id);

        // swtich remote ID to a new one
        bool switchRemoteID (id_t oldID, id_t newID);

    protected:

        //Start the receiving loop
        void start_receive();

        // handling incoming message
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        // if handle_input() returns -1, reactor would call handle_close()
        int handle_close ();

        size_t send(const void *buf, size_t n, ip::udp::endpoint remote_endpoint);

        uint16_t getPort();

    private:

        ~net_overhearing_handler ();

        // info the remote nodes using this socket
        std::vector<id_t>           _remote_ids;
        std::map<id_t, IPaddr>      _remote_addrs;

        // secure connection stream
        bool                        _secure;

        ip::udp::socket             *_udp;
        ip::udp::endpoint           _local_endpoint;
        ip::udp::endpoint           _remote_endpoint_;

        // the same io_service as net_overhearing
        io_service                  *_io_service;
        boost::thread               *_iosthread;
        void                        *_msghandler;

        // generic buffer
        VASTBuffer       _buf;
    };

} // end namespace Vast

#endif // NET_OVERHEARING_HANDLER_H
