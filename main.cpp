#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "bencode.h"

std::string read_file(const std::string& path)
{
    std::ifstream fh;
    std::stringstream buf;
    
    fh.open(path);
    buf << fh.rdbuf();
    fh.close();

    return buf.str();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <torrent>" << std::endl;
        return 1;
    }
    auto bencode = Bencode();
    auto buf = read_file(argv[1]);
    std::cout << bencode.to_string(bencode.decode(buf).get()) << std::endl;

    return 0;
}