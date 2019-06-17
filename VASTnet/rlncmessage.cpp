#include "rlncmessage.h"
#include <algorithm>
#include <boost/format.hpp>
#include <exception>
#include <stdio.h>
#include <boost/crc.hpp>
#include "logger.h"
#include <string>

RLNCMessage::RLNCMessage()
{

}

RLNCMessage::RLNCMessage (RLNCHeader header)
{
    this->header = header;
    header.enc_packet_count = 0;
}

RLNCMessage::RLNCMessage (const RLNCMessage &message):
    socket_addr(message.socket_addr.host, message.socket_addr.port)
{
    this->header = message.header;
    this->msg = message.msg;
    this->pkt_ids = message.pkt_ids;
    this->from_ids = message.from_ids;
    this->to_addrs = message.to_addrs;
    this->socket_addr = socket_addr;
    this->checksum = message.checksum;

//    CPPDEBUG("RLNCMessage copy constructor called" <<std::endl);
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

const std::vector<Vast::IPaddr> RLNCMessage::getToAddrs()
{
    return to_addrs;
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

void RLNCMessage::putToAddr(Vast::IPaddr addr)
{
    to_addrs.push_back (addr);
}

void RLNCMessage::putIdsAddr(packetid_t pkt_id,
                             Vast::id_t from_id,
                             Vast::IPaddr addr, bool autonumber)
{
    putPacketId(pkt_id, autonumber);
    putFromId(from_id);
    putToAddr(addr);
}

void RLNCMessage::putMessage(const char* buffer, size_t len)
{
    msg.assign (buffer, len);
    header.packetsize = len;
}

const char *RLNCMessage::getMessage()
{
        return msg.c_str ();
}

const uint8_t *RLNCMessage::getMessageU()
{
    return reinterpret_cast<const uint8_t*>(getMessage());
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

uint32_t RLNCMessage::getChecksum()
{
    return checksum;
}

void RLNCMessage::setChecksum(uint32_t checksum)
{
    this->checksum = checksum;
}

// size of this class, must be implemented
size_t RLNCMessage::sizeOf ()
{
    size_t size = sizeof(RLNCHeader);
    size += header.enc_packet_count * sizeof(packetid_t);
    size += header.enc_packet_count * sizeof(Vast::id_t);
    //Size of internals of the IPaddr class = 4 bytes IP + 2 * 2 bytes for port and padding
    size += header.enc_packet_count * sizeof (uint32_t) + sizeof (uint16_t) * 2;
    size += header.packetsize;
    //Size of checksum
    size += sizeof(uint32_t);

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

    //Check if pkt_ids, from_ids and to_addrs have same length
    if (pkt_ids.size () == from_ids.size () && from_ids.size() == to_addrs.size ())
    {
        //All is well
    }
    else
    {
        char errmsg[100];
        sprintf(errmsg, "pkt_ids [%lu], from_ids [%lu] and to_addrs [%lu] is not the same size",
                pkt_ids.size(),
                from_ids.size (),
                to_addrs.size ());
        throw std::logic_error(errmsg);
    }

    //Get the checksum
    memcpy(buffer+n, &checksum, sizeof(uint32_t));
    n += sizeof(uint32_t);


    for (int i = 0; i < header.enc_packet_count; i++)
    {
        memcpy(buffer+n, &pkt_ids[i], sizeof(packetid_t));
        n += sizeof(packetid_t);
        memcpy(buffer+n, &from_ids[i], sizeof(Vast::id_t));
        n += sizeof(Vast::id_t);

        to_addrs[i].serialize (buffer+n);
        n += to_addrs[i].sizeOf ();

    }

    //Copy the std::string to buffer
    msg.copy(buffer+n, header.packetsize);
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

        //Get the checksum
        memcpy(&checksum, buffer+n, sizeof(uint32_t));
        n += sizeof(uint32_t);

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

            Vast::IPaddr addr;
            size_t remaining_bytes = size - sizeof(RLNCHeader) - i*(sizeof(packetid_t) + sizeof(Vast::id_t) + addr.sizeOf ());
            addr.deserialize (buffer+n, remaining_bytes);
            putToAddr (addr);
            n += addr.sizeOf ();

        }
        if (header.packetsize > size)
        {
            std::cerr << "RLNCMessage::deserialize Header packet size larger than available bytes" << std::endl;
            std::cerr << "header.start " << header.start << std::endl;
            std::cerr << "header.end: " << header.end << std::endl;
            msg.resize(size - n);
            msg.assign(buffer+n, size-n);
            std::cerr << "RLNCMessage::deserialize" << *(this) << std::endl;
            return -1;
        }


        msg.resize(header.packetsize);
        msg.assign(buffer+n, header.packetsize);

        n += header.packetsize;

        return n;
    }
    else
    {
        return 0;
    }

    return 0;
}

bool RLNCMessage::operator==(const RLNCMessage other) const
{
    bool equals = true;

    equals = equals && (RLNCHeader_factory::isRLNCHeadersEqual (other.header, this->header));
    equals = equals && (other.pkt_ids == this->pkt_ids);
    equals = equals && (other.from_ids == this->from_ids);
    equals = equals && (other.to_addrs == this->to_addrs);

    if (other.header.packetsize != this->header.packetsize)
        return false;

    equals = equals && other.msg == this->msg;
    equals = equals && other.socket_addr == this->socket_addr;
    equals = equals && other.checksum == this->checksum;

    return equals;
}

/**
 * @brief Compares everything similarly, except socket_addr which is not serialized
 * @param other
 * @return
 */
bool RLNCMessage::contentEquals(const RLNCMessage other) const
{
    bool equals = true;

    equals = equals && (RLNCHeader_factory::isRLNCHeadersEqual (other.header, this->header));
    equals = equals && (other.pkt_ids == this->pkt_ids);
    equals = equals && (other.from_ids == this->from_ids);
    equals = equals && (other.to_addrs == this->to_addrs);

    if (other.header.packetsize != this->header.packetsize)
        return false;

    equals = equals && other.msg == this->msg;
    equals = equals && other.checksum == this->checksum;

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
            output << "To addr[" << i << "]" << message.to_addrs[i] << std::endl;
        }
        output << "msg: " << std::endl;
//        boost::format format("%1$#x");
//        output << message.msg;

        output << Logger::printArray(message.msg.c_str(), message.msg.length());

        output << std::endl;

        output << "socket_addr: " << message.socket_addr << std::endl;
        output << "CRC32 checksum" << message.checksum << std::endl;


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

uint32_t RLNCMessage::generateChecksum(const uint8_t *buffer, size_t bufsize)
{
    return generateChecksum(reinterpret_cast<const char*>(buffer), bufsize);
}

uint32_t RLNCMessage::generateChecksum(const char *buffer, size_t bufsize)
{
    boost::crc_32_type  result;
    result.process_bytes(buffer, bufsize);
    return result.checksum();
}
