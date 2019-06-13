#include <sstream>
#include <iostream>

int main(int argc, char** argv) {

    if (argc < 2)
    {
        std::cout << "Not enough arguments to convert" << std::endl;
        exit(1);
    }

    for (int i = 1; i < argc; ++i)
    {
        std::cout << argv[i] << "\n";

        size_t x;
        std::stringstream ss;
        ss << std::hex << argv[i];
        ss >> x;
        // output it as a unsigned type
        std::cout << static_cast<size_t>(x) << std::endl;
    }
}
