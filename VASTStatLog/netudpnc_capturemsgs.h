#ifndef NETUDPNC_CAPTUREMSGS_H
#define NETUDPNC_CAPTUREMSGS_H

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
#include "ncwrapper.h"

#define NETUDPNC_CAPTUREMSG

namespace Vast {

    class netudpnc_capturemsgs
    {
    public:
        //Empty constructor required for std::map [] operator
        netudpnc_capturemsgs() {}

        //Saving constructor
        netudpnc_capturemsgs(id_t id);

        //Retrieving constructor
        netudpnc_capturemsgs(std::string filename);

        static void close();
        ~netudpnc_capturemsgs();

        static void saveNCMessage(timestamp_t timestamp, std::string buffer,
                                  ip::udp::endpoint remote_endpoint,
                                  ip::udp::endpoint local_endpoint,
                                  id_t id = 0);
        void _saveNCMessage(timestamp_t timestamp, std::string buffer,
                            ip::udp::endpoint remote_endpoint,
                            ip::udp::endpoint local_endpoint);

        bool getCurrentNCMessage(std::string &buffer);
        bool getCurrentTimestamp(timestamp_t &timestamp);
        bool restoreNextNCMessage(timestamp_t curr_time, NCWrapper &wrapper);
        bool finished();
        size_t getRestoredMsgsCount();
        std::vector<NCWrapper> getRestoredNCMsgs();

    private:
        void restoreAllNCMessage(std::string filename);


        //Storing variables
        static netudpnc_capturemsgs *instance;
        std::string _logfilename_base = "./logs/msgcap/NCmsgscap";
        std::string _logfilename = "./logs/msgcap/NCmsgscap";
        std::ofstream *ofs = NULL;
        boost::archive::text_oarchive *aro = NULL;

        size_t msgcapCount = 0;
        std::chrono::microseconds msgcapTimer = std::chrono::microseconds::zero();
        std::chrono::high_resolution_clock::time_point t1;


        //Retrieving variables
        std::ifstream *ifs = NULL;
        boost::archive::text_iarchive *ari = NULL;
        size_t step = 0;
        std::vector<NCWrapper> restoredNCMsgs;

    };

}

#endif // NETUDPNC_CAPTUREMSGS_H
