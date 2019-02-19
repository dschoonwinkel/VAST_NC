#include "rlnc_packet_factory.h"

RLNCHeader_factory::RLNCHeader_factory()
{
    resetRLNCHeader(header);
}

RLNCHeader_factory::RLNCHeader_factory(char gensize, char generation, char ordering)
{
    resetRLNCHeader(header);

    header.gensize = gensize;
    header.generation = generation;
    header.ordering = ordering;
}

RLNCHeader RLNCHeader_factory::build()
{
    header.start = START_BITS;
    header.end = END_BITS;
    resetRLNCHeader(header);

    //Every new RLNC header should have its own position in the stream
    ordering++;
    header.ordering = ordering;

    return header;
}

bool RLNCHeader_factory::isRLNCHeader(RLNCHeader header)
{
    if (header.start == START_BITS && header.end == END_BITS)
        return true;
    else
        return false;
}

void RLNCHeader_factory::resetRLNCHeader(RLNCHeader &header)
{
    header.enc_packet_count = 0;
    header.generation = generation;
    header.gensize = gensize;
    header.ordering = ordering;
    header.packetsize = 0;
}
