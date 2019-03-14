#include "rlncdecoder.h"
#include "rlncrecoder.h"
#include "rlnc_packet_factory.h"

#include <boost/asio.hpp>
#include <kodo_rlnc/coders.hpp>

using namespace boost::asio;
using rlnc_encoder = kodo_rlnc::encoder;


int main()
{
    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};

    std::array<uint8_t, MAX_PACKET_SIZE*2> buffer;
    buffer.fill(0);

    RLNCrecoder recoder;
    rlncdecoder decoder;

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);


    message1.putPacketId(123);
    message1.putFromId (123);
    message2.putPacketId(654);
    message2.putFromId (654);

    message1.putMessage(data1, 4);
    message2.putMessage(data2, 4);

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
        std::cout << (*decoded_msg) << std::endl;
    }

    return 0;

}
