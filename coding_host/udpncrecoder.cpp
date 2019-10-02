#include "udpncrecoder.h"
#include <iostream>
#include "net_manager.h"
#include <thread>
#include "logger.h"

UDPNCrecoder::UDPNCrecoder() :
    encoder_factory(field, MAX_SYMBOLS, MAX_PACKET_SIZE),
    header_factory(GENSIZE, 0)
{

}

void UDPNCrecoder::startEncodeTimer()
{
    t1 = std::chrono::high_resolution_clock::now();
    encodeTimerRunning = true;
}

void UDPNCrecoder::stopEncodeTimer()
{
    if (encodeTimerRunning)
    {
        auto t2 = std::chrono::high_resolution_clock::now();
        encodeTimer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
    }
    encodeTimerRunning = false;
}

void UDPNCrecoder::addUDPNCMessage(UDPNCMessage msg)
{
//    Logger::debugThread("UDPNCrecoder::addUDPNCMessage");
    auto pktids = msg.getPacketIds();

    //Ignore all NET_ID_UNASSIGNED
    if (msg.getFirstFromId () == NET_ID_UNASSIGNED)
    {
        CPPDEBUG("UDPNCrecoder::addUDPNCMessage Could not add pkt from NET_ID_UNASSIGNED" << std::endl);
        return;
    }
    //If an uncoded packet and it is within the codable size
    else if (pktids.size() == 1 && msg.sizeOf() < MAX_PACKET_SIZE)
    {
        if (packet_pool.find(pktids.front()) != packet_pool.end())
        {
            CPPDEBUG("Replacing packet " << pktids.front() << std::endl);
        }
        //print threadid
        packet_pool[pktids.front()] = msg;

    }
    else if (msg.getMessageSize() > MAX_PACKET_SIZE)
        CPPDEBUG("udpncrecoder::addUDPNCMessage: Could not add packet, size too large" << std::endl);
    else
    {
        CPPDEBUG("udpncrecoder::addUDPNCMessage: could not add packet, because reasons..." << std::endl);
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

std::shared_ptr<UDPNCMessage> UDPNCrecoder::produceUDPNCMessage()
{
    startEncodeTimer();
//    Logger::debugThread("UDPNCrecoder::produceUDPNCMessage");
    if (packet_pool.size() < 2)
    {
        CPPDEBUG("UDPNCrecoder::produceUDPNCMessage packet_pool.size() too small" << std::endl);
        stopEncodeTimer();
        return nullptr;
    }

    //print packet_pool size

    auto encoder = encoder_factory.build();

    UDPNCMessage *message1 = &packet_pool.begin()->second;
    UDPNCMessage *message2 = &packet_pool.rbegin()->second;

    //Cannot encode packets from the same host together
    for (auto reverse_iter = packet_pool.rbegin();
         message1->getFirstFromId() == message2->getFirstFromId() && reverse_iter != packet_pool.rend();
         ++reverse_iter)
    {
        message2 = &reverse_iter->second;
    }

    if (message1->getFirstFromId() == message2->getFirstFromId())
    {
        CPPDEBUG("udpncrecoder::produceUDPNCMessage could not find packets from different hosts" << std::endl);
        stopEncodeTimer();
        return nullptr;
    }

    std::array<uint8_t, MAX_PACKET_SIZE> data1;
    data1.fill(0);
    std::array<uint8_t, MAX_PACKET_SIZE> data2;
    data2.fill(0);

    //Write itself to the buffer
    message1->serialize(reinterpret_cast<char*>(data1.data()));
    message2->serialize(reinterpret_cast<char*>(data2.data()));

    auto header = header_factory.build();
    std::shared_ptr<UDPNCMessage> message (new UDPNCMessage(header));
    message->putIdsAddr(message1->getPacketIds()[0],
                        message1->getFromIds ()[0],
                        message1->getToAddrs ()[0]);

    message->putIdsAddr(message2->getPacketIds()[0],
                        message2->getFromIds ()[0],
                        message2->getToAddrs ()[0]);

    uint32_t checksum1 = UDPNCMessage::generateChecksum(data1.data(), message1->sizeOf());
#ifdef SAVE_PACKETS
    Logger::saveBinaryArray("UDPNCrecoder_" + std::to_string(message1->getPacketIds()[0]) + ".txt", data1.data(), message1->sizeOf());
#endif
    uint32_t checksum2 = UDPNCMessage::generateChecksum(data2.data(), message2->sizeOf());
#ifdef SAVE_PACKETS
    Logger::saveBinaryArray("UDPNCrecoder_" + std::to_string(message2->getPacketIds()[0]) + ".txt", data2.data(), message2->sizeOf());
#endif
    message->setChecksum(checksum1 + checksum2);

    encoder->set_const_symbol (0, storage::storage (data1));
    encoder->set_const_symbol (1, storage::storage (data2));
    encoder->set_systematic_off();
    std::vector<uint8_t> payload_buffer(encoder->payload_size ());
    encoder->write_payload (payload_buffer.data ());
    message->putMessage (reinterpret_cast<char*>(payload_buffer.data()), encoder->payload_size ());

    packet_pool.erase(packet_pool.find(message1->getPacketIds()[0]));
    packet_pool.erase(packet_pool.find(message2->getPacketIds()[0]));

    stopEncodeTimer();
    packets_encoded++;
    return message;
}

size_t UDPNCrecoder::getPacketPoolSize ()
{
    return packet_pool.size();
}

UDPNCrecoder::~UDPNCrecoder()
{
    std::cout << "~UDPNCrecoder: Max packet_pool size: " << max_packetpool_size << std::endl;
    std::cout << "~UDPNCrecoder: packets_encoded: " << packets_encoded << std::endl;
    std::cout << "~UDPNCrecoder:: time spent encoding: " << encodeTimer.count() / 1000 << " milliseconds " << std::endl;
    if (packets_encoded > 0)
    {
        std::cout << "~UDPNCrecoder:: time spent encoding per packet: "
                 << encodeTimer.count() / packets_encoded
                 << " microseconds " << std::endl << std::endl;
    }

}
