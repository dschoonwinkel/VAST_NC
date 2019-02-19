#include "packet_listener.h"
#include <iostream>
#include "rlnc_packet_factory.h"
#include "VASTnet.h"

packet_listener::packet_listener (ip::udp::endpoint local_endpoint)
{
    _udp = NULL;
    _io_service = NULL;
    _iosthread = NULL;
    _local_endpoint = local_endpoint;
}



int packet_listener::open (io_service *io_service, void *msghandler)
{
    _io_service = io_service;
    _msghandler = msghandler;

    //Open the UDP socket for listening
    if (_udp == NULL) {
        _udp = new ip::udp::socket(*_io_service);
        _udp->open(ip::udp::v4());

        boost::system::error_code ec;
        uint16_t port = _local_endpoint.port();

        do
        {
            //Search for an open port to use
            //Save port number
            _local_endpoint.port(port);
            _udp->bind(_local_endpoint, ec);

            std::cout << "net_udp_handler::open " + ec.message() << std::endl;
            //Try the next port
            port++;

        } while (ec);

        //Open dest_unr_listener as well to receive disconnects
//            _disconn_listener = new dest_unreachable_listener(*io_service, _local_endpoint.address().to_string().c_str(), this);


        //Add async receive to io_service queue
        start_receive();

        std::cout << "net_udp_handler::open _udp->_local_endpoint: " << _udp->local_endpoint() << " _local_endpoint" << _local_endpoint << std::endl;

        //Start the thread handling async receives
        _iosthread = new boost::thread(boost::bind(&boost::asio::io_service::run, io_service));

    }

    return 0;
}

// close connection & unregister from io_service
int packet_listener::close (void)
{
    return this->handle_close();
}

//Start the receiving loop
void packet_listener::start_receive ()
{
    _udp->async_receive_from(
        boost::asio::buffer(_buf, BUFSIZ), _remote_endpoint_,
        boost::bind(&packet_listener::handle_input, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

// handling incoming message
int packet_listener::handle_input (const boost::system::error_code& error,
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
            if (!isRLNCHeader(header))
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
    }
    else {
        std::cout << "Error on UDP socket receive: " << error.message() << std::endl;
    }
    return -1;

}

int packet_listener::handle_close ()
{
    if (_io_service != NULL) {
        if (_udp != NULL && _udp->is_open())
        {
            _udp->close();
        }

        _io_service->stop();
        _iosthread->join();
    }

    return 0;
}

size_t packet_listener::send (const void *buf, size_t n, ip::udp::endpoint remote_endpoint)
{
    if (_udp == NULL)
    {
        std::cerr << "net_udp_handler::send trying to send before socket is ready" << std::endl;
        return -1;
    }

//        std::cout << ("net_udp_handler::send size of sent packet: " << n << std::endl);
    return _udp->send_to(buffer(buf, n), remote_endpoint);

}

packet_listener::~packet_listener ()
{
    // remove UDP listener, net_udp will delete itself
    _udp = NULL;
}
