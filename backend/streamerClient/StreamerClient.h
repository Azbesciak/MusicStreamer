#include <string>

#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

#include <logic/Room.h>

using namespace std;

class Room;

class StreamerClient {
private:
    int socketDescriptor;
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
};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
