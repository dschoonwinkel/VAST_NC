#ifndef NCWRAPPER_H
#define NCWRAPPER_H

#include "VASTTypes.h"
#include <string>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;

namespace Vast {
    class NCWrapper
    {
    public:
        NCWrapper()
        {
            timestamp = 0;
        }

        NCWrapper(timestamp_t timestamp, std::string buffer, ip::udp::endpoint remote_endpoint, ip::udp::endpoint local_endpoint)
            : _buffer(buffer), _remote_endpoint(remote_endpoint), _local_endpoint(local_endpoint)
        {
            this->timestamp = timestamp;
        }

        NCWrapper(const NCWrapper& other)
            : timestamp(other.timestamp),
              _buffer(other._buffer),
              _remote_endpoint(other._remote_endpoint),
              _local_endpoint(other._local_endpoint)
        {
        }

        timestamp_t timestamp;
        std::string _buffer;
        ip::udp::endpoint _remote_endpoint;
        ip::udp::endpoint _local_endpoint;


        bool operator==(NCWrapper other) {
            bool equals = timestamp == other.timestamp;
            equals = equals && _buffer == other._buffer;
            equals = equals && _remote_endpoint == other._remote_endpoint;
            equals = equals && _local_endpoint == other._local_endpoint;
            return equals;
        }

        //Boost serialization
        friend class boost::serialization::access;
        template<typename Archive>
        void save(Archive& ar, const unsigned /*version*/) const
        {
            ar << timestamp;
            ar << _buffer;
            ar << _remote_endpoint.address().to_string();
            ar << _remote_endpoint.port();
            ar << _local_endpoint.address().to_string();
            ar << _local_endpoint.port();
        }

        template<typename Archive>
        void load(Archive& ar, const unsigned /*version*/)
        {
            ar >> timestamp;
            ar >> _buffer;
            //Read remote_endpoint
            std::string ip_addr;
            unsigned short port;
            ar >> ip_addr;
            ar >> port;
            _remote_endpoint.address(ip::address::from_string(ip_addr));
            _remote_endpoint.port(port);
            //Read local_endpoint
            port = 0;
            ip_addr = "";
            ar >> ip_addr;
            ar >> port;
            _local_endpoint.address(ip::address::from_string(ip_addr));
            _local_endpoint.port(port);
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()

        friend std::ostream& operator<<(std::ostream&, NCWrapper const& wrapper);
    };

    inline std::ostream& operator<<(std::ostream& output, NCWrapper const& wrapper )
    {
        output << "Timestamp: " << wrapper.timestamp << std::endl;
        output << "Buffer: ";
        for (size_t i = 0; i < wrapper._buffer.length(); i++)
             output << std::hex << ((int)wrapper._buffer.c_str()[i] & 0xff) << " ";
        output << std::endl;

        output << "Remote endpoint: " << std::resetiosflags << std::endl << wrapper._remote_endpoint << std::endl;
        output << "Local endpoint: " << std::endl << wrapper._local_endpoint << std::endl;
        return output;
    }
}

#endif // NCWRAPPER_H
