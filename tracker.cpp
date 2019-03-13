#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "tracker.h"
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

TorrentFile::TorrentFile(std::string path)
{
    auto buf = read_file(path);
    auto bencode = Bencode();
    auto item = bencode.decode(buf);

    BDict *tmp  = dynamic_cast<BDict*>(item.get());
    if(tmp == nullptr)
        throw FormatError("corrupted torrent file!");
    
    item.release();
    this->torrent.reset(tmp);
}

std::vector<std::string> TorrentFile::getTrackers()
{
    std::vector<std::string> trackers;
    BList *announceList = NULL;
    for (auto const& item : this->torrent->raw())
    {
        if (item.first->raw() == "announce-list")
        {
            announceList = dynamic_cast<BList*>(item.second.get());
            if (announceList == nullptr)
                throw FormatError("invalid announce list!");
            break;
        }
    }
    if (!announceList)
        throw FormatError("no announce list found!");
    
    for (auto const& item : announceList->raw())
    {
        BList *tmpList = dynamic_cast<BList*>(item.get());
        if (tmpList == nullptr)
            throw FormatError("invalid tracker format!");

        BString *tracker = dynamic_cast<BString*>(tmpList->raw()[0].get());
        if (tracker == nullptr)
            throw FormatError("invalid tracker format!");

        trackers.push_back(tracker->raw());
    }
    return trackers;
};

std::vector<PeerInfo> Tracker::peers(size_t up, size_t down, size_t left)
{
    std::vector<PeerInfo> res;
    return res;
};