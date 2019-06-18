#include "customrlncdecoder.h"
#include "net_manager.h"
#include "VASTnet.h"
#include "logger.h"
#include <memory>
#include "autodestructortimer.h"

customrlncdecoder::customrlncdecoder ():
    decoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE),
    encoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE)
{
    CPPDEBUG("RLNCdecoder::constructor: finite field" << std::endl);
    CPPDEBUG(int(field) << std::endl);

}

void customrlncdecoder::startAddLockTimer()
{
    t1 = std::chrono::high_resolution_clock::now();
}

void customrlncdecoder::stopAddLockTimer()
{
    auto t2 = std::chrono::high_resolution_clock::now();
    addLockTimer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
}

void customrlncdecoder::addRLNCMessage(RLNCMessage input_message)
{
#ifndef COMPUTE_RLNC
    return;
#endif

    packets_added_packetpool++;

    startAddLockTimer ();
    auto pktids = input_message.getPacketIds();
    std::lock_guard<std::mutex> guard(packet_pool_mutex);

    //If an uncoded packet, it is within the codable size and not from NET_ID_UNASSIGNED
    if (pktids.size() == 1
            && input_message.getMessageSize() < MAX_PACKET_SIZE
            && input_message.getFirstFromId () != NET_ID_UNASSIGNED)
    {
        if (packet_pool.find (pktids.front()) != packet_pool.end ())
        {
            CPPDEBUG("Replacing a packet " << pktids.front() << std::endl);
            if (!(packet_pool[pktids.front()] == input_message))
            {
                CPPDEBUG("Stored packet: \n" << packet_pool[pktids.front()] << std::endl);
                CPPDEBUG("Recvd packet: \n" << input_message << std::endl);
                throw std::logic_error("customrlncdecoder::addRLNCMessage: packet ids are supposed unique: \
                                       Found different packet with same ID ");
            }
        }

        packet_pool[pktids.front()] = input_message;
    }

    //If encoded, add to NC_packets queue to be decoded
    else if (pktids.size() > 1)
    {
        NC_packets.push_back(input_message);
    }
    stopAddLockTimer ();

    if (packet_pool.size() > max_packetpool_size)
    {
        max_packetpool_size = packet_pool.size();
    }

    if (NC_packets.size() > max_NC_packets_size)
    {
        max_NC_packets_size = NC_packets.size();
    }
}

void customrlncdecoder::clearPacketPool ()
{
    std::lock_guard<std::mutex> packet_guard(packet_pool_mutex);
    packet_pool.clear ();
    NC_packets.clear ();
}

size_t customrlncdecoder::getPacketPoolSize ()
{
    return packet_pool.size ();
}

bool customrlncdecoder::_fetchFromPacketPool(RLNCMessage &active_encoded_packet,
                                       std::map<size_t, RLNCMessage> &available_packets,
                                       size_t &decoded_packet_index)
{

    //Check if we can decode something and then copy out the relevant packets
    std::unique_lock<std::mutex> fetchPackets_guard(packet_pool_mutex);

    for (size_t i = 0; i < NC_packets.size(); i++)
    {
        auto pktids = NC_packets[i].getPacketIds();

        size_t available_ids = 0;
        //Check if we have enough pktids to decode this message
        for (size_t j = 0; j < pktids.size(); j++)
        {
            if (packet_pool.find(pktids[j]) != packet_pool.end())
            {
                available_ids += 1;
            }
        }

        //Even if we have all the pktids, this message could be newer than what we have
        //If we have all the pktids, this message has already been decoded
        if (available_ids == pktids.size())
        {
        }


        //If we have all the pktids except 1, this NC message contains a useful message
        //Copy the available packets to a vector, used for decoding after lock is released
        else if (available_ids >= pktids.size()-1)
        {
            for (size_t j = 0; j < pktids.size(); j++)
            {
                if (packet_pool.find(pktids[j]) != packet_pool.end())
                {
                    available_packets[j] = (packet_pool.find (pktids[j])->second);
                }
                else
                {
                    //Save packet index for later use
                    decoded_packet_index = j;
                }
            }

            //Erase the packets we have just used
            for (size_t j = 0; j < pktids.size (); j++)
            {
                if (packet_pool.find(pktids[j]) != packet_pool.end())
                {
                   packet_pool.erase (packet_pool.find (pktids[j]));
                }
            }


            active_encoded_packet = NC_packets[i];
            //We have found our packet we want to decode, stop looking
            break;
        }

        else {
            CPPDEBUG("Too many packets missing, erasing and trying next packet" << std::endl);
            NC_packets.erase (NC_packets.begin() + i);
            continue;
        }
    }
    //Shared variable access done
    fetchPackets_guard.unlock ();

    if (available_packets.size () == active_encoded_packet.getPacketIds().size())
    {
        packets_already_decoded++;
        return false;
    }
    else if (available_packets.size () != (active_encoded_packet.getPacketIds ().size () - 1))
    {
        CPPDEBUG("customrlncdecoder::produceDecodedRLNCMessage Could not find enough available packets to decode" << std::endl);
        packets_missing_undecodable++;
        return false;
    }
    return true;
}

bool customrlncdecoder::_putAvailableInDecoder(std::shared_ptr<kodo_rlnc::decoder> decoder,
                                         const std::map<size_t, RLNCMessage> &available_packets,
                                         const RLNCMessage active_encoded_packet,
                                         size_t decoded_packet_index,
                                         uint32_t &total_checksum)
{
    for (size_t k = 0; k < active_encoded_packet.getPacketIds ().size(); k++)
    {

        if (k == decoded_packet_index)
            continue;

        //Quickly construct a encoder to give us the correct
        std::array<uint8_t, MAX_PACKET_SIZE> buffer;
        buffer.fill(0);

        //Serialize the whole packet, not just the (un-encoded) message
        available_packets.at(k).serialize(reinterpret_cast<char*>(buffer.data()));
        uint32_t checksum = 0;
        checksum = RLNCMessage::generateChecksum(buffer.data(), available_packets.at(k).sizeOf());

#ifdef SAVE_PACKETS
        Logger::saveBinaryArray("RLNCdecoder_" + std::to_string(available_packets[k].getPacketIds()[0]) + ".txt", buffer.data(), available_packets[k].sizeOf());
#endif

        total_checksum += checksum;

        decoder->read_uncoded_symbol (buffer.data(), k);

        //Stop if we have enough packets to decode.
        if (decoder->is_complete ())
        {
            //Sanity check
            CPPDEBUG("customrlncdecoder::_putAvailableInDecoder Could decode packet with only uncoded symbols" << std::endl);
            break;
        }

    }

    return true;
}

std::shared_ptr<RLNCMessage> customrlncdecoder::produceDecodedRLNCMessage()
{
#ifndef COMPUTE_RLNC
    return NULL;
#endif
    decodes_attempted++;

    AutoDestructorTimer timer("decodingTimer", &decoderTimer);

    std::shared_ptr<RLNCMessage> decoded_msg = nullptr;
    //Leave enough space for RLNC coefficients
    std::array<uint8_t, MAX_PACKET_SIZE + 20> payload;
    payload.fill(0);
    //Enough space for two symbols to be decoded
    std::array<uint8_t, MAX_PACKET_SIZE*2> data_out;
    data_out.fill(0);


    std::map<size_t, RLNCMessage> available_packets;
    RLNCMessage active_encoded_packet;
    size_t decoded_packet_index = -1;


    //
    bool success = _fetchFromPacketPool(active_encoded_packet,
                                        available_packets,
                                        decoded_packet_index);

    if (!success)
    {
        //Either the packet has already been decoded or too few packets are available to decode
        return NULL;
    }

    auto decoder = decoder_factory.build();
    decoder->set_mutable_symbols(storage::storage(data_out));

    //Sanity check
    if (available_packets.size() != 1)
    {
        throw std::logic_error(std::string("customrlncdecoder::produceDecodedRLNCMessage available_packet.size() ") +
                               "should be 1, as we only currently code 2 packets together\n");
    }

    //Used to check if the checksum of two decoded packets are correct
    uint32_t total_checksum = 0;

    _putAvailableInDecoder(decoder, available_packets, active_encoded_packet,
                           decoded_packet_index, total_checksum);

    //Copy the coded packet in
    payload.fill (0);
    memcpy(payload.data(), active_encoded_packet.getMessage(), active_encoded_packet.getMessageSize());
    decoder->read_payload(payload.data());

    if (decoder->is_complete())
    {
        if (packet_pool.size() > 1)
        {
            //This can be a problem - preferrably we only want the necessary
            //packets in the packet pool for decoding, without potentially stale
            // packets, but this should not be a train smash...
//                    std::abort();
        }

        RLNCMessage msg;
        if (msg.deserialize (reinterpret_cast<char*>(data_out.data () + decoded_packet_index * MAX_PACKET_SIZE), MAX_PACKET_SIZE) != -1)
        {
            //Use the newly decoded packet - should be most cases true
            decoded_msg = std::make_shared<RLNCMessage>(msg);
#ifdef SAVE_PACKETS
            Logger::saveBinaryArray("RLNCdecoder_" + std::to_string(msg.getPacketIds()[0]) + ".txt",
                    reinterpret_cast<char*>(data_out.data () + decoded_packet_index * MAX_PACKET_SIZE), msg.sizeOf());
#endif

            //Calculate checksum to see if decoding is correct
            total_checksum += RLNCMessage::generateChecksum(data_out.data () + decoded_packet_index * MAX_PACKET_SIZE,
                                                     msg.sizeOf());

            if (total_checksum != active_encoded_packet.getChecksum())
            {
                CPPDEBUG("customrlncdecoder::produceDecodedRLNCMessage Checksum was not correct"
                         << std::endl << (*decoded_msg) << std::endl);
                CPPDEBUG("customrlncdecoder::produceDecodedRLNCMessage: Packets coded together " << active_encoded_packet.getPacketIds()[0] << "," << active_encoded_packet.getPacketIds()[1] << std::endl);
                packets_checksum_incorrect++;
                return nullptr;
            }

            packets_recovered++;
        }
        else
        {
            CPPDEBUG("customrlncdecoder::produceDecodedRLNCMessage: Could not deserialize the decoded packet" << std::endl);
        }

    }
    else
    {
        CPPDEBUG("customrlncdecoder::produceDecodedRLNCMessage: Could not decode, probably linearly dependent, deleting it and its packets" << std::endl);
        packet_linearly_dependent++;
    }

    return decoded_msg;
}

customrlncdecoder::~customrlncdecoder ()
{
    CPPDEBUG("\n~customrlncdecoder:: packets added to packet pool: " << packets_added_packetpool << std::endl);
    CPPDEBUG("~customrlncdecoder:: decodes_attempted: " << decodes_attempted << std::endl);
    CPPDEBUG("~customrlncdecoder:: packet_recovered: " << packets_recovered << std::endl);
    CPPDEBUG("~customrlncdecoder:: packets_already_decoded: " << packets_already_decoded << std::endl);
    CPPDEBUG("~customrlncdecoder:: packets_missing_undecodable: " << packets_missing_undecodable << std::endl);
    CPPDEBUG("~customrlncdecoder:: packets_checksum_incorrect: " << packets_checksum_incorrect << std::endl);
    CPPDEBUG("~customrlncdecoder::packet_linearly_dependent" << packet_linearly_dependent << std::endl);
    CPPDEBUG("~customrlncdecoder:: max_packetpool_size: " << max_packetpool_size << std::endl);
    CPPDEBUG("~customrlncdecoder:: max_NC_packets_size: " << max_NC_packets_size << std::endl);
    CPPDEBUG("~customrlncdecoder:: time spent in addLock: " << addLockTimer.count() / 1000 << " milliseconds " << std::endl);
    CPPDEBUG("~customrlncdecoder:: time spent decoding: " << decoderTimer.count() / 1000 << " milliseconds " << std::endl);
    if (decodes_attempted > 0)
        CPPDEBUG("~customrlncdecoder:: time spent decoding per msg: " << decoderTimer.count() / decodes_attempted << " microseconds " << std::endl);
}

