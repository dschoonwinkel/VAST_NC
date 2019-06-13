#ifndef RLNC_PACKET_FACTORY_H
#define RLNC_PACKET_FACTORY_H

#define START_BITS 0xD
#define END_BITS 0xE

#include <stdint.h>
#include <vector>
#include <iostream>

typedef size_t packetid_t;

// header used by all RLNC messages
typedef struct
{
    uint32_t start      : 4;     // start marker (number 0xD: indicates 1101, used to differentiate from VASTHeader)
    uint32_t ordering   : 8;     // order of packet in stream
    uint32_t packetsize : 16;    // size of message (max: 65535 bytes - header bytes, same size as UDP payload)
    uint32_t gensize    : 4;    //Showing how many packets are in this generation calculated in 2^gensize

    uint32_t generation : 8;
    uint32_t enc_packet_count : 8;
    uint32_t padding : 12;
    uint32_t end        : 4;     // end marker (number 0xE: 1110)
} RLNCHeader;

std::ostream& operator<<(std::ostream& output, RLNCHeader const& header);

class RLNCHeader_factory
{
public:
    RLNCHeader_factory();

    RLNCHeader_factory(char gensize, char generation, char ordering=0);

    RLNCHeader build();

    static bool isRLNCHeader(RLNCHeader header);
    static size_t getRLNCHeaderOffset(RLNCHeader header);
    static bool isRLNCHeadersEqual(const RLNCHeader header1, const RLNCHeader header2);


    void resetRLNCHeader(RLNCHeader &header);

private:
    RLNCHeader header;
    int ordering = 0;
    int gensize = 0;
    int generation = 0;

};



#endif // RLNC_PACKET_FACTORY_H
