
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "Room.h"

#include "utility/synch.h"
#include "messageSender/MessageSender.h"
#include "streamerClient/ClientResponse.h"

#ifndef MUSICSTREAMER_CONTAINER_H
#define MUSICSTREAMER_CONTAINER_H
class Container;

class StateChangeWatcher {
    thread * watchThread;
    atomic<bool> changeMark;
    Container * container;
    unordered_set<StreamerClient *> receivers;
    recursive_mutex receiversMut;
    void spreadChangeStateInfo();

public:
    explicit StateChangeWatcher(Container * container);
    ~StateChangeWatcher();
    void markGlobalChange();
    void requestUpdate(StreamerClient * client);
};


class Container {
private:
    unordered_map<string, Room*> rooms;
    recursive_mutex roomsMut;
    MessageSender* messageSender;
    unordered_map<string, StreamerClient*> clients;
    recursive_mutex clientsMut;
    StateChangeWatcher * watcher;
    void deleteRoom(const string &name);
    void createRoomIfNotExists(const string &name);
    void sendListOfClientsToAllInRoom(Room *room);
    void addNewClient(StreamerClient *client, const string &name);

public:
    Container();
    ~Container();
    void joinClientToRoom(StreamerClient* client, const string &name);
    bool addUserIfNotKnown(StreamerClient* client, const string &clientName);
    void removeClient(StreamerClient * client);
    vector<string> getRoomsList();
    StreamerClient * subscribeClientForMessages(const string &clientName, int messageSocketFd);

    void removeClientFromRooms(StreamerClient *client);
    void sendToAll(ClientResponse &resp);

    ClientResponse createRoomsResponse();

    void sendResponseToClients(unordered_set<StreamerClient *> &clients, ClientResponse &resp) const;
};



#endif //MUSICSTREAMER_CONTAINER_H
