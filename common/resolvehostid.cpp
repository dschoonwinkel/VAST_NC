#include <iostream>

class IPaddr
{
public:
    IPaddr ()
        : host(0), port(0)
    {
    }

    IPaddr (uint32_t i, uint16_t p)
    {
        host = i;
        port = p;
    }

    IPaddr (const char *ip_string, uint16_t p)
    {
        uint32_t i = 0;

        if (ip_string != NULL)
        {
            int j=0;
            char c;

            uint32_t t = 0;
            while ((c = ip_string[j++]) != '\0')
            {
                if (c >= '0' && c <= '9')
                    t = t*10 + (c - '0');
                else if (c == '.')
                {
                    i = (i<<8) + t;
                    t = 0;
                }
            }
            i = (i<<8) + t;
        }
        else
            i = 0;

        host = i;
        port = p;
    }

    IPaddr(uint64_t id)
    {
        host = id >> 32;
        port = id >> 16;
    }

    ~IPaddr ()
    {
    }

    IPaddr &operator= (IPaddr const &a)
    {
        host = a.host;
        port = a.port;
        pad  = a.pad;

        return *this;
    }

    std::string getString() const
    {
        char temp_string[30];
        sprintf (temp_string, "%d.%d.%d.%d:%u", (int)((host>>24) & 0xff), (int)((host>>16) & 0xff), (int)((host>>8) & 0xff), (int)(host & 0xff), port);

        return std::string(temp_string);

    }

    uint32_t      host;
    uint16_t      port;
    uint16_t      pad;

    friend std::ostream& operator<<(std::ostream&, IPaddr const& addr);

};

inline std::ostream& operator<<(std::ostream& output, IPaddr const& addr )
{
    output << addr.getString();
    return output;
}

int main(int argc, char* argv[])
{
    // Check the number of parameters
    if (argc < 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " <uint64_t>" << std::endl;
        return 1;
    }

    std::string input(argv[1]);

//    std::cout << input << std::endl;

    size_t ID = 0;
    sscanf(input.c_str(), "%zu", &ID);

//    std::cout << ID << std::endl;
    std::cout << "hostID:IP" << std::endl << ID << " : " << IPaddr(ID) << std::endl;

    return 0;
}
