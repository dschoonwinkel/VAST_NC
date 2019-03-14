#include <kodo_rlnc/coders.hpp>
using rlnc_encoder = kodo_rlnc::encoder;
using rlnc_decoder = kodo_rlnc::decoder;

#define MAX_PACKET_SIZE 4
#define MAX_SYMBOLS 2

#include <array>
#include <algorithm>
#include <stdint.h>
#include <cstring>
#include <boost/asio.hpp>

using namespace boost::asio;
#include <iostream>

int main() {
    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};

    std::array<uint8_t, MAX_PACKET_SIZE*2> buffer;
    buffer.fill(0);
    std::array<uint8_t, MAX_PACKET_SIZE+20> payload1;
    payload1.fill(0);
    std::array<uint8_t, MAX_PACKET_SIZE+20> payload2;
    payload2.fill(0);

    std::array<uint8_t, MAX_PACKET_SIZE*2> data_out;
    data_out.fill(0);


    fifi::api::field field = fifi::api::field::binary8;
    rlnc_encoder::factory factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE);
    auto encoder = factory.build();

    encoder->set_systematic_off();

    memcpy(buffer.data(), data1, 4);
    memcpy(buffer.data() + MAX_PACKET_SIZE, data2, 4);


    //Generate a coded packet
    encoder->set_const_symbols(storage::storage(buffer));
    encoder->write_payload(payload2.data());



    //Encapsulate an uncoded packet with a coded header
    rlnc_encoder::factory factory2(field, MAX_SYMBOLS, MAX_PACKET_SIZE);
    auto encoder2 = factory2.build();

    buffer.fill(0);
    memcpy(buffer.data(), data1, 4);
    encoder2->set_const_symbols(storage::storage(buffer));
    encoder2->write_payload(payload1.data());

    io_service ios;
    ip::udp::endpoint dest(ip::address::from_string ("10.0.2.3"), 1037);
    ip::udp::socket _sock(ios);
    _sock.open(ip::udp::v4());

    _sock.send_to(boost::asio::buffer(payload1.data(), encoder->payload_size ()), dest);
    encoder->write_payload(payload2.data());
    _sock.send_to(boost::asio::buffer(payload2.data(), encoder->payload_size ()), dest);
    encoder->write_payload(payload2.data());
    _sock.send_to(boost::asio::buffer(payload2.data(), encoder->payload_size ()), dest);


    rlnc_decoder::factory de_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE);
    auto decoder = de_factory.build();

    decoder->set_mutable_symbols(storage::storage(data_out));

    decoder->read_payload(payload1.data());
    decoder->read_payload(payload2.data());

    std::cout << "decoder is done or not?" << decoder->is_complete() << std::endl;

    return 0;
}
