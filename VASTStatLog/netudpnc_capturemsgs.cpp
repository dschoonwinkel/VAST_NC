#include "netudpnc_capturemsgs.h"

namespace Vast {
    //Static instance
    netudpnc_capturemsgs* netudpnc_capturemsgs::instance = NULL;


    netudpnc_capturemsgs::netudpnc_capturemsgs(id_t id)
    {
        if (id == 0)
        {
            throw std::logic_error("Cannot make a logfilename with id == 0");
        }

        size_t logfile_count = 0;
        do
         {
            _logfilename = _logfilename_base + "_N" + std::to_string(id) +
                    "_"+ std::to_string(logfile_count) + ".txt";
            CPPDEBUG("netudpnc_capturemsgs: _logfilename: " << _logfilename << std::endl);
            logfile_count++;
         } while (boost::filesystem::exists(_logfilename));

        if (ifs)
        {
            ifs->close();
        }

        ofs = new std::ofstream(_logfilename);
        if (!ofs->is_open())
        {
            std::cerr << "netudpnc_capturemsgs::constructor file open : " << (ofs->is_open() ? "true":"false") << std::endl << "EXITING" <<std::endl;
            exit(EXIT_FAILURE);
        }


        aro = new boost::archive::text_oarchive(*ofs);
    }

    netudpnc_capturemsgs::netudpnc_capturemsgs(std::string filename)
    {
        restoreAllNCMessage(filename);
    }

    void netudpnc_capturemsgs::saveNCMessage(timestamp_t timestamp, std::string buffer,
                                             ip::udp::endpoint remote_endpoint,
                                             ip::udp::endpoint local_endpoint,
                                             id_t id)
    {
        if (instance == NULL || instance->aro == NULL)
        {
            if (instance != NULL)
            {
                std::cout << "Reopening netudpnc_capturemsgs for writing" << std::endl;
            }
            instance = new netudpnc_capturemsgs(id);
        }
        instance->_saveNCMessage(timestamp, buffer, remote_endpoint, local_endpoint);
    }

    void netudpnc_capturemsgs::_saveNCMessage(timestamp_t timestamp,
                                              std::string buffer,
                                              ip::udp::endpoint remote_endpoint,
                                              ip::udp::endpoint local_endpoint)
    {
        t1 = std::chrono::high_resolution_clock::now();
        NCWrapper ncwrapper(timestamp, buffer, remote_endpoint, local_endpoint);
        ncwrapper.serialize(*aro, 0);
        ofs->flush();

        auto t2 = std::chrono::high_resolution_clock::now();
        msgcapTimer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
        msgcapCount++;
    }

    void netudpnc_capturemsgs::restoreAllNCMessage(std::string filename)
    {
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        while(!ifs.eof()) {
            NCWrapper msg;
            msg.serialize(ar,0);
            restoredNCMsgs.push_back(msg);
        }
    }

    bool netudpnc_capturemsgs::getCurrentNCMessage(std::string &buffer)
    {
        if (step < restoredNCMsgs.size())
        {
            buffer = restoredNCMsgs[step]._buffer;
            return true;
        }
        return false;
    }

    bool netudpnc_capturemsgs::getCurrentTimestamp(timestamp_t &timestamp)
    {
        if (step < restoredNCMsgs.size())
        {
            timestamp = restoredNCMsgs[step].timestamp;
            return true;
        }
        return false;
    }

    bool netudpnc_capturemsgs::restoreNextNCMessage(timestamp_t curr_time,
                                                    NCWrapper &wrapper)
    {
        if (restoredNCMsgs[step].timestamp <= curr_time && step < restoredNCMsgs.size())
        {
            wrapper = restoredNCMsgs[step];
            step++;
            return true;
        }
        return false;
    }

    bool netudpnc_capturemsgs::finished()
    {
        return (step >= restoredNCMsgs.size());
    }

    size_t netudpnc_capturemsgs::getRestoredMsgsCount()
    {
        return restoredNCMsgs.size();
    }

    std::vector<NCWrapper> netudpnc_capturemsgs::getRestoredNCMsgs()
    {
        return restoredNCMsgs;
    }

    void netudpnc_capturemsgs::close()
    {
        if (instance)
            delete instance;
    }

    netudpnc_capturemsgs::~netudpnc_capturemsgs()
    {
        if (aro)
        {
            CPPDEBUG("~netudpnc_capturemsgs:: time spent in msgcapTimer: " << msgcapTimer.count() / 1000 << " milliseconds " << std::endl);
            CPPDEBUG("~netudpnc_capturemsgs:: msgcapCount " << msgcapCount << std::endl);
        }

        //Do memory free here....
        if (aro)
        {
            delete aro;
        }

        if (ari)
        {
            delete ari;
        }

        if (ofs != NULL)
        {
            ofs->flush();
            ofs->close();
            delete ofs;
        }

        if (ifs != NULL)
        {
            ifs->close();
            delete ifs;
        }

    }

}
