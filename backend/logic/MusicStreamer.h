

#ifndef MUSICSTREAMER_MUSICSTREAMER_H
#define MUSICSTREAMER_MUSICSTREAMER_H


#include <unordered_set>
#include <logic/MusicTrack.h>
#include <server/music/MusicChannel.h>
#include "streamerClient/StreamerClient.h"


class Room;

class MusicStreamer {

private:

    MusicChannel* musicChannel;

public:

    explicit MusicStreamer(Room* room);

    void streamTrack(MusicTrack* track);

    void pauseTrack();
    void resumeTrack();

    void stopStream();

    ~MusicStreamer();
};


#endif //MUSICSTREAMER_MUSICSTREAMER_H
