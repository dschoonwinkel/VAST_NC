#include <iostream>
#include <assert.h>
#include "net_udpNC_handler.h"
#include "udpnc_packet_factory.h"
#include "udpncmessage.h"
#include "abstract_udpnc_msg_receiver_testimpl.h"
#include "absnet_udp_testimpl.h"
#include "VASTnet.h"
#include "VASTBuffer.h"
#include "string.h"
#include <cstring>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>
#include "net_udpnc_consumer.h"
#include "net_udpnc_mchandlermock.h"



using namespace boost::asio;

size_t sleep_time = 200;

namespace Vast
{
    class unittest_net_udpnc_consumer
    {
    public:
        friend class net_udpnc_consumer;

        void testPushPopWaitTime()
        {
            std::cout << "\n\nrunning testPushPopWaitTime" << std::endl;
            char data1[] = "message1";

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();

            UDPNCMessage message1(header1);

            int id1 = rand() % 10;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);

            std::cout << message1 << std::endl;

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl recv_tester;
            Vast::net_udpNC_MChandlerMock mock;
            consumer.open (&recv_tester, &absnet_udp, &mock);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

            std::chrono::microseconds popWaitTimer = std::chrono::microseconds::zero();

            size_t iterations = 10000;
            size_t prev_call_count = 0;

            for (size_t i = 0; i < iterations; i++)
            {
                message1.putOrdering(i);
                consumer.UDPNC_msg_received (message1, IPaddr());
                //Start the clock for processing time here
                auto t1 = std::chrono::high_resolution_clock::now();

                while (recv_tester.UDPNC_msg_received_call_count != prev_call_count + 1)
                {

                }

                auto t2 = std::chrono::high_resolution_clock::now();

                assert(recv_tester.recv_msg == message1);

                popWaitTimer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
                prev_call_count++;
            }


            consumer.close ();
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));


            CPPDEBUG("testPushPopWaitTime: popWaitTimer per iteration: " << popWaitTimer.count() /iterations << " microseconds " << std::endl);


        }


        void testUDPNC_msg_received()
        {
            std::cout << "\n\nrunning testUDPNC_msg_received" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            int id1 = rand() % 10;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);
            message2.putPacketId(id1);
            message2.putFromId (id1);
            message2.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            std::string remote_ip = "127.0.0.1";
            uint16_t remote_port = 1037;
            ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                        ip::address::from_string(remote_ip), remote_port);
            Vast::IPaddr local_addr(local_endpoint.address().to_v4().to_ulong(), local_endpoint.port());

            std::cout << message1 << std::endl;
            std::cout << message2 << std::endl;

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl recv_tester;
            consumer.open (&recv_tester, &absnet_udp, NULL);
            consumer.UDPNC_msg_received (message1, local_addr);
            consumer.UDPNC_msg_received (message2, local_addr);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

            consumer.close ();

            std::cout << message2 << std::endl;
            std::cout << recv_tester.recv_msg << std::endl;
//            std::cout << (size_t)&local_endpoint << std::endl;
            std::cout << recv_tester.recv_msg.socket_addr << std::endl;

            //Add the socket_addr to message2 - it was added in UDPNC_msg_received
            message2.socket_addr = local_addr;
            assert(recv_tester.recv_msg == message2);
            assert(recv_tester.recv_msg.socket_addr == local_addr);

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
        }

        void testFromNETUNASSIGNED()
        {
            std::cout << "\n\nrunning testFromNETUNASSIGNED" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            int id1 = NET_ID_UNASSIGNED;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);
            message2.putPacketId(id1);
            message2.putFromId (id1);
            message2.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            message1.putOrdering (0);
            message2.putOrdering (0);

            std::cout << message1 << std::endl;
            std::cout << message2 << std::endl;

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            consumer.open (&tester, &absnet_udp, NULL);
            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());
            consumer.close ();


            std::cout << tester.recv_msg << std::endl;
            //Returned message is the entire UDPNC payload, i.e. VAST message - contained in buf2
            assert(tester.recv_msg == message2);


            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
        }

        void testOrderingNormalChain()
        {
            std::cout << "\n\nrunning testOrderingNormalChain" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            std::cout << message1 << std::endl;
            std::cout << message2 << std::endl;

            int id1 = rand() % 10;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);
            message2.putPacketId(id1);
            message2.putFromId (id1);
            message2.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            consumer.open (&tester, &absnet_udp, NULL);
            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());
            consumer.close ();

            std::cout << tester.recv_msg << std::endl;
            assert(tester.recv_msg == message2);

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
        }

        void testOrderingWrongChain()
        {
            std::cout << "\n\nrunning testOrderingWrongChain" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            int id1 = rand() % 10;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);

            message2.putPacketId(id1);
            message2.putFromId (id1);
            message2.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            consumer.open (&tester, &absnet_udp, NULL);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());

            std::cout << tester.recv_msg << std::endl;
            assert(tester.recv_msg == message2);

            //Try to process the same packet again - changing the message so that we can distinguish it
            message2.putMessage (buf1.data, buf1.size);

            consumer.process_input(message2, Vast::IPaddr());

            //The latest received packet should still be the same as previously, i.e. changed message 2 should not
            // be processed, i.e. with the original message: buf2
            message2.putMessage (buf2.data, buf2.size);
            assert(tester.recv_msg == message2);

            consumer.close();

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

        }

        void testOrderingIndividualChains()
        {
            std::cout << "\n\nrunning testOrderingIndividualChains" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            int id1 = rand() % 10;
            int id2 = -1;
            do
            {
               id2  = rand() % 10;
            }
            while(id2 == id1);

            Vast::IPaddr addr1("127.0.0.1", 1037);
            Vast::IPaddr addr2("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);
            message2.putPacketId(id2);
            message2.putFromId (id2);
            message2.putToAddr (addr2);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            consumer.open (&tester, &absnet_udp, NULL);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
            consumer.process_input (message1, Vast::IPaddr());
            //Check if we have received the first message from the first ID
            //Returned message is the entire UDPNC payload, i.e. VAST message - contained in buf2
            assert(tester.recv_msg == message1);

            consumer.process_input (message2, Vast::IPaddr());
            //Check if we have received the second message from the second ID
            assert(tester.recv_msg == message2);

            consumer.close ();

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
        }

        void testOrderingEndOfChain()
        {
            std::cout << "\n\nrunning testOrderingEndOfChain" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            message1.putOrdering (255);
            //Ordering 256 == 0 because of overflow
            message2.putOrdering (256);

            int id1 = rand() % 10;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);
            message2.putPacketId(id1);
            message2.putFromId (id1);
            message2.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            std::cout << message1 << std::endl;
            std::cout << message2 << std::endl;

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            Vast::net_udpNC_MChandlerMock mock;
            consumer.open (&tester, &absnet_udp, &mock);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());

            consumer.close();

            std::cout << tester.recv_msg << std::endl;
            //Returned message is the entire UDPNC payload, i.e. VAST message - contained in buf2
            assert(tester.recv_msg == message2);

            assert(mock.clearPacketPoolCalled == 1);

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

        }

        void testOrderingLossyEndOfChain()
        {
            std::cout << "\n\nrunning testOrderingLossyEndOfChain" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            //Simulate loss of packets -> last highest packet and first low packet
            message1.putOrdering (246);
            message2.putOrdering (9);

            std::cout << message1 << std::endl;
            std::cout << message2 << std::endl;

            int id1 = rand() % 10;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);
            message2.putPacketId(id1);
            message2.putFromId (id1);
            message2.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::net_udpNC_MChandlerMock mock;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            consumer.open (&tester, &absnet_udp, &mock);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());

            std::cout << tester.recv_msg << std::endl;
            //Returned message is the entire UDPNC payload, i.e. VAST message - contained in buf2
            assert(tester.recv_msg == message2);

            assert(mock.clearPacketPoolCalled == 1);

            consumer.close();

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
        }

        void testOrderingVeryLossyEndOfChain()
        {
            std::cout << "\n\nrunning testOrderingVeryLossyEndOfChain" << std::endl;
            char data1[] = "message1";
            char data2[] = "message2";
            data2[0] = rand() % 25 + 'a';

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            Vast::VASTHeader vast_header2;
            vast_header2.start = 10;
            vast_header2.type = 0;
            vast_header2.end = 5;
            Vast::VASTBuffer buf2;
            Vast::Message msg2(0);
            msg2.store(data2,  strlen(data2));
            vast_header2.msg_size = msg2.serialize (NULL);
            buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
            buf2.add (&msg2);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            //Simulate loss of packets -> last highest packet and first low packet
            message1.putOrdering (LOWEST_RESET_PACKET_ORDERING_NUMBER-1);
            message2.putOrdering (9);

            int id1 = rand() % 10;

            Vast::IPaddr addr1("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);
            message2.putPacketId(id1);
            message2.putFromId (id1);
            message2.putToAddr (addr1);

            message1.putMessage(buf1.data, buf1.size);
            message2.putMessage(buf2.data, buf2.size);

            std::cout << message1 << std::endl;
            std::cout << message2 << std::endl;

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            Vast::net_udpNC_MChandlerMock mock;
            consumer.open (&tester, &absnet_udp, &mock);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

            //Simulate loss of packets -> last highest packet and first low packet
            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());

            std::cout << tester.recv_msg << std::endl;
            //Do not accept packets out of reset bounds
            assert(tester.recv_msg == message1);

            //Simulate loss of packets -> last highest packet and first low packet
            message1.putOrdering (254);
            message2.putOrdering (HIGHEST_RESET_ACCEPTING_ORDERING_NUMBER + 1);

            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());

            std::cout << tester.recv_msg << std::endl;
            //Do not accept packets out of reset bounds
            assert(tester.recv_msg == message1);

            assert(mock.clearPacketPoolCalled == 0);

            consumer.close();

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

        }

        void testHandleNoToAddress ()
        {
            std::cout << "\n\nrunning testHandleNoToAddress" << std::endl;

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();
            UDPNCHeader header2 = factory.build();

            UDPNCMessage message1(header1);
            UDPNCMessage message2(header2);

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            consumer.open (&tester, &absnet_udp, NULL);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

            consumer.process_input (message1, Vast::IPaddr());
            consumer.process_input (message2, Vast::IPaddr());

            std::cout << tester.recv_msg << std::endl;
            //Do not accept packets without a toAddr
            assert(tester.UDPNC_msg_received_call_count == 0);

            consumer.close();

            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
        }

        void testThrowEncodedPacketException()
        {
            std::cout << "\n\nrunning testThrowEncodedPacketException" << std::endl;
            char data1[] = "message1";

            Vast::VASTHeader vast_header1;
            vast_header1.start = 10;
            vast_header1.type = 0;
            vast_header1.end = 5;

            Vast::VASTBuffer buf1;
            Vast::Message msg1(0);
            msg1.store(data1,  strlen(data1));
            vast_header1.msg_size = msg1.serialize (NULL);
            buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
            buf1.add (&msg1);

            UDPNCHeader_factory factory;

            UDPNCHeader header1 = factory.build();

            UDPNCMessage message1(header1);

            int id1 = rand() % 10;
            int id2 = -1;
            do
            {
               id2  = rand() % 10;
            }
            while(id2 == id1);

            Vast::IPaddr addr1("127.0.0.1", 1037);
            Vast::IPaddr addr2("127.0.0.1", 1037);

            message1.putPacketId(id1);
            message1.putFromId (id1);
            message1.putToAddr (addr1);

            message1.putPacketId(id2);
            message1.putFromId (id2);
            message1.putToAddr (addr2);

            message1.putMessage(buf1.data, buf1.size);

            Vast::net_udpNC_consumer consumer;
            Vast::absnet_udp_testimpl absnet_udp;
            Vast::AbstractUDPNCMsgReceiverTestImpl tester;
            consumer.open (&tester, &absnet_udp, NULL);
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

            try {

                consumer.process_input (message1, Vast::IPaddr());
                std::cout << "Test should have thrown an exception already before this" << std::endl;
                std::abort();
            }
            catch (std::logic_error)
            {
                std::cout << "Test success" << std::endl;
            }
            consumer.close();
            boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
        }
    };
}

int main()
{
    Vast::unittest_net_udpnc_consumer testing_consumer;
    testing_consumer.testUDPNC_msg_received();
    testing_consumer.testFromNETUNASSIGNED ();
    testing_consumer.testOrderingNormalChain ();
    testing_consumer.testOrderingWrongChain ();
    testing_consumer.testOrderingIndividualChains();
    testing_consumer.testOrderingEndOfChain ();
    testing_consumer.testOrderingLossyEndOfChain ();
    testing_consumer.testOrderingVeryLossyEndOfChain ();
    testing_consumer.testHandleNoToAddress ();
    testing_consumer.testThrowEncodedPacketException();
    testing_consumer.testPushPopWaitTime ();






    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;

    return 0;
}
