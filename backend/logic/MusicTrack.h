#ifndef MUSICSTREAMER_TRACK_H
#define MUSICSTREAMER_TRACK_H


#include <upload/UploadedFile.h>
#include <mutex>

class MusicTrack {

private:

    std::recursive_mutex trackMut;

    UploadedFile* trackFile;

public:

    MusicTrack();

    UploadedFile* getTrackFile();
    void setTrackFile(UploadedFile* trackFile);
};


#endif //MUSICSTREAMER_TRACK_H
