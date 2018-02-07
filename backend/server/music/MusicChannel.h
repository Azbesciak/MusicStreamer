#ifndef MUSICSTREAMER_MUSICCHANNEL_H
#define MUSICSTREAMER_MUSICCHANNEL_H

#include <logic/MusicTrack.h>


class MusicChannel {

private:

    MusicTrack* currentTrack;

public:

    MusicChannel();


    ~MusicChannel();
};


#endif //MUSICSTREAMER_MUSICCHANNEL_H
