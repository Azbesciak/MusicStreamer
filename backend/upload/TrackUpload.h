#ifndef MUSICSTREAMER_TRACKUPLOAD_H
#define MUSICSTREAMER_TRACKUPLOAD_H

#include <logic/MusicTrack.h>
#include <logic/Room.h>
#include "FileUpload.h"


class TrackUpload : public FileUpload {

private:

    MusicTrack* track;
    Room* room;

public:

    TrackUpload(MusicTrack* track, Room* room, int fileSize)
        : FileUpload(fileSize) {

        this->track = track;
        this->room = room;
    }

    void onUploadCompleted(UploadedFile* uploadedFile) override;
    void onUploadFailed() override;
};


#endif //MUSICSTREAMER_TRACKUPLOAD_H
