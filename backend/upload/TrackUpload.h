#ifndef MUSICSTREAMER_TRACKUPLOAD_H
#define MUSICSTREAMER_TRACKUPLOAD_H

#include <logic/MusicTrack.h>
#include <logic/Room.h>
#include "FileUpload.h"


class TrackUpload : public FileUpload {

private:

    MusicTrack* track;
    string roomName;

public:

    TrackUpload(MusicTrack* track, const string & roomName, int fileSize)
        : FileUpload(fileSize), roomName(roomName), track(track) {
    }
    string getIdentify();
    void onUploadCompleted(UploadedFile* uploadedFile) override;
    void onUploadFailed() override;

    string getFileName();
};


#endif //MUSICSTREAMER_TRACKUPLOAD_H
