#include <iostream>
#include <utility/TerminalUtils.h>
#include <utility/synch.h>
#include "StreamerClient.h"

using namespace std;

StreamerClient::StreamerClient(int socketDescriptor) :
        communicationSocket(new Socket(socketDescriptor)),
        uploadSocket(nullptr),
        currentRoom(nullptr),
        broadCastSocket(nullptr),
        streamingChannel(nullptr){}

string StreamerClient::getName() const {
    return name;
}

StreamerClient::~StreamerClient() {
    removeSocket(communicationSocket);
    removeSocket(broadCastSocket);
    removeSocket(uploadSocket);
    removeStreamingChannel();
}

ssize_t StreamerClient::sendMessage(const string &message) {
    return sendMessage(message, communicationSocket);
}

ssize_t StreamerClient::sendOnBroadCast(const string &mes) {
    auto wrote = sendMessage(mes, broadCastSocket);
    if (wrote < 0) {
        cout << "Broadcast socket for "
             << MAGENTA_TEXT((name.empty() ? "ANONYMOUS" : name))
             << (broadCastSocket != nullptr ? (string("(") + to_string(broadCastSocket->get()) + ")"): "")
             << " was closed."
             << endl;
        removeSocket(broadCastSocket);
    }
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


void StreamerClient::removeStreamingChannel() {

    if (streamingChannel != nullptr) {

        delete streamingChannel;
        streamingChannel = nullptr;
    }
}


void StreamerClient::subscribeForMessages(int fd) {
    removeSocket(broadCastSocket);
    broadCastSocket = new Socket(fd);
}


MusicChannel* StreamerClient::getStreamingChannel() {
    return streamingChannel;
}


bool StreamerClient::initializeUpload(int socket) {
    synchronized(uploadMutex) {
        if (uploadSocket == nullptr) {
            uploadSocket = new Socket(socket);
            return true;
        }
        return false;
    }
}

void StreamerClient::finishUpload(int socket) {
    synchronized(uploadMutex) {
        if (uploadSocket != nullptr && uploadSocket->get() == socket) {
            removeSocket(uploadSocket);
        }
    }
}


