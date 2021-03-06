#ifndef UDPNCDECODER_H
#define UDPNCDECODER_H

#include "udpncmessage.h"
#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <memory>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <kodo_rlnc/coders.hpp>
#include "rlnc_fieldsize_defs.h"
#include <chrono>

using namespace boost::asio;

using rlnc_decoder = kodo_rlnc::decoder;
using rlnc_encoder = kodo_rlnc::encoder;

class udpNC_decoder
{
public:
    udpNC_decoder();
    ~udpNC_decoder();

    void addUDPNCMessage(UDPNCMessage msg);
    std::shared_ptr<UDPNCMessage> produceDecodedUDPNCMessage();

    //Called to clear packet pool and NC_packets at the end of each "generation"
    void clearPacketPool();
    size_t getPacketPoolSize();

private:

    //produceDecodedUDPNCMessage helper functions
    bool _fetchFromPacketPool(UDPNCMessage &active_encoded_packet,
                              std::map<size_t, UDPNCMessage> &available_packets,
                              size_t &decoded_packet_index);

    bool _putAvailableInDecoder(std::shared_ptr<kodo_rlnc::decoder> decoder,
                                const std::map<size_t, UDPNCMessage> &available_packets,
                                const UDPNCMessage &active_encoded_packet,
                                const size_t &decoded_packet_index,
                                uint32_t &total_checksum);

    std::map<packetid_t, UDPNCMessage> packet_pool;
    std::vector<UDPNCMessage> NC_packets;

    //Timing functions used for determining holdups
    void startAddLockTimer();
    void stopAddLockTimer();

    // Typdefs for the encoder/decoder type we wish to use
    fifi::api::field field = fifi::api::field::FINITE_FIELD_SIZE;

    // In the following we will make an encoder/decoder factory.
    // The factories are used to build actual encoders/decoders
    rlnc_decoder::factory decoder_factory;
    rlnc_encoder::factory encoder_factory;

    std::mutex packet_pool_mutex;
    //For stats collection purposes
    size_t packets_recovered = 0;
    size_t packets_added_packetpool = 0;
    size_t packets_already_decoded = 0;
    size_t packets_missing_undecodable = 0;
    size_t packet_linearly_dependent = 0;
    size_t packets_checksum_incorrect = 0;
    size_t decodes_attempted = 0;
    size_t max_packetpool_size = 0;
    size_t max_NC_packets_size = 0;

    //Add lock timing
    std::chrono::microseconds addLockTimer = std::chrono::microseconds::zero();
    std::chrono::high_resolution_clock::time_point t1;

    std::chrono::microseconds decoderTimer = std::chrono::microseconds::zero();
};

#endif // UDPNCDECODER_H
