#include "vast_capturemsgs.h"

namespace Vast
{
    //Static instance
    vast_capturemsgs* vast_capturemsgs::instance = NULL;


    vast_capturemsgs::vast_capturemsgs(id_t id)
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
            CPPDEBUG("VASTStatLogEntry: _logfilename: " << _logfilename << std::endl);
            logfile_count++;
         } while (boost::filesystem::exists(_logfilename));

        if (ifs)
        {
            ifs->close();
        }

        ofs = new std::ofstream(_logfilename);
        if (!ofs->is_open())
        {
            std::cerr << "vast_capturemsgs::constructor file open : " << (ofs->is_open() ? "true":"false") << std::endl << "EXITING" <<std::endl;
            exit(EXIT_FAILURE);
        }


        aro = new boost::archive::text_oarchive(*ofs);
    }

    vast_capturemsgs::vast_capturemsgs(std::string filename)
    {
        if (filename == "")
        {
            throw std::logic_error("Cannot open an empty filename");
        }

        if (ofs)
        {
            ofs->close();
        }

        std::cerr << "vast_capturemsgs::constructor filename: " << filename << std::endl;
        ifs = new std::ifstream(filename);
        if (!ifs->is_open())
        {

            std::cerr << "vast_capturemsgs::constructor file open : " << (ifs->is_open() ? "true":"false") << std::endl << "EXITING" <<std::endl;
            exit(EXIT_FAILURE);
        }
        ari = new boost::archive::text_iarchive(*ifs);
    }

    void vast_capturemsgs::saveVASTMessage(timestamp_t timestamp, Message msg, id_t id)
    {
        if (instance == NULL || instance->aro == NULL)
        {
            if (instance != NULL)
            {
                std::cout << "Reopening vast_capturemsgs for writing" << std::endl;
            }
            instance = new vast_capturemsgs(id);
        }
        instance->_saveVASTMessage(timestamp, msg);
    }

    void vast_capturemsgs::_saveVASTMessage(timestamp_t timestamp, Message msg)
    {
        t1 = std::chrono::high_resolution_clock::now();
        MessageWrapper msgwrapper(timestamp, msg);
        msgwrapper.serialize(*aro, 0);
        ofs->flush();

        auto t2 = std::chrono::high_resolution_clock::now();
        msgcapTimer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);
        msgcapCount++;
    }

    MessageWrapper vast_capturemsgs::restoreVASTMessage(std::string filename)
    {
        if (instance == NULL || instance->ari == NULL)
        {
            if (instance != NULL)
            {
                std::cout << "Reopening vast_capturemsgs for reading" << std::endl;
            }
            instance = new vast_capturemsgs(filename);
        }


        return instance->_restoreVASTMessage();
    }

    std::vector<MessageWrapper> vast_capturemsgs::restoreAllVASTMessage(std::string filename)
    {
        std::vector<MessageWrapper> restoredMsgs;
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        while(!ifs.eof()) {
            MessageWrapper msg;
            msg.serialize(ar,0);
            restoredMsgs.push_back(msg);
        }

        return restoredMsgs;
    }

    MessageWrapper vast_capturemsgs::_restoreVASTMessage()
    {
        Vast::MessageWrapper msg2;
        msg2.serialize(*ari, 0);
        return msg2;
    }

    void vast_capturemsgs::close()
    {
        if (instance)
            delete instance;
    }



    vast_capturemsgs::~vast_capturemsgs()
    {
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


        CPPDEBUG("~vast_capturemsgs:: time spent in msgcapTimer: " << msgcapTimer.count() / 1000 << " milliseconds " << std::endl);
        CPPDEBUG("~vast_capturemsgs:: msgcapCount " << msgcapCount << std::endl);

    }
}
