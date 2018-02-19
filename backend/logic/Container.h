
#include <unordered_map>
#include <mutex>
#include <atomic>

#include "utility/synch.h"
#include "messageSender/MessageSender.h"
#include "streamerClient/ClientResponse.h"

#ifndef MUSICSTREAMER_CONTAINER_H
#define MUSICSTREAMER_CONTAINER_H
class Container;
class Room;

class StateChangeWatcher {
    thread * watchThread;
    atomic<bool> changeMark;
    Container * container;
    unordered_set<StreamerClient *> receivers;
    recursive_mutex receiversMut;
    void spreadRoomsList();

public:
    explicit StateChangeWatcher(Container * container);
    ~StateChangeWatcher();
    void markGlobalChange();
    void requestUpdate(StreamerClient * client);
};


class Container {
private:
    static Container * instance;
    unordered_map<string, Room*> rooms;
    recursive_mutex roomsMut;
    unordered_map<string, StreamerClient*> clients;
    recursive_mutex clientsMut;
    StateChangeWatcher * watcher;
    void deleteRoom(const string &name);
    void createRoomIfNotExists(const string &name);
    void addNewClient(StreamerClient *client, const string &name);
    void removeClientFromRoomsUnsync(StreamerClient *client);
    StreamerClient * getClientByNameUnsynch(const string &clientName);

public:
    Container();
    ~Container();
    void joinClientToRoom(StreamerClient* client, const string &name);
    bool addUserIfNotKnown(StreamerClient* client, const string &clientName);
    void removeClient(StreamerClient * client);
    vector<string> getRoomsList();
    StreamerClient *subscribeClientForMessages(const string &clientName, int messageSocketFd);

    void removeClientFromRooms(StreamerClient *client);
    void sendToAll(ClientResponse &resp);

    ClientResponse createRoomsResponse();
    void sendResponseToClients(unordered_set<StreamerClient *> &clients, ClientResponse &resp) const;

    StreamerClient *getClient(const string &clientName);

    ClientResponse setClientStreamingPort(StreamerClient *client, int port);
    recursive_mutex* getRoomsMutex();
    recursive_mutex* getClientsMutex();
    Room * getRoom(const string & roomName);

    static Container * getInstance();

    static Container *initialize();
    static void destroy();

    static void withRoom(const string &roomName, const function<void(Room *)> &consumer);

    static ClientResponse withRoom(StreamerClient * client, const function<ClientResponse(Room *)> &consumer);
};



#endif //MUSICSTREAMER_CONTAINER_H
