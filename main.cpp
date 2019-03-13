#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "bencode.h"
#include "tracker.h"


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <torrent>" << std::endl;
        return 1;
    }

    std::cout << "[*] Decoding torrent file..." << std::endl;
    TorrentFile torrent_file(argv[1]);
    PeerId peer_id;

    for (auto tracker_url : torrent_file.getTrackers())
    {
        std::cout << "[*] Connecting to tracker! " << tracker_url << std::endl;
        Tracker tracker = Tracker(tracker_url, torrent_file.getInfoHash(), peer_id);
        for (auto const& item : tracker.peers(0, 0, 0))
        {
            std::cout << "[*] Found peer! " << item.ip << std::endl;
        }
    }
    return 0;
}   