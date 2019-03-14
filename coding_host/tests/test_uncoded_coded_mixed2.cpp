#include <kodo_rlnc/coders.hpp>
using rlnc_encoder = kodo_rlnc::encoder;
using rlnc_decoder = kodo_rlnc::decoder;

fifi::api::field field = fifi::api::field::binary8;
#define MAX_PACKET_SIZE 5
#define MAX_SYMBOLS 2

#include <array>
#include <algorithm>
#include <stdint.h>
#include <cstring>

#include <iostream>

int main() {
    std::array<uint8_t, sizeof("dead")> data1 = {"dead"};
    std::array<uint8_t, sizeof("beef")> data2 = {"beef"};

    //Payloads generated by encoder, resized to accept coding coefficients (too large, I know)
    std::array<uint8_t, MAX_PACKET_SIZE+20> payload1;
    payload1.fill(0);
    std::array<uint8_t, MAX_PACKET_SIZE+20> payload2;
    payload2.fill(0);

    //Output from decoder
    std::array<uint8_t, MAX_PACKET_SIZE*2> data_out;
    data_out.fill(0);


    /******* On the source side ********/
    rlnc_encoder::factory factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE);
    auto encoder = factory.build();

    //Immediately generate an encoded packet
    encoder->set_systematic_off();

    //Generate a coded packet
    encoder->set_const_symbol (0, storage::storage (data1));
    encoder->set_const_symbol (1, storage::storage (data2));
    encoder->write_payload(payload1.data());


    /**** CHANNEL ****/
    // payload1 is sent over the channel multicast mode
    // data2 is sent over a unicast channel


    /*********** On the Decoder side **********/

    //Encapsulate the uncoded packet with a coded header
    rlnc_encoder::factory factory2(field, MAX_SYMBOLS, MAX_PACKET_SIZE);
    auto encoder2 = factory2.build();

    encoder2->set_const_symbol (1, storage::storage(data2));

    encoder2->write_payload (payload2.data());
    std::cout << "Payload 2 bytes:" << std::endl;

    for (size_t i =0 ; i< payload2.size (); i++)
    {
        printf("%x ", payload2[i]);
    }
    std::cout << std::endl;

    rlnc_decoder::factory de_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE);
    auto decoder = de_factory.build();

    decoder->set_mutable_symbols(storage::storage(data_out));

    //Read the one encoded payload
    decoder->read_payload(payload1.data());

    decoder->read_payload (payload2.data());

    if (decoder->is_complete ())
    {
        std::cout << "decoder is complete: " << std::endl;
        std::cout << "Symbol 1: " << std::endl;
        for (size_t i = 0; i < MAX_PACKET_SIZE; i++)
        {
            std::cout << data_out[i];
        }
        std::cout << std::endl;
        std::cout << "Symbol 2: " << std::endl;
        for (size_t i = MAX_PACKET_SIZE; i < MAX_PACKET_SIZE*2; i++)
        {
            std::cout << data_out[i];
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "decoder could not decode" << std::endl;
    }

    return 0;
}
