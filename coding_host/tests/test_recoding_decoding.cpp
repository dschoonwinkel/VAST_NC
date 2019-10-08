#include "udpnc_decoder.h"
#include "udpncrecoder.h"
#include "udpnc_packet_factory.h"

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

    UDPNCrecoder recoder;
    udpNC_decoder decoder;

    UDPNCHeader_factory factory;
    UDPNCHeader header1 = factory.build();
    UDPNCHeader header2 = factory.build();

    UDPNCMessage message1(header1);
    UDPNCMessage message2(header2);


    message1.putPacketId(123);
    message1.putFromId (123);
    message2.putPacketId(654);
    message2.putFromId (654);

    message1.putMessage(data1, 4);
    message2.putMessage(data2, 4);

    std::cout << "Symbol 1:" << message1 << std::endl;
    std::cout << "Symbol 2:" << message2 << std::endl;

    recoder.addUDPNCMessage(message1);
    recoder.addUDPNCMessage(message2);

    std::shared_ptr<UDPNCMessage> temp_msg = recoder.produceUDPNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce coded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Coded Symbol:" << (*temp_msg) << std::endl;

    decoder.addUDPNCMessage(*temp_msg);

    decoder.addUDPNCMessage(message1);
    std::shared_ptr<UDPNCMessage> decoded_msg = decoder.produceDecodedUDPNCMessage();

    if (decoded_msg != NULL)
    {
        std::cout << (*decoded_msg) << std::endl;
    }

    return 0;

}
