#include "net_overhearing_handler.h"
#include "net_overhearing.h"
#include <iostream>

namespace Vast {

    net_overhearing_handler::net_overhearing_handler()
    {
        _udp = NULL;
        _io_service = NULL;
        _iosthread = NULL;
    }

    net_overhearing_handler::~net_overhearing_handler()
    {
        // remove UDP listener
        if (_udp != NULL)
            delete _udp;
    }

    int net_overhearing_handler::open(boost::asio::io_service *io_service, void *msghandler, id_t remote_id) {
        _io_service = io_service;
        _msghandler = msghandler;

        if (remote_id != NET_ID_UNASSIGNED) {
//            remote_id = ((net_manager *)msghandler)->resolveHostID(&_remote_addr);
            _remote_ids.push_back(remote_id);
            if (((net_overhearing*)msghandler)->socket_connected(remote_id, this, _secure) == false)
            {
                std::cerr << "Socket connection failed, maybe connection is already established" << std::endl;
            }
        }
        else {
            std::cerr << "net_overhearing_handler::open opening to NET_ID_UNASSIGNED failed" << std::endl;
        }

        return 0;
    }

    void net_overhearing_handler::start_receive()
    {
        _udp->async_receive_from(
            boost::asio::buffer(_buf.data, _buf.size), _remote_endpoint_,
            boost::bind(&net_overhearing_handler::handle_input, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }

    // handling incoming message
    int net_overhearing_handler::handle_input (const boost::system::error_code& error,
          std::size_t bytes_transferred)
    {
        size_t n = bytes_transferred;
        VASTHeader header;
        id_t remote_id;

        if (!error)
        {
            //Store UDP messages

            char *p = _buf.data;

            //NOTE: there may be several valid UDP messages received at once -- is this really necessary?
            while (n > sizeof (VASTHeader))
            {
                //extract message header
                mempcpy (&header, p, sizeof (VASTHeader));
                n -= sizeof(VASTHeader);
                p += sizeof(VASTHeader);

                //Next message
                p += header.msg_size;
                n -= header.msg_size;

                Message *msg = new Message(0);
                if (0 == msg->deserialize (p, header.msg_size))
                {
                    printf("net_overhearing_handler::handle_input deserialize message fail: size = %lu\n", header.msg_size);
                }
                remote_id = msg->from;

                //Store the host_id : IPaddr pair
                IPaddr remote_addr(_remote_endpoint_.address().to_v4().to_ulong(), _remote_endpoint_.port());
                _remote_addrs[remote_id] = remote_addr;

                ((net_overhearing*)_msghandler)->msg_received(remote_id, p, header.msg_size);
            }


            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message());
        }
        return -1;
    }

    int net_overhearing_handler::close() {
        return this->handle_close();
    }

    int net_overhearing_handler::handle_close()
    {
        //Unregister from message handler, do this first to avoid sending messages
        //on a closed socket
        for (id_t remote_id : _remote_ids)
        {
            ((net_overhearing*)_msghandler)->socket_disconnected(remote_id);
        }

        if (_io_service != NULL) {
            _io_service->stop();
            _iosthread->join();
        }

        _udp->close();
        delete this;
        return 0;
    }

    size_t net_overhearing_handler::send(const void *buf, size_t n, ip::udp::endpoint remote_endpoint) {

        if (_udp == NULL)
        {
            return -1;
            std::cerr << "net_overhearing_handler: trying to send before socket is ready" << std::endl;
        }

        return _udp->send_to(buffer(buf, n), remote_endpoint);

    }

    bool net_overhearing_handler::switchRemoteID (id_t oldID, id_t newID) {
        std::vector<id_t>::iterator it;

        it = std::find(_remote_ids.begin(), _remote_ids.end(), oldID);

        //Could not find old ID, nothing to replace
        if (it == _remote_ids.end())
            return false;
        else
        {
            //Replace the oldID with the new one
            *it = newID;
            return true;
        }
    }

    IPaddr net_overhearing_handler::getRemoteAddress(id_t host_id)
    {
        //Return the address if we have heard from this host before
        if (_remote_addrs.find(host_id) != _remote_addrs.end())
        {
            return _remote_addrs[host_id];
        }
        //There was no address found for this host id, return a null address
        else return IPaddr();
    }

}
