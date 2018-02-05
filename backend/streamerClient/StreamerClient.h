#include <string>

#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

#include <logic/Room.h>
#include "Socket.h"

using namespace std;

class Room;

class StreamerClient {
private:
    Socket * connectionSocket;
    Socket * broadCastSocket;
    string name;
    Room* currentRoom;
public:
    explicit StreamerClient(int socketDescriptor);
    ~StreamerClient();
    string getName() const;
    void setName(const string& name);

    Room* getCurrentRoom();
    void setCurrentRoom(Room* room);

    ssize_t sendMessage(const string &mes);

    void removeSocket(Socket *&socket);
    void subscribeForMessages(int fd);
};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
