#pragma once

#include <iostream>
#include <vector>
#include <array>
#include "bencode.h"

// ================================

struct InfoHash {
    std::array<char, 20> sha1;
    std::string infohash;
};

typedef std::array<char, 20> PeerId;

class TorrentFile
{
private:
    std::unique_ptr<BDict> torrent;
    InfoHash info_hash;

public:
    TorrentFile(std::string path);
    std::vector<std::string> getTrackers();
    InfoHash& getInfoHash() { return info_hash; };
};

// ================================

enum TrackerEvent
{
    started,
    stopped,
    completed
};

struct PeerInfo
{
    PeerId peer_id;
    std::string ip;
    unsigned short port;
};

class Tracker
{
private:
    std::string url;
    InfoHash info_hash;
    PeerId peer_id;
    std::unique_ptr<BList> send(size_t up,
                                size_t down,
                                size_t left,
                                TrackerEvent event);

public:
    Tracker(std::string url,
            InfoHash info_hash,
            PeerId peer_id) : url(url), info_hash(info_hash), peer_id(peer_id) {};

    std::vector<PeerInfo> peers(size_t up, size_t down, size_t left);
    bool stopped(size_t up, size_t down, size_t left) { return false; };
    bool completed(size_t up, size_t down, size_t left) { return false; };
};