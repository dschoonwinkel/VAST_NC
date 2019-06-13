#include "rlncdecoder.h"
#include "net_manager.h"
#include "VASTnet.h"
#include "logger.h"

rlncdecoder::rlncdecoder ():
    decoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE),
    encoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE)
{
    CPPDEBUG("RLNCdecoder::constructor: finite field" << std::endl);
    CPPDEBUG(int(field) << std::endl);

}

void rlncdecoder::startAddLockTimer()
{
    t1 = std::chrono::high_resolution_clock::now();
}

void rlncdecoder::stopAddLockTimer()
{
    auto t2 = std::chrono::high_resolution_clock::now();
    addLockTimer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
}

void rlncdecoder::startDecodeTimer()
{
    decode_t1 = std::chrono::high_resolution_clock::now();
    decodeTimerRunning = true;
}

void rlncdecoder::stopDecodeTimer()
{
    if (decodeTimerRunning)
    {
        auto decode_t2 = std::chrono::high_resolution_clock::now();
        decodeTimer += std::chrono::duration_cast<std::chrono::microseconds>(decode_t2-decode_t1);
    }
    decodeTimerRunning = false;
}

void rlncdecoder::addRLNCMessage(RLNCMessage input_message)
{
#ifndef COMPUTE_RLNC
    return;
#endif

    packets_added_packetpool++;

    startAddLockTimer ();
//    CPPDEBUG("rlncdecoder::addRLNCMessage " << std::endl);
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
//            CPPDEBUG("Original: " << packet_pool.find(pktids.front())->second << std::endl);
//            CPPDEBUG("New: " << msg << std::endl);
            if (!(packet_pool[pktids.front()] == input_message))
            {
                CPPDEBUG("Stored packet: \n" << packet_pool[pktids.front()] << std::endl);
                CPPDEBUG("Recvd packet: \n" << input_message << std::endl);
                throw std::logic_error("rlncdecoder::addRLNCMessage: packet ids are supposed unique: \
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

void rlncdecoder::clearPacketPool ()
{
    std::lock_guard<std::mutex> packet_guard(packet_pool_mutex);
    packet_pool.clear ();
    NC_packets.clear ();
}

size_t rlncdecoder::getPacketPoolSize ()
{
    return packet_pool.size ();
}

RLNCMessage *rlncdecoder::produceDecodedRLNCMessage()
{
#ifndef COMPUTE_RLNC
    return NULL;
#endif
    decodes_attempted++;

    startDecodeTimer();

    RLNCMessage *decoded_msg = NULL;
    //Leave enough space for RLNC coefficients
    std::array<uint8_t, MAX_PACKET_SIZE + 20> payload;
    payload.fill(0);
    //Enough space for two symbols to be decoded
    std::array<uint8_t, MAX_PACKET_SIZE*2> data_out;
    data_out.fill(0);


    std::map<size_t, RLNCMessage> available_packets;
    RLNCMessage active_encoded_packet;
    size_t decoded_packet_index = -1;

    //Check if we can decode something and then copy out the relevant packets
//  CPPDEBUG("produceDecodedRLNCMessage Starting lockguard block" << std::endl);
    std::unique_lock<std::mutex> guard(packet_pool_mutex);

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
//        //If we have all the pktids, this message has already been decoded
        if (available_ids == pktids.size())
        {
//                    CPPDEBUG("We have all the packetids, this message has already been decoded,\n"
//                             << "Or an old packet is clouding our judgement " << std::endl);
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
//                    CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage Index of missing packet : " << decoded_packet_index << std::endl);
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
//            NC_packets.clear();
            break;
        }

        else {
            CPPDEBUG("Too many packets missing, erasing and trying next packet" << std::endl);
            NC_packets.erase (NC_packets.begin() + i);
            continue;
        }
    }

    //Shared variable access done
    guard.unlock ();

    if (available_packets.size () == active_encoded_packet.getPacketIds().size())
    {
        CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage Packet already decoded" << std::endl);
        packets_already_decoded++;
        stopDecodeTimer();
        return NULL;
    }
    else if (available_packets.size () != (active_encoded_packet.getPacketIds ().size () - 1))
    {
        CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage Could not find enough available packets to decode" << std::endl);
        packets_missing_undecodable++;
        stopDecodeTimer();
        return NULL;
    }

//        CPPDEBUG("\nrlncdecoder::produceDecodedRLNCMessage Attempting to decode, all msgs available except 1" << std::endl);
    auto decoder = decoder_factory.build();
    decoder->set_mutable_symbols(storage::storage(data_out));
    size_t k = 0;

    //Sanity check
    if (available_packets.size() != 1)
    {
        throw std::logic_error(std::string("rlncdecoder::produceDecodedRLNCMessage available_packet.size() ") +
                               "should be 1, as we only currently code 2 packets together\n");
    }

    CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage packets encoded together: "
             << active_encoded_packet.getPacketIds()[0]
            << "+" << active_encoded_packet.getPacketIds()[1] << std::endl);
    CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage decoded_msg->to_addrs[0] " << active_encoded_packet.getToAddrs()[0] << std::endl);
    CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage decoded_msg->from_ids[0] " << active_encoded_packet.getFromIds()[0] << std::endl);
    CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage decoded_msg->to_addrs[1] " << active_encoded_packet.getToAddrs()[1] << std::endl);
    CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage decoded_msg->from_ids[1] " << active_encoded_packet.getFromIds()[1] << std::endl << std::endl);



    //Used to check if the checksum of two decoded packets are correct
    uint32_t total_checksum = 0;

    for (k = 0; k < active_encoded_packet.getPacketIds ().size(); k++)
    {

        if (k == decoded_packet_index)
            continue;

//            payload.fill(0);

//            //Quickly construct a encoder to give us the correct
        std::array<uint8_t, MAX_PACKET_SIZE> buffer;
//            buffer.fill(0);
//            auto encoder = encoder_factory.build();
//            encoder->set_systematic_on();

//            //Serialize the whole packet, not just the (un-encoded) message
        available_packets[k].serialize(reinterpret_cast<char*>(buffer.data()));
        uint32_t checksum = 0;
        checksum = RLNCMessage::generateChecksum(buffer.data(), available_packets[k].sizeOf());
        Logger::saveBinaryArray("RLNCdecoder " + std::to_string(available_packets[k].getPacketIds()[0]) + ".txt", buffer.data(), available_packets[k].sizeOf());
        total_checksum += checksum;
        CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage packet:" << available_packets[k].getPacketIds()[0] << "->checksum=" << checksum << std::endl);
        CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage packet:" << available_packets[k].getPacketIds()[0] << "->sizeOf=" << available_packets[k].sizeOf() << std::endl);
        CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage packet" << available_packets[k] << std::endl);

//            CPPDEBUG("Packet from packet pool: " << available_packets[k] << std::endl);

//            encoder->set_const_symbol(k, storage::storage(buffer));
//            encoder->write_payload(payload.data());

        //Use the new systematic uncoded packet to decode received packet
//            decoder->read_payload(payload.data());
        decoder->read_uncoded_symbol (buffer.data(), k);

        //Stop if we have enough packets to decode.
        if (decoder->is_complete ())
        {
            break;
            //Sanity check
        }

    }

    //Copy the coded packet in
    payload.fill (0);
    memcpy(payload.data(), active_encoded_packet.getMessage(), active_encoded_packet.getMessageSize());
    decoder->read_payload(payload.data());

//    CPPDEBUG("Coded packet from NC_packets: " << (active_encoded_packet) << std::endl);

    if (decoder->is_complete())
    {
    //    CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage: Size of packet_pool: " << packet_pool.size () << std::endl);
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
            decoded_msg = new RLNCMessage(msg);

            //Calculate checksum to see if decoding is correct
            total_checksum += RLNCMessage::generateChecksum(data_out.data () + decoded_packet_index * MAX_PACKET_SIZE,
                                                     msg.sizeOf());

            if (total_checksum != active_encoded_packet.getChecksum())
            {
                CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage Checksum was not correct"
                         << std::endl << (*decoded_msg) << std::endl);
                packets_checksum_incorrect++;
                delete decoded_msg;
                decoded_msg = NULL;
                stopDecodeTimer();
                return NULL;
            }

            packets_recovered++;
        }
        else
        {
            CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage: Could not deserialize the decoded packet" << std::endl);
        }

    }
    else
    {
        CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage: Could not decode, probably linearly dependent, deleting it and its packets" << std::endl);
//            std::vector<packetid_t> pkt_ids_to_delete = NC_packets[i].getPacketIds ();
//            for (auto it = pkt_ids_to_delete.begin (); it != pkt_ids_to_delete.end (); ++it)
//            {
//                if (packet_pool.find(*it) != packet_pool.end ())
//                {
//                    packet_pool.erase (packet_pool.find (*it));
//                }
//            }
        //CPPDEBUG("Sizeof NC_packets: " << NC_packets.size() << std::endl);
    }

    stopDecodeTimer();
    return decoded_msg;
}

rlncdecoder::~rlncdecoder ()
{
    CPPDEBUG("\n~rlncdecoder:: packets added to packet pool: " << packets_added_packetpool << std::endl);
    CPPDEBUG("~rlncdecoder:: decodes_attempted: " << decodes_attempted << std::endl);
    CPPDEBUG("~rlncdecoder:: packet_recovered: " << packets_recovered << std::endl);
    CPPDEBUG("~rlncdecoder:: packets_already_decoded: " << packets_already_decoded << std::endl);
    CPPDEBUG("~rlncdecoder:: packets_missing_undecodable: " << packets_missing_undecodable << std::endl);
    CPPDEBUG("~rlncdecoder:: packets_checksum_incorrect: " << packets_checksum_incorrect << std::endl);
    CPPDEBUG("~rlncdecoder:: max_packetpool_size: " << max_packetpool_size << std::endl);
    CPPDEBUG("~rlncdecoder:: max_NC_packets_size: " << max_NC_packets_size << std::endl);
    CPPDEBUG("~rlncdecoder:: time spent in addLock: " << addLockTimer.count() / 1000 << " milliseconds " << std::endl);
    CPPDEBUG("~rlncdecoder:: time spent decoding: " << decodeTimer.count() / 1000 << " milliseconds " << std::endl);
    if (decodes_attempted > 0)
        CPPDEBUG("~rlncdecoder:: time spent decoding per msg: " << decodeTimer.count() / decodes_attempted << " microseconds " << std::endl);
}

