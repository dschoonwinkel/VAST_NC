#include "rlncrecoder.h"
#include <iostream>
#include "net_manager.h"
#include <thread>
#include "logger.h"

RLNCrecoder::RLNCrecoder() :
    encoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE),
    header_factory(GENSIZE, 0)
{

}

void RLNCrecoder::addRLNCMessage(RLNCMessage msg)
{
    Logger::debugThread("RLNCrecoder::addRLNCMessage");
    auto pktids = msg.getPacketIds();

    //Ignore all NET_ID_UNASSIGNED
    if (msg.getFirstFromId () == NET_ID_UNASSIGNED)
    {
        CPPDEBUG("RLNCrecoder::addRLNCMessage Could not add pkt from NET_ID_UNASSIGNED" << std::endl);
        return;
    }
    //If an uncoded packet and it is within the codable size
    else if (pktids.size() == 1 && msg.sizeOf() < MAX_PACKET_SIZE)
    {
        if (packet_pool.find(pktids.front()) != packet_pool.end())
        {
            std::cout << "Replacing packet " << pktids.front() << std::endl;
        }
        //print threadid
        packet_pool[pktids.front()] = msg;

    }
    else if (msg.getMessageSize() > MAX_PACKET_SIZE)
        std::cerr << "rlncrecoder::addRLNCMessage: Could not add packet, size too large" << std::endl;
    else
    {
        CPPDEBUG("rlncrecoder::addRLNCMessage: could not add packet, because reasons..." << std::endl);
    }

    if (packet_pool.size() > max_packetpool_size)
    {
        max_packetpool_size = packet_pool.size();
    }

    for (auto iter = packet_pool.begin(); iter != packet_pool.end(); ++iter)
    {
        if (iter->first != iter->second.getPacketIds()[0])
        {
            CPPDEBUG("key: " << iter->first << " should be: " << iter->second.getPacketIds()[0] << std::endl);
            throw std::logic_error("Packet ids and keys do not match");
        }
    }
}

RLNCMessage *RLNCrecoder::produceRLNCMessage()
{
    Logger::debugThread("RLNCrecoder::produceRLNCMessage");
    if (packet_pool.size() < 2)
    {
        CPPDEBUG("RLNCrecoder::produceRLNCMessage packet_pool.size() too small" << std::endl);
        return NULL;
    }

    //print packet_pool size

    RLNCMessage *message = NULL;

    auto encoder = encoder_factory.build();
//    RLNCMessage& message1 = packet_pool.begin()->second;
//    RLNCMessage& message2 = packet_pool.rbegin()->second;

    RLNCMessage *message1 = &packet_pool.begin()->second;
    RLNCMessage *message2 = &packet_pool.rbegin()->second;

    //Cannot encode packets from the same host together
    for (auto reverse_iter = packet_pool.rbegin();
         message1->getFirstFromId() == message2->getFirstFromId() && reverse_iter != packet_pool.rend();
         ++reverse_iter)
    {
        message2 = &reverse_iter->second;
//        CPPDEBUG("rlncrecoder::produceRLNCMessage searching for packet from different host" << std::endl);
    }

    if (message1->getFirstFromId() == message2->getFirstFromId())
    {
        CPPDEBUG("rlncrecoder::produceRLNCMessage could not find packets from different hosts" << std::endl);
        return NULL;
    }

    std::array<uint8_t, MAX_PACKET_SIZE> data1;
    data1.fill(0);
    std::array<uint8_t, MAX_PACKET_SIZE> data2;
    data2.fill(0);

    //Write itself to the buffer
    message1->serialize(reinterpret_cast<char*>(data1.data()));
    message2->serialize(reinterpret_cast<char*>(data2.data()));

    auto header = header_factory.build();
    message = new RLNCMessage(header);
//    message->putPacketId(message1->getPacketIds()[0]);
//    message->putFromId (message1->getFromIds ()[0]);
//    message->putToAddr (message1->getToAddrs ()[0]);
    message->putIdsAddr(message1->getPacketIds()[0],
                        message1->getFromIds ()[0],
                        message1->getToAddrs ()[0]);

//    message->putPacketId(message2->getPacketIds()[0]);
//    message->putFromId (message2->getFromIds ()[0]);
//    message->putToAddr (message2->getToAddrs ()[0]);

    message->putIdsAddr(message2->getPacketIds()[0],
                        message2->getFromIds ()[0],
                        message2->getToAddrs ()[0]);

    uint32_t checksum = RLNCMessage::generateChecksum(data1.data(), message1->sizeOf());
    checksum += RLNCMessage::generateChecksum(data2.data(), message2->sizeOf());
    message->setChecksum(checksum);

    encoder->set_const_symbol (0, storage::storage (data1));
    encoder->set_const_symbol (1, storage::storage (data2));
    encoder->set_systematic_off();
//    std::cout << "Payload size: " << encoder->payload_size () << std::endl;
//    encoder->write_payload(reinterpret_cast<uint8_t*>(message->getMessage(encoder->payload_size())));
    std::vector<uint8_t> payload_buffer(encoder->payload_size ());
    encoder->write_payload (payload_buffer.data ());
    message->putMessage (reinterpret_cast<char*>(payload_buffer.data()), encoder->payload_size ());

    packet_pool.erase(packet_pool.find(message1->getPacketIds()[0]));
    packet_pool.erase(packet_pool.find(message2->getPacketIds()[0]));

    //This is almost always 0
//    std::cout << "Recoder packet_pool size: " << packet_pool.size() << std::endl;

    return message;
}

size_t RLNCrecoder::getPacketPoolSize ()
{
    return packet_pool.size();
}

RLNCrecoder::~RLNCrecoder()
{
    std::cout << "~RLNCrecoder: Max packet_pool size: " << max_packetpool_size << std::endl;
}
