
#include <unordered_map>
#include <mutex>
#include "../streamerClient/Room.h"

#include "../utility/synch.h"
#ifndef MUSICSTREAMER_CONTAINER_H
#define MUSICSTREAMER_CONTAINER_H


class Container {
private:
    std::unordered_map<std::string, Room*> rooms;
    std::recursive_mutex mut;
    void deleteRoom(const std::string &name);
    void createRoom(const std::string &name);

public:
    Container();
    ~Container();
    Room* joinClientToRoom(StreamerClient* client, const std::string &name);

};


#endif //MUSICSTREAMER_CONTAINER_H
