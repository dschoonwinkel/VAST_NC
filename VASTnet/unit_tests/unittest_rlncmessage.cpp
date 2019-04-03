#include <iostream>
#include <rlncmessage.h>
#include <assert.h>
#include <string.h>

void runUnitTest1()
{
    std::cout << "Not implemented yet!" << std::endl;
    std::abort();
}

void testPacketIds()
{
    std::cout << "testPacketIds" << std::endl;
    RLNCMessage msg1;
    packetid_t id1 = 1234567890;

    msg1.putPacketId (id1);

    assert(msg1.getPacketIds ()[0] == id1);
}

void testFromId()
{
    std::cout << "testFromId" << std::endl;
    RLNCMessage msg1;
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
    RLNCMessage msg1;
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

    RLNCMessage msg1;

    msg1.putMessage (test1.c_str (), test1.length ());

    assert(strcmp(msg1.getMessage (), test1.c_str ()) == 0);
    assert(memcmp(msg1.getMessageU (), test1.c_str (), test1.length ()) == 0);

    assert(strlen(msg1.getMessage ()) == test1.length ());
}

void testOrdering()
{
    std::cout << "testOrdering" << std::endl;
    RLNCMessage msg1;

    msg1.putOrdering (123);

    assert(msg1.getOrdering () == 123);
}

void testFactoryOrdering()
{
    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build ();
    RLNCHeader header2 = factory.build();

    assert(header1.ordering == 1);
    assert(header2.ordering == 2);
}

void testSizeOfSerialize()
{
    std::cout << "testSizeOfSerialize" << std::endl;

    std::string test1 = "Hello World!123";

    RLNCHeader_factory factory1;
    RLNCHeader header1 = factory1.build ();

    header1.enc_packet_count = 1;
    header1.generation = 1;
    header1.gensize = 0;
    header1.ordering = 3;

    RLNCMessage msg1(header1);
    packetid_t id1 = 1234567890;
    Vast::id_t from_id = 13132323123;
    Vast::IPaddr addr1("127.0.0.1", 1037);

    msg1.putPacketId (id1);
    msg1.putFromId (from_id);
    msg1.putToAddr (addr1);

    msg1.putMessage (test1.c_str (), test1.length ());

    assert(msg1.sizeOf () == (sizeof(RLNCHeader)
                              + sizeof(packetid_t) * 1 + sizeof(Vast::id_t) * 1
                              + sizeof(char) * test1.length ()
                              + sizeof(uint32_t) + 2 * sizeof(uint16_t)
                              )
           );

    char buffer[msg1.sizeOf ()];

    msg1.serialize (buffer);

    RLNCMessage msg2;

    msg2.deserialize (buffer, msg1.sizeOf ());

    assert(msg1 == msg2);
}

void testEquals()
{
    std::cout << "testEquals" << std::endl;

    std::string test1 = "Hello World!123";

    RLNCHeader_factory factory1;
    RLNCHeader header1 = factory1.build ();

    header1.enc_packet_count = 1;
    header1.generation = 1;
    header1.gensize = 0;
    header1.ordering = 3;

    RLNCMessage msg1(header1);
    packetid_t id1 = 1234567890;
    Vast::id_t from_id = 13132323123;
    Vast::IPaddr addr1("127.0.0.1", 1037);

    msg1.putPacketId (id1);
    msg1.putFromId (from_id);
    msg1.putToAddr (addr1);

    msg1.putMessage (test1.c_str (), test1.length ());

    RLNCMessage msg2(header1);

    msg2.putPacketId (id1);
    msg2.putFromId (from_id);
    msg2.putToAddr (addr1);

    msg2.putMessage (test1.c_str (), test1.length ());

    assert(msg1 == msg2);

    Vast::id_t from_id2 = 1345982345;
    msg2.putPacketId (id1);
    msg2.putFromId (from_id2);
    msg2.putToAddr (addr1);

    assert(!(msg1 == msg2));

}

int main()
{
//    runUnitTest1();
    testPacketIds ();
    testFromId ();
    testToAddrs();
    testPutMessage();
    testOrdering ();
    testFactoryOrdering ();
    testSizeOfSerialize();
    testEquals();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;


    return 0;
}
