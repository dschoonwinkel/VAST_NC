#include "rlncdecoder.h"

rlncdecoder::rlncdecoder ():
    decoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE),
    encoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE)
{
    CPPDEBUG("RLNCdecoder::constructor: finite field" << std::endl);
    CPPDEBUG(int(field) << std::endl);

}

void rlncdecoder::addRLNCMessage(RLNCMessage msg)
{
    auto pktids = msg.getPacketIds();
    std::lock_guard<std::mutex> guard(packet_pool_mutex);

    //If an uncoded packet and it is within the codable size
    if (pktids.size() == 1 && msg.getMessageSize())
    {
        if (packet_pool.find (pktids.front()) != packet_pool.end ())
        {
            //CPPDEBUG("Replacing a packet " << pktids.front() << std::endl);
//            CPPDEBUG("Original: " << packet_pool.find(pktids.front())->second << std::endl);
//            CPPDEBUG("New: " << msg << std::endl);
        }
        packet_pool[pktids.front()] = msg;
    }

    //If encoded, add to NC_packets queue to be decoded
    else if (pktids.size() > 1)
    {
        NC_packets.push_back(msg);    
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
    RLNCMessage *decoded_msg = NULL;
    //Leave enough space for RLNC coefficients
    std::array<uint8_t, MAX_PACKET_SIZE + 20> payload;
    payload.fill(0);
    //Enough space for two symbols to be decoded
    std::array<uint8_t, MAX_PACKET_SIZE*2> data_out;
    data_out.fill(0);

    std::lock_guard<std::mutex> guard(packet_pool_mutex);

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
            CPPDEBUG("We have all the packetids, this message has already been decoded,\n"
                     << "Or an old packet is clouding our judgement " << std::endl);
        }

        //If we have all the pktids except 1, this NC message contains a useful message
        if (available_ids >= pktids.size()-1)
        {
            CPPDEBUG("\nrlncdecoder::produceDecodedRLNCMessage Attempting to decode, all msgs available except 1" << std::endl);
            auto decoder = decoder_factory.build();
            decoder->set_mutable_symbols(storage::storage(data_out));
            size_t k = 0;

            for (k = 0; k < pktids.size(); k++)
            {
//                CPPDEBUG("Iterating over pktids: " << k << std::endl);
                auto packet = packet_pool.find(pktids[k]);
                if (packet != packet_pool.end())
                {
                    payload.fill(0);

                    //Quickly construct a encoder to give us the correct
                    std::array<uint8_t, MAX_PACKET_SIZE> buffer;
                    buffer.fill(0);
                    auto encoder = encoder_factory.build();
                    encoder->set_systematic_on();

                    //Serialize the whole packet, not just the (un-encoded) message
                    packet->second.serialize(reinterpret_cast<char*>(buffer.data()));

                    //CPPDEBUG("Packet from packet pool: " << (packet->second) << std::endl);

                    encoder->set_const_symbol(k, storage::storage(buffer));
                    encoder->write_payload(payload.data());

                    //Use the new systematic uncoded packet to decode received packet
                    decoder->read_payload(payload.data());

                    //Stop if we have enough packets to decode.
                    if (decoder->is_complete ())
                    {
                        break;
                    }
                }

            }

            //Copy the coded packet in
            payload.fill (0);
            memcpy(payload.data(), NC_packets[i].getMessage(), NC_packets[i].getMessageSize());
            decoder->read_payload(payload.data());

//            CPPDEBUG("Coded packet from NC_packets: " << (NC_packets[i]) << std::endl);

            if (decoder->is_complete())
            {
                CPPDEBUG("rlncdecoder::produceDecodedRLNCMessage: Size of packet_pool: " << packet_pool.size () << std::endl);
                if (packet_pool.size() > 1)
                {
                    //This can be a problem - preferrably we only want the necessary
                    //packets in the packet pool for decoding, without potentially stale
                    // packets, but this should not be a train smash...
//                    std::abort();
                }

                for (size_t m = 0; m < pktids.size (); m ++)
                {
                    RLNCMessage msg;
                    if (msg.deserialize (reinterpret_cast<char*>(data_out.data () + m * MAX_PACKET_SIZE), MAX_PACKET_SIZE) != -1)
                    {

                        //If I have received a message with similar packetid, check if it is unique
                        if (packet_pool.find(msg.getPacketIds ().front ()) != packet_pool.end())
                        {
                            RLNCMessage& storedMsg = (packet_pool.find(msg.getPacketIds ().front ()))->second;
                            if (storedMsg == msg)
                            {
//                                CPPDEBUG("Decoded packet was (maybe) not unique, skipping" << std::endl);
//                                CPPDEBUG(msg << std::endl);
                                //Erase the packet from packet pool. It will not be used by codinghost again
                                packet_pool.erase (packet_pool.find (msg.getPacketIds ().front()));

                                //Ignore the already received packet
                                continue;
                            }

                        }
                        //Else, use the newly decoded packet - should be most cases true
                        decoded_msg = new RLNCMessage(msg);
                        packets_recovered++;
                        NC_packets.erase (NC_packets.begin () + i);
//                        return decoded_msg;
                    }
                    else
                    {
                        CPPDEBUG("Could not deserialize the decoded packet" << std::endl);
                    }

//                    CPPDEBUG("Decoded packet details: " << msg << std::endl);
                }

            }
            else
            {
                CPPDEBUG("Could not decode, probably linearly dependent, deleting it and its packets" << std::endl);
                std::vector<packetid_t> pkt_ids_to_delete = NC_packets[i].getPacketIds ();
                for (auto it = pkt_ids_to_delete.begin (); it != pkt_ids_to_delete.end (); ++it)
                {
                    if (packet_pool.find(*it) != packet_pool.end ())
                    {
                        packet_pool.erase (packet_pool.find (*it));
                    }
                }
                NC_packets.erase (NC_packets.begin() + i);
                CPPDEBUG("Sizeof NC_packets: " << NC_packets.size() << std::endl);
            }

        }
        else {
            CPPDEBUG("Too many packets missing, deleting from NC_packets" << std::endl);
            NC_packets.erase (NC_packets.begin() + i);
        }
    }

    return decoded_msg;
}

rlncdecoder::~rlncdecoder ()
{
    CPPDEBUG("~rlncdecoder:: packet_recovered: " << packets_recovered << std::endl);
}

