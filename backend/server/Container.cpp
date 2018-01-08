
#include <algorithm>
#include "Container.h"

Room *Container::joinClientToRoom(StreamerClient *client, const std::string &name) {
    synchronized(roomsMut) {
        for (auto && room: rooms) {
            auto removed = room.second->removeClient(client);
            if (removed) break;
        }
        if (rooms.count(name) == 0) {
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

Container::Container()
        : clients(unordered_map<string, StreamerClient*>()),
            rooms(unordered_map<string, Room*>()) {};

Container::~Container() {
    synchronized(roomsMut) {
        for(auto&& room: rooms) {
            deleteRoom(room.first);
        }
    }
}

vector<string> Container::getRoomsList() {
    synchronized(roomsMut) {
        vector<string> roomsList;
        roomsList.reserve(rooms.size());
        for(auto && room: rooms) {
            roomsList.push_back(room.second->getName());
        }
        return roomsList;
    }
}

bool Container::addUserIfNotKnown(StreamerClient * client, const string &clientName) {
    synchronized(clientsMut) {
        if (clients.count(clientName) == 0) {
            clients[clientName] = client;
            return true;
        }
        return false;
    }
}

void Container::removeClient(const string &clientName) {
    synchronized(clientsMut) {
        clients.erase(clientName);
    }
}
