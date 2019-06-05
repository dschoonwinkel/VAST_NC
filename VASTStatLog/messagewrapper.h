#ifndef MESSAGEWRAPPER_H
#define MESSAGEWRAPPER_H

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
}

#endif // MESSAGEWRAPPER_H
