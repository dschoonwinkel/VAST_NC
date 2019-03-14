#include "rlncrecoder.h"
#include <iostream>

RLNCrecoder::RLNCrecoder() :
    encoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE),
    header_factory(GENSIZE, 0)
{

}

void RLNCrecoder::addRLNCMessage(RLNCMessage msg)
{
    auto pktids = msg.getPacketIds();
    //If an uncoded packet and it is within the codable size
    if (pktids.size() == 1 && msg.sizeOf() < MAX_PACKET_SIZE)
        packet_pool[pktids.front()] = msg;
    else if (msg.getMessageSize() > MAX_PACKET_SIZE)
        std::cerr << "rlncrecoder::addRLNCMessage: Could not add packet, size too large" << std::endl;
}

RLNCMessage *RLNCrecoder::produceRLNCMessage()
{
    if (packet_pool.size() < 2)
    {
        return NULL;
    }

    RLNCMessage *message = NULL;

    auto encoder = encoder_factory.build();
    RLNCMessage& message1 = packet_pool.begin()->second;
    RLNCMessage& message2 = packet_pool.rbegin()->second;

    std::array<uint8_t, MAX_PACKET_SIZE> data1;
    data1.fill(0);
    std::array<uint8_t, MAX_PACKET_SIZE> data2;
    data2.fill(0);

    //Write itself to the buffer
    message1.serialize(reinterpret_cast<char*>(data1.data()));
    message2.serialize(reinterpret_cast<char*>(data2.data()));


    auto header = header_factory.build();
    message = new RLNCMessage(header);
    message->putPacketId(message1.getPacketIds()[0]);
    message->putFromId (message1.getFromIds ()[0]);
    message->putPacketId(message2.getPacketIds()[0]);
    message->putFromId (message2.getFromIds ()[0]);

    encoder->set_const_symbol (0, storage::storage (data1));
    encoder->set_const_symbol (1, storage::storage (data2));
    encoder->set_systematic_off();
    std::cout << "Payload size: " << encoder->payload_size () << std::endl;
    encoder->write_payload(reinterpret_cast<uint8_t*>(message->getMessage(encoder->payload_size())));

    packet_pool.erase(packet_pool.find(message1.getPacketIds()[0]));
    packet_pool.erase(packet_pool.find(message2.getPacketIds()[0]));

    return message;
}
