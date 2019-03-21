#include "mc_packet_listener.h"
#include <iostream>

MC_packet_listener::MC_packet_listener():
    packet_listener(ip::udp::endpoint(ip::address::from_string("0.0.0.0"), 1037)),
    MC_address(ip::address::from_string("239.255.0.1"), 1037)
{

}

int MC_packet_listener::open(io_service *ios)
{
    _recv_io_service = ios;

    //Open the UDP socket for listening
    if (_recv_udp == NULL) {
        _recv_udp = new ip::udp::socket(*_recv_io_service);
        _recv_udp->open(ip::udp::v4());
        _recv_udp->set_option(ip::udp::socket::reuse_address(true));
        _recv_udp->set_option(ip::multicast::join_group(MC_address.address ()));

        boost::system::error_code ec;
        uint16_t port = _local_endpoint.port();

        _local_endpoint.port(port);
        _recv_udp->bind(_local_endpoint, ec);

        std::cout << "MC_packet_listener::open " + ec.message() << std::endl;

        if (ec)
        {
            std::cout << "MC_packet_listener:: open MC address failed" << ec.message() << std::endl;
        }

        //Add async receive to io_service queue
        start_receive();

//        std::cout << "MC_packet_listener::open _udp->_local_endpoint: " << _udp->local_endpoint() << " _local_endpoint" << _local_endpoint << std::endl;

        //Start the thread handling async receives
        _iosthread_recv = new boost::thread(boost::bind(&boost::asio::io_service::run, ios));
    }

    return 0;
}


//Start the receiving loop
void MC_packet_listener::start_receive ()
{
    _recv_udp->async_receive_from(
        boost::asio::buffer(_buf, BUFSIZ), _remote_endpoint_,
        boost::bind(&MC_packet_listener::handle_input, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

// handling incoming message
int MC_packet_listener::handle_input (const boost::system::error_code& error,
                  std::size_t bytes_transferred)
{
    RLNCHeader header;

    CPPDEBUG("MC_packet_listener::handle_input Received from : " << _remote_endpoint_ << std::endl);

    if (!error)
    {
        //Store UDP messages
//        CPPDEBUG("Received a message!" << std::endl);

        char *p = _buf;

        memcpy(&header, p, sizeof(RLNCHeader));

            //Check if it is really a VAST message: Start and end bytes of header should be correct
            if (!RLNCHeader_factory::isRLNCHeader(header))
            {
                CPPDEBUG("MC_packet_listener::handle_input Non-RLNC message received on UDP socket" << std::endl);
            }
            else {
                CPPDEBUG("RLNC message received on the coding host" << std::endl);
                RLNCMessage message;

                message.deserialize(_buf, bytes_transferred);

                std::cout << "Received message: \n" << message << std::endl;
//                recoder.addRLNCMessage(message);

//                msgs_mutex.lock();
//                RLNCMessage *temp_msg = recoder.produceRLNCMessage();


//                if (temp_msg != NULL)
//                {
//                    msgs.push_back(RLNCMessage(*temp_msg));
//                    delete temp_msg;
//                }

//                msgs_mutex.unlock();
            }


        //Restart waiting for new packets
        start_receive();
    }
    else {
        std::cout << "Error on UDP socket receive: " << error.message() << std::endl;
    }
    return -1;

}
