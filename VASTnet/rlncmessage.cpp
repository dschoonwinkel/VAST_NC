#include "rlncmessage.h"
#include <algorithm>
#include <boost/format.hpp>

RLNCMessage::RLNCMessage()
{

}

RLNCMessage::RLNCMessage (RLNCHeader header)
{
    this->header = header;
    header.enc_packet_count = 0;
}

RLNCMessage::RLNCMessage (const RLNCMessage &message)
{
    this->header = message.header;
    this->msg = new char[message.header.packetsize];
    memcpy(this->msg, message.msg, message.header.packetsize);
    this->pkt_ids = message.pkt_ids;
    this->from_ids = message.from_ids;
}

const std::vector<packetid_t> RLNCMessage::getPacketIds()
{
    return pkt_ids;
}

const std::vector<Vast::id_t> RLNCMessage::getFromIds()
{
    return from_ids;
}

Vast::id_t RLNCMessage::getFirstFromId()
{
    return from_ids.front ();
}

void RLNCMessage::putPacketId(packetid_t pkt_id, bool autonumber)
{
    pkt_ids.push_back(pkt_id);
    if (autonumber)
        header.enc_packet_count = pkt_ids.size();
}

void RLNCMessage::putFromId (Vast::id_t from_id)
{
    from_ids.push_back(from_id);
}

void RLNCMessage::putMessage(const char* buffer, size_t len)
{
    msg = new char[len];
    memcpy(msg, buffer, len);
    header.packetsize = len;
}

char* RLNCMessage::getMessage(size_t len)
{
    //If I just want to read it, return the pointer
    if (len == 0)
    {
        return msg;
    }
    //If I want to write to it, assure that there is enough space
    msg = new char[len];
    header.packetsize = len;
    return msg;
}

uint8_t* RLNCMessage::getMessageU (size_t len)
{
    return reinterpret_cast<uint8_t*>(getMessage(len));
}

size_t RLNCMessage::getMessageSize()
{
    return header.packetsize;
}

void RLNCMessage::putOrdering(uint8_t ordering)
{
    header.ordering = ordering;
}

uint8_t RLNCMessage::getOrdering()
{
    return header.ordering;
}

// size of this class, must be implemented
size_t RLNCMessage::sizeOf ()
{
    size_t size = sizeof(RLNCHeader);
    size += header.enc_packet_count * sizeof(packetid_t);
    size += header.enc_packet_count * sizeof(Vast::id_t);
    size += header.packetsize;

    return size;
}

// store into a buffer (assuming the buffer has enough space)
// buffer can be NULL (simply to query the total size)
// returns the total size of the packed class
size_t RLNCMessage::serialize (char *buffer)
{
    size_t n = 0;
    if (buffer == NULL)
        return sizeOf();

    memcpy(buffer+n, &header, sizeof(RLNCHeader));
    n += sizeof(RLNCHeader);

    for (int i = 0; i < header.enc_packet_count; i++)
    {
        memcpy(buffer+n, &pkt_ids[i], sizeof(packetid_t));
        n += sizeof(packetid_t);
        memcpy(buffer+n, &from_ids[i], sizeof(Vast::id_t));
        n += sizeof(Vast::id_t);
    }
    memcpy(buffer+n, msg, header.packetsize);
    n += header.packetsize;

    return n;

}

// restores a buffer into the object
// returns number of bytes restored (should be > 0 if correct)
int RLNCMessage::deserialize (const char *buffer, size_t size)
{
    size_t n = 0;
    if (size > sizeof(RLNCHeader) && buffer != NULL)
    {
        memcpy(&header, buffer+n, sizeof(RLNCHeader));
        n += sizeof(RLNCHeader);

        if (!RLNCHeader_factory::isRLNCHeader (header))
        {
            CPPDEBUG("RLNCMessage::deserialize Not a RLNC Header" << std::endl);

            std::cout << "Sample of first few bytes in buffer (max =" << size << std::endl;
            for (size_t i = 0; i < 10; i++)
                printf("%x.", buffer[i] & 0xff);

            std::cout << std::endl;


            return -1;
        }

        packetid_t pkt_id = -1;
        Vast::id_t from_id = -1;
        for (int i = 0; i < header.enc_packet_count; i++)
        {
            memcpy(&pkt_id, buffer+n, sizeof(packetid_t));
            putPacketId(pkt_id, false);
            n += sizeof(packetid_t);
            memcpy(&from_id, buffer+n, sizeof(Vast::id_t));
            putFromId (from_id);
            n += sizeof(Vast::id_t);
        }
        if (header.packetsize > size)
        {
            std::cerr << "RLNCMessage::deserialize Header packet size larger than available bytes" << std::endl;
            std::cerr << "header.start " << header.start << std::endl;
            std::cerr << "header.end: " << header.end << std::endl;
            msg = new char[size - n];
            memcpy(msg, buffer+n, size-n);
            std::cerr << "RLNCMessage::deserialize" << *(this) << std::endl;
            return -1;
        }


        msg = new char[header.packetsize];
        memcpy(msg, buffer+n, header.packetsize);

        return n;
    }
    else
    {
        return 0;
    }

    return 0;
}

bool RLNCMessage::operator==(const RLNCMessage other)
{
    bool equals = true;

    equals = equals && (RLNCHeader_factory::isRLNCHeadersEqual (other.header, this->header));
    equals = equals && (other.pkt_ids == this->pkt_ids);
    equals = equals && (other.from_ids == this->from_ids);

    if (other.header.packetsize != this->header.packetsize)
        return false;

    for (size_t i = 0; i < this->header.packetsize; i++)
    {
        equals = equals && (other.msg[i] == this->msg[i]);
    }

    return equals;
}

std::ostream& operator<<(std::ostream& output, RLNCMessage const& message )
{

        output << "RLNCMessage::stream >> output: ******************************\n";
        output << "Header: " << std::endl;
        output << "header.ordering " << message.header.ordering<< std::endl;
        output << "header.packetsize " << message.header.packetsize << std::endl;
        output << "header.gensize " << message.header.gensize << std::endl;
        output << "header.generation " << message.header.generation << std::endl;
        output << "header.enc_pack_count " << message.header.enc_packet_count << std::endl;
        for (size_t i = 0; i < message.pkt_ids.size(); i++)
        {
            output << "Pkt_ids[" << i << "]" << message.pkt_ids[i] << std::endl;
            output << "From id[" << i << "]" << message.from_ids[i] << std::endl;
        }
        output << "msg: " << std::endl;
        boost::format format("%1$#x");
        for (int i = 0; i < std::min<unsigned short>(message.header.packetsize, 10); i++)
        {
            output << format % message.msg[i];
        }
        output << std::endl;

        output << "******************************************************" << std::endl;
        return output;
}

packetid_t RLNCMessage::generatePacketId(Vast::id_t id, int ordering)
{
    packetid_t x = id;
    x += 0x9e3779b97f4a7c15;
    x ^= (x >> 30);
    x *= 0xbf58476d1ce4e5b9;
    x ^= (x >> 27);
    x *= 0x94d049bb133111eb;
    x ^= (x >> 31);
    x += ordering;
    return x;
}
