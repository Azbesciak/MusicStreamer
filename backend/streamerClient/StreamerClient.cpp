#include "StreamerClient.h"

using namespace std;

StreamerClient::StreamerClient(int socketDescriptor) :
        connectionSocket(new Socket(socketDescriptor)){}

string StreamerClient::getName() const {
    return name;
}

StreamerClient::~StreamerClient() {
    removeSocket(connectionSocket);
    removeSocket(broadCastSocket);
}

ssize_t StreamerClient::sendMessage(const string &message) {
    if (connectionSocket != nullptr)
        return write(connectionSocket->get(), message.c_str(), message.size());
    return 0;
}

void StreamerClient::setName(const string &name) {
    this->name = name;
}

Room* StreamerClient::getCurrentRoom() {
    return currentRoom;
}

void StreamerClient::setCurrentRoom(Room *room) {
    this->currentRoom = room;
}

void StreamerClient::removeSocket(Socket *& socket) {
    delete socket;
    socket = nullptr;
}

void StreamerClient::subscribeForMessages(int fd) {
    removeSocket(broadCastSocket);
    broadCastSocket = new Socket(fd);
}

