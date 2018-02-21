#ifndef MUSICSTREAMER_MUSICCHANNEL_H
#define MUSICSTREAMER_MUSICCHANNEL_H

#include <logic/MusicTrack.h>
#include <streamerClient/Socket.h>
#include <netinet/in.h>
#include <logic/SoundChunk.h>


class MusicChannel {

private:
    std::recursive_mutex sockMut;
    Socket * streamingSocket;
    sockaddr_in streamAddress;
    int port;

public:

    MusicChannel(int port, const std::string &host);

    void setStreamingSocket(Socket *socket);

    void leaveStreamingSocket();

    ssize_t sendSound(SoundChunk* soundChunk);

    int getPort();
};


#endif //MUSICSTREAMER_MUSICCHANNEL_H
