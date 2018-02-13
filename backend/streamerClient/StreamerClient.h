
#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

#include <logic/Room.h>
#include "Socket.h"

#include <string>

using namespace std;

class MusicStreamer;
class Room;

class StreamerClient {
private:
    Socket * communicationSocket;
    Socket * broadCastSocket;
    Socket * uploadSocket = nullptr;
    MusicChannel* streamingChannel;

    recursive_mutex uploadMutex;

    string name;
    Room * currentRoom;
    ssize_t sendMessage(const string &mes, Socket * socket);
public:
    explicit StreamerClient(int socketDescriptor);
    ~StreamerClient();
    string getName() const;
    void setName(const string& name);

    Room* getCurrentRoom();
    void setCurrentRoom(Room* room);

    MusicChannel* getStreamingChannel();

    ssize_t sendMessage(const string &mes);
    ssize_t sendOnBroadCast(const string &mes);

    void removeSocket(Socket *&socket);
    void removeStreamingChannel();
    void subscribeForMessages(int fd);

    bool initializeUpload(int socket);
    void finishUpload(int socket);

};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
