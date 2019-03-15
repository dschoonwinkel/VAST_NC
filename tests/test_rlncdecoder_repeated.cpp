#include "rlncmessage.h"
#include "rlncrecoder.h"
#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include "VASTnet.h"
#include <array>
#include <time.h>
#include <stdlib.h>
#include "net_udpnc_mchandler.h"

int main() {

    srand(time(NULL));

    Vast::net_udpNC_MChandler mchandler;
    mchandler.open(NULL);

    for (int i = 0; i < 1000000; i++)
    {

        char data1[] = {'d', 'e', 'a', 'd'};
        char data2[] = {'b', 'e', 'e', 'f'};
        data2[0] = rand() % 25 + 'a';

        std::array<char, 100> vast_data1;

        Vast::VASTHeader vast_header1;
        vast_header1.start = 10;
        vast_header1.type = 0;
        vast_header1.end = 5;
        vast_header1.msg_size = 4;
        std::fill(vast_data1.begin (), vast_data1.end (), 0);
        memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
        memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

        std::array<char, 100> vast_data2;

        Vast::VASTHeader vast_header2;
        vast_header2.start = 10;
        vast_header2.type = 0;
        vast_header2.end = 5;
        vast_header2.msg_size = 4;
        std::fill(vast_data2.begin (), vast_data2.end (), 0);
        memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
        memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);


        RLNCrecoder recoder;

        RLNCHeader_factory factory;
        RLNCHeader header1 = factory.build();
        RLNCHeader header2 = factory.build();

        RLNCMessage message1(header1);
        RLNCMessage message2(header2);

        int id1 = rand() % 10;
        int id2 = -1;
        do
        {
           id2  = rand() % 10;
        }
        while(id2 == id1);

        message1.putPacketId(id1);
        message1.putFromId (id1);
        message2.putPacketId(id2);
        message2.putFromId (id2);

        message1.putMessage(vast_data1.data (), 100);
        message2.putMessage(vast_data2.data (), 100);

        std::cout << "Symbol 1:" << message1 << std::endl;
        std::cout << "Symbol 2:" << message2 << std::endl;

        recoder.addRLNCMessage(message1);
        recoder.addRLNCMessage(message2);

        RLNCMessage *temp_msg = recoder.produceRLNCMessage();

        if (!temp_msg)
        {
            std::cerr << "Could not produce RLNC encoded message" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::array<uint8_t, 1400> payload;
        payload.fill (0);
        message1.serialize(reinterpret_cast<char*>(payload.data()));
        //Equivalent to receive
        mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

        temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
        payload.fill (0);
        //Equivalent to receive
        mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());
    }

}