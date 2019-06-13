#include "rlnc_packet_factory.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;


int main (int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "Usage: ./rlncdissector.cpp <filename>" << endl;
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

    RLNCHeader header;
    memcpy(&header, memblock + offset, sizeof(RLNCHeader));

    cout << header << endl;


    delete[] memblock;
    return 0;
}
