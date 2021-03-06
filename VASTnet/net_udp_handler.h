#ifndef NET_UDP_HANDLER_H
#define NET_UDP_HANDLER_H

#include "VASTTypes.h"
#include "VASTBuffer.h"
#include "net_manager.h"
#include "abstract_net_udp.h"
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
        virtual ~net_udp_handler ();

        virtual int open (io_service *io_service, abstract_net_udp *msghandler, bool startthread = true);

        /**
         * @brief close connection & unregister from io_service
         * @return
         */
        virtual int close (void);

        void handle_disconnect (IPaddr ip_addr);

        virtual size_t send (const char *msg, size_t n, ip::udp::endpoint remote_endpoint);

        void process_input(const char* buffer, std::size_t bytes_transferred, IPaddr remote_addr,
                           id_t fromhost = NET_ID_UNASSIGNED);

        // obtain address of remote host
        const IPaddr *getRemoteAddress (id_t host_id);

        // obtain id_t of remote host IP
        id_t getRemoteIDByIP (IPaddr ip);

        //Store address of remote host by ID
        void storeRemoteAddress (id_t host_id, IPaddr addr);

        // swtich remote ID to a new one
        virtual bool switchRemoteID (id_t oldID, id_t newID);

        bool isOpen();

        //Do nothing here, for now
        virtual void tick() {}

    protected:

        //Start the receiving loop
        virtual void start_receive ();

        // handling incoming message
        virtual int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        // if handle_input() returns -1, reactor would call handle_close()
        int handle_close ();

        uint16_t getPort ();

        bool                        is_open = false;

        ip::udp::socket             *_udpsocket;
        ip::udp::endpoint           _remote_endpoint_;
        ip::udp::endpoint           _local_endpoint;
        char                        _buf[VAST_BUFSIZ];
        abstract_net_udp            *_net_udp;

        size_t packets_received = 0;
        size_t stacked_packets_received = 0;

    private:



        // info the remote nodes using this socket
        std::vector<id_t>           _remote_ids;
        std::map<id_t, IPaddr>      _remote_addrs;
        std::map<IPaddr, id_t>      _remote_ids_IPs;

        // secure connection stream
        bool                        _secure;

        // the same io_service as net_udp
        io_service                  *_io_service = NULL;
        boost::thread               *_iosthread = NULL;
        dest_unreachable_listener   *_disconn_listener;

        // generic buffer
//        VASTBuffer       _buf;
    };

} // end namespace Vast

#endif // NET_UDP_HANDLER_H
