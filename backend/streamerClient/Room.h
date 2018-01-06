#include <string>
#include <vector>

#ifndef MUSICSTREAMER_ROOM_H
#define MUSICSTREAMER_ROOM_H

#include <unordered_set>
#include <mutex>
#include "MusicStreamer.h"

using namespace std;

class Room {
private:
    MusicStreamer *streamer;
    std::unordered_set<StreamerClient*> clients;
    std::recursive_mutex mut;
    string name;
public:
    explicit Room(string name);
    ~Room();
    void addClient(StreamerClient *client);
    void removeClient(StreamerClient * client);
    vector<string> getUsers();
    bool isEmpty();

};


#endif //MUSICSTREAMER_ROOM_H
