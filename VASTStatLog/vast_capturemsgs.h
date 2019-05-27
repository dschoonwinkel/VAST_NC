#ifndef VAST_CAPTUREMSGS_H
#define VAST_CAPTUREMSGS_H

#include <VASTTypes.h>

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;
#include <chrono>
#include <vector>

#define VAST_CAPTUREMSGS

namespace Vast {

    class MessageWrapper
    {
    public:
        MessageWrapper() : _msg(0)
        {
            timestamp = 0;
        }

        MessageWrapper(timestamp_t timestamp, Message msg)
            : _msg(msg)
        {
            this->timestamp = timestamp;
        }

        MessageWrapper(const MessageWrapper& other)
            : timestamp(other.timestamp), _msg(other._msg)
        {
        }

        timestamp_t timestamp;
        Message _msg;

        bool operator==(MessageWrapper other) {
            bool equals = _msg == other._msg;
            equals = equals && timestamp == other.timestamp;
            return equals;
        }

        //Boost serialization
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive& ar, const unsigned /*version*/)
        {
            ar & timestamp;
            ar & _msg;
        }

        friend std::ostream& operator<<(std::ostream&, MessageWrapper const& wrapper);
    };

    inline std::ostream& operator<<(std::ostream& output, MessageWrapper const& wrapper )
    {
        output << "Timestamp: " << wrapper.timestamp << std::endl;
        output << "Message: " << std::endl << wrapper._msg;
        return output;
    }

    class vast_capturemsgs
    {
    public:
        //Saving constructor
        vast_capturemsgs(id_t id);

        //Retrieving constructor
        vast_capturemsgs(std::string filename);

        static void close();
        ~vast_capturemsgs();

        static void saveVASTMessage(timestamp_t timestamp, Message msg, id_t id = 0);
        void _saveVASTMessage(timestamp_t timestamp, Message msg);

        static MessageWrapper restoreVASTMessage(std::string filename);
        static std::vector<MessageWrapper> restoreAllVASTMessage(std::string filename);
        MessageWrapper _restoreVASTMessage();

    private:
        static vast_capturemsgs *instance;
        std::string _logfilename_base = "./logs/msgcap/VASTmsgscap";
        std::string _logfilename = "./logs/msgcap/VASTmsgscap";
        std::ofstream *ofs = NULL;
        std::ifstream *ifs = NULL;
        boost::archive::text_oarchive *aro = NULL;
        boost::archive::text_iarchive *ari = NULL;

        size_t msgcapCount = 0;
        std::chrono::microseconds msgcapTimer = std::chrono::microseconds::zero();
        std::chrono::high_resolution_clock::time_point t1;
    };

}

#endif // VAST_CAPTUREMSGS_H
