#include "rlncdecoder.h"
#include "rlncrecoder.h"
#include "rlnc_packet_factory.h"

#include <assert.h>

#include <boost/asio.hpp>

using namespace boost::asio;

void testEncodedChecksum()
{
    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};
    char data3[] = {'f', 'e', 'e', 'd'};

    RLNCrecoder recoder;
    rlncdecoder decoder;

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);
    RLNCMessage message3(header2);

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(123);
    message1.putFromId (123);
    message1.putToAddr(addr1);

    message2.putPacketId(654);
    message2.putFromId (654);
    message2.putToAddr(addr1);

    //Incorrect replacement for message1
    message3.putPacketId(123);
    message3.putFromId (123);
    message3.putToAddr(addr1);

    message1.putMessage(data1, 4);
    message2.putMessage(data2, 4);
    message3.putMessage(data3, 4);

    uint32_t checksum = 0;
    std::array<char, MAX_PACKET_SIZE> buffer;
    message1.serialize(buffer.data());
    checksum = RLNCMessage::generateChecksum(buffer.data(), message1.sizeOf());

    buffer.fill(0);
    message2.serialize(buffer.data());
    checksum += RLNCMessage::generateChecksum(buffer.data(), message1.sizeOf());

    std::cout << "Symbol 1:" << message1 << std::endl;
    std::cout << "Symbol 2:" << message2 << std::endl;

    recoder.addRLNCMessage(message1);
    recoder.addRLNCMessage(message2);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    if (temp_msg == NULL)
    {
        std::cerr << "Could not produce coded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Coded Symbol:" << (*temp_msg) << std::endl;

    decoder.addRLNCMessage(*temp_msg);

    decoder.addRLNCMessage(message1);
    RLNCMessage *decoded_msg = decoder.produceDecodedRLNCMessage();

    if (decoded_msg != NULL)
    {
        std::cout << "Decoded Symbol: " << std::endl
                  << (*decoded_msg) << std::endl;
    }

    assert(*decoded_msg == message2);

    //Try to decode with the wrong packet
    decoder.addRLNCMessage(*temp_msg);
    decoder.addRLNCMessage(message3);
    decoded_msg = decoder.produceDecodedRLNCMessage();

    assert(decoded_msg == NULL);
}

void testSameFromIDs()
{
    std::cout << "Running testSameFromIDs" << std::endl;
    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};
    char data3[] = {'f', 'e', 'e', 'd'};

    RLNCrecoder recoder;

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();
    RLNCHeader header3 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);
    RLNCMessage message3(header3);

    Vast::IPaddr addr1("127.0.0.1", 1037);
    Vast::IPaddr addr2("127.0.0.2", 1037);
    Vast::IPaddr addr3("127.0.0.3", 1037);

    message1.putPacketId(123);
    message1.putFromId (123);
    message1.putToAddr(addr1);

    message2.putPacketId(654);
    message2.putFromId (123);
    message2.putToAddr(addr2);

    message2.putPacketId(789);
    message2.putFromId (234);
    message2.putToAddr(addr3);

    message1.putMessage(data1, 4);
    message2.putMessage(data2, 4);
    message3.putMessage(data3, 4);

    uint32_t checksum = 0;
    std::array<char, MAX_PACKET_SIZE> buffer;
    message1.serialize(buffer.data());
    checksum = RLNCMessage::generateChecksum(buffer.data(), message1.sizeOf());

    buffer.fill(0);
    message3.serialize(buffer.data());
    checksum += RLNCMessage::generateChecksum(buffer.data(), message3.sizeOf());

    std::cout << "Symbol 1:" << message1 << std::endl;
    std::cout << "Symbol 2:" << message2 << std::endl;

    recoder.addRLNCMessage(message1);
    recoder.addRLNCMessage(message2);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    //Packets have the same fromID, should not be encoded together
    assert(temp_msg == NULL);

    std::cout << "Symbol 3:" << message3 << std::endl;
    std::cout << "Adding symbol 3 to rlncrecoder" << std::endl;

    recoder.addRLNCMessage(message3);

    temp_msg = recoder.produceRLNCMessage();

    assert(temp_msg != NULL);

    assert(temp_msg->getChecksum() == checksum);

}


int main()
{
    testEncodedChecksum();
    testSameFromIDs();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;
    return 0;
}
