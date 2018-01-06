
#include "Container.h"

Room *Container::joinClientToRoom(StreamerClient *client, const std::string &name) {
    synchronized(mut) {
        auto iterator = rooms.find(name);
        if (iterator == rooms.end()) {
            createRoom(name);
        }
        auto room = rooms[name];
        room->addClient(client);
        return room;
    }
}

void Container::createRoom(const std::string &name) {
    rooms[name] = new Room(name);
}

void Container::deleteRoom(const std::string &name) {
    auto iterator = rooms.find(name);
    if (iterator != rooms.end()) {
        delete(iterator->second);
        rooms.erase(name);
    }
}

Container::Container() = default;

Container::~Container() {
    for(auto&& room: rooms) {
        deleteRoom(room.first);
    }
}
