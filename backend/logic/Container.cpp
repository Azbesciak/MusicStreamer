
#include "Container.h"
#include "streamerClient/StreamerClient.h"
#include "Room.h"

Container *Container::instance = nullptr;

void Container::joinClientToRoom(StreamerClient *client, const std::string &name) {
    synchronized(roomsMut) {
        if (client->getCurrentRoomName() == name)
            return;
        removeClientFromRoomsUnsync(client);
        createRoomIfNotExists(name);
        addNewClient(client, name);
    }
}


void Container::addNewClient(StreamerClient *client, const string &name) {
    auto room = rooms[name];
    room->addClient(client);
    client->setCurrentRoomName(room->getName());
}


void Container::removeClientFromRooms(StreamerClient *client) {
    synchronized(roomsMut) {
        removeClientFromRoomsUnsync(client);
    }
}

void Container::removeClientFromRoomsUnsync(StreamerClient *client) {
    for (auto &&room: rooms) {
        if (room.second != nullptr) {
            auto removed = room.second->removeClient(client);
            if (removed) {
                if (room.second->isEmpty()) {
                    deleteRoom(room.first);
                }
                break;
            }
        }
    }
    client->leaveRoom();
}

void Container::sendResponseToClients(unordered_set<StreamerClient *> &clients, ClientResponse &resp) const {
    MessageSender::sendMessage(clients, &resp);
}

void Container::createRoomIfNotExists(const std::string &name) {
    if (rooms[name] == nullptr) {
        rooms[name] = new Room(name);
        watcher->markGlobalChange();
    }
}

void Container::deleteRoom(const std::string &name) {
    auto iterator = rooms.find(name);
    if (iterator != rooms.end()) {

        Room *room = iterator->second;

        // Probably unnecessary, but added for the sake of safety
        for (StreamerClient *client : room->getClients())
            client->leaveRoom();

        delete (room);
        rooms.erase(iterator);
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
        : clients(unordered_map<string, StreamerClient *>()),
          rooms(unordered_map<string, Room *>()),
          watcher(new StateChangeWatcher(this)) {};

Container::~Container() {
    delete watcher;
    synchronized(roomsMut) {
        for (auto &&room: rooms) {
            deleteRoom(room.first);
        }
    }
}

vector<string> Container::getRoomsList() {
    synchronized(roomsMut) {
        vector<string> roomsList;
        roomsList.reserve(rooms.size());
        for (auto &&room: rooms) {
            if (room.second != nullptr) {
                roomsList.push_back(room.second->getName());
            }
        }
        return roomsList;
    }
}

bool Container::addUserIfNotKnown(StreamerClient *client, const string &clientName) {
    synchronized(clientsMut) {
        if (clients.count(clientName) == 0) {
            clients[clientName] = client;
            return true;
        }
        return false;
    }
}

void Container::removeClient(StreamerClient *client) {
    synchronized(clientsMut) {
        clients.erase(client->getName());
    }
    synchronized(roomsMut) {
        removeClientFromRoomsUnsync(client);
    }
}

void Container::sendToAll(ClientResponse &resp) {
    synchronized(clientsMut) {
        unordered_set<StreamerClient *> clientsSet;
        transform(clients.begin(), clients.end(), inserter(clientsSet, clientsSet.begin()),
                  [](const pair<string, StreamerClient *> &key_value) {
                      return key_value.second;
                  });
        sendResponseToClients(clientsSet, resp);
    }
}

StreamerClient *Container::subscribeClientForMessages(const string &clientName, int messageSocketFd) {
    synchronized(clientsMut) {
        auto client = getClientByNameUnsynch(clientName);
        if (client != nullptr) {
            client->subscribeForMessages(messageSocketFd);
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

ClientResponse Container::setClientStreamingPort(StreamerClient *client, int port) {
    synchronized(clientsMut) {
        auto clientAddress = client->getAddr();
        for (auto c: clients) {
            if (c.second->hasStreamingAddr(clientAddress, port)) {
                return ClientResponse::error(403, "Address already assigned");
            }
        }
        if (client->initializeStreamingChannel(port)) {
            return ClientResponse::ok();
        } else {
            return ClientResponse::error(403, "You cannot change your streaming host");
        }
    }
}

recursive_mutex *Container::getRoomsMutex() {
    return &roomsMut;
}

recursive_mutex *Container::getClientsMutex() {
    return &clientsMut;
}

Container *Container::initialize() {
    instance = new Container();
    return getInstance();
}

void Container::destroy() {
    delete instance;
    instance = nullptr;
}

Container *Container::getInstance() {
    return instance;
}

Room *Container::getRoom(const string &roomName) {
    return rooms[roomName];
}

void Container::withRoom(const string &roomName, const function<void(Room *)> &consumer) {
    if (roomName.empty()) return;
    synchronized(instance->roomsMut) {
        Room *&room = instance->rooms[roomName];
        if (room != nullptr) {
            consumer(room);
        }
    }
}

ClientResponse Container::withRoom(StreamerClient *client, const function<ClientResponse(Room *)> &consumer) {
    synchronized(instance->roomsMut) {
        if (client->hasRoomAssigned()) {
            const string &roomName = client->getCurrentRoomName();
            Room *&room = instance->rooms[roomName];
            if (room != nullptr) {
                return consumer(room);
            } else {
                return ClientResponse::error(403, "Room no longer exists");
            }
        } else {
            return ClientResponse::error(403, "Client has no assigned room");
        }
    }
}

void StateChangeWatcher::spreadRoomsList() {
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
                spreadRoomsList();
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
    delete watchThread;
}

void StateChangeWatcher::markGlobalChange() {
    changeMark = true;
}

void StateChangeWatcher::requestUpdate(StreamerClient *client) {
    synchronized(receiversMut) {
        receivers.insert(client);
    }
}
