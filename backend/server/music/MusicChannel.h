#ifndef MUSICSTREAMER_MUSICCHANNEL_H
#define MUSICSTREAMER_MUSICCHANNEL_H

#include <logic/MusicTrack.h>
#include <streamerClient/Socket.h>
#include <netinet/in.h>


class MusicChannel {

private:

    Socket* frameSocket;
    Socket* soundSocket;
    sockaddr_in streamAddress;

public:

    MusicChannel(Socket* frameSocket, Socket* soundSocket, sockaddr_in streamAddress);

    void sendHeader(char* headerBytes, int headerSize);
    void sendSound(char* soundBytes);

    ~MusicChannel();
};


#endif //MUSICSTREAMER_MUSICCHANNEL_H
