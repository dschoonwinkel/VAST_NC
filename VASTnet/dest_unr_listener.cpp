//
// dest_unr_listener.cpp
// ~~~~~~~~
//
// Adapted from ping.cpp by
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Daniel Schoonwinkel (2018)

#include "dest_unr_listener.h"
#include "VASTTypes.h"
#include "net_overhearing_handler.h"

dest_unreachable_listener::dest_unreachable_listener(boost::asio::io_service& io_service, const char* destination, Vast::net_overhearing_handler* disconn_handler)
    : resolver_(io_service), socket_(io_service, icmp::v4())
{
    icmp::resolver::query query(icmp::v4(), destination, "");
    destination_ = *resolver_.resolve(query);
    _disconn_handler = disconn_handler;

    start_receive();
}

void dest_unreachable_listener::start_receive()
{
    // Discard any data already in the buffer.
    reply_buffer_.consume(reply_buffer_.size());

    // Wait for a reply. We prepare the buffer to receive up to 64KB.
    socket_.async_receive(reply_buffer_.prepare(65536),
                          boost::bind(&dest_unreachable_listener::handle_receive, this, _2));
}

void dest_unreachable_listener::handle_receive(std::size_t length)
{
    std::cout << "Receiving" << std::endl;
    // The actual number of bytes received is committed to the buffer so that we
    // can extract it using a std::istream object.
    reply_buffer_.commit(length);

    // Decode the reply packet.
    std::istream is(&reply_buffer_);
    ipv4_header ipv4_hdr;
    icmp_header icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;

    ipv4_header returned_iphdr;
    udp_header returned_udphdr;

    // We can receive all ICMP packets received by the host, so we need to
    // filter out only the echo replies that match the our identifier and
    // expected sequence number.
    if (is && icmp_hdr.type() == icmp_header::destination_unreachable)
    {

        // Print out some information about the reply packet.
        std::cout << length - ipv4_hdr.header_length()
                  << " bytes from " << ipv4_hdr.source_address()
                  << " icmp_type " << (int)icmp_hdr.type()
                  << " icmp_code " << (int)icmp_hdr.code()
                  << " icmp_code_string " << icmp_hdr.code_to_string()
                  << std::endl;
    }

    if (is && icmp_hdr.type() == icmp_header::destination_unreachable) {

        is >> returned_iphdr;
        // std::cout << "Returned ip hdr: " << returned_iphdr.source_address()
        //           << " ip protocol: "     << (int)returned_iphdr.protocol()
        //           << " protocol name: "   << returned_iphdr.protocol_name()
        //           << " " << std::endl;

        if (returned_iphdr.protocol() == ipv4_header::UDP) {

            is >> returned_udphdr;
            // std::cout << "Returned udp hdr: src_port: " << (int)returned_udphdr.src_port()
            //           << " dest_port: "     << (int)returned_udphdr.dest_port()
            //           << " length: "   << (int)returned_udphdr.length()
            //           << " " << std::endl;
            // returned_udphdr.print_bytes();

            std::cout << returned_iphdr.source_address() << ":" << returned_udphdr.dest_port() << " disconnected" << std::endl;
            Vast::IPaddr addr(returned_iphdr.source_address().to_ulong(), returned_udphdr.dest_port());
            _disconn_handler->handle_disconnect(addr);
        }
    }

    start_receive();
}

//int main(int argc, char* argv[])
//{
//    try
//    {
//        if (argc != 2)
//        {
//            std::cerr << "Usage: ping <host>" << std::endl;
//#if !defined(BOOST_ASIO_WINDOWS)
//            std::cerr << "(You may need to run this program as root.)" << std::endl;
//#endif
//            return 1;
//        }

//        boost::asio::io_service io_service;
//        dest_unreachable_listener p(io_service, argv[1]);
//        io_service.run();
//    }
//    catch (std::exception& e)
//    {
//        std::cerr << "Exception: " << e.what() << std::endl;
//    }
//}
