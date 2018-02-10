#ifndef MUSICSTREAMER_TRACK_H
#define MUSICSTREAMER_TRACK_H


#include <upload/UploadedFile.h>
#include <mutex>

class MusicTrack {

private:

    std::recursive_mutex trackMut;

    UploadedFile* trackFile;
    std::string trackName;

    int openedDescriptor;

public:

    MusicTrack(const std::string& trackName);

    const std::string& getTrackName();

    void setTrackFile(UploadedFile* trackFile);

    void openTrack();
    void closeTrack();

    char* getTrackHeader();
    char* getTrackSignal();

    ~MusicTrack();
};


#endif //MUSICSTREAMER_TRACK_H
