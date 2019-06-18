#ifndef RLNCRECODER_H
#define RLNCRECODER_H

#include "rlncmessage.h"
#include <iostream>
#include <map>
#include <array>
#include <memory>
#include <chrono>

#include <boost/asio.hpp>
#include "rlnc_fieldsize_defs.h"

#include <kodo_rlnc/coders.hpp>

using rlnc_encoder = kodo_rlnc::encoder;

class RLNCrecoder
{
public:
    RLNCrecoder();
    ~RLNCrecoder();

    void addRLNCMessage(RLNCMessage msg);
    std::shared_ptr<RLNCMessage> produceRLNCMessage();
    size_t getPacketPoolSize();


private:
    std::map<packetid_t, RLNCMessage> packet_pool;

    // Typdefs for the encoder/decoder type we wish to use
    fifi::api::field field = fifi::api::field::FINITE_FIELD_SIZE;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_encoder::factory encoder_factory;
    RLNCHeader_factory header_factory;

    size_t max_packetpool_size = 0;

    void startEncodeTimer();
    void stopEncodeTimer();
    std::chrono::microseconds encodeTimer = std::chrono::microseconds::zero();
    std::chrono::high_resolution_clock::time_point t1;
    bool encodeTimerRunning = false;
};

#endif // RLNCRECODER_H
