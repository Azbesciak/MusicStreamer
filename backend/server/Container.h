
#include <unordered_map>
#include <mutex>
#include "../streamerClient/Room.h"

#include "../utility/synch.h"
#include "../messageSender/MessageSender.h"
#include "../streamerClient/ClientResponse.h"

#ifndef MUSICSTREAMER_CONTAINER_H
#define MUSICSTREAMER_CONTAINER_H


class Container {
private:
    unordered_map<string, Room*> rooms;
    recursive_mutex roomsMut;
    MessageSender* messageSender;
    unordered_map<string, StreamerClient*> clients;
    recursive_mutex clientsMut;
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

    void removeClientFromRooms(StreamerClient *client);
    void sendToAll(const string &message);
};



#endif //MUSICSTREAMER_CONTAINER_H
