#include "net_udpNC_handler.h"

namespace Vast
{
    net_udpNC_handler::net_udpNC_handler(ip::udp::endpoint local_endpoint):
        net_udp_handler(local_endpoint)
    {

    }

    size_t net_udpNC_handler::send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint)
    {
        if (_udp == NULL)
        {
            std::cerr << "net_udpNC_handler::send trying to send before socket is ready" << std::endl;
            return -1;

        }

        RLNCHeader_factory header_factory(GENSIZE, generation);
        RLNCMessage message(header_factory.build());

        message.putMessage(msg, n);
        message.putPacketId(1);

        char *buf = new char[message.sizeOf()];

        int sending_len = message.serialize(buf);

        return _udp->send_to(buffer(buf, sending_len), remote_endpoint);
    }

    void net_udpNC_handler::start_receive()
    {
        _udp->async_receive_from(
            boost::asio::buffer(_buf, VAST_BUFSIZ), _remote_endpoint_,
            boost::bind(&net_udpNC_handler::handle_input, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }

    // handling incoming message
    int net_udpNC_handler::handle_input (const boost::system::error_code& error,
          std::size_t bytes_transferred)
    {
        size_t n = bytes_transferred;
    //    Vast::VASTHeader header;
        RLNCHeader header;

        if (!error)
        {
            //Store UDP messages

            std::cout << "Received a message!" << std::endl;

            char *p = _buf;

            //NOTE: there may be several valid UDP messages received at once -- is this really necessary?
            //while (n > sizeof (RLNCHeader))
    //        {
    //            //extract message header
    //            mempcpy (&header, p, sizeof (Vast::VASTHeader));
    //            n -= sizeof(Vast::VASTHeader);
    //            p += sizeof(Vast::VASTHeader);
            memcpy(&header, p, sizeof(RLNCHeader));

                //Check if it is really a VAST message: Start and end bytes of header should be correct
                if (!RLNCHeader_factory::isRLNCHeader(header))
                {
                    std::cout << "net_udp_handler::handle_input Non-RLNC message received on UDP socket" << std::endl;
    //                return -1;
                }
                else {
                    std::cout << "RLNC message received on the coding host" << std::endl;
                }

    //            Message *msg = new Message(0);
    //            if (0 == msg->deserialize (p, header.msg_size))
    //            {
    //                printf("net_udp_handler::handle_input deserialize message fail: size = %u\n", header.msg_size);
    //            }
    //            remote_id = msg->from;

    //            //Store the host_id : IPaddr pair
    //            IPaddr remote_addr(_remote_endpoint_.address().to_v4().to_ulong(), _remote_endpoint_.port());


    //            id_t temp_id = remote_id;
    //            //This host is looking for an ID, assign it a temporary ID to store the connection
    //            if (remote_id == NET_ID_UNASSIGNED && msg->msgtype == ID_REQUEST)
    //            {
    //                temp_id = ((net_udp*)_msghandler)->resolveHostID(&remote_addr);
    //            }

    ////                _remote_addrs[remote_id] = remote_addr;
    //            storeRemoteAddress(temp_id, remote_addr);

    //            //We assume if we can get a packet from the host, we are connected to that host
    //            ((net_udp*)_msghandler)->socket_connected(temp_id, this, false);

    //            //Break up messages into VASTMessage sizes
    //            //msg start at p - 4, i.e. start of header
    //            //msgsize = header.msg_size + 4 for header
    //            ((net_udp*)_msghandler)->msg_received(temp_id, p - sizeof(VASTHeader), header.msg_size + sizeof(VASTHeader));



    //            //Next message
    //            p += header.msg_size;
    //            n -= header.msg_size;
    //        }


            //Restart waiting for new packets
            start_receive();

//        RLNCHeader header;
//        VASTHeader header;
//        id_t remote_id;

//        if (!error)
//        {
//            //Store UDP messages

//            char *p = _buf;

//            //NOTE: there may be several valid UDP messages received at once -- is this really necessary?
//            while (n > sizeof (VASTHeader))
//            {
//                //extract message header
//                mempcpy (&header, p, sizeof (VASTHeader));
//                n -= sizeof(VASTHeader);
//                p += sizeof(VASTHeader);

//                //Check if it is really a VAST message: Start and end bytes of header should be correct
//                if (!(header.start == 10 && header.end == 5))
//                {
//                    CPPDEBUG("net_udp_handler::handle_input Non-VAST message received on UDP socket" << std::endl);
//                    return -1;
//                }

//                Message *msg = new Message(0);
//                if (0 == msg->deserialize (p, header.msg_size))
//                {
//                    printf("net_udp_handler::handle_input deserialize message fail: size = %u\n", header.msg_size);
//                }
//                remote_id = msg->from;

//                //Store the host_id : IPaddr pair
//                IPaddr remote_addr(_remote_endpoint_.address().to_v4().to_ulong(), _remote_endpoint_.port());


//                id_t temp_id = remote_id;
//                //This host is looking for an ID, assign it a temporary ID to store the connection
//                if (remote_id == NET_ID_UNASSIGNED && msg->msgtype == ID_REQUEST)
//                {
//                    temp_id = ((net_udp*)_msghandler)->resolveHostID(&remote_addr);
//                }

////                _remote_addrs[remote_id] = remote_addr;
//                storeRemoteAddress(temp_id, remote_addr);

//                //We assume if we can get a packet from the host, we are connected to that host
//                ((net_udp*)_msghandler)->socket_connected(temp_id, this, false);

//                //Break up messages into VASTMessage sizes
//                //msg start at p - 4, i.e. start of header
//                //msgsize = header.msg_size + 4 for header
//                ((net_udp*)_msghandler)->msg_received(temp_id, p - sizeof(VASTHeader), header.msg_size + sizeof(VASTHeader));



//                //Next message
//                p += header.msg_size;
//                n -= header.msg_size;
//            }


            //Restart waiting for new packets
//            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

}
