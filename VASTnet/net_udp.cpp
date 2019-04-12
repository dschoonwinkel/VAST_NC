/*
 * VAST, a scalable peer-to-peer network for virtual environments
 * Copyright (C) 2005-2011 Shun-Yun Hu (syhu@ieee.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "net_udp.h"
#include "net_udp_handler.h"
#include "net_udpNC_handler.h"
#include <map>
#include <chrono>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

using namespace boost::asio;
using namespace std::chrono_literals;

namespace Vast
{
    net_udp::net_udp (uint16_t port, const char *bindAddress, VAST_NetModel net_model): _port_self (port)
    {
        CPPDEBUG("net_udp::constructor called" << std::endl);

//        // initialize rand generator (for node fail simulation, NOTE: same seed is used to produce exactly same results)
//        //srand ((unsigned int)time (NULL));

        _hostname[0] = 0;
        _IPaddr[0] = 0;

        if (bindAddress != NULL && bindAddress[0] != 0)
        {
            strcpy(_IPaddr, bindAddress);
        }

        _io_service = new io_service();

        printf ("net_udp::net_udp(): Host IP: %s\n", getIPFromHost ());

        ip::udp::endpoint endpoint(ip::address::from_string(_IPaddr), port);

        //NOTE: Potential bug -> what happens if it cannot open communication on this port?
        _self_addr.setPublic(endpoint.address().to_v4().to_ulong(), _port_self);

        //Assign the host_id based on the combination of host_ip and port information
        _self_addr.host_id = this->resolveHostID(&_self_addr.publicIP);

        char ip_string[20];
        _self_addr.publicIP.getString(ip_string);
        printf ("net_udp::constructor: _self_addr: %s : %d\n", ip_string, _self_addr.publicIP.port);

        if (net_model == VAST_NET_UDP)
            _udphandler = new net_udp_handler(endpoint);
        else if (net_model == VAST_NET_UDPNC)
            _udphandler = new net_udpNC_handler(endpoint);

        // set the conversion rate between seconds and timestamp unit
        // for net_ace it's 1000 timestamp units = 1 second (each step is 1 ms)
        _sec2timestamp = 1000;

        CPPDEBUG("net_udp::constructor getPublicIPaddr() : " << getPublicIPaddr ());

    }

    net_udp::~net_udp ()
    {
        //Maybe this should rather be done in net_udp lifecycle?
//        _io_service.reset();
        delete _io_service;
        _io_service = NULL;

        CPPDEBUG("~net_udp() " << std::endl);
        if (_udphandler)
        {
            delete _udphandler;
        }
    }

    void 
    net_udp::start ()
    {
        CPPDEBUG("net_udp::start" << std::endl);
        _active = true;
        net_manager::start ();

        _udphandler->open(_io_service, this);

        //Update the port number once it was determine by the _udphandler
        _port_self = _udphandler->getPort();
        _self_addr.setPublic(_self_addr.publicIP.host, _port_self);

        //Assign the host_id based on the combination of host_ip and port information
        _self_addr.host_id = this->resolveHostID(&_self_addr.publicIP);

        //This cannot work, otherwise it does not request its ID from GW.
//        //Set up my temporary ID to enable communication
//        _id = _self_addr.host_id;

        socket_connected(_self_addr.host_id, _udphandler, false);
    }

    void 
    net_udp::stop ()
    {
        CPPDEBUG("net_udp::stop" << std::endl);
        net_manager::stop ();
        if (_udphandler)
            _udphandler->close ();

        _io_service->stop();

        _active = false;

    }

    // get current physical timestamp
    timestamp_t 
    net_udp::getTimestamp ()
    {
        uint64_t time_now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return time_now;
    }

    // get IP address from host name
    const char *
    net_udp::getIPFromHost (const char *host)
    {
        char hostname[255];

        //if host is NULL, then we use our own hostname
        if (host == NULL)
        {

            //if we've already looked up, return previous record
            if (_IPaddr[0] != 0) {
                return _IPaddr;
            }

            strcpy(hostname, boost::asio::ip::host_name().c_str());
        }
        else
        {
            strcpy (hostname, host);
        }

        using boost::asio::ip::tcp;

        tcp::resolver resolver(*_io_service);
        tcp::resolver::query query(hostname, "");
        tcp::resolver::iterator iter = resolver.resolve(query);
        tcp::resolver::iterator end; // End marker.
        if(iter != end) {
            tcp::endpoint ep = *iter++;
//            CPPDEBUG(ep << std::endl);
//            CPPDEBUG(ep.address().to_string() << std::endl);

            //If we do not know our own host IP and it has now been resolved
            if (_IPaddr[0] == 0 && host == NULL)
            {
                strcpy(_IPaddr, ep.address().to_string().c_str());
                strcpy(_hostname, hostname);
            }
            return ep.address().to_string().c_str();
        }
        else {
            return NULL;
        }
    }

    // obtain the IP / port of a remotely connected host
    // returns a null address if not available
    //This function only returns a value if we have heard from this host before
    bool 
    net_udp::getRemoteAddress (id_t host_id, IPaddr &addr)
    {

        IPaddr *resolved_addr = _udphandler->getRemoteAddress (host_id);
        CPPDEBUG("net_udp::getRemoteAddress: host_id " << host_id << std::endl);
        //IPaddr could not be found, probably because we have never communicated with it
        if (resolved_addr == NULL)
            return false;

        //Assign the address if it was found
        addr = *resolved_addr;

        return true;
    }

    const IPaddr
    net_udp::getPublicIPaddr ()
    {
        return this->getAddress ().publicIP;
    }

    // Note:
    //      when connecting to an address with connect(Addr& addr), 
    //      id is NET_ID_UNASSIGNED:
    //          means connecting to an outside node, 
    //          so the network layer should allocate a temp & private id and return 
    //          an addr structure 
    //      any non-private id: 
    //          connects to the node, and register addr.id to the address
    bool
    net_udp::
    connect (id_t target, unsigned int host, unsigned short port, bool is_secure)
    {
        if (_active == false)
            return false;

        // avoid self-connection & re-connection
        if (target == _id || isConnected (target))
        {
            printf ("net_udp::connect () connection for [%lu] already exists\n", target);
            return false;
        }        

//        //If the UDP socket is not open yet, open it now
//        _udphandler->open(_io_service, this);
        if (!_udphandler->isOpen ())
            throw std::logic_error("net_udp::connect Trying to connect, but _udphandler is not open yet");

        //Store the address for later use
        _udphandler->storeRemoteAddress(target, IPaddr(host, port));

        //UDP is NOT a connection orientated protocol, thus no connecting is done here
        //As long as the UDP socket is open and we have an addr, it is assumed to be
        //connected
        socket_connected(target, _udphandler, is_secure);

        return true;
    }

    bool
    net_udp::
    disconnect (Vast::id_t target)
    {
        net_udp_handler *handler = NULL;
        std::map<id_t, ConnectInfo>::iterator it;

        //Use conn mutex to ensure that the handler is deleted correctly and
        //cleanly
        _conn_mutex.lock();

        if ((it = _id2conn.find(target)) != _id2conn.end ())
            handler = (net_udp_handler *)(it->second.stream);

#ifdef DEBUG_DETAIL
        printf ("[%lu] disconnection success\n", target);
#endif
        if (handler == NULL)
            return false;

        //General UDP socket should not close, unlike TCP connection
//        handler->close();

        CPPDEBUG("net_udp::disconnect " << target << " disconnected" << std::endl);

        _conn_mutex.unlock();

        this->socket_disconnected (target);
        return true;
    }

    // send an outgoing message to a remote host
    // return the number of bytes sent
    size_t 
    net_udp::send (id_t target, char const *msg, size_t size, const Addr *addr)
    {        
        if (_active == false)
            return 0;

        //Resolve addr from id_t if not given
        if (addr == NULL)
        {
            IPaddr *resolved_addr = _udphandler->getRemoteAddress(target);
            if (resolved_addr == NULL)
            {
                std::cerr << "\n net_udp::send IPaddr could not be resolved for id_t " << target << std::endl;
                return 0;
            }
            Addr temp_addr(target, resolved_addr);
            addr = &temp_addr;
        }

        ip::udp::endpoint target_addr(ip::address_v4(addr->publicIP.host), addr->publicIP.port);
        size_t result = _udphandler->send(msg, size, target_addr);
        return result;
    }

    // receive an incoming message
    // return pointer to next NetSocketMsg structure or NULL for no more message
    NetSocketMsg *
    net_udp::receive ()
    {
        // clear last received message
        if (_recvmsg != NULL)
        {            
            delete _recvmsg;
            _recvmsg = NULL;
        }

        // return if no more message in queue 
        // or all messages are beyond current logical time
  
        if (_recv_queue.size () == 0)
            return NULL;

        //FIFO, return the first message in queue
        _msg_mutex.lock();
        _recvmsg = _recv_queue[0];
        _recv_queue.erase (_recv_queue.begin ());
        _msg_mutex.unlock();

        return _recvmsg;
    }

    // change the ID for a remote host
    bool 
    net_udp::switchID (id_t prevID, id_t newID)
    {
        bool result = false;

//        _conn_mutex.lock();

//        //Only checks for new ID already in use, not for previous ID, because UDP is not connection based and therefore no connection exists before the packet arrives
        if (_id2conn.find (newID) != _id2conn.end ())
////        if (_id2conn.find (newID) != _id2conn.end () || _id2conn.find (prevID) == _id2conn.end ())
        {
            printf("net_udp::switchID: [%lu] new ID already exists\n", _id);
        }

        if (_id2conn.find (prevID) != _id2conn.end() && prevID != newID)
        {
            printf("[%lu] net_udp::switchID () replace [%lu] with [%lu]\n", _id, prevID, newID);

            //copy to new ID
            _id2conn[newID] = _id2conn[prevID];

            //erase old ID mapping
            _id2conn.erase (prevID);

//            change to remote ID knowledge at stream
            result = ((net_udp_handler *)(_id2conn[newID].stream))->switchRemoteID (prevID, newID);
            return result;
        }

//        else  //Simply save the connection ID
//        {
//            _id2conn.insert();
//        }

//        }

//        _conn_mutex.unlock();

//        return result;
        return true;
    }

    // store a message into priority queue
    // returns success or not
    bool 
    net_udp::msg_received (id_t fromhost, const char *message, size_t size, timestamp_t recvtime, bool in_front)
    {        
        NetSocketMsg *msg = new NetSocketMsg;

        msg->fromhost = fromhost;
        msg->recvtime = recvtime;
        msg->size     = size;

        if (size > 0)
        {
            msg->msg = new char[size];
            memcpy (msg->msg, message, size);
        }
        msg->size = size;

        if (recvtime == 0)
            msg->recvtime = this->getTimestamp ();

        // we store message according to message priority
        _msg_mutex.lock();
        if (in_front)
            _recv_queue.insert (_recv_queue.begin(), msg);
        else
            _recv_queue.push_back (msg);
        _msg_mutex.unlock();

        // update last access time of the connection
        _conn_mutex.lock();
        std::map<id_t, ConnectInfo>::iterator it = _id2conn.find (fromhost);
        if (it != _id2conn.end ())
        {
            if (msg->recvtime > it->second.lasttime)
                it->second.lasttime = msg->recvtime; 
        }

        //If I have never heard from this connection before, add it as a new connection
        if (it == _id2conn.end() && message != NULL)
        {
            socket_connected(fromhost, _udphandler, false);
        }
        _conn_mutex.unlock();

        return true;
    }

    //This will probably be replaced by
//    // store a MC message into priority queue
//    // returns success or not
//    //NOTE: MC messages have lost their remote_addr info, so cannot be used for socket_connected
//    bool
//    net_udp::MCmsg_received (id_t fromhost, const char *message, size_t size, timestamp_t recvtime, bool in_front)
//    {
//        //TODO:: Do ordering check here...


//        NetSocketMsg *msg = new NetSocketMsg;

//        msg->fromhost = fromhost;
//        msg->recvtime = recvtime;
//        msg->size     = size;

//        if (size > 0)
//        {
//            msg->msg = new char[size];
//            memcpy (msg->msg, message, size);
//        }
//        msg->size = size;

//        if (recvtime == 0)
//            msg->recvtime = this->getTimestamp ();

//        // we store message according to message priority
//        _msg_mutex.lock();
//        if (in_front)
//            _recv_queue.insert (_recv_queue.begin(), msg);
//        else
//            _recv_queue.push_back (msg);
//        _msg_mutex.unlock();

//        // update last access time of the connection
//        _conn_mutex.lock();
//        std::map<id_t, ConnectInfo>::iterator it = _id2conn.find (fromhost);
//        //Only update if I have heard from this host before
//        if (it != _id2conn.end ())
//        {
//            if (msg->recvtime > it->second.lasttime)
//                it->second.lasttime = msg->recvtime;
//        }

//        _conn_mutex.unlock();

//        return true;
//    }

    // methods to keep track of active connections (associate ID with connection stream)
    // returns NET_ID_UNASSIGNED if failed
    bool
    net_udp::socket_connected (id_t id, void *stream, bool is_secure)
    {
        if (id == NET_ID_UNASSIGNED)
        {
            printf("net_udp::socket_connected () empty id given\n");
            return false;
        }

        //store the connection info
        ConnectInfo conn (stream, getTimestamp (), is_secure);
        bool stored = false;

        _conn_mutex.lock ();
        if (_id2conn.find (id) == _id2conn.end ())
        {
            _id2conn.insert(std::map<id_t, ConnectInfo>::value_type (id, conn));
            stored = true;
        }
        else {
//            //Update last received
//            CPPDEBUG("net_udp::socket_connected Updating lasttime from conn " << id << " to current timestamp " << getTimestamp() << std::endl);
//            _id2conn[id].lasttime = getTimestamp();
        }
        _conn_mutex.unlock();

        //An error occurred - unknown reason
        if (_id2conn.find(id) == _id2conn.end() && stored == false)
        {
            printf("net_udp::socket_connected () connection was not stored\n");
        }

        return stored;
    }

    bool
    net_udp::socket_disconnected (id_t id)
    {
        if (_active == false)
            return false;

        bool success = false;

        _conn_mutex.lock();
        // Cut connection if it exists
        if (_id2conn.find (id) != _id2conn.end ())
        {
            _id2conn.erase (id);
            success = true;
        }
        _conn_mutex.unlock();

        if (success)
        {
            // store a NULL message to indicate disconnection to message queue
            this->msg_received (id, NULL, 0, this->getTimestamp());
        }

        return success;
    }

    net_udp* net_udp::getReal_net_udp ()
    {
        return this;
    }

    //Calculate the UDP timeouts here
    void net_udp::tickLogicalClock ()
    {
//        CPPDEBUG("net_udp::tickLogicalClock ()");
        timestamp_t time_now = getTimestamp();
        std::vector<ConnectInfo> dead_connections;

        std::map<id_t, ConnectInfo>::iterator it;

        for (it = _id2conn.begin(); it != _id2conn.end(); it++ )
        {
//            CPPDEBUG("net_udp::tickLogicalClock: " << it->first  // string (key)
//                      << ':'
//                      << it->second.lasttime   // string's value
//                      << std::endl ;
            if  (time_now > it->second.lasttime + _UDP_TIMEOUT_MS_)
            {
                     dead_connections.push_back(it->second);
//                    CPPDEBUG("Dead connection found: " << it->first << " dead for " << time_now - it->second.lasttime << std::endl);
            }
        }
    }

} // end namespace Vast

