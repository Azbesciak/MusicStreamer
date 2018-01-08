
#include <unordered_map>
#include <mutex>
#include "../streamerClient/Room.h"

#include "../utility/synch.h"
#ifndef MUSICSTREAMER_CONTAINER_H
#define MUSICSTREAMER_CONTAINER_H


class Container {
private:
    unordered_map<string, Room*> rooms;
    recursive_mutex roomsMut;

    unordered_map<string, StreamerClient*> clients;
    recursive_mutex clientsMut;
    void deleteRoom(const string &name);
    void createRoom(const string &name);

public:
    Container();
    ~Container();
    Room* joinClientToRoom(StreamerClient* client, const string &name);
    bool addUserIfNotKnown(StreamerClient* client, const string &clientName);
    void removeClient(const string &clientName);
    vector<string> getRoomsList();
};


#endif //MUSICSTREAMER_CONTAINER_H
