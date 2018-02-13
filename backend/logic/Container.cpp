
#include "Container.h"


void Container::joinClientToRoom(StreamerClient *client, const std::string &name) {
    synchronized(roomsMut) {
        auto currentClientRoom = client->getCurrentRoom();
        if (currentClientRoom != nullptr && currentClientRoom->getName() == name)
            return;
        removeClientFromRoomsUnsync(client);
        createRoomIfNotExists(name);
        addNewClient(client, name);
    }
}


void Container::addNewClient(StreamerClient *client, const string &name) {

    auto room = rooms[name];

    room->addClient(client);
    client->setCurrentRoom(room);

    sendListOfClientsToAllInRoom(room);
}


void Container::removeClientFromRooms(StreamerClient *client) {
    synchronized(roomsMut) {
        removeClientFromRoomsUnsync(client);
    }
}

void Container::removeClientFromRoomsUnsync(StreamerClient *client) {
    for (auto && room: rooms) {
        auto removed = room.second->removeClient(client);
        if (removed) {
            if (room.second->isEmpty()) {
                deleteRoom(room.first);
            } else {
                sendListOfClientsToAllInRoom(room.second);
            }
            break;
        }
    }
    // FIXME sense of problems...
    client->setCurrentRoom(nullptr);
}


void Container::sendListOfClientsToAllInRoom(Room *room) {
    auto clients = room->getClients();
    vector<string> names;
    names.reserve(clients.size());
    for(auto && cli: clients) {
        names.push_back(cli ->getName());
    }

    ClientResponse resp;
    resp.addToBody("room", room->getName());
    resp.addToBody("clients", names);
    resp.setStatus(200);
    sendResponseToClients(clients, resp);
}

void Container::sendResponseToClients(unordered_set<StreamerClient *> &clients, ClientResponse &resp) const {
    Message message(move(clients), resp.serialize());
    messageSender->sendMessage(message);
}

void Container::createRoomIfNotExists(const std::string &name) {
    if (rooms.count(name) == 0) {
        rooms[name] = new Room(name);
        watcher->markGlobalChange();
    }
}

void Container::deleteRoom(const std::string &name) {
    auto iterator = rooms.find(name);
    if (iterator != rooms.end()) {

        Room* room = iterator->second;

        // Probably unnecessary, but added for the sake of safety
        for (StreamerClient* client : room->getClients())
            client->setCurrentRoom(nullptr);

        delete(room);
        rooms.erase(name);
        watcher->markGlobalChange();
    }
}

ClientResponse Container::createRoomsResponse() {
    auto response = ClientResponse();
    response.setStatus(200);
    response.addToBody("rooms", getRoomsList());
    return response;
}

Container::Container()
        : clients(unordered_map<string, StreamerClient*>()),
          rooms(unordered_map<string, Room*>()),
          messageSender(new MessageSender()),
          watcher(new StateChangeWatcher(this)){};

Container::~Container() {
    delete watcher;
    delete messageSender;
    synchronized(roomsMut) {
        for (auto&& room: rooms) {
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

void Container::removeClient(StreamerClient * client) {
    synchronized(clientsMut) {
        clients.erase(client->getName());
    }
    synchronized(roomsMut) {
        removeClientFromRoomsUnsync(client);
    }
}

void Container::sendToAll(ClientResponse &resp) {
    synchronized(clientsMut) {
        unordered_set<StreamerClient*> clientsSet;
        transform(clients.begin(), clients.end(), inserter(clientsSet, clientsSet.begin()),
                       [](const pair<string, StreamerClient*>& key_value) {
                           return key_value.second;
                       });
        sendResponseToClients(clientsSet, resp);
    }
}

StreamerClient * Container::subscribeClientForMessages(const string &clientName, int messageSocketFd) {
    synchronized(clientsMut) {
        auto client = getClientByNameUnsynch(clientName);
        if (client != nullptr) {
            client-> subscribeForMessages(messageSocketFd);
            watcher->requestUpdate(client);
        }
        return client;
    }
}

StreamerClient *Container::getClient(const string &clientName) {
    synchronized(clientsMut) {
        return getClientByNameUnsynch(clientName);
    }
}

StreamerClient *Container::getClientByNameUnsynch(const string &clientName) {
    auto clientEntry = clients.find(clientName);
    if (clientEntry != clients.end()) {
        return clientEntry->second;
    }
    return nullptr;
}


void StateChangeWatcher::spreadChangeStateInfo() {
    auto message = container->createRoomsResponse();
    container->sendToAll(message);
}

StateChangeWatcher::StateChangeWatcher(Container *container)
        : container(container),
          changeMark(false) {
    watchThread = new thread([=]() {
        while (true) {
            if (changeMark) {
                changeMark = false;
                spreadChangeStateInfo();
                synchronized(receiversMut) {
                    receivers.clear();
                }
            } else {
                synchronized(receiversMut) {
                    if (!receivers.empty()) {
                        auto message = container->createRoomsResponse();
                        container->sendResponseToClients(receivers, message);
                        receivers.clear();
                    }
                }
            }
            sleep(1);
        } 
    });
    watchThread->detach();
}

StateChangeWatcher::~StateChangeWatcher() {
    delete(watchThread);
}

void StateChangeWatcher::markGlobalChange() {
    changeMark = true;
}

void StateChangeWatcher::requestUpdate(StreamerClient *client) {
    synchronized(receiversMut) {
        receivers.insert(client);
    }
}
