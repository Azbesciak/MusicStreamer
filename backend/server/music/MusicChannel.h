#ifndef MUSICSTREAMER_MUSICCHANNEL_H
#define MUSICSTREAMER_MUSICCHANNEL_H

#include <logic/MusicTrack.h>
#include <streamerClient/Socket.h>


class MusicChannel {

private:

    Socket* headerStreamingSocket;
    Socket* signalStreamingSocket;

    MusicTrack* currentTrack;

public:

    MusicChannel();



    ~MusicChannel();
};


#endif //MUSICSTREAMER_MUSICCHANNEL_H
