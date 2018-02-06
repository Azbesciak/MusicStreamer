#include "StreamerClient.h"

using namespace std;

StreamerClient::StreamerClient(int socketDescriptor) :
        communicationSocket(new Socket(socketDescriptor)),
        broadCastSocket(nullptr),
        uploadSocket(nullptr),
        streamingSocket(nullptr){}

string StreamerClient::getName() const {
    return name;
}

StreamerClient::~StreamerClient() {
    removeSocket(communicationSocket);
    removeSocket(broadCastSocket);
    removeSocket(uploadSocket);
    removeSocket(streamingSocket);
}

ssize_t StreamerClient::sendMessage(const string &message) {
    return sendMessage(message, communicationSocket);
}

ssize_t StreamerClient::sendOnBroadCast(const string &mes) {
    return sendMessage(mes, broadCastSocket);
}

ssize_t StreamerClient::sendMessage(const string &mes, Socket *socket) {
    if (socket != nullptr) {
        return write(socket->get(), mes.c_str(), mes.size());
    }
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


