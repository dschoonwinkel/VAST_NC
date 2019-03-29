#include "net_udp_handler.h"
#include "net_udp.h"
#include <iostream>
#include <cstring>
#include <string>

namespace Vast {

    net_udp_handler::net_udp_handler(ip::udp::endpoint local_endpoint)
    {
        _udp = NULL;
        _io_service = NULL;
        _iosthread = NULL;
        _local_endpoint = local_endpoint;
        _remote_ids.clear();
        _remote_ids.push_back(0);
    }

    net_udp_handler::~net_udp_handler()
    {
        // remove UDP listener, net_udp will delete itself
        CPPDEBUG("~net_udp_handler" << std::endl);
        _udp = NULL;
        _io_service->stop ();
    }

    int net_udp_handler::open(boost::asio::io_service *io_service, abstract_net_udp *msghandler) {
        _io_service = io_service;
        _msghandler = msghandler;

        //Open the UDP socket for listening
        if (_udp == NULL) {
            _udp = new ip::udp::socket(*_io_service);
            _udp->open(ip::udp::v4());
            _udp->set_option(ip::udp::socket::reuse_address(true));

            boost::system::error_code ec;
            uint16_t port = _local_endpoint.port();

            do
            {
                //Search for an open port to use
                //Save port number
                _local_endpoint.port(port);
                _udp->bind(_local_endpoint, ec);

                CPPDEBUG("net_udp_handler::open " + ec.message() << std::endl);
                //Try the next port
                port++;

            } while (ec);

            //Open dest_unr_listener as well to receive disconnects
//            _disconn_listener = new dest_unreachable_listener(*io_service, _local_endpoint.address().to_string().c_str(), this);


            //Add async receive to io_service queue
            start_receive();

            CPPDEBUG("net_udp_handler::open _udp->_local_endpoint: " << _udp->local_endpoint() << " _local_endpoint" << _local_endpoint << std::endl);

            //Start the thread handling async receives
            _iosthread = new boost::thread(boost::bind(&boost::asio::io_service::run, io_service));

        }

        return 0;
    }

    void net_udp_handler::start_receive()
    {
        _udp->async_receive_from(
            boost::asio::buffer(_buf, VAST_BUFSIZ), _remote_endpoint_,
            boost::bind(&net_udp_handler::handle_input, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }

    // handling incoming message
    int net_udp_handler::handle_input (const boost::system::error_code& error,
          std::size_t bytes_transferred)
    {

        if (!error)
        {
            process_input(_buf, bytes_transferred);

            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("net_udp_handler::handle_input: Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

    void net_udp_handler::process_input(const char *buffer, std::size_t bytes_transferred, size_t offset)
    {
        //Process UDP messages
        size_t n = bytes_transferred - offset;
        VASTHeader header;
        id_t remote_id;

        //Contents is char, not pointer
        const char *p = buffer + offset;

        //NOTE: there may be several valid UDP messages received at once -- is this really necessary?
        while (n > sizeof (VASTHeader))
        {
            //extract message header
            mempcpy (&header, p, sizeof (VASTHeader));
            n -= sizeof(VASTHeader);
            p += sizeof(VASTHeader);

            //Check if it is really a VAST message: Start and end bytes of header should be correct
            if (!(header.start == 10 && header.end == 5))
            {
                CPPDEBUG("net_udp_handler::process_input Non-VAST message received on UDP socket" << std::endl);
                return;
            }

            Message msg(0);
            if (0 == msg.deserialize (p, header.msg_size))
            {
                printf("net_udp_handler::handle_input deserialize message fail: size = %u\n", header.msg_size);
            }
            remote_id = msg.from;

            //Store the host_id : IPaddr pair
            IPaddr remote_addr(_remote_endpoint_.address().to_v4().to_ulong(), _remote_endpoint_.port());


            id_t temp_id = remote_id;
            //This host is looking for an ID, assign it a temporary ID to store the connection
            if (remote_id == NET_ID_UNASSIGNED && msg.msgtype == ID_REQUEST)
            {
                temp_id = net_manager::resolveHostID(&remote_addr);
            }

//                _remote_addrs[remote_id] = remote_addr;
            storeRemoteAddress(temp_id, remote_addr);

            //We assume if we can get a packet from the host, we are connected to that host
            _msghandler->socket_connected(temp_id, this, false);

            //Break up messages into VASTMessage sizes
            //msg start at p - 4, i.e. start of header
            //msgsize = header.msg_size + 4 for header
            _msghandler->msg_received(temp_id, p - sizeof(VASTHeader), header.msg_size + sizeof(VASTHeader));

            //Next message
            p += header.msg_size;
            n -= header.msg_size;
        }
    }

    int net_udp_handler::close() {
        return this->handle_close();
    }

    int net_udp_handler::handle_close()
    {
        //Unregister from message handler, do this first to avoid sending messages
        //on a closed socket
        CPPDEBUG("net_udp_handler::handle_close() - _remote_ids.size(): " << _remote_ids.size() << std::endl;);
//        for (id_t remote_id : _remote_ids)
//        {
//            CPPDEBUG("net_udp_handler::handle_close() - remote id" << remote_id << std::endl;);
//            if (_msghandler == NULL)
//                CPPDEBUG("net_udp_handler::handle_close() _msghandler was null");
//            _msghandler->socket_disconnected(remote_id);
//        }


        if (_io_service != NULL) {
//            _io_service->reset();
            if (_udp != NULL && _udp->is_open())
            {
                _udp->close();
            }

            _io_service->stop();
            _iosthread->join();
        }

        return 0;
    }

    void net_udp_handler::handle_disconnect (IPaddr ip_addr)
    {

        char ip_addr_str[22] = "";
        ip_addr.getString(ip_addr_str);
        CPPDEBUG("net_udp_handler::handle_disconnect: Incoming addr to disconnect" << std::string(ip_addr_str) << std::endl);
        id_t remote_disconnected_id = getRemoteIDByIP(ip_addr);
        if (remote_disconnected_id != NET_ID_UNASSIGNED)
        {
            CPPDEBUG("net_udp_handler::handle_disconnect. Disconnecting ID " << remote_disconnected_id << std::endl);
            _msghandler->socket_disconnected(remote_disconnected_id);
        }
    }

    size_t net_udp_handler::send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint) {

        if (_udp == NULL)
        {
            std::cerr << "net_udp_handler::send trying to send before socket is ready" << std::endl;
            return -1;

        }

//        CPPDEBUG("net_udp_handler::send size of sent packet: " << n << std::endl);
        return _udp->send_to(buffer(msg, n), remote_endpoint);

    }

    bool net_udp_handler::switchRemoteID (id_t oldID, id_t newID) {
        std::vector<id_t>::iterator it;

        //I do not think this functionality is needed for UDP connections
        it = std::find(_remote_ids.begin(), _remote_ids.end(), oldID);

//        //Could not find old ID, nothing to replace
        if (it == _remote_ids.end())
            return false;
        else
        {
            //Replace the oldID with the new one
            *it = newID;
            return true;
        }
        return true;
    }

    IPaddr* net_udp_handler::getRemoteAddress (id_t host_id)
    {
        //Return the address if we have heard from this host before
        if (_remote_addrs.find (host_id) != _remote_addrs.end ())
        {
            return &_remote_addrs[host_id];
        }
        //There was no address found for this host id, return a null address
        else return NULL;
    }

    id_t net_udp_handler::getRemoteIDByIP (IPaddr addr)
    {
//        char ip_addr_str[22] = "";
//        addr.getString(ip_addr_str);

        if (_remote_ids_IPs.find (addr) != _remote_ids_IPs.end())
        {
            return _remote_ids_IPs[addr];
        }
        else
        {

//            for (std::map<IPaddr, id_t>::iterator it = _remote_ids_IPs.begin(); it!= _remote_ids_IPs.end(); it++)
//            {
//                CPPDEBUG(it->first << "  " << it->second << std::endl);
//            }
            std::cerr << "net_udp_handler::getRemoteIDByIP id_t could not be found" << std::endl;
            return NET_ID_UNASSIGNED;
        }
    }

    void net_udp_handler::storeRemoteAddress (id_t host_id, IPaddr addr)
    {
        _remote_addrs[host_id] = addr;

//        char ip_addr_str[22] = "";
//        addr.getString(ip_addr_str);
//        if (strlen(ip_addr_str) == 0)
//        {
//            std::cerr << "net_udp_handler::storeRemoteAddress IP addr was probably not copied to ip_addr_str" << std::endl;
//        }

        _remote_ids_IPs[addr] = host_id;
    }

    uint16_t net_udp_handler::getPort ()
    {
        return _local_endpoint.port();
    }

}
