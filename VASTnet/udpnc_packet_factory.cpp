#include "udpnc_packet_factory.h"
#include "VASTTypes.h"

UDPNCHeader_factory::UDPNCHeader_factory()
{
    resetUDPNCHeader(header);
}

UDPNCHeader_factory::UDPNCHeader_factory(char gensize, char generation, char ordering)
{
    resetUDPNCHeader(header);

    header.gensize = gensize;
    header.generation = generation;
    header.ordering = ordering;
}

UDPNCHeader UDPNCHeader_factory::build()
{
    header.start = START_BITS;
    header.end = END_BITS;
    resetUDPNCHeader(header);

    //Every new UDPNC header should have its own position in the stream
    ordering++;
    header.ordering = ordering;

    return header;
}

bool UDPNCHeader_factory::isUDPNCHeader(UDPNCHeader header)
{
    if (header.start == START_BITS && header.end == END_BITS)
        return true;
    else
        return false;
}

void UDPNCHeader_factory::resetUDPNCHeader(UDPNCHeader &header)
{
    header.enc_packet_count = 0;
    header.generation = generation;
    header.gensize = gensize;
    header.ordering = ordering;
    header.packetsize = 0;
}

size_t UDPNCHeader_factory::getUDPNCHeaderOffset(UDPNCHeader header)
{
    size_t offset = 0;
    offset += sizeof(UDPNCHeader);
    offset += header.enc_packet_count * sizeof(packetid_t);
    offset += header.enc_packet_count * sizeof(Vast::id_t);
//    CPPDEBUG("UDPNCHeader_factory::getUDPNCHeaderOffset Offset: " << offset << std::endl);
    return offset;
}

bool UDPNCHeader_factory::isUDPNCHeadersEqual(const UDPNCHeader header1, const UDPNCHeader header2)
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

std::ostream& operator<<(std::ostream& output, UDPNCHeader const& header )
{

        output << "UDPNCHeader::stream >> output: ******************************\n";
        output << "Header: " << std::endl;
        output << "header.ordering " << header.ordering<< std::endl;
        output << "header.packetsize " << header.packetsize << std::endl;
        output << "header.gensize " << header.gensize << std::endl;
        output << "header.generation " << header.generation << std::endl;
        output << "header.enc_pack_count " << header.enc_packet_count << std::endl;
        return output;
}
