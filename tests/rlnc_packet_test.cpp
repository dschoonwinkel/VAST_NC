#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/chrono.hpp>

#include <kodo_rlnc/coders.hpp>
#include "rlnc_packet_factory.h"

int main(int argc, char* argv[])
{
    // Set the number of symbols and the size of a symbol in bytes
    uint32_t symbols = 1;
    uint32_t symbol_size = 1;

    // UDP endpoint (where the receiver will be listening)
    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;

    uint32_t random_bytes = 1;

    // We will generate random data to transmit
    srand((uint32_t)time(0));

    // Typdefs for the encoder/decoder type we wish to use
    fifi::api::field field = fifi::api::field::binary;

    using rlnc_encoder = kodo_rlnc::encoder;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory(field, symbols, symbol_size);

    // Fill the data buffer with random bytes
    std::vector<uint8_t> data(random_bytes);
    std::generate(data.begin(), data.end(), rand);

    auto encoder = encoder_factory.build();
    encoder->set_const_symbols(storage::storage(data));

    std::cout << "Encoder parameters: " << std::endl;
    std::cout << "  Symbols: " << symbols << std::endl;
    std::cout << "  Symbol size: " << symbol_size << std::endl;

    // The endpoint (represents the reciever)
    auto receiver = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address::from_string(remote_ip), remote_port);

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();
    header1.generation = 1;
    header1.gensize = 2;
    header1.ordering = 0;
    header1.packetsize = encoder->symbol_size();
    header1.packet_id = 4;

    // Create a boost::asio socket
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket(io_service);

    socket.open(boost::asio::ip::udp::v4());

    //TODO: decide what payload size would work well here
    std::vector<uint8_t> payload(1400);

//    // The first packet will contain the metadata necessary for the decoder
//    std::vector<uint8_t> metadata(16);
//    uint8_t* buffer = metadata.data();
//    // The packet starts with the 4-byte magic string "META"
//    std::string meta("META");
//    meta.copy((char*)buffer, 4, 0);
//    buffer += 4;
//    endian::big_endian::put<uint32_t>(object_size, buffer);
//    buffer += sizeof(uint32_t);
//    endian::big_endian::put<uint32_t>(symbols, buffer);
//    buffer += sizeof(uint32_t);
//    endian::big_endian::put<uint32_t>(symbol_size, buffer);
//    buffer += sizeof(uint32_t);

    // In a practical system, the metadata might be transmitted periodically
    // to allow the late-joining receivers to complete the file transfer.
    // Alternatively, the receivers might acquire the metadata by pulling this
    // information from the sender using a separate unicast connection.
    // In this example, we simply start the transmission with the metadata.
    payload.resize(sizeof(RLNCHeader)+encoder->payload_size());
    memcpy(payload.data(), (char*)(&header1), sizeof(header1));
    encoder->write_payload(payload.data()+sizeof(RLNCHeader));

    std::cout << "sizeof RLNCHeader " << sizeof(RLNCHeader) << std::endl;;
    std::cout << "sizeof encoder->payload_size() " << encoder->payload_size() << std::endl;
    std::cout << "sizeof payload.data() " << payload.size() << std::endl;

    socket.send_to(boost::asio::buffer(payload, payload.size()), receiver);
    std::cout << "VASTHeader sent." << std::endl;

//    payload.resize(encoder->payload_size(b));

//    // Write a symbol into the payload buffer
//    encoder->write_payload(payload.data());

//    total_bytes +=
//        socket.send_to(boost::asio::buffer(payload), receiver);

}
