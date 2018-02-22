#ifndef MUSICSTREAMER_TRACK_H
#define MUSICSTREAMER_TRACK_H


#include <upload/UploadedFile.h>
#include <mutex>
#include "SoundChunk.h"
#include "TrackHeader.h"

class MusicTrack {

private:

    std::recursive_mutex trackMut;

    UploadedFile* trackFile;
    std::string trackName;

    TrackHeader trackHeader;

    int openedTrackFile;
    bool headerProcessed;
    bool trackFinished;

    bool readTrackHeader();



public:

    static const int WAV_HEADER_SIZE = 44;
    static const int SOUND_CHUNK_SIZE = 8192;


    explicit MusicTrack(const std::string& trackName);

    const std::string& getTrackName();

    void setTrackFile(UploadedFile* trackFile);

    bool isSaved();
    bool isOpened();
    bool isFinished();

    void openTrack();
    void closeTrack();

    int getChunkTimeGapMicrosec();

    TrackHeader getTrackHeader();
    SoundChunk* nextSoundChunk();

    ~MusicTrack();
};


#endif //MUSICSTREAMER_TRACK_H
