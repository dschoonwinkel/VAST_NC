#ifndef RLNCDECODER_H
#define RLNCDECODER_H

#include "rlncmessage.h"
#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <memory>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <kodo_rlnc/coders.hpp>
#include "rlnc_fieldsize_defs.h"

using namespace boost::asio;

using rlnc_decoder = kodo_rlnc::decoder;
using rlnc_encoder = kodo_rlnc::encoder;

class rlncdecoder
{
public:
    rlncdecoder();
    ~rlncdecoder();

    void addRLNCMessage(RLNCMessage msg);
    RLNCMessage* produceDecodedRLNCMessage();

    //Called to clear packet pool and NC_packets at the end of each "generation"
    void clearPacketPool();

private:

    std::map<packetid_t, RLNCMessage> packet_pool;
    std::vector<RLNCMessage> NC_packets;

    // Typdefs for the encoder/decoder type we wish to use
    fifi::api::field field = fifi::api::field::FINITE_FIELD_SIZE;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_decoder::factory decoder_factory;
    rlnc_encoder::factory encoder_factory;
    RLNCHeader_factory header_factory;

    //For stats collection purposes
    size_t packets_recovered = 0;
};

#endif // RLNCDECODER_H
