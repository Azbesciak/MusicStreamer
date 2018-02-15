#include "MusicChannel.h"
#include "../../utility/synch.h"
#include <cstring>
#include <server/SocketFactory.h>


MusicChannel::MusicChannel(int port, const std::string &host)
        : streamingSocket(nullptr),
          streamAddress(SocketFactory::getSockAddr(host, port)),
          port(port) {
}

void MusicChannel::setStreamingSocket(Socket *socket) {
    synchronized(sockMut) {
        streamingSocket = socket;
    }
}

void MusicChannel::leaveStreamingSocket() {
    synchronized(sockMut) {
        streamingSocket = nullptr;
    }
}

ssize_t MusicChannel::sendSound(char *soundBytes) {
    synchronized(sockMut) {
        if (streamingSocket != nullptr) {
            int bytes = strlen(soundBytes);
            sendto(streamingSocket->get(), soundBytes, bytes, 0, (sockaddr *) &streamAddress, sizeof(streamAddress));
        }
    }
}

int MusicChannel::getPort() {
    return port;
}