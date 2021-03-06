#ifndef UDPNC_PACKET_FACTORY_H
#define UDPNC_PACKET_FACTORY_H

#define START_BITS 0xD
#define END_BITS 0xE

#include <stdint.h>
#include <vector>
#include <iostream>

typedef size_t packetid_t;

// header used by all UDPNC messages
typedef struct
{
    uint32_t start      : 4;     // start marker (number 0xD: indicates 1101, used to differentiate from VASTHeader)
    uint32_t ordering   : 8;     // order of packet in stream
    uint32_t gensize    : 4;    //Showing how many packets are in this generation calculated in 2^gensize
    uint32_t generation : 8;
    uint32_t enc_packet_count : 8;

    uint32_t packetsize : 24;    // size of message (max: 65535 bytes - header bytes, same size as UDP payload)
    uint32_t padding : 4;
    uint32_t end        : 4;     // end marker (number 0xE: 1110)
} UDPNCHeader;

std::ostream& operator<<(std::ostream& output, UDPNCHeader const& header);

class UDPNCHeader_factory
{
public:
    UDPNCHeader_factory();

    UDPNCHeader_factory(char gensize, char generation, char ordering=0);

    UDPNCHeader build();

    static bool isUDPNCHeader(UDPNCHeader header);
    static size_t getUDPNCHeaderOffset(UDPNCHeader header);
    static bool isUDPNCHeadersEqual(const UDPNCHeader header1, const UDPNCHeader header2);


    void resetUDPNCHeader(UDPNCHeader &header);

private:
    UDPNCHeader header;
    int ordering = 0;
    int gensize = 0;
    int generation = 0;

};



#endif // UDPNC_PACKET_FACTORY_H
