
#include <algorithm>
#include "Room.h"

#include "../utility/synch.h"
#include "StreamerClient.h"

Room::Room(string name): name(std::move(name)) {

}

Room::~Room() {

}

bool Room::removeClient(StreamerClient *client) {
    synchronized(mut) {
        return clients.erase(client) != 0;
    }
}

vector<string> Room::getUsers() {
    synchronized(mut) {
        vector<string> res;
        res.reserve(clients.size());
        transform(clients.begin(), clients.end(),res.begin(), [](StreamerClient* cli){return cli->getName();});
        return res;
    }
}

void Room::addClient(StreamerClient *client) {
    synchronized(mut) {
        clients.insert(client);
    }
}

bool Room::isEmpty() {
    return clients.empty();
}

string Room::getName() {
    return name;
}
