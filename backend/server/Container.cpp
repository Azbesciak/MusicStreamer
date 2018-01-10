
#include <algorithm>
#include "Container.h"


void Container::joinClientToRoom(StreamerClient *client, const std::string &name) {
    synchronized(roomsMut) {
        removeClientFromRooms(client);
        createRoomIfNotExists(name);
        addNewClient(client, name);
    }
}

void Container::addNewClient(StreamerClient *client, const string &name) {
    auto room = rooms[name];
    room->addClient(client);
    sendListOfClientsToAllInRoom(room);
}

void Container::removeClientFromRooms(StreamerClient *client) {
    for (auto && room: rooms) {
        auto removed = room.second->removeClient(client);
        if (removed) {
            if (room.second->isEmpty()) {
                rooms.erase(room.first);
                delete room.second;
            } else {
                sendListOfClientsToAllInRoom(room.second);
            }
            break;
        }
    }
}


void Container::sendListOfClientsToAllInRoom(Room *room) {
    auto clients = room->getClients();
    vector<string> names;
    names.reserve(clients.size());
    for(auto && cli: clients) {
        names.push_back(cli ->getName());
    }

    ClientResponse resp;
    resp.addToBody("clients", names);
    resp.setStatus(200);
    Message message(move(clients), resp.serialize());
    messageSender->sendMessage(message);
}

void Container::createRoomIfNotExists(const std::string &name) {
    if (rooms.count(name) == 0)
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
          rooms(unordered_map<string, Room*>()),
          messageSender(new MessageSender()) {};

Container::~Container() {
    synchronized(roomsMut) {
        for(auto&& room: rooms) {
            deleteRoom(room.first);
        }
    }
    delete messageSender;
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

void Container::removeClient(StreamerClient * client) {
    synchronized(clientsMut) {
        clients.erase(client->getName());
    }
    synchronized(roomsMut) {
        removeClientFromRooms(client);
    }
}

void Container::sendToAll(const string &message) {
    synchronized(clientsMut) {
        for (auto && client: clients) {
            client.second;
        }
    }
}
