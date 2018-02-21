#include <iostream>
#include <utility/TerminalUtils.h>
#include <utility/synch.h>
#include "StreamerClient.h"
#include "../server/music/MusicChannel.h"

using namespace std;

StreamerClient::StreamerClient(int socketDescriptor, const string &addr) :
        communicationSocket(new Socket(socketDescriptor)),
        uploadSocket(nullptr),
        broadCastSocket(nullptr),
        streamingChannel(nullptr),
        addr(addr) {}

string StreamerClient::getName() const {
    return name;
}

StreamerClient::~StreamerClient() {
    removeSocket(communicationSocket);
    removeSocket(broadCastSocket);
    removeSocket(uploadSocket);
    delete streamingChannel;
}

ssize_t StreamerClient::sendMessage(const string &message) {
    return sendMessage(message, communicationSocket);
}

ssize_t StreamerClient::sendOnBroadCast(const string &mes) {
    auto wrote = sendMessage(mes, broadCastSocket);
    if (wrote < 0) {
        cout << "Broadcast socket for "
             << MAGENTA_TEXT((name.empty() ? "ANONYMOUS" : name))
             << (broadCastSocket != nullptr ? (string("(") + to_string(broadCastSocket->get()) + ")") : "")
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

void StreamerClient::sendSound(SoundChunk* soundChunk) {
    if (streamingChannel != nullptr)
        streamingChannel->sendSound(soundChunk);
}


void StreamerClient::setName(const string &name) {
    this->name = name;
}

string StreamerClient::getCurrentRoomName() {
    return currentRoomName;
}

void StreamerClient::setCurrentRoomName(const string & roomName) {
    currentRoomName = roomName;
}

void StreamerClient::removeSocket(Socket *&socket) {
    delete socket;
    socket = nullptr;
}

void StreamerClient::subscribeForMessages(int fd) {
    removeSocket(broadCastSocket);
    broadCastSocket = new Socket(fd);
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

void StreamerClient::setStreamingSocket(Socket *socket) {
    if (streamingChannel != nullptr) {
        streamingChannel->setStreamingSocket(socket);
    }
}

void StreamerClient::leaveStreamingChannel() {
    if (streamingChannel != nullptr) {
        streamingChannel->leaveStreamingSocket();
    }
}

bool StreamerClient::hasStreamingAddr(const string &addr, int port) {
    return streamingChannel != nullptr &&
            this->addr != addr &&
            streamingChannel->getPort() == port;
}

bool StreamerClient::initializeStreamingChannel(int port) {
    if (streamingChannel == nullptr) {
        streamingChannel = new MusicChannel(port, addr);
        return true;
    }
    return false;
}

string StreamerClient::getAddr() {
    return addr;
}

void StreamerClient::leaveRoom() {
    currentRoomName.clear();
}

bool StreamerClient::hasRoomAssigned() {
    return !currentRoomName.empty();
}


