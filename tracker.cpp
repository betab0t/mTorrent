#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <openssl/sha.h>
#include <curl/curl.h>
#include <arpa/inet.h>

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
    BList* announceList = NULL;
    BItem* tmp = this->torrent->find("announce-list");
    if (tmp == nullptr)
        throw FormatError("no announce list found!");

    announceList = dynamic_cast<BList*>(tmp);
    if (announceList == nullptr)
        throw FormatError("invalid announce list!");


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

static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
    if(writerData == NULL)
        return 0;
    writerData->append(data, size*nmemb);
    return size * nmemb;
}

std::vector<PeerInfo> Tracker::peers(size_t up, size_t down, size_t left)
{
    CURL *curl;
    CURLcode res;
    std::string buf;

    std::vector<PeerInfo> peers_info;

    if((curl = curl_easy_init())) 
    {
        std::string info_hash = curl_easy_escape(curl, std::string(std::begin(this->info_hash), std::end(this->info_hash)).c_str(), 20);
        std::string full_url = this->url +
                            "?info_hash=" + info_hash +
                            "&peer_id=MTORRENTIDIDIDIDIDID" +
                            "&port=6881" +
                            "&uploaded=" + std::to_string(up) +
                            "&downloaded=" + std::to_string(down) +
                            "&left=" + std::to_string(left) + 
                            "&compact=0" +
                            "&no_peer_id=0" +
                            "&event=started";

        if(curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str()) != CURLE_OK)
            throw std::runtime_error("Error while initializing connection!");

        if(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer) != CURLE_OK)
            throw std::runtime_error("Error while initializing connection!");

        if(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf) != CURLE_OK)
            throw std::runtime_error("Error while initializing connection!");

        if((res = curl_easy_perform(curl)) == CURLE_OK)
        {
            auto bencode = Bencode();
            auto tracker_response = bencode.decode(buf);
            auto tmp = dynamic_cast<BDict*>(tracker_response.get());
            if (tmp == nullptr)
                return peers_info;
            
            auto peers_raw = dynamic_cast<BString*>(tmp->find("peers"));
            if (peers_raw) // binary model
            {
                auto ips = peers_raw->raw();
                size_t i;
                char buffer[INET_ADDRSTRLEN + 1];
                for(i=0; i < ips.length(); i+= 6)
                {
                    auto ip = inet_ntop(AF_INET, &ips[i], buffer, sizeof(buffer));
                    PeerInfo peer_info;
                    peer_info.ip = ip;
                    peer_info.port = *(unsigned short*)&ips[i+4];
                    peers_info.push_back(peer_info);
                }
            }
        }
        curl_easy_cleanup(curl);
    }
    return peers_info;
};


InfoHash TorrentFile::getInfoHash()
{
    BItem* info;
    InfoHash info_hash;
    if ((info = this->torrent->find("info")) == nullptr)
        throw FormatError("no info key found!");
    
    Bencode bencode = Bencode();
    std::string raw_data = bencode.encode(info);
    SHA1((const unsigned char*)raw_data.c_str(), raw_data.length(), (unsigned char*)&info_hash);

    return info_hash;
}