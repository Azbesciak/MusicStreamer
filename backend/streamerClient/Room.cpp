
#include "Room.h"

#include "../utility/synch.h"

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
    return vector<string>();
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
