#include "udpnc_packet_factory.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include "VASTTypes.h"
#include "VASTnet.h"
#include "VAST.h"

using namespace std;


int main (int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "Usage: ./udpncdissector.cpp <filename>" << endl;
        exit(1);
    }

    streampos size;
    char * memblock;

    string packetfilename(argv[1]);

    ifstream ifs(packetfilename, ios::in | ios::binary | ios::ate);

    if (ifs.is_open())
    {
        size = ifs.tellg();
        memblock = new char [size];
        ifs.seekg (0, ios::beg);
        ifs.read(memblock, size);
        ifs.close();

        cout << "File is in memory" << endl;
    }
    else
    {
        cout << "Something went wrong with opening the file" << endl;
        exit(EXIT_FAILURE);
    }

    size_t offset = 0;

    UDPNCHeader header;
    memcpy(&header, memblock + offset, sizeof(UDPNCHeader));
    offset += sizeof(UDPNCHeader);

    cout << header << endl;

    uint32_t checksum = 0;
    memcpy(&checksum, memblock + offset, sizeof(uint32_t));
    std::cout << "checksum:" << checksum << std::endl;
    offset += sizeof(uint32_t);

    for (int i = 0; i < header.enc_packet_count; i++)
    {
        packetid_t tempid = 0;
        memcpy(&tempid, memblock+offset, sizeof(packetid_t));
        std::cout << "packetid[" << i << "]:" << tempid << std::endl;
        offset += sizeof(packetid_t);

        Vast::id_t temp_fromID = 0;
        memcpy(&temp_fromID, memblock+offset, sizeof(Vast::id_t));
        std::cout << "fromID[" << i << "]:" << temp_fromID << std::endl;
        offset += sizeof(Vast::id_t);

        Vast::IPaddr temp_addr;
        temp_addr.deserialize (memblock+offset, ((size_t)size) - offset);
        std::cout << "to_addr[" << i << "]:" << temp_addr << std::endl;
        offset += temp_addr.sizeOf ();
    }

    Vast::VASTHeader vast_header;
    memcpy(&vast_header, memblock+offset, sizeof(Vast::VASTHeader));
    std::cout << "\nVAST packet: \n" <<
                 Logger::printArray(memblock+offset, vast_header.msg_size) << std::endl;

    offset += sizeof(Vast::VASTHeader);
    std::cout << std::endl << "VASTHeader:" << std::endl;
    std::cout << vast_header << std::endl;

    Vast::Message msg1(0);
    msg1.deserialize(memblock+offset, ((size_t)size) - offset);
    offset += msg1.sizeOf() - msg1.size;
    std::cout << msg1 << std::endl;

    if (msg1.msgtype == Vast::MOVE)
    {
        Vast::id_t temp_subID = 0;
        memcpy(&temp_subID, memblock+offset, sizeof(Vast::id_t));
        std::cout << "subID: " << temp_subID << std::endl;
        offset += sizeof(Vast::id_t);

        Vast::timestamp_t timestamp = 0;
        memcpy(&timestamp, memblock+offset, sizeof(Vast::timestamp_t));
        std::cout << "timestamp: " << timestamp << std::endl;
        offset += sizeof(Vast::timestamp_t);

        Vast::VONPosition pos;
        std::cout << "sizeof(VONPosition)=" << sizeof(Vast::VONPosition) << std::endl;
        memcpy(&pos, memblock+offset, sizeof(Vast::VONPosition));
        std::cout << "pos.x: " << pos.x << std::endl;
        std::cout << "pos.y: " << pos.y << std::endl;
        offset += sizeof(Vast::VONPosition);
    }

    std::cout << "Bytes left: " << ((size_t)size) - offset << std::endl;
    std::cout << "memblock size: " << ((size_t)size) << "offset: " << offset << std::endl;

    delete[] memblock;
    return 0;
}
