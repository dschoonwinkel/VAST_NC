#include "rlnc_packet_factory.h"
#include "VASTTypes.h"

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

size_t RLNCHeader_factory::getRLNCHeaderOffset(RLNCHeader header)
{
    size_t offset = 0;
    offset += sizeof(RLNCHeader);
    offset += header.enc_packet_count * sizeof(packetid_t);
    offset += header.enc_packet_count * sizeof(Vast::id_t);
//    CPPDEBUG("RLNCHeader_factory::getRLNCHeaderOffset Offset: " << offset << std::endl);
    return offset;
}

bool RLNCHeader_factory::isRLNCHeadersEqual(const RLNCHeader header1, const RLNCHeader header2)
{
    bool equals = true;
    equals = equals && (header1.start == header2.start);
    equals = equals && (header1.ordering == header2.ordering);
    equals = equals && (header1.packetsize == header2.packetsize);
    equals = equals && (header1.gensize == header2.gensize);
    equals = equals && (header1.generation == header2.generation);
    equals = equals && (header1.enc_packet_count == header2.enc_packet_count);
    equals = equals && (header1.padding == header2.padding);
    equals = equals && (header1.end == header2.end);

    return equals;
}
