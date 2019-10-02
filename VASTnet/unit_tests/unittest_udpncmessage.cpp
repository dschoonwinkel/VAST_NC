#include <iostream>
#include <udpncmessage.h>
#include <assert.h>
#include <string.h>
#include <queue>
#include "logger.h"

void runUnitTest1()
{
    std::cout << "Not implemented yet!" << std::endl;
    std::abort();
}

void testPacketIds()
{
    std::cout << "testPacketIds" << std::endl;
    UDPNCMessage msg1;
    packetid_t id1 = 1234567890;

    msg1.putPacketId (id1);

    assert(msg1.getPacketIds ()[0] == id1);
}

void testFromId()
{
    std::cout << "testFromId" << std::endl;
    UDPNCMessage msg1;
    packetid_t id1 = 1234567890;
    Vast::id_t from_id = 13132323123;
    Vast::id_t from_id2 = 1345982345;

    msg1.putPacketId (id1);
    msg1.putFromId (from_id);

    assert(msg1.getFromIds ()[0] == from_id);
    assert(msg1.getFirstFromId () == from_id);

    msg1.putFromId (from_id2);

    assert(msg1.getFirstFromId () != from_id2);
}

void testToAddrs()
{
    std::cout << "testToAddrs" << std::endl;
    UDPNCMessage msg1;
    packetid_t id1 = 1234567890;
    Vast::id_t from_id = 13132323123;
    Vast::IPaddr addr1("127.0.0.1", 1037);

    msg1.putPacketId (id1);
    msg1.putFromId (from_id);
    msg1.putToAddr (addr1);

    assert(msg1.getFromIds ()[0] == from_id);
    assert(msg1.getFirstFromId () == from_id);
    assert(msg1.getToAddrs ()[0] == addr1);

}

void testPutMessage()
{
    std::cout << "testPutMessage" << std::endl;
    std::string test1 = "Hello World!123";

    UDPNCMessage msg1;

    msg1.putMessage (test1.c_str (), test1.length ());

    assert(strcmp(msg1.getMessage (), test1.c_str ()) == 0);
    assert(memcmp(msg1.getMessageU (), test1.c_str (), test1.length ()) == 0);

    assert(strlen(msg1.getMessage ()) == test1.length ());
}

void testOrdering()
{
    std::cout << "testOrdering" << std::endl;
    UDPNCMessage msg1;

    msg1.putOrdering (123);

    assert(msg1.getOrdering () == 123);
}

void testFactoryOrdering()
{
    UDPNCHeader_factory factory;
    UDPNCHeader header1 = factory.build ();
    UDPNCHeader header2 = factory.build();

    assert(header1.ordering == 1);
    assert(header2.ordering == 2);
}

void testSizeOfSerialize()
{
    std::cout << "testSizeOfSerialize" << std::endl;

    std::string test1 = "Hello World!123";

    UDPNCHeader_factory factory1;
    UDPNCHeader header1 = factory1.build ();

    header1.enc_packet_count = 1;
    header1.generation = 1;
    header1.gensize = 0;
    header1.ordering = 3;

    UDPNCMessage msg1(header1);
    packetid_t id1 = 1234567890;
    Vast::id_t from_id = 13132323123;
    Vast::IPaddr addr1("127.0.0.1", 1037);

    msg1.putPacketId (id1);
    msg1.putFromId (from_id);
    msg1.putToAddr (addr1);

    msg1.putMessage (test1.c_str (), test1.length ());

    //Sizeof UDPNCMessage:
    //Header = 8 bytes
    //Packetid = 8 bytes
    //FromId = 8 bytes
    //ToAddr = 4 bytes IP + 2 bytes port
    //Variable length msg in chars
    //Checksum
    assert(msg1.sizeOf () == (sizeof(UDPNCHeader)
                              + sizeof(packetid_t) * 1 + sizeof(Vast::id_t) * 1
                              + sizeof(uint32_t) + 2 * sizeof(uint16_t)
                              + sizeof(char) * test1.length ()
                              + sizeof(uint32_t)
                              )
           );

    char buffer[msg1.sizeOf ()];

    msg1.serialize (buffer);

    UDPNCMessage msg2;

    msg2.deserialize (buffer, msg1.sizeOf ());

    assert(msg1 == msg2);
}

void testEquals()
{
    std::cout << "testEquals" << std::endl;

    std::string test1 = "Hello World!123";

    UDPNCHeader_factory factory1;
    UDPNCHeader header1 = factory1.build ();

    header1.enc_packet_count = 1;
    header1.generation = 1;
    header1.gensize = 0;
    header1.ordering = 3;

    UDPNCMessage msg1(header1);
    packetid_t id1 = 1234567890;
    Vast::id_t from_id = 13132323123;
    Vast::IPaddr addr1("127.0.0.1", 1037);
    Vast::IPaddr sock_addr("127.0.1.2", 1039);

    msg1.putPacketId (id1);
    msg1.putFromId (from_id);
    msg1.putToAddr (addr1);

    msg1.putMessage (test1.c_str (), test1.length ());
    msg1.socket_addr = sock_addr;

    UDPNCMessage msg2(header1);

    msg2.putPacketId (id1);
    msg2.putFromId (from_id);
    msg2.putToAddr (addr1);

    msg2.putMessage (test1.c_str (), test1.length ());
    msg2.socket_addr = sock_addr;

    assert(msg1 == msg2);

    Vast::id_t from_id2 = 1345982345;
    msg2.putPacketId (id1);
    msg2.putFromId (from_id2);
    msg2.putToAddr (addr1);

    assert(!(msg1 == msg2));

}

void testQueueDequeue()
{
    std::queue<UDPNCMessage> queue1;

    std::cout << "testQueueDequeue" << std::endl;

    std::string test1 = "Hello World!123";

    UDPNCHeader_factory factory1;
    UDPNCHeader header1 = factory1.build ();

    header1.enc_packet_count = 1;
    header1.generation = 1;
    header1.gensize = 0;
    header1.ordering = 3;

    UDPNCMessage msg1(header1);
    packetid_t id1 = 1234567890;
    Vast::id_t from_id = 13132323123;
    Vast::IPaddr addr1("127.0.0.1", 1037);
    Vast::IPaddr sock_addr("127.0.1.2", 1039);

    msg1.putPacketId (id1);
    msg1.putFromId (from_id);
    msg1.putToAddr (addr1);

    msg1.putMessage (test1.c_str (), test1.length ());
    msg1.socket_addr = sock_addr;

    queue1.push(msg1);

    UDPNCMessage msg2 = queue1.front();

    assert(msg1 == msg2);

}

void testSerializeHeader()
{
    std::cout << "testSerializeHeader" << std::endl;

    UDPNCHeader_factory factory1;
    UDPNCHeader header1 = factory1.build ();

    header1.enc_packet_count = 1;
    header1.generation = 1;
    header1.gensize = 0;
    header1.ordering = 0xf2;
    header1.packetsize = 0xbeef;

    std::array<unsigned char, 8> buffer;
    memcpy(buffer.data(), &header1, sizeof(UDPNCHeader));
    printf("Sizeof UDPNCHeader %d", sizeof(UDPNCHeader));

    Logger::saveBinaryArray("header.txt", buffer.data(), 8);

}

int main()
{
    testPacketIds ();
    testFromId ();
    testToAddrs();
    testPutMessage();
    testOrdering ();
    testFactoryOrdering ();
    testSizeOfSerialize();
    testEquals();
    testQueueDequeue();
    testSerializeHeader();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;


    return 0;
}
