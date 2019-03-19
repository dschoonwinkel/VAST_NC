#include "packet_listener.h"
#include <iostream>
#include "rlnc_packet_factory.h"
#include "VASTnet.h"

packet_listener::packet_listener (ip::udp::endpoint local_endpoint)
{
    _udp = NULL;
    _io_service = NULL;
    _iosthread_recv = NULL;
    _local_endpoint = local_endpoint;
}



int packet_listener::open (io_service *io_service, void *msghandler)
{
    _io_service = io_service;
    _msghandler = msghandler;

    //Open the UDP socket for listening
    if (_udp == NULL) {
        _udp = new ip::udp::socket(*_io_service, _local_endpoint);
//        _udp = new ip::udp::socket(*_io_service);
//        _udp->open(ip::udp::v4());

//        boost::system::error_code ec;
//        uint16_t port = _local_endpoint.port();

//        do
//        {
//            //Search for an open port to use
//            //Save port number
//            _local_endpoint.port(port);
//            _udp->bind(_local_endpoint, ec);

//            CPPDEBUG("packet_listener::open " + ec.message() << std::endl);
//            //Try the next port
//            port++;

//        } while (ec);

        //Open dest_unr_listener as well to receive disconnects
//            _disconn_listener = new dest_unreachable_listener(*io_service, _local_endpoint.address().to_string().c_str(), this);


        //Add async receive to io_service queue
        start_receive();

//        CPPDEBUG("packet_listener::open _udp->_local_endpoint: " << _udp->local_endpoint() << " _local_endpoint" << _local_endpoint << std::endl);

        //Start the thread handling async receives
        _iosthread_recv = new boost::thread(boost::bind(&boost::asio::io_service::run, io_service));

        //Start the thread handling async sends
        _iosthread_send = new boost::thread(boost::bind(&packet_listener::start_send, this));

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
    recv_msgs_count++;
    RLNCHeader header;

    if (!error)
    {
        //Store UDP messages
//        CPPDEBUG("Received a message!" << std::endl);

        char *p = _buf;

        memcpy(&header, p, sizeof(RLNCHeader));

            //Check if it is really a VAST message: Start and end bytes of header should be correct
            if (!RLNCHeader_factory::isRLNCHeader(header))
            {
                CPPDEBUG("packet_listener::handle_input Non-RLNC message received on UDP socket" << std::endl);
            }
            else {
//                CPPDEBUG("RLNC message received on the coding host" << std::endl);
                RLNCMessage message;

                message.deserialize(_buf, bytes_transferred);
                recoder.addRLNCMessage(message);

                msgs_mutex.lock();
                RLNCMessage *temp_msg = recoder.produceRLNCMessage();


                if (temp_msg != NULL)
                {
                    msgs.push_back(RLNCMessage(*temp_msg));
//                    CPPDEBUG("packet_listener::handle_input: Adding RLNCmessage to queue" << std::endl);
                    delete temp_msg;
                }

                msgs_mutex.unlock();
            }


        //Restart waiting for new packets
        start_receive();
    }
    else {
        std::cerr << "Error on UDP socket receive: " << error.message() << std::endl;
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
        _iosthread_recv->join();
    }

    return 0;
}

void packet_listener::start_send()
{
    while (msgs.size() < 1);


    msgs_mutex.lock();
    RLNCMessage message(msgs.front());
    msgs.pop_front();
    msgs_mutex.unlock();

    size_t sendlen = message.serialize(_sendbuf);
    send(_sendbuf, sendlen, ip::udp::endpoint(ip::address::from_string("239.255.0.1"), 1037));

    //Call itself to continue sending until the msgs queue has been depleted
    start_send();

}

size_t packet_listener::send (const char *buf, size_t n, ip::udp::endpoint remote_endpoint)
{
    if (_udp == NULL)
    {
        std::cerr << "packet_listener::send trying to send before socket is ready" << std::endl;
        return -1;
    }

//    CPPDEBUG("packet_listener::send size of sent packet: " << n << std::endl);
    _udp->async_send_to(buffer(buf, n), remote_endpoint,
                               boost::bind(&packet_listener::handle_send_to, this,
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));

    return n;

}

void packet_listener::handle_send_to(const boost::system::error_code&, std::size_t)
{

    coded_msgs_sent++;
    total_coded_msgs_sent++;

    if (total_coded_msgs_sent % 100 == 0)
    {
      CPPDEBUG(total_coded_msgs_sent << " packets sent async " << std::endl);
    }
//        coded_msgs_sent = 0;
    //CPPDEBUG(total_coded_msgs_sent << " packets sent async, " << coded_msgs_sent << " this timeslot" << std::endl);
}

packet_listener::~packet_listener ()
{
    // remove UDP listener, net_udp will delete itself
    _udp = NULL;
}

size_t packet_listener::getRecvMsgsCount ()
{
    return recv_msgs_count;
}
