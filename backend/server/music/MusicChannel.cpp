#include "MusicChannel.h"

#include <cstring>


MusicChannel::MusicChannel(Socket* frameSocket, Socket* soundSocket, sockaddr_in streamAddress) {

    this->frameSocket = frameSocket;
    this->soundSocket = soundSocket;
    this->streamAddress = streamAddress;
}


void MusicChannel::sendHeader(char* headerBytes, int headerSize) {

    send(frameSocket->get(), headerBytes, headerSize, 0);
}


void MusicChannel::sendSound(char* soundBytes) {

    int bytes = strlen(soundBytes);
    sendto(soundSocket->get(), soundBytes, bytes, 0, (sockaddr*) &streamAddress, sizeof(streamAddress));
}


MusicChannel::~MusicChannel() {

    delete frameSocket;
    delete soundSocket;
}
