#include "net_udp_handler.h"
#include "net_udp.h"
#include <iostream>
#include <cstring>
#include <string>
#include "VAST.h"
#include "VASTnet.h"
#include "logger.h"

namespace Vast {

    net_udp_handler::net_udp_handler(ip::udp::endpoint local_endpoint)
    {
        _udpsocket = NULL;
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
        if (_udpsocket != NULL)
        {
            delete _udpsocket;
            _udpsocket = NULL;
        }

        if (_iosthread != NULL)
        {
            delete _iosthread;
            _iosthread = NULL;
        }
    }

    int net_udp_handler::open(boost::asio::io_service *io_service, abstract_net_udp *msghandler, bool startthread) {
        CPPDEBUG("net_udp_handler::open " << std::endl);
        is_open = true;
        _io_service = io_service;
        _msghandler = msghandler;

        //Open the UDP socket for listening
        if (_udpsocket == NULL) {
            _udpsocket = new ip::udp::socket(*_io_service);
            _udpsocket->open(ip::udp::v4());
            _udpsocket->set_option(ip::udp::socket::reuse_address(true));

            boost::system::error_code ec;
            uint16_t port = _local_endpoint.port();

            do
            {
                //Search for an open port to use
                //Save port number
                _local_endpoint.port(port);
                _udpsocket->bind(_local_endpoint, ec);

                CPPDEBUG("net_udp_handler::open " + ec.message() << std::endl);
                //Try the next port
                port++;

            } while (ec);

            //Open dest_unr_listener as well to receive disconnects
//            _disconn_listener = new dest_unreachable_listener(*io_service, _local_endpoint.address().to_string().c_str(), this);


            //Add async receive to io_service queue
            start_receive();

            CPPDEBUG("net_udp_handler::open _udp->_local_endpoint: " << _udpsocket->local_endpoint() << " _local_endpoint" << _local_endpoint << std::endl);

            if (startthread)
            {
                //Start the thread handling async receives
                _iosthread = new boost::thread(boost::bind(&boost::asio::io_service::run, io_service));
            }

        }

        return 0;
    }

    void net_udp_handler::start_receive()
    {
        _udpsocket->async_receive_from(
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
            IPaddr remote_addr(_remote_endpoint_.address().to_v4().to_ulong(), _remote_endpoint_.port());
            id_t fromhost = net_manager::resolveHostID(&remote_addr);
            process_input(_buf, bytes_transferred, remote_addr, fromhost);

            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("net_udp_handler::handle_input: Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

    /**
     * @brief Process incoming messages, directly received or passed by net_udpNC_handler and net_udpNC_MChandler
     * @param buffer Buffer containing VAST message
     * @param bytes_transferred Number of bytes in the buffer
     * @param remote_addr Source address of VAST message, if available
     * @param fromhost From ID of sender, only unavailable in ID_REQUEST messages
     */
    void net_udp_handler::process_input(const char *buffer, std::size_t bytes_transferred,
                                        IPaddr remote_addr, id_t fromhost)
    {
        //Process UDP messages
        size_t n = bytes_transferred;
        VASTHeader header;

        //Contents is char, not pointer
        const char *p = buffer;

        if (!(remote_addr.host == 0 && remote_addr.port == 0))
        {
//            CPPDEBUG("net_udp_handler::process_input Using a unicast packet" << std::endl);
        }
        else
        {
            CPPDEBUG("net_udp_handler::process_input Using a multicast packet" << std::endl);
        }

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
            if (msg.deserialize (p, header.msg_size) > 0)
            {
//                Logger::debugPeriodic("net_udp_handler::proces_input MSG_TYPE " + VAST::MSGTYPEtoString(msg.msgtype)
//                                      + "(" + std::to_string(msg.msgtype) + ")");
            }

            id_t temp_id = fromhost;

            //Check if this is a legitimate address, i.e. both host and port is not 0
            if (!(remote_addr.host == 0 && remote_addr.port == 0))
            {
                //Store the host_id : IPaddr pair
                //Check if this works correctly...
//                Logger::debug();

                //Always use the from IP to generate a fromhost - needed by forwarded messages:
                //fromhost can be != msg.from - the message is from ID given, but forwarded through
                // fromhost
                temp_id = net_manager::resolveHostID(&remote_addr);
                if (temp_id != fromhost)
                {
                    CPPDEBUG("net_udp_handler::process_input Something strange happened - fromhost != resolveHostID: "
                             << fromhost << ":" << temp_id << std::endl);
                }

                if (getRemoteAddress (temp_id))
                {
                    if (!(*getRemoteAddress (temp_id) == remote_addr))
                    {
                        CPPDEBUG("net_udp_handler::process_input Id [" + std::to_string(temp_id) + "] already has an address: " << std::endl);
                        char ip_string2[30];
                        getRemoteAddress(temp_id)->getString(ip_string2);
                        CPPDEBUG("saved address: " << std::string(ip_string2) << std::endl);
                        remote_addr.getString (ip_string2);
                        CPPDEBUG("new address: " << std::string(ip_string2) << std::endl);
                    }

                }
                storeRemoteAddress(temp_id, remote_addr);

                //We assume if we can get a packet from the host, we are connected to that host
                _msghandler->socket_connected(temp_id, this, false);
            }
            else if (temp_id == NET_ID_UNASSIGNED && msg.msgtype == ID_REQUEST)
            {
//                temp_id = net_manager::resolveHostID(&remote_addr);
                CPPDEBUG("msg.msgtype was ID_REQUEST from: " << fromhost << " socket_addr: " << remote_addr << std::endl);
            }

            if (temp_id == NET_ID_UNASSIGNED)
            {
                CPPDEBUG("temp_id still unassigned even after process remote_addr" << std::endl);
            }

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
        CPPDEBUG("net_udp_handler::close()" << std::endl);
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
            if (_udpsocket != NULL && _udpsocket->is_open())
            {
                _udpsocket->close();
                _io_service->stop();
            }

            if (_iosthread != NULL)
            {
                _iosthread->join();
            }
        }
	
		is_open = false;

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

        if (_udpsocket == NULL)
        {
            std::cerr << "net_udp_handler::send trying to send before socket is ready" << std::endl;
            return -1;

        }

//        CPPDEBUG("net_udp_handler::send size of sent packet: " << n << std::endl);
        return _udpsocket->send_to(buffer(msg, n), remote_endpoint);

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

    const IPaddr* net_udp_handler::getRemoteAddress (id_t host_id)
    {

//        CPPDEBUG("net_udp_handler::getRemoteAddress" << std::endl);

        for (auto iter = _remote_addrs.begin(); iter != _remote_addrs.end(); ++iter)
        {
            char ip_string[30];
            iter->second.getString(ip_string);
            IPaddr addr_from_id((iter->first >> 32), 1037);
            if (!(addr_from_id == iter->second))
            {
                CPPDEBUG("id: " << iter->first << " - " << std::string(ip_string) << std::endl);
                CPPDEBUG(addr_from_id << " IPaddr from id: " << std::endl);
                CPPDEBUG("Equal: " << (addr_from_id == iter->second) << std::endl);
            }

        }

        //Return the address if we have heard from this host before
        if (_remote_addrs.find (host_id) != _remote_addrs.end ())
        {
//            CPPDEBUG("net_udp_handler::getRemoteAddress ["
//                     << std::to_string(host_id) << "]: " << _remote_addrs[host_id] << std::endl);
            return &_remote_addrs[host_id];
        }

        //There was no address found for this host id, return a null address
        else
        {
            CPPDEBUG("net_udp_handler::getRemoteAddress Could not determine address for ["
                     << std::to_string(host_id) << "]" << std::endl);
            return NULL;
        }
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
        if (_remote_addrs[host_id] == addr)
        {
//            CPPDEBUG("net_udp_handler::storeRemoteAddress Got the same address again, not saving" << std::endl);
            return;
        }

        else if (_remote_addrs.find (host_id) != _remote_addrs.end())
        {
            CPPDEBUG("net_udp_handler::storeRemoteAddress Trying to replace address for ["
                     + std::to_string(host_id)
                     + "] (previously [" << _remote_addrs[host_id].getString() << "]) with "
                     << "[" << addr << "]" << std::endl);
        }

        IPaddr addr_from_id((host_id >> 32), 1037);
        if (!(addr_from_id == addr))
        {
            CPPDEBUG("net_udp_handler::storeRemoteAddress: " << std::endl << addr << std::endl);
            CPPDEBUG(addr_from_id << " IPaddr from id: " << std::endl);
            CPPDEBUG("Equal: " << (addr_from_id == addr) << std::endl);
        }

         if (host_id != 0 && !(addr_from_id == addr))
         {
             CPPDEBUG("Using fromID as IP address"<<std::endl);
             addr = addr_from_id;
         }
         else if (host_id == 0)
         {
             CPPDEBUG("Host ID was 0" << std::endl);
         }

        _remote_addrs[host_id] = addr;

//        char ip_addr_str[22] = "";
//        addr.getString(ip_addr_str);
//        if (strlen(ip_addr_str) == 0)
//        {
//            std::cerr << "net_udp_handler::storeRemoteAddress IP addr was probably not copied to ip_addr_str" << std::endl;
//        }

        _remote_ids_IPs[addr] = host_id;

        for (auto iter = _remote_addrs.begin(); iter != _remote_addrs.end(); ++iter)
        {
            char ip_string[30];
            iter->second.getString(ip_string);

            for (auto iter2 = _remote_addrs.begin(); iter2 != _remote_addrs.end(); ++iter2)
            {
                if (iter->first != iter2->first && iter->second == iter2->second)
                {
                    char ip_string2[30];
                    iter2->second.getString(ip_string2);
                    CPPDEBUG("net_udp_handler::storeRemoteAddress : Found duplicate IP addr" << std::endl);
                    CPPDEBUG("id2: " << iter2->first << " - " << std::string(ip_string2) << std::endl);
                    CPPDEBUG("id : " << iter->first << " - " << std::string(ip_string) << std::endl);
                }
            }

//            CPPDEBUG("id: " << iter->first << " - " << std::string(ip_string) << std::endl);
        }
    }

    uint16_t net_udp_handler::getPort ()
    {
        return _local_endpoint.port();
    }

    bool net_udp_handler::isOpen()
    {
        return is_open;
    }

}
