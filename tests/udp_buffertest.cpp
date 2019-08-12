#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>

using namespace boost::asio;
using namespace std::chrono_literals;


size_t packets_received = 0;
size_t stacked_packets_received = 0;

void SIGINT_handler (int)
{
    std::cout << "Interrupt received" << std::endl;
    std::cout << "packets_received: " << packets_received << std::endl;
    std::cout << "stacked_packets_received: " << stacked_packets_received << std::endl;
    exit(0);

}

int main()
{
    signal(SIGINT, SIGINT_handler);

    // UDP endpoint (where the receiver will be listening)
    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;


    // The endpoint (represents the reciever)
    auto receiver = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address::from_string(remote_ip), remote_port);

    // Create a boost::asio socket
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket(io_service);

    socket.open(boost::asio::ip::udp::v4());
    socket.bind(receiver);

    while(true)
    {
        ip::udp::endpoint remote_endpoint;
        boost::array<uint8_t, 1024> buf;
        std::cout << socket.available() << std::endl;
        std::this_thread::sleep_for(5s);
        size_t recvcount = socket.receive_from(buffer(buf), remote_endpoint);
        packets_received++;
        for (size_t i = 0; i < recvcount; i++)
        {
            std::cout << buf[i] << " ";
        }
        if (socket.available() > 0)
            stacked_packets_received++;


        std::cout << std::endl;
    }
}
